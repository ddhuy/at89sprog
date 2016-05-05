# at89sprog
An AT89S52 programmer using Arduino Mega 2560

8051 MCU: AT89s52
Programmer: Arduino Mega 2560 (Atmega2560)
PC host: Linux application (Ubuntu 14.04 distribution)

src/programmer
- This directory stores Arduino code. Receive commands, hex file data content from PC host then programm the AT89S52 MCU.

src/commmander
- This directory stores PC host code. Read AT89S52 hex file, send hex file and commands to arduino

src/utils
- This directory stores common source code used for Arduino and PC host


Diagram:
+-----------+                 +--------------+               +---------+
|  PC host  |-------USB-------| Arduino Mega |------ISP------| AT89s52 |
+-----------+                 +--------------+               +---------+

PC host <--> Arduino Mega: communicate via USB Serial cable.
Arduino <--> AT89s52: communicate via ISP (In System Programming) interface.

I write this project for myself studying about embedded programming. I am beginner and start from ground up.
I will appreciate if you share me your ideas to improve this project.
You are freely to get this project for yourself development.
