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


#ifndef __HOT_CALL_COMMON_H
#define __HOT_CALL_COMMON_H

#include <stdint.h>

//#define SYSCALL_DEBUG 1

//#define  SYSCALL_DEBUG_FINISHED 1 // whether to print hot-call and related calls
//#pragma pack(push, 1)

//#define HOTCALL_ENABLE 1


typedef struct {
    int32_t *result_p;
    void *f;
    uint64_t node_number;
    uint8_t* buffer;
    uint32_t node_size;
    uint64_t HotCallAddr;
} sgxprotectedfs_fread_nodeParams;

#endif