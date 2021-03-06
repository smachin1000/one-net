#ifndef ONE_NET_ACKNOWLEDGE_H
#define ONE_NET_ACKNOWLEDGE_H

#include "config_options.h"
#include "one_net_types.h"
#include "one_net_packet.h"
#include "one_net_features.h"
#include "one_net_xtea.h"



//! \defgroup ONE-NET_ACKNOWLEDGE ONE-NET ACK / NACK constants and functions
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
    \file one_net_acknowledge.h
    \brief Constants and typedefs dealing with acks and nacks

    Functions, constants and typedefs dealing with acks and nacks.
*/




//==============================================================================
//                                  TYPEDEFS
//! \defgroup ONE-NET_ACKNOWLEDGE_typedefs
//! \ingroup ONE-NET_ACKNOWLEDGE
//! @{

/*!
    Nack Reasons
*/
typedef enum
{
    // add more as needed in the gaps provided -- 0 = no error, 0x01 to 0x3F are ONE-NET
    // non-fatal errors, 0x80 to 0xBF are ONE-NET fatal errors, 0x40 to 0x7F are user-
    // defined non-fatal NACK Reasons, 0xC0 to 0xFF are user defined fatal NACK reasons.
    // Any reason that can be either fatal OR non-fatal should be defined as non-fatal
    // and application code will be called to adjust the fatality if the application
    // code desires.
    
    // 0 is no error
    ON_NACK_RSN_NO_ERROR,            //! Not an error.  Should not be part of a packet, but defined so that we can use it as "success" when coding

    ON_NACK_RSN_MIN_NON_FATAL = 0x01,
    ON_NACK_RSN_MIN_ONE_NET_NON_FATAL = ON_NACK_RSN_MIN_NON_FATAL,

    // 0x01 through 0x3F are reserved for non-fatal ONE-NET-defined errors
    ON_NACK_RSN_RSRC_UNAVAIL_ERR = ON_NACK_RSN_MIN_ONE_NET_NON_FATAL,    //! resource(s) necessary to complete the transaction not currently available
    ON_NACK_RSN_INTERNAL_ERR,        //! something unanticipated occurred - Under normal circumstances, this should never be received (as it indicates an implementation fault in the sender); Devices are required to process it, however.    
    ON_NACK_RSN_BUSY_TRY_AGAIN,      //! Application level code specifying that the device cannot service the request at this time, but will likely be able to do so very soon.  Considered "non-fatal" by ONE-NET.
    ON_NACK_RSN_BUSY_TRY_AGAIN_TIME, //! Application level code specifying that the device cannot service the request at this time, but will likely be able to do so very soon.  Considered "non-fatal" by ONE-NET, but application code may override.
    ON_NACK_RSN_BAD_POSITION_ERROR,  //! Position/Offset is invalid and/or does not match what is expected by the device.
    ON_NACK_RSN_BAD_SIZE_ERROR,      //! Length/Size is invalid and/or does not mach what is expected by the device.  Different from ON_NACK_RSN_INVALID_LENGTH_ERRON_NACK_RSN_INVALID_LENGTH_ERR, which means that the device cannot HANDLE something this size.
    ON_NACK_RSN_BAD_ADDRESS_ERR,     //! DID or NID is either not decodable or is incorrect.
    ON_NACK_RSN_INVALID_MAX_HOPS,    //! Packet either has a "max hops" that is too large or there  is some other problem dealing with max hops.
    ON_NACK_RSN_INVALID_HOPS,        //! Packet has a "hops" field that is invalid for whatever reason.
    ON_NACK_RSN_INVALID_PEER,        //! Some problem dealing with an invalid peer.  Peer is not in the peer table, peer is out of range or cannot handle this message, etc.
    ON_NACK_RSN_OUT_OF_RANGE,        //! Device is out of range, is asleep, or cannot be reached for some reason.
    ON_NACK_RSN_ROUTE_ERROR,         //! message is not routed properly or no repeaters are available that can reach the device.
    ON_NACK_RSN_INVALID_DATA_RATE,   //! The transaction cannot be completed at this data rate either because device(s) cannot handle the rate it or currently unabled / not allowed to to swith to it. 
    ON_NACK_RSN_NO_RESPONSE,         //! A single message within the transaction has timed out with no response.
    ON_NACK_RSN_INVALID_MSG_ID,      //! Invalid message id
    ON_NACK_RSN_NEED_FEATURES,       //! Do not have the device's features
    ON_NACK_RSN_FEATURES,            //! A general error involving features / capabilities other than not having them.
    ON_NACK_RSN_BAD_CRC,             //! A CRC calculation failed
    ON_NACK_RSN_BAD_KEY,             //! Sent when the other device is using the wrong key
    ON_NACK_RSN_ALREADY_IN_PROGRESS, //! Similar to "Resouces Unavailable", but more detailed.
    ON_NACK_RSN_NOT_ALREADY_IN_PROGRESS, //! Similar to the "alredy in progress" error, but sort of inverted.
                                         //! A request is being made that cannot be complied with because something else
                                         //! needed to happen first which has not yet happened.
    ON_NACK_RSN_INVALID_CHANNEL,     //! The channel requested is a channel that this device cannot access or is invalid
                                     //! for some other reason.
    ON_NACK_RSN_INVALID_CHUNK_SIZE,  //! An invalid chunk size has been specified for a block / stream transfer
    ON_NACK_RSN_INVALID_CHUNK_DELAY, //! An invalid chunk delay has been specified for a block / stream transfer
    ON_NACK_RSN_INVALID_BYTE_INDEX,  //! An invalid byte index has been specified for a block / stream transfer
    ON_NACK_RSN_INVALID_FRAG_DELAY,  //! An invalid fragment delay has been specified for a block / stream transfer
    ON_NACK_RSN_INVALID_PRIORITY,    //! An invalid priority has been specified.
    ON_NACK_RSN_PERMISSION_DENIED_NON_FATAL, //! Generic "Permission denied" non-fatal error.

    
    // Add any more non-fatal ONE-NET Nack Reasons here
    
    ON_NACK_RSN_UNSET = 0x3E,        //! NACK Reason is not set yet.
    ON_NACK_RSN_GENERAL_ERR = 0x3F,  //! If no specific reason is known
    
    // 0x40 through 0x7F are reserved for non-fatal user-defined errors
    ON_NACK_RSN_MIN_USR_NON_FATAL = 0x40, //! User-defined non-fatal NACK code start
    ON_NACK_RSN_MAX_USR_NON_FATAL = 0x7F, //! User-defined non-fatal NACK code start

    // 0x80 through 0xBF are reserved for fatal ONE-NET-defined errors

    // fatal ONE-NET Errors
    ON_NACK_RSN_MIN_FATAL = 0x80,
    ON_NACK_RSN_MIN_ONE_NET_FATAL = ON_NACK_RSN_MIN_FATAL,
    ON_NACK_RSN_INVALID_LENGTH_ERR = ON_NACK_RSN_MIN_ONE_NET_FATAL,  //! specified Device/Unit cannot handle a transaction with the specified length
    ON_NACK_RSN_DEVICE_FUNCTION_ERR, //! specified Device lacks the functionality to properly process the received packet
    ON_NACK_RSN_UNIT_FUNCTION_ERR,   //! specified Unit lacks the functionality to properly process the received packet (although the Device itself does)
    ON_NACK_RSN_INVALID_UNIT_ERR,    //! nonexistent Unit specified
    ON_NACK_RSN_MISMATCH_UNIT_ERR,   //! Unit pair specified in Block Data Segment does not match that in Request to Receive Block
    ON_NACK_RSN_BAD_DATA_ERR,        //! improperly formatted data
    ON_NACK_RSN_TRANSACTION_ERR,     //! invalid transaction specified (such as a Block Data packet in the absence of a previous Block Request)
    ON_NACK_RSN_MAX_FAILED_ATTEMPTS_REACHED, //! Attempted and failed too many times.
    ON_NACK_RSN_BUSY,                //! Application level code specifying that the device cannot service the request at this time.  No specification of when to try again.  Considered "fatal" by ONE-NET.
    ON_NACK_RSN_NO_RESPONSE_TXN,     //! The transaction has timed out with no response.
    ON_NACK_RSN_UNIT_IS_INPUT,       //! There was an attempt to do something to an input unit which cannot be done.  This is a more detailed form of ON_NACK_RSN_UNIT_FUNCTION_ERR
    ON_NACK_RSN_UNIT_IS_OUTPUT,      //! There was an attempt to do something to an output unit which cannot be done.  This is a more detailed form of ON_NACK_RSN_UNIT_FUNCTION_ERR

    ON_NACK_RSN_DEVICE_NOT_IN_NETWORK, //! A device is not in the network
    ON_NACK_RSN_DEVICE_IS_THIS_DEVICE, //! Attempting to do something with another device when the other device is yourself
    ON_NACK_RSN_SENDER_AND_DEST_ARE_SAME, //! Attempting to send a message where the sender and the recipient are the same device
    ON_NACK_RSN_PERMISSION_DENIED_FATAL, //! Generic "Permission denied" fatal error.
    ON_NACK_RSN_ABORT,                   //! Abort
    
    // stick any more ONE-NET fatal errors here.
    
    ON_NACK_RSN_FATAL_ERR = 0xBF,    //! Some unspecified fatal error occurred.  Don't try to resend.
    
    
    // 0xC0 through 0xFF are reserved for fatal user-defined errors
    ON_NACK_RSN_MIN_USR_FATAL = 0xC0, //! User-defined fatal NACK code start
    ON_NACK_RSN_MAX_USR_FATAL = 0xFF //! User-defined fatal NACK code start
} on_nack_rsn_t;



// note : below are some named constants.  Since this is C, not C++, enumerated types are simply aliases for
// integers, so we'll use on_ack_handle_t, on_nack_handle_t, and on_ack_nack_handle_t interchangably.
// ON_ACK_STATUS only makes sense for an ACK, so don't use it for an on_nack_handle_t.  The
// compiler could not care less, but consistency makes it easier for us humans to read.

// Note that "handle" does not refer to the computer science term "handle".  Rather, it refers to
// how ONE-NET should interpret and "handle" packet information (either as no payload (ON_ACK/ON_NACK),
// a generic data array (ON_ACK_DATA, ON_NACK_DATA), a route message (ON_ACK_ROUTE, ON_NACK_ROUTE),
// a 32-but integer value (ON_ACK_VALUE / ON_NACK_VALUE) or any of the time handles), a features message,
// (ON_ACK_FEATURES, ON_NACK_FEATURES),  or for ACKs only, a bitwise array of block packets received
// (ON_ACK_BLK_PKTS_RCVD), or a single administration or application message(ON_ACK_ADMIN_MSG, ON_ACK_APP_MSG),
// possibly in response to a query.
/*!
    Specifies what an ACK means and whether there is any data accompanying it
*/
typedef enum
{
    ON_ACK,                //! Normal ACK with no accompanying data
    ON_ACK_FEATURES,       //! Normal ACK accompanied with four bytes of features
    ON_ACK_DATA,           //! The ACK is accompanied by 5 bytes of data.
    ON_ACK_VALUE,          //! The ACK is accompanied by a 32 bit unsigned integers.
    ON_ACK_TIME_MS,        //! The ACK is accompanied by a 32 bit unsigned integer representing generic time in milliseconds
    ON_ACK_TIMEOUT_MS,     //! Same as ON_ACK_TIME_MS, but represents the fact that something has timed out.
    ON_ACK_SLOW_DOWN_TIME_MS, //! Same as ON_ACK_TIME_MS, but represents a request to send the packets slower by
                              //! the time specified.
    ON_ACK_SPEED_UP_TIME_MS, //! Same as ON_ACK_TIME_MS, but represents a request to send the packets faster by
                              //! the time specified.
    ON_ACK_PAUSE_TIME_MS, //! Same as ON_ACK_TIME_MS, but represents a request to pause in milliseconds.
    ON_ACK_RESPONSE_TIME_MS, // Same as ON_ACK_TIME_MS, but represents a request to set the response timeout / fragment delay
                             //! to this exact value.
    ON_ACK_KEY,   //! This ACK or NACK is sent when sending a key fragment or a key (the packet length will determine which it is)
    ON_ACK_BLK_PKTS_RCVD,  //! This is an ACK accompanied by a bitwise boolean array representing which packet indexes have and have not been received.
                           //! Valid only for block transactions.  Always sent as an ACK even if it represents a problem.
    ON_ACK_ROUTE,
    ON_ACK_APP_MSG,         //! The ACK is accompanied by an application message
    ON_ACK_ADMIN_MSG,      //! Sending back an Admin message with an ACK
    ON_ACK_APPLICATION_HANDLE, //! Application-specific handle.  This will be treated by ONE-NET
                                  //! as ON_ACK_DATA when building and parsing packets.  It is provided by ONE-NET
                                  //! but its meaning is to be interpreted by the application code.
    ON_ACK_MAX_HANDLE = 15 //! Application-specific handles are allowable and will be treated by ONE-NET
} on_ack_handle_t;


/*!
    Specifies what a NACK means and whether there is any data accompanying it.  Same as ACK values.
    Just defined here so you can use whichever you like.  These MUST correspond to the on_ack_handle_t values.
*/
#define ON_NACK ON_ACK
#define ON_NACK_FEATURES ON_ACK_FEATURES
#define ON_NACK_DATA ON_ACK_DATA
#define ON_NACK_VALUE ON_ACK_VALUE
#define ON_NACK_TIME_MS ON_ACK_TIME_MS
#define ON_NACK_TIMEOUT_MS ON_ACK_TIMEOUT_MS
#define ON_NACK_SLOW_DOWN_TIME_MS ON_ACK_SLOW_DOWN_TIME_MS
#define ON_NACK_SPEED_UP_TIME_MS ON_ACK_SPEED_UP_TIME_MS
#define ON_NACK_PAUSE_TIME_MS ON_ACK_PAUSE_TIME_MS
#define ON_NACK_RESPONSE_TIME_MS ON_ACK_RESPONSE_TIME_MS
#define ON_NACK_KEY ON_ACK_KEY
#define ON_NACK_APPLICATION_HANDLE ON_ACK_APPLICATION_HANDLE
#define ON_NACK_MAX_HANDLE ON_ACK_MAX_HANDLE

typedef on_ack_handle_t on_ack_nack_handle_t; // it's all ints anyway
typedef on_ack_handle_t on_nack_handle_t; // it's all ints anyway


/*!
    The payload of an ACK or a NACK.
*/
typedef union
{
    UInt8 app_msg[ONA_MAX_SINGLE_PACKET_PAYLOAD_LEN]; //! only valid for
      //! ACKs.  Generally, but not exclusively intended for "fast query"/
      //! "poll" responses.
    UInt8 admin_msg[ONA_MAX_SINGLE_PACKET_PAYLOAD_LEN];
    UInt8 ack_payload[ONA_MAX_SINGLE_PACKET_PAYLOAD_LEN];
    UInt32 ack_time_ms;
    UInt8 nack_payload[ONA_MAX_SINGLE_PACKET_PAYLOAD_LEN - 1];
      // subtract 1 byte for the nack reason
    UInt32 ack_value;
    UInt32 nack_value;
    UInt32 nack_time_ms;
    #ifdef EXTENDED_SINGLE
    one_net_xtea_key_t key;
    #endif
    one_net_xtea_key_fragment_t key_frag;
    on_features_t features;
} ack_nack_payload_t;



// to preserve stack space for function calls, combining nack reason,
// nack handle, and nack payload into single structure.  I wish I didn't
// have to do this and maybe it shouldn't be done, but stack space is at
// a premium.  This requires being careful about which elements of the
// struct to adjust and which not to when the structure is passed.
typedef struct
{
    on_nack_rsn_t nack_reason;
    on_ack_nack_handle_t handle;
    ack_nack_payload_t* payload;
} on_ack_nack_t;



//! @} ONE-NET_ACKNOWLEDGE_typedefs
//                                  TYPEDEFS END
//==============================================================================


//==============================================================================
//                                  CONSTANTS
//! \defgroup ONE-NET_ACKNOWLEDGE_const
//! \ingroup ONE-NET_ACKNOWLEDGE
//! @{


//! @} ONE-NET_ACKNOWLEDGE_const
//                                  CONSTANTS END
//==============================================================================


//==============================================================================
//                      PUBLIC FUNCTION DECLARATIONS
//! \defgroup ONE-NET_ACKNOWLEDGE_pub_func
//! \ingroup ONE-NET_ACKNOWLEDGE
//! @{

BOOL nack_reason_is_fatal(const on_nack_rsn_t nack_reason);

//! @} ONE-NET_ACKNOWLEDGE_pub_func
//                      PUBLIC FUNCTION DECLARATIONS END
//==============================================================================



//! @} ONE-NET_ACKNOWLEDGE

#endif // ONE_NET_ACKNOWLEDGE_H //
