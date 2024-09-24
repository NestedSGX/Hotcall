#define _GNU_SOURCE
#include <stdlib.h>
#include <sys/mman.h>
#include "ocalls.h"

void *occlum_ocall_posix_memalign(size_t alignment, size_t size);
void occlum_ocall_free(void *ptr);


// hot-calls
void occlum_ocall_posix_memalign_hotcall( void* d ) 
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    occlum_ocall_posix_memalignParams *data = d;
    *(data->result_p) = occlum_ocall_posix_memalign(data->alignment, data->size);
}

void occlum_ocall_free_hotcall( void* d )
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    occlum_ocall_freeParams *data = d;
    occlum_ocall_free(data->ptr);
}

// end hot-calls

void *occlum_ocall_posix_memalign(size_t alignment, size_t size) {
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    void *ptr = NULL;
    int ret = posix_memalign(&ptr, alignment, size);
    if (ret == 0) {
        return ptr;
    }

    // Handle errors
    switch (ret) {
        case ENOMEM:
            PAL_ERROR("Out of memory on the untrusted side");
            break;
        case EINVAL:
            PAL_ERROR("Invalid arguments given to occlum_ocall_posix_memalign");
            break;
        default:
            PAL_ERROR("Unexpected error in occlum_ocall_posix_memalign");
    }
    return NULL;
}

void occlum_ocall_free(void *ptr) {
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    free(ptr);
}

int occlum_ocall_mprotect(void *addr, size_t len, int prot) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    return mprotect(addr, len, prot);
}

int occlum_ocall_pkey_alloc(unsigned int flags, unsigned int access_rights) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    return pkey_alloc(flags, access_rights);
}

int occlum_ocall_pkey_mprotect(void *addr, size_t len, int prot, int pkey) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    return pkey_mprotect(addr, len, prot, pkey);
}

int occlum_ocall_pkey_free(int pkey) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    return pkey_free(pkey);
}
