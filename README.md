# PID_Motor_controller_arduino
Electronic embedded system for automatic regulation of speed rotation of motor via incremental encoder. Application is based on PID kind of regulation technique. 

A PID controller is an instrument used in industrial control applications to regulate temperature, flow, pressure, speed and other process variables. PID (proportional integral derivative) controllers use a control loop feedback mechanism to control process variables and are the most accurate and stable controller. 
For the application, a motor drive with a coupled incremental encoder was used as the controlled object. Due to rotating motor the encoder capturing pulses and sending to microcontroller to calculate speed feedback. The difference between the speed feedback and the reference value represents the control quality error. PID unit calculate parameters besed on errors. PID unit calculate parameters based on these errors which are sent to PWM unit. PWM methos is premised on sending binary digital signals to transistor with a specific pulse length, in result of these signals, a specific voltage is applied to the motor.

Electronic components used:
1. Encoder: 27-P720R.25/200
2. Allied Motion 9904 120 52605 12VDC
3. Arduino Uno with processor ATMEGA328P
4. Voltage unit measurement ARD-11772
5. Current unit measurement ACS712
6. Voltage stabilizer LM317
7. LCD Display 16x2
8. Potentiometer 10k
9. Monostable button
10. Resistors: 2x10k, 2x720, 3x300, 1x240
11. Transistor NPN BD679A 1x
12. Tranistor MOSFET n-channel IRF740 1x
13. LED Diode 20mA 1x
14. Optotransistor TLP283

![electronic_schematic_img](https://user-images.githubusercontent.com/75945631/173930218-08e78058-f40e-409e-aac5-f0ef4f5af423.jpg)
