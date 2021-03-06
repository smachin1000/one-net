#ifndef ONE_NET_H
#define ONE_NET_H

#include "config_options.h"
#include "one_net_packet.h"
#include "one_net_message.h"
#include "one_net_data_rate.h"
#include "one_net_channel.h"
#include "one_net_port_const.h"
#include "one_net_features.h"
#include "one_net_application.h"
#include "one_net_acknowledge.h"


//! \defgroup ONE-NET ONE-NET
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
    \file ONE_NET.h
    \brief ONE-NET declarations.

    Basis for ONE-NET.  Everything in this file is application independent
    (including independent from being a MASTER or a CLIENT).  The only places
    these functions should be called from is in one_net_master.c &
    one_net_client.c.
*/

#include "one_net_status_codes.h"
#include "one_net_types.h"
#include "one_net_xtea.h"


//==============================================================================
//                                  CONSTANTS
//! \defgroup ONE-NET_const
//! \ingroup ONE-NET
//! @{

//! ONE-NET Version numbers<br>
//! There are two types of ONE-NET vesion numbers. One set of version numbers is used
//! to identify a release of the ONE-NET source code. There are three levels to this
//! source code release version number, major, minor, and revision. These version numbers
//! appear in the one_net.h file as the three #defines ONE_NET_VERSION_MAJOR, ONE_NET_VERSION_MINOR,
//! and ONE_NET_VERSION_REVISION. For example, the source code that was released on CD at Rensas
//! DevCon on 10/13/08 was labeled as ONE_NET_SRC_V1_3_7.zip. This filename was constructed
//! based on the ONE-NET source code release version numbers of 1.3.7 (major=1, minor=3,
//! revision=7). 
//! There is a second type of ONE-NET version number named ON_VERSION. This is a one byte
//! version number which is really a ONE-NET protocol version number. This version number is
//! included in Status Response basic administrative message and is used to verfiy that the
//! master and client are communicating with compatible versions of the ONE-NET protocol.
//! Ideally, we would have only one ONE-NET version number and use that to determine both
//! the source code release version and the ONE-NET protocol compatibility. But ONE-NET 
//! efficiency requires that we use as small a number as possible for the protocol compatibility
//! number so we use one byte. We do not expect this ONE-NET compatibilty version number to 
//! change very often. However, one byte is not sufficient to represent the number of
//! iterations we expect to have for ONE-NET source code. So, we end up with two different
//! ONE-NET version numbers. Most people will be using the source code release version number
//! to identify releases of ONE-NET most of the time. 
//! In order to facilitate final testing of a release candidate, we also include have a 
//! ONE_NET_VERSION_BUILD number. This number will be inremented when a new load is put
//! on any piece of hardware. When a particular load has completed pre-release testing,
//! the build number for the load that was tested becomes the build number for the release.



#define one_net_client_send_single push_queue_element
#define one_net_master_send_single push_queue_element
#define one_net_send_single push_queue_element


enum
{
    ONE_NET_VERSION_MAJOR =     2,  //! ONE-NET major version number
    ONE_NET_VERSION_MINOR =     3,  //! ONE-NET minor version number
    ONE_NET_VERSION_REVISION =  1,  //! ONE-NET revision version number
    ONE_NET_VERSION_BUILD =     108 //! ONE-NET build version number
};


#ifdef PID_BLOCK
typedef enum
{
    PID_REJECT_IF_PRESENT, //! packet is accepted ONLY if this attribute is NOT present
    PID_ACCEPT_IF_PRESENT, //! packet is accepted ONLY if this attribute is present
    PID_ACCEPT             //! packet is accepted regardless of wether this attribute is present
} pid_block_criteria_t;


typedef struct
{
    //! Stores whether PIDs are blocked or not
    //! Interpreted as a 16 bit boolean array.  PID is accepted if the bit is 1, rejected if 0.
    //! PID 0x03 will be accepted if the 3rd bit index is true.  Index 0 is the least significant bit
    //! xxxxxxxxxxxx1xxx   --> PID 3 accepted
    //! xxxxxxxxxxxx0xxx   --> PID 3 rejected
    UInt16 block_pid_list;
    pid_block_criteria_t sa_block; //! Stores whether Stay-Awake PIDs are accepted
    pid_block_criteria_t mh_block; //! Stores whether Multi-Hop PIDs are accepted
} pid_block_t;
#endif


enum
{
    //! The ONE-NET version
    ON_VERSION = 0x09,

    //! The version of the parameter structures
    ON_PARAM_VERSION = 0x09,

    //! The version of the MASTER Invite New CLIENT packet.
    ON_INVITE_PKT_VERSION = 0x02,

    //! The maximum number of times to try a transaction
    ON_MAX_RETRY = 8,

    #ifdef ONE_NET_MULTI_HOP
    //! Represents an invalid hops value
    ON_INVALID_HOPS = ON_MAX_HOPS_LIMIT + 1,

    //! The number of hops to try when sending the first Multi-Hop packet
    ON_FIRST_MH_MAX_HOPS_COUNT = 2,
    #endif
};

#define ON_MAX_MSG_ID 4095

//! Default timeout in milliseconds for block / stream
#define DEFAULT_BLOCK_STREAM_TIMEOUT 3000


extern const on_raw_did_t MASTER_RAW_DID;
extern const on_encoded_did_t MASTER_ENCODED_DID;

//! The interval at which to wait for an ACK in Stream Transactions. If defined
//! as 5000, the device sending a Stream Data transaction will pause for an ACK
//! every 5000 milliseconds.
#define STREAM_RESPONSE_INTERVAL 5000

//! All transfer sizes <= 2000 bytes are considered "short"
#define ON_SHORT_BLOCK_TRANSFER_MAX_SIZE 2000


    
//! @} ONE-NET_const
//                                  CONSTANTS END
//==============================================================================

//==============================================================================
//                                  TYPEDEFS
//! \defgroup ONE-NET_typedefs
//! \ingroup ONE-NET
//! @{



/*!
    \brief States.

    Changing the states will likely require changing the code were they are
    used. All the WRITE_WAIT states need to be 1 more than the send states.
    All the states after the WRITE_WAIT states need to be 1 more than the
    WRITE_WAIT states.
*/
typedef enum
{
    //! Listen for single, block, or stream (or repeat) data packets
    ON_LISTEN_FOR_DATA,


    //! Sends a packet that does not expect a response back
    ON_SEND_PKT = 10,

    //! Waits for the write to end
    ON_SEND_PKT_WRITE_WAIT,

    #ifdef ONE_NET_MASTER
    //! Sends an invite packet
    ON_SEND_INVITE_PKT = 15,

    //! Waits for the invite packet write to end
    ON_SEND_INVITE_PKT_WRITE_WAIT,
    #endif

    //! Sends a Single Data Packet
    ON_SEND_SINGLE_DATA_PKT = 20,

    //! Waits for the write to end
    ON_SEND_SINGLE_DATA_WRITE_WAIT,

    //! Waits for the response to a single data packet
    ON_WAIT_FOR_SINGLE_DATA_RESP,
    

    //! State to send the single data response
    ON_SEND_SINGLE_DATA_RESP = 30,

    //! Waits for the write to end
    ON_SEND_SINGLE_DATA_RESP_WRITE_WAIT,
    
    //! State used when determining a route for a block / stream trans.
    ON_BS_FIND_ROUTE = 0x40,
    
    //! Wait for response for finding route
    ON_BS_SEND_FIND_ROUTE,
    
    //! Wait for the write to end
    ON_BS_SEND_FIND_ROUTE_WRITE_WAIT,
    
    //! Waits for a response to a find route packet
    ON_BS_WAIT_FOR_FIND_ROUTE_RESP,
    
    
    #ifdef DATA_RATE_CHANNEL
    //! State used when changing a data rate and channel for block / stream
    ON_BS_CHANGE_DR_CHANNEL,
    
    //! State used when changing a data rate and channel for block / stream
    //! and sending a message
    ON_BS_SEND_CHANGE_DR_CHANNEL,
    
    //! State used when changing a data rate and channel for block / stream
    //! and waiting for the write to complete
    ON_BS_SEND_CHANGE_DR_CHANNEL_WRITE_WAIT,
    
    //! State used when changing a data rate and channel for block / stream
    //! and waiting for a response
    ON_BS_WAIT_FOR_CHANGE_DR_CHANNEL_RESP,
    
    //! State used after the destination and repeaters have changed channels and
    //! before this device has.
    ON_BS_CHANGE_MY_DR_CHANNEL,
    #endif
    
    
    //! State used when confirming a route for a block / stream trans.
    ON_BS_CONFIRM_ROUTE,
    
    //! Wait for response for confirming route
    ON_BS_SEND_CONFIRM_ROUTE,
    
    //! Wait for the write to end
    ON_BS_SEND_CONFIRM_ROUTE_WRITE_WAIT,
    
    //! Waits for a response to a confirm route packet
    ON_BS_WAIT_FOR_CONFIRM_ROUTE_RESP,


    //! Ask the device for permission
    ON_BS_DEVICE_PERMISSION,
    
    //! Send device permission packet
    ON_BS_SEND_DEVICE_PERMISSION,
    
    //! Wait for write to complete
    ON_BS_SEND_DEVICE_PERMISSION_WRITE_WAIT,
    
    //! Wait for response from device
    ON_BS_WAIT_FOR_DEVICE_PERMISSION_RESP,
    
    
    #ifdef ONE_NET_MULTI_HOP
    //! Start state of requesting repeater permission
    ON_BS_REPEATER_PERMISSION_START,
        
    //! Ask the master for repeater permission
    ON_BS_REPEATER_PERMISSION,
    
    //! Send master repeater permission packet
    ON_BS_SEND_REPEATER_PERMISSION,
    
    //! Wait for write to complete
    ON_BS_SEND_REPEATER_PERMISSION_WRITE_WAIT,
    
    //! Wait for response from repeater
    ON_BS_WAIT_FOR_REPEATER_PERMISSION_RESP,
    
    //! End state of requesting repeater permission
    ON_BS_REPEATER_PERMISSION_END,
    
    
    #ifdef BLOCK_STREAM_REQUEST_MASTER_PERMISSION
    //! Start state of requesting repeaters from master
    ON_BS_MASTER_REPEATER_PERMISSION_START,
    
    //! Ask the master for repeater permission
    ON_BS_MASTER_REPEATER_PERMISSION,
    
    //! Send master repeater permission packet
    ON_BS_SEND_MASTER_REPEATER_PERMISSION,
    
    //! Wait for write to complete
    ON_BS_SEND_MASTER_REPEATER_PERMISSION_WRITE_WAIT,
    
    //! Wait for response from master
    ON_BS_WAIT_FOR_MASTER_REPEATER_PERMISSION_RESP,
    
    //! End state of requesting repeaters from master
    ON_BS_MASTER_REPEATER_PERMISSION_END,
    #endif
    #endif
    
    
    #ifdef BLOCK_STREAM_REQUEST_MASTER_PERMISSION
    //! Ask the master for device permission
    ON_BS_MASTER_DEVICE_PERMISSION,
    
    //! Send master device permission packet
    ON_BS_SEND_MASTER_DEVICE_PERMISSION,
    
    //! Wait for write to complete
    ON_BS_SEND_MASTER_DEVICE_PERMISSION_WRITE_WAIT,
    
    //! Wait for response from master
    ON_BS_WAIT_FOR_MASTER_DEVICE_PERMISSION_RESP,
    #endif
    

    //! Block or stream transaction ready to commence
    ON_BS_COMMENCE,
    
    //! Ready or waiting to prepare a block or stream packet
    ON_BS_PREPARE_DATA_PACKET,
    
    //! Sends a Block / Stream Data Packet
    ON_BS_SEND_DATA_PKT,

    //! Waits for the write to end
    ON_BS_SEND_DATA_WRITE_WAIT,

    //! Wait for the response to a block / stream data packet
    ON_BS_WAIT_FOR_DATA_RESP,
    
    //! Pausing between chunks in a block / stream packet
    ON_BS_CHUNK_PAUSE,
    
    //! First stage of the termination process for block / stream
    ON_BS_TERMINATE,
    
    //! Sending the termination packet
    ON_BS_SEND_TERMINATE_PACKET,
    
    //! Waits for the write to end
    ON_BS_SEND_TERMINATE_WRITE_WAIT,
    
    //! Waits for a response for the termination
    ON_BS_WAIT_FOR_TERMINATE_RESP,
    
    //! Termination is complete
    ON_BS_TERMINATE_COMPLETE,

#ifdef IDLE	
	//! do nothing
	ON_IDLE = 0xF0,
#endif

    //! State when the device has not yet joined the netwrok.  If the device is
    //! a client, it looks for the invite from the MASTER.  If the device is a
    //! MASTER, it looks for a clear channel to establish its network on.
    ON_JOIN_NETWORK,

    //! The default state when the device starts up (to ensure the proper
    //! initialization routines are called)
    ON_INIT_STATE
} on_state_t;



enum
{
#ifdef ONE_NET_CLIENT
    CLIENT_NV_PARAM_SIZE_BYTES = sizeof(on_base_param_t) + sizeof(on_master_t),
#else
    CLIENT_NV_PARAM_SIZE_BYTES = 0,
#endif
#ifdef ONE_NET_MASTER
    MIN_MASTER_NV_PARAM_SIZE_BYTES = sizeof(on_base_param_t) +
      sizeof(on_master_param_t),
    MAX_MASTER_NV_PARAM_SIZE_BYTES = MIN_MASTER_NV_PARAM_SIZE_BYTES +
      ONE_NET_MASTER_MAX_CLIENTS * sizeof(on_client_t),
#else
    MAX_MASTER_NV_PARAM_SIZE_BYTES = 0,
#endif
    NV_PARAM_SIZE_BYTES =
      CLIENT_NV_PARAM_SIZE_BYTES > MAX_MASTER_NV_PARAM_SIZE_BYTES ?
      CLIENT_NV_PARAM_SIZE_BYTES : MAX_MASTER_NV_PARAM_SIZE_BYTES
};


typedef enum
{
    MEMORY_GENERIC,  // can refer to either peer or non-peer memory
    MEMORY_NON_PEER, // memory refers to non-peer memory
    MEMORY_PEER      // memory referes to peer memory
} memory_type_t;



//! Function to retrieve sending device information
typedef on_sending_device_t* (*one_net_get_sender_info_func_t)
  (const on_encoded_did_t * const DID);;


//! Packet Handling Function for data packets
typedef on_message_status_t (*on_pkt_hdlr_t)(on_txn_t** txn,
  on_pkt_t* const pkt, UInt8* raw_pld, UInt8* msg_type,
  on_ack_nack_t* ack_nack);

#ifdef BLOCK_MESSAGES_ENABLED
typedef on_message_status_t (*on_bs_pkt_hdlr_t)(on_txn_t* txn,
  block_stream_msg_t* bs_msg, void* bs_pkt, on_ack_nack_t* ack_nack);
  
//! Packet Handling Function for block / stream responses
typedef on_message_status_t (*on_bs_ack_nack_hdlr_t)(on_txn_t* txn,
  block_stream_msg_t* bs_msg, const on_pkt_t* pkt,
  const UInt8* raw_payload_bytes, on_ack_nack_t* ack_nack);
#endif

//! Packet Handling Function for responses
typedef on_message_status_t (*on_ack_nack_hdlr_t)(on_txn_t* txn,
  on_pkt_t* const pkt, UInt8* raw_pld, UInt8* msg_type,
  on_ack_nack_t* ack_nack);

#ifndef ONE_NET_SIMPLE_CLIENT
typedef void (*on_recip_list_hdlr_t)(const on_single_data_queue_t*
  const msg, on_recipient_list_t** recipient_send_list);
#endif

//! Single Transaction handler
typedef on_message_status_t (*on_txn_hdlr_t)(on_txn_t ** txn,
  on_pkt_t* const pkt,  UInt8* raw_pld, UInt8* msg_type,
  const on_message_status_t status, on_ack_nack_t* ack_nack);

#ifdef BLOCK_MESSAGES_ENABLED
//! Block / Stream Transaction Handler
typedef on_message_status_t (*on_bs_txn_hdlr_t)(const block_stream_msg_t* msg,
  const on_encoded_did_t* terminating_device, on_message_status_t* status,
  on_ack_nack_t* ack_nack);
#endif
  
  
//! The set of needed packet handlers
typedef struct
{
    //! Single Data Packet Handler
    on_pkt_hdlr_t single_data_hdlr;
	
	//! Single Data ACK/NACK Handler
	on_ack_nack_hdlr_t single_ack_nack_hdlr;

    //! Single transaction handler
    on_txn_hdlr_t single_txn_hdlr;
    
    #ifndef ONE_NET_SIMPLE_CLIENT
    on_recip_list_hdlr_t adj_recip_list_hdlr;
    #endif

    #ifdef BLOCK_MESSAGES_ENABLED
    //! Single Data Packet Handler
    on_bs_pkt_hdlr_t block_data_hdlr;
	
	//! Single Data ACK/NACK Handler
	on_bs_ack_nack_hdlr_t block_ack_nack_hdlr;

    //! Single transaction handler
    on_bs_txn_hdlr_t block_txn_hdlr;
    #endif

    #ifdef STREAM_MESSAGES_ENABLED
    //! Stream Data Packet Handler
    on_bs_pkt_hdlr_t stream_data_hdlr;
	
	//! Stream Data ACK/NACK Handler
	on_bs_ack_nack_hdlr_t stream_ack_nack_hdlr;

    //! Stream transaction handler
    on_bs_txn_hdlr_t stream_txn_hdlr;
    #endif
} on_pkt_hdlr_set_t;


typedef enum
{
    ON_DR_CHANNEL_NO_SCHEDULED_CHANGE,
    ON_DR_CHANNEL_CHANGE_SCHEDULED,
    ON_DR_CHANNEL_CHANGE_DONE,
    NUM_DR_CHANNEL_STAGES
} dr_channel_stage_t;


//! @} ONE-NET_typedefs
//                                  TYPEDEFS END
//==============================================================================

//==============================================================================
//                              PUBLIC VARIABLES
//! \defgroup ONE-NET_pub_var
//! \ingroup ONE-NET
//! @{



//! The encoded broadcast did.
extern const on_encoded_did_t ON_ENCODED_BROADCAST_DID;

//! The raw broadcast did.
extern const on_raw_did_t ON_RAW_BROADCAST_DID;

extern UInt8 nv_param[];
extern on_base_param_t* const on_base_param;


extern on_txn_t response_txn;
extern on_txn_t single_txn;
#ifdef BLOCK_MESSAGES_ENABLED
extern on_txn_t bs_txn;
#endif

#ifdef PID_BLOCK
extern BOOL pid_blocking_on;
extern pid_block_t pid_block_info;
#endif


//! An array that contains the number of of units of each type that this
//! device supports.  If values are changed here, see ONE_NET_NUM_UNIT_TYPES &
//! ONE_NET_NUM_UNITS
extern const ona_unit_type_count_t
  ONE_NET_DEVICE_UNIT_TYPE[ONE_NET_NUM_UNIT_TYPES];

//! true if device is functioning as a master, false otherwise 
extern BOOL device_is_master;

//! The current state.
extern on_state_t on_state;


//! The set of packet handlers
extern on_pkt_hdlr_set_t pkt_hdlr;


//! an on_pkt_t structure for data packets
extern on_pkt_t data_pkt_ptrs;

//! an on_pkt_t structure for response packets
extern on_pkt_t response_pkt_ptrs;

//! a function to retrieve the sender information
extern one_net_get_sender_info_func_t get_sender_info;


//! A place to store a single message with payload.
extern on_single_data_queue_t single_msg;

//! A place to store the single message raw payload.
extern UInt8 single_data_raw_pld[];

//! Pointer to the current single message being sent.  If none, this will be
//! NULL.  Generally this will point to single_msg.
extern on_single_data_queue_t* single_msg_ptr;


//! A place to store the raw packet bytes when encrypting, decrypting, etc.
//! so that it will not have to be declared inside of functions and risk a
//! overflow.
extern UInt8 raw_payload_bytes[];

//! Unique key of the device being invited into the network
extern one_net_xtea_key_t invite_key;

//! The current invite transaction
extern on_txn_t invite_txn;

//! Boolean value denoting whether a key change should occur in the very
//! near future
extern BOOL key_change_requested;

//! Time of the last key change request
extern tick_t key_change_request_time;


// The buffer length required for both receiving and sending is...
//
// Both masters need to be able to store both an outgoing message and
// an incoming message.  For devices without block messages, the length of
// the packet will be ON_SINGLE_ENCODED_PKT_SIZE.  For devices with block
// messages, the length of the packet will be ON_BLOCK_ENCODED_PKT_SIZE.
// The buffer length must be at least twice this length (one for send, one for
// receive.
//
// In addition, masters which send invite packets need an additional
// ON_INVITE_ENCODED_PKT_SIZE bytes are needed for sending the invite packet.
//
// Clients do not need these additional buffer bytes.  They need to RECEIVE
// an invite packet, but that invite packet length is less than the twice
// ON_SINGLE_ENCODED_PKT_SIZE, so no more bytes are needed.
//
// 
// Repeaters need to be able to repeat all packets, including block packets.
// Repeaters will always be clients, so the buffer must be at least the length
// of a multi-hop block packet.
#ifndef ONE_NET_MASTER
     #define MIN_ENCODED_BYTES_BUFFER_LEN (2 * ON_MAX_ENCODED_DATA_PKT_SIZE)
#else
    #define MIN_ENCODED_BYTES_BUFFER_LEN (2 * ON_MAX_ENCODED_DATA_PKT_SIZE + ON_INVITE_ENCODED_PKT_SIZE)
#endif

#if defined(ONE_NET_MH_CLIENT_REPEATER) && defined(BLOCK_MESSAGES_ENABLED)
    #if MIN_ENCODED_BYTES_BUFFER_LEN < ON_BLOCK_ENCODED_PKT_SIZE
        #define ENCODED_BYTES_BUFFER_LEN ON_BLOCK_ENCODED_PKT_SIZE
    #else
        #define ENCODED_BYTES_BUFFER_LEN MIN_ENCODED_BYTES_BUFFER_LEN
    #endif
#else
    #define ENCODED_BYTES_BUFFER_LEN MIN_ENCODED_BYTES_BUFFER_LEN
#endif

//! A buffer containing all encoded bytes for transmitting and receiving
extern UInt8 encoded_pkt_bytes[];

//! The expected source of the next packet.
extern on_encoded_did_t expected_src_did;

//! Denotes which key was used.  If true, the current key is being used.
extern BOOL decrypt_using_current_key;

#ifdef NON_VOLATILE_MEMORY
extern BOOL save;
#endif

#ifdef ROUTE
//! variable denoting the start of a route message.
extern tick_t route_start_time;
#endif

#ifdef BLOCK_MESSAGES_ENABLED
extern block_stream_msg_t bs_msg;
#endif

#ifdef DATA_RATE_CHANNEL
extern dr_channel_stage_t dr_channel_stage;
extern UInt16 dormant_data_rate_time_ms;
extern UInt8 next_data_rate;
extern UInt8 next_channel;
#endif


//! @} ONE-NET_pub_var
//                              PUBLIC VARIABLES END
//==============================================================================


#ifdef __cplusplus
extern "C"
{
#endif


//==============================================================================
//                      PUBLIC FUNCTION DECLARATIONS
//! \defgroup ONE-NET_pub_func
//! \ingroup ONE-NET
//! @{


// initialization
void one_net_init(void);

//! the main function
void one_net(on_txn_t ** txn);


on_message_status_t rx_single_data(on_txn_t** txn, on_pkt_t* sing_pkt_ptr,
  UInt8* raw_payload, on_ack_nack_t* ack_nack);
#if defined(BLOCK_MESSAGES_ENABLED) || defined(ONE_NET_MH_CLIENT_REPEATER)
one_net_status_t on_rx_packet(const on_txn_t* const txn, on_txn_t** this_txn,
  on_pkt_t** this_pkt_ptrs, UInt8* raw_payload_bytes);
#else
one_net_status_t on_rx_packet(on_txn_t** this_txn, on_pkt_t** this_pkt_ptrs,
  UInt8* raw_payload_bytes);
#endif
  

#ifdef PID_BLOCK
void set_pid_block(UInt8 raw_pid, BOOL accept);
void set_pid_block_sa(pid_block_criteria_t sa_block);
void set_pid_block_mh(pid_block_criteria_t mh_block);
BOOL pid_is_blocked(UInt8 pid);
#endif

#ifdef RANGE_TESTING
void enable_range_testing(BOOL on);
BOOL devices_within_range(on_encoded_did_t* enc_dids, UInt8* num_in_range,
  BOOL* on);
BOOL adjust_range_test_did_array(on_encoded_did_t* const did, BOOL add);
void reset_range_test_did_array(void);
BOOL device_in_range(on_encoded_did_t* did);
#endif


#ifdef ROUTE
one_net_status_t send_route_msg(const on_raw_did_t* raw_did);
UInt16 extract_raw_did_from_route(const UInt8* route, UInt8 index);
SInt8 find_raw_did_in_route(const UInt8* route,
  const on_raw_did_t* const raw_did, SInt8 minimum_index);
SInt8 append_raw_did_to_route(UInt8* route, const on_raw_did_t* const raw_did);
BOOL extract_repeaters_and_hops_from_route(const on_encoded_did_t* const
  dst, UInt8* route, UInt8* hops, UInt8* return_hops,
  UInt8* num_repeaters, on_encoded_did_t* repeaters);
#endif


#ifdef DATA_RATE_CHANNEL
on_nack_rsn_t on_change_dr_channel(const on_encoded_did_t* enc_did,
  UInt16 pause_time_ms, UInt16 dormant_time_ms, UInt8 new_channel,
  UInt8 new_data_rate);
#endif

void reset_msg_ids(void);

BOOL new_key_fragment(const one_net_xtea_key_fragment_t* const fragment,
  BOOL copy_key);

BOOL one_net_reject_bad_msg_id(const on_sending_device_t* device);


#ifdef BLOCK_MESSAGES_ENABLED
UInt32 estimate_block_transfer_time(const block_stream_msg_t* bs_msg);
void adjust_bs_priority(block_stream_msg_t* msg, UInt8 priority);
void adjust_bs_chunk_pause(block_stream_msg_t* msg, UInt16 chunk_pause);
void pause_bs_msg(block_stream_msg_t* msg, UInt16 pause_ms);
void adjust_bs_frag_delay(block_stream_msg_t* msg, UInt16 frag_delay);
#if defined(ONE_NET_MULTI_HOP) && defined(ONE_NET_CLIENT) && defined(BLOCK_MESSAGES_ENABLED)
on_single_data_queue_t* request_reserve_repeater(
  const block_stream_msg_t* bs_msg, const on_encoded_did_t* repeater);
#endif

// TODO -- Do we really want to require block messages for this function?
#ifdef ONE_NET_MULTI_HOP
UInt16 estimate_response_time(UInt8 data_len, UInt8 response_len,
  UInt8 hops, UInt16 dst_process_time, UInt16 repeater_process_time,
  UInt8 data_rate);
#else
UInt16 estimate_response_time(UInt8 response_len, UInt8 dst_process_time,
  UInt8 data_rate);
#endif

void terminate_bs_msg(block_stream_msg_t* bs_msg,
  const on_encoded_did_t* terminating_did, on_message_status_t status,
  on_ack_nack_t* ack_nack);
#endif



//! @} ONE-NET_pub_func
//                      PUBLIC FUNCTION DECLARATIONS END
//==============================================================================


#ifdef __cplusplus
}
#endif

//! @} ONE-NET

#endif // ONE_NET_H //
