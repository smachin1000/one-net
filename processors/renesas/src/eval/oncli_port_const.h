#ifndef ONCLI_PORT_CONST_H
#define ONCLI_PORT_CONST_H


//! \defgroup oncli_port_const ONE-NET Command Line Interface port specific
//!   constants
//! \ingroup oncli_port
//! @{

/*
    Copyright (c) 2011, Threshold Corporation
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
    \file oncli_port_const.h
    \brief ONE-NET Command Line Interface port specific constants.

    These are constants that are specific to each device that implements the
    ONE-NET Command Line Interface.

    \note See one_net.h for the version of the ONE-NET source as a whole.  If
      any one file is modified, the version number in one_net.h will need to be
      updated.
*/


#include "config_options.h"

#ifdef ENABLE_CLI

#include "tick.h"


//==============================================================================
//                                  CONSTANTS
//! \defgroup oncli_port_const_const
//! \ingroup oncli_port_const
//! @{

enum
{
    //! The maximum command string length, including the NULL termination
    //! (needs to be big enough to handle the longest command string in
    //! oncli_str.h).
    ONCLI_MAX_INPUT_STR_LEN = 128,
    
    //! Max length for the output string, including byte for NULL termination
    ONCLI_MAX_OUTPUT_STR_LEN = 128
};

//! @} oncli_port_const_const
//                                  CONSTANTS END
//==============================================================================

//==============================================================================
//                                  TYPEDEFS
//! \defgroup oncli_port_const_typedefs
//! \ingroup oncli_port_const
//! @{

//! @} oncli_port_const_typedefs
//                                  TYPEDEFS END
//==============================================================================

//==============================================================================
//                              PUBLIC VARIABLES
//! \defgroup oncli_port_const_pub_var
//! \ingroup oncli_port_const
//! @{

//! @} oncli_port_const_pub_var
//                              PUBLIC VARIABLES END
//==============================================================================

//==============================================================================
//                      PUBLIC FUNCTION DECLARATIONS
//! \defgroup oncli_port_const_pub_func
//! \ingroup oncli_port_const
//! @{

//! @} oncli_port_const_pub_func
//                      PUBLIC FUNCTION DECLARATIONS END
//==============================================================================

//! @} oncli_port_const

#endif // #ifdef ENABLE_CLI

#endif // ONCLI_PORT_CONST_H //

