use super::*;
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
pub struct occlum_sendmsgParams {
    result_p: *mut ssize_t,
    fd: c_int,
    msg_name: *const c_void,
    msg_namelen: libc::socklen_t,
    msg_data: *const libc::iovec,
    msg_datalen: size_t,
    msg_control: *const c_void,
    msg_controllen: size_t,
    flags: c_int,
}

extern "C" {
    // hot-calls
    pub fn HotCall_requestCall( 
        hotCall: *mut HotCall,
        callID: u16, 
        data: *mut c_void,
    ) -> c_int;

    pub fn occlum_sendmsg_hotcall(
        data: *mut occlum_sendmsgParams,
    ) -> sgx_status_t;
}

impl HostSocket {
    pub fn send(&self, buf: &[u8], flags: SendFlags) -> Result<usize> {
        self.sendto(buf, flags, &None)
    }

    pub fn sendmsg<'a, 'b>(&self, msg: &'b MsgHdr<'a>, flags: SendFlags) -> Result<usize> {
        let msg_iov = msg.get_iovs();

        self.do_sendmsg(
            msg_iov.as_slices(),
            flags,
            msg.get_name(),
            msg.get_control(),
        )
    }

    pub(super) fn do_sendmsg(
        &self,
        data: &[&[u8]],
        flags: SendFlags,
        name: Option<&[u8]>,
        control: Option<&[u8]>,
    ) -> Result<usize> {
        let data_length = data.iter().map(|s| s.len()).sum();
        let u_allocator = UntrustedSliceAlloc::new(data_length)?;
        let u_data = {
            let mut bufs = Vec::new();
            for buf in data {
                bufs.push(u_allocator.new_slice(buf)?);
            }
            bufs
        };

        self.do_sendmsg_untrusted_data(&u_data, flags, name, control)
    }

    fn do_sendmsg_untrusted_data(
        &self,
        u_data: &[UntrustedSlice],
        flags: SendFlags,
        name: Option<&[u8]>,
        control: Option<&[u8]>,
    ) -> Result<usize> {
        // Prepare the arguments for OCall
        let mut retval: isize = 0;
        // Host socket fd
        let host_fd = self.raw_host_fd() as i32;
        // Name
        let (msg_name, msg_namelen) = name.as_ptr_and_len();
        let msg_name = msg_name as *const c_void;
        // Iovs
        let raw_iovs: Vec<libc::iovec> = u_data
            .iter()
            .map(|slice| slice.as_ref().as_libc_iovec())
            .collect();
        let (msg_iov, msg_iovlen) = raw_iovs.as_slice().as_ptr_and_len();
        // Control
        let (msg_control, msg_controllen) = control.as_ptr_and_len();
        let msg_control = msg_control as *const c_void;
        // Flags
        let raw_flags = flags.bits();

        // Do OCall
        unsafe {
            /*let status = occlum_ocall_sendmsg(
                &mut retval as *mut isize,
                host_fd,
                msg_name,
                msg_namelen as u32,
                msg_iov,
                msg_iovlen,
                msg_control,
                msg_controllen,
                raw_flags,
            );*/

            // hot-calls begin
            let mut data = occlum_sendmsgParams {
                result_p: &mut retval as *mut isize,
                fd: host_fd,
                msg_name: msg_name,
                msg_namelen: msg_namelen as u32,
                msg_data: msg_iov,
                msg_datalen: msg_iovlen,
                msg_control: msg_control,
                msg_controllen: msg_controllen, 
                flags: raw_flags,
            };

            let data_p: *mut c_void = &mut data as *mut _ as *mut c_void;
            //let mut hotOcall: HotCall = HOTCALL_INITIALIZER;
            let mut hotOcall_p = p_hotOcall as *mut u64 as *mut HotCall;

            HotCall_requestCall( hotOcall_p, 6, data_p );
            let status = sgx_status_t::SGX_SUCCESS;

            assert!(status == sgx_status_t::SGX_SUCCESS);
        }
        let bytes_sent = if flags.contains(SendFlags::MSG_NOSIGNAL) {
            try_libc!(retval)
        } else {
            try_libc_may_epipe!(retval)
        };

        debug_assert!(bytes_sent >= 0);
        Ok(bytes_sent as usize)
    }
}

extern "C" {
    fn occlum_ocall_sendmsg(
        ret: *mut ssize_t,
        fd: c_int,
        msg_name: *const c_void,
        msg_namelen: libc::socklen_t,
        msg_data: *const libc::iovec,
        msg_datalen: size_t,
        msg_control: *const c_void,
        msg_controllen: size_t,
        flags: c_int,
    ) -> sgx_status_t;
}