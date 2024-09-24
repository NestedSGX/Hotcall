#include "ocalls.h"
#include <errno.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/vfs.h>

int occlum_ocall_ioctl(int fd, int request, void *arg, size_t len);

// hot-calls
void occlum_ocall_ioctl_hotcall( void* d ) 
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    occlum_ocall_ioctlParams *data = d;
    *(data->result_p) = occlum_ocall_ioctl(data->fd, data->request, data->arg, data->len);
}


void occlum_ocall_sync(void) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    sync();
}

int occlum_ocall_ioctl_repack(int fd, int request, char *buf, int len, int *recv_len) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    int ret = 0;

    switch (request) {
        case SIOCGIFCONF:
            if (recv_len == NULL) {
                errno = EINVAL;
                return -1;
            }

            struct ifconf config = { .ifc_len = len, .ifc_buf = buf };
            ret = ioctl(fd, SIOCGIFCONF, &config);
            if (ret == 0) {
                *recv_len = config.ifc_len;
            }
            break;

        default:
            errno = EINVAL;
            return -1;
    }

    return ret;
}

int occlum_ocall_ioctl(int fd, int request, void *arg, size_t len) {
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    if (((arg == NULL) ^ (len == 0)) == 1) {
        errno = EINVAL;
        return -1;
    }

    return ioctl(fd, request, arg);
}

int occlum_ocall_statfs(const char *path, struct statfs *buf) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    return statfs(path, buf);
}