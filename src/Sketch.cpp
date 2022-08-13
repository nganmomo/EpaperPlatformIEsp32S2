#include <Arduino.h>
//			            _____[]_____
//			       VIN|     		 |GND
//		          PA03|  		     |RST
//		           GND|	    	  D0 |PA11 D3
//	               3V3|		      D1 |PA10 D2
//	A1      D16   PB02|		      D2 |PA08 D11  MOSI
//	A4	    D19   PA05|		      D3 |PA09 D12  SCK
//	A3	    D18   PA04|		      D4 |PA14 D28  *CS
//	Key1    D33   PB09|		      D5 |PA15 D29  *DC
//	Key2    D32   PB08|		      D6 |PA20 D6	  *RES
//	key3    D15   PA02|		      D7 |PA21 D7   *BUSY
//SCL *SCK	D9    PA17|		         |PA06 D20  CS2
//	  MISO	D10   PA19|		         |PA07 D21
//SDA *MOSI	D8    PA16|		         |PA23 D1
//      	D24   PA18|		         |PA20 D0
//	        	      |__MRK PINOUT__|

#include "parasetup.h"
#include <SPI.h>
#include "epd2in13b_V4.h"
#include "imagedata.h"
#include "epdpaint.h"
#include "SDCard.h"
#include "renderer.h"	

unsigned char image[4000];
//This is contained in rendererg

char linetext[8][22];

//Beginning of Auto generated function prototypes by Atmel Studio
void Draw(Paint paint, int x, int y);
void Draw(Paint paint, int x, int y, int cornor);
bool hasAnyInput();
bool ReadPin(int toRead);
void ReadPins();
void test(void);
//End of Auto generated function prototypes by Atmel Studio

//Defined Variables
Epd epd;        //Epaper
SDCard sdCard;  //Container for SD card File management
//Paint paint(image, 128, 63);
Paint paint(image, 122, 250);
Renderer menu;  //Container for Paint epdPaint
bool sdInitialized = false;
//End of variables


//UI Drawing
void DrawInfoBar(const bool & sdStatus, const String & currentPath){
    //Paint paint = newEmptyPaint(12, 250);
    String sdText = sdStatus ? "SD" : "--";
    String shortenedPath = currentPath;
    if(currentPath.length() > 27){
        shortenedPath = ".." + currentPath.substring(currentPath.length() - 27 - 2);
    }
    paint.DrawStringAt(0, 0, sdText, &Font12, COLORED);
    paint.DrawStringAt(50, 0, shortenedPath, &Font12, COLORED);
    //return paint;
}

void DrawQuickMenu(const String & options, const int & selected){
    int x = 20, y = 50;
    int w = 70, h = 50;
    const uint8_t fontSize = 12;
    h = (fontSize + 2) * MENU_SIZE;

    paint.DrawFilledRectangle(x, y, x + w, y + h, UNCOLORED);
    paint.DrawRectangle(x, y, x + w, y + h, COLORED);
    String commands[MENU_SIZE];

    SplitStringBy(options, commands, MENU_SIZE, '\n');

    for(uint8_t i = 0; i < MENU_SIZE; i++){
        paint.DrawStringAt(x + 8, y + (fontSize + 2) * i + 2, commands[i], &Font12, COLORED);
        paint.DrawHorizontalLine(x, y + (fontSize + 2) * i, w, COLORED);
    }
    if(selected != -1){
        paint.DrawStringAt(x + 2, y + (fontSize + 2) * selected + 2, "->", &Font12, COLORED);
    }
    //return paint;
}

int CountRenderLines(const String & rawContent, const uint8_t maxChar = 35){
    uint8_t charCounter = 0;
    int lineCount = rawContent == "" ? 0 : 1;
    String content = "";

    for(uint8_t i = 0; i < rawContent.length(); i++){
        content += rawContent[i];
        if(charCounter == 0 && rawContent[i] == ' '){
            //Skip this
        }
        if(rawContent[i] == '\n'){
            charCounter = 0;
            lineCount++;
        }
        if(charCounter == maxChar){
            charCounter = 0;
            content += "\n";
            lineCount++;
        }
        charCounter += 1;
    }
    return lineCount;
}

void DrawNotesPage(const String & rawContent){
    int x = 10, y = 12;
    Serial.println("Trying to draw notes page with");
    Serial.println(rawContent);
    const uint8_t maxLines = 8;
    const uint8_t maxChar = 35;
    const uint8_t fontSize = 12;

    uint8_t charCounter = 0;
    String content = "";

    for(uint8_t i = 0; i < rawContent.length(); i++){
        content += rawContent[i];
        if(charCounter == 0 && rawContent[i] == ' '){
            //Skip this
        }
        if(rawContent[i] == '\n'){
            charCounter = 0;
        }
        if(charCounter == maxChar){
            charCounter = 0;
            content += "\n";
        }
        charCounter += 1;
        if(i > maxLines * maxChar){
            break;
        }
    }

    content = content.substring(0, maxLines * maxChar);
    String toPrint[maxLines];
    SplitStringBy(content, toPrint, maxLines, '\n');


    for(uint8_t i = 0; i < maxLines; i++){
        toPrint[i].trim();
        paint.DrawStringAt(x, y + (fontSize + 2) * i + 1, toPrint[i], &Font12, COLORED,POSITIVE);
    }
    
}

String DrawFilesPage(String directories, int & selected, int & viewPortStart){
    int x = 0, y = 20;
    
    //Serial.println("Page: Drawing Files page with");
    const uint8_t fontSize = 12;
    const uint8_t FILE_DISPLAY_COUNT = 9;
    const uint8_t FILE_MAX_COUNT = 100;
    const uint8_t maxChar = 20;

    int onScreenSelect = 0;
    int optionCount = 0;
    String options[FILE_DISPLAY_COUNT];
    String optionDisplays[FILE_MAX_COUNT];
    String current = "";
    SplitStringBy(directories, optionDisplays, FILE_DISPLAY_COUNT, '\n');

    for(int i = 0; i < FILE_MAX_COUNT; i++){
        //Loop all of these, take note of position of last non-empty
        if(optionDisplays[i] != ""){
            optionCount = i + 1;
        }
    }

    //Clamping selection in range
    if(selected >= optionCount){
        selected = 0;
    }else if(selected < 0){
        selected = optionCount - 1;
    }
    
    if(optionCount > FILE_DISPLAY_COUNT){
        //If outside range...
        if(selected < viewPortStart){
            //Smaller than range, move window
            viewPortStart = selected;
        }
        else if(selected <= viewPortStart + FILE_DISPLAY_COUNT){
            //Larger than range, move window
            viewPortStart = selected - (FILE_DISPLAY_COUNT - 1);
        }
    }
    else{
        //Force to start
        viewPortStart = 0;
    }
    //Offset selection
    onScreenSelect = selected - viewPortStart;

    for(uint8_t i = 0; i < FILE_DISPLAY_COUNT; i++){
        //show window
        options[i] = optionDisplays[viewPortStart + i];
        options[i].trim();
        
        Serial.print("file ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(options[i]);
        
        if(options[i].length() >= maxChar){
            options[i] = options[i].substring(0, maxChar - 2) + "..";
        }
    }

    for(uint8_t i = 0; i < FILE_DISPLAY_COUNT; i++){
        paint.DrawStringAt(x + 24, y + (fontSize + 2) * i + 2, options[i], &Font12, COLORED);
        //paint.DrawHorizontalLine(x + 12, y + (fontSize + 2) * (i + 1), 250, COLORED);
    }
    if(onScreenSelect != -1){
        paint.DrawStringAt(x + 8, y + (fontSize + 2) * onScreenSelect + 2, "->", &Font12, COLORED);
        current = options[onScreenSelect];
    }
    Serial.println("Page: End of Files");
    return current;
}

bool SplitStringBy(String raw, String * parameters, int paraSize, char deliminater){
    uint8_t lastIndex = 0;
    uint8_t count = 0;
    if(raw[raw.length()-1] != deliminater){
        raw += deliminater;
    }
    for(uint8_t i = 0; i < raw.length(); i++){
        if(raw[i] == deliminater){
            parameters[count++] = raw.substring(lastIndex, i);
            if(++i >= raw.length()){
                break;
            }
            lastIndex = i;
            if(count >= paraSize){
                break;
            }
        }
    }
}

void Draw(Paint paint, int x, int y){
    //need to translate x and y since they are not at the correct orientation;
    int x_translated = y;// - paint.GetHeight();
    int y_translated = 250 - x - paint.GetHeight();
    //epd.DisplayFrame(paint.GetImage(), x_translated, y_translated, paint.GetWidth(), paint.GetHeight());
}

void Draw(Paint paint, int x, int y, int cornor){//buttom left, top left, top right, bottom right
    //Default cornor, top left
    int x_translated = x;
    int y_translated = y;

    if(cornor == 1){//top right
        x_translated = x - paint.GetHeight();
    }else if(cornor == 2){//bottom right
        x_translated = x - paint.GetHeight();
        y_translated = y - paint.GetWidth();
    }else if(cornor == 3){//bottom left
        y_translated = y - paint.GetWidth();
    }
    Draw(paint, x_translated, y_translated);
}

bool hasAnyInput(){
    return (ReadPin(KEY1) || ReadPin(KEY2) || ReadPin(KEY3));
}

bool ReadPin(int toRead){//A0, A1, or A2
    if(digitalRead(toRead) == LOW){
        return digitalRead(toRead) == LOW;
    }
    return false;
}


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Initialization: ");
    pinMode(KEY1, INPUT);
    pinMode(KEY2, INPUT);
    pinMode(KEY3, INPUT);
    if (epd.Init(FULL) != 0) {
        Serial.print("e-Paper init failed, ");
        return;
    }
    paint.SetRotate(ROTATE_270);
    Serial.print("e-Paper starting, ");
    delay(500);
    //epd.ClearFrame();
    for(int i = 0; i <4000; i++)  {image[i]=WHITE;}    
    
    for(int i = 0; i < 3; i ++){
        //Initialization is tried for three time
        if(sdCard.initialize()){
            sdInitialized = true;
            Serial.println("SD Initialization Successful");
            break;
        }
        //wait for a few seconds before retrying
        delay(500 * i);
    }
    //String infoBar = sdInitialized ? "SD " + sdCard.getPath() : "";
    //char text[64];
    //infoBar.toCharArray(text, 64);
    
    epd.Init(PART);  
    DrawInfoBar(sdInitialized, sdCard.getPath());
    //DrawQuickMenu("Option1\nOption2\nOption3\nOption4\nOption5", 3);
    //DrawNotesPage("This is some really really long notes that I used to fill up the content and possibly for the users to read");
    DrawNotesPage("\nInitialization Complete, press Any Key");
    //DrawFilesPage(sdCard.ListDirectory(), 1);
    
    epd.UpdateDisplay(image);
}

bool isMenuOpen = false;
String currentPageName = "files";
String selectedText = "";
int selectedIndex = 0;
int viewPortIndex = 0;
int fileLine = 0;

void loop() {
    if(!hasAnyInput()){
        return;
        //We do not do anything unless there's user input
    }
    bool pin3 = ReadPin(KEY1/*Right*/);
    bool pin1 = ReadPin(KEY2/*left*/);
    bool pin2 = ReadPin(KEY3/*Middle*/);
    if(pin1){
        Serial.println("Pin Pressed: left");//Actually Middle
    }
    if(pin2){
        Serial.println("Pin Pressed: middle");//Actually right
    }
    if(pin3){
        Serial.println("Pin Pressed: right");//Actually Left
    }
    
    epd.Init(PART); 

    /*
    if(pin2 && !isMenuOpen){
        isMenuOpen = true;
        selectedIndex = 0;
    }*/
    Serial.println("Input Actions");
    
    //When menu is not open
    //First, draw an empty object
    //DrawPage(currentPageName, -1);
    if(currentPageName == "files"){
        isMenuOpen = false;
        if(pin1){
            selectedIndex = (selectedIndex - 1);
        }
        if(pin2){
            sdCard.enter(selectedText);
            if(sdCard.isFile()){
                currentPageName = "notes";
                return;
            }
            selectedIndex = 0;
            viewPortIndex = 0;
        }
        if(pin3){
            selectedIndex = (selectedIndex + 1);
        }
        //sdCard.printDirectory();
        selectedText = DrawFilesPage(sdCard.ListDirectory(), selectedIndex, viewPortIndex);
    }
    if(currentPageName == "notes" && !isMenuOpen){
        if(pin1){
            selectedIndex = (selectedIndex - 1);
        }
        if(pin2){
            isMenuOpen = true;
        }
        if(pin3){
            selectedIndex = (selectedIndex + 1);
        }
    }
    //When menu is open
    //TODO: When menu is triggered, selectedIndex and viewPortIndex should be reset
    if(isMenuOpen){
        if(pin1){
            selectedIndex = (selectedIndex + 4) % 5;
        }
        if(pin2){
            switch(selectedIndex)
            {
                case 0:
                    currentPageName = "files";
                    break;
                case 1:
                    currentPageName = "Menu Item 1";
                    break;
                case 2:
                    currentPageName = "Menu Item 2";
                    break;
                case 3:
                    currentPageName = "Menu Item 3";
                    break;
                case 4:
                    //Menu Item 4, closing menu;
                    isMenuOpen = false;
                    break;
                default:
                    currentPageName = "files";
                    break;
            }
        }
        if(pin3){
            selectedIndex = (selectedIndex + 6) % 5;
        }
        DrawQuickMenu("Files\nNothing\nNothing\nNothing\nReturn", selectedIndex);
    }
    
    DrawInfoBar(sdInitialized, sdCard.getPath());
    epd.UpdateDisplay(image);
    delay(100);
}
