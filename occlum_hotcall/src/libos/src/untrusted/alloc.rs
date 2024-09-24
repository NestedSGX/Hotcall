use super::*;
use std::alloc::{AllocError, Allocator, Layout};
use std::ptr::{self, write_bytes, NonNull};

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
pub struct occlum_ocall_posix_memalignParams {
    result_p: *mut *mut c_void,
    alignment: usize,
    size: usize,
}

#[repr(C)]
pub struct occlum_ocall_freeParams {
    ptr: *mut c_void,
}

extern "C" {
    // hot-calls
    pub fn HotCall_requestCall( 
        hotCall: *mut HotCall,
        callID: u16, 
        data: *mut c_void,
    ) -> c_int;

    pub fn occlum_ocall_posix_memalign_hotcall(
        data: *mut occlum_ocall_posix_memalignParams,
    ) -> sgx_status_t;

    pub fn occlum_ocall_free_hotcall(
        data: *mut occlum_ocall_freeParams,
    ) -> sgx_status_t;
}

/// The global memory allocator for untrusted memory
pub static mut UNTRUSTED_ALLOC: UntrustedAlloc = UntrustedAlloc;

pub struct UntrustedAlloc;

unsafe impl Allocator for UntrustedAlloc {
    fn allocate(&self, layout: Layout) -> std::result::Result<NonNull<[u8]>, AllocError> {
        if layout.size() == 0 {
            return Err(AllocError);
        }

        // Do OCall to allocate the untrusted memory according to the given layout
        let layout = layout
            .align_to(std::mem::size_of::<*const c_void>())
            .unwrap();
        let mem_ptr = {
            let mut mem_ptr: *mut c_void = ptr::null_mut();
            
            //let sgx_status = unsafe { occlum_ocall_posix_memalign(&mut mem_ptr as *mut _, layout.align(), layout.size()) };

            let sgx_status = sgx_status_t::SGX_SUCCESS;            
            unsafe {
                let mut data = occlum_ocall_posix_memalignParams {
                    result_p: &mut mem_ptr as *mut *mut c_void,
                    alignment: layout.align(),
                    size: layout.size(),
                };

                let data_p: *mut c_void = &mut data as *mut _ as *mut c_void;
                //let mut hotOcall: HotCall = HOTCALL_INITIALIZER;
                let mut hotOcall_p = p_hotOcall as *mut u64 as *mut HotCall;

                HotCall_requestCall( hotOcall_p, 8, data_p );
            };

            debug_assert!(sgx_status == sgx_status_t::SGX_SUCCESS);
            mem_ptr
        } as *mut u8;
        if mem_ptr == std::ptr::null_mut() {
            return Err(AllocError);
        }

        // Sanity checks
        // Post-condition 1: alignment
        debug_assert!(mem_ptr as usize % layout.align() == 0);
        // Post-condition 2: out-of-enclave
        assert!(sgx_trts::trts::rsgx_raw_is_outside_enclave(
            mem_ptr as *const u8,
            layout.size()
        ));
        Ok(NonNull::new(unsafe {
            core::slice::from_raw_parts_mut(mem_ptr, layout.size() as usize)
        })
        .unwrap())
    }

    unsafe fn deallocate(&self, ptr: NonNull<u8>, layout: Layout) { 
        // Pre-condition: out-of-enclave
        debug_assert!(sgx_trts::trts::rsgx_raw_is_outside_enclave(
            ptr.as_ptr(),
            layout.size()
        ));

        //let sgx_status = unsafe { occlum_ocall_free(ptr.as_ptr() as *mut c_void) };
        let sgx_status = sgx_status_t::SGX_SUCCESS;            
        unsafe {
            let mut data = occlum_ocall_freeParams {
                ptr: ptr.as_ptr() as *mut c_void,
            };

            let data_p: *mut c_void = &mut data as *mut _ as *mut c_void;
            //let mut hotOcall: HotCall = HOTCALL_INITIALIZER;
            let mut hotOcall_p = p_hotOcall as *mut u64 as *mut HotCall;

            HotCall_requestCall( hotOcall_p, 9, data_p );
        };
        
        debug_assert!(sgx_status == sgx_status_t::SGX_SUCCESS);
    }
}

extern "C" {
    fn occlum_ocall_posix_memalign(
        ptr: *mut *mut c_void,
        align: usize, // must be power of two and a multiple of sizeof(void*)
        size: usize,
    ) -> sgx_status_t;
    fn occlum_ocall_free(ptr: *mut c_void) -> sgx_status_t;
}
