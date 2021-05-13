#ifndef I2C_H_
#define I2C_H_

// peripheral clock in MHz
#define I2C_PORT				GPIOB
#define I2C_SCL					PB4
#define I2C_SDA					PB5
#define I2C_PINS				(I2C_SCL|I2C_SDA)

#define I2C_FREQ_PERP		(CPU_CLOCK/1000000UL)
#define I2C_SPEED				350000UL
#define I2C_RATIO				3
#define I2C_CLKCTRL			(CPU_CLOCK/I2C_SPEED/I2C_RATIO)

#define I2C_RISETIME				5   // 300/62.5 + 1= 5  (maximum 300ns)

enum _I2C_STATES
{
	I2C_Idle,	I2C_Addr,
	
	I2C_Read_StartBit,
	I2C_Read_1Left,
	I2C_Read_2Left,
	I2C_Read_3Left,
	I2C_Read_LotsLeft,
	
	I2C_Write_StartBit,
	I2C_Writes,

	I2C_READ = 0x01,
	I2C_WRITE = 0x00
};

typedef struct
{
	uint8_t	Addr:7;
	uint8_t Mode:1;
	uint8_t *Buf;	
	uint8_t Size;
	uint8_t State;
} I2C_t;

#define I2C_WAIT					1
#define I2C_POLL					0

// Delay between write to a read
#define I2C_WR_RD_DELAY		US_DELAY(40)

void I2C_Init(void);
uint8_t I2C_Read(uint8_t DeviceAddr, uint8_t Reg_Offset, uint8_t *Buffer, uint8_t Size);
uint8_t I2C_Write(uint8_t DeviceAddr, uint8_t *Buffer, uint8_t Size);
uint8_t I2C_Busy(uint8_t WaitLoop);

// brute force I2C reset
void i2c_reset(void);

#endif /* I2C_H */

