#ifndef ONE_NET_MASTER_H
#define ONE_NET_MASTER_H

#include "config_options.h"

#ifdef ONE_NET_MASTER

#include "one_net.h"



//! \defgroup ONE-NET_MASTER ONE-NET MASTER device functionality
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
    \file one_net_master.h
    \brief ONE-NET MASTER functionality declarations.

    Derives from ONE-NET.  MASTER dependent functionality.

    \note See one_net.h for the version of the ONE-NET source as a whole.  If
      any one file is modified, the version number in one_net.h will need to be
      updated.
*/



//==============================================================================
//                                  CONSTANTS
//! \defgroup ONE-NET_MASTER_const
//! \ingroup ONE-NET_MASTER
//! @{


//! The various MAC settings that can be updated.  These are used to report the
//! results of changing a devices parameters.
typedef enum
{
    //! Update a device's data rate
    ONE_NET_UPDATE_DATA_RATE = 0x00,
    
    //! Updating the network key is complete
    ONE_NET_UPDATE_NETWORK_KEY = 0x01,

    #ifdef PEER
    //! Assigning a peer
    ONE_NET_UPDATE_ASSIGN_PEER = 0x02,

    //! Unassigning a peer
    ONE_NET_UPDATE_UNASSIGN_PEER = 0x03,
    #endif

    #ifdef BLOCK_MESSAGES_ENABLED
    //! Fragment delays update
    ONE_NET_UPDATE_FRAGMENT_DELAY = 0x04,
    #endif

    //! Updates the keep alive interval for a device
    ONE_NET_UPDATE_KEEP_ALIVE = 0x05,

    //! Indicates an attempt to remove a device from the network
    ONE_NET_UPDATE_REMOVE_DEVICE = 0x06,
    
    ONE_NET_UPDATE_SETTINGS = 0x07,

    //! Indicates an attempt to add a device to the network    
    ONE_NET_UPDATE_ADD_DEVICE = 0x08,

    //! This is to mark nothing was updated.  This item should ALWAYS be
    //! LAST IN THE LIST
    ONE_NET_UPDATE_NOTHING = 0x09
} one_net_mac_update_t;


enum
{
    //! The initial CLIENT DID.  Since a DID is only 12 bits, only the upper 12
    //! bits of this value is used.  Note the order this is stored in memory may
    //! not be the proper order for being sent.
    ONE_NET_INITIAL_CLIENT_DID = 0x0020
};


//! Value to increment the next CLIENT did by each time a device is added
//! to the network
#define ON_CLIENT_DID_INCREMENT 0x0010



//! @} ONE-NET_MASTER_const
//                                  CONSTANTS END
//==============================================================================

//==============================================================================
//                                  TYPEDEFS
//! \defgroup ONE-NET_MASTER_typedefs
//! \ingroup ONE-NET_MASTER
//! @{



//! @} ONE-NET_MASTER_typedefs
//                                  TYPEDEFS END
//==============================================================================

//==============================================================================
//                              PUBLIC VARIABLES
//! \defgroup ONE-NET_MASTER_pub_var
//! \ingroup ONE-NET_MASTER
//! @{


extern on_master_param_t * const master_param;
extern on_client_t * const client_list;
extern one_net_xtea_key_t invite_key;



//! @} ONE-NET_MASTER_pub_var
//                              PUBLIC VARIABLES END
//==============================================================================

//==============================================================================
//                      PUBLIC FUNCTION DECLARATIONS
//! \defgroup ONE-NET_MASTER_pub_func
//! \ingroup ONE-NET_MASTER
//! @{



one_net_status_t one_net_master_create_network(
  const on_raw_sid_t * const SID, const one_net_xtea_key_t * const KEY);
void one_net_master_clear_client_memory(void);
void one_net_master_condense_client_memory(void);
#ifndef PEER
one_net_status_t one_net_master_init(const UInt8 * PARAM, UInt16 PARAM_LEN);
#else
one_net_status_t one_net_master_init(const UInt8 * PARAM, UInt16 PARAM_LEN,
  memory_type_t memory_type);
#endif



/*!
    \brief Changes the key.

    The key is changed by removing the most significant 32 bits of the key and
    appending the 32 bits passed in.

    \param[in] key_fragment The new key fragment.

    \return ONS_SUCCESS If the key was accepted
            ONS_ALREADY_IN_PROGRESS If still changing the key from the last
              time it was updated.
*/
one_net_status_t one_net_master_change_key_fragment(
  const one_net_xtea_key_fragment_t key_fragment);


one_net_status_t one_net_master_invite(const one_net_xtea_key_t * const KEY,
  UInt32 timeout);
one_net_status_t one_net_master_cancel_invite(
  const one_net_xtea_key_t * const KEY);

one_net_status_t one_net_master_remove_device(
  const on_raw_did_t * const RAW_DID);

void one_net_master(void);

one_net_status_t one_net_master_add_client(const on_features_t features,
  on_base_param_t* out_base_param, on_master_t* out_master_param,
  BOOL send_update_to_network);
  
#ifdef PEER
one_net_status_t one_net_master_peer_assignment(const BOOL ASSIGN,
  const on_raw_did_t * const SRC_DID, const UInt8 SRC_UNIT,
  const on_raw_did_t * const PEER_DID, const UInt8 PEER_UNIT);
#endif
  
one_net_status_t one_net_master_change_client_keep_alive(
  const on_raw_did_t * const RAW_DST, const UInt32 KEEP_ALIVE);
#ifdef BLOCK_MESSAGES_ENABLED
one_net_status_t one_net_master_change_frag_dly(
  const on_raw_did_t * const RAW_DST, const UInt16 LOW_DELAY,
  const UInt16 HIGH_DELAY);
#endif
one_net_status_t one_net_master_set_flags(on_client_t* client, UInt8 flags);
  
#ifndef PEER
int master_nv_crc(const UInt8* param);
#else
int master_nv_crc(const UInt8* param, const UInt8* peer_param);
#endif

one_net_xtea_key_t* master_get_encryption_key(
  const on_encoded_did_t* const did);
  
on_client_t* client_info(const on_encoded_did_t* CLIENT_DID);

#ifdef BLOCK_MESSAGES_ENABLED
/*!
    \brief Fills in some default values determined by ONE-NET for a proposed block transfer.
    
    \param[in] src_client The source of the transfer.  If NULL, the device is this device.
    \param[in] dst_client The destination of the transfer.
    \param[in] transfer_size The number of bytes to be transferred.
    \param[out] priority The priority of the transfer.
    \param[out] chunk_size The "chunk size" involved in the transfer.
    \param[out] frag_delay The time to wait between packet sends.
    \param[out] chunk_delay The time to pause between "chunks" of the message.
    \param[out] data_rate The data rate to use for the transfer.
    \param[out] channel The channel to use for the transfer.
    \param[out] timeout The time to wait for a response before assuming that
                   communication has been lost.
    \param[out] ack_nack If rejecting the transfer and there is an ack or nack associated
                   with it, this value will be filled in.
    
    \return The nack reason if rejecting the transfer.  
*/
on_nack_rsn_t on_master_get_default_block_transfer_values(
  on_client_t* src_client, on_client_t* dst_client, UInt32 transfer_size,
  UInt8* priority, UInt8* chunk_size, UInt16* frag_delay, UInt16* chunk_delay,
  UInt8* data_rate, UInt8* channel, UInt16* timeout, on_ack_nack_t* ack_nack);


/*!
    \brief Called when this device desires to send another device a block message
           AFTER the block message parameters have been loaded.
    
    This function is called (usually by the by the application code) after the
    parameters have been filled in.  In this function, ONE-NET sets up the
    state machine to find a route to the source, request permissions and inform
    any other devices of the transfers, switch data rates, then start sending
    the actual packets.  This function is called by the SOURCE of the message.
    
    \param[in] msg The parameters of the block message that is to be
                   transferred.  These parameters should be filled in before
                   this function is called.
    \param[out] ack_nack If rejecting the transfer and there is an ack or nack associated
                   with it, this value will be filled in.
    
    \return The nack reason if rejecting the transfer.
*/
on_nack_rsn_t on_master_initiate_block_msg(block_stream_msg_t* msg,
  on_ack_nack_t* ack_nack);
#endif

#ifdef STREAM_MESSAGES_ENABLED
/*!
    \brief Fills in some default values determined by ONE-NET for a proposed stream transfer.
    
    \param[in] src_client The source of the transfer.  If NULL, the device is this device.
    \param[in] dst_client The destination of the transfer.
    \param[in] time_ms Proposed duration of the stream transfer.  If time is 0, then the time is unknown.
    \param[out] priority The priority of the transfer.
    \param[out] frag_delay The time to wait between packet sends.
    \param[out] data_rate The data rate to use for the transfer.
    \param[out] channel The channel to use for the transfer.
    \param[out] timeout The time to wait for a response before assuming that
                   communication has been lost.
    \param[out] ack_nack If rejecting the transfer and there is an ack or nack associated
                   with it, this value will be filled in.
    
    \return The nack reason if rejecting the transfer.
*/
on_nack_rsn_t on_master_get_default_stream_transfer_values(
  const on_client_t* src, const on_client_t* dst, UInt32 time_ms,
  UInt8* priority, UInt16* frag_delay, UInt8* data_rate, UInt8* channel,
  UInt16* timeout, on_ack_nack_t* ack_nack);
  
  
/*!
    \brief Called when this device desires to send another device a stream message
           AFTER the block message parameters have been loaded.
    
    This function is called (usually by the by the application code) after the
    parameters have been filled in.  In this function, ONE-NET sets up the
    state machine to find a route to the source, request permissions and inform
    any other devices of the transfers, switch data rates, then start sending
    the actual packets.  This function is called by the SOURCE of the message.
    
    \param[in] msg The parameters of the stream message that is to be
                   transferred.  These parameters should be filled in before
                   this function is called.
    \param[out] ack_nack If rejecting the transfer and there is an ack or nack associated
                   with it, this value will be filled in.
    
    \return The nack reason if rejecting the transfer.
*/
on_nack_rsn_t on_master_initiate_stream_msg(block_stream_msg_t* msg,
  on_ack_nack_t* ack_nack);
#endif



//! @} ONE-NET_MASTER_pub_func
//                      PUBLIC FUNCTION DECLARATIONS END
//==============================================================================

//! @} ONE-NET_MASTER

#endif // if master is defined

#endif // ONE_NET_MASTER_H //
