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

#include <sched.h>
#include <time.h>
#include <errno.h>

// hot-calls
#include "../../../src/pal/src/hotcalls/common.h"
#include "../../../src/pal/src/hotcalls/hot_calls.h"

int  u_sched_yield_ocall(int *error) 
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int ret = sched_yield();
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;
}

int u_nanosleep_ocall(int *error, const struct timespec *req, struct timespec *rem) 
{
#ifdef SYSCALL_DEBUG
    printf("[%s] %s: %d\n", \
			 __FILE__, __FUNCTION__, __LINE__);
    
#endif
    int ret = nanosleep(req, rem);
    if (error) {
        *error = ret == -1 ? errno : 0;
    }
    return ret;          
}

