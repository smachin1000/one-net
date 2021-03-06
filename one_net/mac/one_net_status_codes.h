#ifndef ONE_NET_STATUS_CODES_H
#define ONE_NET_STATUS_CODES_H


//! \defgroup ONE-NET_status_codes ONE-NET Status Codes
//! \ingroup ONE-NET
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
    \file one_net_status_codes.h
    \brief ONE-NET status codes.

    Status codes returned by ONE-NET functions.
    
    \note See one_net.h for the version of the ONE-NET source as a whole.  If
      any one file is modified, the version number in one_net.h will need to be
      updated.
*/


//==============================================================================
//                                  CONSTANTS
//! \defgroup ONE-NET_status_codes_const
//! \ingroup ONE-NET_status_codes
//! @{

//! @} ONE-NET_status_codes_const
//                                  CONSTANTS END
//==============================================================================

//==============================================================================
//                                  TYPEDEFS
//! \defgroup ONE-NET_status_codes_typedefs
//! \ingroup ONE-NET_status_codes
//! @{

typedef enum
{
    ONS_SUCCESS,                    //!< [0] Function was successful

    ONS_BAD_PARAM,                  //!< [1] The parameters passed in are invalid
    ONS_NOT_INIT,                   //!< [2] Was not initialized
    ONS_ALREADY_IN_PROGRESS,        //!< [3] The same operation is in progress
    ONS_INVALID_DATA,               //!< [4] Received invalid data
    ONS_MORE,                       //!< [5] If more data is expected
    ONS_END,                        //!< [6] The end has been reached.
    ONS_RSRC_FULL,                  //!< [7] The resources have reached capacity
    ONS_CANCELED,                   //!< [8] The operation was canceled
    ONS_TIME_OUT,                   //!< [9] The operation timed out
    ONS_INTERNAL_ERR,               //!< [10] An internal error occured.

    ONS_UNHANDLED_VER,              //!< [11] The version is not handled by this dev.
    ONS_CRC_FAIL,                   //!< [12] Computed crc did not match received
    ONS_RX_STAY_AWAKE,              //!< [13] A Stay Awake ACK was received.
    ONS_RX_NACK,                    //!< [14] Received a NACK

    ONS_SINGLE_END = 16,            //!< [16] The transaction completed successfully
    ONS_SINGLE_FAIL,                //!< [17] Single transaction failed.

    ONS_BLOCK_END,                  //!< [18] Block txn completed successfully
    ONS_BLOCK_FAIL,                 //!< [19] The block transaction has failed

    ONS_STREAM_END,                 //!< [20] The other side has ended the stream
    ONS_STREAM_FAIL,                //!< [21] The stream transaction has failed

    ONS_TXN_QUEUED,                 //!< [22] If a txn has been queued to send back
    ONS_TXN_DOES_NOT_EXIST,         //!< [23] The transaction does not exist

    ONS_BAD_RAW_PKT_LEN,            //!< [24] Invalid length for a raw message
    ONS_BAD_RAW_DATA,               //!< [25] The data in a raw block was not valid
    ONS_BAD_ENCODING,               //!< [26] The encoding was not valid
    ONS_BAD_PKT_TYPE,               //!< [27] Received an unknown pkt type
    ONS_BAD_PKT,                    //!< [28] The packet was invalid
    ONS_UNHANDLED_PKT,              //!< [29] The packet is not being handled

    ONS_UNICAST_ADDR,               //!< [30] Our unicast address.
    ONS_MULTICAST_ADDR,             //!< [31] Received one of our group addresses
    ONS_BROADCAST_ADDR,             //!< [32] Received a broadcast address
    ONS_BAD_ADDR,                   //!< [33] The address format is invalid
    ONS_INCORRECT_ADDR,             //!< [34] Received sender addr not expected.
    ONS_NID_FAILED,                 //!< [35] The NID portion of the address failed
    ONS_DID_FAILED,                 //!< [36] The DID portion of the address failed
    
    ONS_DEVICE_LIMIT,               //!< [37] The MASTER can't handle anymore devices
    ONS_NOT_JOINED,                 //!< [38] The device has not joined the network.
    
    ONS_READ_ERR,                   //!< [39] Did not read specified number of bytes.
    ONS_WRITE_ERR,                  //!< [40] Didn't write specified number of bytes.
    ONS_FAIL,                       //!< [41] Generic failure code.
    ONS_DEVICE_NOT_CAPABLE,         //!< [42] Physical (or logical) device does not
                                    //!<      have the capability of achieving
                                    //!<      this request (i.e. a low cost device being
                                    //!<      requested t change to a very high data rate
                                    //!<      that its transceiver cannot handle.
    ONS_PKT_RCVD,                   //!< [43] A packet has been received.
    ONS_BAD_MSG_ID,                 //!< [44] Packet has an invalid message ID.
    ONS_BUSY,                       //!< [45] Device is busy with something and cannot service
                                    //!<      this request right now.  This is similar to, but
                                    //!<      not identical to ONS_ALREADY_IN_PROGRESS.
    ONS_BAD_KEY_FRAGMENT,           //!< [46] Key fragment is invalid.
    ONS_SNGH_INTERNAL_ERR           //!< [47] "Should Not Get Here" in the code internal error. 
} one_net_status_t;


typedef enum
{
    ON_STARTUP_SUCCESS, //! startup was successful and is complete
    ON_STARTUP_IN_PROGRESS, //! startup is in progress
    ON_STARTUP_FAIL //! startup failed and is complete
} one_net_startup_status_t;


//! Generally set by application data message handlers and the ack/nack
//! handlers to tell ONE-NET how / whether to proceed with a transaction.
//! Also used to inform application code of how a transaction concluded
//! (i.e. success, failure, aborted, etc.)
typedef enum
{
    ON_MSG_DEFAULT_BHVR, //! use default ONE-NET behavior.
    ON_MSG_CONTINUE, //! continue processing message.
    ON_MSG_ABORT, //! abort this transaction(note : this status DOES NOT
                  //! NECESSARILY mean a failure occurred.  Its meaning will
                  //! depend on the application -- for example it could be
                  //! interpreted as "I have all the information I need.  No "
                  //! need to send any more")
    ON_MSG_SUCCESS, //! Successful completion.
    ON_MSG_FAIL, //! Failure.
    ON_MSG_RESPOND, //! Message should be responded to with an ACK or a NACK.
    ON_MSG_TIMEOUT, //! Message has timed out.
    ON_MSG_IGNORE, //! Message should be ignored.  ONE-NET should not send a response.
               //! This doesn't mean a response won't be sent.  It could be sent
               //! by the application code later.
    ON_MSG_ACCEPT_PACKET, //! A block / stream packet should be marked as received
    ON_MSG_REJECT_PACKET, //! A block / stream packet should be marked as not received
    ON_MSG_ACCEPT_CHUNK, //! A block / stream chunk should be marked as received
    ON_MSG_REJECT_CHUNK, //! A block / stream chunk should be marked as not received
    ON_MSG_TERMINATE,    //! A block / stream transfer should terminate.  Basically the
                         //! same as ON_MSG_ABORT.  ONE-NET will treat these as identical.
                         //! The application code may treat them differently.
    ON_MSG_INTERNAL_ERR, //! Represents an internal error
    ON_MSG_ROUTE_UNAVAILABLE, //! No route to the device and back is available
    ON_BS_MSG_SETUP_CHANGE,  //! The Block / Stream parameters have changed and the
                             //! setup process should start again with the new parameters.
    ON_MSG_PAUSE,        //! Pause the transaction
    ON_NUM_MESSAGE_STATUS_CODES, //! The number of predefined message codes except for
                                 //! "Unset" and "General Error"
    
    //! 0x11 - 0x7D are reserved for future use
    
    ON_MSG_STATUS_UNSET = 0x7E, //! Catch-all fill-in for when no code fits
    ON_MSG_GENERAL_ERR = 0x7F, //! Catch-all fill-in error code for when no other error code fits
    
    //! Any on_message_status_t value >= ON_MIN_APPLICATION_STATUS_CODE is to
    //! be interpreted purely by the application handler.  These codes will
    //! be user-supplied and vary from application to application.
    ON_MIN_APPLICATION_STATUS_CODE = 0x80,
    ON_MAX_APPLICATION_STATUS_CODE = 0xFF,
} on_message_status_t;



//! @} ONE-NET_status_codes_typedefs
//                                  TYPEDEFS END
//==============================================================================

//==============================================================================
//                              PUBLIC VARIABLES
//! \defgroup ONE-NET_status_codes_pub_var
//! \ingroup ONE-NET_status_codes
//! @{

//! @} ONE-NET_status_codes_pub_var
//                              PUBLIC VARIABLES END
//==============================================================================

//==============================================================================
//                      PUBLIC FUNCTION DECLARATIONS
//! \defgroup ONE-NET_status_codes_pub_func
//! \ingroup ONE-NET_status_codes
//! @{

//! @} ONE-NET_status_codes_pub_func
//                      PUBLIC FUNCTION DECLARATIONS END
//==============================================================================
//! @} ONE-NET_status_codes

#endif // ONE_NET_STATUS_CODES_H //

