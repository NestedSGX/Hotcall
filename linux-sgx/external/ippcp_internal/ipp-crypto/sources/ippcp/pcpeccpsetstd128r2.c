/*******************************************************************************
* Copyright (C) 2016 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
* http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions
* and limitations under the License.
* 
*******************************************************************************/

/*
//
//  Purpose:
//     Cryptography Primitive.
//     EC over Prime Finite Field (setup/retrieve domain parameters)
//
//  Contents:
//        ippsECCPSetStd128r2()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpeccp.h"

/*F*
//    Name: ippsECCPSetStd128r2
//
// Purpose: Set EC128r2 parameters
//
// Returns:                Reason:
//    ippStsNullPtrErr        NULL == pEC
//
//    ippStsContextMatchErr   illegal pEC->idCtx
//
//    ippStsNoErr             no errors
//
// Parameters:
//    pEC     pointer to the ECC context
//
*F*/
IPPFUN(IppStatus, ippsECCPSetStd128r2, (IppsECCPState* pEC))
{
   /* test pEC */
   IPP_BAD_PTR1_RET(pEC);

   /* set domain parameters */
   return ECCPSetDP(ippsGFpMethod_pArb(),
                        BITS_BNU_CHUNK(128), secp128r2_p,
                        BITS_BNU_CHUNK(128), secp128r2_a,
                        BITS_BNU_CHUNK(128), secp128r2_b,
                        BITS_BNU_CHUNK(128), secp128r2_gx,
                        BITS_BNU_CHUNK(128), secp128r2_gy,
                        BITS_BNU_CHUNK(128), secp128r2_r,
                        secp128r2_h,
                        pEC);
}