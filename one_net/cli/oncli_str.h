#ifndef ONCLI_STR_H
#define ONCLI_STR_H


//! \defgroup oncli_str ONE-NET Command Line Interface Strings
//! \ingroup oncli
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
    \file oncli_str.h

    \brief Contains string constants associated with the ONE-NET Command Line
      interface.

    These strings are to be used in a case insensitive manner.
*/


#include "config_options.h"
#include "one_net_acknowledge.h"


#ifdef UART


//==============================================================================
//								PUBLIC CONSTANTS
//! \defgroup oncli_str_pub_const
//! \ingroup oncli_str
//! @{
    
    
// Application Format
extern const char * const ONCLI_STARTUP_FMT;

//! Tail end of the startup banner, because the va_args does not seem to be able to 
//! handle more than two arguments.
extern const char * const ONCLI_STARTUP_REV_FMT;



// Mode strings (TODO - move these into application code?)
#ifdef AUTO_MODE
extern const char * const ONCLI_AUTO_MODE_STR;
#endif
#ifdef SNIFFER_MODE
extern const char * const ONCLI_SNIFFER_STR;
#endif
extern const char * const ONCLI_SERIAL_MODE_STR;



// Device strings
extern const char * const ONCLI_MASTER_STR;
extern const char * const ONCLI_CLIENT_STR;



// Region Strings -- add as more regions are added (so far only US and Europe)
#ifdef US_CHANNELS
extern const char * const ONCLI_US_STR;
#endif
#ifdef EUROPE_CHANNELS
//! European argument string
extern const char * const ONCLI_EUR_STR;
#endif



// Argument strings
extern const char * const ONCLI_ON_STR;
extern const char * const ONCLI_OFF_STR;
extern const char * const ONCLI_LOW_STR;
extern const char * const ONCLI_HIGH_STR;



// Command Strings
#ifdef ENABLE_ECHO_COMMAND
    extern const char * const ONCLI_ECHO_CMD_STR;
#endif

#ifdef ENABLE_LIST_COMMAND
extern const char * const ONCLI_LIST_CMD_STR;
#ifdef PEER
extern const char * const ONCLI_LIST_PEER_TABLE_HEADING;
extern const char * const ONCLI_LIST_PEER_FMT;
extern const char * const ONCLI_LIST_NO_PEERS;
#endif
#endif

#if defined(SNIFFER_MODE) && defined(ENABLE_SNIFF_COMMAND)
extern const char * const ONCLI_SNIFF_CMD_STR;
#endif

#ifdef ENABLE_SINGLE_COMMAND
extern const char * const ONCLI_SINGLE_TXT_CMD_STR;
extern const char * const ONCLI_SINGLE_CMD_STR;
extern const char * const ONCLI_STATUS_PIN_CMD_STR;
extern const char * const ONCLI_QUERY_PIN_CMD_STR;
extern const char * const ONCLI_FAST_QUERY_PIN_CMD_STR;
extern const char * const ONCLI_SET_PIN_CMD_STR;
#endif

#ifdef ENABLE_ERASE_COMMAND
extern const char * const ONCLI_ERASE_CMD_STR;
#endif

#ifdef ENABLE_SAVE_COMMAND
extern const char * const ONCLI_SAVE_CMD_STR;
#endif

#ifdef ENABLE_SET_DR_CHANNEL_COMMAND
extern const char * const ONCLI_SET_DR_CHANNEL_CMD_STR;
#endif

#ifdef ENABLE_USER_PIN_COMMAND
extern const char * const ONCLI_USER_PIN_CMD_STR;
#endif
extern const char * const ONCLI_INPUT_STR;
extern const char * const ONCLI_OUTPUT_STR;
extern const char * const ONCLI_DISABLE_STR;


// Master only commands
#ifdef ENABLE_INVITE_COMMAND
extern const char * const ONCLI_INVITE_CMD_STR;
#endif
#ifdef ENABLE_CANCEL_INVITE_COMMAND
extern const char * const ONCLI_CANCEL_INVITE_CMD_STR;
#endif
#ifdef ENABLE_ASSIGN_PEER_COMMAND
extern const char * const ONCLI_ASSIGN_PEER_CMD_STR;
#endif
#ifdef ENABLE_UNASSIGN_PEER_COMMAND
extern const char * const ONCLI_UNASSIGN_PEER_CMD_STR;
#endif
#ifdef ENABLE_SET_FLAGS_COMMAND
extern const char * const ONCLI_SET_FLAGS_CMD_STR;
#endif
#ifdef ENABLE_CHANGE_KEEP_ALIVE_COMMAND
extern const char * const ONCLI_CHANGE_KEEP_ALIVE_CMD_STR;
#endif
#ifdef ENABLE_CHANGE_FRAGMENT_DELAY_COMMAND
extern const char * const ONCLI_CHANGE_FRAGMENT_DELAY_CMD_STR;
#endif
#ifdef ENABLE_CHANGE_KEY_COMMAND
extern const char * const ONCLI_CHANGE_KEY_CMD_STR;
#endif
#ifdef ENABLE_REMOVE_DEVICE_COMMAND
extern const char * const ONCLI_RM_DEV_CMD_STR;
#endif
#ifdef ENABLE_CHANNEL_COMMAND
extern const char * const ONCLI_CHANNEL_CMD_STR;
#endif
#ifdef ENABLE_JOIN_COMMAND
extern const char * const ONCLI_JOIN_CMD_STR;
#endif
#ifdef ENABLE_SETNI_COMMAND
extern const char * const ONCLI_SETNI_CMD_STR;
#endif
#ifdef RANGE_TESTING
extern const char* const ONCLI_RANGE_TEST_CMD_STR;
#endif
#ifdef PID_BLOCK
extern const char* const ONCLI_PID_BLOCK_CMD_STR;
#endif
#if defined(RANGE_TESTING) || defined(PID_BLOCK)
extern const char* const ADD_STR;
extern const char* const REMOVE_STR;
extern const char* const CLEAR_STR;
extern const char* const DISPLAY_STR;
#endif

#ifdef BLOCK_MESSAGES_ENABLED
extern const char* const ONCLI_BLOCK_CMD_STR;
#endif

#ifdef STREAM_MESSAGES_ENABLED
extern const char* const ONCLI_STREAM_CMD_STR;
#endif

// Response Formats
extern const char * const ONCLI_GET_CHANNEL_RESPONSE_FMT;


#ifdef ONE_NET_MASTER
extern const char * const ONCLI_CLIENT_MISS_CHECK_IN_FMT;
extern const char * const ONCLI_DEVICE_ADD_FMT;
extern const char * const ONCLI_DEVICE_NOT_ADDED_FMT;
#endif
#ifdef ONE_NET_CLIENT
extern const char * const ONCLI_JOINED_FMT;
#endif



// Reponse Strings
extern const char * const ONCLI_CMD_SUCCESS_STR;
extern const char * const ONCLI_SUCCEEDED_STR;
extern const char * const ONCLI_FAILED_STR;
extern const char * const ONCLI_CLR_INPUT_STR;
extern const char * const ONCLI_CHANNEL_NOT_SELECTED_STR;
extern const char * const ONCLI_CHANNEL_INVALID_STR;
extern const char * const ONCLI_SINGLE_RESULT_FMT;



// Text messages
//! Format output when a text transaction has been received
extern const char * const ONCLI_RX_TXT_FMT;



// Message types
//! string for a single transaction
extern const char * const ONCLI_SINGLE_TXN_STR;

#ifdef BLOCK_MESSAGES_ENABLED
//! string for a block transaction
extern const char * const ONCLI_BLOCK_TXN_STR;
#endif

#ifdef STREAM_MESSAGES_ENABLED
//! string for a stream transaction
extern const char * const ONCLI_STREAM_TXN_STR;
#endif


extern const char* const ONCLI_DEVICE_STATE_FMT;
extern const char* const ONCLI_CHANGE_PIN_STATE_FMT;


// Error Formats
extern const char * const ONCLI_INVALID_CMD_FMT;
extern const char * const ONCLI_CMD_FAIL_FMT;
extern const char * const ONCLI_INVALID_CMD_FOR_DEVICE_FMT;
extern const char * const ONCLI_INTERNAL_ERR_FMT;
extern const char * const ONCLI_GENERAL_ERROR_FMT;
extern const char * const ONCLI_RX_INVALID_CH_FMT;
extern const char * const ONCLI_OUTPUT_STR_TOO_SHORT_FMT;


#ifdef BLOCK_MESSAGES_ENABLED
extern const char* const ONCLI_FRAG_DELAY_FMT;
#endif


// Error Strings
extern const char * const ONCLI_IN_PROGRESS_STR;
extern const char * const ONCLI_RSRC_UNAVAILABLE_STR;
extern const char * const ONCLI_UNSUPPORTED_STR;
extern const char * const ONCLI_INVALID_FORMAT_STR;
extern const char * const ONCLI_SNGH_STR;
extern const char * const ONCLI_ONS_NOT_INIT_ERR_STR;
extern const char * const ONCLI_INTERNAL_ERR_STR;
extern const char * const ONCLI_INVALID_DST_STR;
extern const char * const ONCLI_NEED_TO_JOIN_STR;
extern const char * const ONCLI_BAD_KEY_FRAGMENT_STR;
extern const char * const ONCLI_INVALID_CMD_LEN_STR;
extern const char * const ONCLI_LOAD_FAIL_STR;
extern const char * const ONCLI_FATAL_ERR_1_STR;
enum
{
    ONCLI_FATAL_ERR_1_LEN = 15
};



#ifdef ONE_NET_CLIENT
extern const char * const ONCLI_DISPLAY_INVITE_STR;
#endif


// Data Rate Strings
extern const char* const DATA_RATE_STR[];


extern const char* const CAPABLE_STR;
extern const char* const NOT_CAPABLE_STR;
extern const char* const TRUE_STR;
extern const char* const FALSE_STR;


extern const char* const ONCLI_MSG_STATUS_STR[];


#if DEBUG_VERBOSE_LEVEL > 3

// strings and formats dealing with ACKs and NACKS
extern const char* const ONCLI_ACK_STR;
extern const char* const ONCLI_NACK_STR;
extern const char* const ACK_NACK_HANDLE_STR_ARRAY[];
extern const char* const ACK_NACK_DISPLAY_FMT;

#endif


#ifdef ONE_NET_MASTER
// formats for the results of admin messages from the master.
extern const char * const ONCLI_UPDATE_RESULT_FMT;
extern const char * const ONCLI_UPDATE_RESULT_WITH_OUT_DID_FMT;
extern const char * const ONCLI_UNKNOWN_UPDATE_RESULT_FMT;

// master admin update strings
extern const char* const ONCLI_M_UPDATE_RESULT_DATA_RATE_STR;
extern const char* const ONCLI_M_UPDATE_RESULT_KEY_STR;
#ifdef PEER
extern const char* const ONCLI_M_UPDATE_RESULT_ASSIGN_PEER_STR;
extern const char* const ONCLI_M_UPDATE_RESULT_UNASSIGN_PEER_STR;
#endif
#ifdef BLOCK_MESSAGES_ENABLED
extern const char* const ONCLI_M_UPDATE_RESULT_FRAG_STR;
#endif
extern const char* const ONCLI_M_UPDATE_RESULT_KEEP_ALIVE_STR;
extern const char* const ONCLI_M_UPDATE_RESULT_SETTINGS_STR;
extern const char* const ONCLI_M_UPDATE_RESULT_RM_DEV_STR;
extern const char* const ONCLI_M_UPDATE_RESULT_ADD_DEV_STR;
#endif


#ifdef DEBUGGING_TOOLS
extern const char* const ONCLI_CSDF_CMD_STR;
extern const char* const ONCLI_MEMDUMP_CMD_STR;
extern const char* const ONCLI_MEMLOAD_CMD_STR;
extern const char* const ONCLI_MEMSET_CMD_STR;
extern const char* const ONCLI_MEMORY_CMD_STR;
extern const char* const ONCLI_PAUSE_CMD_STR;
extern const char* const ONCLI_PROCEED_CMD_STR;
extern const char* const ONCLI_RATCHET_CMD_STR;
extern const char* const ONCLI_INTERVAL_CMD_STR;
#endif


#ifdef ENABLE_BAUD_COMMAND
extern const char* const ONCLI_BAUD_CMD_STR;
#endif

#ifdef ENABLE_VERBOSE_LEVEL_COMMAND
extern const char* const ONCLI_VERBOSE_LEVEL_CMD_STR;
#endif


#ifdef ENABLE_ROUTE_COMMAND
extern const char* const ONCLI_ROUTE_CMD_STR;
#endif

extern const char* const EMPTY_STRING;


//! @} oncli_str_pub_const
//								PUBLIC CONSTANTS END
//==============================================================================

//==============================================================================
//								TYPEDEFS
//! \defgroup oncli_str_typedefs
//! \ingroup oncli_str
//! @{


// this is used by the oncli_format_channel function and must be long
// enough to accomodate the longest possible string format for a channel,
// including the NULL terminator.  Currently that is "EUR 3" or "US 25",
// which is 6 characters including the NULL terminator.  This value should
// be adjusted if the channel format changes, loocales are added, or if
// this maximum size should change. I must also be long enough to accomodate
// the ONCLI_CHANNEL_INVALID_STR, which is "invalid channel" string, which is
// 16 including the NULL terminator
enum
{
    MAX_CHANNEL_STRING_FORMAT_LENGTH = 16 // includes NULL terminator
};



//! @} oncli_str_typedefs
//								TYPEDEFS END
//==============================================================================

//==============================================================================
//                              PUBLIC VARIABLES
//! \defgroup oncli_str_pub_var
//! \ingroup oncli_str
//! @{

//! @} oncli_str_pub_var
//                              PUBLIC VARIABLES END
//==============================================================================

//==============================================================================
//						PUBLIC FUNCTION DECLARATIONS
//! \defgroup oncli_str_pub_func
//! \ingroup oncli_str
//! @{

#if DEBUG_VERBOSE_LEVEL > 3
const char* get_nack_reason_str(on_nack_rsn_t nack_reason);
#endif

//! @} oncli_str_pub_func
//						PUBLIC FUNCTION DECLARATIONS END
//==============================================================================

//! @} oncli_str


#endif // #ifdef UART //

#endif // #ifdef ONCLI_STR_H //
