# at89sprog
An AT89S52 programmer using Arduino Mega 2560

8051 MCU: AT89s52
Programmer: Arduino Mega 2560
PC server: Linux application (Ubuntu distribution)

arduino_src/
- This directory stores Arduino code. Receive commands, hex file data content from PC server then programm the AT89S52 MCU.

host_src/
- This directory stores PC server code. Read AT89S52 hex file, send hex file and commands to arduino

common/
- This directory stores common source code used for Arduino and PC server


Diagram:
+-----------+                 +--------------+               +---------+
| PC server |-------USB-------| Arduino Mega |------ISP------| AT89s52 |
+-----------+                 +--------------+               +---------+

PC server <--> Arduino Mega: communicate via USB Serial cable.
Arduino <--> AT89s52: communicate via ISP (In System Programming) interface.

I write this project for myself studying about embedded programming. I am beginner and start from ground to up.
I will appreciate if you share me your ideas to improve this project.
You are freely to get this project for yourself development.
