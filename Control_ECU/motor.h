/*
 * motor.h
 *
 *  Created on: Sep 23, 2025
 *      Author: Fatma Foley
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

/******************************************************************************
 *                             Include Files                                   *
 ******************************************************************************/

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/*
 * Define the port and pin IDs for Motor 1 control pins and buttons
 */
#define DC_MOTOR_1_IN3_PORT_ID                    PORTB_ID
#define DC_MOTOR_1_IN3_PIN_ID                     PIN0_ID
#define DC_MOTOR_1_IN4_PORT_ID                    PORTB_ID
#define DC_MOTOR_1_IN4_PIN_ID                     PIN1_ID
#define DC_MOTOR_1_EN_PORT_ID                     PORTB_ID
#define DC_MOTOR_1_EN_PIN_ID                      PIN3_ID
#define DC_MOTOR_1_OPEN_BUTTON_PORT_ID            PORTB_ID
#define DC_MOTOR_1_OPEN_BUTTON_PIN_ID             PIN6_ID
#define DC_MOTOR_1_CLOSE_BUTTON_PORT_ID           PORTB_ID
#define DC_MOTOR_1_CLOSE_BUTTON_PIN_ID            PIN7_ID

/*
 * Define the port and pin IDs for Motor 2 control pins and buttons
 */
#define DC_MOTOR_2_IN1_PORT_ID                    PORTB_ID
#define DC_MOTOR_2_IN1_PIN_ID                     PIN2_ID
#define DC_MOTOR_2_IN2_PORT_ID                    PORTB_ID
#define DC_MOTOR_2_IN2_PIN_ID                     PIN4_ID
#define DC_MOTOR_2_EN_PORT_ID                     PORTB_ID
#define DC_MOTOR_2_EN_PIN_ID                      PIN3_ID
#define DC_MOTOR_2_OPEN_BUTTON_PORT_ID            PORTA_ID
#define DC_MOTOR_2_OPEN_BUTTON_PIN_ID             PIN2_ID
#define DC_MOTOR_2_CLOSE_BUTTON_PORT_ID           PORTA_ID
#define DC_MOTOR_2_CLOSE_BUTTON_PIN_ID            PIN3_ID

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

/* Enum for motor rotation states */
typedef enum {
    OPEN_WINDOW,    /* Rotate motor to open the window */
    CLOSE_WINDOW,   /* Rotate motor to close the window */
    Stop           /* Stop motor rotation */
} DcMotor_State;

/* Enum to specify which window/motor is being controlled */
typedef enum {
    WINDOW_1,
    WINDOW_2
} Window_ID;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/*
 * Function: DcMotor_Init
 * ----------------------
 * Initializes the motor control pins and input buttons as output/input pins respectively.
 * Stops both motors by default.
 */
void DcMotor_Init(void);

/*
 * Function: DcMotor_Rotate
 * ------------------------
 * Rotates the specified motor according to button inputs to open, close or stop the window.
 *
 *  window_id: The identifier for which motor/window to control.
 *
 * Returns the current state of the motor rotation.
 */
DcMotor_State DcMotor_Rotate(Window_ID window_id);

#endif /* DC_MOTOR_H_ */
