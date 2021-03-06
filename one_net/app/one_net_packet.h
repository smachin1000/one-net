#ifndef ONE_NET_PACKET_H
#define ONE_NET_PACKET_H

#include "config_options.h"
#include "one_net_types.h"
#include "one_net_constants.h"
#include "one_net_xtea.h"
#include "one_net_encode.h"


//! \defgroup ONE-NET_PACKET ONE-NET Packet Definitions and Lengths
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
    \file one_net_message.h
    \brief Constants and typedefs dealing with packets and packet lengths.

    Lengths of packets should now all be in one file instead of in
     transceiver-specific files.  I'm not entirely sure what should go in
     here as opposed to one_net_application.h and one_net_message.h, but the
     general idea is to get the packet-length constants out of one_net.h and
     even more importantly, get them out of files list adi.c.  Packet lengths
     are not application-specific, transceiver-specific, or processor-specific
     so we want them out of those files.
*/




//==============================================================================
//                                  TYPEDEFS
//! \defgroup ONE-NET_PACKET_typedefs
//! \ingroup ONE-NET_PACKET
//! @{



#define ONE_NET_RAW_PID_PACKET_TYPE_MASK 0x3F
#define ONE_NET_RAW_PID_MH_MASK 0x80
#define ONE_NET_RAW_PID_MH_SHIFT 7
#define ONE_NET_RAW_PID_STAY_AWAKE_MASK 0x40
#define ONE_NET_RAW_PID_STAY_AWAKE_SHIFT 6
#define ONE_NET_RAW_PID_SIZE_MASK 0xF00
#define ONE_NET_RAW_PID_SIZE_SHIFT 8




/*!
    \brief Raw Packet Types
*/

 

//! Single Data Packet.
#define ONE_NET_RAW_SINGLE_DATA 0x00

//! Acknowledgment of successful reception of a single data packet.
#define ONE_NET_RAW_SINGLE_DATA_ACK 0x01

//! Acknowledges that a single data packet was received, but an
//! error was encountered. The NACK reason field on this NACK specifies
//! the error condition that resulted in the NACK.
#define ONE_NET_RAW_SINGLE_DATA_NACK_RSN 0x02



#ifdef ROUTE
//! Route Packet
#define ONE_NET_RAW_ROUTE      0x03

//! Route Packet ACK
#define ONE_NET_RAW_ROUTE_ACK  0x04

//! Route Packet NACK
#define ONE_NET_RAW_ROUTE_NACK 0x05
#endif



#ifdef BLOCK_MESSAGES_ENABLED
//! Block Data Packet
#define ONE_NET_RAW_BLOCK_DATA 0x06

//! Acknowledgment of successful reception of a block data packet.
#define ONE_NET_RAW_BLOCK_DATA_ACK 0x07

//! Acknowledges that a block data packet was received, but an
//! error was encountered. The NACK reason field on this NACK specifies
//! the error condition that resulted in the NACK.
#define ONE_NET_RAW_BLOCK_DATA_NACK_RSN 0x08

//! A block transfer is being terminated for some reason.  This could be due
//! to a problem or due to a successful completion.  This should not be
//! assumed to be either an ACK or a NACK.  The packet must be parsed for
//! further meaning.
#define ONE_NET_RAW_BLOCK_TERMINATE 0x09
#endif



#ifdef STREAM_MESSAGES_ENABLED
//! Stream Data Packet
#define ONE_NET_RAW_STREAM_DATA 0x0A

//! Acknowledgment of successful reception of a stream data packet.
#define ONE_NET_RAW_STREAM_DATA_ACK 0x0B

//! Acknowledges that a stream data packet was received, but an
//! error was encountered. The NACK reason field on this NACK specifies
//! the error condition that resulted in the NACK.
#define ONE_NET_RAW_STREAM_DATA_NACK_RSN 0x0C

//! A stream transfer is being terminated for some reason.  This could be due
//! to a problem or due to a successful completion.  This should not be
//! assumed to be either an ACK or a NACK.  The packet must be parsed for
//! further meaning.
#define ONE_NET_RAW_STREAM_TERMINATE 0x0D
#endif



//! MASTER broadcast inviting a new CLIENT to join the network.
#define ONE_NET_RAW_MASTER_INVITE_NEW_CLIENT 0x0E

//! CLIENT message sent requesting an invitation
#define ONE_NET_RAW_CLIENT_REQUEST_INVITE 0x0F



typedef enum _ona_msg_type
{
    //! (0-1-2)(OFF/ON/TOGGLE)
    ONA_SWITCH = 0x00,

    //! Represents 1/100th of a Percent Intervals.  10,000 = 100%
    ONA_PERCENT = 0x01,

    //! Represents 1/100th of a Degree Kelvin.  100,000 = 1000 degrees Kelvin
    ONA_TEMPERATURE = 0x02,

    //! Represents 1/100th of a Percent Intervals.  5000 = 50%
    ONA_HUMIDITY = 0x03,

    //! Represents 1/100th of a millibar.  10,000 = 100 mb
    ONA_PRESSURE_100THS_MBAR = 0x04,
    
    //! Represents a millibar.  100 = 100 mb
    ONA_PRESSURE_MBAR = 0x05,
    
    //! Represents length in angstroms.  10,000 = 10,000 angstroms, 1 micrometer
    ONA_LENGTH_ANGSTROM = 0x06,
    
    //! Represents length in micrometers.  100 = 100 micrometers, 0.1 mmm
    ONA_LENGTH_MICRO = 0x07,
    
    //! Represents length in 1/10ths of a millimeter.  100 = 10 mm, 1 cm
    ONA_LENGTH_10THS_MM = 0x08,
    
    //! Represents length in centimeters.  1000 = 1000 cm, 10 meters
    ONA_LENGTH_CM = 0x09,
    
    //! Represents length in meters.  10,000 = 10,000 meters, 10 kilometers
    ONA_LENGTH_METER = 0x0A,
    
    //! Represents depth in 1/10th millimeter increments.  10,000 = 1 meter
    ONA_DEPTH_10THS_MM = 0x0B,
    
    //! Represents depth in centimeter increments.  100 = 1 meter
    ONA_DEPTH_CM = 0x0C,
    
    //! Represents depth in meter increments.  100 = 100 meters
    ONA_DEPTH_METER = 0x0D,
    
    //! Represents velocity in 1/10th millimeters / second increments.  10,000 = 1 meter /second
    ONA_VELOCITY_10THS_MM_SEC = 0x0E,
    
    //! Represents velocity in centimeters per second increments.  100 = 1 meter / second
    ONA_VELOCITY_CM_SEC = 0x0F,
    
    //! Represents an angle measurement in 1/100th degree increments.  10,000 = 100 degrees
    ONA_DIRECTION = 0x10,
    
    //! Represents an angle measurement in 1/1000th degree increments in radians.  2,000 = 2 degrees in radians
    ONA_DIRECTION_RADIANS = 0x11,

    //! Opening Status.  Generally used for doors, windows, gates, etc. (0 = closed, 1 = open)
    ONA_OPENING = 0x12,

    //! Seal Status.  (0 = sealed / unbroken / intact, 1 = sealed / broken)
    //! Generally used to test whether something is still intact (i.e. window unbroken, fuse still connected, wire not cut, etc.)
    //! Used often in alarms / intrusion detection
    ONA_SEAL = 0x13,

    //! Set/Query/Report Unit Type Count 
    ONA_UNIT_TYPE_COUNT = 0x14,

    //! Set/Query/Report Unit Type
    ONA_UNIT_TYPE = 0x15,
    
    //! Time in microseconds
    ONA_TIME_MICRO_SEC = 0x16,
    
    //! Time in milliseconds
    ONA_TIME_MS = 0x17,
    
    //! Time in seconds
    ONA_TIME_SEC = 0x18,
    
    //! Time of Day (0 - 86,399) -- Represents the number of seconds after midnight
    ONA_TIME_OF_DAY = 0x19,

    //! Query Voltage
    ONA_VOLTAGE = 0x1A,

    //! Voltage (in volts)
    ONA_VOLTAGE_VOLTS = 0x1B,

    //! Voltage (in 100ths of a volt)
    ONA_VOLTAGE_100THS_VOLTS = 0x1C,

    //! Voltage (in simple good/bad -- 0 = Good, 1 = Bad)
    ONA_VOLTAGE_SIMPLE = 0x1D,
    
    //! ASCII Simple Text (2 ASCII chars, no NULL terminator)
    ONA_SIMPLE_TEXT = 0x1E,
    
    //! ASCII Text (ASCII string with C-Style NULL terminator)
    ONA_TEXT = 0x1F,

    //! Incremental Energy Query 
    ONA_ENERGY = 0x20,
    
    //! Incremental Energy (in 1 Watt-Second units)
    ONA_ENERGY_1_WATT_SECONDS = 0x21,

    //! Incremental Energy (in 100 Watt-Seconds units)
    ONA_ENERGY_100_WATT_SECONDS = 0x22,

    //! Incremental Energy (in 10 Kilowatt-Seconds units)
    ONA_ENERGY_10_KWATT_SECONDS = 0x23,

    //! Incremental Energy (in 1000 Kilowatt-Seconds units)
    ONA_ENERGY_1000_KWATT_SECONDS = 0x24,

    //! Incremental Energy (in Kilowatt-Hour units)
    ONA_ENERGY_1_KWATT_HOUR = 0x25,

    //! Incremental Energy (in 100 Kilowatt-Hour units)
    ONA_ENERGY_100_KWATT_HOUR = 0x26,

    //! Incremental Energy (in 10,000 Kilowatt-Hour units)
    ONA_ENERGY_10000_KWATT_HOUR = 0x27,



    //! Accumulated Energy Query 
    ONA_ACCUM_ENERGY = 0x28,
    
    //! Accumulated Energy (in 1 Watt-Second units)
    ONA_ACCUM_ENERGY_1_WATT_SECONDS = 0x29,

    //! Accumulated Energy (in 100 Watt-Seconds units)
    ONA_ACCUM_ENERGY_100_WATT_SECONDS = 0x2A,

    //! Accumulated Energy (in 10 Kilowatt-Seconds units)
    ONA_ACCUM_ENERGY_10_KWATT_SECONDS = 0x2B,

    //! Accumulated Energy (in 1000 Kilowatt-Seconds units)
    ONA_ACCUM_ENERGY_1000_KWATT_SECONDS = 0x2C,

    //! Accumulated Energy (in Kilowatt-Hour units)
    ONA_ACCUM_ENERGY_1_KWATT_HOUR = 0x2D,

    //! Accumulated Energy (in 100 Kilowatt-Hour units)
    ONA_ACCUM_ENERGY_100_KWATT_HOUR = 0x2E,

    //! Accumulated Energy (in 10,000 Kilowatt-Hour units)
    ONA_ACCUM_ENERGY_10000_KWATT_HOUR = 0x2F,
    
    

    //! Peak Energy Query 
    ONA_PEAK_ENERGY = 0x30,
    
    //! Peak Energy (in 1 Watt-Second units)
    ONA_PEAK_ENERGY_1_WATT_SECONDS = 0x31,

    //! Peak Energy (in 100 Watt-Seconds units)
    ONA_PEAK_ENERGY_100_WATT_SECONDS = 0x32,

    //! Peak Energy (in 10 Kilowatt-Seconds units)
    ONA_PEAK_ENERGY_10_KWATT_SECONDS = 0x33,

    //! Peak Energy (in 1000 Kilowatt-Seconds units)
    ONA_PEAK_ENERGY_1000_KWATT_SECONDS = 0x34,

    //! Peak Energy (in Kilowatt-Hour units)
    ONA_PEAK_ENERGY_1_KWATT_HOUR = 0x35,

    //! Peak Energy (in 100 Kilowatt-Hour units)
    ONA_PEAK_ENERGY_100_KWATT_HOUR = 0x36,

    //! RPeak Energy (in 10,000 Kilowatt-Hour units)
    ONA_PEAK_ENERGY_10000_KWATT_HOUR = 0x37,
    
    

    //! Power Query 
    ONA_POWER = 0x38,
    
    //! Power (in 1 Watt units)
    ONA_POWER_1_WATT = 0x39,

    //! Power (in 100 Watt units)
    ONA_POWER_100_WATTS = 0x3A,

    //! Power (in 10 Kilowatt units)
    ONA_POWER_10_KWATTS = 0x3B,

    //! Power (in 1000 Kilowatt units)
    ONA_POWER_1000_KWATTS = 0x3C,




    //! Instantaneous Gas Query 
    ONA_GAS = 0x40,

    //! Instantaneous Gas Therm-Seconds
    ONA_GAS_THERM_SECS = 0x41,

    //! Instantaneous Gas Therm-Minutes
    ONA_GAS_THERM_MINS = 0x42,

    //! Instantaneous Gas Therm-Hours
    ONA_GAS_THERM_HRS = 0x43,

    //! Accumulated Gas Query 
    ONA_ACCUM_GAS = 0x44,

    //! Accumulated Gas Therm-Seconds
    ONA_ACCUM_GAS_THERM_SECS = 0x45,

    //! Accumulated Gas Therm-Minutes
    ONA_ACCUM_GAS_THERM_MINS = 0x46,

    //! Accumulated Gas Therm-Hours
    ONA_ACCUM_GAS_THERM_HRS = 0x47,

    //! Average Gas Query 
    ONA_AVER_GAS = 0x48,

    //! Average Gas Therm-Seconds
    ONA_AVER_GAS_THERM_SECS = 0x49,

    //! Average Gas Therm-Minutes
    ONA_AVER_GAS_THERM_MINS = 0x4A,

    //! Average Gas Therm-Hours
    ONA_AVER_GAS_THERM_HRS = 0x4B,

    //! Peak Gas Query 
    ONA_PEAK_GAS = 0x4C,

    //! Peak Gas Therm-Seconds
    ONA_PEAK_GAS_THERM_SECS = 0x4D,

    //! Peak Gas Therm-Minutes
    ONA_PEAK_GAS_THERM_MINS = 0x4E,

    //! Peak Gas Therm-Hours
    ONA_PEAK_GAS_THERM_HRS = 0x4F,
    
    
    // Servo and Motor Control, plus anything else.  See also the ONA_DIRECTION and
    // ONA_DIRECTION_RADIANS messages for use with servos.
    
    //! Angular Velocity -- Rotations per minute
    ONA_VELOCITY_RPM = 0x50,
    
    //! Angular Velocity -- Rotations per second
    ONA_VELOCITY_RPS = 0x51,
    
    //! Angular Velocity -- Rotations per hour
    ONA_VELOCITY_RPH = 0x52,
    
    //! Angular Velocity -- Thousandths of a rotation per minute
    ONA_VELOCITY_RPM_THOUSANDTHS = 0x53,
    
    //! Angular Velocity -- Thousandths of a rotation per second
    ONA_VELOCITY_RPS_THOUSANDTHS = 0x54,
    
    //! Angular Velocity -- Thousandths of a rotation per hour
    ONA_VELOCITY_RPH_THOUSANDTHS = 0x55,
    
    //! Angular Velocity -- Degrees per second
    ONA_VELOCITY_DEGREES_PER_SEC = 0x56,
    
    //! Angular Velocity -- Degrees per millisecond
    ONA_VELOCITY_DEGREES_PER_MILLISEC = 0x57,
    
    //! Angular Velocity -- Degrees per microsecond
    ONA_VELOCITY_DEGREES_PER_MICROSEC = 0x58,
    
    //! Angular Velocity -- Radians per second
    ONA_VELOCITY_RADIANS_PER_SEC = 0x59,
    
    //! Angular Velocity -- Degrees per millisecond
    ONA_VELOCITY_RADIANS_PER_MILLISEC = 0x5A,
    
    //! Angular Velocity -- Degrees per microsecond
    ONA_VELOCITY_RADIANS_PER_MICROSEC = 0x5B,
    
    
    
    //! Codes 0x5C to 0x7E are reserved for future use
    
    
    //! Unknown -- used when message type is unknown or irrelevant
    ONA_MSG_TYPE_UNKNOWN = 0x7F,
    
    
    //! Codes 0x80 to 0xBF cannot be contained in 20 bits and thus require special parsing
    ONA_MSG_TYPE_MIN_SPECIAL_PARSE = 0x80,
    
    //! The number of seconds since January 1, 1970. Corresponds to the time_t structure in C.
    ONA_SEC_SINCE_1970 = 0x80,
    
    //! Date: Month / Day / Year
    ONA_DATE = 0x81,
    
    //! The number of milliseconds since the program started
    ONA_PROGRAM_RUN_TIME_MS = 0x82,
    
    //! The number of seconds since the program started
    ONA_PROGRAM_RUN_TIME_SEC = 0x83,
    
    //! Color in 24-bit RGB format
    ONA_COLOR = 0x84,
    
    //! Codes 0x85 to 0xBF are reserved for future use.
    
    ONA_MSG_TYPE_MAX_SPECIAL_PARSE = 0xBF,
    
    
    //! Codes 0xC0 to 0xFF are application-specific types. Application code writers can
    //! add codes here starting with 0xC0 if none of the other types match their needs
    ONA_MSG_TYPE_MIN_APLICATION = 0xC0,
    
    ONA_MSG_TYPE_MAX_APLICATION = 0xFF,
} ona_msg_type_t;


//! Packet related constants
enum
{
    //! Length of preamble and header
    ONE_NET_PREAMBLE_HEADER_LEN = 4,
    
    //! Length of encoded message CRC
    ONE_NET_ENCODED_MSG_CRC_LEN = 1,
    
    //! The size of the encoded Packet ID field
    ON_ENCODED_PID_SIZE = 2,

    //! The number of bytes required to store the raw hops field
    ON_RAW_HOPS_SIZE = 1,

    //! The size of the encoded hops field (in bytes)
    ON_ENCODED_HOPS_SIZE = 1,    
    
    //! The index into the encoded packet where the repeater did lies
    ON_ENCODED_RPTR_DID_IDX = ONE_NET_PREAMBLE_HEADER_LEN,
    
    //! The index into the encoded packet where the message CRC lies
    ON_ENCODED_MSG_CRC_IDX = ON_ENCODED_RPTR_DID_IDX + ON_ENCODED_DID_LEN,
        
    ON_ENCODED_DST_DID_IDX = ON_ENCODED_MSG_CRC_IDX + ONE_NET_ENCODED_MSG_CRC_LEN,

    //! The index into the encoded packet where the NID starts.
    ON_ENCODED_NID_IDX = ON_ENCODED_DST_DID_IDX + ON_ENCODED_DID_LEN,

    //! The index into the encoded packet where the source DID starts.
    ON_ENCODED_SRC_DID_IDX = ON_ENCODED_NID_IDX + ON_ENCODED_NID_LEN,

    //! The index into the encoded packet where the PID starts.
    ON_ENCODED_PID_IDX = ON_ENCODED_SRC_DID_IDX + ON_ENCODED_DID_LEN,

    //! The index into the encoded packet where the payload starts
    ON_ENCODED_PLD_IDX = ON_ENCODED_PID_IDX + ON_ENCODED_PID_SIZE
};


#ifdef ONE_NET_MULTI_HOP
//! Hops field related constants
enum
{
    //! The number of bits to shift the max hops field
    ON_MAX_HOPS_BUILD_SHIFT = 2,

    //! Number of bits to shift the hops field
    ON_HOPS_BUILD_SHIFT = 5,

    //! Mask to use on the max hops field when building the packet
    ON_MAX_HOPS_BUILD_MASK = 0x1C,

    //! Mask to use on the hops field when building the packet
    ON_HOPS_BUILD_MASK = 0xE0,

    //! Shift to use when parsing hops
    ON_PARSE_HOPS_SHIFT = 5,

    //! Shift to use when parsing max hops
    ON_PARSE_MAX_HOPS_SHIFT = 2,
    
    //! Mask to use when parsing hops / max hops once shifted
    ON_PARSE_RAW_HOPS_FIELD_MASK = 0x07
};
#endif


enum
{
    ON_ACK_NACK_ENCODED_PLD_LEN = 11,
    #ifdef EXTENDED_SINGLE
    ON_LARGE_ACK_NACK_ENCODED_PLD_LEN = 22,
    ON_EXTENDED_ACK_NACK_ENCODED_PLD_LEN = 33,
    ON_MAX_ENCODED_ACK_NACK_LEN = ON_EXTENDED_ACK_NACK_ENCODED_PLD_LEN,
    #else
    ON_MAX_ENCODED_ACK_NACK_LEN = ON_ACK_NACK_ENCODED_PLD_LEN,
    #endif
    
    
    ON_SINGLE_ENCODED_PLD_LEN = 11,
    #ifdef EXTENDED_SINGLE
    ON_LARGE_SINGLE_ENCODED_PLD_LEN = 22,
    ON_EXTENDED_SINGLE_ENCODED_PLD_LEN = 33,
    ON_MAX_ENCODED_SINGLE_LEN = ON_EXTENDED_SINGLE_ENCODED_PLD_LEN,
    #else
    ON_MAX_ENCODED_SINGLE_LEN = ON_SINGLE_ENCODED_PLD_LEN,
    #endif
    
    
    ON_INVITE_ENCODED_PLD_LEN = 33,
    #ifdef BLOCK_MESSAGES_ENABLED
    ON_BLOCK_ENCODED_PLD_LEN = 43,
    #endif
    #ifdef STREAM_MESSAGES_ENABLED
    ON_STREAM_ENCODED_PLD_LEN = 43,
    #endif
    
    #ifdef BLOCK_MESSAGES_ENABLED
        //! The maximum length of a raw payload field (not including the extra
        //! byte needed to store the 2 bits for the encryption method type).
        ON_MAX_ENCODED_PLD_LEN = ON_BLOCK_ENCODED_PLD_LEN - 1,
    #else // if block messages are defined //
        //! The maximum length of a raw payload field (not including the extra
        //! byte needed to store the 2 bits for the encryption method type).
        ON_MAX_ENCODED_PLD_LEN = ON_INVITE_ENCODED_PLD_LEN - 1,
    #endif // else if block messages are not defined //
    
    ON_MAX_ENCODED_PLD_LEN_WITH_TECH = ON_MAX_ENCODED_PLD_LEN + 1,
    
    ON_INVITE_RAW_PLD_LEN = 24,
    ON_INVITE_RAW_PLD_LEN_WITH_TECH = ON_INVITE_RAW_PLD_LEN + 1,
    
    #ifdef BLOCK_MESSAGES_ENABLED
    ON_BLOCK_RAW_PLD_LEN = 32,
    ON_BLOCK_RAW_PLD_LEN_WITH_TECH = ON_BLOCK_RAW_PLD_LEN + 1,
    ON_MAX_RAW_PLD_LEN = ON_BLOCK_RAW_PLD_LEN,
    #else
    ON_MAX_RAW_PLD_LEN = ON_INVITE_RAW_PLD_LEN,
    #endif
    ON_MAX_RAW_PLD_LEN_WITH_TECH = ON_MAX_RAW_PLD_LEN + 1
};


enum
{
    ON_ACK_NACK_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_ACK_NACK_ENCODED_PLD_LEN,
    #ifdef EXTENDED_SINGLE
    ON_LARGE_ACK_NACK_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_LARGE_ACK_NACK_ENCODED_PLD_LEN,
    ON_EXTENDED_ACK_NACK_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_EXTENDED_ACK_NACK_ENCODED_PLD_LEN,
    #endif
    ON_MAX_ACK_NACK_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_MAX_ENCODED_ACK_NACK_LEN,


    ON_SINGLE_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_SINGLE_ENCODED_PLD_LEN,
    #ifdef EXTENDED_SINGLE
    ON_LARGE_SINGLE_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_LARGE_SINGLE_ENCODED_PLD_LEN,
    ON_EXTENDED_SINGLE_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_EXTENDED_SINGLE_ENCODED_PLD_LEN,
    #endif    
    ON_MAX_SINGLE_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_MAX_ENCODED_SINGLE_LEN,
    
    
    ON_INVITE_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_INVITE_ENCODED_PLD_LEN,
    #ifdef BLOCK_MESSAGES_ENABLED
    ON_BLOCK_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_BLOCK_ENCODED_PLD_LEN,
    #endif
    #ifdef STREAM_MESSAGES_ENABLED
    ON_STREAM_ENCODED_PKT_LEN = ON_ENCODED_PLD_IDX + ON_STREAM_ENCODED_PLD_LEN
    #endif
};


enum
{
    #ifdef ONE_NET_MULTI_HOP
        ON_ACK_NACK_ENCODED_PKT_SIZE = ON_MAX_ACK_NACK_ENCODED_PKT_LEN + ON_ENCODED_HOPS_SIZE,
        ON_SINGLE_ENCODED_PKT_SIZE = ON_MAX_SINGLE_ENCODED_PKT_LEN + ON_ENCODED_HOPS_SIZE,
        #ifdef BLOCK_MESSAGES_ENABLED
        ON_BLOCK_ENCODED_PKT_SIZE = ON_BLOCK_ENCODED_PKT_LEN + ON_ENCODED_HOPS_SIZE,
        #endif
        #ifdef STREAM_MESSAGES_ENABLED
        ON_STREAM_ENCODED_PKT_SIZE = ON_STREAM_ENCODED_PKT_LEN + ON_ENCODED_HOPS_SIZE,
        #endif
        ON_INVITE_ENCODED_PKT_SIZE = ON_INVITE_ENCODED_PKT_LEN + ON_ENCODED_HOPS_SIZE,              
    #else // ifdef ONE_NET_MULTI_HOP //
        ON_ACK_NACK_ENCODED_PKT_SIZE = ON_MAX_ACK_NACK_ENCODED_PKT_LEN,
        ON_SINGLE_ENCODED_PKT_SIZE = ON_MAX_SINGLE_ENCODED_PKT_LEN,
        #ifdef BLOCK_MESSAGES_ENABLED
        ON_BLOCK_ENCODED_PKT_SIZE = ON_BLOCK_ENCODED_PKT_LEN,
        #endif
        #ifdef STREAM_MESSAGES_ENABLED
        ON_STREAM_ENCODED_PKT_SIZE = ON_STREAM_ENCODED_PKT_LEN,
        #endif
        ON_INVITE_ENCODED_PKT_SIZE = ON_INVITE_ENCODED_PKT_LEN,
    #endif // else ONE_NET_MULTI_HOP is not defined //
    
    ON_MIN_ENCODED_PKT_SIZE = ON_ENCODED_PLD_IDX + 11,
    #if defined(BLOCK_MESSAGES_ENABLED)
    ON_MAX_ENCODED_PKT_SIZE = ON_BLOCK_ENCODED_PKT_SIZE,
    #else
    ON_MAX_ENCODED_PKT_SIZE = ON_INVITE_ENCODED_PKT_SIZE,
    #endif
    
    #ifdef BLOCK_MESSAGES_ENABLED
    ON_MAX_ENCODED_DATA_PKT_SIZE = ON_BLOCK_ENCODED_PKT_SIZE
    #else
    ON_MAX_ENCODED_DATA_PKT_SIZE = ON_SINGLE_ENCODED_PKT_SIZE
    #endif
};


//! Nonce, Message type, Payload indexes, masks, shifts
enum
{
    //! Index into the payload where the crc starts
    ON_PLD_CRC_IDX = 0,
    
    //! Index into the payload of the Message ID
    ON_PLD_MSG_ID_IDX = 1,

    //! Index for the ack / nack handle
    ON_PLD_RESP_HANDLE_IDX = 2,

    //! Index for the message type
    ON_PLD_MSG_TYPE_IDX = 2,

    //! Index for the data portion
    ON_PLD_DATA_IDX = 3,
    
    //! Index for the admin type in an admin message
    ON_PLD_ADMIN_TYPE_IDX = ON_PLD_DATA_IDX,
    
    //! Index of the data portion of an admin message
    ON_PLD_ADMIN_DATA_IDX = ON_PLD_ADMIN_TYPE_IDX + 1,

    //! The mask to use for the handle when
    //! building the payload field of a data packet
    ON_RESP_HANDLE_BUILD_MASK = 0x0F,

    //! The mask to use for the message type when building or parsing the
    //! payload field of a data packet
    ON_PLD_MSG_TYPE_MASK = ON_RESP_HANDLE_BUILD_MASK
};


enum
{
    //! Index for the payload chunk index and size
    ON_BS_PLD_CHUNK_IDX = ON_PLD_MSG_TYPE_IDX,
    
    //! Index for the byte index in a block payload
    ON_BS_PLD_BYTE_IDX = 4,

    //! Index for the time in a stream payload
    ON_BS_STREAM_TIME_IDX = ON_BS_PLD_BYTE_IDX,
    
    //! Index for the "response needed" field in a stream payload    
    ON_BS_STREAM_RESPONSE_NEEDED_IDX = ON_BS_STREAM_TIME_IDX - 1,

    //! Index for the data
    ON_BS_DATA_PLD_IDX = ON_BS_PLD_BYTE_IDX + 3,
    
    //! Data size in a block / stream data packet
    ON_BS_DATA_PLD_SIZE = 4 * ONE_NET_XTEA_BLOCK_SIZE - ON_BS_DATA_PLD_IDX
};


//! Admin packet indexes
enum
{
    #ifdef BLOCK_MESSAGES_ENABLED
    //! Index for the low priority fragment delay in an admin message
    //! containing both fragment delays
    ON_FRAG_LOW_IDX = 0,

    //! Index for the high priority fragment delay in an admin message
    //! containing both fragment delays
    ON_FRAG_HIGH_IDX = 2,
    #endif

    //! Index for encoded did of peer device for (un)assignment of peers
    ON_PEER_DID_IDX = 0,

    //! Index for peer unit for (un)assignment of peers.  This is the unit in
    //! the peer device being assigned to a unit in this device
    ON_PEER_PEER_UNIT_IDX = 2,

    //! Index for destination unit for (un)assignment of peers.  This is the
    //! unit in the device that is being assigned a peer
    ON_PEER_SRC_UNIT_IDX = 3,
};


/* Normal Single message payload constants */
enum
{
    //! Five bytes are as follows...
    //!
    //! Byte 0 -- Message type
    //! Byte 1 -- 4 MSB bits are the message class, 4 LSB bits are the source unit.
    //! Byte 2 -- 4 MSB bytes are the destination unit.
    //!
    //! Bytes 2 thru 4 -- 20 bits.  Message data.  The "data" of the message.
    //!           For a normal switch message, this would be ONA_ON, ONA_OFF, or
    //!           ONA_TOGGLE.
    //!
    //!           The MSB bit is the sign bit (0 for non-negative, 1 for negative).
    //!           Negative numbers are NOT sent in 2's complement.  Instead, there
    //!           is a sign bit, followed by the absolute value of the data.  For
    //!           example, 9 and -9 would be sent as follows...
    //!
    //!           00000000000000001001   (9)
    //!           10000000000000001001   (-9, same as 9 except that left-mostbit is
    //!                                   1, denoting a negative number)
    //!
    //!
    //!
    //! So a "Turn switch on command from unit 4 to unit 6 would be as follows
    //!
    //! Message Type = ONA_SWITCH   = 0 = 00000000
    //! Message Class = ONA_COMMAND = 5 = 0101
    //! Source Unit                 = 4 = 0100
    //! Destination Unit            = 6 = 0110
    //! Message Data = ONA_ON       = 1 = 00000000000000000001 (20 bits)

    //!
    //!
    //! 5 byte message is...
    //!
    //! TTTTTTTTCCCCSSSSDDDDXXXXXXXXXXXXXXXXXXXX
    //! 0000000001010100011000000000000000000001
    //!
    //! which is 0x0054600001 in Hex
    //!
    //! T = Message Type
    //! C = Message Class
    //! S = Source Unit
    //! D = Dest. Unit
    //! X = Message Data
    //!
    //! one_net_application.h defines "getters" and "setters" for source
    //! unit, destination unit, header, and data.
    //!
    //! 1) put_src_unit and get_src_unit
    //! 2) put_dst_unit and get_dst_unit
    //! 3) put_msg_class and get_msg_class
    //! 4) put_msg_type and get_msg_type
    //! 5) put_msg_data and get_msg_data
    //!
    //!
    //! To place the message above into a 5 byte payload, the following
    //! code may be used...
    //!
    //!
    //!   UInt8 payload[5];
    //!   put_src_unit(4, payload);
    //!   put_dst_unit(6, payload);
    //!   put_msg_type(ONA_SWITCH, payload);
    //!   put_msg_class(ONA_COMMAND, payload);
    //!   put_msg_data(ONA_ON, ON_APP_MSG, payload);
    //!
    //!
    //!   The "get" functions work in precisely in the reverse, so they
    //!   take a five byte payload and parse it.
    //!
    //!
    //!   This 5-byte payload is specified in the ONE-NET specification and
    //!   should be used whenever for all "ON_APP_MSG" messages.  Users can send
    //!   5-byte payloads which do not follow this layout, but they must not
    //!   specify "ON_APP_MSG" when creating the message.  Not ethat there are
    //!   many message types that do not take full advantage of all 20 message
    //!   data bytes.  Examples are switch messages, which use only 2, and text
    //!   messages, which use 16 of the 20.  In fact, the majority of messages
    //!   will not use all 20 bits, but some do.  If this is the case, it may
    //!   be useful to change the code so taht message data does not use a
    //!   UInt32, but rather a UInt8 or a UInt16.  That can save stack space and
    //!   memory.
    
    //!   The above corresponds to parsing using the ON_APP_MSG parsing technique.
    
    //!   Messages using the ON_APP_MSG_TYPE_2 parsing technique will not contain the
    //!   source and destination units.  In this case, the message data will be 28 bits,
    //!   not 20 bits, represented as follows.
    
    //! 5 byte message is...
    //!
    //! TTTTTTTTCCCCXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    //! 0000000001010000000000000000000000000001
    //!
    //! which is 0x0050000001 in Hex
    //!
    //! C = Message Class
    //! T = Message Type
    //! X = Message Data
    //!
    //! ON_APP_MSG_TYPE_3 contains the message type, but no message class.
    //! ON_APP_MSG_TYPE_4 is simply data.  There is no type, class, or units.  Only
    //! application-level functions can parse this
    


    ONA_MSG_SRC_UNIT_IDX   = 1,     // Where the byte is
    ONA_MSG_SRC_UNIT_MASK  = 0x0F,  // Where the bits are in the byte
    ONA_MSG_SRC_UNIT_SHIFT = 0,     // Shift left this much to put them in

    ONA_MSG_DST_UNIT_IDX   = 2,     // Where the byte is
    ONA_MSG_DST_UNIT_MASK  = 0xF0,  // Where the bits are in the byte
    ONA_MSG_DST_UNIT_SHIFT = 4,     // Shift left this much to put them in

    ONA_MSG_TYPE_IDX = 0,
    ONA_MSG_CLASS_IDX = 1,

    //! Index of Message Data within payload
    ONA_TEXT_DATA_IDX = 3,

    //! Length of Message Data
    ONA_SIMPLE_TEXT_DATA_LEN = 2,
};


//! constants dealing with the raw payload of a data packet (i.e. does not
//! include msg id, crc, or nack reason.
enum
{
    ONA_DATA_INDEX = 3, //! the index in a data packet where the actual
                        //! data starts
    ONA_SINGLE_PACKET_PAYLOAD_LEN = ONE_NET_XTEA_BLOCK_SIZE -
      ONA_DATA_INDEX, //! the number of data bytes in a single message
    
    #ifdef EXTENDED_SINGLE
    ONA_LARGE_SINGLE_PACKET_PAYLOAD_LEN = ONA_SINGLE_PACKET_PAYLOAD_LEN +
      ONE_NET_XTEA_BLOCK_SIZE, //! the number of data bytes in a large
                               //! single message
    ONA_EXTENDED_SINGLE_PACKET_PAYLOAD_LEN = ONA_LARGE_SINGLE_PACKET_PAYLOAD_LEN +
      ONE_NET_XTEA_BLOCK_SIZE, //! the number of data bytes in an extended
                               //! single message
                               
    ONA_MAX_SINGLE_PACKET_PAYLOAD_LEN = ONA_EXTENDED_SINGLE_PACKET_PAYLOAD_LEN
    #else
    ONA_MAX_SINGLE_PACKET_PAYLOAD_LEN = ONA_SINGLE_PACKET_PAYLOAD_LEN    
    #endif
};


//! Invite related constants
enum
{
    //! The length (in bytes) of the raw invite request packet.  The 25th byte
    //! contains the 2 bits for the method used to encrypt the data with.
    ON_RAW_INVITE_SIZE = 25,

    //! The length (in bytes) of the encoded invite portion of the invite
    //! request packet.  This includes the extra byte needed to store the 2 bits
    //! for the encryption type.  This is also the size of the invite in 6 bit
    //! words
    ON_ENCODED_INVITE_SIZE = 33,

    //! The number of bytes to compute the crc over in the invite message
    ON_INVITE_DATA_LEN = 23,

    //! The version index
    ON_INVITE_VERSION_IDX = 1,

    //! The assigned DID index
    ON_INVITE_ASSIGNED_DID_IDX = 2,

    //! The key index
    ON_INVITE_KEY_IDX = 4,

    //! The features index
    ON_INVITE_FEATURES_IDX = 20,

    //! The crc index
    ON_INVITE_CRC_IDX = 0,
    
    //! The index to start computing the CRC over
    ON_INVITE_CRC_START_IDX = ON_INVITE_CRC_IDX + 1
};


typedef struct
{
    UInt8* packet_bytes;
    UInt16 raw_pid;  //! raw pid of the packet
    UInt16 msg_id; //! raw message id of the packet (0 to 4095)
    UInt8 payload_len; //! length of the encoded payload in bytes
    
    // TODO -- can we get rid of these 2 hops fields?
    #ifdef ONE_NET_MULTI_HOP
    UInt8 hops; //! hops of the packet.  May or may not be relevant.
    UInt8 max_hops; //! Maximum hops of the packet.  May or may not be relevant
    #endif
} on_pkt_t;


//! @} ONE-NET_PACKET_typedefs
//                                  TYPEDEFS END
//==============================================================================


//==============================================================================
//                                  CONSTANTS
//! \defgroup ONE-NET_PACKET_const
//! \ingroup ONE-NET_PACKET
//! @{

//! @} ONE-NET_PACKET_const
//                                  CONSTANTS END
//==============================================================================


#ifdef __cplusplus
extern "C"
{
#endif


//==============================================================================
//                      PUBLIC FUNCTION DECLARATIONS
//! \defgroup ONE-NET_PACKET_pub_func
//! \ingroup ONE-NET_PACKET
//! @{

SInt8 get_encoded_payload_len(UInt16 raw_pid);
SInt8 get_raw_payload_len(UInt16 raw_pid);
SInt8 get_num_payload_blocks(UInt16 raw_pid);
UInt8 get_encoded_packet_len(UInt16 raw_pid, BOOL include_header);
BOOL set_ack_or_nack_pid(UInt16* raw_pid, BOOL is_ack);

#ifdef ONE_NET_MULTI_HOP
BOOL set_multihop_pid(UInt16* raw_pid, BOOL is_multihop);
#endif
BOOL packet_is_data(UInt16 raw_pid);

BOOL packet_is_ack(UInt16 raw_pid);
BOOL packet_is_nack(UInt16 raw_pid);

SInt16 get_single_response_pid(UInt16 raw_single_pid, BOOL isACK,
  BOOL stay_awake);
  
BOOL packet_length_is_valid(UInt16 raw_pid);
SInt8 get_default_num_blocks(UInt16 raw_pid);



// inline function implementation below //


#ifdef ONE_NET_MULTI_HOP
/*!
    \brief Determines whether a given PID represents a multi-hop packet.

    Determines whether a given PID represents a multi-hop packet.

    \param[in] raw_pid The pid to check

    \return True if encoded_pid is a multi-hop packet, false otherwise.
*/
ONE_NET_INLINE BOOL packet_is_multihop(UInt16 raw_pid)
{
    return ((raw_pid & ONE_NET_RAW_PID_MH_MASK) > 0);
}
#endif


/*!
    \brief Determines whether a given PID represents a stay-awake packet.

    Determines whether a given PID represents a stay-awake packet.

    \param[in] raw_pid The pid to check

    \return True if encoded_pid is a stay-awake packet, false otherwise.
*/
ONE_NET_INLINE BOOL packet_is_stay_awake(UInt16 raw_pid)
{
    return ((raw_pid & ONE_NET_RAW_PID_STAY_AWAKE_MASK) > 0);
}


/*!
    \brief Determines whether a given PID represents a single packet.

    Determines whether a given PID represents a single packet.

    \param[in] raw_pid The pid to check

    \return True if pid is a single packet, false otherwise.
*/
ONE_NET_INLINE BOOL packet_is_single(UInt16 raw_pid)
{
    raw_pid &= 0x3F;
    #ifdef ROUTE
    return (raw_pid <= ONE_NET_RAW_ROUTE_NACK);
    #else
    return (raw_pid <= ONE_NET_RAW_SINGLE_DATA_NACK_RSN);
    #endif
}


#ifdef BLOCK_MESSAGES_ENABLED
/*!
    \brief Determines whether a given PID represents a block packet.

    Determines whether a given PID represents a block packet.

    \param[in] raw_pid The pid to check

    \return True if pid is a block packet, false otherwise.
*/
ONE_NET_INLINE BOOL packet_is_block(UInt16 raw_pid)
{
    raw_pid &= 0x3F;
    return (raw_pid >= ONE_NET_RAW_BLOCK_DATA &&
      raw_pid <= ONE_NET_RAW_BLOCK_TERMINATE);
}
#endif


#ifdef STREAM_MESSAGES_ENABLED
/*!
    \brief Determines whether a given PID represents a stream packet.

    Determines whether a given PID represents a stream packet.

    \param[in] raw_pid The pid to check

    \return True if pid is a stream packet, false otherwise.
*/
ONE_NET_INLINE BOOL packet_is_stream(UInt16 raw_pid)
{
    raw_pid &= 0x3F;
    return (raw_pid >= ONE_NET_RAW_STREAM_DATA &&
      raw_pid <= ONE_NET_RAW_STREAM_TERMINATE);
}
#endif


/*!
    \brief Determines whether a given PID represents an invite packet.

    Determines whether a given PID represents an invite packet.

    \param[in] raw_pid The pid to check

    \return True if pid is an invite packet, false otherwise.
*/
ONE_NET_INLINE BOOL packet_is_invite(UInt16 raw_pid)
{
    raw_pid &= 0x3F;
    return (raw_pid == ONE_NET_RAW_MASTER_INVITE_NEW_CLIENT);
}


#ifdef ROUTE
ONE_NET_INLINE BOOL packet_is_route(UInt16 raw_pid)
{
    raw_pid &= 0x3F;
    return (raw_pid >= ONE_NET_RAW_ROUTE && raw_pid <= ONE_NET_RAW_ROUTE_NACK);
}
#endif


// don't #include one_net_port_specific.h here due to some circular
// dependecies.  Just declare a few functions from it here to avoid compiler
// errors.

UInt16 one_net_byte_stream_to_uint16(const UInt8 * const BYTE_STREAM);
void one_net_uint16_to_byte_stream(const UInt16 VAL, UInt8 * const byte_stream);
UInt32 one_net_byte_stream_to_uint32(const UInt8 * const BYTE_STREAM);
void one_net_uint32_to_byte_stream(const UInt32 VAL, UInt8 * const byte_stream);


BOOL get_raw_pid(const UInt8* payload, UInt16* raw_pid);
void put_raw_pid(UInt8* payload, UInt16 raw_pid);

    
    
//! @} ONE-NET_PACKET_pub_func
//                      PUBLIC FUNCTION DECLARATIONS END
//==============================================================================


#ifdef __cplusplus
}
#endif


//! @} ONE-NET_PACKET

#endif // ONE_NET_PACKET_H //
