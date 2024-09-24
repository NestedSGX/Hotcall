// ----------------------------------------
// HotCalls
// Copyright 2017 The Regents of the University of Michigan
// Ofir Weisse, Valeria Bertacco and Todd Austin

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ---------------------------------------------

//Author: Ofir Weisse, www.OfirWeisse.com, email: oweisse (at) umich (dot) edu
//Based on ISCA 2017 "HotCalls" paper. 
//Link to the paper can be found at http://www.ofirweisse.com/previous_work.html
//If you make nay use of this code for academic purpose, please cite the paper. 


#ifndef __COMMON_H
#define __COMMON_H

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <stdint.h>

#include <signal.h>
#include <poll.h>

//#define SYSCALL_DEBUG 1

//#define  SYSCALL_DEBUG_FINISHED 1 // whether to print hot-call and related calls


//typedef struct {
//    uint64_t* cyclesCount;
 //   uint64_t  counter;
//} OcallParams;

typedef struct {
    int * result_p;
    int * error_p;
    int sockfd;
    int level;
    int optname;
    const void * optval;
    socklen_t optlen;
} setsockoptParams;

typedef struct {
    int * result_p;
    int * error_p;
    int sockfd;
    struct sockaddr *addr;
    socklen_t addrlen_in;
    socklen_t *addrlen_out;
    int flags;
} accept4Params;

typedef struct {
    int * result_p;
    int * error_p;
    int sockfd;
    int level;
    int optname;
    const void * optval;
    socklen_t optlen_in;
    socklen_t *optlen_out;
} getsockoptParams;

typedef struct {
    int * result_p;
    int * error_p;
    int sockfd;
    int how;
} shutdownParams;

typedef struct {
    int * result_p;
    int * error_p;
    int fd;
    int cmd;
    int arg;
} fcntl_arg1Params;


typedef struct {
    ssize_t * result_p;
    int * error_p;
    int fd;
    void *buf;
    size_t count;
} readParams;

typedef struct {
    ssize_t * result_p;
    int sockfd;
    const void *msg_name;
    socklen_t msg_namelen;
    const struct iovec *msg_iov;
    size_t msg_iovlen;
    const void *msg_control;
    size_t msg_controllen;
    int flags;
} occlum_sendmsgParams;

typedef struct {
    ssize_t * result_p;
    int sockfd;
    void *msg_name;
    socklen_t msg_namelen;
    socklen_t *msg_namelen_recv;
    struct iovec *msg_iov;
    size_t msg_iovlen;
    void *msg_control;
    size_t msg_controllen;
    size_t *msg_controllen_recv;
    int *msg_flags_recv;
    int flags;
} occlum_recvmsgParams;

typedef struct {
    void ** result_p;
    size_t alignment;
    size_t size;
} occlum_ocall_posix_memalignParams;

typedef struct {
    void *ptr;
} occlum_ocall_freeParams;

typedef struct {
    int * result_p;
    int fd;
    int request;
    void *arg;
    size_t len;
} occlum_ocall_ioctlParams;

typedef struct {
    int * result_p;
    int * error_p;
    int fd;
} closeParams;

typedef struct {
    int clockid;
    struct timespec *tp;
} occlum_ocall_clock_gettimeParams;

typedef struct {
    int * result_p;
    int * error_p;
    int epfd;
    struct epoll_event *events;
    int maxevents;
    int timeout;
} epoll_waitParams;

typedef struct {
    int * result_p;
    struct pollfd *pollfds;
    nfds_t nfds;
    struct timespec *timeout;
    int eventfd_idx;
} occlum_poll_with_eventfdParams;

typedef struct {
    int *eventfds;
    size_t num_fds;
    uint64_t val;
} occlum_eventfd_write_batchParams;

#endif