/*
 ================================================================================================
 Name        : main.c
 Author      : Fatma Foley
 Description : HMI_ECU Code
 Date        : 25/4/2014
 ================================================================================================
 */

#include "keypad.h"
#include "uart.h"
#include "lcd.h"
#include "timer.h"
#include <util/delay.h> /* For the delay functions */
#include <avr/io.h>       /* Access to AVR IO registers */

uint8 g_tick = 0;

/* Timer1 callback function increments global tick counter */
void Timer1_callback_fun(void)
{
    g_tick++;
}

/* Enum for the motor rotation states */
typedef enum {
    OPEN_WINDOW,    /* Clockwise rotation */
    CLOSE_WINDOW,   /* Anti-Clockwise rotation */
    Stop           /* Stop the motor */
} DcMotor_State;

int main(void)
{
    /* Variable declarations and initialization */
    uint8 key = 0, temp = 0, distance_high_byte = 0, distance_low_byte = 0, repeat = 1;
    uint8 P001_Dist_error_counter = 0, P002_Temp_error_counter = 0;
    uint8 tick_loop_counter = 0, check = 0, tick_prev = 0;
    uint16 distance = 0;
    DcMotor_State window1_state, window2_state;

    /* UART configuration */
    UART_ConfigType ConfigUART_Ptr = {BIT_DATA_8, PARITY_DISABLED, STOP_BIT_1, 9600};
    UART_init(&ConfigUART_Ptr);
    LCD_init();

    /* Timer1 configuration for 1-second intervals (with prescaler 1024 and compare value 15625) */
    Timer_ConfigType Config_Ptr = {0, 15625, TIMER1, PRESCALER_1024, COMPARE_MODE};

    /* Enable Global Interrupt I-Bit (bit 7 in SREG) for ICU and timer operations */
    SREG |= (1 << 7);

    while(1)
    {
        /* Display main menu on LCD */
        LCD_clearScreen();
        LCD_displayStringRowColumn(0,0,"1.StartOperation");
        LCD_displayStringRowColumn(1,0,"2.Display Values");
        LCD_displayStringRowColumn(2,0,"3.RetrieveFaults");
        LCD_displayStringRowColumn(3,0,"4.StopMonitoring");

        /* Get keypad input */
        key = KEYPAD_getPressedKey();

        /* Send selected key via UART to MC2 */
        UART_sendByte(key);

        _delay_ms(500);

        switch(key)
        {
            case 1: /* Start operation */
                LCD_clearScreen();
                g_tick = 0;

                /* Initialize timer and set callback */
                Timer_init(&Config_Ptr);
                Timer_setCallBack(Timer1_callback_fun, TIMER1);

                LCD_displayStringRowColumn(0,0,"OperationStarted");
                LCD_displayStringRowColumn(1,0,"MonitoringActive");

                /* Send tick values until tick reaches 5 */
                UART_sendByte(g_tick);
                while(g_tick < 5)
                {
                    UART_sendByte(g_tick);
                }
                UART_sendByte(g_tick);

                /* Deinitialize timer */
                Timer_deInit(TIMER1);
                break;

            case 2: /* Display sensor values */
                while(repeat)
                {
                    LCD_clearScreen();
                    g_tick = 0;

                    /* Initialize timer and set callback */
                    Timer_init(&Config_Ptr);
                    Timer_setCallBack(Timer1_callback_fun, TIMER1);

                    /* Display sensor value labels */
                    LCD_displayStringRowColumn(0,0,"Temp = ");
                    LCD_displayStringRowColumn(0, 11, " C");
                    LCD_displayStringRowColumn(1,0,"Dist = ");
                    LCD_displayStringRowColumn(1, 10, " cm");

                    UART_sendByte(g_tick);

                    /* Loop until 5 timer ticks have elapsed */
                    while(g_tick < 5)
                    {
                        UART_sendByte(g_tick);
                        tick_prev = g_tick;
                        _delay_ms(200);

                        if(tick_loop_counter > 0)
                        {
                            distance = (distance_high_byte << 8) | distance_low_byte;

                            /* Display temperature */
                            LCD_moveCursor(0, 7);
                            LCD_intgerToString(temp);

                            /* Display distance */
                            LCD_moveCursor(1, 7);
                            LCD_intgerToString(distance);

                            /* Clear extra digits if temp or distance less than certain thresholds */
                            if((temp >= 10 ) && (temp < 100))
                            {
                                LCD_displayStringRowColumn(0, 9, " ");
                            }
                            else if(temp < 10)
                            {
                                LCD_displayStringRowColumn(0, 8, "  ");
                            }
                            if((distance >= 10 ) && (distance < 100))
                            {
                                LCD_displayStringRowColumn(1, 9, " ");
                            }
                            else if(distance < 10)
                            {
                                LCD_displayStringRowColumn(1, 8, "  ");
                            }

                            /* Display window 1 motor state */
                            switch(window1_state)
                            {
                                case OPEN_WINDOW:
                                    LCD_displayStringRowColumn(2,0,"Window1:Open");
                                    LCD_displayStringRowColumn(2,12," ");
                                    break;
                                case CLOSE_WINDOW:
                                    LCD_displayStringRowColumn(2,0,"Window1:Close");
                                    break;
                                case Stop:
                                    LCD_displayStringRowColumn(2,0,"Window1:Stop");
                                    LCD_displayStringRowColumn(2,12," ");
                                    break;
                            }

                            /* Display window 2 motor state */
                            switch(window2_state)
                            {
                                case OPEN_WINDOW:
                                    LCD_displayStringRowColumn(3,0,"Window2:Open");
                                    LCD_displayStringRowColumn(3,12," ");
                                    break;
                                case CLOSE_WINDOW:
                                    LCD_displayStringRowColumn(3,0,"Window2:Close");
                                    break;
                                case Stop:
                                    LCD_displayStringRowColumn(3,0,"Window2:Stop");
                                    LCD_displayStringRowColumn(3,12," ");
                                    break;
                            }
                        }
                        tick_loop_counter++;

                        /* Receive temperature and send ACK */
                        temp = UART_recieveByte();
                        UART_sendACK();

                        /* Receive high byte of distance and send ACK */
                        distance_high_byte = UART_recieveByte();
                        UART_sendACK();

                        /* Receive low byte of distance and send ACK */
                        distance_low_byte = UART_recieveByte();
                        UART_sendACK();

                        /* Receive motor states for windows and send ACK */
                        window1_state = UART_recieveByte();
                        UART_sendACK();
                        window2_state = UART_recieveByte();
                    }

                    UART_sendByte(g_tick);

                    tick_loop_counter = 0;

                    /* Deinitialize timer */
                    Timer_deInit(TIMER1);
                    LCD_clearScreen();

                    /* Ask user if they want to display again */
                    LCD_displayStringRowColumn(0,0,"Display again?");
                    LCD_displayStringRowColumn(1,0,"Press 2 = YES");
                    LCD_displayStringRowColumn(2,0,"Other key = MAIN MENU");

                    key = KEYPAD_getPressedKey();
                    _delay_ms(500);

                    if(2 == key)
                    {
                        repeat = 1;
                    }
                    else
                    {
                        repeat = 0;
                    }

                    /* Send repeat response to MC2 */
                    UART_sendByte(repeat);
                    _delay_ms(200);
                }
                repeat = 1;
                break;

            case 3: /* Retrieve faulty/error counts */
                while(repeat)
                {
                    g_tick = 0;

                    /* Initialize timer and callback */
                    Timer_init(&Config_Ptr);
                    Timer_setCallBack(Timer1_callback_fun, TIMER1);

                    LCD_clearScreen();
                    LCD_displayStringRowColumn(0,0,"Logged Faults:");
                    LCD_displayStringRowColumn(1,0,"P001: ");
                    LCD_displayStringRowColumn(2,0,"P002: ");
                    LCD_displayStringRowColumn(3,0,"--End of List--");

                    UART_sendByte(g_tick);

                    while(g_tick < 5)
                    {
                        UART_sendByte(g_tick);
                        _delay_ms(200);

                        /* Receive fault counters for distance and temperature */
                        P001_Dist_error_counter = UART_recieveByte();
                        UART_sendACK();
                        P002_Temp_error_counter = UART_recieveByte();

                        /* Display faults on LCD */
                        LCD_moveCursor(1, 6);
                        LCD_intgerToString(P001_Dist_error_counter);
                        LCD_moveCursor(2, 6);
                        LCD_intgerToString(P002_Temp_error_counter);

                        /* Clear extra spaces for fault count display */
                        if(P001_Dist_error_counter < 10)
                        {
                            LCD_displayStringRowColumn(1,7,"   ");
                        }
                        else if(P001_Dist_error_counter > 100)
                        {
                            LCD_displayStringRowColumn(1,8," ");
                        }
                        if(P002_Temp_error_counter < 10)
                        {
                            LCD_displayStringRowColumn(2,7,"   ");
                        }
                        else if(P002_Temp_error_counter > 100)
                        {
                            LCD_displayStringRowColumn(2,8," ");
                        }
                    }

                    UART_sendByte(g_tick);
                    _delay_ms(200);

                    /* Deinitialize timer */
                    Timer_deInit(TIMER1);
                    LCD_clearScreen();

                    /* Ask user if they want to display faults again */
                    LCD_displayStringRowColumn(0,0,"Display again?");
                    LCD_displayStringRowColumn(1,0,"Press 3 = YES");
                    LCD_displayStringRowColumn(2,0,"Other key = MAIN MENU");

                    key = KEYPAD_getPressedKey();
                    _delay_ms(500);

                    if(3 == key)
                    {
                        repeat = 1;
                    }
                    else
                    {
                        repeat = 0;
                    }

                    UART_sendByte(repeat);
                    _delay_ms(200);
                }
                repeat = 1;
                _delay_ms(300);
                break;

            case 4: /* Stop monitoring */
                LCD_clearScreen();
                g_tick = 0;

                /* Initialize timer and set callback */
                Timer_init(&Config_Ptr);
                Timer_setCallBack(Timer1_callback_fun, TIMER1);

                /* Display stopping message until 5 ticks */
                while(g_tick < 5)
                {
                    LCD_displayStringRowColumn(0,0,"SystemMonitoring");
                    LCD_displayStringRowColumn(1,0,"Stopped!");
                    LCD_displayStringRowColumn(2,0,"ReturningToMenu");
                }

                /* Deinitialize timer */
                Timer_deInit(TIMER1);
                break;
        }
    }
}
