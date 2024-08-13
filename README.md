# Project Overview
In this school project I designed an automatic vehicle. The key features of the project are:
* A joystick to control the direction and speed of movement.
* An OLED display that shows the direcction in which the vehicle is moving and provides alerts.
* A HC-SR04 sensor that automatically stops the vehicle when an obstacle is detected.
* A buzzer to alert when an obstacle is detected.
* A bluetooth module that receives commands from a mobil application.
* A mobile application developed in [App Inventor](https://appinventor.mit.edu/) that allows you to controll the vehicle by voice or by pressing buttons.

The project utilizes almost every peripheral available on the [EK-TM4C1294XL](https://www.ti.com/tool/EK-TM4C1294XL) board, and the code was developed at a low level using C.

## Main task
The main function initializes the UART, ADC, I2C, SPI, and PWM, and configures all necessary ports. It then enters an infinite loop that reads ADC data and controls the vehicle's movement.

## Interrupt tasks:

The project uses two interrupts:

* Port C5 on the rising edge: This interrupt is associated with the ECHO pin of the HC-SR04.
* FIFO interrupt on UART: This interrupt is associated with the commands received from the mobile app.

In this case, the Port C5 interrupt has the highest priority because it stops the vehicle when necessary.

Just as each interrupt has its priority, there is also a priority in how you control the movement of the vehicle. For example, any instruction given using the mobile application takes higher priority than the joystick. Likewise, no method has a higher priority than stopping the vehicle when an obstacle is detected.

