#ifndef ONE_NET_FEATURES_H
#define ONE_NET_FEATURES_H

#include "config_options.h"
#include "one_net_port_const.h"
#include "one_net_types.h"
#include "one_net_data_rate.h"



//! \defgroup ONE-NET_FEATURES ONE-NET Features constants and functions
//! \ingroup ONE-NET
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
    \file one_net_features.h
    \brief Constants and typedefs dealing with features

    Functions, constants and typedefs dealing with features.
*/




//==============================================================================
//                                  TYPEDEFS
//! \defgroup ONE-NET_FEATURES_typedefs
//! \ingroup ONE-NET_FEATURES
//! @{


/*!
    Features (byte 0)
    Bit 7 is most significant bit, bit 0 is least significant

    00011010 represents a non-simple device that never sleeps, is not  multi-hop repeater,
    and has block capability, but not stream capability
*/
enum
{
    ON_DR_CHANNEL_CHANGE_FEATURE_SHIFT = 0,//! Bit 0 -- Mask = 0x01   True if the device can change channels / data rates
    ON_PEER_FEATURE_SHIFT           = 1,  //! Bit 1 -- Mask = 0x02   True if Device is peer capable
    ON_NON_SIMPLE_CLIENT_SHIFT      = 2,  //! Bit 2 -- Mask = 0x04   True if Device is not a simple client
    ON_DEVICE_NEVER_SLEEPS_SHIFT    = 3,  //! Bit 3 -- Mask = 0x08.  True if Device is always awake
    ON_BLOCK_FEATURE_SHIFT          = 4,  //! Bit 2 -- Mask = 0x10.  True if block messages are enabled
    ON_MH_FEATURE_SHIFT             = 5,  //! Bit 5 -- Mask = 0x20.  True if Multi-Hop capable
    ON_MH_REPEATER_FEATURE_SHIFT    = 6,  //! Bit 6 -- Mask = 0x40.  True if Multi-Hop repeater
    ON_STREAM_FEATURE_SHIFT         = 7,  //! Bit 7 -- Mask = 0x80.  True if stream messages enabled
    
    ON_DR_CHANNEL_CHANGE_FEATURE_MASK = 0x01,//! Bit 0 -- Mask = 0x01   True if the device can change channels / data rates
    ON_PEER_FEATURE_MASK            = 0x02,  //! Bit 1 -- Mask = 0x02   True if Device is peer capable
    ON_NON_SIMPLE_CLIENT_MASK       = 0x04,  //! Bit 2 -- Mask = 0x04   True if Device is not a simple client
    ON_DEVICE_NEVER_SLEEPS_MASK     = 0x08,  //! Bit 3 -- Mask = 0x08.  True if Device is always awake
    ON_BLOCK_FEATURE_MASK           = 0x10,  //! Bit 2 -- Mask = 0x10.  True if block messages are enabled
    ON_MH_FEATURE_MASK              = 0x20,  //! Bit 5 -- Mask = 0x20.  True if Multi-Hop capable
    ON_MH_REPEATER_FEATURE_MASK     = 0x40,  //! Bit 6 -- Mask = 0x40.  True if Multi-Hop repeater
    ON_STREAM_FEATURE_MASK          = 0x80,  //! Bit 7 -- Mask = 0x80.  True if stream messages enabled
};


/*!
    Extended Single And Route (bits 6 and 7 of byte 1 -- bytes 0 to 5 are data rates)
    Bit 7 is most significant bit, bit 0 is least significant

    01000001 represents a device that has Extended Single, but not Route Capabilities, and can only function at the 38,400 data rate
*/
enum
{
    ON_EXTENDED_SINGLE_FEATURE_SHIFT    = 6,  //! Bit 6 -- Mask = 0x40.  True if capable of sending 3-block XTEA Block messages.
    ON_ROUTE_FEATURE_SHIFT              = 7,  //! Bit 7 -- Mask = 0x80.  True if capable of participating in Route Messages.
    
    ON_EXTENDED_SINGLE_FEATURE_MASK     = 0x40,  //! Bit 6 -- Mask = 0x40.  True if capable of sending 3-block XTEA Block messages.
    ON_ROUTE_FEATURE_MASK               = 0x80,  //! Bit 7 -- Mask = 0x80.  True if participating in Route Messages.
};




enum
{
    ON_QUEUE_SIZE_MASK = 0xF0,
    ON_QUEUE_SIZE_SHIFT = 4,
    ON_QUEUE_LEVEL_MASK = 0x0C,
    ON_QUEUE_LEVEL_SHIFT = 2
};


enum
{
    ON_FEATURES_PEER_MASK = 0xF0,
    ON_FEATURES_PEER_SHIFT = 4,
    ON_FEATURES_MAX_HOPS_MASK = 0x0F    
};


// DEVICE_SLEEPS should be defined in one_net_client_port_const.h if this
// device EVER goes to sleep.  Note that one_net_port_const.h should #include
// one_net_client_port_const.h if ONE_NET_CLIENT is defined, so we don't need
// to #include it here.
enum
{
    THIS_DEVICE_FEATURE_BITS =
        0
        #ifdef DATA_RATE_CHANNEL
            + ON_DR_CHANNEL_CHANGE_FEATURE_MASK
        #endif
        #ifdef PEER
            + ON_PEER_FEATURE_MASK
        #endif
        #ifndef ONE_NET_SIMPLE_CLIENT
            + ON_NON_SIMPLE_CLIENT_MASK
        #endif
        #ifndef DEVICE_SLEEPS
            + ON_DEVICE_NEVER_SLEEPS_MASK
        #endif
        #ifdef BLOCK_MESSAGES_ENABLED
            + ON_BLOCK_FEATURE_MASK
        #endif        
        #ifdef ONE_NET_MULTI_HOP
            + ON_MH_FEATURE_MASK
        #endif
        #ifdef ONE_NET_MH_CLIENT_REPEATER
            + ON_MH_REPEATER_FEATURE_MASK
        #endif

        #ifdef STREAM_MESSAGES_ENABLED
            + ON_STREAM_FEATURE_MASK
        #endif
        ,
        
    THIS_DEVICE_DATA_RATES_EXTENDED_SINGLE_ROUTE =
        // note -- all devices have 38,400
        ONE_NET_DATA_RATE_38_4_MASK        
        #ifdef DATA_RATE_76_8_CAPABLE
            + ONE_NET_DATA_RATE_76_8_MASK
        #endif
        #ifdef DATA_RATE_115_2_CAPABLE
            + ONE_NET_DATA_RATE_115_2_MASK
        #endif
        #ifdef DATA_RATE_153_6_CAPABLE
            + ONE_NET_DATA_RATE_153_6_MASK
        #endif
        #ifdef DATA_RATE_192_0_CAPABLE
            + ONE_NET_DATA_RATE_192_0_MASK
        #endif
        #ifdef DATA_RATE_230_4_CAPABLE
            + ONE_NET_DATA_RATE_230_4_MASK
        #endif
        #ifdef EXTENDED_SINGLE
            + ON_EXTENDED_SINGLE_FEATURE_MASK
        #endif
        #ifdef ROUTE
            + ON_ROUTE_FEATURE_MASK
        #endif
        ,

    THIS_DEVICE_QUEUE_BITS = 0
      #if SINGLE_QUEUE_LEVEL > NO_SINGLE_QUEUE_LEVEL
      + (SINGLE_DATA_QUEUE_SIZE << ON_QUEUE_SIZE_SHIFT)
      #endif
      + (SINGLE_QUEUE_LEVEL << ON_QUEUE_LEVEL_SHIFT),


    THIS_DEVICE_PEERS_HOPS = 0
    #ifdef PEER
    + (ONE_NET_MAX_PEER_UNIT << ON_FEATURES_PEER_SHIFT)
    #endif
    #ifdef ONE_NET_MULTI_HOP
    + ON_MAX_HOPS_LIMIT
    #endif
};


// a structure with a lot of stuff squeezed together
typedef struct
{
    UInt8 feature_flags;
    UInt8 data_rates_extended_single_route_flags;
    UInt8 queue_values;
    UInt8 peers_hops;
} on_features_t;



//! @} ONE-NET_FEATURES_typedefs
//                                  TYPEDEFS END
//==============================================================================


//==============================================================================
//                                  CONSTANTS
//! \defgroup ONE-NET_FEATURES_const
//! \ingroup ONE-NET_FEATURES
//! @{

extern const on_features_t THIS_DEVICE_FEATURES;
extern const on_features_t FEATURES_UNKNOWN;

//! @} ONE-NET_FEATURES_const
//                                  CONSTANTS END
//==============================================================================


#ifdef __cplusplus
extern "C"
{
#endif


//==============================================================================
//                      PUBLIC FUNCTION DECLARATIONS
//! \defgroup ONE-NET_FEATURES_pub_func
//! \ingroup ONE-NET_FEATURES
//! @{

BOOL features_known(on_features_t features);
BOOL features_device_sleeps(on_features_t features);
#ifndef ONE_NET_SIMPLE_CLIENT
UInt8 features_max_hops(on_features_t features);
UInt8 features_max_peers(on_features_t features);
UInt8 features_queue_size(on_features_t features);
UInt8 features_queue_level(on_features_t features);
BOOL features_data_rate_capable(on_features_t features, UInt8 data_rate);
// TODO -- add #ifdef check for _DATA_RATE?
UInt8 features_highest_data_rate(on_features_t features);
UInt8 features_highest_matching_data_rate(on_features_t dev1_features,
  on_features_t dev2_features);
BOOL features_peer_capable(on_features_t features);
BOOL features_mh_capable(on_features_t features);
BOOL features_mh_repeat_capable(on_features_t features);
BOOL features_block_capable(on_features_t features);
BOOL features_stream_capable(on_features_t features);
BOOL features_simple_client(on_features_t features);
BOOL features_dr_channel_capable(on_features_t features);
BOOL features_extended_single_capable(on_features_t features);
BOOL features_route_capable(on_features_t features);
#endif

//! @} ONE-NET_FEATURES_pub_func
//                      PUBLIC FUNCTION DECLARATIONS END
//==============================================================================


#ifdef __cplusplus
}
#endif



//! @} ONE-NET_FEATURES

#endif // ONE_NET_FEATURES_H //
