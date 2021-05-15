/*
 * hardware.c
 *
 * Created: May 10, 2021, 1:59:19 PM
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

void Init_Hardware(void)
{	
	Check_OPTION_BYTE();
	CLK->CKDIVR = 0; // HSIDIV = 0; CPUDIV = 0
		
	USB_disconnect();
	TIM4_Init();
	I2C_Init();
	USB_Init();
	Delay(US_DELAY(90000));
	enableInterrupts();
	MPU6050_Init();	
	USB_connect();
}

#define IWDG_KEY_ENABLE 0xcc

void RESET_CHIP(void)
{
	IWDG->KR = IWDG_KEY_ENABLE; // Reset the CPU: Enable the watchdog and wait until it expires
	while(1);    // Wait until reset occurs from IWDG
}

void FLASH_Data_lock(uint8_t lock)
{
	if(lock)
		FLASH->IAPSR = 0;
	else
	{
		/* Warning: keys are reversed on data memory !!! */
    FLASH->DUKR = FLASH_RASS_KEY2;
    FLASH->DUKR = FLASH_RASS_KEY1;		
	}
}

void FLASH_Wait(void)
{
	while(!(uint8_t)(FLASH->IAPSR & (FLASH_IAPSR_EOP | FLASH_IAPSR_WR_PG_DIS)))
		/* wait*/;
}

void Check_OPTION_BYTE(void)
{
	uint8_t option_byte;

	if ((OPT->OPT2 != (uint8_t)(~OPT->NOPT2))||((OPT->OPT2 & AFR0) == 0)) 
	{ // check AFR0
		option_byte = OPT->OPT2|AFR0; 		// set AFR0 = 1 // PORT C[7..5] Alternate Function
		
		FLASH_Data_lock(0);
		FLASH->CR2 |= FLASH_CR2_OPT;
		FLASH->NCR2 &= (uint8_t)(~FLASH_NCR2_NOPT);

		OPT->OPT2 = option_byte;
		FLASH_Wait();

		OPT->NOPT2 = (uint8_t)(~option_byte);
		FLASH_Wait();
		
		// Disable write access to option bytes
		FLASH->CR2 &= (uint8_t)(~FLASH_CR2_OPT);
		FLASH->NCR2 |= FLASH_NCR2_NOPT;

		FLASH_Data_lock(1);
    RESET_CHIP();
	}
	
	if ((OPT->OPT3 != (uint8_t)(~OPT->NOPT3))||((OPT->OPT3 & HSITRIM) == 0)) 
	{ // check HSITRIM
		option_byte = OPT->OPT3|HSITRIM;			// 4 bit on-the-fly trimming
		
		FLASH_Data_lock(0);
		FLASH->CR2 |= FLASH_CR2_OPT;
		FLASH->NCR2 &= (uint8_t)(~FLASH_NCR2_NOPT);

		OPT->OPT3 = option_byte;
		FLASH_Wait();

		OPT->NOPT3 = (uint8_t)(~option_byte);
		FLASH_Wait();
		
		// Disable write access to option bytes
		FLASH->CR2 &= (uint8_t)(~FLASH_CR2_OPT);
		FLASH->NCR2 |= FLASH_NCR2_NOPT;
		FLASH_Data_lock(1);
    RESET_CHIP();
	}
}

void TIM4_Init(void)
{
	TIM4->ARR = TIM4_RELOAD;
	TIM4->PSCR = TIM4_PRSC;
	// Counter Enable
	TIM4->CR1 = TIM4_CR1_CEN;
}

void Short_Delay(void)
{
		_asm("nop");
		_asm("nop");
		_asm("nop");
		_asm("nop");
		_asm("nop");	
		_asm("nop");		
}

// Use US_DELAY(X) to set value in us.  Maximum is ~104ms
void Delay(uint16_t t)
{
	for(;t;t--)
		Short_Delay();
}
