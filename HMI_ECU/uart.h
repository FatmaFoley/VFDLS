/*
 * uart.h
 *
 *  Created on: Nov 1, 2025
 *      Author: Fatma Foley
 */

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Define maximum timeout value for ACK waiting loops */
#define TIMEOUT_MAX 10000

/* Typedef for baud rate */
typedef uint32 UART_BaudRateType;

/* Enum for data bit length configuration */
typedef enum
{
    BIT_DATA_5 = 0,
    BIT_DATA_6 = 1,
    BIT_DATA_7 = 2,
    BIT_DATA_8 = 3,
    BIT_DATA_9 = 7
} UART_BitDataType;

/* Enum for parity bit configuration */
typedef enum
{
    PARITY_DISABLED = 0,
    PARITY_EVEN = 2,
    PARITY_ODD  = 3
} UART_ParityType;

/* Enum for stop bit configuration */
typedef enum
{
    STOP_BIT_1 = 0,
    STOP_BIT_2 = 1
} UART_StopBitType;

/* Struct to hold UART configuration parameters */
typedef struct {
    UART_BitDataType bit_data;      /* Number of data bits */
    UART_ParityType parity;         /* Parity setting */
    UART_StopBitType stop_bit;      /* Number of stop bits */
    UART_BaudRateType baud_rate;    /* Baud rate value */
} UART_ConfigType;

/*
 * Description :
 * Function to initialize the UART peripheral.
 * Configures frame format, enables UART transmitter and receiver,
 * and sets the baud rate as per the configuration.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Sends a single byte over UART.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Receives a single byte from UART.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Sends a null-terminated string through UART.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receives a string through UART until the '#' character is received.
 * The '#' is replaced with a null terminator.
 */
void UART_receiveString(uint8 *Str);

/*
 * Description :
 * Sends an ACK byte (0xAA) to the UART receiver.
 */
void UART_sendACK(void);

/*
 * Description :
 * Waits to receive an ACK byte (0xAA) from UART transmitter.
 * Returns 1 if ACK received before timeout; otherwise 0.
 */
uint8 UART_waitForACK(void);


#endif /* UART_H_ */
