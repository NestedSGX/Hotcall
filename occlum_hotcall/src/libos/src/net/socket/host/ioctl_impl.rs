use super::*;
use fs::{occlum_ocall_ioctl, BuiltinIoctlNum, IfConf, IoctlCmd};

use crate::libc::ocall::p_hotOcall;

#[repr(C)]
pub struct HotCall {
    responderThread: pthread_t,
    spinlock: sgx_spinlock_t,
    data: *mut c_void,
    callID: u16,
    keepPolling: bool,
    runFunction: bool,
    isDone: bool,
    busy: bool,
} 

#[repr(C)]
pub struct occlum_ocall_ioctlParams {
    result_p: *mut i32,
    fd: i32,
    request: c_int,
    arg: *mut c_void,
    len: size_t,
}

extern "C" {
    // hot-calls
    pub fn HotCall_requestCall( 
        hotCall: *mut HotCall,
        callID: u16, 
        data: *mut c_void,
    ) -> c_int;

    pub fn occlum_ocall_ioctl_hotcall(
        data: *mut occlum_ocall_ioctlParams,
    ) -> sgx_status_t;
}


impl HostSocket {
    pub(super) fn ioctl_impl(&self, cmd: &mut IoctlCmd) -> Result<i32> {
        if let IoctlCmd::SIOCGIFCONF(arg_ref) = cmd {
            return self.ioctl_getifconf(arg_ref);
        }

        let cmd_num = cmd.cmd_num() as c_int;
        let cmd_arg_ptr = cmd.arg_ptr() as *mut c_void;
        let ret = try_libc!({
            let mut retval: i32 = 0;
            let status = occlum_ocall_ioctl(
                &mut retval as *mut i32,
                self.raw_host_fd() as i32,
                cmd_num,
                cmd_arg_ptr,
                cmd.arg_len(),
            );

            // hot-calls begin
            /*let mut data = occlum_ocall_ioctlParams {
                result_p: &mut retval as *mut i32,
                fd: self.raw_host_fd() as i32,
                request: cmd_num,
                arg: cmd_arg_ptr,
                len: cmd.arg_len(),
            };

            let data_p: *mut c_void = &mut data as *mut _ as *mut c_void;
            //let mut hotOcall: HotCall = HOTCALL_INITIALIZER;
            let mut hotOcall_p = p_hotOcall as *mut u64 as *mut HotCall;

            HotCall_requestCall( hotOcall_p, 10, data_p );
            let status = sgx_status_t::SGX_SUCCESS;*/

            assert!(status == sgx_status_t::SGX_SUCCESS);
            retval
        });
        // FIXME: add sanity checks for results returned for socket-related ioctls
        cmd.validate_arg_and_ret_vals(ret)?;
        Ok(ret)
    }

    fn ioctl_getifconf(&self, arg_ref: &mut IfConf) -> Result<i32> {
        if !arg_ref.ifc_buf.is_null() && arg_ref.ifc_len == 0 {
            return Ok(0);
        }

        let ret = try_libc!({
            let mut recv_len: i32 = 0;
            let mut retval: i32 = 0;
            let status = occlum_ocall_ioctl_repack(
                &mut retval as *mut i32,
                self.raw_host_fd() as i32,
                BuiltinIoctlNum::SIOCGIFCONF as i32,
                arg_ref.ifc_buf,
                arg_ref.ifc_len,
                &mut recv_len as *mut i32,
            );
            assert!(status == sgx_status_t::SGX_SUCCESS);
            // If ifc_req is NULL, SIOCGIFCONF returns the necessary buffer
            // size in bytes for receiving all available addresses in ifc_len
            // which is irrelevant to the orginal ifc_len.
            if !arg_ref.ifc_buf.is_null() {
                assert!(arg_ref.ifc_len >= recv_len);
            }

            arg_ref.ifc_len = recv_len;
            retval
        });
        Ok(ret)
    }
}

extern "C" {
    // Used to ioctl arguments with pointer members.
    //
    // Before the call the area the pointers points to should be assembled into
    // one continous memory block. Then the block is repacked to ioctl arguments
    // in the ocall implementation in host.
    //
    // ret: holds the return value of ioctl in host
    // fd: the host fd for the device
    // cmd_num: request number of the ioctl
    // buf: the data to exchange with host
    // len: the size of the buf
    // recv_len: accepts transferred data length when buf is used to get data from host
    //
    fn occlum_ocall_ioctl_repack(
        ret: *mut i32,
        fd: c_int,
        cmd_num: c_int,
        buf: *const u8,
        len: i32,
        recv_len: *mut i32,
    ) -> sgx_status_t;
}
