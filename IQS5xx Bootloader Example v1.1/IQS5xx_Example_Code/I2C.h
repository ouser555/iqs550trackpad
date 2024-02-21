#ifndef I2C_h
#define I2C_h

#define START           0x08
#define REPEATED_START  0x10
#define MT_SLA_ACK		0x18
#define MT_SLA_NACK		0x20
#define MT_DATA_ACK     0x28
#define MT_DATA_NACK    0x30
#define MR_SLA_ACK		0x40
#define MR_SLA_NACK		0x48
#define MR_DATA_ACK     0x50
#define MR_DATA_NACK    0x58
#define LOST_ARBTRTN    0x38
#define TWI_STATUS      (TWSR & 0xF8)
	

void I2C_Setup();
uint8_t I2C_Write_16(uint8_t ui8SlaveAdr, uint16_t ui16RegisterAddress, uint8_t *pData, uint8_t ui8NoOfBytes);
uint8_t I2C_Read_16(uint8_t ui8SlaveAdr, uint16_t ui16RegisterAddress, uint8_t *pData, uint8_t ui8NoOfBytes);
void RDY_wait();
void Close_Comms();
uint8_t I2C_Write_8(uint8_t ui8SlaveAdr, uint8_t ui8RegisterAddress, uint8_t *pData, uint8_t ui8NoOfBytes);
uint8_t I2C_Read_8(uint8_t ui8SlaveAdr, uint8_t ui8RegisterAddress, uint8_t *pData, uint8_t ui8NoOfBytes);
uint8_t BL_Block_Read_16(uint8_t ui8SlaveAdr, uint16_t ui16RegisterAddress, uint8_t *pData, uint8_t ui8NoOfBytes);

#endif
