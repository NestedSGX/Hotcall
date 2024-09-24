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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

// hot-calls
#include "../../../src/pal/src/hotcalls/common.h"
#include "../../../src/pal/src/hotcalls/hot_calls.h"

int u_setsockopt_ocall(int *error,
                       int sockfd,
                       int level,
                       int optname,
                       const void *optval,
                       socklen_t optlen);

int u_accept4_ocall(int *error,
                    int sockfd,
                    struct sockaddr *addr,
                    socklen_t addrlen_in,
                    socklen_t *addrlen_out,
                    int flags);

int u_getsockopt_ocall(int *error,
                       int sockfd,
                       int level,
                       int optname,
                       void *optval,
                       socklen_t optlen_in,
                       socklen_t *optlen_out);

int u_shutdown_ocall(int *error, int sockfd, int how);

void setsockopt_hotcall( void* d )
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    setsockoptParams *data = d;
    *(data->result_p) = u_setsockopt_ocall(data->error_p, data->sockfd, data->level, data->optname, data->optval, data->optlen);
}

void accept4_hotcall( void* d )
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    accept4Params *data = d;

    *(data->result_p) = u_accept4_ocall(data->error_p, data->sockfd, data->addr, data->addrlen_in, data->addrlen_out, data->flags);
}

void getsockopt_hotcall( void* d )
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
#endif
    getsockoptParams *data = d;
    *(data->result_p) = u_getsockopt_ocall(data->error_p, data->sockfd, data->level, data->optname, data->optval, data->optlen_in, data->optlen_out);
}

void shutdown_hotcall( void* d )
{   
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    shutdownParams *data = d;
    *(data->result_p) = u_shutdown_ocall(data->error_p, data->sockfd, data->how);
}
///////////////

int u_socket_ocall(int *error, int domain, int ty, int protocol)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif

    int ret = socket(domain, ty, protocol);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_socketpair_ocall(int *error, int domain, int ty, int protocol, int sv[2])
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int ret = socketpair(domain, ty, protocol, sv);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_bind_ocall(int *error, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int ret = bind(sockfd, addr, addrlen);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_listen_ocall(int *error, int sockfd, int backlog)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int ret = listen(sockfd, backlog);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_accept_ocall(int *error,
                   int sockfd,
                   struct sockaddr *addr,
                   socklen_t addrlen_in,
                   socklen_t *addrlen_out)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    *addrlen_out = addrlen_in;
    int ret = accept(sockfd, addr, addrlen_out);
     if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_accept4_ocall(int *error,
                    int sockfd,
                    struct sockaddr *addr,
                    socklen_t addrlen_in,
                    socklen_t *addrlen_out,
                    int flags)
{
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    *addrlen_out = addrlen_in;
    int ret = accept4(sockfd, addr, addrlen_out, flags);
     if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_connect_ocall(int *error, int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int ret = connect(sockfd, addr, addrlen);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_recv_ocall(int *error, int sockfd, void *buf, size_t len, int flags)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    ssize_t ret = recv(sockfd, buf, len, flags);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_recvfrom_ocall(int *error,
                         int sockfd,
                         void *buf,
                         size_t len,
                         int flags,
                         struct sockaddr *src_addr,
                         socklen_t addrlen_in,
                         socklen_t *addrlen_out)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    *addrlen_out = addrlen_in;
    ssize_t ret = recvfrom(sockfd, buf, len, flags, src_addr, addrlen_out);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_recvmsg_ocall(int *error, int sockfd, struct msghdr *msg, int flags)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    ssize_t ret = recvmsg(sockfd, msg, flags);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_send_ocall(int *error, int sockfd, const void *buf, size_t len, int flags)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    ssize_t ret = send(sockfd, buf, len, flags);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_sendto_ocall(int *error,
                       int sockfd,
                       const void *buf,
                       size_t len,
                       int flags,
                       const struct sockaddr *dest_addr,
                       socklen_t addrlen)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    ssize_t ret = sendto(sockfd, buf, len, flags, dest_addr, addrlen);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

ssize_t u_sendmsg_ocall(int *error, int sockfd, const struct msghdr *msg, int flags)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    ssize_t ret = sendmsg(sockfd, msg, flags);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_getsockopt_ocall(int *error,
                       int sockfd,
                       int level,
                       int optname,
                       void *optval,
                       socklen_t optlen_in,
                       socklen_t *optlen_out)
{
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    *optlen_out = optlen_in;
    int ret = getsockopt(sockfd, level, optname, optval, optlen_out);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_setsockopt_ocall(int *error,
                       int sockfd,
                       int level,
                       int optname,
                       const void *optval,
                       socklen_t optlen)
{
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int ret = setsockopt(sockfd, level, optname, optval, optlen);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_getsockname_ocall(int *error,
                        int sockfd,
                        struct sockaddr *addr,
                        socklen_t addrlen_in,
                        socklen_t *addrlen_out)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    *addrlen_out = addrlen_in;
    int ret = getsockname(sockfd, addr, addrlen_out);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_getpeername_ocall(int *error,
                        int sockfd,
                        struct sockaddr *addr,
                        socklen_t addrlen_in,
                        socklen_t *addrlen_out)
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    *addrlen_out = addrlen_in;
    int ret = getpeername(sockfd, addr, addrlen_out);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_shutdown_ocall(int *error, int sockfd, int how)
{
#ifdef SYSCALL_DEBUG_FINISHED
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int ret = shutdown(sockfd, how);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}