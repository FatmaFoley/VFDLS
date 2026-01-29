/*
 ================================================================================================
 Name        : main.c
 Author      : Fatma Foley
 Description : Control_ECU Code
 Date        : 25/4/2014
 ================================================================================================
 */

#include "uart.h"
#include "lm35_temp_sensor.h"
#include "ultrasonic_sensor.h"
#include "external_eeprom.h"
#include "common_macros.h"
#include "motor.h"
#include "twi.h"
#include <util/delay.h>
#include <avr/io.h>

#define ERROR_TEMP_HIGH_ADDR  0x10  // EEPROM address for temperature error counter
#define ERROR_DIST_LOW_ADDR   0x20  // EEPROM address for distance error counter

int main(void)
{
    /* Variable declarations and initialization */
    uint8 key = 0, tick = 0, temp_prev = 0, temp = 0, distance_high_byte = 0, distance_low_byte = 0;
    uint8 P001_Dist_error_counter = 0, P002_Temp_error_counter = 0, repeat = 1;
    uint8 eeprom_data = 0, faults_buffer[2] = {0,0}, tick_loop_counter = 0, check = 0;
    uint16 distance_prev = 0, distance = 0;
    DcMotor_State window1_state, window2_state;

    /* UART configuration struct */
    UART_ConfigType Config_Ptr = {BIT_DATA_8, PARITY_DISABLED, STOP_BIT_1, 9600};

    /* Initialize sensors, UART, motor, and TWI */
    LM35_init();
    Ultrasonic_init();
    UART_init(&Config_Ptr);
    DcMotor_Init();
    TWI_ConfigType twi_settings = {0x01, 400000};

    /* Initialize TWI (I2C) */
    TWI_init(&twi_settings);

    /* Enable global interrupts */
    SREG |= (1 << 7);

    while(1)
    {
        /* Receive command key from UART */
        key = UART_recieveByte();

        switch(key)
        {
            case 1:
                /* In case 1: collect sensor data and update error counters */
                while(tick < 5)
                {
                    tick = UART_recieveByte();

                    /* Read current temperature and distance */
                    temp = LM35_getTemperature();
                    distance = Ultrasonic_readDistance();

                    _delay_ms(100);

                    /* Read current error counters from EEPROM */
                    EEPROM_readByte(ERROR_DIST_LOW_ADDR, &eeprom_data);
                    _delay_ms(10);
                    P001_Dist_error_counter = eeprom_data;
                    EEPROM_readByte(ERROR_TEMP_HIGH_ADDR, &eeprom_data);
                    _delay_ms(10);
                    P002_Temp_error_counter = eeprom_data;

                    /* If temperature exceeds 90 and has changed, increment temp error counter */
                    if(temp > 90 && temp != temp_prev)
                    {
                        P002_Temp_error_counter++;
                        EEPROM_writeByte(ERROR_TEMP_HIGH_ADDR, P002_Temp_error_counter);
                        _delay_ms(10);
                    }

                    /* If distance less than 10 and changed, increment distance error counter */
                    if(distance < 10 && distance != distance_prev)
                    {
                        P001_Dist_error_counter++;
                        EEPROM_writeByte(ERROR_DIST_LOW_ADDR, P001_Dist_error_counter);
                        _delay_ms(10);
                    }

                    /* Save previous values for next comparison */
                    temp_prev = temp;
                    distance_prev = distance;
                }
                break;

            case 2:
                /* Case 2: continuous monitoring and controlling motor state with UART data exchange */
                while(repeat)
                {
                    tick = UART_recieveByte();

                    while(tick < 5)
                    {
                        tick = UART_recieveByte();
                        _delay_ms(200);

                        if(5 == tick)
                        {
                            break;
                        }

                        /* Get latest temperature and distance */
                        temp = LM35_getTemperature();
                        distance = Ultrasonic_readDistance();

                        /* Split distance into high and low bytes for transmission */
                        distance_high_byte = (distance >> 8);
                        distance_low_byte = (distance & 0xFF);

                        /* Rotate motors/windows based on input */
                        window1_state = DcMotor_Rotate(WINDOW_1);
                        window2_state = DcMotor_Rotate(WINDOW_2);

                        /* Update error counters if thresholds exceeded and values changed */
                        if(temp > 90 && temp != temp_prev)
                        {
                            P002_Temp_error_counter++;
                            EEPROM_writeByte(ERROR_TEMP_HIGH_ADDR, P002_Temp_error_counter);
                            _delay_ms(10);
                        }
                        if(distance < 10 && distance != distance_prev)
                        {
                            P001_Dist_error_counter++;
                            EEPROM_writeByte(ERROR_DIST_LOW_ADDR, P001_Dist_error_counter);
                            _delay_ms(10);
                        }
                        if(tick_loop_counter > 0)
                        {
                            _delay_ms(100);
                        }
                        tick_loop_counter++;

                        /* Update previous values */
                        temp_prev = temp;
                        distance_prev = distance;

                        /* Send latest sensor and motor state data through UART with ACK synchronization */
                        UART_sendByte(temp);
                        UART_waitForACK();
                        UART_sendByte(distance_high_byte);
                        UART_waitForACK();
                        UART_sendByte(distance_low_byte);
                        UART_waitForACK();
                        UART_sendByte(window1_state);
                        UART_waitForACK();
                        UART_sendByte(window2_state);
                    }

                    /* Check for repeat command */
                    repeat = UART_recieveByte();
                    _delay_ms(200);

                    tick_loop_counter = 0;
                    check = 0;
                }
                tick = 0;
                repeat = 1;
                break;

            case 3:
                /* Case 3: send the error counters and basic sensors continuously */
                check = 0;

                while(repeat)
                {
                    tick = UART_recieveByte();

                    while(tick < 5)
                    {
                        tick = UART_recieveByte();

                        if(5 == tick)
                        {
                            break;
                        }

                        /* Read error counters from EEPROM */
                        EEPROM_readByte(ERROR_DIST_LOW_ADDR, &eeprom_data);
                        _delay_ms(10);
                        faults_buffer[0] = eeprom_data;

                        EEPROM_readByte(ERROR_TEMP_HIGH_ADDR, &eeprom_data);
                        _delay_ms(10);
                        faults_buffer[1] = eeprom_data;

                        /* Read sensors */
                        temp = LM35_getTemperature();
                        distance = Ultrasonic_readDistance();

                        /* Update error counters for threshold breaches */
                        if(temp > 90 && temp != temp_prev)
                        {
                            P002_Temp_error_counter++;
                            EEPROM_writeByte(ERROR_TEMP_HIGH_ADDR, P002_Temp_error_counter);
                            _delay_ms(10);
                        }
                        if(distance < 10 && distance != distance_prev)
                        {
                            P001_Dist_error_counter++;
                            EEPROM_writeByte(ERROR_DIST_LOW_ADDR, P001_Dist_error_counter);
                            _delay_ms(10);
                        }

                        /* Update previous sensor values */
                        temp_prev = temp;
                        distance_prev = distance;

                        /* Send error counters through UART and wait for ACK */
                        UART_sendByte(faults_buffer[0]);
                        UART_waitForACK();
                        UART_sendByte(faults_buffer[1]);
                    }

                    repeat = UART_recieveByte();
                    _delay_ms(200);
                }

                tick = 0;
                repeat = 1;
                break;

            case 4:
                /* Case 4: Reset error counters and sensor previous values to zero */
                P001_Dist_error_counter = 0;
                P002_Temp_error_counter = 0;
                temp_prev = 0;
                distance_prev = 0;

                /* Write zero to EEPROM error counter addresses */
                EEPROM_writeByte(ERROR_TEMP_HIGH_ADDR, 0);
                _delay_ms(10);
                EEPROM_writeByte(ERROR_DIST_LOW_ADDR, 0);
                _delay_ms(10);
                break;
        }
    }
}
