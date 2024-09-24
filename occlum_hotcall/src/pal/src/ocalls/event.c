#define _GNU_SOURCE
#include "ocalls.h"
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <unistd.h>
#include <sys/eventfd.h>

// hot-calls
int occlum_ocall_poll_with_eventfd(
    struct pollfd *pollfds,
    nfds_t nfds,
    struct timespec *timeout,
    int eventfd_idx
);

void occlum_ocall_eventfd_write_batch(
    int *eventfds,
    size_t num_fds,
    uint64_t val
);

void occlum_poll_with_eventfd_hotcall( void* d ) 
{   
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    occlum_poll_with_eventfdParams *data = d;
    *(data->result_p) = occlum_ocall_poll_with_eventfd(data->pollfds, data->nfds, data->timeout, data->eventfd_idx);
}

void occlum_eventfd_write_batch_hotcall( void* d ) 
{   
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    occlum_eventfd_write_batchParams *data = d;
    occlum_ocall_eventfd_write_batch(data->eventfds, data->num_fds, data->val);
}

// end of hot-calls

int occlum_ocall_eventfd(unsigned int initval, int flags) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    return eventfd(initval, flags);
}

int occlum_ocall_eventfd_poll(int eventfd, struct timespec *timeout) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int ret;

    struct pollfd pollfds[1];
    pollfds[0].fd = eventfd;
    pollfds[0].events = POLLIN;
    pollfds[0].revents = 0;

    // We use the ppoll syscall directly instead of the libc wrapper. This
    // is because the syscall version updates the timeout argument to indicate
    // how much time was left (which what we want), while the libc wrapper
    // keeps the timeout argument unchanged.
    ret = RAW_PPOLL(pollfds, 1, timeout);
    if (ret < 0) {
        return -1;
    }

    char buf[8];
    read(eventfd, buf, 8);
    return 0;
}

void occlum_ocall_eventfd_write_batch(
    int *eventfds,
    size_t num_fds,
    uint64_t val
) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    for (int fd_i = 0; fd_i < num_fds; fd_i++) {
        write(eventfds[fd_i], &val, sizeof(val));
    }
}

int occlum_ocall_poll_with_eventfd(
    struct pollfd *pollfds,
    nfds_t nfds,
    struct timespec *timeout,
    int eventfd_idx
) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    if (eventfd_idx >= 0) {
        pollfds[eventfd_idx].events |= POLLIN;
    }

    // We use the ppoll syscall directly instead of the libc wrapper. This
    // is because the syscall version updates the timeout argument to indicate
    // how much time was left (which what we want), while the libc wrapper
    // keeps the timeout argument unchanged.
    int ret = RAW_PPOLL(pollfds, nfds, timeout);
    if (ret < 0) {
        return -1;
    }

    if (eventfd_idx >= 0 && (pollfds[eventfd_idx].revents & POLLIN) != 0) {
        int eventfd = pollfds[eventfd_idx].fd;
        char buf[8];
        read(eventfd, buf, 8);
    }

    return ret;
}
