/*
 * motion.c
 *
 * Created: May 11, 2021, 8:31:16 AM
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
 
#include "hardware.h"

int16_t MPU6050_Regs[MPU6050_REGSIZE/2];
uint8_t MPU6050_State = USB_Idle;

// Init table values in pairs: (addr, value)
const uint8_t MPU6050_InitValues[]=
{
	// Cycle mode, clock source = X axis gyro
	MPU6050_RA_PWR_MGMT_1,MPU6050_PWR1_CYCLE_BIT|1,
	// Low pass filter = 42Hz
	MPU6050_RA_CONFIG,MPU6050_DLPF_BW_42,
	
	// Accelerometer: +/- 2g
	//MPU6050_RA_ACCEL_CONFIG,MPU6050_ACCEL_FS_2<<MPU6050_GCONFIG_FS_SEL_BIT-1,
	
	// Gyro: +/- 500 deg/s
	MPU6050_RA_GYRO_CONFIG,MPU6050_GYRO_FS_500<<MPU6050_GCONFIG_FS_SEL_BIT-1,

};

void MPU6050_Init(void)
{
	uint8_t i;

	for(i=0;i<sizeof(MPU6050_InitValues);i+=2)
	{
		I2C_Write(MPU6050_ADDRESS_AD0_LOW,&MPU6050_InitValues[i],2);
		I2C_Busy(I2C_WAIT);
	}
	
	Report_Acc[0]= ID_Acc;
	Report_Gyro[0]= ID_Gyro;
}

void MPU6050_Scale(void)
{	
	uint8_t i, *src, *dst;

	// fix up raw data before transfer
	
	
	// transfer to reports
	src = (uint8_t*) MPU6050_Regs;
	dst = &Report_Acc[1];								// after the report ID
	
	// transfer & swap endianness
	for(i=0;i<6;i+=2)
	{
		dst[i+1] = *src++;
		dst[i] = *src++;		
	}

	// skip over temperature
	src+=2;
	dst = &Report_Gyro[1];							// after the report ID
	
	// transfer & swap endianness
	for(i=0;i<6;i+=2)
	{
		dst[i+1] = *src++;
		dst[i] = *src++;		
	}
}