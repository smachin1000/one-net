//! \defgroup HAL_ADI Processor Abstraction Layer for ADI ADF7025.
//! \ingroup ADI
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

#include "config_options.h"
#ifdef HAS_LEDS
    #include "one_net_led.h"
#endif

/*!
    \file hal_adi.h
    \brief Processor abstraction layer for the ADI ADF7025.

    This file declares the processor & board specific functionality needed by
    the ADI transceiver.
*/


//=============================================================================
//                                  CONSTANTS
//! \defgroup HAL_ADI_const
//! \ingroup HAL_ADI
//! @{

//! @} HAL_ADI_const
//                                  CONSTANTS END
//=============================================================================

//=============================================================================
//                                  TYPEDEFS
//! \defgroup HAL_ADI_typedefs
//! \ingroup HAL_ADI
//! @{

//! @} HAL_ADI_typedefs
//                                  TYPEDEFS END
//=============================================================================

//=============================================================================
//                              PUBLIC VARIABLES
//! \defgroup HAL_ADI_pub_var
//! \ingroup HAL_ADI
//! @{

//! @} HAL_ADI_pub_var
//                              PUBLIC VARIABLES END
//=============================================================================

//=============================================================================
//                      PUBLIC FUNCTION DECLARATIONS
//! \defgroup HAL_ADI_pub_func
//! \ingroup HAL_ADI
//! @{


void tal_init_ports(void);


/*!
    \brief Enables the interrupt used to receive bits.

    This is the interrupt attached to the DATACLK pin from the ADI.

    \param void

    return void
*/
#ifdef HAS_LEDS
#define ENABLE_RX_BIT_INTERRUPTS()   bit_mask = 0x80; int0en = 1; set_rx_led(TRUE)
#else
#define ENABLE_RX_BIT_INTERRUPTS()   bit_mask = 0x80; int0en = 1
#endif


/*!
    \brief Disables the interrupt used to receive bits.

    This is the interrupt attached to the DATACLK pin from the ADI.

    \param void

    return void
*/
#define DISABLE_RX_BIT_INTERRUPTS() int0en = 0


/*!
    \brief Enables the interrupt used to transmit bits.

    This is a timer interrupt.

    \param void

    return void
*/
#ifdef HAS_LEDS
#define ENABLE_TX_BIT_INTERRUPTS()   bit_mask = 0x80; tstart_tracr = 1; set_tx_led(TRUE)
#else
#define ENABLE_TX_BIT_INTERRUPTS()   bit_mask = 0x80; tstart_tracr = 1
#endif


/*!
    \brief Disables the interrupt used to transmit bits.

    This is a timer interrupt.

    \param void

    return void
*/
#ifdef HAS_LEDS
#define DISABLE_TX_BIT_INTERRUPTS()  tstart_tracr = 0; set_tx_led(FALSE)
#else
#define DISABLE_TX_BIT_INTERRUPTS()  tstart_tracr = 0
#endif



//! @} HAL_ADI_pub_func
//                      PUBLIC FUNCTION DECLARATIONS END
//=============================================================================

//! @} ADI
