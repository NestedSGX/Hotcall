#include <pthread.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/prctl.h>
#include "ocalls.h"

void occlum_ocall_clock_gettime(int clockid, struct timespec *tp);

// hot-calls
void occlum_ocall_clock_gettime_hotcall( void* d ) 
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    occlum_ocall_clock_gettimeParams *data = d;
    occlum_ocall_clock_gettime(data->clockid, data->tp);
}



void occlum_ocall_gettimeofday(struct timeval *tv) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    gettimeofday(tv, NULL);
}

void occlum_ocall_clock_gettime(int clockid, struct timespec *tp) {
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    clock_gettime(clockid, tp);
}

void occlum_ocall_clock_getres(int clockid, struct timespec *res) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    clock_getres(clockid, res);
}

int occlum_ocall_clock_nanosleep(clockid_t clockid, int flags, const struct timespec *req,
                                 struct timespec *rem) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    return clock_nanosleep(clockid, flags, req, rem);
}

int occlum_ocall_thread_getcpuclock(struct timespec *tp) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    clockid_t thread_clock_id;
    int ret = pthread_getcpuclockid(pthread_self(), &thread_clock_id);
    if (ret != 0) {
        PAL_ERROR("failed to get clock id");
        return -1;
    }

    return clock_gettime(thread_clock_id, tp);
}

void occlum_ocall_rdtsc(uint32_t *low, uint32_t *high) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    uint64_t rax, rdx;
    asm volatile("rdtsc" : "=a"(rax), "=d"(rdx));
    *low = (uint32_t)rax;
    *high = (uint32_t)rdx;
}

void occlum_ocall_get_timerslack(int *timer_slack) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int nanoseconds = prctl(PR_GET_TIMERSLACK, 0, 0, 0, 0);
    *timer_slack = nanoseconds;
}

int occlum_ocall_timerfd_create(int clockid, int flags) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    return timerfd_create(clockid, flags);
}

int occlum_ocall_timerfd_settime(int fd, int flags, const struct itimerspec *new_value,
                                 struct itimerspec *old_value) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    return timerfd_settime(fd, flags, new_value, old_value);
}

int occlum_ocall_timerfd_gettime(int fd, struct itimerspec *curr_value) {
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    return timerfd_gettime(fd, curr_value);
}