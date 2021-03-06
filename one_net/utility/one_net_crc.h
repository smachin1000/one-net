#ifndef ONE_NET_CRC_H
#define ONE_NET_CRC_H

#include "config_options.h"


//! \defgroup one_net_crc Functionality for computing CRCs.
//! @{

/*
    Copyright (c) 2010, Threshold Corporation
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

        * Redistributions of source code must retain the above copyright notice,
          this list of conditions, and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in the
          documentation and/or other materials provided with the distribution.
        * Neither the name of Threshold Corporation (trustee of ONE-NET) nor the
          names of its contributors may be used to endorse or promote products
          derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
    CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
    BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
    OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHEWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*!
    \file one_net_crc.h
    \brief crc declarations.

    Declarations for computing various crcs.
    
    \note See one_net.h for the version of the ONE-NET source as a whole.  If
      any one file is modified, the version number in one_net.h will need to be
      updated.
*/

#include "one_net_types.h"


//==============================================================================
//                                  CONSTANTS
//! \defgroup one_net_crc_const 
//! \ingroup one_net_crc
//! @{

//! @} one_net_crc_const
//                                  CONSTANTS END
//==============================================================================

//==============================================================================
//                                  TYPEDEFS
//! \defgroup one_net_crc_typedefs
//! \ingroup one_net_crc
//! @{

//! @} one_net_crc_typedefs
//                                  TYPEDEFS END
//==============================================================================

//==============================================================================
//                              PUBLIC VARIABLES
//! \defgroup one_net_crc_pub_var
//! \ingroup one_net_crc
//! @{
    
    
#ifdef __cplusplus
extern "C"
{
#endif
    
    
//! Payload CRC releated constants
enum
{
    //! The size of the crc in the payload (in bytes)
    ON_PLD_CRC_SIZE = 1,

    //! The initial payload crc
    ON_PLD_INIT_CRC = 0xFF,

    //! The order of the payload crc
    ON_PLD_CRC_ORDER = 8,

    //! The initial crc for the non-volatile parameters
    ON_PARAM_INIT_CRC = 0xFF,

    //! The order of the crc computed over the non-volatile parameters
    ON_PARAM_CRC_ORDER = 8
};    
    

//! @} one_net_crc_pub_var
//                              PUBLIC VARIABLES END
//==============================================================================


//==============================================================================
//                      PUBLIC FUNCTION DECLARATIONS
//! \defgroup one_net_crc_pub_func
//! \ingroup one_net_crc
//! @{

UInt16 one_net_compute_crc(const UInt8 * const DATA, const UInt8 LEN, 
  const UInt16 STARTING_CRC, const UInt8 ORDER);

//! @} one_net_crc_pub_func
//                      PUBLIC FUNCTION DECLARATIONS END
//==============================================================================


#ifdef __cplusplus
}
#endif


//! @} one_net_crc

#endif // ONE_NET_CRC_H //
