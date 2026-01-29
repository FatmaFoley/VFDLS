# VFDLS
Vehicle Fault Detection and Logging System (VFDLS)Using ATmega32 (Dual ECU Design).
🚗 Vehicle Fault Detection and Logging System (VFDLS)

Dual ECU Design using ATmega32

📌 Project Overview

The Vehicle Fault Detection and Logging System (VFDLS) is an embedded automotive system designed to continuously monitor critical vehicle subsystems, detect abnormal conditions in real time, log fault codes permanently, and display system status to the driver.

The system is built using two ATmega32 microcontrollers communicating via UART, following a layered architecture commonly used in real automotive ECUs.

🧠 System Architecture
🔹 ECUs Used

HMI ECU (ATmega32 #1)

Handles user interaction

Connected to 4×4 Keypad and 4×16 LCD

Sends commands to Control ECU via UART

Control ECU (ATmega32 #2)

Core processing unit

Reads sensors, controls actuators

Detects faults and logs them into external EEPROM

Communicates system data to HMI ECU

🔌 Hardware Components
Component	Description
ATmega32 (x2)	Main microcontrollers (8 MHz)
Ultrasonic Sensor	Obstacle distance measurement
LM35 Temperature Sensor	Engine temperature monitoring
DC Motors (x2)	Car window control
External EEPROM (24C16)	Permanent fault storage
4×4 Keypad	User input
4×16 LCD	System display
UART	ECU-to-ECU communication
I2C	EEPROM communication
ADC	Sensor data acquisition
⚙️ Monitored Subsystems & Fault Detection
🚘 Ultrasonic Sensor (Parking Aid)

Fault Condition: Distance < 10 cm

DTC Code: P001 – ACCIDENT_MIGHT_HAPPENED

🌡️ Engine Temperature (LM35)

Fault Condition: Temperature > 90°C

DTC Code: P002 – ENGINE_HIGH_TEMPERATURE

🪟 Car Windows

Controlled via DC motors

State (Open / Closed) reported to HMI ECU

⚠️ Each fault is logged only once per occurrence and stored permanently in EEPROM.

🧾 Fault Logging

Diagnostic Trouble Codes (DTCs) are saved in external EEPROM (24C16) using I2C

Fault data is retained even after system reset or power loss

Stored faults can be retrieved and scrolled on the LCD

📟 User Interface (HMI ECU)
📋 Main Menu
1. Start Operation
2. Display Values
3. Retrieve Faults
4. Stop Monitoring

🧪 Live Display Example
Temp: 82 C
Distance: 40 cm
Win1: Closed  Win2: Open

🔄 System Workflow

Start Monitoring

Activates all sensors and fault detection

Displays real-time data

Display Values

Shows current temperature, distance, and window status for 10 seconds

Retrieve Faults

Reads all logged DTCs from EEPROM

Scrolls them automatically on the LCD

Stop Monitoring

Stops sensor readings and fault detection

Returns to main menu

🧩 Software Design

Layered Architecture:

MCAL

HAL

Application Layer

Separate projects for:

HMI ECU

Control ECU

🧪 Drivers Implemented

GPIO

UART (configurable using structures)

I2C (TWI)

ADC

PWM (Timer0)

Timers (Timer0, Timer1, Timer2 with callbacks)

DC Motor

EEPROM

LCD

Keypad

LM35

ICU

Ultrasonic

All drivers follow configuration-based design using structs.

🛠️ Tools & Environment

Microcontroller: ATmega32

Clock Frequency: 8 MHz

IDE: Eclipse

Simulation: Proteus

Programming Language: Embedded C

📦 Deliverables

Proteus Simulation

Two Eclipse projects (HMI ECU & Control ECU)

Demo video showcasing system functionality

✨ Key Features

✔ Real-time fault detection
✔ Permanent fault logging
✔ Dual-ECU communication
✔ Modular driver-based design
✔ Automotive-inspired architecture
