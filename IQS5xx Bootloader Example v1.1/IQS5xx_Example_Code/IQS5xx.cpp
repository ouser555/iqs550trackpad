#include "Arduino.h"
#include "defs.h"
#include "I2C.h"
#include "IQS5xx.h"
#include "HEX_Array.h"
#include <avr/pgmspace.h>   

//
// Local function prototypes
//
void Print_signed(int16_t num);
void Print_unsigned(uint16_t num);

extern uint8_t 	Data_Buff[44];
extern uint16_t	ui16SnapStatus[15], ui16PrevSnap[15];

//*****************************************************************************
//
//! Acknowledge the reset flag
//!
//! This function simply sets the ACK_RESET bit found in the SYSTEM_CONTROL_0 
//! register.  By setting this bit the SHOW_RESET flag is cleared in the 
//! SYSTEM_INFO_0 register.  During normal operation, the SHOW_RESET bit can be 
//! monitored and if it becomes set, then an unexpected reset has occurred.  
//! If any device configuration is needed, it must then be repeated.
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void IQS5xx_AcknowledgeReset(void) 
{
    static  uint8_t System_ctrl_0 = ACK_RESET;  

	I2C_Write_16(IQS5xx_ADDR, SystemControl0_adr, &System_ctrl_0, 1);
}


//*****************************************************************************
//
//! Reset the IQS5xx with the NRST line
//!
//! This function relies on the fact that the IQS5xx NRST pin is connected to
//! the Arduino digital I/O 6.  Note this is not connected on the IQS572 shield 
//! PCB, and needs to be manually wired to gain access to the NRST net. 
//! The I/O is simply set to output, guaranteed to go LOW, and then changed 
//! to an input so that the internal pull-up on the IQS5xx releases it from 
//! reset.
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void IQS5xx_Reset(void) 
{
	pinMode(NRST_PIN, OUTPUT);
	digitalWrite(NRST_PIN, LOW);
	pinMode(NRST_PIN, INPUT);
}

//*****************************************************************************
//
//! Program the IQS5xx through the i2c bootloader
//!
//! This function resets the IQS5xx device, by means of toggling the digital
//! pin 7 (which possibly needs to be manually wired to the NRST on the IQS5xx)
//! Immediately after reset the bootloader is polled on the i2c, and once this
//! is acknowledged, then bootloader is entered.  The version is then confirmed,
//! and then the device is programmed.  A CRC check is done, and also a read
//! back of the section excluded in the CRC to confirm successful programming.
//! Then the IC is reset again, or bootloader exit command is given.
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void ProgramIQS5xx(void) 
{
	uint8_t ui8DataBuffer[64], ui8BlockNo, ui8ByteNo, ui8Temp;
	bool bNVFail;
	
	// Reset the IQS5xx so that the window opportunity after reset can be used 
	// to enter the bootloader mode.
	//
	IQS5xx_Reset();
	delay(1);
	// 
	// Change i2c slave to the bootloader address, which is defined as 0x40
	// XOR'ed with the slave address
	//
	//
	// Now poll on that address to enter bootloader
	//
	while(!I2C_Read_8(BL_ADDR, BL_CMD_VERSION, &ui8DataBuffer[0] ,2))
	{
		Serial.print("Polling to enter bootloader...");
	};
	if((ui8DataBuffer[0] == 0x02) && (ui8DataBuffer[1] == 0x00))
	{
		Serial.println("Bootloader version correct: 2.0");
	}
	else
	{
		Serial.print("Incorrect bootloader version: ");	
		Serial.print(ui8DataBuffer[0]);
		Serial.print(".");
		Serial.println(ui8DataBuffer[1]);
	};
	//
	// Now write the program area, which is address 0x8400 to 0xBFFF.
	// This is written in blocks of 64 bytes, and gives a total of 240
	// such blocks required to write.
	//
	for (ui8BlockNo = 0; ui8BlockNo < 240; ui8BlockNo++)
    {
		for (ui8ByteNo = 0; ui8ByteNo < 64; ui8ByteNo++)
		{
			ui8DataBuffer[ui8ByteNo] = pgm_read_byte_near(hex_array + ((ui8BlockNo*64)+ui8ByteNo));
		}
		I2C_Write_16(BL_ADDR, 0x8400 + (ui8BlockNo*64), &ui8DataBuffer[0], 64);
		//
		// This delay is necessary to give the bootloader time to write 
        // the 64 byte buffer into memory
        // This delay is only necessary after each block write
        // 
		delay(7);
    }
	//
	// Now program the checksum descriptor, so that the CRC check can be run
	//
	I2C_Write_16(BL_ADDR, 0x83C0, &crc_array[0], 64);
	delay(7);
	//
	// Perform CRC check, a '0x00' returned is pass, '0x01' is fail
	//
	I2C_Read_8(BL_ADDR, BL_CMD_CRCCHECK, &ui8DataBuffer[0], 1);
	
	if(ui8DataBuffer[0] == 0x00)
	{
		Serial.println("CRC Success");
	}
	else
	{
		Serial.println("CRC Fail");
	}
	//
	// The IQS5xx-B000 non-volatile memory does not form part of the CRC 
	// checksum, so a read back and compare is needed to verify this section, 
	// which is located 0xBE00 to 0xBFFF (512 bytes, or 8x64 blocks)
	//
	bNVFail = FALSE;
	for (ui8BlockNo = 0; ui8BlockNo < 8; ui8BlockNo++)
    {
		BL_Block_Read_16(BL_ADDR, 0xBE00 + (ui8BlockNo*64), &ui8DataBuffer[0], 64);
		//
		// Now compare each byte read back to that in the array, to make sure 
        // each byte is correct.
        //
		for (ui8ByteNo = 0; ui8ByteNo < 64; ui8ByteNo++)
		{
			// this read method is needed because the data is stored in program memory
			//
			ui8Temp = pgm_read_byte_near(hex_array + (0xBE00-0x8400) + (ui8BlockNo*64) + ui8ByteNo);
			if(ui8DataBuffer[ui8ByteNo] != ui8Temp)
			{
				bNVFail = TRUE;
			}
		}
    }
	if(bNVFail)
	{
		Serial.println("Non-volatile read back: FAIL");
	}
	else
	{
		Serial.println("Non-volatile read back: PASS");
	}
	//
	// Here you can either reset the IQS5xx, and then go to the application, or
	// you can send the 0x02 command to jump to the application code.
	//
	//
	
	// Reset to exit bootloader and go into application
	//
	// IQS5xx_Reset();
	
	//
	// Give command to enter application code, thus exiting bootloader
	//
	I2C_Write_8(BL_ADDR, BL_CMD_EXIT, &ui8DataBuffer[0], 1);
	//
	// Delay to allow IQS5xx to power up and initialise
	//
	delay(10);	
}

//*****************************************************************************
//
//! Read and display version
//!
//! This function reads the version details from the IQS5xx and sends this to 
//! the display port.
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void IQS5xx_CheckVersion(void) 
{
	uint8_t ui8DataBuffer[6];
	//
	// Dont wait for RDY here, since the device could be in EventMode, and then
	// there will be no communication window to complete this.  Rather do a 
	// forced communication, where clock stretching will be done on the IQS5xx
	// until an appropriate time to complete the i2c.
	//
	I2C_Read_16(IQS5xx_ADDR, ProductNumber_adr, &ui8DataBuffer[0] ,6);
  
	Serial.print("Product "); 
	Serial.print((ui8DataBuffer[0]<<8) + ui8DataBuffer[1]);
	Serial.print("  Project ");
	Serial.print((ui8DataBuffer[2]<<8) + ui8DataBuffer[3]);
	Serial.print("  Version "); 
	Serial.print(ui8DataBuffer[4]); Serial.print("."); 
	Serial.println(ui8DataBuffer[5]);
}

//*****************************************************************************
//
//! Display a snap state change
//!                            
//! If the state of any snap output has changed, then this function can be used
//!	to display which Rx/Tx channel has changed status.  
//!			                         
//! \param None
//!                                           
//! \return None
//                                                      
//*****************************************************************************
void DisplaySnap(void)
{
	uint8_t		ui8Tx, ui8Rx;
	uint16_t	ui16ToggledBits;
	
	for(ui8Tx = 0; ui8Tx < 15; ui8Tx++)
	{
		ui16ToggledBits = ui16PrevSnap[ui8Tx] ^ ui16SnapStatus[ui8Tx];

		for(ui8Rx = 0; ui8Rx < 10; ui8Rx++)
		{
			if(BitIsSet(ui16ToggledBits, ui8Rx))
			{
				if(BitIsSet(ui16SnapStatus[ui8Tx], ui8Rx))
				{
					Serial.print("Snap set on Rx");
				}
				else
				{
					Serial.print("Snap released on Rx");
				}
				Serial.print(ui8Rx);
				Serial.print("/Tx");
				Serial.print(ui8Tx);
				Serial.println(" channel    ");
			}
		}
	}
}

//*****************************************************************************
//
//! Process the data received
//!                            
//! This function sorts the read bytes from the IQS5xx and prints relevant data 
//! on serial port. 
//! REL_X[n]: Relative X Position of the finger n; n is from (1 to 5)
//! REL_Y[n]: Relative X Position of the finger n; n is from (1 to 5)
//! ABS_X[n]: Absolute X Position of the finger n; n is from (1 to 5)
//! ABS_Y[n]: Absolute Y Position of the finger n; n is from (1 to 5)
//! ui16TouchStrength[n]   : Touch strength of finger n; n is from (1 to 5)
//! ui8Area[n]   : Touch area of finger n; this is number of channels under touch 
//! for a particular finger; 
//! Where 'n' is from (1 to 5)
//!			                         
//! \param None
//!                                           
//! \return None
//                                                      
//*****************************************************************************
void Process_XY(void) 
{ 
	uint8_t 	i; 
	static uint8_t ui8FirstTouch = 0;
	uint8_t 	ui8NoOfFingers;
	uint8_t 	ui8SystemFlags[2];
	int16_t 	i16RelX[6];
	int16_t 	i16RelY[6];
	uint16_t 	ui16AbsX[6];
	uint16_t 	ui16AbsY[6];
	uint16_t 	ui16TouchStrength[6];
	uint8_t  	ui8Area[6];
 
	ui8SystemFlags[0] = Data_Buff[2];
	ui8SystemFlags[1] = Data_Buff[3];
	ui8NoOfFingers = Data_Buff[4];
	//
	// Re-initialize the device if unexpected RESET detected
	//
	if((ui8SystemFlags[0] & SHOW_RESET) != 0)
	{
		Serial.println("RESET DETECTED");
		IQS5xx_AcknowledgeReset(); 
		return;
	}

	if((ui8SystemFlags[1] & SNAP_TOGGLE) != 0)
	{
		// A snap state has changed, thus indicate which channel
		//
		DisplaySnap();
		return;
	}

	if((Data_Buff[0]) == SINGLE_TAP) 
	{		
		Serial.println("Single Tap  "); 
	}
	else if((Data_Buff[1]) == TWO_FINGER_TAP)   
	{
		Serial.println("2 Finger Tap"); 
	}		

	if(ui8NoOfFingers != 0) 
	{
		if (!(ui8FirstTouch)) 
		{
			Serial.print("Gestures:    ");
			Serial.print(" RelX: ");
			Serial.print("RelY: ");
			Serial.print("Fig: ");
			Serial.print("X1:  "); Serial.print("Y1:  "); Serial.print("TS1: "); Serial.print("TA1: ");
			Serial.print("X2:  "); Serial.print("Y2:  "); Serial.print("TS2: "); Serial.print("TA2: ");
			Serial.print("X3:  "); Serial.print("Y3:  "); Serial.print("TS3: "); Serial.print("TA3: ");
			Serial.print("X4:  "); Serial.print("Y4:  "); Serial.print("TS4: "); Serial.print("TA4: ");
			Serial.print("X5:  "); Serial.print("Y5:  "); Serial.print("TS5: "); Serial.println("TA5: ");
			ui8FirstTouch = 1;
		}

		switch (Data_Buff[0])
		{
			case SINGLE_TAP		:  	Serial.print("Single Tap  "); 
									break;
			case TAP_AND_HOLD	:  	Serial.print("Tap & Hold  "); 
									break;
			case SWIPE_X_NEG	:  	Serial.print("Swipe X-    "); 
									break;
			case SWIPE_X_POS	:  	Serial.print("Swipe X+    "); 
									break;
			case SWIPE_Y_POS	:  	Serial.print("Swipe Y+    "); 
									break;
			case SWIPE_Y_NEG	:  	Serial.print("Swipe Y-    "); 
									break;
		}

		switch (Data_Buff[1])
		{
			case TWO_FINGER_TAP	:  	Serial.print("2 Finger Tap"); 
									break;
			case SCROLL			:  	Serial.print("Scroll      "); 
									break;
			case ZOOM			:  	Serial.print("Zoom        "); 
									break;
		}
		if((Data_Buff[0] | Data_Buff[1]) == 0) 
		{
			Serial.print("            ");
		}

		i16RelX[1] = ((Data_Buff[5] << 8) | (Data_Buff[6]));
		i16RelY[1] = ((Data_Buff[7] << 8) | (Data_Buff[8]));
		Print_signed(i16RelX[1]);
		Print_signed(i16RelY[1]);
		Print_unsigned(ui8NoOfFingers);    

		for (i = 0; i < 5; i++)
		{
			ui16AbsX[i+1] = ((Data_Buff[(7*i)+9] << 8) | (Data_Buff[(7*i)+10])); //9-16-23-30-37//10-17-24-31-38
			ui16AbsY[i+1] = ((Data_Buff[(7*i)+11] << 8) | (Data_Buff[(7*i)+12])); //11-18-25-32-39//12-19-26-33-40
			ui16TouchStrength[i+1] = ((Data_Buff[(7*i)+13] << 8) | (Data_Buff[(7*i)+14])); //13-20-27-34-11/14-21-28-35-42
			ui8Area[i+1] = (Data_Buff[7*i+15]); //15-22-29-36-43
			Print_unsigned(ui16AbsX[i+1]);
			Print_unsigned(ui16AbsY[i+1]);
			Print_unsigned(ui16TouchStrength[i+1]);
			Print_unsigned(ui8Area[i+1]);
		}
		Serial.println("");
	} 
	else 
	{
		ui8FirstTouch = 0;
	}
}

//*****************************************************************************
//
//! Print a signed value on serial display
//!                            
//! Print the signed integer on the serial port with adjusted tabs 
//! on serial port for easy column reading. 
//!			                         
//! \param None
//!                                           
//! \return None
//                                                      
//*****************************************************************************
void Print_signed(int16_t i16Num)
{
	if(i16Num < (-99))
	{
		Serial.print(" ");
	}
	else if(i16Num < (-9))
	{
		Serial.print("  ");
	}
	else if(i16Num < 0)
	{
		Serial.print("   ");
	}
	else if(i16Num < 10)
	{
		Serial.print("    ");
	}
	else if(i16Num < 100)
	{
		Serial.print("   ");
	}
	else if(i16Num < 1000)
	{
		Serial.print("  ");
	}
	Serial.print(i16Num);
}

//*****************************************************************************
//
//! Print an unsigned value on serial display
//!                            
//! Print the unsigned integer on the serial port with adjusted tabs 
//! on serial port for easy column reading. 
//!			                         
//! \param None
//!                                           
//! \return None
//                                                      
//*****************************************************************************
void Print_unsigned(uint16_t ui16Num)
{
	if(ui16Num < 10)
	{
		Serial.print("    ");
	}
	else if(ui16Num < 100)
	{
		Serial.print("   ");
	}
	else if(ui16Num < 1000)
	{
		Serial.print("  ");
	}
	else if(ui16Num < 10000)
	{
		Serial.print(" ");
	}

	if(ui16Num > 10000)
	{
		Serial.print("  xxx");
	}
	else
	{
		Serial.print(ui16Num);
	}
}


