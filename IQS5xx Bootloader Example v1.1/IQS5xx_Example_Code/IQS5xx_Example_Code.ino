


/******************************************************************************
*   			...............,MMMMM...................
                ...............,MMMMM...................
                ....MM.........,MMMMM.........MM........
                ..MMMMM...... .,MMMMM........MMMMM......
                .MMMMM....... .,MMMMM. .... ..MMMMM ....
                MMMMM..........,MMMMM...........MMMM....
                MMMM.. .. ..................... MMMMD...
                MMMM.... .......................MMMMM...
                MMMM............................MMMM ...
                .MMMMO.........................MMMMM....
                .MMMMM8   ........ . ...  ....MMMMM ....
                ...=MMMMMM................DMMMMMM ......
                .... MMMMMMMM .........7MMMMMMM.........
                ......... MMMMMMMMMMMMMMMM~ ............
                ...............MMMMMMMMMMMMM............
                .................MMM$......MMMMMMMMMMMM.
                ................MMMM.........:MMMMMMM ..
                ....... . ........ . ..................
				
                   IQS5xx_B000 I2C Bootloader example code
				   
   This program is free software; you can redistribute  it and/or modify it
   under  the terms of  the GNU General  Public License as published by the
   Free Software Foundation;  either version 2 of the  License, or (at your
   option) any later version.

   Azoteq (Pty) Ltd does not take responsibility for the use of this driver

   This driver is an example driver. It will need to be ported to
   the specific platform and for the specific case in which it is used.
   Copyright (C) 2016 Azoteq (Pty) Ltd                                       
   Name:              :   IQS5xx_B000 Arduino Bootloader
   Revision           :   V1.1
   Author             :   Azoteq
   Date               :   08/02/2017

                            Copyright by 
                           Azoteq (Pty) Ltd                              
                        Republic of South Africa                           
                                        
                         Tel: +27(0)21 863 0033                         
                         E-mail: info@azoteq.com  

       
    Connections between IQS5xx and Ardiuno for are shown below, please add pull 
	ups on the SDA and SCL if not present on the IQS5xx board 
		
			IQS5xx                            ARDUINO
			___________                _____________________
			|         |                |                     |
			|      RDY|----------------|DIGITAL PIN 2        |
			|      SDA|----------------|SDA                  |
			|      SCL|----------------|SCL                  |
			|    VDDHI|----------------|3v3                  |
			|      GND|----------------|GND                  |  
			|     NRST|----------------|DIGITAL PIN 7        | **Must possibly be manually connected		
			-----------                -----------------------
******************************************************************************/
#include "IQS5xx.h"
#include "defs.h"
#include "I2C.h"

uint8_t 	Data_Buff[44];
uint16_t	ui16SnapStatus[15], ui16PrevSnap[15];

//*****************************************************************************
//
//! General arduino device setup
//!                            
//! This function configures the i2c protocol, and then goes ahead with 
//!	progamming the IQS5xx device through the i2c bootloader.  After this the  
//!	standard IQS5xx-B000 functionality is confirmed, so the reset indication 
//!	oflag is acknowledged (cleared), and the version of B000 firmware is   
//! checked.				                         
//                                                      
//*****************************************************************************
void setup()
{
	// Setup serial baud rate
	//
	Serial.begin(115200);
	//
	// Configure RDY pin
	//
	pinMode(RDY_PIN, INPUT);
	
	I2C_Setup();
	//
	// Use the bootloader to program the IQS5xx
	//
	ProgramIQS5xx();
	//
	// Clear the reset indication bit, so that from here on a reset can be 
	// detected if the bit becomes set
	//
	IQS5xx_AcknowledgeReset();
	//
	// Read the version and display on the serial terminal
	//
	IQS5xx_CheckVersion();
	//
	// End the communication window
	//
	Close_Comms();
	delay(1);
}

//*****************************************************************************
//
//! Arduino continuous loop function
//!                            
//! The loop function reads the data from the IQS5xx, and processes it under 
//! Process_XY function.	
//!		                         
//                                                      
//*****************************************************************************
void loop()
{
	uint8_t		ui8TempData[30], i;
	//
	// Wait for RDY to be set, this means that a communication window is 
	// available.  Then read the 'important' data from the IQS5xx (Address 
	// 0x000D to 0x0038).  This includes gesture status, system info flags and 
	// all XY data.  NOTE: if less multi-touches are enabled, then less XY
	// data can be read.  It would be good to configure the RDY pin as an
	// interrupt, and then trigger the i2c when the RDY interrupt sees a rising
	// edge.
	//
	RDY_wait();
	
	I2C_Read_16(IQS5xx_ADDR, GestureEvents0_adr, &Data_Buff[0], 44);

	if((Data_Buff[3] & SNAP_TOGGLE) != 0)
	{
		// If there was a change in a snap status, then read the snap status 
		// bytes additionally. Keep previous values to identify a state change
		//
		I2C_Read_16(IQS5xx_ADDR, SnapStatus_adr, &ui8TempData[0], 30);
		for(i = 0; i < 15; i++)
		{
			ui16PrevSnap[i] = ui16SnapStatus[i];
			ui16SnapStatus[i] = ((uint16_t)(ui8TempData[2*i])<<8) + 
								 (uint16_t)ui8TempData[(2*i)+1];
		}
	}
	//
	// Terminate the communication session, so that the IQS5xx can continue 
	// with sensing and processing
	//
	Close_Comms();
	//
	// Process received data 
	//
	Process_XY();
}



