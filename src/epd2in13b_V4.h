//			           _____[]_____
//			       VIN| 		   |GND
//		          PA03|  		   |RST
//		           GND|	    	D0 |PA11 D3
//	               3V3|		    D1 |PA10 D2
//	A1      D16   PB02|		    D2 |PA08 D11  MOSI
//	A4	    D19   PA05|		    D3 |PA09 D12  SCK
//	A3	    D18   PA04|		    D4 |PA14 D28  *CS
//	Key1    D33   PB09|		    D5 |PA15 D29  *DC
//	Key2    D32   PB08|		    D6 |PA20 D6	  *RST
//	key3    D15   PA02|		    D7 |PA21 D7   *BUSY
//SCL *SCK	D9    PA17|		       |PA06 D20  *CS2
//	  MISO	D10   PA19|		       |PA07 D21
//SDA *MOSI	D8    PA16|		       |PA23 D1
//   		D24   PA18|		       |PA20 D0
//	    		      |_MRK PINOUT_|
//SERcom1 for debug
//			        _____[]_____
//KEY1  DAC  A0  D0|            |5V
//KEY2	     A1  D1|  		    |GND
//KEY3	     A2  D2|    	    |3V3
//*CS	     A3  D3|	        |D10 A10  *MOSI
//*CS2  SDA  A4  D4|	        |D9  A9   *MISO
//*BUSY SCL  A5	 D5|	        |D8  A8   *SCK
//*RST  TX   D18 D6|____XIAO____|D7  A7   RX     *DC
//
//See parasetup.h for pin assign //
//SERcom1 for debug
#ifndef EPD2IN13B_V4_H
#define EPD2IN13B_V4_H

#include "epdif.h"

#define FULL			0
#define PART			1

// Display resolution
#define EPD_WIDTH       122
#define EPD_HEIGHT      250

#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define READ_TEMPERATURE_SENSOR						0x18
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF


#define COMMAND 0xCF

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

#define FULL      0
#define PART      1

class Epd : EpdIf {
public:
    int width;
    int bufwidth;
    int height;
    int bufheight;
    int count;
    
    Epd();
    ~Epd();
    int Init(void);  
	int Init(char Mode);  
    void SendCommand(unsigned char command);
    void SendData(unsigned char data);
    void WriteByte(unsigned char data);
    void WaitUntilIdle(void);
    void Lut(const unsigned char *lut);
    void Reset(void);
    void SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);
    void SetCursor(UWORD Xstart, UWORD Ystart);
    //void DisplayFrame(const unsigned char* frame_buffer_black, const unsigned char* frame_buffer_red);
    //void DisplayFrame(void);
    void Display(const unsigned char* frame_buffer) ;
    void ClearFrame(void);
    void Sleep(void);
	void Refresh(unsigned char partorfull);
	void UpdateDisplay(unsigned char* frame_buffer);
	
    void DisplayFrame(
        const unsigned char* image_buffer,
        int x,
        int y,
        int image_width,
        int image_height
    );//Author: Leo Yan

private:
    unsigned int sck_pin;
    unsigned int mosi_pin;
    unsigned int reset_pin;
    unsigned int dc_pin;
    unsigned int cs_pin;
    unsigned int busy_pin;
};

#endif

/* END OF FILE */
