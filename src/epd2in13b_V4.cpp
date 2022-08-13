/**
 *  @filename   :   epd2in13b_V4.cpp
 *  @brief      :   Implements for Three-color e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     April 25 2022
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "epd2in13b_V4.h"
#include "parasetup.h"
//#include <pgmspace.h>

Epd::~Epd() {
};

Epd::Epd() {
    sck_pin=SCK_PIN;
    mosi_pin = MOSI_PIN;
    reset_pin = RST_PIN;
    dc_pin = DC_PIN;
    cs_pin = CS_PIN;
    busy_pin = BUSY_PIN;
    width = EPD_WIDTH;
    bufwidth = 128/8;
    height = EPD_HEIGHT;
    bufheight = 63;
};

void Epd::Lut(const unsigned char *lut)
{
    unsigned char count;
    SendCommand(WRITE_LUT_REGISTER);
    for(count = 0; count < 153; count++) {
      SendData(lut[count]);
    }
  
    SendCommand(0x3f);
    SendData(*(lut+153));
    SendCommand(0x03);  // gate voltage
    SendData(*(lut+154));
    SendCommand(0x04);  // source voltage
    SendData(*(lut+155)); // VSH
    SendData(*(lut+156)); // VSH2
    SendData(*(lut+157)); // VSL
    SendCommand(0x2c);    // VCOM
    SendData(*(lut+158));
}


int Epd::Init(){
	return Init(FULL);
}

int Epd::Init(char Mode) {
	/* this calls the peripheral hardware interface, see epdif */
	if (IfInit() != 0) {
		return -1;
	}
	
	Reset();
	
	//int count;
	if(Mode == FULL) {
		WaitUntilIdle();
		SendCommand(0x12); // soft reset
		WaitUntilIdle();

		SendCommand(0x01); //Driver output control
		SendData(0xF9);
		SendData(0x00);
		SendData(0x00);

		SendCommand(0x11); //data entry mode
		SendData(0x03);

		SetWindows(0, 0, width-1, height-1);
		SetCursor(0, 0);
		
		SendCommand(0x3C); //BorderWavefrom
		SendData(0x05);
		
		SendCommand(0x21); //  Display update control
		SendData(0x00);
		SendData(0x80);
		
		SendCommand(0x18); //Read built-in temperature sensor
		SendData(0x80);
		
		WaitUntilIdle();
		
		Lut(lut_full_update);
		} else if(Mode == PART) {
		
		
		DigitalWrite(reset_pin, LOW);                //module reset
		DelayMs(1);
		DigitalWrite(reset_pin, HIGH);

		Lut(lut_partial_update);
		
		SendCommand(0x37);
		SendData(0x00);
		SendData(0x00);
		SendData(0x00);
		SendData(0x00);
		SendData(0x00);
		SendData(0x40);
		SendData(0x00);
		SendData(0x00);
		SendData(0x00);
		SendData(0x00);

		SendCommand(0x3C); //BorderWavefrom
		SendData(0x80);
		
		SendCommand(0x22); //Display Update Sequence Option
		SendData(0xC0);    // Enable clock and  Enable analog
		SendCommand(0x20);  //Activate Display Update Sequence
		WaitUntilIdle();
		
		SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
		SetCursor(0, 0);
		} else {
		return -1;
	}

	return 0;
}

void Epd::WriteByte(UBYTE data)
{
    //SPI.beginTransaction(spi_settings);
    digitalWrite(cs_pin, LOW);

    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0) digitalWrite(MOSI_PIN, LOW); 
        else                    digitalWrite(MOSI_PIN, HIGH);

        data <<= 1;
        digitalWrite(sck_pin, HIGH);     
        digitalWrite(sck_pin, LOW);
    }

    //SPI.transfer(data);
    digitalWrite(cs_pin, HIGH);
    //SPI.endTransaction();	
}
/**
 *  @brief: basic function for sending commands
 */
void Epd::SendCommand(unsigned char command) {
    DigitalWrite(dc_pin, LOW);
    DigitalWrite(cs_pin, LOW);
    WriteByte(command);     
    DigitalWrite(cs_pin, HIGH);   
    //DigitalWrite(dc_pin, LOW);
    //SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void Epd::SendData(unsigned char data) {
    DigitalWrite(dc_pin,HIGH);
    DigitalWrite(cs_pin,LOW);
    WriteByte(data);
    DigitalWrite(cs_pin,HIGH);
    //DigitalWrite(dc_pin, HIGH);
    //SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void Epd::WaitUntilIdle(void) {
    while(DigitalRead(busy_pin) == 1) {      //1: busy, 0: idle
        DelayMs(10);
    }
    DelayMs(10);
}

/**
 *  @brief: module reset. 
 *          often used to awaken the module in deep sleep, 
 *          see Epd::Sleep();
 */
void Epd::Reset(void) {
    DigitalWrite(reset_pin, HIGH);
    DelayMs(20);  
    DigitalWrite(reset_pin, LOW);
    DelayMs(2);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(20); 
    //this->count = 0;    
}


// Setting the display window
void Epd::SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend) {
    SendCommand(SET_RAM_X_ADDRESS_START_END_POSITION); // SET_RAM_X_ADDRESS_START_END_POSITION
    SendData((Xstart>>3) & 0xFF);
    SendData((Xend>>3) & 0xFF);
	
    SendCommand(SET_RAM_Y_ADDRESS_START_END_POSITION); // SET_RAM_Y_ADDRESS_START_END_POSITION
    SendData(Ystart & 0xFF);
    SendData((Ystart >> 8) & 0xFF);
    SendData(Yend & 0xFF);
    SendData((Yend >> 8) & 0xFF);
}

// Set Cursor
void Epd::SetCursor(UWORD Xstart, UWORD Ystart) {
    SendCommand(SET_RAM_X_ADDRESS_COUNTER); // SET_RAM_X_ADDRESS_COUNTER
    SendData(Xstart & 0xFF);

    SendCommand(SET_RAM_Y_ADDRESS_COUNTER); // SET_RAM_Y_ADDRESS_COUNTER
    SendData(Ystart & 0xFF);
    SendData((Ystart >> 8) & 0xFF);
}

/**
 * @brief: refresh and displays the frame
 */

void Epd::Refresh(unsigned char partorfull){
   	SendCommand(DISPLAY_UPDATE_CONTROL_2);
   	SendData(partorfull);
   	SendCommand(MASTER_ACTIVATION);
   	WaitUntilIdle();
}


/**
 * @brief: clear the frame data from the SRAM, this won't refresh the display
 */
void Epd::ClearFrame(void) {
    SendCommand(WRITE_RAM);           
    for(int i = 0; i < bufwidth * height; i++) {
        SendData(WHITE);  
    }  
    Refresh(0xf7);
    //this->count = 0;
}

/**
 * @brief: This displays the frame data from SRAM
 */
//void Epd::DisplayFrame(void) {
//    Refresh(0xf7);  // Activate Display Update Sequence
//}
/**
 *  @brief: put an image buffer to the frame memory.
 *          this won't update the display.
 */
/*
void Epd::DisplayFrame(
    const unsigned char* image_buffer,
    int x,
    int y,
    int image_width,
    int image_height
) {
    int x_start = x;
    int x_end = x + image_width - 1;
    int y_end = y + image_height - 1;

    if (
        image_buffer == NULL ||
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
    ) {
        return;
    }
    // x point must be the multiple of 8 or the last 3 bits will be ignored 
    
    //x &= 0xF8;
    //image_width &= 0xF8;
    if (x + image_width >= this->width) {
        x_end = this->width - 1;
    } else {
        x_end = x + image_width - 1;
    }
    if (y + image_height >= this->height) {
        y_end = this->height - 1;
    } else {
        y_end = y + image_height - 1;
    }
    SetWindows(x_start, y, x_end, y_end);
    // set the frame memory line by line 
    for (int j = y; j <= y_end; j++) {
        //SetMemoryPointer(x, j);
        SetCursor(x_start, j);
        SendCommand(WRITE_RAM);
        for (int i = x / 8; i <= x_end / 8; i++) {
            SendData(image_buffer[(i - x / 8) + (j - y) * (image_width / 8)]);
        }
    }
}
*/
/**
 * @brief: After this command is transmitted, the chip would enter the deep-sleep mode to save power. 
 *         The deep sleep mode would return to standby by hardware reset. The only one parameter is a 
 *         check code, the command would be executed if check code = 0xA5. 
 *         You can use Epd::Reset() to awaken and use Epd::Init() to initialize.
 */
void Epd::Sleep() {
    SendCommand(DEEP_SLEEP_MODE);  //enter deep sleep
    SendData(0x01);
    DelayMs(100);   
    //this->count = 0;
}

void Epd::UpdateDisplay(unsigned char* frame_buffer)
{
	int w, h;
	w = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
	h = EPD_HEIGHT;
	SendCommand(WRITE_RAM);	
	for(int i = 0; i <4000; i++)
		{SendData(frame_buffer[i]);		
		}	
	SendCommand(DISPLAY_UPDATE_CONTROL_2);
   	SendData(0x0f);
   	SendCommand(MASTER_ACTIVATION);
	for(int i = 0; i <4000; i++)
		{frame_buffer[i]=WHITE;};				   
   	WaitUntilIdle();
	//Refresh(0x0f);	//DISPLAY REFRESH	
}

/* END OF FILE */
