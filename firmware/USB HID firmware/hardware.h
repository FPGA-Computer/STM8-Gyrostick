/*
 * hardware.h
 *
 * Created: October 31, 2019, 12:33:01 PM
 *  Author: K. C. Lee
 * Copyright (c) 2019 by K. C. Lee
 
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

#ifndef HARDWARE_H_
#define HARDWARE_H_

#ifndef __CSMC__
#define __CSMC__
#endif
#ifndef STM8S003
#define STM8S003
#endif

#include "stm8s.h"
#include <stdio.h>
#include "i2c.h"
#include "motion.h"
#include "mpu6050.h"

// STM8S003F3P6
enum _PA { PA1=0x02, PA2=0x04, PA3=0x08 };
enum _PB { PB4=0x10, PB5=0x20 };
enum _PC { PC3=0x08, PC4=0x10, PC5=0x20, PC6=0x40, PC7=0x80 };
enum _PD { PD1=0x02, PD2=0x04, PD3=0x08, PD4=0x10, PD5=0x20, PD6=0x40 };

#define GPIO(X)					((GPIO_TypeDef *)(X))

#define CPU_CLOCK				16000000UL
#define EE_Addr					0x4000

enum _OPT2 { AFR0 = 0x01, AFR1=0x02, AFR2=0x04, AFR3=0x08, AFR4=0x10, AFR5=0x20, AFR6=0x40, AFR7 = 0x80 };
enum _OPT3 { HSITRIM = 0x10, LSI_EN = 0x08, IWDG_HW = 0x04, WWDG_HW = 0x02, WWDG_HALT = 0x01 };

#define FLASH_RASS_KEY1 ((uint8_t)0x56) /*!< First RASS key */
#define FLASH_RASS_KEY2 ((uint8_t)0xAE)

#define TIM4_PRSC				7
#define TIM4_FREQ				500
#define TIM4_Slow				100
#define TIM4_RELOAD			(CPU_CLOCK/(1<<TIM4_PRSC)/TIM4_FREQ)
#define TIM4_CNTMAX			(TIM4_FREQ/TIM4_Slow)

#define EXTI_CR1				0x30

extern volatile uint8_t TIM4_Update_Flag;


// Delays in us.  The delay has been calibrated for 16MHz
// Maximum delay is about 100ms (16-bit)

#define DELAY_TWEAK			0.63
#define US_DELAY(X)			((uint16_t)(X*DELAY_TWEAK))
#define USB_IDLE_DELAY	US_DELAY(10)

#define ms_TO_TICKS(X)	((X)/10)

#define REPORT_SIZE		  7

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

enum EndPoint
{
	EP0, EP1
};

extern uint8_t Report_Acc[REPORT_SIZE],Report_Gyro[REPORT_SIZE];
extern uint8_t ReportID;

void Init_Hardware(void);
void RESET_CHIP(void);
void Check_OPTION_BYTE(void);
void TIM4_Init(void);
void Delay(uint16_t t);
void HID_Task(void);
void FLASH_Data_lock(uint8_t lock);
void FLASH_Wait(void);

#endif
