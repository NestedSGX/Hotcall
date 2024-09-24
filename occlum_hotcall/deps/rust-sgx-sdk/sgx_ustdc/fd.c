// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License..

#define _LARGEFILE64_SOURCE

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

// hot-calls
#include "../../../src/pal/src/hotcalls/common.h"
#include "../../../src/pal/src/hotcalls/hot_calls.h"

ssize_t u_read_ocall(int *error, int fd, void *buf, size_t count); 
int u_fcntl_arg1_ocall(int *error, int fd, int cmd, int arg);
int u_close_ocall(int *error, int fd);

inline __attribute__((always_inline))  uint64_t rdtscp(void)
{
        unsigned int low, high;

        asm volatile("rdtscp" : "=a" (low), "=d" (high));

        return low | ((uint64_t)high) << 32;
}

void read_hotcall( void* d )
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    static uint64_t startTime = 0;
   
    readParams *data = d;

    printf("time: %lld, count: %d\n", rdtscp() - startTime, data->count);

    *(data->result_p) = u_read_ocall(data->error_p, data->fd, data->buf, data->count);

    startTime = rdtscp();
}

void fcntl_arg1_hotcall( void* d )
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    fcntl_arg1Params *data = d;

    *(data->result_p) = u_fcntl_arg1_ocall(data->error_p, data->fd, data->cmd, data->arg);
}

void close_hotcall( void* d )
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    closeParams *data = d;

    *(data->result_p) = u_close_ocall(data->error_p, data->fd);
}

// end hot-calls

ssize_t u_read_ocall(int *error, int fd, void *buf, size_t count)
{
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    ssize_t ret = read(fd, buf, count);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_pread64_ocall(int *error, int fd, void *buf, size_t count, off64_t offset)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    ssize_t ret = pread64(fd, buf, count, offset);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_readv_ocall(int *error, int fd, const struct iovec *iov, int iovcnt)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    ssize_t ret = readv(fd, iov, iovcnt);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_preadv64_ocall(int *error, int fd, const struct iovec *iov, int iovcnt, off64_t offset)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    ssize_t ret = preadv64(fd, iov, iovcnt, offset);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_write_ocall(int *error, int fd, const void *buf, size_t count)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    ssize_t ret = write(fd, buf, count);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_pwrite64_ocall(int *error, int fd, const void *buf, size_t count, off64_t offset)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    ssize_t ret = pwrite64(fd, buf, count, offset);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_writev_ocall(int *error, int fd, const struct iovec *iov, int iovcnt)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    ssize_t ret = writev(fd, iov, iovcnt);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_pwritev64_ocall(int *error, int fd, const struct iovec *iov, int iovcnt, off64_t offset)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    ssize_t ret = pwritev64(fd, iov, iovcnt, offset);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_fcntl_arg0_ocall(int *error, int fd, int cmd)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    int ret = fcntl(fd, cmd);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_fcntl_arg1_ocall(int *error, int fd, int cmd, int arg)
{
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    int ret = fcntl(fd, cmd, arg);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_ioctl_arg0_ocall(int *error, int fd, int request)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    int ret = ioctl(fd, (unsigned long)request);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_ioctl_arg1_ocall(int *error, int fd, int request, int *arg)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    int ret = ioctl(fd, (unsigned long)request, arg);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_close_ocall(int *error, int fd)
{
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    int ret = close(fd);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}