#ifndef defs_h
#define defs_h

#define IQS5xx_ADDR          	0x74
#define RDY_PIN              	2
#define	END_WINDOW				(uint16_t)0xEEEE

// #define I2C_SPEED				100000		// 100kHz i2c clock
#define I2C_SPEED				400000		// 400kHz i2c clock

#define BitIsSet(VAR,Index)		(VAR & (1<<Index)) != 0
#define BitIsNotSet(VAR,Index)	(VAR & (1<<Index)) == 0

#define SetBit(VAR,Index)		VAR |= (1<<Index)
#define ClearBit(VAR,Index)		VAR &= (uint8_t)(~(1<<Index))

#define FALSE 					0
#define	TRUE 					!FALSE

// BOOTLOADER DEFINES
#define NRST_PIN				7
#define	BL_ADDR					(IQS5xx_ADDR^0x40)
#define	BL_CMD_VERSION			0x00
#define BL_CMD_READ				0x01
#define	BL_CMD_EXIT				0x02
#define	BL_CMD_CRCCHECK			0x03

void Process_XY();
void IQS5xx_AcknowledgeReset(); 
void IQS5xx_CheckVersion();
void IQS5xx_Reset();
void ProgramIQS5xx();

#endif