//! \addtogroup oncli_hdlr
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
    \file oncli_hdlr.c
    \brief Contains the implementation for handling of ONE-NET Command Line
      Interface commands.

	2012 - By Arie Rechavel at D&H Global Enterprise, LLC., based on the Renesas Evaluation Board Project
*/

#include "config_options.h"

#ifdef UART

// TODO -- this is a bit messy.  Find a better #define test.
#if defined(_R8C_TINY) && !defined(QUAD_OUTPUT)
    #pragma section program program_high_rom
#endif // if _R8C_TINY and not a 16K chip //


#include "oncli_hdlr.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "oncli.h"
#include "oncli_str.h"
#include "str.h"
#include "one_net.h"
#include "oncli_port.h"
#include "one_net_data_rate.h"
#include "tal.h"
#include "one_net_encode.h"
#include "one_net_application.h"
#ifdef PEER
#include "one_net_peer.h"
#endif
#include "one_net_port_specific.h"
#ifdef ONE_NET_CLIENT
#include "one_net_client.h"
#include "one_net_client_port_specific.h"
#endif
#ifdef ONE_NET_MASTER
#include "one_net_master_port_specific.h"
#include "one_net_master_port_const.h"
#include "one_net_master.h"
#endif
#ifdef NON_VOLATILE_MEMORY
#include "dfi.h"
#endif

#ifdef DEBUGGING_TOOLS
#include "one_net_timer.h"
#ifdef ONE_NET_CLIENT
#include "one_net_client_port_const.h"
#endif
#endif

#include "tick.h"

#ifdef ENABLE_SINGLE_COMMAND
#include "io_port_mapping.h"
#endif

#ifdef ENABLE_BAUD_COMMAND
#include "uart.h"
#include "uart_hal.h"
#include "cb.h"
extern cb_rec_t uart_tx_cb;
#endif




//==============================================================================
//								CONSTANTS
//! \defgroup oncli_hdlr_const
//! \ingroup oncli_hdlr
//! @{



enum
{
    //! The number of ASCII hex characters per raw did
    ONCLI_ASCII_RAW_DID_SIZE = 3,
};


//! Parameter delimiter
static const char ONCLI_PARAM_DELIMITER = ':';


//! @} oncli_hdlr_const
//								CONSTANTS END
//==============================================================================

//==============================================================================
//								TYPEDEFS
//! \defgroup oncli_hdlr_typedefs
//! \ingroup oncli_hdlr
//! @{

//! @} oncli_hdlr_typedefs
//								TYPEDEFS END
//==============================================================================

//==============================================================================
//							PRIVATE VARIABLES
//! \defgroup oncli_hdlr_pri_var
//! \ingroup oncli_hdlr
//! @{


#ifdef DEBUGGING_TOOLS
// Debug memory
typedef enum
{
    DEBUG_MEMORY_ON_STATE,
    DEBUG_MEMORY_TIMER,
    #ifdef ONE_NET_CLIENT
    DEBUG_MEMORY_SEND_LIST,
    DEBUG_MEMORY_MASTER,
    #endif
    #ifdef PEER
    DEBUG_MEMORY_PEER,
    #endif
    DEBUG_MEMORY_BASE_PARAM,
    #ifdef ONE_NET_MASTER
    DEBUG_MEMORY_MASTER_PARAM,
    DEBUG_MEMORY_CLIENT_LIST,
    #endif
    DEBUG_MEMORY_INVITE_TXN,
    DEBUG_MEMORY_RESPONSE_TXN,
    DEBUG_MEMORY_SINGLE_TXN,
    #ifdef BLOCK_MESSAGES_ENABLED
    DEBUG_MEMORY_BS_TXN,
    DEBUG_MEMORY_BS_MSG,
    #endif
    DEBUG_MEMORY_COUNT
} debug_memory_t;


static const char* debug_memory_str[DEBUG_MEMORY_COUNT] =
{
    "on_state",
    "timer",
    #ifdef ONE_NET_CLIENT
    "send_list",
    "master",
    #endif
    #ifdef PEER
    "peer",
    #endif
    "base_param",
    #ifdef ONE_NET_MASTER
    "master_param",
    "client_list",
    #endif
    "invite_txn",
    "response_txn",
    "single_txn",
    #ifdef BLOCK_MESSAGES_ENABLED
    "bs_txn",
    "bs_msg"
    #endif
};


//! The memory currently being pointed to.
static UInt8* memory_ptr = NULL;

//! The relevant length of the memory currently being pointed to.
int memory_len = 0;
#endif






//! @} oncli_hdlr_pri_var
//							PRIVATE VARIABLES END
//==============================================================================



//==============================================================================
//							PRIVATE VARIABLES
//! \defgroup oncli_hdlr_pub_var
//! \ingroup oncli_hdlr
//! @{



//! @} oncli_hdlr_pub_var
//							PRIVATE VARIABLES END
//==============================================================================




//==============================================================================
//						PRIVATE FUNCTION DECLARATIONS
//! \defgroup oncli_hdlr_pri_func
//! \ingroup oncli_hdlr
//! @{


// Command handlers.
#ifdef ENABLE_ECHO_COMMAND
	static oncli_status_t echo_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_LIST_COMMAND
	static oncli_status_t list_cmd_hdlr(void);
#endif

#ifdef ENABLE_ERASE_COMMAND
	static oncli_status_t erase_cmd_hdlr(void);
#endif

#ifdef ENABLE_SAVE_COMMAND
	static oncli_status_t save_cmd_hdlr(void);
#endif

#if defined(SNIFFER_MODE) && defined(ENABLE_SNIFF_COMMAND)
oncli_status_t sniff_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_SINGLE_COMMAND
static oncli_status_t single_cmd_hdlr(const char * const ASCII_PARAM_LIST);
static oncli_status_t single_txt_cmd_hdlr(const char * const ASCII_PARAM_LIST);
static oncli_status_t status_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST);
static oncli_status_t query_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST);
static oncli_status_t fast_query_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST);
static oncli_status_t set_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST);
static oncli_status_t parse_and_send_pin_msg(
  const char * const ASCII_PARAM_LIST, UInt8 msg_class);
#endif

#ifdef ENABLE_SET_DR_CHANNEL_COMMAND
oncli_status_t set_dr_channel_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_USER_PIN_COMMAND
	static oncli_status_t user_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_CHANGE_FRAGMENT_DELAY_COMMAND
	static oncli_status_t change_frag_dly_cmd_hdlr(
	  const char * const ASCII_PARAM_LIST);
#endif



// MASTER only command handlers
#ifdef ENABLE_INVITE_COMMAND
	static oncli_status_t invite_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif
#ifdef ENABLE_CANCEL_INVITE_COMMAND
	static oncli_status_t cancel_invite_cmd_hdlr(void);
#endif
#ifdef ENABLE_REMOVE_DEVICE_COMMAND
	static oncli_status_t rm_dev_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#if defined(ENABLE_ASSIGN_PEER_COMMAND) || defined(ENABLE_UNASSIGN_PEER_COMMAND)
static oncli_status_t oncli_change_peer_list(BOOL ASSIGN,
  const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_ASSIGN_PEER_COMMAND
	static oncli_status_t assign_peer_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif
#ifdef ENABLE_UNASSIGN_PEER_COMMAND
	static oncli_status_t unassign_peer_cmd_hdlr(
	  const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_SET_FLAGS_COMMAND
	static oncli_status_t set_flags_cmd_hdlr(
	  const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_CHANGE_KEEP_ALIVE_COMMAND
	static oncli_status_t change_keep_alive_cmd_hdlr(
	  const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_CHANGE_KEY_COMMAND
static oncli_status_t change_single_block_key_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_CHANNEL_COMMAND
static oncli_status_t channel_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_JOIN_COMMAND
oncli_status_t join_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_SETNI_COMMAND
static oncli_status_t setni_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef PID_BLOCK
static oncli_status_t pid_block_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef RANGE_TESTING
static oncli_status_t range_test_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef BLOCK_MESSAGES_ENABLED
static oncli_status_t block_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

#ifdef STREAM_MESSAGES_ENABLED
static oncli_status_t stream_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif

// Parsing functions.
static UInt16 ascii_hex_to_byte_stream(const char * STR, UInt8 * byte_stream,
  const UInt16 NUM_ASCII_CHAR);
#ifdef ENABLE_SINGLE_COMMAND

#ifdef PEER
static const char * parse_ascii_tx_param(const char * PARAM_PTR,
  UInt8 * const src_unit, UInt8 * const dst_unit,
  on_encoded_did_t * const enc_dst, BOOL* send_to_peer_list);
#else
static const char * parse_ascii_tx_param(const char * PARAM_PTR,
  UInt8 * const src_unit, UInt8 * const dst_unit,
  on_encoded_did_t * const enc_dst);
#endif

static const char * parse_ascii_tx_data(const char * ASCII, UInt8 * data,
  UInt16 * data_len);
static const char * parse_ascii_tx_text_data(const char * ASCII, UInt8 * data,
  UInt16 * data_len);
#endif
static oncli_status_t oncli_parse_channel(const char * ASCII,
  UInt8 * const channel);

#ifdef ENABLE_INVITE_COMMAND
static oncli_status_t parse_invite_key(const char * ASCII,
  char** end_ptr, one_net_xtea_key_t * const key);
#endif


// debugging tools
#ifdef DEBUGGING_TOOLS
static int get_memory_loc(UInt8** mem_ptr, debug_memory_t memory_type,
  int index, int offset);
static int parse_memory_str(UInt8** mem_ptr,
  const char * const ASCII_PARAM_LIST);
static oncli_status_t csdf_cmd_hdlr(const char * const ASCII_PARAM_LIST);
static oncli_status_t memory_cmd_hdlr(
  const char * const ASCII_PARAM_LIST);
static oncli_status_t memdump_cmd_hdlr(void);
static oncli_status_t memset_cmd_hdlr(
  const char * const ASCII_PARAM_LIST);
static oncli_status_t memload_cmd_hdlr(
  const char * const ASCII_PARAM_LIST);
static oncli_status_t pause_cmd_hdlr(void);
static oncli_status_t proceed_cmd_hdlr(void);
static oncli_status_t ratchet_cmd_hdlr(void);
static oncli_status_t interval_cmd_hdlr(
  const char * const ASCII_PARAM_LIST);
#endif


#ifdef ENABLE_BAUD_COMMAND
static oncli_status_t baud_cmd_hdlr(const char* const ASCII_PARAM_LIST);
#endif

#ifdef ENABLE_VERBOSE_LEVEL_COMMAND
static oncli_status_t verbose_level_cmd_hdlr(
  const char* const ASCII_PARAM_LIST);
#endif


#ifdef ENABLE_ROUTE_COMMAND
static oncli_status_t route_cmd_hdlr(const char * const ASCII_PARAM_LIST);
#endif





//! @} oncli_hdlr_pri_func
//						PRIVATE FUNCTION DECLARATIONS END
//==============================================================================

//==============================================================================
//						PUBLIC FUNCTION IMPLEMENTATION
//! \defgroup oncli_hdlr_pub_func
//! \ingroup oncli_hdlr
//! @{

/*!
    \brief Parses a command that has been completely read in.

    Parses the command and will call the appropriate handler to parse the
    parameters and execute the command.  This function will also return why
    a command may have failed. It is within this function that we decide
    whether or not to continue reading data looking for paramters to
    functions.

    12/12/08: With the revised implementation of the SID command, we want
    the behavior of the command to vary depending on whether or not
    parameters are supplied. So, we must set the commnand to expect
    parameters. This means that at least a semicolon must be typed
    after the command to ensure that the command handler is executed.
    It is then up to the command handler to figure out if there are any
    parameters after the semicolon and take the appropriate action.

    \param[in] CMD The command to be parsed.
    \param[out] CMD_STR The command string of the command that was passed in.
    \param[out] read_param_state The next state for reading in the command
      parameters.  This is only set if a state to read in parameters is
      required.
    \param[out] cmd_hdlr The handler that should be called when the parameters
      are read in.  If this is NULL, and ONCLI_SUCCESS is returned, that means
      the command had no parameters are is complete.

    \return ONCLI_SUCCESS The the command was successfully parsed
            ONCLI_BAD_PARAM If any of the parameters passed in were invalid.
            ONCLI_PARSE_ERR If the command was not parsed correctly
*/
oncli_status_t oncli_parse_cmd(const char * const CMD, const char ** CMD_STR,
  UInt8 * const next_state, oncli_cmd_hdlr_t * const cmd_hdlr)
{
    if(!CMD || !CMD_STR || !next_state || !cmd_hdlr)
    {
        return ONCLI_BAD_PARAM;
    } // if any of the parameters are invalid //

    *cmd_hdlr = 0;

	#ifdef ENABLE_ECHO_COMMAND
    if(!strncmp(ONCLI_ECHO_CMD_STR, CMD, strlen(ONCLI_ECHO_CMD_STR)))
    {
        *CMD_STR = ONCLI_ECHO_CMD_STR;

        if(CMD[strlen(ONCLI_ECHO_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end of the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &echo_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the echo command was received //
	#endif

	#ifdef ENABLE_LIST_COMMAND
    if(!strncmp(ONCLI_LIST_CMD_STR, CMD, strlen(ONCLI_LIST_CMD_STR)))
    {
        *CMD_STR = ONCLI_LIST_CMD_STR;

        if(CMD[strlen(ONCLI_LIST_CMD_STR)] != '\n')
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        return list_cmd_hdlr();
    } // else if the list command was received //
	#endif

	#ifdef ENABLE_ERASE_COMMAND
    else if(!strncmp(ONCLI_ERASE_CMD_STR, CMD, strlen(ONCLI_ERASE_CMD_STR)))
    {
        *CMD_STR = ONCLI_ERASE_CMD_STR;

        if(CMD[strlen(ONCLI_ERASE_CMD_STR)] != '\n')
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        return erase_cmd_hdlr();
    } // else if the erase command was received //
	#endif

	#ifdef ENABLE_SAVE_COMMAND
    else if(!strncmp(ONCLI_SAVE_CMD_STR, CMD, strlen(ONCLI_SAVE_CMD_STR)))
    {
        *CMD_STR = ONCLI_SAVE_CMD_STR;

        if(CMD[strlen(ONCLI_SAVE_CMD_STR)] != '\n')
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        return save_cmd_hdlr();
    } // else if the save command was received //
	#endif

	#if defined(SNIFFER_MODE) && defined(ENABLE_SNIFF_COMMAND)
    else if(!strncmp(ONCLI_SNIFF_CMD_STR, CMD, strlen(ONCLI_SNIFF_CMD_STR)))
    {
        *CMD_STR = ONCLI_SNIFF_CMD_STR;

        if(CMD[strlen(ONCLI_SNIFF_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &sniff_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the sniff command was received //
	#endif

	#ifdef ENABLE_SINGLE_COMMAND
    else if(!strncmp(ONCLI_SINGLE_TXT_CMD_STR, CMD, strlen(ONCLI_SINGLE_TXT_CMD_STR)))
    {
        *CMD_STR = ONCLI_SINGLE_TXT_CMD_STR;

        if(CMD[strlen(ONCLI_SINGLE_TXT_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &single_txt_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the send single text command was received //

    else if(!strncmp(ONCLI_SINGLE_CMD_STR, CMD, strlen(ONCLI_SINGLE_CMD_STR)))
    {
        *CMD_STR = ONCLI_SINGLE_CMD_STR;

        if(CMD[strlen(ONCLI_SINGLE_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &single_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the send single command was received //

    else if(!strncmp(ONCLI_STATUS_PIN_CMD_STR, CMD,
      strlen(ONCLI_STATUS_PIN_CMD_STR)))
    {
        *CMD_STR = ONCLI_STATUS_PIN_CMD_STR;

        if(CMD[strlen(ONCLI_STATUS_PIN_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &status_pin_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the status pin command was received //

    else if(!strncmp(ONCLI_QUERY_PIN_CMD_STR, CMD,
      strlen(ONCLI_QUERY_PIN_CMD_STR)))
    {
        *CMD_STR = ONCLI_QUERY_PIN_CMD_STR;

        if(CMD[strlen(ONCLI_QUERY_PIN_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &query_pin_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the query command was received //

    else if(!strncmp(ONCLI_FAST_QUERY_PIN_CMD_STR, CMD,
      strlen(ONCLI_FAST_QUERY_PIN_CMD_STR)))
    {
        *CMD_STR = ONCLI_FAST_QUERY_PIN_CMD_STR;

        if(CMD[strlen(ONCLI_FAST_QUERY_PIN_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &fast_query_pin_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the fast query command was received //

    else if(!strncmp(ONCLI_SET_PIN_CMD_STR, CMD, strlen(ONCLI_SET_PIN_CMD_STR)))
    {
        *CMD_STR = ONCLI_SET_PIN_CMD_STR;

        if(CMD[strlen(ONCLI_SET_PIN_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &set_pin_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the set pin command was received //
	#endif // ENABLE_SINGLE_COMMAND //

	#ifdef ENABLE_SET_DR_CHANNEL_COMMAND
    else if(!strncmp(ONCLI_SET_DR_CHANNEL_CMD_STR, CMD,
      strlen(ONCLI_SET_DR_CHANNEL_CMD_STR)))
    {
        *CMD_STR = ONCLI_SET_DR_CHANNEL_CMD_STR;

        if(CMD[strlen(ONCLI_SET_DR_CHANNEL_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end of the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &set_dr_channel_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the set data rate/channel command was received //
	#endif

    #ifdef ENABLE_BAUD_COMMAND
    else if(!strncmp(ONCLI_BAUD_CMD_STR, CMD,
      strlen(ONCLI_BAUD_CMD_STR)))
    {
        *CMD_STR = ONCLI_BAUD_CMD_STR;

        if(CMD[strlen(ONCLI_BAUD_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end of the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &baud_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the "baud" command was received //
    #endif

    #ifdef ENABLE_VERBOSE_LEVEL_COMMAND
    else if(!strncmp(ONCLI_VERBOSE_LEVEL_CMD_STR, CMD,
      strlen(ONCLI_VERBOSE_LEVEL_CMD_STR)))
    {
        *CMD_STR = ONCLI_VERBOSE_LEVEL_CMD_STR;

        if(CMD[strlen(ONCLI_VERBOSE_LEVEL_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end of the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &verbose_level_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the "verbose level" command was received //
    #endif

	#ifdef ENABLE_USER_PIN_COMMAND
    else if(!strncmp(ONCLI_USER_PIN_CMD_STR, CMD,
      strlen(ONCLI_USER_PIN_CMD_STR)))
    {
        *CMD_STR = ONCLI_USER_PIN_CMD_STR;

        if(CMD[strlen(ONCLI_USER_PIN_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end of the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &user_pin_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the user pin command was received //
	#endif


	#ifdef ENABLE_INVITE_COMMAND
	else if(!strncmp(ONCLI_INVITE_CMD_STR, CMD, strlen(ONCLI_INVITE_CMD_STR)))
	//else if(!strncmp(ONCLI_INVITE_CMD_STR, CMD, strlen(ONCLI_INVITE_CMD_STR))))
    {
        *CMD_STR = ONCLI_INVITE_CMD_STR;

        if(CMD[strlen(ONCLI_INVITE_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &invite_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the invite command was received //
	#endif

	#ifdef ENABLE_CANCEL_INVITE_COMMAND
    else if(!strncmp(ONCLI_CANCEL_INVITE_CMD_STR, CMD,
      strlen(ONCLI_CANCEL_INVITE_CMD_STR)))
    {
        *CMD_STR = ONCLI_CANCEL_INVITE_CMD_STR;

        if(CMD[strlen(ONCLI_CANCEL_INVITE_CMD_STR)] != '\n')
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        return cancel_invite_cmd_hdlr();
    } // else if the cancel invite command was received //
	#endif

	#ifdef ENABLE_CHANGE_FRAGMENT_DELAY_COMMAND
    else if(!strncmp(ONCLI_CHANGE_FRAGMENT_DELAY_CMD_STR, CMD,
      strlen(ONCLI_CHANGE_FRAGMENT_DELAY_CMD_STR)))
    {
        *CMD_STR = ONCLI_CHANGE_FRAGMENT_DELAY_CMD_STR;

        if(CMD[strlen(ONCLI_CHANGE_FRAGMENT_DELAY_CMD_STR)]
          != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &change_frag_dly_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the change fragment delay command was received //
	#endif

	#ifdef ENABLE_REMOVE_DEVICE_COMMAND
    else if(!strncmp(ONCLI_RM_DEV_CMD_STR, CMD, strlen(ONCLI_RM_DEV_CMD_STR)))
    {
        *CMD_STR = ONCLI_RM_DEV_CMD_STR;

        if(CMD[strlen(ONCLI_RM_DEV_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end of the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &rm_dev_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the remove device command was received //
	#endif

	#ifdef ENABLE_ASSIGN_PEER_COMMAND
    else if(!strncmp(ONCLI_ASSIGN_PEER_CMD_STR, CMD,
      strlen(ONCLI_ASSIGN_PEER_CMD_STR)))
    {
        *CMD_STR = ONCLI_ASSIGN_PEER_CMD_STR;

        if(CMD[strlen(ONCLI_ASSIGN_PEER_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &assign_peer_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the assign peer command was received //
	#endif

	#ifdef ENABLE_UNASSIGN_PEER_COMMAND
    else if(!strncmp(ONCLI_UNASSIGN_PEER_CMD_STR, CMD,
      strlen(ONCLI_UNASSIGN_PEER_CMD_STR)))
    {
        *CMD_STR = ONCLI_UNASSIGN_PEER_CMD_STR;

        if(CMD[strlen(ONCLI_UNASSIGN_PEER_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &unassign_peer_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the unassign peer command was received //
	#endif

	#ifdef ENABLE_SET_FLAGS_COMMAND
    else if(!strncmp(ONCLI_SET_FLAGS_CMD_STR, CMD,
      strlen(ONCLI_SET_FLAGS_CMD_STR)))
    {
        *CMD_STR = ONCLI_SET_FLAGS_CMD_STR;

        if(CMD[strlen(ONCLI_SET_FLAGS_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &set_flags_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the set flags command was received //
	#endif

	#ifdef ENABLE_CHANGE_KEEP_ALIVE_COMMAND
    else if(!strncmp(ONCLI_CHANGE_KEEP_ALIVE_CMD_STR, CMD,
      strlen(ONCLI_CHANGE_KEEP_ALIVE_CMD_STR)))
    {
        *CMD_STR = ONCLI_CHANGE_KEEP_ALIVE_CMD_STR;

        if(CMD[strlen(ONCLI_CHANGE_KEEP_ALIVE_CMD_STR)]
          != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &change_keep_alive_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the change keep alive command was received //
	#endif

	#ifdef ENABLE_CHANGE_KEY_COMMAND
    else if(!strncmp(ONCLI_CHANGE_KEY_CMD_STR, CMD,
      strlen(ONCLI_CHANGE_KEY_CMD_STR)))
    {
        *CMD_STR = ONCLI_CHANGE_KEY_CMD_STR;

        if(CMD[strlen(ONCLI_CHANGE_KEY_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end of the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &change_single_block_key_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the change key command was received //
	#endif

    #ifdef BLOCK_MESSAGES_ENABLED
    if(!strncmp(ONCLI_BLOCK_CMD_STR, CMD, strlen(ONCLI_BLOCK_CMD_STR)))
    {
        *CMD_STR = ONCLI_BLOCK_CMD_STR;

        if(CMD[strlen(ONCLI_BLOCK_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &block_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the "block" command was received //
    #endif

    #ifdef STREAM_MESSAGES_ENABLED
    if(!strncmp(ONCLI_STREAM_CMD_STR, CMD, strlen(ONCLI_STREAM_CMD_STR)))
    {
        *CMD_STR = ONCLI_STREAM_CMD_STR;

        if(CMD[strlen(ONCLI_STREAM_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &stream_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the "stream" command was received //
    #endif

	#ifdef ENABLE_CHANNEL_COMMAND
    if(!strncmp(ONCLI_CHANNEL_CMD_STR, CMD,
      strlen(ONCLI_CHANNEL_CMD_STR)))
    {
        *CMD_STR = ONCLI_CHANNEL_CMD_STR;

        if(CMD[strlen(ONCLI_CHANNEL_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &channel_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the channel command was received //
	#endif

	#ifdef ENABLE_JOIN_COMMAND
    if(!strncmp(ONCLI_JOIN_CMD_STR, CMD, strlen(ONCLI_JOIN_CMD_STR)))
    {
        *CMD_STR = ONCLI_JOIN_CMD_STR;

        #ifndef ENHANCED_INVITE
        if(CMD[strlen(ONCLI_JOIN_CMD_STR)] != '\n')
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

		return (one_net_client_reset_client(one_net_client_get_invite_key()) ==
          ONS_SUCCESS) ? ONCLI_SUCCESS : ONCLI_CMD_FAIL;
        #else
        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &join_cmd_hdlr;

        return ONCLI_SUCCESS;
        #endif
    } // else if the join command was received //
    #endif

    #ifdef ENABLE_SETNI_COMMAND
    if(!strncmp(ONCLI_SETNI_CMD_STR, CMD, strlen(ONCLI_SETNI_CMD_STR)))
    {
        *CMD_STR = ONCLI_SETNI_CMD_STR;

        if(CMD[strlen(ONCLI_SETNI_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &setni_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the sid command was received //
    #endif

    #ifdef PID_BLOCK
    if(!strncmp(ONCLI_PID_BLOCK_CMD_STR, CMD,
      strlen(ONCLI_PID_BLOCK_CMD_STR)))
    {
        *CMD_STR = ONCLI_PID_BLOCK_CMD_STR;

        if(CMD[strlen(ONCLI_PID_BLOCK_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &pid_block_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the pid block command was received //
    #endif

    #ifdef RANGE_TESTING
    if(!strncmp(ONCLI_RANGE_TEST_CMD_STR, CMD,
      strlen(ONCLI_RANGE_TEST_CMD_STR)))
    {
        *CMD_STR = ONCLI_RANGE_TEST_CMD_STR;

        if(CMD[strlen(ONCLI_RANGE_TEST_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &range_test_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the range test command was received //
    #endif

    #ifdef DEBUGGING_TOOLS
    if(!strncmp(ONCLI_CSDF_CMD_STR, CMD, strlen(ONCLI_CSDF_CMD_STR)))
    {
        *CMD_STR = ONCLI_CSDF_CMD_STR;

        if(CMD[strlen(ONCLI_CSDF_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &csdf_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the csdf command was received //

    if(!strncmp(ONCLI_MEMORY_CMD_STR, CMD, strlen(ONCLI_MEMORY_CMD_STR)))
    {
        *CMD_STR = ONCLI_MEMORY_CMD_STR;

        if(CMD[strlen(ONCLI_MEMORY_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &memory_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the memory command was received //

    if(!strncmp(ONCLI_MEMLOAD_CMD_STR, CMD, strlen(ONCLI_MEMLOAD_CMD_STR)))
    {
        *CMD_STR = ONCLI_MEMLOAD_CMD_STR;

        if(CMD[strlen(ONCLI_MEMLOAD_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &memload_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the memload command was received //

    if(!strncmp(ONCLI_MEMSET_CMD_STR, CMD, strlen(ONCLI_MEMSET_CMD_STR)))
    {
        *CMD_STR = ONCLI_MEMSET_CMD_STR;

        if(CMD[strlen(ONCLI_MEMSET_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &memset_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the memset command was received //

    if(!strncmp(ONCLI_MEMDUMP_CMD_STR, CMD, strlen(ONCLI_MEMDUMP_CMD_STR)))
    {
        *CMD_STR = ONCLI_MEMDUMP_CMD_STR;

        if(CMD[strlen(ONCLI_MEMDUMP_CMD_STR)] != '\n')
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        return memdump_cmd_hdlr();
    } // else if the memdump command was received //

    if(!strncmp(ONCLI_INTERVAL_CMD_STR, CMD, strlen(ONCLI_INTERVAL_CMD_STR)))
    {
        *CMD_STR = ONCLI_INTERVAL_CMD_STR;

        if(CMD[strlen(ONCLI_INTERVAL_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &interval_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the interval command was received //

    if(!strncmp(ONCLI_PAUSE_CMD_STR, CMD, strlen(ONCLI_PAUSE_CMD_STR)))
    {
        *CMD_STR = ONCLI_PAUSE_CMD_STR;

        if(CMD[strlen(ONCLI_PAUSE_CMD_STR)] != '\n')
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        return pause_cmd_hdlr();
    } // else if the pause command was received //
    if(!strncmp(ONCLI_RATCHET_CMD_STR, CMD, strlen(ONCLI_RATCHET_CMD_STR)))
    {
        *CMD_STR = ONCLI_RATCHET_CMD_STR;

        if(CMD[strlen(ONCLI_RATCHET_CMD_STR)] != '\n')
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        return ratchet_cmd_hdlr();
    } // else if the ratchet command was received //
    if(!strncmp(ONCLI_PROCEED_CMD_STR, CMD, strlen(ONCLI_PROCEED_CMD_STR)))
    {
        *CMD_STR = ONCLI_PROCEED_CMD_STR;

        if(CMD[strlen(ONCLI_PROCEED_CMD_STR)] != '\n')
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        return proceed_cmd_hdlr();
    } // else if the proceed command was received //
    #endif

    #ifdef ENABLE_ROUTE_COMMAND
    if(!strncmp(ONCLI_ROUTE_CMD_STR, CMD, strlen(ONCLI_ROUTE_CMD_STR)))
    {
        *CMD_STR = ONCLI_ROUTE_CMD_STR;

        if(CMD[strlen(ONCLI_ROUTE_CMD_STR)] != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the end the command is not valid //

        *next_state = ONCLI_RX_PARAM_NEW_LINE_STATE;
        *cmd_hdlr = &route_cmd_hdlr;

        return ONCLI_SUCCESS;
    } // else if the route command was received //
    #endif

    else
    {
        *CMD_STR = CMD;
        return ONCLI_INVALID_CMD;
    } // else the command was invalid //
}



//! @} oncli_hdlr_pub_func
//						PUBLIC FUNCTION IMPLEMENTATION END
//==============================================================================

//==============================================================================
//						PRIVATE FUNCTION IMPLEMENTATION
//! \addtogroup oncli_hdlr_pri_func
//! \ingroup oncli_hdlr
//! @{


#ifdef ENABLE_ECHO_COMMAND
/*!
    \brief Handles receiving the echo command and its parameters

    The echo command has the form

    echo:[ON | OFF]

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_CMD_FAIL If the command failed.
*/
oncli_status_t echo_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    BOOL echo = FALSE;
    UInt8 len;

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    if(!strncmp(ONCLI_ON_STR, ASCII_PARAM_LIST, strlen(ONCLI_ON_STR)))
    {
        echo = TRUE;
        len = strlen(ONCLI_ON_STR);
    } // if going to quiet mode //
    else if(!strncmp(ONCLI_OFF_STR, ASCII_PARAM_LIST, strlen(ONCLI_OFF_STR)))
    {
        echo = FALSE;
        len = strlen(ONCLI_OFF_STR);
    } // else if verbose mode //
    else
    {
        return ONCLI_PARSE_ERR;
    } // else unknown value //

    if(ASCII_PARAM_LIST[len] != '\n')
    {
        return ONCLI_PARSE_ERR;
    } // if the termination is not correct //

    echo_on = echo;
    return ONCLI_SUCCESS;
} // echo_cmd_hdlr //
#endif


#ifdef ENABLE_BAUD_COMMAND
static oncli_status_t baud_cmd_hdlr(const char* const ASCII_PARAM_LIST)
{
    char * end_ptr;
    UInt32 new_baud_rate;
    baud_rate_t br;

    if(!ASCII_PARAM_LIST)
    {
        return 0;
    } // if any of the parameters are invalid //

    new_baud_rate = one_net_strtol(ASCII_PARAM_LIST, &end_ptr, 10);

    // check the parameter delimiter
    if(*end_ptr != '\n')
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //


    if(new_baud_rate == 38400)
    {
        br = BAUD_38400;
    }
    else if(new_baud_rate == 115200)
    {
        br = BAUD_115200;
    }
    else
    {
        return ONCLI_UNSUPPORTED;
    }


    // TODO -- there's a better way to do this, I think.  This is more than
    // a little risky.  We need to disable uart altogether before re-
    // initializing.

    // let everything in the uart go out, then a short pause
    while(cb_bytes_queued(&uart_tx_cb)){}
    delay_ms(10); // short pause
    uart_init(br, DATA_BITS_8, STOP_BITS_1, PARITY_NONE);
    delay_ms(10); //  short pause
    return ONCLI_SUCCESS;
}
#endif


#ifdef ENABLE_VERBOSE_LEVEL_COMMAND
static oncli_status_t verbose_level_cmd_hdlr(const char* const ASCII_PARAM_LIST)
{
    char * end_ptr;
    UInt8 new_verbose_level;
    const UInt8 max_verbose_level = DEBUG_VERBOSE_LEVEL;

    if(!ASCII_PARAM_LIST)
    {
        return 0;
    } // if any of the parameters are invalid //

    new_verbose_level = one_net_strtol(ASCII_PARAM_LIST, &end_ptr, 10);

    // check the parameter delimiter
    if(*end_ptr != '\n')
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    if(new_verbose_level > max_verbose_level)
    {
        oncli_send_msg("Level should be <= %d.\n", max_verbose_level);
        return ONCLI_BAD_PARAM;
    }

    verbose_level = new_verbose_level;
    return ONCLI_SUCCESS;
}
#endif


#ifdef ENABLE_LIST_COMMAND
/*!
    \brief Prints information about the current configuration
    of the device.

    \param void

    \return ONCLI_SUCCESS if listing the current settings was successful
*/
static oncli_status_t list_cmd_hdlr(void)
{
    oncli_status_t status;
    UInt16 i;
    UInt8 flags;

    oncli_send_msg(ONCLI_STARTUP_FMT, ONE_NET_VERSION_MAJOR,
      ONE_NET_VERSION_MINOR);
    oncli_send_msg(ONCLI_STARTUP_REV_FMT, ONE_NET_VERSION_REVISION,
      ONE_NET_VERSION_BUILD);

    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        oncli_print_invite();
    }
    #endif


    #ifdef ONE_NET_MULTI_HOP
    oncli_send_msg("# of Network MH Devices : %d\n", on_base_param->num_mh_devices);
    oncli_send_msg("# of Network MH Repeaters : %d\n", on_base_param->num_mh_repeaters);
    #endif

    #if defined(ONE_NET_MASTER) && defined(ONE_NET_CLIENT)
	if(device_is_master || client_joined_network)
    #elif defined(ONE_NET_CLIENT)
    if(client_joined_network)
    #endif
	{
        // print encryption key
		oncli_send_msg    ("Message key : ");
	    oncli_print_xtea_key((const one_net_xtea_key_t*) &(on_base_param->current_key));
        oncli_send_msg("\n");
		oncli_send_msg    ("Old Message key : ");
	    oncli_print_xtea_key((const one_net_xtea_key_t*) &(on_base_param->old_key));
        oncli_send_msg("\n\n");
        // print the NID and the DID
        if(oncli_print_sid((const on_encoded_sid_t*)(on_base_param->sid)) !=
          ONCLI_SUCCESS)
        {
            return ONCLI_CMD_FAIL;
        }

        #ifdef BLOCK_MESSAGES_ENABLED
        oncli_send_msg("\n\n");
        oncli_print_fragment_delays();
        #endif
	}

    #if DEBUG_VERBOSE_LEVEL > 3
    if(verbose_level > 3)
    {
        oncli_send_msg("\n\nDevice Features...\n");
        oncli_print_features(on_base_param->features);
    }
    #endif

    #ifdef ONE_NET_CLIENT
    if(!device_is_master && !client_joined_network)
    {
        oncli_send_msg("\n\nCLIENT : Not Joined\n\n");
        return ONCLI_SUCCESS;
    }
    #endif

    #ifdef ONE_NET_MASTER
    if(device_is_master && (on_state == ON_JOIN_NETWORK ||
      on_state == ON_INIT_STATE))
    {
        oncli_send_msg("\n\nMASTER : Initializing\n\n");
        return ONCLI_SUCCESS;
    }
    #endif

    // print channel
    oncli_send_msg("\n\nChannel: ");
    if((status = oncli_print_channel(on_base_param->channel))
      != ONCLI_SUCCESS)
    {
        return status;
    }

    {
        #ifdef ONE_NET_CLIENT
        if(!device_is_master)
        {
            #ifdef ONE_NET_CLIENT
            flags = master->flags;
            oncli_send_msg("Keep-Alive Interval:%ld ms\n",
              master->keep_alive_interval);
            oncli_send_msg("Send To Master: %s\n", flags &
              ON_SEND_TO_MASTER ? TRUE_STR : FALSE_STR);
            oncli_send_msg("Reject Bad Msg ID: %s\n", flags &
              ON_REJECT_INVALID_MSG_ID ? TRUE_STR : FALSE_STR);
            #endif
        }
        else
        #endif
        {
            #if defined(ONE_NET_MASTER) && defined(BLOCK_MESSAGES_ENABLED)
            flags = master_param->block_stream_flags;
            #endif
        }
        #ifdef BLOCK_MESSAGES_ENABLED
        oncli_send_msg("Blk/Strm Data Rate: %s\n", flags &
          ON_BS_ELEVATE_DATA_RATE ? TRUE_STR : FALSE_STR);
        oncli_send_msg("Blk/Strm Chg Channel: %s\n", flags &
          ON_BS_CHANGE_CHANNEL ? TRUE_STR : FALSE_STR);
        oncli_send_msg("Blk/Strm High Prior.: %s\n", flags &
          ON_BS_HIGH_PRIORITY ? TRUE_STR : FALSE_STR);
        oncli_send_msg("Blk/Strm Allow Long.: %s\n", flags &
          ON_BS_ALLOWED ? TRUE_STR : FALSE_STR);
        #endif

        #ifdef ONE_NET_CLIENT
        #if DEBUG_VERBOSE_LEVEL > 3
        if(!device_is_master && verbose_level > 3)
        {
            oncli_send_msg("\nMaster Features...\n");
            oncli_print_features(master->device.features);
        }
        #endif
        #if DEBUG_VERBOSE_LEVEL > 5
        if(!device_is_master && verbose_level > 5)
        {
            oncli_send_msg("\n\n");
            print_sending_device_t(&(master->device));
            oncli_send_msg("\n\n");
        }
        #endif
        #endif
    }

    #ifdef ONE_NET_MASTER
    if(device_is_master)
    {
        UInt16 index = 0;
        // print the client list
        i = 0;
        oncli_send_msg("Client count: %d\n", master_param->client_count);
        while(i < master_param->client_count)
        {
            on_client_t* client = &client_list[index];
            index++;

            if(is_broadcast_did((const on_encoded_did_t*) client->device.did))
            {
                continue;
            }

            oncli_send_msg("\n\n\n  Client %d : ", i + 1);
            oncli_print_did((const on_encoded_did_t*) &(client->device.did));
            oncli_send_msg("\n\n");
            #if DEBUG_VERBOSE_LEVEL > 3
            if(verbose_level > 3)
            {
                oncli_send_msg("Keep-Alive Interval:%ld ms\n",
                  client->keep_alive_interval);
                oncli_send_msg("Send To Master: %s\n",
                  client->flags & ON_SEND_TO_MASTER ? TRUE_STR : FALSE_STR);
                oncli_send_msg("Reject Bad Msg ID: %s\n",
                  client->flags & ON_REJECT_INVALID_MSG_ID ? TRUE_STR : FALSE_STR);

                #ifdef BLOCK_MESSAGES_ENABLED
                if(features_block_capable(client->device.features))
                {
                    oncli_send_msg("Blk/Strm Data Rate: %s\n", client->flags &
                      ON_BS_ELEVATE_DATA_RATE ? TRUE_STR : FALSE_STR);
                    oncli_send_msg("Blk/Strm Chg Channel: %s\n", client->flags
                      & ON_BS_CHANGE_CHANNEL ? TRUE_STR : FALSE_STR);
                    oncli_send_msg("Blk/Strm High Prior.: %s\n", client->flags
                      & ON_BS_HIGH_PRIORITY ? TRUE_STR : FALSE_STR);
                    oncli_send_msg("Blk/Strm Allow Long.: %s\n", client->flags
                      & ON_BS_ALLOWED ? TRUE_STR : FALSE_STR);
                }
                #endif
            }
            #endif
            #if DEBUG_VERBOSE_LEVEL > 4
            if(verbose_level > 4)
            {
                oncli_send_msg("\n\nCurrent Key: %s\n", client->use_current_key ?
                  TRUE_STR : FALSE_STR);
                oncli_send_msg("Send Add Dev: %s\n", client->send_add_device_message
                  ? TRUE_STR : FALSE_STR);
                oncli_send_msg("Send Remove Dev: %s\n",
                  client->send_remove_device_message ? TRUE_STR : FALSE_STR);
                #if DEBUG_VERBOSE_LEVEL > 5
                if(verbose_level > 5)
                {
                    print_sending_device_t(&(client->device));
                }
                #endif
                oncli_send_msg("\n\n");
            }
            #endif

            #if DEBUG_VERBOSE_LEVEL > 3
            if(verbose_level > 3)
            {
                oncli_send_msg("\n\nFeatures...\n");
                oncli_print_features(client->device.features);
            }
            #endif

            i++;
        }
    }
    #endif

    #ifdef PEER
    #ifdef ONE_NET_CLIENT
    if(device_is_master || client_joined_network)
    #endif
    {
        oncli_print_peer_list();
    }
    #endif
    oncli_send_msg("\n\n");
    oncli_print_user_pin_cfg();
    return ONCLI_SUCCESS;
} // list_cmd_hdlr //
#endif


#ifdef ENABLE_ERASE_COMMAND
/*!
    \brief Erases the settings from non-volatile memory

    \param void

    \return ONCLI_SUCCESS if erasing the current settings was successful
            ONCLI_CMD_FAIL otherwise
*/
static oncli_status_t erase_cmd_hdlr(void)
{
    #ifdef ONE_NET_MASTER
    #ifdef ONE_NET_CLIENT
    if(device_is_master)
    {
        return ((one_net_master_erase_settings() == ONS_SUCCESS) ?
          ONCLI_SUCCESS : ONCLI_CMD_FAIL);
    }
    #else
    return ((one_net_master_erase_settings() == ONS_SUCCESS) ?
      ONCLI_SUCCESS : ONCLI_CMD_FAIL);
    #endif
    #endif

    #ifdef ONE_NET_CLIENT
    return ((one_net_client_erase_settings() == ONS_SUCCESS) ?
      ONCLI_SUCCESS : ONCLI_CMD_FAIL);
    #endif
}
#endif


#ifdef ENABLE_SAVE_COMMAND
/*!
    \brief Saves the settings to non-volatile memory

    \param void

    \return ONCLI_SUCCESS if saving the current settings was successful
            ONCLI_CMD_FAIL otherwise
*/
static oncli_status_t save_cmd_hdlr(void)
{
    save = TRUE;
    return ONCLI_SUCCESS;
}
#endif


/*!
    \brief Puts the device into sniffer mode

    The sniff command has the form

    sniff:NN

    where NN is the channel number to sniff.  The channel is 0 based.

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_CMD_FAIL If the command failed.
*/
#if defined(SNIFFER_MODE) && defined(ENABLE_SNIFF_COMMAND)
oncli_status_t sniff_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    oncli_status_t status;
    tick_t duration_ms = 0;
    UInt8 channel;

    char* END_PTR;
    const char* PARAM_PTR = ASCII_PARAM_LIST;

    // Get the duration if it's there
    if(isdigit(*PARAM_PTR))
    {
        duration_ms = one_net_strtol(PARAM_PTR, &END_PTR, 0);
	    PARAM_PTR = END_PTR;
        if(*PARAM_PTR != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the command isn't formatted properly //
        PARAM_PTR++;
    } // get the duration in milliseconds


    if((status = oncli_parse_channel(PARAM_PTR, &channel)) !=
      ONCLI_SUCCESS)
    {
        return status;
    } // if parsing the channel was not successful //

    return oncli_reset_sniff(channel, duration_ms);
} // sniff_cmd_hdlr //
#endif


#ifdef ENABLE_SINGLE_COMMAND
/*!
    \brief Handles receiving the single command and all its parameters.

    The single command has the form

    single:RAW DST DID:AABBCCDDEE

    where AABBCCDDEE is the packet to send in ASCII hex characters ('0' - '9',
    'A' - 'F' (lower case is valid also)).

    If sending to the peer list, instead of specifiying a destination did, use 000.


    For example, a "toggle" relay command from unit 1 would be the following.

    single:XXX:0051U00002   where XXX is the destination DID and U is the destination unit.
                            If XXX is a real device, U will need to be specified.  This message
                            will NOT go to the peer list.

                            To send to the peer list, specify the destination device as 000.  In this
                            case, the destination unit will be overwritten, so it can be anything.  In
                            the example below, U has been replaced with F, which is the ONE_NET_DEV_UNIT,
                            or device as a whole.

    single:000:0051F00002 would be a message from unit 1 to all units peered with unit 1
           telling them to toggle their relays.

    single:004:0051200002 would be a message from unit 1 to unit 2 of 004 telling
           it to toggle its relay.



    \param ASCII_PARAM_LIST ASCII parameter list.  The parameters in this list
      should be seperated by ':'.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_INVALID_CMD_FOR_DEVICE If the command is not valid for the
              current mode of the device.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_RSRC_UNAVAILABLE If the command can not be carried out
              because the resource is full.
            ONCLI_INVALID_DST If the destination is invalid
            ONCLI_NOT_JOINED If the device needs to join a network before the
              command can be carried out.
            ONCLI_INTERNAL_ERR If something unexpected occured
*/
static oncli_status_t single_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    on_encoded_did_t enc_dst;
    UInt8 src_unit;
    #ifdef PEER
    BOOL send_to_peer_list;
    #endif
    UInt16 data_len;
    UInt8 raw_pld[ONA_SINGLE_PACKET_PAYLOAD_LEN];

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    // get the send parameters
    #ifdef PEER
    if(!(PARAM_PTR = parse_ascii_tx_param(ASCII_PARAM_LIST, NULL, NULL,
      &enc_dst, &send_to_peer_list)))
    #else
    if(!(PARAM_PTR = parse_ascii_tx_param(ASCII_PARAM_LIST, NULL, NULL, &enc_dst)))
    #endif
    {
        return ONCLI_PARSE_ERR;
    } // if failed parsing parameters //

    // check the parameter delimiter
    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER)
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // get the data
    data_len = sizeof(raw_pld);
    if(!(PARAM_PTR = parse_ascii_tx_data(PARAM_PTR, raw_pld,
      &data_len)) || (*PARAM_PTR != '\n'))
    {
        return ONCLI_PARSE_ERR;
    } // if parsing the data portion failed //


    src_unit = get_src_unit(raw_pld);
    if(one_net_send_single(ONE_NET_RAW_SINGLE_DATA,
      ON_APP_MSG, raw_pld, ONA_SINGLE_PACKET_PAYLOAD_LEN,
      ONE_NET_HIGH_PRIORITY, NULL,
      #ifdef PEER
          send_to_peer_list ? NULL : (const on_encoded_did_t*) &enc_dst, send_to_peer_list,  src_unit
      #else
          (const on_encoded_did_t*) &enc_dst
      #endif
      #if SINGLE_QUEUE_LEVEL > MIN_SINGLE_QUEUE_LEVEL
          , 0
      #endif
      #if SINGLE_QUEUE_LEVEL > MED_SINGLE_QUEUE_LEVEL
          , 0
      #endif
      ))
    {
        return ONS_SUCCESS;
    }
    else
    {
        return ONCLI_RSRC_UNAVAILABLE;
    }
} // single_cmd_hdlr //


/*!
    \brief Handles receiving the single text command and all its parameters.

    The single text command has the form

    single text:SRC UNIT:DST UNIT:RAW DST DID:text

    single text:2:3:004:"abcd" will send "abcd" from source unit 2 to dest. unit
      3 of device 004.


    To send to the peer list, specify 000 for the DID.

    single text:2:3:000:"abcd" will send "abcd" from source unit 2 to dest. unit
      3 to all device / unit pairs peered with unit 2 (note that "PEER" must be
      enabled in config_options.h"


    \param ASCII_PARAM_LIST ASCII parameter list.  The parameters in this list
      should be seperated by ':'.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_INVALID_CMD_FOR_DEVICE If the command is not valid for the
              current mode of the device.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_RSRC_UNAVAILABLE If the command can not be carried out
              because the resource is full.
            ONCLI_CMD_FAIL If the command failed.
*/
static oncli_status_t single_txt_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    on_encoded_did_t enc_dst;
    UInt16 data_len, raw_pid, num_blocks;
    UInt8 src_unit, dst_unit;
    UInt8 raw_pld[ONA_MAX_SINGLE_PACKET_PAYLOAD_LEN] = {0};
    #ifdef PEER
    BOOL send_to_peer_list;
    #endif

    UInt8 pld_len, msg_type;

    #ifdef EXTENDED_SINGLE
    UInt8* text_start_ptr = &raw_pld[ONA_TEXT_DATA_IDX];
    #endif

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    // get the send parameters
    #ifdef PEER
    if(!(PARAM_PTR = parse_ascii_tx_param(ASCII_PARAM_LIST, &src_unit,
      &dst_unit, &enc_dst, &send_to_peer_list)))
    #else
    if(!(PARAM_PTR = parse_ascii_tx_param(ASCII_PARAM_LIST, &src_unit,
      &dst_unit, &enc_dst)))
    #endif
    {
        return ONCLI_PARSE_ERR;
    } // if failed parsing parameters //

    // check the parameter delimiter
    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER)
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // get the data
    data_len = sizeof(raw_pld) - ONA_TEXT_DATA_IDX;
    #ifdef EXTENDED_SINGLE
    data_len = sizeof(raw_pld) - ONA_TEXT_DATA_IDX - 1;
    #endif

    if(!(PARAM_PTR = parse_ascii_tx_text_data(PARAM_PTR,
      &raw_pld[ONA_TEXT_DATA_IDX], &data_len)) || (*PARAM_PTR != '\n'))
    {
        return ONCLI_PARSE_ERR;
    } // if parsing the data portion failed //

    raw_pid = ONE_NET_RAW_SINGLE_DATA;
    num_blocks = 1;
    pld_len = ONA_SINGLE_PACKET_PAYLOAD_LEN;
    msg_type = ONA_SIMPLE_TEXT;

    #ifdef EXTENDED_SINGLE
    if(data_len <= ONA_SINGLE_PACKET_PAYLOAD_LEN - ONA_TEXT_DATA_IDX)
    {
        // pid, pld_len, msg_type already set.  Do nothing.
    }
    else if(data_len < ONA_LARGE_SINGLE_PACKET_PAYLOAD_LEN -
      ONA_TEXT_DATA_IDX -1)
    {
        num_blocks = 2;
        pld_len = ONA_LARGE_SINGLE_PACKET_PAYLOAD_LEN;
        msg_type = ONA_TEXT;
        // add a NULL terminator
        text_start_ptr[data_len] = 0;
    }
    else
    {
        num_blocks = 3;
        pld_len = ONA_EXTENDED_SINGLE_PACKET_PAYLOAD_LEN;
        msg_type = ONA_TEXT;
        // add a NULL terminator
        text_start_ptr[data_len] = 0;
    }
    #endif

    // store the message class/message type in the payload
    put_msg_class(ONA_COMMAND, raw_pld);
    put_msg_type(msg_type, raw_pld);

    // store the source and destination unit numbers in the payload
    put_dst_unit(dst_unit, raw_pld);
    put_src_unit(src_unit, raw_pld);

    raw_pid |= (num_blocks << 8);


    if(one_net_send_single(raw_pid, ON_APP_MSG, raw_pld, pld_len,
      ONE_NET_HIGH_PRIORITY, NULL,
      #ifdef PEER
          send_to_peer_list ? NULL : (const on_encoded_did_t*) &enc_dst, send_to_peer_list,  src_unit
      #else
          (const on_encoded_did_t*) &enc_dst
      #endif
      #if SINGLE_QUEUE_LEVEL > MIN_SINGLE_QUEUE_LEVEL
          , 0
      #endif
      #if SINGLE_QUEUE_LEVEL > MED_SINGLE_QUEUE_LEVEL
          , 0
      #endif
      ))
    {
        return ONS_SUCCESS;
    }
    else
    {
        return ONCLI_RSRC_UNAVAILABLE;
    }
} // single_txt_cmd_hdlr //


static oncli_status_t status_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    return parse_and_send_pin_msg(ASCII_PARAM_LIST, ONA_STATUS);
}


static oncli_status_t query_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    return parse_and_send_pin_msg(ASCII_PARAM_LIST, ONA_QUERY);
}


static oncli_status_t fast_query_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    return parse_and_send_pin_msg(ASCII_PARAM_LIST, ONA_FAST_QUERY);
}


static oncli_status_t set_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    return parse_and_send_pin_msg(ASCII_PARAM_LIST, ONA_COMMAND);
}


/*!
    \brief Parses and sends commands, status messages, queries, and fast queries

    Valid commands are of the following type...

    status pin:DDD:P
    query pin:DDD:P
    fast query pin:DDD:P
    set pin:DDD:P:V

    where DDD is the raw DID of the device receiving the message, P is the pin
              number, V is the value to set the pin.

    status pin:003:2 sends the status of THIS DEVICE's pin number 2 to device 003
    query pin:003:2 tells device 003 to report back the current state of it pin number 2
    fast query pin:003:2 Same as "query pin:003:2" except this is a "fast query" (i.e.
        device 003 should NOT send back a separate message, but instead only send
        back the status in the response message.
    set pin:003:2:1 commands device device 003 to set pin 2 to state 1(high).  Valid states
        are 0(low), 1(high), or 2(toggle).  This can only be used as if pin 2 of device 003
        is an output pin.



    \param[in] ASCII_PARAM_LIST ASCII parameter list.
    \param[in] msg_class The message class of the outgoing single app message

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            See user_pin for more return values.
*/
static oncli_status_t parse_and_send_pin_msg(
  const char * const ASCII_PARAM_LIST, UInt8 msg_class)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    char * endptr = NULL;
    on_encoded_did_t enc_dst;
    UInt16 raw_did_int;
    on_raw_did_t raw_dst;
    UInt8 src_unit, dst_unit, unit;
    UInt8 pin_value = ONE_NET_NUM_UNITS;
    UInt8 raw_pld[ONA_SINGLE_PACKET_PAYLOAD_LEN];

    one_net_memset(raw_pld, 0, sizeof(raw_pld));

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    // store the message class/message type in the payload
    put_msg_class(msg_class, raw_pld);
    put_msg_type(ONA_SWITCH, raw_pld);

    raw_did_int = one_net_strtol(PARAM_PTR, &endptr, 16);
    if(raw_did_int < 0x001 || raw_did_int > 0xFFF || *endptr != ':')
    {
        return ONCLI_PARSE_ERR;
    }

    raw_did_int <<= 4;
    raw_dst[0] = (raw_did_int & 0xFF00) >> 8;
    raw_dst[1] = raw_did_int & 0xFF;
    on_encode(enc_dst, raw_dst, ON_ENCODED_DID_LEN);

    if(*endptr == '\n')
    {
        return ONCLI_PARSE_ERR;
    }

    endptr++;
    PARAM_PTR = endptr;

    unit = one_net_strtol(PARAM_PTR, &endptr, 16);
    if(msg_class == ONA_STATUS && unit >= ONE_NET_NUM_UNITS)
    {
        return ONCLI_PARSE_ERR;
    }

    if(msg_class == ONA_COMMAND)
    {
        if(*endptr != ':')
        {
            return ONCLI_PARSE_ERR;
        }
        endptr++;
        PARAM_PTR = endptr;

        pin_value = one_net_strtol(PARAM_PTR, &endptr, 10);
        if(pin_value >= ONA_TOGGLE)
        {
            return ONCLI_PARSE_ERR;
        }
    }

    if(*endptr != '\n')
    {
        return ONCLI_PARSE_ERR;
    }

    src_unit = ONE_NET_DEV_UNIT;
    dst_unit = unit;

    if(msg_class == ONA_STATUS)
    {
        src_unit = unit;
        dst_unit = ONE_NET_DEV_UNIT;
        switch(src_unit)
        {
            #ifndef ATXMEGA256A3B
                // Renesas Pins
                case 0: pin_value = USER_PIN0; break;
                case 1: pin_value = USER_PIN1; break;
                case 2: pin_value = USER_PIN2; break;
                case 3: pin_value = USER_PIN3; break;
            #else
                // Atmel Pins
                case 0:
                    pin_value = USER_PIN0_INPUT_PORT_REG & (1 << USER_PIN0_BIT);
                    break;
                case 1:
                    pin_value = USER_PIN1_INPUT_PORT_REG & (1 << USER_PIN1_BIT);
                    break;
                case 2:
                    pin_value = USER_PIN2_INPUT_PORT_REG & (1 << USER_PIN2_BIT);
                    break;
                case 3:
                    pin_value = USER_PIN3_INPUT_PORT_REG & (1 << USER_PIN3_BIT);
                    break;
            #endif
                default:
                    return ONCLI_PARSE_ERR;  // TODO -- Do we want a parse error or something else here?
        }

        #ifdef ATXMEGA256A3B
        if(pin_value != 0)
        {
    	    pin_value = 1;
        }
        #endif
    }

    if(msg_class == ONA_STATUS || msg_class == ONA_COMMAND)
    {
        put_msg_data(pin_value, raw_pld, ON_APP_MSG);
    }


    // store the source and destination unit numbers in the payload
    put_dst_unit(dst_unit, raw_pld);
    put_src_unit(src_unit, raw_pld);

    if(one_net_send_single(ONE_NET_RAW_SINGLE_DATA,
      ON_APP_MSG, raw_pld, ONA_SINGLE_PACKET_PAYLOAD_LEN,
      ONE_NET_HIGH_PRIORITY, NULL, (const on_encoded_did_t*) &enc_dst
      #ifdef PEER
          , FALSE,  src_unit
      #endif
      #if SINGLE_QUEUE_LEVEL > MIN_SINGLE_QUEUE_LEVEL
          , 0
      #endif
      #if SINGLE_QUEUE_LEVEL > MED_SINGLE_QUEUE_LEVEL
          , 0
      #endif
      ))
    {
        return ONCLI_SUCCESS;
    }
    else
    {
        return ONCLI_RSRC_UNAVAILABLE;
    }
}
#endif // ENABLE_SINGLE_COMMAND //


#ifdef ENABLE_SET_DR_CHANNEL_COMMAND
// set dr_channel:1:003:4000:1500:US:7 will send a message to device DID 003 to
// set data rate to 76,800 KHz and the channel to US Channel 7 in 4000
// milliseconds and to set the data rate back to where it was if it does not
// see a relevant message in a 1500 millisecond period.
oncli_status_t set_dr_channel_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    SInt8 new_data_rate, new_channel;
    oncli_status_t status;
    const char* endptr = 0;
    UInt16 pause_time_ms;
    UInt16 dormant_time_ms;
    const char* PARAM_PTR = (const char*) ASCII_PARAM_LIST;
    on_raw_did_t raw_did;
    UInt16 raw_did_int;
    on_encoded_did_t enc_did;


    new_data_rate = one_net_strtol(PARAM_PTR, &endptr, 10);
    if(*endptr != ':')
    {
        return ONCLI_PARSE_ERR;
    }

    if(new_data_rate < 0 || new_data_rate >= ONE_NET_DATA_RATE_LIMIT)
    {
        return ONCLI_UNSUPPORTED;
    }
    PARAM_PTR = endptr + 1;

    raw_did_int = one_net_strtol(PARAM_PTR, &endptr, 16);
    if(*endptr != ':')
    {
        return ONCLI_PARSE_ERR;
    }
    PARAM_PTR = endptr + 1;

    pause_time_ms = one_net_strtol(PARAM_PTR, &endptr, 10);
    if(*endptr != ':')
    {
        return ONCLI_PARSE_ERR;
    }
    PARAM_PTR = endptr + 1;

    dormant_time_ms = one_net_strtol(PARAM_PTR, &endptr, 10);
    if(*endptr != ':')
    {
        return ONCLI_PARSE_ERR;
    }

    if(!u16_to_did(raw_did_int, (on_raw_did_t*) raw_did))
    {
        return ONCLI_PARSE_ERR;
    }

    if(on_encode(enc_did, raw_did, ON_ENCODED_DID_LEN) != ONS_SUCCESS)
    {
        return ONCLI_PARSE_ERR;
    }
    PARAM_PTR = endptr + 1;

    if((status = oncli_parse_channel(PARAM_PTR, (UInt8*)(&new_channel))) != ONCLI_SUCCESS)
    {
        return status;
    }

    switch(on_change_dr_channel((on_encoded_did_t*) enc_did, pause_time_ms,
      dormant_time_ms, new_channel, (UInt8) new_data_rate))
    {
        case ON_NACK_RSN_NO_ERROR: return ONCLI_SUCCESS;
        default: return ONCLI_CMD_FAIL;
    }
}
#endif


/*!
    \brief Handles receiving the user pin command and all its parameters.

    \param[in] ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            See user_pin for more return values.
*/
#ifdef ENABLE_USER_PIN_COMMAND
static oncli_status_t user_pin_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;

    char * end_ptr;

    UInt8 pin;
    UInt8 pin_type;

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    if(!isdigit(*PARAM_PTR))
    {
        return ONCLI_PARSE_ERR;
    } // if the data is not formatted correctly //

    // read in the pin
    pin = (UInt8) one_net_strtol(PARAM_PTR, &end_ptr, 0);
    if(!end_ptr || end_ptr == PARAM_PTR)
    {
        return ONCLI_PARSE_ERR;
    } // if parsing the data failed //
    PARAM_PTR = end_ptr;

    // check the delimiter
    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER)
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // check the action
    if(!strncmp(PARAM_PTR, ONCLI_INPUT_STR, strlen(ONCLI_INPUT_STR)))
    {
        pin_type = ON_INPUT_PIN;
        PARAM_PTR += strlen(ONCLI_INPUT_STR);
    } // if it should be an input //
    else if(!strncmp(PARAM_PTR, ONCLI_OUTPUT_STR, strlen(ONCLI_OUTPUT_STR)))
    {
        pin_type = ON_OUTPUT_PIN;
        PARAM_PTR += strlen(ONCLI_OUTPUT_STR);
    } // else if it should be an output //
    else if(!strncmp(PARAM_PTR, ONCLI_DISABLE_STR, strlen(ONCLI_DISABLE_STR)))
    {
        pin_type = ON_DISABLE_PIN;
        PARAM_PTR += strlen(ONCLI_DISABLE_STR);
    } // else if it should be an output //
    else
    {
        return ONCLI_PARSE_ERR;
    } // else if the priority is invalid //

    if(*PARAM_PTR != '\n')
    {
        return ONCLI_PARSE_ERR;
    } // if the data is not formatted correctly //

    return oncli_set_user_pin_type(pin, pin_type);
} // user_pin_cmd_hdlr //
#endif


#ifdef ENABLE_INVITE_COMMAND
/*!
    \brief Handles receiving the invite command and all its parameters.

    The invite command has the form

    invite:AAAA-BBBB:TTTT

    where AAAA-BBBB is the unique invite key for the CLIENT to invite
    and TTTT (note : TTTT is not necessarily exactly 4 characters) is an
    optional timeout time

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
*/
static oncli_status_t invite_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    oncli_status_t status;
    char* ptr;
    char* end_ptr;
    long int timeout = ONE_NET_MASTER_INVITE_DURATION;
    one_net_xtea_key_t invite_key;

    // 3-22-13 ////////////////////////////////
    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        return ONCLI_INVALID_CMD_FOR_NODE;
    }
    #endif
    ////////////////////////////////////////////

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    if((status = parse_invite_key(ASCII_PARAM_LIST, &end_ptr,
      &invite_key)) != ONCLI_SUCCESS)
    {
        return status;
    } // if parsing the key was not successful //

    if(*end_ptr == ONCLI_PARAM_DELIMITER)
    {
        ptr = end_ptr + 1;
        timeout = one_net_strtol(ptr, &end_ptr, 10);
        if(timeout <= 0)
        {
            return ONCLI_BAD_PARAM;
        }
    }

    if(*end_ptr != '\n')
    {
        return ONCLI_PARSE_ERR;
    }

    switch(one_net_master_invite((const one_net_xtea_key_t*)&invite_key, (UInt32) timeout))
    {
        case ONS_SUCCESS: return ONCLI_SUCCESS;
        case ONS_DEVICE_LIMIT: return ONCLI_RSRC_UNAVAILABLE;
        case ONS_ALREADY_IN_PROGRESS: return ONCLI_ALREADY_IN_PROGRESS;
        case ONS_NOT_INIT: return ONCLI_ONS_NOT_INIT_ERR;
        case ONS_BAD_PARAM: return ONCLI_BAD_PARAM;
        case ONS_INTERNAL_ERR: return ONCLI_INTERNAL_ERR;
        default: return ONCLI_CMD_FAIL;
    }
}
#endif


#ifdef ENABLE_CANCEL_INVITE_COMMAND
extern one_net_xtea_key_t invite_key;
static oncli_status_t cancel_invite_cmd_hdlr(void)
{
    one_net_master_invite_result(ONS_CANCELED, &invite_key, 0);
    one_net_master_cancel_invite((const one_net_xtea_key_t*) &invite_key);
    return ONCLI_SUCCESS;
}
#endif


#ifdef ENABLE_REMOVE_DEVICE_COMMAND
/*!
    \brief Handles receiving the remove device command and all its parameters.

    \param[in] ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was successful
            ONCLI_ALREADY_IN_PROGRESS if the system is already in the middle
                of a device deletion.
            ONCLI_RSRC_UNAVAILABLE is the maste is currently processing a
                transaction.
            ONCLI_BAD_PARAM If any of the parameters passed into this functtion
              are invalid.
            ONCLI_INVALID_DST If the device that we are attempting to remove is
              not part of the network
*/
static oncli_status_t rm_dev_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    on_raw_did_t dst;

    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        return ONCLI_INVALID_CMD_FOR_NODE;
    }
    #endif

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    // read in the did
    if(ascii_hex_to_byte_stream(PARAM_PTR, dst, ONCLI_ASCII_RAW_DID_SIZE)
      != ONCLI_ASCII_RAW_DID_SIZE)
    {
        return ONCLI_PARSE_ERR;
    } // if converting the raw peer did failed //
    PARAM_PTR += ONCLI_ASCII_RAW_DID_SIZE;

    if(*PARAM_PTR != '\n')
    {
        return ONCLI_PARSE_ERR;
    } // if the data is not formatted correctly //

    switch(one_net_master_remove_device((const on_raw_did_t*)&dst))
    {
        case ONS_SUCCESS: return ONCLI_SUCCESS;
        case ONS_ALREADY_IN_PROGRESS: return ONCLI_ALREADY_IN_PROGRESS;
        case ONS_BUSY: return ONCLI_RSRC_UNAVAILABLE;
        case ONS_INCORRECT_ADDR: return ONCLI_INVALID_DST;
        default: return ONCLI_CMD_FAIL;
    }
} // rm_dev_cmd_hdlr //
#endif


#if defined(ENABLE_ASSIGN_PEER_COMMAND) || defined(ENABLE_UNASSIGN_PEER_COMMAND)
/*!
    \brief Changes a peer list.

    The command has the form "aaaaaaa:bbb:c:ddd:e"

    where aaaaaaa is either "assign peer" or "unassign peer"

    Note that "unassign peer" has "wildcards".  "assign peer" does not.

    where bbb is the three digit raw did of the source.
    where c is the source unit ("f" is a wildcard)
    where ddd is the three digit raw did of the peer ("000" is a wildcard).
    where e is the peer unit ("f" is a wildcard).


    \param [in] ASSIGN.  If true, we are adding a peer.  If false, we are
                         deleting a peer.
    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_RSRC_FULL If the peer assignment is for the master and there
              is no room on the list.
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_INVALID_DST If the either address is undecipherable or not
              part of the network or both addresses are the same.
            ONCLI_INVALID_CMD_FOR_NODE If a client attempts to execute this
              master-only command.
            ONCLI_CMD_FAIL For any other failure
*/
static oncli_status_t oncli_change_peer_list(BOOL ASSIGN,
  const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    char * end_ptr = 0;
    on_raw_did_t peer_did, src_did;
    UInt8 peer_unit, src_unit;

    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        return ONCLI_INVALID_CMD_FOR_NODE;
    }
    #endif


    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    // read in the source did
    if(ascii_hex_to_byte_stream(PARAM_PTR, src_did, ONCLI_ASCII_RAW_DID_SIZE)
      != ONCLI_ASCII_RAW_DID_SIZE)
    {
        return ONCLI_PARSE_ERR;
    } // if converting the source peer did failed //
    PARAM_PTR += ONCLI_ASCII_RAW_DID_SIZE;

    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER)
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // read in the source unit
    src_unit = one_net_strtol(PARAM_PTR, &end_ptr, 16);
    if(!end_ptr || end_ptr == PARAM_PTR)
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //
    PARAM_PTR = end_ptr;

    // check the parameter delimiter
    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER)
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // read in the peer did
    if(ascii_hex_to_byte_stream(PARAM_PTR, peer_did, ONCLI_ASCII_RAW_DID_SIZE)
      != ONCLI_ASCII_RAW_DID_SIZE)
    {
        return ONCLI_PARSE_ERR;
    } // if converting the peer destination did failed //
    PARAM_PTR += ONCLI_ASCII_RAW_DID_SIZE;

    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER)
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // read in the peer unit
    peer_unit = one_net_strtol(PARAM_PTR, &end_ptr, 16);
    if(!end_ptr || end_ptr == PARAM_PTR || (*end_ptr != '\n'))
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //


    // we're the master and we want to assign a peer to a client
    switch(one_net_master_peer_assignment(ASSIGN, (const on_raw_did_t*) &src_did, src_unit,
        (const on_raw_did_t*)&peer_did, peer_unit))
    {
        case ONS_SUCCESS: return ONCLI_SUCCESS;
        case ONS_RSRC_FULL: return ONCLI_RSRC_UNAVAILABLE;
        case ONS_INCORRECT_ADDR: return ONCLI_INVALID_DST;
        default: return ONCLI_CMD_FAIL;
    }
}
#endif


#ifdef ENABLE_ASSIGN_PEER_COMMAND
/*!
    \brief Adds a peer to a peer list of a device

    The command has the form "assign peer:bbb:c:ddd:e"


    where bbb is the three digit raw did of the source.
    where c is the source unit
    where ddd is the three digit raw did of the peer
    where e is the peer unit


    \param [in] ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONS_RSRC_FULL If the peer assignment is for the master and there
              is no room on the list.
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONS_INCORRECT_ADDR If the either address is undecipherable or not
              part of the network or both addresses are the same.
            ONCLI_INVALID_CMD_FOR_NODE If a client attempts to execute this
              master-only command.
            ONCLI_CMD_FAIL For any other failure
*/
static oncli_status_t assign_peer_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    return oncli_change_peer_list(TRUE, ASCII_PARAM_LIST);
}
#endif


#ifdef ENABLE_UNASSIGN_PEER_COMMAND
/*!
    \brief Removes peer(s) from a peer list

    The command has the form "unassign peer:bbb:c:ddd:e"

    where bbb is the three digit raw did of the source.
    where c is the source unit ("f" is a wildcard)
    where ddd is the three digit raw did of the peer ("000" is a wildcard).
    where e is the peer unit ("f" is a wildcard).


    Wildcards mean that all existing peers that match will be deleted.

    For example, "unassign peer:003:f:000:5" will delete all peers in device
    003 with any source unit, any peer device, and a peer unit of 5.



    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_RSRC_FULL If the peer assignment is for the master and there
              is no room on the list.
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_INVALID_DST  If the either address is undecipherable or not
              part of the network or both addresses are the same.
            ONCLI_INVALID_CMD_FOR_NODE If a client attempts to execute this
              master-only command.
            ONCLI_CMD_FAIL For any other failure
*/
static oncli_status_t unassign_peer_cmd_hdlr(
  const char * const ASCII_PARAM_LIST)
{
    return oncli_change_peer_list(FALSE, ASCII_PARAM_LIST);
}
#endif


#if defined(ENABLE_JOIN_COMMAND) && defined(ENHANCED_INVITE)
/*!
    \brief Handles receiving the join command and all its parameters

    The join command has the form

    join:TTT:LL:NN

	TTT = timeout (in milliseconds)
	LL  = Locale (US or Europe)
	NN  = Channel number within locale

    If no parameters, all channels on all locales are scanned, no timeout.
	If no timeout is specified, there is no timeout.
	If a locale is specified, but not channel number, all channels for that
	locale are searched.
	If a locale and a channel are specified, only that channel is listened to.

	Example: join:30000:US:5
	will look for an invite on US Channel 5 for 30 seconds

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_CMD_FAIL If the command failed.
*/
oncli_status_t join_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
	oncli_status_t status;
	one_net_xtea_key_t* invite_key;
	UInt16 timeout = 0;
    UInt8 low_channel = 0;
	UInt8 high_channel = ONE_NET_MAX_CHANNEL;
    const char * PARAM_PTR = ASCII_PARAM_LIST;
	char* END_PTR = 0;

    // get the unique invite code for this device
    invite_key = one_net_client_get_invite_key();

	if(*PARAM_PTR == '\n')
    {
		return (one_net_client_reset_client((const one_net_xtea_key_t*)invite_key,
          low_channel, high_channel, timeout) == ONS_SUCCESS) ? ONCLI_SUCCESS :
          ONCLI_CMD_FAIL;
	}

    // Get the timeout if it's there
    if(isdigit(*PARAM_PTR))
    {
        timeout = one_net_strtol(PARAM_PTR, &END_PTR, 0);
	    PARAM_PTR = END_PTR;
        if(*PARAM_PTR != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        } // if the command isn't formatted properly //
        PARAM_PTR++;
    } // get the timeout time

	if(*PARAM_PTR != '\n')
    {
		if((status = oncli_parse_channel(PARAM_PTR, &low_channel)) != ONCLI_SUCCESS)
        {
            return status;
        } // if parsing the channel was not successful //

		high_channel = low_channel;
	}

	return (one_net_client_reset_client((const one_net_xtea_key_t*)invite_key,
      low_channel, high_channel, timeout) == ONS_SUCCESS) ? ONCLI_SUCCESS :
      ONCLI_CMD_FAIL;
} // join_cmd_hdlr //
#endif


#ifdef ENABLE_SET_FLAGS_COMMAND
/*!
    \brief Sets the flags in a CLIENT.

    The set_flags_cmd_hdlr command has the form

    set flags:did:new_flags

    where new_flags is the new flags value.  See the on_master_flag_t
    options.  "OR" them for the options you want.  This is a MASTER only
    function.  The flags value should be entered in hexadecimal.

    For example, if you want the ON_JOINED and ON_SEND_TO_MASTER flags set for
    Raw DID 003, the ON_JOINED flag is 0x80  and the ON_SEND_TO_MASTER flag is
    0x40.  "ORing" 0x80 and 0x40 results in 0xC0, so the command would be...

    set flags:003:C0

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
*/
static oncli_status_t set_flags_cmd_hdlr(
  const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    on_raw_did_t dst;
    on_encoded_did_t enc_did;
    on_client_t* client;
    UInt8 flags;

    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        return ONCLI_INVALID_CMD_FOR_NODE;
    }
    #endif

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    // read in the device did
    if(ascii_hex_to_byte_stream(PARAM_PTR, dst, ONCLI_ASCII_RAW_DID_SIZE)
      != ONCLI_ASCII_RAW_DID_SIZE)
    {
        return ONCLI_PARSE_ERR;
    } // if converting the raw peer did failed //
    PARAM_PTR += ONCLI_ASCII_RAW_DID_SIZE;

    // check the parameter delimiter
    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER)
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // read in the new flags value.  It's a UInt8, so two characters are
    // expected for one byte
    if(ascii_hex_to_byte_stream(PARAM_PTR, &flags, sizeof(UInt8) * 2)
      != sizeof(UInt8) * 2)
    {
        return ONCLI_PARSE_ERR;
    } // if converting the flags value failed //
    PARAM_PTR += (sizeof(UInt8) * 2);


    if(*PARAM_PTR != '\n')
    {
        return ONCLI_PARSE_ERR;
    } // if the data is not formatted correctly //

    if(on_encode(enc_did, dst, ON_ENCODED_DID_LEN) != ONS_SUCCESS)
    {
        return ONCLI_PARSE_ERR;
    }

    #ifdef BLOCK_MESSAGES_ENABLED
    if(is_master_did((const on_encoded_did_t*) &enc_did))
    {
        master_param->block_stream_flags = flags;
        return ONCLI_SUCCESS;
    }
    #endif

    client = client_info((const on_encoded_did_t*)&enc_did);
    if(!client)
    {
        return ONCLI_INVALID_DST;
    }

    switch(one_net_master_set_flags(client, flags))
    {
        case ONS_SUCCESS:
        {
            return ONCLI_SUCCESS;
        } // success case //

        default:
        {
            return ONCLI_CMD_FAIL;
        } // default case //
    } // switch(one_net_master_set_flaga) //
}
#endif


#ifdef ENABLE_CHANGE_KEEP_ALIVE_COMMAND
static oncli_status_t change_keep_alive_cmd_hdlr(
  const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    char * end_ptr = 0;
    on_raw_did_t dst;
    UInt32 keep_alive;

    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        return ONCLI_INVALID_CMD_FOR_NODE;
    }
    #endif

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    // read in the peer did
    if(ascii_hex_to_byte_stream(PARAM_PTR, dst, ONCLI_ASCII_RAW_DID_SIZE)
      != ONCLI_ASCII_RAW_DID_SIZE)
    {
        return ONCLI_PARSE_ERR;
    } // if converting the raw peer did failed //
    PARAM_PTR += ONCLI_ASCII_RAW_DID_SIZE;

    // check the parameter delimiter
    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER || !isdigit(*PARAM_PTR))
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // get the value
    keep_alive = one_net_strtol(PARAM_PTR, &end_ptr, 0);
    if(!end_ptr || end_ptr == PARAM_PTR || (*end_ptr != '\n'))
    {
        return ONCLI_PARSE_ERR;
    } // if parsing the data failed //


    switch(one_net_master_change_client_keep_alive((const on_raw_did_t*) &dst, keep_alive))
    {
        case ONS_SUCCESS:
        {
            return ONCLI_SUCCESS;
        } // success case //

        case ONS_INCORRECT_ADDR:
        {
            return ONCLI_INVALID_DST;
        } // incorrect address case //

        default:
        {
            return ONCLI_CMD_FAIL;
        } // default case //
    } // switch(one_net_master_change_client_keep_alive) //
}
#endif


#ifdef ENABLE_CHANGE_FRAGMENT_DELAY_COMMAND
/*!
    \brief Handles receiving the change fragment delay command and all its
      parameters.

    The change fragment delay command has the form

    change fragment delay:did:priority:time_ms


    For example, "Change fragment delay:003:low:100" will change the low
    priority fragment delay of device 003 to 100 ms.

    This is a MASTER only command.

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.

            See below for other errors

*/
static oncli_status_t change_frag_dly_cmd_hdlr(
  const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    char * end_ptr;
    on_raw_did_t did;
    UInt16 delay;
    UInt8 priority;

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        return ONCLI_INVALID_CMD_FOR_NODE;
    }
    #endif

    // read in the peer did
    if(ascii_hex_to_byte_stream(PARAM_PTR, did, ONCLI_ASCII_RAW_DID_SIZE)
      != ONCLI_ASCII_RAW_DID_SIZE)
    {
        return ONCLI_PARSE_ERR;
    } // if converting the raw did failed //
    PARAM_PTR += ONCLI_ASCII_RAW_DID_SIZE;

    // check the parameter delimiter
    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER)
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // read in the priority
    if(!strncmp(PARAM_PTR, ONCLI_LOW_STR, strlen(ONCLI_LOW_STR)))
    {
        priority = ONE_NET_LOW_PRIORITY;
        PARAM_PTR += strlen(ONCLI_LOW_STR);
    } // if it's low priority //
    else if(!strncmp(PARAM_PTR, ONCLI_HIGH_STR, strlen(ONCLI_HIGH_STR)))
    {
        priority = ONE_NET_HIGH_PRIORITY;
        PARAM_PTR += strlen(ONCLI_HIGH_STR);
    } // else if it's high priority
    else
    {
        return ONCLI_PARSE_ERR;
    } // else the priority is invalid //

    // check the parameter delimiter
    if(*PARAM_PTR++ != ONCLI_PARAM_DELIMITER || !isdigit(*PARAM_PTR))
    {
        return ONCLI_PARSE_ERR;
    } // if malformed parameter //

    // read in delay
    delay = one_net_strtol(PARAM_PTR, &end_ptr, 0);
    if(!end_ptr || end_ptr == PARAM_PTR || (*end_ptr != '\n'))
    {
        return ONCLI_PARSE_ERR;
    } // if parsing the data failed //

    switch(one_net_master_change_frag_dly((const on_raw_did_t*) &did,
      priority == ONE_NET_LOW_PRIORITY ? delay : 0,
      priority == ONE_NET_HIGH_PRIORITY ? delay : 0))
    {
        case ONS_DEVICE_NOT_CAPABLE: return ONCLI_UNSUPPORTED;
        case ONS_RSRC_FULL: return ONCLI_RSRC_UNAVAILABLE;
        case ONS_INCORRECT_ADDR: return ONCLI_INVALID_DST;
        case ONS_SUCCESS: return ONCLI_SUCCESS;
        default: return ONCLI_CMD_FAIL;
    }
} // change_frag_dly_cmd_hdlr //
#endif


#ifdef BLOCK_MESSAGES_ENABLED
/*!
    \brief Handles receiving the block command and all its
      parameters.

    The change block command has the form

    block:did:transfer_size:chunk_delay


    For example, "block:006:5000:50" will send 5000 characters to device
    006 with a "chunk delay" of 50 milliseconds

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.

            See oncli_status_t for other errors

*/
static oncli_status_t block_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    UInt16 raw_did_int;
    on_raw_did_t raw_did;
    UInt16 chunk_delay_ms;
    const char* ptr = ASCII_PARAM_LIST;
    char* end_ptr;
    on_ack_nack_t ack_nack;
    UInt8 priority;
    on_encoded_did_t enc_dst_did;

    if(bs_msg.transfer_in_progress)
    {
        return ONCLI_RSRC_UNAVAILABLE;
    }

    raw_did_int = one_net_strtol(ptr, &end_ptr, 16);
    if(*end_ptr != ':')
    {
        return ONCLI_PARSE_ERR;
    }
    ptr = end_ptr + 1;

    bs_msg.bs.block.transfer_size = one_net_strtol(ptr, &end_ptr, 10);
    if(*end_ptr != ':')
    {
        return ONCLI_PARSE_ERR;
    }
    ptr = end_ptr + 1;

    chunk_delay_ms = one_net_strtol(ptr, &end_ptr, 10);
    if(*end_ptr != '\n')
    {
        return ONCLI_PARSE_ERR;
    }

    if(!u16_to_did(raw_did_int, (on_raw_did_t*) raw_did))
    {
        return ONCLI_PARSE_ERR;
    }
    if(on_encode(enc_dst_did, raw_did, ON_ENCODED_DID_LEN) != ONS_SUCCESS)
    {
        return ONCLI_PARSE_ERR;
    }

    if(is_my_did((const on_encoded_did_t*) &enc_dst_did))
    {
        return ONCLI_INVALID_DST;
    }



    bs_msg.dst = (*get_sender_info)((const on_encoded_did_t* const) &enc_dst_did);
    if(!bs_msg.dst)
    {
        return ONCLI_INVALID_DST;
    }

    // We'll use the defaults for most things, but override at least one default
    // with our own parameters.  The functions need parameters, even for
    // values we are throwing away, so we'll let fill them in and throw away.
    // We'll use a few obsolete variables to avoid having to declare new ones
    // and avoid overwriting values where we don't want to.

    // We aren't using the defaults for the chunk delay, so fill it in now and
    // pass the function a variable we're throwing away (chunk_delay_ms).  Thus
    // it is NOT a mistake that we are passing the function several elements of
    // the bs_msg structure, but passing it the local variable chunk_delay_ms.
    // That's because we are not interested in the default value there.
    bs_msg.bs.block.chunk_pause = chunk_delay_ms;

    #ifdef ONE_NET_MASTER
    if(device_is_master)
    {
        on_client_t* dst_client = client_info((const on_encoded_did_t*)&enc_dst_did);
        if(!dst_client)
        {
            return ONCLI_INVALID_DST;
        }

        if(on_master_get_default_block_transfer_values(NULL, dst_client,
          bs_msg.bs.block.transfer_size, &priority, &bs_msg.bs.block.chunk_size,
          &bs_msg.frag_dly, &chunk_delay_ms, &bs_msg.data_rate,
          &bs_msg.channel, &bs_msg.timeout, &ack_nack) != ON_NACK_RSN_NO_ERROR)
        {
            return ONCLI_CMD_FAIL; // TODO -- do some conversions on the NACK
                                   // reasons?
        }
        set_bs_priority(&bs_msg.flags, priority);
        return (on_master_initiate_block_msg(&bs_msg, &ack_nack)
          == ON_NACK_RSN_NO_ERROR) ? ONCLI_SUCCESS : ONCLI_CMD_FAIL;
    }
    #endif
    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        if(on_client_get_default_block_transfer_values((const on_encoded_did_t*) &(bs_msg.dst->did),
          bs_msg.bs.block.transfer_size, &priority, &bs_msg.bs.block.chunk_size,
          &bs_msg.frag_dly, &chunk_delay_ms, &bs_msg.data_rate,
          &bs_msg.channel, &bs_msg.timeout, &ack_nack) != ON_NACK_RSN_NO_ERROR)
        {
            return ONCLI_CMD_FAIL; // TODO -- do some conversions on the NACK
                                   // reasons?
        }
        set_bs_priority(&bs_msg.flags, priority);
        return (on_client_initiate_block_msg(&bs_msg, &ack_nack)
          == ON_NACK_RSN_NO_ERROR) ? ONCLI_SUCCESS : ONCLI_CMD_FAIL;
    }
    #endif

    return ONCLI_SNGH_INTERNAL_ERR;
}
#endif


#ifdef STREAM_MESSAGES_ENABLED
/*!
    \brief Handles receiving the stream command and all its
      parameters.

    The stream command has the form

    stream:did:time_ms


    For example, "stream:006:5000" will send stream characters to device
    006 for 5000 milliseconds

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.

            See oncli_status_t for other errors
*/
static oncli_status_t stream_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    UInt16 raw_did_int;
    on_raw_did_t raw_did;
    UInt8 priority;
    const char* ptr = ASCII_PARAM_LIST;
    char* end_ptr;
    on_ack_nack_t ack_nack;
    on_encoded_did_t enc_dst_did;

    if(bs_msg.transfer_in_progress)
    {
        return ONCLI_RSRC_UNAVAILABLE;
    }

    raw_did_int = one_net_strtol(ptr, &end_ptr, 16);
    if(*end_ptr != ':')
    {
        return ONCLI_PARSE_ERR;
    }
    ptr = end_ptr + 1;
    bs_msg.time = one_net_strtol(ptr, &end_ptr, 10);
    if(*end_ptr != '\n')
    {
        return ONCLI_PARSE_ERR;
    }

    if(!u16_to_did(raw_did_int, (on_raw_did_t*) raw_did))
    {
        return ONCLI_PARSE_ERR;
    }

    if(on_encode(enc_dst_did, raw_did, ON_ENCODED_DID_LEN) != ONS_SUCCESS)
    {
        return ONCLI_PARSE_ERR;
    }

    if(is_my_did((const on_encoded_did_t*) &enc_dst_did))
    {
        return ONCLI_INVALID_DST;
    }

    bs_msg.dst = (*get_sender_info)((const on_encoded_did_t*) &enc_dst_did);
    if(!bs_msg.dst)
    {
        return ONCLI_INVALID_DST;
    }

    #ifdef ONE_NET_MASTER
    if(device_is_master)
    {
        on_client_t* dst_client = client_info((const on_encoded_did_t*) &enc_dst_did);
        if(!dst_client)
        {
            return ONCLI_INVALID_DST;
        }

        if(on_master_get_default_stream_transfer_values(NULL, dst_client,
          bs_msg.time, &priority, &bs_msg.frag_dly, &bs_msg.data_rate,
          &bs_msg.channel, &bs_msg.timeout, &ack_nack) != ON_NACK_RSN_NO_ERROR)
        {
            return ONCLI_CMD_FAIL; // TODO -- do some conversions on the NACK
                                   // reasons?
        }

        set_bs_priority(&bs_msg.flags, priority);
        return on_master_initiate_stream_msg(&bs_msg, &ack_nack)
          == ON_NACK_RSN_NO_ERROR ? ONCLI_SUCCESS : ONCLI_CMD_FAIL;
    }
    #endif
    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        if(on_client_get_default_stream_transfer_values((const on_encoded_did_t*) &(bs_msg.dst->did),
          bs_msg.time, &priority, &bs_msg.frag_dly, &bs_msg.data_rate,
          &bs_msg.channel, &bs_msg.timeout, &ack_nack) != ON_NACK_RSN_NO_ERROR)
        {
            return ONCLI_CMD_FAIL; // TODO -- do some conversions on the NACK
                                   // reasons?
        }

        set_bs_priority(&bs_msg.flags, priority);
        return on_client_initiate_stream_msg(&bs_msg, &ack_nack)
          == ON_NACK_RSN_NO_ERROR ? ONCLI_SUCCESS : ONCLI_CMD_FAIL;
    }
    #endif

    return ONCLI_SNGH_INTERNAL_ERR;
}
#endif


#ifdef ENABLE_CHANGE_KEY_COMMAND
static oncli_status_t change_single_block_key_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    enum
    {
        // number of characters in the key that are grouped together.
        KEY_CH_GROUP_SIZE = 2,

        // The position where the group delimiter character occurs (every nth).
        // This is 1 more than the size of the grouping.
        KEY_DELIMITER_POS,

        // number of ascii characters to convert to a binary stream
        NUM_CH_TO_CONVERT = 2
    };

    one_net_status_t status;

    // delimits the unique key into 4 sets of 4 values.
    const char GROUP_DELIMITER = '-';

    one_net_xtea_key_fragment_t key_fragment;

    UInt8 param_idx, key_idx;

    #ifdef ONE_NET_CLIENT
    if(!device_is_master)
    {
        return ONCLI_INVALID_CMD_FOR_NODE; // master-only command
    }
    #endif

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    // parse the key parameter
    param_idx = 0;
    key_idx = 0;
    while(param_idx < 11)
    {
        if((param_idx % KEY_DELIMITER_POS) == KEY_CH_GROUP_SIZE)
        {
            if(ASCII_PARAM_LIST[param_idx] != GROUP_DELIMITER)
            {
                return ONCLI_PARSE_ERR;
            } // if the character is not the delimiter //

            param_idx++;
        } // if the character should be the delimiter //
        else
        {
            if(ascii_hex_to_byte_stream(&(ASCII_PARAM_LIST[param_idx]),
              &(key_fragment[key_idx++]), NUM_CH_TO_CONVERT)
              != NUM_CH_TO_CONVERT)
            {
                return ONCLI_PARSE_ERR;
            } // if the character is invalid //

             param_idx += NUM_CH_TO_CONVERT;
        } // else it should be a key character //
    } // loop to read in the unique key //

    status = one_net_master_change_key_fragment(key_fragment);
    switch(status)
    {
        case ONS_SUCCESS: return ONCLI_SUCCESS;
        case ONS_ALREADY_IN_PROGRESS: return ONCLI_ALREADY_IN_PROGRESS;
        case ONS_BAD_KEY_FRAGMENT: return ONCLI_BAD_KEY_FRAGMENT;
        default: return ONCLI_CMD_FAIL;
    }
}
#endif


#ifdef ENABLE_CHANNEL_COMMAND
/*!
    \brief Handles receiving the channel command and all its parameters

    The channel command has the form

    channel:LL:NN

    where LL is the locale("US" or "EUR" and NN is the channel number to change to.
    The channel is 1 based(1-25 for US and 1-3 for Europe).  This command is only
    valid in MASTER mode.

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_CMD_FAIL If the command failed.
*/
static oncli_status_t channel_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    oncli_status_t status;
    UInt8 channel;

    if((status = oncli_parse_channel(ASCII_PARAM_LIST, &channel)) !=
      ONCLI_SUCCESS)
    {
        return status;
    } // if parsing the channel was not successful //

    one_net_master_reset_master((const on_raw_sid_t*)one_net_master_get_raw_sid(), channel);
    return ONCLI_SUCCESS;
} // channel_cmd_hdlr //
#endif


#ifdef ENABLE_SETNI_COMMAND
/*!
    \brief Handles receiving the setni command and all its parameters.

    The setni command has the form

    setni:123456789:GGGG-HHHH

    where 123456789 is a valid NID (one of the NID's allocated to ONE-NET evaluation boards).

    where GGGG-HHHH is an invite code. It will be repeated to produce the
    full invite code (GGGG-HHHH-GGGG-HHHH).

    The manufacturing data segment in data flash will contain a full SID (where the
    master DID is appended to the NID).

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_CMD_FAIL If the command failed.
*/
oncli_status_t setni_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    enum
    {
        SETNI_CMD_NID_OFFSET = 0,
        SETNI_CMD_NID_NIBBLES = 9,
        SETNI_CMD_SID_NIBBLES = 12,
        SETNI_CMD_SEPARATOR_OFFSET = 9,
        SETNI_CMD_INVITE_OFFSET = 10,
        SETNI_CMD_INVITE_SEP_OFFSET = 14,
        SETNI_CMD_INVITE_INPUT_LENGTH = 9,
        SETNI_CMD_INVITE_KEY_SEGMENT_LENGTH = 4,
        // total parameter length is SID + delimiter + invite + newline
        SETNI_CMD_TOTAL_LENGTH = SETNI_CMD_NID_NIBBLES+1+SETNI_CMD_INVITE_INPUT_LENGTH+1
    };

    UInt8 i;
    on_raw_sid_t raw_sid;
    one_net_xtea_key_t invite_key;

    // delimits the starting address and length
    const char SETNI_CMD_SEPARATOR = ':';
    const char SID_INVITE_SEPARATOR = '-';

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //
    else if (strlen(ASCII_PARAM_LIST) != SETNI_CMD_TOTAL_LENGTH)
    {
        return ONCLI_PARSE_ERR;
    }
    else if (*(ASCII_PARAM_LIST+SETNI_CMD_SEPARATOR_OFFSET) != SETNI_CMD_SEPARATOR)
    {
        return ONCLI_PARSE_ERR;
    }
    else if (*(ASCII_PARAM_LIST+SETNI_CMD_INVITE_SEP_OFFSET) != SID_INVITE_SEPARATOR)
    {
        return ONCLI_PARSE_ERR;
    }


    //
    // extract the NID from the parameters, convert the NID to binary,
    // extend it to become an SID, and copy it to the manufacturing data segment buffer.
    //
    if(ascii_hex_to_byte_stream(ASCII_PARAM_LIST + SETNI_CMD_NID_OFFSET,
      raw_sid, SETNI_CMD_NID_NIBBLES) != SETNI_CMD_NID_NIBBLES)
    {
        return ONCLI_PARSE_ERR;
    }
    // a raw SID is 6 bytes (nn nn nn nn nd dd), the setni provides the 9 nibble raw NID
    // (nn nn nn nn n). we need to set the d dd portion of the rae SID to 0 01 so the NID
    // supplied becomes a raw SID of nn nn nn nn n0 01 as we save it to flash,
    // since we always want the master DID to be 001.
    raw_sid[ON_RAW_NID_LEN] &= 0xF0;
    raw_sid[ON_RAW_NID_LEN] = 0x01;

    //
    // copy the invite code from the parameter list.  The second half will be the
    // same as the first half, so copy it and make a 16-byte key.
    //
    one_net_memmove(invite_key, ASCII_PARAM_LIST + SETNI_CMD_INVITE_OFFSET,
      ONE_NET_XTEA_KEY_FRAGMENT_SIZE);
    one_net_memmove(&invite_key[ONE_NET_XTEA_KEY_FRAGMENT_SIZE],
      ASCII_PARAM_LIST + SETNI_CMD_INVITE_OFFSET +
      ONE_NET_XTEA_KEY_FRAGMENT_SIZE + 1, ONE_NET_XTEA_KEY_FRAGMENT_SIZE);

    one_net_memmove(&invite_key[ONE_NET_XTEA_KEY_LEN / 2], invite_key,
      ONE_NET_XTEA_KEY_LEN / 2);

    //
    // verify that the invite key characters supplied are valid for an invite key
    //
    for (i = 0; i< ONE_NET_XTEA_KEY_LEN; i++)
    {
        if(!oncli_is_valid_unique_key_ch(invite_key[i]))
        {
            return ONCLI_PARSE_ERR;
        }
    }

    //
    // write the manufacturing data segment buffer to non-volatile memory
    //
    if (one_net_save_mfg_settings((const on_raw_sid_t*) raw_sid,
      (const one_net_xtea_key_t*) invite_key) != ONS_SUCCESS)
    {
        return ONCLI_CMD_FAIL;
    }

    oncli_send_msg("Erasing current settings and resetting the system with "
      "a new NID and invite key.\n");
    #ifdef ONE_NET_MASTER
    if(device_is_master)
    {
        one_net_master_erase_settings();
        one_net_master_reset_master((const on_raw_sid_t*)one_net_master_get_raw_sid(), -1);
    }
    #endif

    // TODO -- what about resetting the client?
    return ONCLI_SUCCESS;
} // setni_cmd_hdlr //
#endif


#ifdef PID_BLOCK
/*!
    \brief Turns on or off pid blocking for debugging purposes.  Sets a
           pid as either ignored or not ignored.

    The "pid block" command has the form

    pid block:on  --> turns pid blocking on
    pid block:off  --> turns pid blocking off
    pid block:display --> Displays criteria
    pid block:accept:FF --> Accept all PIDs
    pid block:reject:FF --> Accept no PIDs
    pid block:accept:01 --> Accept raw pid 0x01
    pid block:reject:01 --> Reject raw pid 0x01
    pid block:accept:sa --> Accept ONLY Stay-Awake packets.
    pid block:reject:sa --> Accept ONLY non-Stay-Awake packets.
    pid block:all:sa -->    Accept packets regardless of Stay-Awake status.
    pid block:accept:mh --> Accept ONLY Multi-Hop packets.
    pid block:reject:mh --> Accept ONLY Multi-Hop packets.
    pid block:all:mh -->    Accept packets regardless of Multi-Hop status.

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_CMD_FAIL If the command failed.
*/
static oncli_status_t pid_block_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    UInt8 raw_pid;
    BOOL turn_on = FALSE;
    BOOL turn_off = FALSE;
    BOOL display = FALSE;
    BOOL sa = FALSE;
    BOOL mh = FALSE;
    BOOL adjust_array = FALSE;
    pid_block_criteria_t crit = PID_ACCEPT;

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    if(!strncmp(PARAM_PTR, ONCLI_ON_STR, strlen(ONCLI_ON_STR)))
    {
        PARAM_PTR += strlen(ONCLI_ON_STR);
        turn_on = TRUE;
    }
    else if(!strncmp(PARAM_PTR, ONCLI_OFF_STR, strlen(ONCLI_OFF_STR)))
    {
        PARAM_PTR += strlen(ONCLI_OFF_STR);
        turn_off = TRUE;
    }
    else if(!strncmp(PARAM_PTR, DISPLAY_STR, strlen(DISPLAY_STR)))
    {
        PARAM_PTR += strlen(DISPLAY_STR);
        display = TRUE;
    }
    else
    {
        if(!strncmp(PARAM_PTR, "accept", strlen("accept")))
        {
            crit = PID_ACCEPT_IF_PRESENT;
            PARAM_PTR += strlen("accept");
        }
        else if(!strncmp(PARAM_PTR, "reject", strlen("reject")))
        {
            crit = PID_REJECT_IF_PRESENT;
            PARAM_PTR += strlen("remove");
        }
        else if(!strncmp(PARAM_PTR, "all", strlen("all")))
        {
            PARAM_PTR += strlen("all");
        }
        else
        {
            return ONCLI_PARSE_ERR;
        }

        if(*PARAM_PTR != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        }
        PARAM_PTR++;

        // See if the next string is "sa" or "mh".  Otherwise it should be a PID
        if(!strncmp(PARAM_PTR, "sa", strlen("sa")))
        {
            sa = TRUE;
        }
        else if(!strncmp(PARAM_PTR, "mh", strlen("mh")))
        {
            mh = TRUE;
        }
        else if(crit == PID_ACCEPT)
        {
            return ONCLI_PARSE_ERR;
        }
        else
        {
            adjust_array = TRUE;
            // read in the pid
            if(ascii_hex_to_byte_stream(PARAM_PTR, &raw_pid, sizeof(raw_pid) * 2)
              != sizeof(raw_pid) * 2)
            {
                return ONCLI_PARSE_ERR;
            }
        } // if converting the did failed //
        PARAM_PTR += (sizeof(raw_pid) * 2);
    }

    if(*PARAM_PTR != '\n')
    {
        return ONCLI_PARSE_ERR;
    }

    if(turn_on || turn_off)
    {
        pid_blocking_on = turn_on;
    }
    else if(display)
    {
        // TODO -- place display code in oncli.h and oncli.c
        UInt8 i;
        UInt16 mask = 0x01;
        const char* const ACCEPT_REJECT_STR[3] = {"Reject", "Accept", "N/A"};

        oncli_send_msg("PID Blocking is %s\n", pid_blocking_on ? ONCLI_ON_STR : ONCLI_OFF_STR);
        for(i = 0; i < 0x0F; i++)
        {
            oncli_send_msg("PID 0x%02X:%s\n", i,
              ACCEPT_REJECT_STR[((mask & pid_block_info.block_pid_list) > 0)]);
            delay_ms(1);
            mask <<= 1;
        }
        oncli_send_msg("Stay Awake:%s\n",
              ACCEPT_REJECT_STR[pid_block_info.sa_block]);
        oncli_send_msg("Multi-Hop:%s\n",
              ACCEPT_REJECT_STR[pid_block_info.mh_block]);
    }
    else if(sa)
    {
        set_pid_block_sa(crit);
    }
    else if(mh)
    {
        set_pid_block_mh(crit);
    }
    else if(adjust_array)
    {
        set_pid_block(raw_pid, (crit == PID_ACCEPT_IF_PRESENT));
    }
    else
    {
        return ONCLI_CMD_FAIL;
    }
    return ONCLI_SUCCESS;
} // pid_block_cmd_hdlr //
#endif


#ifdef RANGE_TESTING
/*!
    \brief Turns on or off range testing for debugging purposes.  Sets a
           device as either within range or out of range of this device

    The "range test" command has the form

    range test:on  --> turns range testing on
    range test:off  --> turns range testing off
    range test:clear --> makes all devices out of range
    range test:display --> Displays in-range DIDs.
    range test:add:003 --> places device with raw DID 003 within range
    range test:remove:003 --> places device with raw DID 003 out of range

    \param ASCII_PARAM_LIST ASCII parameter list.

    \return ONCLI_SUCCESS if the command was succesful
            ONCLI_BAD_PARAM If any of the parameters passed into this function
              are invalid.
            ONCLI_PARSE_ERR If the cli command/parameters are not formatted
              properly.
            ONCLI_CMD_FAIL If the command failed.
*/
static oncli_status_t range_test_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    const char * PARAM_PTR = ASCII_PARAM_LIST;
    on_raw_did_t did;
    on_encoded_did_t enc_did;
    BOOL turn_on = FALSE;
    BOOL turn_off = FALSE;
    BOOL clear = FALSE;
    BOOL display = FALSE;
    BOOL add = FALSE;
    BOOL on;

    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    if(!strncmp(PARAM_PTR, ONCLI_ON_STR, strlen(ONCLI_ON_STR)))
    {
        PARAM_PTR += strlen(ONCLI_ON_STR);
        turn_on = TRUE;
    }
    else if(!strncmp(PARAM_PTR, ONCLI_OFF_STR, strlen(ONCLI_OFF_STR)))
    {
        PARAM_PTR += strlen(ONCLI_OFF_STR);
        turn_off = TRUE;
    }
    else if(!strncmp(PARAM_PTR, CLEAR_STR, strlen(CLEAR_STR)))
    {
        PARAM_PTR += strlen(CLEAR_STR);
        clear = TRUE;
    }
    else if(!strncmp(PARAM_PTR, DISPLAY_STR, strlen(DISPLAY_STR)))
    {
        PARAM_PTR += strlen(DISPLAY_STR);
        display = TRUE;
    }
    else
    {
        if(!strncmp(PARAM_PTR, ADD_STR, strlen(ADD_STR)))
        {
            add = TRUE;
            PARAM_PTR += strlen(ADD_STR);
        }
        else if(!strncmp(PARAM_PTR, REMOVE_STR, strlen(REMOVE_STR)))
        {
            PARAM_PTR += strlen(REMOVE_STR);
        }
        else
        {
            return ONCLI_PARSE_ERR;
        }

        if(*PARAM_PTR != ONCLI_PARAM_DELIMITER)
        {
            return ONCLI_PARSE_ERR;
        }
        PARAM_PTR++;

        // read in the did
        if(ascii_hex_to_byte_stream(PARAM_PTR, did, ONCLI_ASCII_RAW_DID_SIZE)
          != ONCLI_ASCII_RAW_DID_SIZE)
        {
            return ONCLI_PARSE_ERR;
        } // if converting the source peer did failed //
        PARAM_PTR += ONCLI_ASCII_RAW_DID_SIZE;

        if(on_encode(enc_did, did, ON_ENCODED_DID_LEN) != ONS_SUCCESS)
        {
            return ONCLI_PARSE_ERR;
        }
    }

    if(*PARAM_PTR != '\n')
    {
        return ONCLI_PARSE_ERR;
    }

    if(turn_on || turn_off)
    {
        enable_range_testing(turn_on);
        return ONCLI_SUCCESS;
    }
    else if(clear)
    {
        reset_range_test_did_array();
        return ONCLI_SUCCESS;
    }
    else if(display)
    {
        UInt8 i;
        on_encoded_did_t enc_did_array[RANGE_TESTING_ARRAY_SIZE];
        UInt8 num_in_range = RANGE_TESTING_ARRAY_SIZE;
        if(!devices_within_range(enc_did_array, &num_in_range, &on))
        {
            oncli_send_msg("In-Range Device List Unretrievable.\n");
            return ONCLI_CMD_FAIL;
        }

        oncli_send_msg("Range Testing is %s : # of In-Range Devices : %d\n",
          (on ? ONCLI_ON_STR : ONCLI_OFF_STR), num_in_range);

        for(i = 0; i < num_in_range; i++)
        {
            oncli_send_msg("Encoded DID : %02X%02X -- Raw ",
              enc_did_array[i][0], enc_did_array[i][1]);
            oncli_print_did((const on_encoded_did_t*) enc_did_array[i]);
            oncli_send_msg("\n");
        }
        return ONCLI_SUCCESS;
    }


    if(adjust_range_test_did_array(&enc_did, add))
    {
        return ONCLI_SUCCESS;
    }

    return ONCLI_CMD_FAIL;
} // range_test_cmd_hdlr //
#endif


#ifdef ENABLE_ROUTE_COMMAND
static oncli_status_t route_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    on_raw_did_t raw_did;
    on_encoded_did_t enc_did;

    const char * PARAM_PTR = ASCII_PARAM_LIST;

    // read in the did
    if(ascii_hex_to_byte_stream(PARAM_PTR, raw_did, ONCLI_ASCII_RAW_DID_SIZE)
      != ONCLI_ASCII_RAW_DID_SIZE)
    {
        return ONCLI_PARSE_ERR;
    } // if converting the source peer did failed //
    PARAM_PTR += ONCLI_ASCII_RAW_DID_SIZE;

    if(on_encode(enc_did, raw_did, ON_ENCODED_DID_LEN) != ONS_SUCCESS)
    {
        return ONCLI_PARSE_ERR;
    }

    if(*PARAM_PTR != '\n')
    {
        return ONCLI_PARSE_ERR;
    }

    switch(send_route_msg((const on_raw_did_t* const) raw_did))
    {
        case ONS_SUCCESS: return ONCLI_SUCCESS;
        default: return ONCLI_CMD_FAIL;
    }
}
#endif


#ifdef DEBUGGING_TOOLS
/*!
    \brief Loads memory into the stored memory location.

    This function takes a hex string representing bytes, converts it to a
    byte stream, then copies it into the stored memory location.

    \param[in] ASCII_PARAM_LIST String containing the memory to load

    \return ONCLI_SUCCESS if the parsing and copying of the string succeeded.
            ONCLI_CDM_FAIL otherwise
*/
static oncli_status_t memload_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    enum
    {
        MAX_LOAD_LEN = 50,
        MAX_ASCII_CHAR = MAX_LOAD_LEN * 2
    };

    UInt8 byte_stream[MAX_LOAD_LEN];
    UInt16 num_ascii_char = 0;
    UInt16 actual_len = 0;
    UInt16 num_convert;
    UInt16 max_len = MAX_LOAD_LEN > memory_len ? memory_len : MAX_LOAD_LEN;

    const char* ptr = ASCII_PARAM_LIST;
    while(num_ascii_char < MAX_ASCII_CHAR && *ptr != '\n')
    {
        num_ascii_char++;
        ptr++;
    }

    num_convert = ascii_hex_to_byte_stream(ASCII_PARAM_LIST, byte_stream,
        num_ascii_char);

    actual_len = max_len >= (num_convert / 2) ? (num_convert / 2) : max_len;

    if(actual_len == 0)
    {
        return ONCLI_PARSE_ERR;
    }

    one_net_memmove(memory_ptr, byte_stream, actual_len);
    oncli_send_msg("Copied %d bytes.\n", actual_len);
    return ONCLI_SUCCESS;
} // memload_cmd_hdlr //


/*!
    \brief Similar to memset in the C specification

    memset:18:A6 will store 0xA6 into the stored memory location.  For
         example, if the stored memory location was 0x5670, after this
         command, addresses 0x5670 through 0x5681 would contain the value
         0xA6.

    \param[in] ASCII_PARAM_LIST String containing the number of bytes to
         set and the value to set them to.

    \return ONCLI_SUCCESS if the parsing and setting of the value succeeded.
            ONCLI_CDM_FAIL otherwise
*/
static oncli_status_t memset_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    char* ptr = (char*) ASCII_PARAM_LIST;
    char* end_ptr;
    int num_bytes = 0;
    UInt8 fill_byte_digit, fill_byte;
    UInt16 i;

    num_bytes = one_net_strtol(ptr, &end_ptr, 0);
    if(memory_ptr == NULL || !end_ptr || end_ptr == ptr || num_bytes == 0)
    {
        return ONCLI_PARSE_ERR; // error
    } // if parsing the data failed //
    ptr = end_ptr;

    if(*ptr != ':')
    {
        return ONCLI_PARSE_ERR; // error
    }
    ptr++;

    fill_byte_digit = ascii_hex_to_nibble(*ptr);
    if(fill_byte_digit > 0x0F)
    {
       return ONCLI_PARSE_ERR;
    } // if the conversion failed //
    ptr++;
    fill_byte = fill_byte_digit << 4;

    fill_byte_digit = ascii_hex_to_nibble(*ptr);
    if(fill_byte_digit > 0x0F)
    {
       return ONCLI_PARSE_ERR;
    } // if the conversion failed //

    fill_byte |= fill_byte_digit;

    ptr = memory_ptr;
    for(i = 0; i < num_bytes; i++)
    {
        *ptr = fill_byte;
        ptr++;
    }

    return ONCLI_SUCCESS;
} // memset_cmd_hdlr //


/*!
    \brief Displays the contents of the stored memory location in hex.

    \return ONCLI_SUCCESS if there is a valid stored memory location
            ONCLI_CMD_FAIL otherwise.
*/
static oncli_status_t memdump_cmd_hdlr(void)
{
    if(memory_ptr == NULL || memory_len <= 0)
    {
        return ONCLI_CMD_FAIL;
    }

    oncli_send_msg("Ratchet = %s\n", ratchet ? ONCLI_ON_STR : ONCLI_OFF_STR);
    oncli_send_msg("Proceed = %s\n", proceed ? ONCLI_ON_STR : ONCLI_OFF_STR);
    oncli_send_msg("Pause = %s\n", pause ? ONCLI_ON_STR : ONCLI_OFF_STR);
    oncli_send_msg("Pausing = %s\n", pausing ? ONCLI_ON_STR : ONCLI_OFF_STR);
    oncli_send_msg("\n\n");
    delay_ms(25);
    print_timers();
    delay_ms(25);
    print_intervals();
    oncli_send_msg("\n\n");
    delay_ms(25);

    xdump(memory_ptr, memory_len);
    return ONCLI_SUCCESS;
}


/*!
    \brief Returns a certain segment of memory given parameeters.


    \param[out] mem_ptr The address of the memory requested.
    \param[in] memory_type The type of mempory requested.
    \param[in] index If the memory type is an array, the element requested
    \param[in] offset The offset(in bytes) requested from the start of the
                 memory of element in question.  Must be non-negative.

    \return the number of bytes of relevant memory.  -1 upon error
*/
static int get_memory_loc(UInt8** mem_ptr, debug_memory_t memory_type,
  int index, int offset)
{
    int len = 0;

    if(!mem_ptr)
    {
        return 0;
    }

    *mem_ptr = NULL;

    switch(memory_type)
    {
        case DEBUG_MEMORY_ON_STATE:
            *mem_ptr = (UInt8*) &on_state;
            len = sizeof(on_state_t);
            break;
        case DEBUG_MEMORY_TIMER:
            *mem_ptr = (UInt8*) &timer[0];
            if(index < 0)
            {
                len = ONT_NUM_TIMERS * sizeof(ont_timer_t);
            }
            else if(index < ONT_NUM_TIMERS)
            {
                len = sizeof(ont_timer_t);
                *mem_ptr = (UInt8*) &timer[index];
            }
            else
            {
                return -1; // error
            }
            break;
        #ifdef ONE_NET_CLIENT
        case DEBUG_MEMORY_SEND_LIST:
            *mem_ptr = (UInt8*) &sending_dev_list[0];
            if(index < 0)
            {
                len = ONE_NET_RX_FROM_DEVICE_COUNT *
                  sizeof(on_sending_dev_list_item_t);
            }
            else if(index < ONE_NET_RX_FROM_DEVICE_COUNT)
            {
                len = sizeof(ont_timer_t);
                *mem_ptr = (UInt8*) &sending_dev_list[index];
            }
            else
            {
                return -1; // error
            }
            break;
        case DEBUG_MEMORY_MASTER:
            *mem_ptr = (UInt8*) master;
            len = sizeof(on_master_t);
            break;
        #endif

        #ifdef PEER
        case DEBUG_MEMORY_PEER:
            *mem_ptr = (UInt8*) &peer[0];
            if(index < 0)
            {
                len = ONE_NET_MAX_PEER_UNIT * sizeof(on_peer_unit_t);
            }
            else if(index < ONE_NET_MAX_PEER_UNIT)
            {
                len = sizeof(on_peer_unit_t);
                *mem_ptr = (UInt8*) &peer[index];
            }
            else
            {
                return -1; // error
            }

            break;
        #endif
        case DEBUG_MEMORY_BASE_PARAM:
            *mem_ptr = (UInt8*) on_base_param;
            len = sizeof(on_base_param_t);
            break;
        case DEBUG_MEMORY_INVITE_TXN:
            *mem_ptr = (UInt8*) &invite_txn;
            len = sizeof(on_txn_t);
            break;
        case DEBUG_MEMORY_RESPONSE_TXN:
            *mem_ptr = (UInt8*) &response_txn;
            len = sizeof(on_txn_t);
            break;
        case DEBUG_MEMORY_SINGLE_TXN:
            *mem_ptr = (UInt8*) &single_txn;
            len = sizeof(on_txn_t);
            break;
        #ifdef BLOCK_MESSAGES_ENABLED
        case DEBUG_MEMORY_BS_TXN:
            *mem_ptr = (UInt8*) &bs_txn;
            len = sizeof(on_txn_t);
            break;
        case DEBUG_MEMORY_BS_MSG:
            *mem_ptr = (UInt8*) &bs_msg;
            len = sizeof(block_stream_msg_t);
            break;
        #endif
        #ifdef ONE_NET_MASTER
        case DEBUG_MEMORY_CLIENT_LIST:
            *mem_ptr = (UInt8*) &client_list[0];
            if(index < 0)
            {
                len = ONE_NET_MASTER_MAX_CLIENTS * sizeof(on_client_t);
            }
            else if(index < ONE_NET_MASTER_MAX_CLIENTS)
            {
                len = sizeof(on_client_t);
                *mem_ptr = (UInt8*) &client_list[index];
            }
            else
            {
                return -1; // error
            }
            break;
        case DEBUG_MEMORY_MASTER_PARAM:
            *mem_ptr = (UInt8*) master_param;
            len = sizeof(on_master_param_t);
            break;
        #endif
        default:
            return -1; // error
    }


    if(offset < 0 || offset >= len)
    {
        return -1; // error
    }

    (*mem_ptr) += offset;
    return len - offset;
}


/*!
    \brief Parses a string and returns a certain segment of memory
      given the string parameeters.


    \param[out] mem_ptr The address of the memory requested.
    \param[in] ASCII_PARAM_LIST The string to parse for parameters.


    \return the number of bytes of relevant memory.  -1 upon error
*/
static int parse_memory_str(UInt8** mem_ptr,
  const char * const ASCII_PARAM_LIST)
{
    int i, offset;
    debug_memory_t memory_type = DEBUG_MEMORY_COUNT;
    const char* ptr = ASCII_PARAM_LIST;
    char* end_ptr;
    BOOL has_index; // true for memory that is an array, false otherwise
    int index = -1; // relevant only if has_index is true

    if(!mem_ptr || !ASCII_PARAM_LIST)
    {
        return -1; // error
    }

    offset = 0;

    for(i = 0; i < DEBUG_MEMORY_COUNT; i++)
    {
        if(!strncmp(debug_memory_str[i], ptr, strlen(debug_memory_str[i])))
        {
            memory_type = i;
            ptr += strlen(debug_memory_str[i]);
            break;
        }
    }

    switch(memory_type)
    {
        case DEBUG_MEMORY_TIMER:
        #ifdef PEER
        case DEBUG_MEMORY_PEER:
        #endif
        #ifdef ONE_NET_MASTER
        case DEBUG_MEMORY_CLIENT_LIST:
        #endif
            has_index = TRUE; // these are arrays.
            break;
        case DEBUG_MEMORY_COUNT:
            return -1;  // invalid string.
        default:
            has_index = FALSE; // not an array.
    }

    // look for an index, if relevant
    if(has_index && ((*ptr) == ':'))
    {
        ptr++;
        index = one_net_strtol(ptr, &end_ptr, 0);
        if(!end_ptr || end_ptr == ptr)
        {
            return -1; // error
        } // if parsing the data failed //
        ptr = end_ptr;
    }

    // now look for an offset, if any.
    if((*ptr) == ':')
    {
        ptr++;
        offset = one_net_strtol(ptr, &end_ptr, 0);
        if(!end_ptr || end_ptr == ptr)
        {
            return -1; // error
        } // if parsing the data failed //
        ptr = end_ptr;
    }

    if((*ptr) != '\n')
    {
        return -1; // error
    }

    return get_memory_loc(mem_ptr, memory_type, index, offset);
}


extern UInt8 csdf;
static oncli_status_t csdf_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    tick_t old_tick_count = get_tick_count();
    long new_csdf;
    const char* ptr = ASCII_PARAM_LIST;
    const char* end_ptr = ptr;
    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_PARSE_ERR;
    }

    new_csdf = one_net_strtol(ptr, (char **)&end_ptr, 0);

    if(!end_ptr || end_ptr == ptr || *end_ptr != '\n' ||
        new_csdf < 1 || new_csdf > 255)
    {
        return ONCLI_PARSE_ERR;
    } // if parsing the data failed //


    csdf = (UInt8) new_csdf;
    set_tick_count(old_tick_count);
    oncli_send_msg("csdf=%u write_pause=%lu tick_count=%lu\n", csdf, write_pause, get_tick_count());
    return ONCLI_SUCCESS;
}


/*!
    \brief Parses a string and sets the location and length of the memory
      pointed to it in global variables


    \param[in] ASCII_PARAM_LIST The string to parse for parameters.


    \return ONCLI_SUCCESS if the memory string is valid, ONCLI_PARSE_ERR
      otherwise.
*/
static oncli_status_t memory_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    UInt8* mem_ptr;
    int len = parse_memory_str(&mem_ptr, ASCII_PARAM_LIST);
    if(len < 1)
    {
        return ONCLI_PARSE_ERR;
    }

    oncli_send_msg("Address = (%p) : Length = %d\n", mem_ptr, len);
    memory_ptr = mem_ptr;
    memory_len = len;
    return ONCLI_SUCCESS;
}




static oncli_status_t pause_cmd_hdlr(void)
{
    pause = !pause;
    return ONCLI_SUCCESS;
}


static oncli_status_t proceed_cmd_hdlr(void)
{
    proceed = TRUE;
    return ONCLI_SUCCESS;
}


static oncli_status_t ratchet_cmd_hdlr(void)
{
    ratchet = !ratchet;
    return ONCLI_SUCCESS;
}


/*!
    \brief Sets an interval.  See the debug_intervals[] array
           for a list of settable intervals.

    interval:1:25000 would set interval 1 to 25,000 ms(again, see the
             debug_intevals[] array for a list of intervals and their indexes.

    \param[in] ASCII_PARAM_LIST The string to parse for parameters.

    \return ONCLI_SUCCESS if the interval was set.
            ONCLI_PARSE_ERR otherwise.
*/
static oncli_status_t interval_cmd_hdlr(const char * const ASCII_PARAM_LIST)
{
    UInt8 interval_index;
    tick_t new_interval;
    const char* ptr = ASCII_PARAM_LIST;
    const char* end_ptr = ptr;
    if(!ASCII_PARAM_LIST)
    {
        return ONCLI_PARSE_ERR;
    }

    interval_index = one_net_strtol(ptr, (char **)&end_ptr, 0);

    if(!end_ptr || end_ptr == ptr || *end_ptr != ':' ||
        interval_index >= NUM_DEBUG_INTERVALS)
    {
        return ONCLI_PARSE_ERR;
    } // if parsing the data failed //

    ptr = end_ptr;
    ptr++;

    new_interval = one_net_strtol(ptr, (char **)&end_ptr, 0);

    if(!end_ptr || end_ptr == ptr || *end_ptr != '\n')
    {
        return ONCLI_PARSE_ERR;
    } // if parsing the data failed //

    *(debug_intervals[interval_index]) = new_interval;
    return ONCLI_SUCCESS;
}
#endif



/*!
    \brief Converts a string of ASCII hex digits to a byte stream.

    \param[in] STR The ASCII string of hex digits.
    \param[out] byte_stream The byte stream that results from STR
    \param[in] NUM_ASCII_CHAR The number of ascii characters to convert.  This
      is really twice the number of bytes that were converted.

    \return The number of ASCII characters that were converted.
*/
static UInt16 ascii_hex_to_byte_stream(const char * STR, UInt8 * byte_stream,
  const UInt16 NUM_ASCII_CHAR)
{
    UInt16 num_converted;

    UInt8 hex;

    if(!STR || !byte_stream || !NUM_ASCII_CHAR)
    {
        return 0;
    } // if any of the parameters are invalid //

    for(num_converted = 0; num_converted < NUM_ASCII_CHAR; num_converted++)
    {
        hex = ascii_hex_to_nibble(STR[num_converted]);
        if(hex > 0x0F)
        {
            break;
        } // if the conversion failed //

        if(num_converted & 0x01)
        {
            byte_stream[num_converted >> 1] |= hex;
        } // if the second nibble in the byte //
        else
        {
            byte_stream[num_converted >> 1] = hex << 4;
        } // else the first nibble in the byte //
    } // loop to convert payload from ascii //

    return num_converted;
} // ascii_hex_to_byte_stream //


/*!
    \brief Parses the channel data from the oncli and returns the adjust channel
      number based on the data read in.

    \param[in] ASCII The ascii parameters to be parsed.
    \param[out] channel The adjusted channel.

    \return ONCLI_SUCCESS If the parameters were successfully parsed.
            ONCLI_BAD_PARAM If any of the parameter are invalid
            ONCLI_PARSE_ERR If ASCII could not be parsed
            ONCLI_INTERNAL_ERR If something unexpected occured.
*/
static oncli_status_t oncli_parse_channel(const char * ASCII, UInt8 * const channel)
{
    enum
    {
        ONCLI_US,                   //! US region
        ONCLI_EUR                   //! European region
    };

    char * END_PTR = 0;

    UInt8 region;

    if(!ASCII || !channel)
    {
        return ONCLI_BAD_PARAM;
    } // if the parameter is invalid //

    // get the region
#ifdef US_CHANNELS
    if(!strncmp(ASCII, ONCLI_US_STR, strlen(ONCLI_US_STR)))
    {
        region = ONCLI_US;
        ASCII += strlen(ONCLI_US_STR);
    } // if it's a US frequency //
#endif
#ifdef EUROPE_CHANNELS
#ifdef US_CHANNELS
    else if(!strncmp(ASCII, ONCLI_EUR_STR, strlen(ONCLI_EUR_STR)))
#else
    if(!strncmp(ASCII, ONCLI_EUR_STR, strlen(ONCLI_EUR_STR)))
#endif
    {
        region = ONCLI_EUR;
        ASCII += strlen(ONCLI_EUR_STR);
    } // else if it's a European frequency //
#endif
    else
    {
        return ONCLI_PARSE_ERR;
    } // else the priority is invalid //

    if(*ASCII != ONCLI_PARAM_DELIMITER)
    {
        return ONCLI_PARSE_ERR;
    } // if the command isn't formatted properly //
    ASCII++;

    // Get the channel number
    if(!isdigit(*ASCII))
    {
        return ONCLI_PARSE_ERR;
    } // if the data is not formatted correctly //

    *channel = one_net_strtol(ASCII, &END_PTR, 0) - 1;
    if(!END_PTR || END_PTR == ASCII || (*END_PTR != '\n'))
    {
        return ONCLI_PARSE_ERR;
    } // if the parameter was not valid //

    switch(region)
    {
#ifdef US_CHANNELS
        case ONCLI_US:
        {
            if((SInt8)(*channel) < (SInt8)ONE_NET_MIN_US_CHANNEL
              || *channel > ONE_NET_MAX_US_CHANNEL)
            {
                return ONCLI_PARSE_ERR;
            } // if the parameter is invalid //
            break;
        } // US case //
#endif
#ifdef EUROPE_CHANNELS
        case ONCLI_EUR:
        {
            #ifdef US_CHANNELS
            *channel += ONE_NET_EUR_CHANNEL_1;
            #endif
			// typecast to override "comparison is always false" warning
            if((SInt8) *channel < (SInt8)ONE_NET_MIN_EUR_CHANNEL
              || *channel > ONE_NET_MAX_EUR_CHANNEL)
            {
                return ONCLI_PARSE_ERR;
            } // if the parameter is invalid //
            break;
        } // European case //
#endif
        default:
        {
            return ONCLI_INTERNAL_ERR;
            break;
        } // default case //
    } // switch(region) //

    return ONCLI_SUCCESS;
} // oncli_parse_channel //


/*!
    \brief Parses the key (or portion of the key) from the ASCII parameter.

    ASCII contains two groups of four chars: xxxx-xxxx.
    These are duplicated in the upper two groups. So, for example if this
    function is passed "eval-0006" as the ASCII invite key

    the key is created as a 16 byte ASCII conversion of "eval0006eval0006"


    \param[in] ASCII The ASCII representation of the key
    \param[out] end_ptr Pointer to the character AFTER the key
    \param[out] key They key that ASCII represents.

    \return ONCLI_SUCCESS If the key was successfully parsed
            ONCLI_BAD_PARAM if any of the parameters are invalid
            ONCLI_PARSE_ERR If the ASCII parameter was not formatted correctly.
*/
#ifdef ENABLE_INVITE_COMMAND
static oncli_status_t parse_invite_key(const char * ASCII,
  char** end_ptr, one_net_xtea_key_t * const key)
{
    enum
    {
        END_OF_PARAM_IDX = 9, // now it requires xxxx-xxxx

        // number of characters in the key that are grouped together.
        KEY_CH_GROUP_SIZE = 4,

        // The position where the group delimiter character occurs (every nth).
        // This is 1 more than the size of the grouping.
        KEY_DELIMITER_POS
    };

    // delimits the unique key into 2 sets of 4 values.
    const char GROUP_DELIMITER = '-';

    UInt8 param_idx, key_idx;

    if(!ASCII || !key || !end_ptr)
    {
        return ONCLI_BAD_PARAM;
    } // if any of the parameters are invalid //

    *end_ptr = (char*) (ASCII + END_OF_PARAM_IDX);

    // parse the key parameter
    for(param_idx = 0, key_idx = 0; param_idx < END_OF_PARAM_IDX; param_idx++)
    {
        if((param_idx % KEY_DELIMITER_POS) == KEY_CH_GROUP_SIZE)
        {
            if(ASCII[param_idx] != GROUP_DELIMITER)
            {
                return ONCLI_PARSE_ERR;
            } // if the character is not the delimiter //
        } // if the character should be the delimiter //
        else
        {
            if(!oncli_is_valid_unique_key_ch(ASCII[param_idx]))
            {
                return ONCLI_PARSE_ERR;
            } // if the character is invalid //

            (*key)[key_idx+8] = ASCII[param_idx]; // group is duplicated
            (*key)[key_idx++] = ASCII[param_idx];
        } // else it should be a key character //
    } // loop to read in the unique key //

    return ONCLI_SUCCESS;
} // parse_invite_key //
#endif


#ifdef ENABLE_SINGLE_COMMAND
/*!
    \brief Parses the ASCII parameter string for the parameters needed to send
      a transaction.

    Some parameters to be parsed are optional. That is, if the pointer to an
    ouput paramter is NULL (zero), then that paramter will not be parsed.
    This variation was added when we found that we needed to modify the
    syntax of the "single" command after changing from 8 bit to 4 bit
    unit numbers. The new syntax of the "single" command no longer includes
    the source and destination unit numbers. Rather than writing a new
    parse function, we decided to make the src_unit and dst_unit parameters
    optional.

    Entering 000 for the DID when send_to_peer_list is not NULL results in
    the BOOL pointed to by send_to_peer_list being set to TRUE.

    \param[in] PARAM_PTR The ASCII parameter list.
    \param[out] src_unit The unit in the source device sending the message or NULL
    if there is no src_unit to be parsed.
    \param[out] dst_unit The unit in the destination device the message i or NULL
    if there is no dst_unit to be parsed.
      intended for.
    \param[out] enc_dst The encoded destination device.
    \param[out] send_to_peer_list True if sending to peer list, false otherwise
    \return If successful, pointer to first characer after the last parameter
      that was parsed, or 0 if parsing was not successful.
*/
#ifdef PEER
static const char * parse_ascii_tx_param(const char * PARAM_PTR,
  UInt8 * const src_unit, UInt8 * const dst_unit,
  on_encoded_did_t * const enc_dst, BOOL* send_to_peer_list)
#else
static const char * parse_ascii_tx_param(const char * PARAM_PTR,
  UInt8 * const src_unit, UInt8 * const dst_unit,
  on_encoded_did_t * const enc_dst)
#endif
{
    char * end_ptr;
    on_raw_did_t raw_did;

    if(!PARAM_PTR || !enc_dst)
    {
        return 0;
    } // if any of the parameters are invalid //

    if (src_unit)
    {
        // read the src unit
        *src_unit = one_net_strtol(PARAM_PTR, &end_ptr, 16);
        if(!end_ptr || end_ptr == PARAM_PTR)
        {
            return 0;
        } // if the parameter was not valid //
        PARAM_PTR = end_ptr;

        // check the parameter delimiter
        if(*PARAM_PTR != ONCLI_PARAM_DELIMITER)
        {
            return 0;
        } // if malformed parameter //
        PARAM_PTR++;
    }

    if (dst_unit)
    {
        // read the dst unit
        *dst_unit = one_net_strtol(PARAM_PTR, &end_ptr, 16);
        if(!end_ptr || end_ptr == PARAM_PTR)
        {
            return 0;
        } // if the parameter was not valid //
        PARAM_PTR = end_ptr;

        // check the parameter delimiter
        if(*PARAM_PTR != ONCLI_PARAM_DELIMITER)
        {
            return 0;
        } // if malformed parameter //
        PARAM_PTR++;
    }

    // read in the raw did
    if(*PARAM_PTR != ONCLI_PARAM_DELIMITER)
    {
        #ifdef PEER
        *send_to_peer_list = FALSE;
        #endif
        if(ascii_hex_to_byte_stream(PARAM_PTR, raw_did, ONCLI_ASCII_RAW_DID_SIZE)
          != ONCLI_ASCII_RAW_DID_SIZE)
        {
            return 0;
        } // if converting the raw destination did failed //

        // make sure the raw did is skipped
        PARAM_PTR += ONCLI_ASCII_RAW_DID_SIZE;

        if(on_encode(*enc_dst, raw_did, ON_ENCODED_DID_LEN) != ONS_SUCCESS)
        {
            return 0;
        }

        #ifdef PEER
        if(did_to_u16((const on_raw_did_t*) &raw_did) == 0)
        {
            *send_to_peer_list = TRUE;
        }
        #endif
    } // if the did is present //
    else
    {
        #ifdef PEER
        *send_to_peer_list = TRUE;
        #endif
        return 0;
    } // else the did is not present //

    return PARAM_PTR;
} // parse_ascii_tx_param //


/*!
    \brief Parses the ASCII form of data to send in a transaction.

    Converts the ASCII data to a byte stream.  ASCII must be twice as big
    as data since it takes 2 ASCII bytes for every byte that will be sent.

    \param[in] ASCII The ASCII data to convert.
    \param[out] data The binary version of the data in ASCII.
    \param[in/out] On input, the size of data.  On output, the number of bytes
      contained in data.

    \return If successful, a pointer to the first byte beyond the data that was
      converted, or 0 if failed to convert the data.
*/
static const char * parse_ascii_tx_data(const char * ASCII, UInt8 * data,
  UInt16 * data_len)
{
    if(!ASCII || !data || !data_len || !*data_len)
    {
        return 0;
    } // if any of the parameters are invalid //

    if((*data_len  = ascii_hex_to_byte_stream(ASCII, data, *data_len << 1))
      & 0x01)
    {
        // did not parse an even number of ASCII bytes, meaning the last byte
        // was not complete.
        *data_len = 0;
        return 0;
    } // if converted an odd number of characters //

    ASCII += *data_len;

    // ascii_hex_to_byte_stream returns the number of ASCII characters that
    // were converted, so divide the result by 2 to get the actual number
    // of bytes in the stream.
    *data_len >>= 1;

    return ASCII;
} // parse_ascii_tx_data //


/*!
    \brief Parses the ASCII text to send in the transaction.

    The ASCII string passed in must contain the '"''s delimiting the start and
    end of the text.

    \param[in] ASCII The text to parse.
    \param[out] data The resulting data.
    \param[in/out] data_len On input, the size of data.  On output, the number
      of bytes that were parsed into data.

    \return If successful, a pointer to the first byte beyond the data that was
      converted, or 0 if failed to convert the data.
*/
static const char * parse_ascii_tx_text_data(const char * ASCII, UInt8 * data,
  UInt16 * data_len)
{
    const char TEXT_DELIMITER = '"';

    UInt16 num_ch = 0;

    if(!ASCII || !data || !data_len || !*data_len)
    {
        return 0;
    } // if any of the parameters are invalid //

    if(*ASCII++ != TEXT_DELIMITER)
    {
        return FALSE;
    } // if the string is invalid //

    while(*ASCII != TEXT_DELIMITER && num_ch < *data_len)
    {
        data[num_ch++] = *ASCII++;
    } // while more text to read, and more space to store it in //

    if(*ASCII++ != TEXT_DELIMITER)
    {
        *data_len = 0;
        return 0;
    } // if the text data was invalid //

    *data_len = num_ch;
    return ASCII;
} // parse_ascii_tx_text_data //
#endif



//! @} oncli_hdlr_pri_func
//						PRIVATE FUNCTION IMPLEMENTATION END
//==============================================================================

//! @} oncli_hdlr


#endif // #ifdef UART //
