//! \addtogroup SPI SPI Driver for the R8C series microcontroller.
//! \ingroup PAL
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
    \file spi.c
    \brief Driver for the spi port on the R8C series microcontroller.
*/

#include "one_net_types.h"
#include "pal.h"


//=============================================================================
//                                  CONSTANTS
//! \defgroup SPI_const
//! \ingroup SPI
//! @{

//! @} SPI_const
//                                  CONSTANTS END
//=============================================================================

//=============================================================================
//                                  TYPEDEFS
//! \defgroup SPI_typedefs
//! \ingroup SPI
//! @{

//! @} SPI_typedefs
//                                  TYPEDEFS END
//=============================================================================

//=============================================================================
//                              PRIVATE VARIABLES
//! \defgroup SPI_pri_var
//! \ingroup SPI
//! @{

//! @} SPI_pri_var
//                              PRIVATE VARIABLES END
//=============================================================================

//=============================================================================
//                      PRIVATE FUNCTION DECLARATIONS
//! \defgroup SPI_pri_func
//! \ingroup SPI
//! @{

//! @} SPI_pri_func
//                      PRIVATE FUNCTION DECLARATIONS END
//=============================================================================

//=============================================================================
//                      PUBLIC FUNCTION IMPLEMENTATION
//! \defgroup SPI_pub_func
//! \ingroup SPI
//! @{

/*!
    \brief Initializes the SPI port
    
    \param[in] MASTER TRUE if this device is to be the SPI master
                      FALSE if this device is to be a SPI slave
               CLK_POLARITY 0 for clock polarity 0 (read on rising edge, change
                              on falling edge).
                            !0 for clock polarity 1 (change on rising edge, read
                              on falling edge).
               PHASE 0 for cpha = 0 (data present at first edge)
                     1 for cpha = 1 (data changed at first edge)
               MSB TRUE if sending most significant bit first.
                   FALSE if sending least significant bit first.
*/
void init_spi(const BOOL MASTER, const UInt8 CLK_POLARITY, const UInt8 PHASE,
  const BOOL MSB)
{
    #ifdef _BIT_BANG
        SCLK_DIR = OUTPUT;
        SCLK = 0;
        MOSI_DIR = OUTPUT;
        MISO_DIR = INPUT;
    #else
        re_sser = 0;
        te_sser = 0;

        if(MASTER)
        {
            sscrh = 0x26;
        } // if MASTER device //
        else
        {
            sscrh = 0x00;
        } // else SLAVE device //
        sscrl = 0x00;
    
        if(CLK_POLARITY)
        {
            cpos_ssmr = 0;
        } // if cpol == 1 //
        else
        {
            cpos_ssmr = 1;
        } // else cpol == 0 //
    
        if(PHASE)
        {
            cphs_ssmr = 0;
        } // if cpha == 1 //
        else
        {
            cphs_ssmr = 1;
        } // else cpha == 0 //
    
        if(MSB)
        {
            mls_ssmr = 0;
        } // if sending MSb //
        else
        {
            mls_ssmr = 1;
        } // else sending LSb //
    
        ssmr2 = 0x40;
        sser = 0x08;
        pmr = 0x08;
    #endif
} // init_spi //


/*
    \brief Transmits &/or receives SPI Data
    
    The SSNOT pin of the slave device needs to be deasserted before this
    function is called, and asserted after the function is complete.  This
    function does a full duplex transmit of SPI data.  For half duplex, call
    this function with only data to be sent, or data to be received.  0's are
    transmitted in the receive only case.
    
    \param[in] TX_DATA The data to send (if sending).
    \param[in] TX_LEN The number of bytes to send.
    \param[out] rx_data The data to receive (if receiving).
    \param[in] RX_LEN The number of bytes to receive.
    
    \return TRUE if the transfer was successful
            FALSE if the transfer was unsuccessful
*/
BOOL spi_transfer(const UInt8 * const TX_DATA, const UInt8 TX_LEN,
  UInt8 * const rx_data, const UInt8 RX_LEN)
{
    #ifdef _BIT_BANG
        // the number of bytes to be transferred (greater of TX_LEN, RX_LEN)
        const UInt8 TRANSFER_LEN = TX_LEN > RX_LEN ? TX_LEN : RX_LEN;

        UInt8 i;
        UInt8 bitmap;

        // values used to avoid indexing in the loop that sends individual bits    
        UInt8 tx_byte, rx_byte;

        if((TX_LEN && !TX_DATA) || (RX_LEN && !rx_data))
        {
            return FALSE;
        } // if the paramters are invalid //

        // By adding tx_byte, rx_byte and not indexing inside the loop that
        // sends the individual bits, saves over 25% of the time it takes to
        // send 1 byte.
        for(i = 0; i < TRANSFER_LEN; i++)
        {
            if(i < TX_LEN)
            {
                tx_byte = TX_DATA[i];
            } // if more data to transmit //
            else
            {
                tx_byte = 0;
            } // else no more data to transmit //

            rx_byte = 0;
            for(bitmap = 0x80; bitmap; bitmap >>= 1)
            {
                MOSI = (tx_byte & bitmap) ? 1 : 0;
                SCLK = 1;
                if(i < RX_LEN)
                {
                    rx_byte = (rx_byte << 1) | MISO;
                } // if receiving data //
                SCLK = 0;
            } // loop to bitbang the bits //
            
            if(i < RX_LEN)
            {
                rx_data[i] = rx_byte;
            } // if more data to receive //
        } // loop through all the bytes to transfer //

        return TRUE;
    #else
        // the number of bytes to be transferred (greater of TX_LEN, RX_LEN)
        const UInt8 TRANSFER_LEN = TX_LEN > RX_LEN ? TX_LEN : RX_LEN;

        UInt8 i;
    
        // read unwanted values into read_byte & discard
        UInt8 read_byte;

        if((TX_LEN && !TX_DATA) || (RX_LEN && !rx_data))
        {
            return FALSE;
        } // if the paramters are invalid //
 
        re_sser = 1;
        te_sser = 1;

        for(i = 0; i < TRANSFER_LEN; i++)
        {
            // wait for transmit data register to be ready
            while(!tdre_sssr);
            sstdr = i < TX_LEN ? TX_DATA[i] : 0x00;

            // wait for data to be received
            while(!rdrf_sssr);
            if(i < RX_LEN)
            {
                rx_data[i] = ssrdr;
            } // if receiving more data //
            else
            {
                read_byte = ssrdr;
            } // no more data to receive //
        } // transfer loop //

        // make sure the transmission is complete
        while(!tend_sssr);
    
        tend_sssr = 1;
        re_sser = 0;
        te_sser = 0;
    
        return TRUE;
    #endif
} // spi_transfer //

//! @} SPI_pub_func
//                      PUBLIC FUNCTION IMPLEMENTATION END
//=============================================================================

//=============================================================================
//                      PRIVATE FUNCTION IMPLEMENTATION
//! \addtogroup SPI_pri_func
//! \ingroup SPI
//! @{

//! @} SPI_pri_func
//                      PRIVATE FUNCTION IMPLEMENTATION END
//=============================================================================

//! @} SPI
