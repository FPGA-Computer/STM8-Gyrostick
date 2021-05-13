# STM8 Gyrostick

This is a USB HID game controller inspired by [Cemu Gyro Joystick](https://github.com/ArsenicBismuth/Cemu-GyroStick) project. 
The hardware is based on cheap [MPU6050 modules](http://wiki.sunfounder.cc/index.php?title=MPU6050_Module) used by [Gyro_Joystick](https://github.com/ArsenicBismuth/Arduino-Programs/tree/master/Gyro_Joystick).

It uses a firmware based USB library on a STM8S003 similar to VUSB and has to live with the HID report limitations of 8 bytes for low speed devices. The 3 Accelerometers and 3 Gyroscopes are grouped logically under their own reportIDs and takes turn being sent.
There is a long list of STM8 I2C peripheral errata, so I have no choice but to base mine on ST's heavy IRQ code from [AN3281 - STM8 8-bit MCUs I2C optimized examples](https://www.st.com/en/embedded-software/stsw-stm8004.html).
A simple busy wait was all that was need to make both of them coexist.

# STM8 VUSB

The firmware only USB stack is an unofficial fork from STM8S-VUSB-KEYBOARD at https://github.com/BBS215/STM8S-VUSB-KEYBOARD

- Works with current special edition of Cosmic C (4.4.10) and current version of STVD (4.3.12)

Note: 

The USB stack calibrates the HSI clock while connected to the PC. The calibration is store in EEPROM. It can take a couple of minutes the first time before it functions correctly.

[Project page](https://hw-by-design.blogspot.com/2021/05/stm8-gyrostock.html)


Licenses: 
Hardware
-Creative Common Attribution 4.0 International [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/)

Firmware
- Code written by me: GPL 3.0
- STM8 USB stack is under GPL-2.0. License is based on work by Github user: BBS215
- ST STM8 header file and their I2C library are under their own licenses.

Initial release.
