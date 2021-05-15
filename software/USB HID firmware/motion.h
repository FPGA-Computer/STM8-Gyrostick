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
#define REPORT_SIZE		  7

extern uint8_t Report_Acc[REPORT_SIZE],Report_Gyro[REPORT_SIZE];
extern uint8_t ReportID;

extern int16_t MPU6050_Regs[];
extern uint8_t MPU6050_State;

enum REPORT_ID
{
	ID_Acc = 0x01,
	ID_Gyro = 0x02
 };

enum _MOTION_STATES_
{
	USB_Idle,
	USB_Rdy,
	MPU6050_I2C_Read
};

enum _SensorIndex
{
	RAW_Acc_X,RAW_Acc_Y,RAW_Acc_Z,Raw_Temp,RAW_Gyro_X,RAW_Gyro_Y,RAW_Gyro_Z	
};

enum _ReportIndex
{
	Report_ID = 0x00, Report_X = 0x01, Report_Y = 0x03, Report_Z = 0x05 
};

#define ACCEL_XY_SCALE	-2L
#define ACCEL_Z_SCALE		-1L
#define GYRO_SCALE			-1L

void UpdateReport(uint8_t *Report,uint8_t Index,int32_t Value);
void Motion_Task(void);

#endif
