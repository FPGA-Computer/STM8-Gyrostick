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
#include "usb.h"

uint8_t Report_Acc[REPORT_SIZE],Report_Gyro[REPORT_SIZE];
uint8_t ReportID = ID_Acc;

int16_t MPU6050_Regs[MPU6050_REGSIZE/2];
uint8_t MPU6050_State = USB_Idle;

// Init table values in pairs: (addr, value)
const uint8_t MPU6050_InitValues[]=
{
	// Cycle mode, clock source = X axis gyro
	MPU6050_RA_PWR_MGMT_1,MPU6050_PWR1_CYCLE_BIT|1,
	// Low pass filter = 42Hz
	MPU6050_RA_CONFIG,MPU6050_DLPF_BW_42,
	
	//Accelerometer: +/- 2g
	//MPU6050_RA_ACCEL_CONFIG,MPU6050_ACCEL_FS_2<<MPU6050_GCONFIG_FS_SEL_BIT-1,
	
	// Gyro: +/- 1000 deg/s
	MPU6050_RA_GYRO_CONFIG,MPU6050_GYRO_FS_1000<<MPU6050_GCONFIG_FS_SEL_BIT-1,
};

void MPU6050_Init(void)
{
	uint8_t i;

	for(i=0;i<sizeof(MPU6050_InitValues);i+=2)
	{
		I2C_Write(MPU6050_ADDRESS_AD0_LOW,&MPU6050_InitValues[i],2);
		I2C_Busy(I2C_WAIT);
	}
	
	Report_Acc[Report_ID]= ID_Acc;
	Report_Gyro[Report_ID]= ID_Gyro;
}

void UpdateReport(uint8_t *Report,uint8_t Index,int32_t Value)
{
	int16_t Value16;

	// clip scaled values to int16
	if(Value > S16_MAX)
		Value = S16_MAX;
	else if(Value < S16_MIN)
		Value = S16_MIN;
	
 Report[Index] = Value & 0x00ff; 
 Report[Index+1] = Value >> 8;
}

void MPU6050_Scale(void)
{	
	// scale and transfer to reports
	UpdateReport(Report_Acc,Report_X,MPU6050_Regs[RAW_Acc_Y]*ACCEL_XY_SCALE);
	UpdateReport(Report_Acc,Report_Y,MPU6050_Regs[RAW_Acc_X]*ACCEL_XY_SCALE);
	UpdateReport(Report_Acc,Report_Z,MPU6050_Regs[RAW_Acc_Z]*ACCEL_Z_SCALE);
	
	UpdateReport(Report_Gyro,Report_X,-MPU6050_Regs[RAW_Gyro_Y]*GYRO_SCALE);
	UpdateReport(Report_Gyro,Report_Y,-MPU6050_Regs[RAW_Gyro_X]*GYRO_SCALE);
	UpdateReport(Report_Gyro,Report_Z,-MPU6050_Regs[RAW_Gyro_Z]*GYRO_SCALE);
}

void Motion_Task(void)
{
	if(USB_Tx_Ready(EP1))		
		switch(MPU6050_State)
		{
			case USB_Rdy:
				Delay(USB_IDLE_DELAY);		// wait until USB packet is done, so it won't interfere with I2C
				I2C_Read(MPU6050_ADDRESS_AD0_LOW,MPU6050_RA_ACCEL_XOUT_H,(uint8_t*)MPU6050_Regs,MPU6050_REGSIZE);
				MPU6050_State = MPU6050_I2C_Read;
				break;
				
			case MPU6050_I2C_Read:
				if (!I2C_Busy(I2C_POLL))
				{ 
					MPU6050_Scale();
					
					// Alternate sending each of the reports
					if(ReportID == ID_Acc)
					{
						USB_Send_Data(Report_Acc,REPORT_SIZE,EP1);
						ReportID = ID_Gyro;
					}
					else
					{
						USB_Send_Data(Report_Gyro,REPORT_SIZE,EP1);
						ReportID = ID_Acc;
					}
					MPU6050_State = USB_Rdy;
				}
		}
}
