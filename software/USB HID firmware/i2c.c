/** 
	created: May 11, 2021, 8:04:48 AM
	modified from ST app note: AN3281 - STM8 8-bit MCUs I2C optimized examples

	*
  ******************************************************************************
  * @file    i2c_master_interrupt.c
  * @author  MCD Application Team
  * @version V0.0.3
  * @date    Feb 2010
  * @brief   This file contains all I2c function for optimized I2C master
  ******************************************************************************
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  *                COPYRIGHT 2009 STMicroelectronics
  */ 

#include "hardware.h"
#include "I2C.h"

I2C_t i2c_drv;

void I2C_Init(void) 
{	

#ifdef I2C_RESET
	if(I2C->SR3 & I2C_SR3_BUSY)
		i2c_reset();
#endif

	I2C->FREQR = I2C_FREQ_PERP;
	I2C->CCRL = I2C_CLKCTRL; 			// 350kHz
	I2C->CCRH = I2C_CCRH_FS; 			// Fast mode timing
	I2C->TRISER = I2C_RISETIME;		// 300ns
	
	I2C->OARH = I2C_OARH_ADDCONF; // addr configuration	
	
	// Event, Buffer IRQ
	I2C->ITR = I2C_ITR_ITEVTEN;
	I2C->CR1 = I2C_CR1_PE;
	i2c_drv.State = I2C_Idle;
}

@far @interrupt void I2C_IRQ(void)
{
	uint8_t sr1=I2C->SR1, sr2=I2C->SR2;
	
	if(sr1&I2C_SR1_SB)
	{
		// send 7-bit device addr
		I2C->DR = (i2c_drv.Addr<<1)|i2c_drv.Mode;
		i2c_drv.State = I2C_Addr;
	}
	
	if(sr1 & I2C_SR1_ADDR)
	{	
		I2C->SR3;
		
		if(i2c_drv.Mode == I2C_READ)
		{	
			//  Corrupted last received data in I2C Master Receiver mode
			switch(i2c_drv.Size)
			{
				case 1:
					/* Clear Add Ack Flag */
					I2C->CR2 &= ~I2C_CR2_ACK;
					I2C->CR2 |= I2C_CR2_STOP;
					I2C->ITR |= I2C_ITR_ITBUFEN;
					i2c_drv.State = I2C_Read_1Left;
					break;				
					
				case 2:
					i2c_drv.State = I2C_Read_2Left;
					break;
				
				case 3:
					i2c_drv.State = I2C_Read_3Left;
					break;
					
				default:
					i2c_drv.State = I2C_Read_LotsLeft;
			}
		}
		else
		{
			I2C->DR = *i2c_drv.Buf++;
			i2c_drv.Size--;
			i2c_drv.State = I2C_Writes;
		}
	}
	
	if(sr1 & I2C_SR1_RXNE)
	{
		if(i2c_drv.State == I2C_Read_1Left)
		{
			*i2c_drv.Buf++ = I2C->DR;			
			i2c_drv.State = I2C_Idle;
		}
		I2C->ITR &= ~I2C_ITR_ITBUFEN;  // Disable Buffer interrupts (errata)			
	}
	
	if(sr1 & I2C_SR1_BTF)
	{
		switch(i2c_drv.State)
		{				
			case I2C_Read_LotsLeft:
				*i2c_drv.Buf++ = I2C->DR;				
				i2c_drv.Size--;			
				
				if (i2c_drv.Size<= 3)
					i2c_drv.State = I2C_Read_3Left;
				break;
				
			case I2C_Read_3Left:
				I2C->CR2 &= ~I2C_CR2_ACK;                     			// Set NACK (ACK=0)
				*i2c_drv.Buf++ = I2C->DR;														// Read next data byte
				I2C->CR2 |= I2C_CR2_STOP;                   				// generate stop request here (STOP=1)
				*i2c_drv.Buf++ = I2C->DR;														// Read next data byte
				I2C->ITR |= I2C_ITR_ITBUFEN; 												// Enable Buffer interrupts				
				i2c_drv.State =	I2C_Read_1Left;
				break;
				
			case I2C_Read_2Left:
				I2C->CR2 |= I2C_CR2_STOP;                   				// generate stop request here (STOP=1)
				*i2c_drv.Buf++ = I2C->DR;				
				*i2c_drv.Buf++ = I2C->DR;	
				i2c_drv.State = I2C_Idle;				
				break;
				
			case I2C_Writes:
				if(i2c_drv.Size && (I2C->SR1 & I2C_SR1_TXE))
				{
					I2C->DR = *i2c_drv.Buf++;
					i2c_drv.Size--;
				}
				else
				{
					I2C->CR2 |= I2C_CR2_STOP;                   			// Generate stop here (STOP=1)
					i2c_drv.State = I2C_Idle;
				}
		}
	}
}

uint8_t I2C_Read(uint8_t DeviceAddr, uint8_t Reg_Offset, uint8_t *Buffer, uint8_t Size)
{
	if ((I2C->SR3 & I2C_SR3_BUSY)||(i2c_drv.State != I2C_Idle))
		return 0 ;

	// Write register offset
	I2C_Write(DeviceAddr,&Reg_Offset,1);
	
	// was here I2C get stuck due to USB traffic interference (fixed)
	// This delay skips over the Stop from I2C_Write()
	
	if (I2C_Busy(I2C_POLL))
		Delay(I2C_WR_RD_DELAY);

	// Change to read mode
	// set ACK
	I2C->CR2 |= I2C_CR2_ACK;
	// reset POS
	I2C->CR2 &= ~I2C_CR2_POS;
	// setup I2C comm. in Read
	i2c_drv.Mode = I2C_READ;
	i2c_drv.Addr = DeviceAddr;
	i2c_drv.Buf = Buffer;
	i2c_drv.Size = Size;
	
	//generate Start
	I2C->CR2 |= I2C_CR2_START;
	i2c_drv.State = I2C_Read_StartBit;
	I2C->ITR |= I2C_ITR_ITEVTEN;                  // re-enable interrupt
	
	return 1;
}

uint8_t I2C_Write(uint8_t DeviceAddr, uint8_t *Buffer, uint8_t Size)
{
	static uint8_t Reg[2];
	
	if ((I2C->SR3 & I2C_SR3_BUSY)||(i2c_drv.State != I2C_Idle))
		return 0 ;	
	
	// set ACK
	I2C->CR2 |= I2C_CR2_ACK;
	// reset POS
	I2C->CR2 &= ~I2C_CR2_POS;
	
	i2c_drv.Mode = I2C_WRITE;
	i2c_drv.Addr = DeviceAddr;
	i2c_drv.Buf = Buffer;
	i2c_drv.Size = Size;
	
	// generate Start
	I2C->CR2 |= I2C_CR2_START;	
	i2c_drv.State = I2C_Write_StartBit;
	
	return 1;	
}

uint8_t I2C_Busy(uint8_t WaitLoop)
{
	if(WaitLoop)
		while(i2c_drv.State != I2C_Idle)
			/* wait */;
		
	return (i2c_drv.State != I2C_Idle);
}

#ifdef I2C_RESET
void i2c_reset(void)
{ 
	uint8_t i;
		// manually toggle SCL to unstuck
	I2C_PORT->ODR = I2C_PINS;
	I2C_PORT->DDR = I2C_PINS;
	
	I2C->CR1 &= ~I2C_CR1_PE;
	
	for(i=9;i && ((I2C_PORT->IDR & I2C_PINS)!=I2C_PINS);i--)
	{
		I2C_PORT->ODR &= ~I2C_SCL;
		Delay(1);
		I2C_PORT->ODR |= I2C_SCL;
		Delay(1);
	}
	
	// I2C stop
	I2C_PORT->ODR &= ~I2C_SDA;
	Delay(1);	
	I2C_PORT->ODR = I2C_PINS;
	
	I2C->CR1 = I2C_CR1_PE;
	I2C->CR2 |= I2C_CR2_SWRST;
	
	while(I2C->SR3 & I2C_SR3_BUSY)
	  /* wait */;
		
	I2C->CR2 &= ~I2C_CR2_SWRST;		
}
#endif