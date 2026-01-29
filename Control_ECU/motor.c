/*
 * motor.c
 *
 *  Created on: Sep 23, 2025
 *      Author: Fatma Foley
 */

/******************************************************************************
 *                             Include Files                                   *
 ******************************************************************************/

#include "motor.h"
#include "pwm.h"
#include "common_macros.h"
#include <avr/delay.h>
#include "gpio.h"

/*
 * Function: DcMotor_Init
 * ----------------------
 * Initializes the GPIO pins connected to two DC motors and their control buttons.
 * Sets motor pins as output and stop the motors initially by setting motor input pins low.
 * Sets the buttons for opening and closing windows as input pins.
 */
void DcMotor_Init(void)
{
    /* Set motor control pins as output for Motor 1 */
    GPIO_setupPinDirection(DC_MOTOR_1_IN3_PORT_ID, DC_MOTOR_1_IN3_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(DC_MOTOR_1_IN4_PORT_ID, DC_MOTOR_1_IN4_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(DC_MOTOR_1_EN_PORT_ID, DC_MOTOR_1_EN_PIN_ID, PIN_OUTPUT);

    /* Stop Motor 1 by setting input pins low */
    GPIO_writePin(DC_MOTOR_1_IN3_PORT_ID, DC_MOTOR_1_IN3_PIN_ID, LOGIC_LOW);
    GPIO_writePin(DC_MOTOR_1_IN4_PORT_ID, DC_MOTOR_1_IN4_PIN_ID, LOGIC_LOW);

    /* Configure Motor 1 buttons as input */
    GPIO_setupPinDirection(DC_MOTOR_1_OPEN_BUTTON_PORT_ID, DC_MOTOR_1_OPEN_BUTTON_PIN_ID, PIN_INPUT);
    GPIO_setupPinDirection(DC_MOTOR_1_CLOSE_BUTTON_PORT_ID, DC_MOTOR_1_CLOSE_BUTTON_PIN_ID, PIN_INPUT);

    /* Set motor control pins as output for Motor 2 */
    GPIO_setupPinDirection(DC_MOTOR_2_IN1_PORT_ID, DC_MOTOR_2_IN1_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(DC_MOTOR_2_IN2_PORT_ID, DC_MOTOR_2_IN2_PIN_ID, PIN_OUTPUT);
    GPIO_setupPinDirection(DC_MOTOR_2_EN_PORT_ID, DC_MOTOR_2_EN_PIN_ID, PIN_OUTPUT);

    /* Stop Motor 2 by setting input pins low */
    GPIO_writePin(DC_MOTOR_2_IN1_PORT_ID, DC_MOTOR_2_IN1_PIN_ID, LOGIC_LOW);
    GPIO_writePin(DC_MOTOR_2_IN2_PORT_ID, DC_MOTOR_2_IN2_PIN_ID, LOGIC_LOW);

    /* Configure Motor 2 buttons as input */
    GPIO_setupPinDirection(DC_MOTOR_2_OPEN_BUTTON_PORT_ID, DC_MOTOR_2_OPEN_BUTTON_PIN_ID, PIN_INPUT);
    GPIO_setupPinDirection(DC_MOTOR_2_CLOSE_BUTTON_PORT_ID, DC_MOTOR_2_CLOSE_BUTTON_PIN_ID, PIN_INPUT);
}

/*
 * Function: DcMotor_Rotate
 * ------------------------
 * Controls rotation of the specified motor corresponding to a window.
 * Checks the state of the open and close buttons for the specified window.
 * Starts PWM for speed control.
 * Sets motor direction pins to rotate motor clockwise (close), anticlockwise (open), or stop.
 *
 *  window_id: Identifier to select which motor/window to control (WINDOW_1 or WINDOW_2).
 *
 *  returns: The current state of the motor: OPEN_WINDOW, CLOSE_WINDOW, or Stop.
 */
DcMotor_State DcMotor_Rotate(Window_ID window_id)
{
    DcMotor_State state;
    /* Start PWM with 100% duty cycle (speed) for motor enable pin */
    PWM_Timer0_Start(100);

    state = Stop;

    /* Check button states for requested window to set motor state */
    if( !(GPIO_readPin(DC_MOTOR_2_OPEN_BUTTON_PORT_ID, DC_MOTOR_2_OPEN_BUTTON_PIN_ID)) && window_id == WINDOW_2 )
    {
        state = OPEN_WINDOW;
    }
    if( !(GPIO_readPin(DC_MOTOR_1_OPEN_BUTTON_PORT_ID, DC_MOTOR_1_OPEN_BUTTON_PIN_ID)) && window_id == WINDOW_1 )
    {
        state = OPEN_WINDOW;
    }
    if( !(GPIO_readPin(DC_MOTOR_2_CLOSE_BUTTON_PORT_ID, DC_MOTOR_2_CLOSE_BUTTON_PIN_ID)) && window_id == WINDOW_2 )
    {
        state = CLOSE_WINDOW;
    }
    if( !(GPIO_readPin(DC_MOTOR_1_CLOSE_BUTTON_PORT_ID, DC_MOTOR_1_CLOSE_BUTTON_PIN_ID)) && window_id == WINDOW_1 )
    {
        state = CLOSE_WINDOW;
    }

    /* Redundant check segregated by window to confirm motor state */
    switch(window_id)
    {
        case WINDOW_1:
            if( !(GPIO_readPin(DC_MOTOR_1_OPEN_BUTTON_PORT_ID, DC_MOTOR_1_OPEN_BUTTON_PIN_ID)) )
            {
                state = OPEN_WINDOW;
            }
            else if( !(GPIO_readPin(DC_MOTOR_1_CLOSE_BUTTON_PORT_ID, DC_MOTOR_1_CLOSE_BUTTON_PIN_ID)) )
            {
                state = CLOSE_WINDOW;
            }
            else
            {
                state = Stop;
            }
            break;

        case WINDOW_2:
            if( !(GPIO_readPin(DC_MOTOR_2_OPEN_BUTTON_PORT_ID, DC_MOTOR_2_OPEN_BUTTON_PIN_ID)) )
            {
                state = OPEN_WINDOW;
            }
            else if( !(GPIO_readPin(DC_MOTOR_2_CLOSE_BUTTON_PORT_ID, DC_MOTOR_2_CLOSE_BUTTON_PIN_ID)) )
            {
                state = CLOSE_WINDOW;
            }
            else
            {
                state = Stop;
            }
            break;
    }

    /* Set motor control pins to rotate motor or stop based on the state */
    switch(state)
    {
        case Stop:
            /* Both control pins low to stop motor */
            if(window_id == WINDOW_1)
            {
                GPIO_writePin(DC_MOTOR_1_IN3_PORT_ID, DC_MOTOR_1_IN3_PIN_ID, LOGIC_LOW);
                GPIO_writePin(DC_MOTOR_1_IN4_PORT_ID, DC_MOTOR_1_IN4_PIN_ID, LOGIC_LOW);
            }
            else
            {
                GPIO_writePin(DC_MOTOR_2_IN1_PORT_ID, DC_MOTOR_2_IN1_PIN_ID, LOGIC_LOW);
                GPIO_writePin(DC_MOTOR_2_IN2_PORT_ID, DC_MOTOR_2_IN2_PIN_ID, LOGIC_LOW);
            }
            break;

        case OPEN_WINDOW:
            /* Rotate motor Anti-Clockwise: control pin1 low, control pin2 high */
            if(window_id == WINDOW_1)
            {
                GPIO_writePin(DC_MOTOR_1_IN3_PORT_ID, DC_MOTOR_1_IN3_PIN_ID, LOGIC_LOW);
                GPIO_writePin(DC_MOTOR_1_IN4_PORT_ID, DC_MOTOR_1_IN4_PIN_ID, LOGIC_HIGH);
            }
            else
            {
                GPIO_writePin(DC_MOTOR_2_IN1_PORT_ID, DC_MOTOR_2_IN1_PIN_ID, LOGIC_LOW);
                GPIO_writePin(DC_MOTOR_2_IN2_PORT_ID, DC_MOTOR_2_IN2_PIN_ID, LOGIC_HIGH);
            }
            break;

        case CLOSE_WINDOW:
            /* Rotate motor Clockwise: control pin1 high, control pin2 low */
            if(window_id == WINDOW_1)
            {
                GPIO_writePin(DC_MOTOR_1_IN3_PORT_ID, DC_MOTOR_1_IN3_PIN_ID, LOGIC_HIGH);
                GPIO_writePin(DC_MOTOR_1_IN4_PORT_ID, DC_MOTOR_1_IN4_PIN_ID, LOGIC_LOW);
            }
            else
            {
                GPIO_writePin(DC_MOTOR_2_IN1_PORT_ID, DC_MOTOR_2_IN1_PIN_ID, LOGIC_HIGH);
                GPIO_writePin(DC_MOTOR_2_IN2_PORT_ID, DC_MOTOR_2_IN2_PIN_ID, LOGIC_LOW);
            }
            break;
    }

    return state;
}
