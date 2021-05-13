/*
 * motion.h
 *
 * Created: May 11, 2021, 8:32:45 AM
 *  Author: K. C. Lee
 * Copyright (c) 2021 by K. C. Lee
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.

 If not, see http://www.gnu.org/licenses/gpl-3.0.en.html 
 */ 
 
#ifndef MOTION_H_
#define MOTION_H_

void MPU6050_Init(void);
void MPU6050_Scale(void);

#define MPU6050_REGSIZE (MPU6050_RA_GYRO_ZOUT_L-MPU6050_RA_ACCEL_XOUT_H+1)

extern int16_t MPU6050_Regs[];
extern uint8_t MPU6050_State;

enum _SensorIndex
{
	RAW_Acc_X,RAW_Acc_Y,RAW_Acc_Z,Raw_Temp,RAW_Gyro_X,RAW_Gyro_Y,RAW_Gyro_Z	
};

#endif
