# Basic Heater Control System

This project is a solution for the upliance.ai Embedded Systems Intern Assignment. It implements a simple heater control system on an ESP32 using a DS18B20 temperature sensor.

## Features

- **State-Driven Control**: The system operates on a state machine with the following states: `IDLE`, `HEATING`, `STABILIZING`, `TARGET_REACHED`, and `OVERHEAT`.
- **Temperature Monitoring**: Continuously reads temperature using a DS18B20 digital sensor.
- **Hysteresis Control**: Prevents rapid on/off switching of the heater around the target temperature.
- **Overheat Protection**: A critical `OVERHEAT` state disables the heater and provides a visual warning if the temperature exceeds a safety threshold.
- **Periodic Operation**: Uses a hardware timer interrupt to ensure that temperature checks and logic updates occur reliably every second.
- **Serial Logging**: Outputs the current temperature, system state, and heater status to the Serial Monitor for easy debugging.
- **Visual Feedback**: An LED indicator provides visual cues for the `OVERHEAT` state.

## Hardware Requirements

- ESP32 Development Board
- DS18B20 Temperature Sensor
- 1x Relay Module
- 1x Red LED (to simulate the Heater)
- 1x Yellow/Blue LED (for status indication)
- 2x 220 Ohm Resistors (for the LEDs)
- 1x 4.7k Ohm Resistor (pull-up for DS18B20 data line)
- Breadboard and Jumper Wires

## Pinout & Wiring

| Component           | ESP32 Pin |
| ------------------- | --------- |
| Relay IN            | GPIO 2    |
| Indicator LED       | GPIO 5    |
| DS18B20 Data Line   | GPIO 4    |

*   **DS18B20:** Connect VCC to 3.3V, GND to GND, and the Data pin to GPIO 4. Place a 4.7k Ohm pull-up resistor between the Data pin and 3.3V.
*   **Relay:** Connect VCC to 3.3V, GND to GND, and the IN(signal) pin to GPIO 2.
*   **LED1:** Connect the longer lead (anode) of LED to GPIO 5 pin through a 220 Ohm resistor, and the shorter lead (cathode) to GND.
*   **LED2:** Connect the longer lead (anode) of LED to NO(Normally Open) port of the Relay module,  and the shorter lead (cathode) to GND.

## How to Use

1.  **Setup Hardware**: Wire the components according to the pinout table.
2.  **Upload Code**: Open the `.ino` file in the Arduino IDE (with the ESP32 board manager installed), select your ESP32 board and port, and upload the code.
3.  **Monitor**: Open the Serial Monitor and set the baud rate to `115200`. You will see status logs printed every second.
4.  **Test**: Gently heat the DS18B20 sensor to observe the state transitions from `HEATING` to `TARGET_REACHED`. If you exceed 90°C, the system will enter the `OVERHEAT` state.