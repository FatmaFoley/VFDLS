/*
 * uart.c
 *
 *  Created on: Nov 1, 2025
 *      Author: Fatma Foley
 */

#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include <util/delay.h>
#include "common_macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/


/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr)
{
    uint16 ubrr_value = 0;

    /* Configure frame format: stop bits */
    UCSRC = ((1<<USBS) | Config_Ptr->stop_bit);

    /* Configure parity mode bits */
    switch(Config_Ptr->parity)
    {
        case PARITY_DISABLED :
            CLEAR_BIT(UCSRC, UPM0);
            CLEAR_BIT(UCSRC, UPM1);
            break;
        case PARITY_EVEN :
            CLEAR_BIT(UCSRC, UPM0);
            SET_BIT(UCSRC, UPM1);
            break;
        case PARITY_ODD :
            SET_BIT(UCSRC, UPM0);
            SET_BIT(UCSRC, UPM1);
            break;
    }

    /* Configure data bits size */
    switch(Config_Ptr->bit_data)
    {
        case BIT_DATA_5 :
            CLEAR_BIT(UCSRC, UCSZ0);
            CLEAR_BIT(UCSRC, UCSZ1);
            CLEAR_BIT(UCSRB, UCSZ2);
            break;
        case BIT_DATA_6 :
            SET_BIT(UCSRC, UCSZ0);
            CLEAR_BIT(UCSRC, UCSZ1);
            CLEAR_BIT(UCSRB, UCSZ2);
            break;
        case BIT_DATA_7 :
            CLEAR_BIT(UCSRC, UCSZ0);
            SET_BIT(UCSRC, UCSZ1);
            CLEAR_BIT(UCSRB, UCSZ2);
            break;
        case BIT_DATA_8 :
            SET_BIT(UCSRC, UCSZ0);
            SET_BIT(UCSRC, UCSZ1);
            CLEAR_BIT(UCSRB, UCSZ2);
            break;
        case BIT_DATA_9 :
            SET_BIT(UCSRC, UCSZ0);
            SET_BIT(UCSRC, UCSZ1);
            SET_BIT(UCSRB, UCSZ2);
            break;
    }

    /* Enable double transmission speed */
    UCSRA = (1<<U2X);

    /************************** UCSRB Description **************************
     * RXCIE = 0 Disable USART RX Complete Interrupt Enable
     * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
     * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
     * RXEN  = 1 Receiver Enable
     * TXEN  = 1 Transmitter Enable
     * UCSZ2 = 0 For 8-bit data mode (can be set for others)
     ***********************************************************************/
    UCSRB = (1<<RXEN) | (1<<TXEN);

    /************************** UCSRC Description **************************
     * URSEL   = 1 The URSEL must be one when writing the UCSRC to select UCSRC register
     * UMSEL   = 0 Asynchronous Operation
     * UPM1:0  = 00 Disable parity bit (set above as needed)
     * USBS    = 0 One stop bit (set above as needed)
     * UCSZ1:0 = 11 For 8-bit data mode (set above as needed)
     * UCPOL   = 0 Used only in synchronous mode
     ***********************************************************************/
    UCSRC |= (1<<URSEL);

    /* Calculate and set baud rate register value (for double speed mode) */
    ubrr_value = (uint16)(((F_CPU / (Config_Ptr->baud_rate * 8UL))) - 1);

    /* Assign baud rate values to registers */
    UBRRH = ubrr_value >> 8;
    UBRRL = ubrr_value;
}

/*
 * Description :
 * Function responsible for sending a byte via UART.
 */
void UART_sendByte(const uint8 data)
{
    /*
     * Put the required data in the UDR register and it also clears the UDRE flag
     * since the UDR register is not empty now.
     */
    UDR = data;

    /*
     * Wait until UDRE flag is set, indicating the buffer is empty and
     * new data can be sent.
     */
    while(BIT_IS_CLEAR(UCSRA, UDRE)) {}
}

/*
 * Description :
 * Function responsible for receiving a byte via UART.
 */
uint8 UART_recieveByte(void)
{
    /* Wait until RXC flag is set, indicating data received */
    while(BIT_IS_CLEAR(UCSRA, RXC)) {}

    /*
     * Read the received data from the Rx buffer (UDR),
     * which clears the RXC flag.
     */
    return UDR;
}

/*
 * Description :
 * Function responsible for sending a null-terminated string via UART.
 */
void UART_sendString(const uint8 *Str)
{
    uint8 i = 0;

    /* Send characters one by one until null terminator */
    while(Str[i] != '\0')
    {
        UART_sendByte(Str[i]);
        i++;
    }

    /************************* Alternative method *************************
    while(*Str != '\0')
    {
        UART_sendByte(*Str);
        Str++;
    }
    **********************************************************************/
}

/*
 * Description :
 * Function responsible for receiving a string from UART until '#' delimiter.
 */
void UART_receiveString(uint8 *Str)
{
    uint8 i = 0;

    /* Receive first character */
    Str[i] = UART_recieveByte();

    /* Continue receiving characters until '#' is received */
    while(Str[i] != '#')
    {
        i++;
        Str[i] = UART_recieveByte();
    }

    /* Replace terminating '#' with null character */
    Str[i] = '\0';
}

/*
 * Description :
 * Sends an ACK byte (0xAA) to the UART receiver.
 */
void UART_sendACK(void)
{
    UART_sendByte(0xAA);  // Fixed ACK value
}

/*
 * Description :
 * Waits to receive an ACK byte (0xAA) from UART transmitter with timeout.
 *
 * Returns:
 *  1 if ACK received successfully, 0 if timeout occurred.
 */
uint8 UART_waitForACK(void)
{
    uint8 ack;
    uint32 timeout = 0;
    do
    {
        ack = UART_recieveByte();
        timeout++;
        if(timeout > TIMEOUT_MAX)
            return 0; // Timeout error: no ACK received
    } while(ack != 0xAA);
    return 1; // ACK received
}

