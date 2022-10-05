//Author: Leo Yan

#ifndef SD_CARD
#define SD_CARD


#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "parasetup.h"
//#include "Arduino_AVRSTL.h"

class SDCard{
public:
    SDCard(){};
    SDCard(int cspin);
    bool initialize();
    String getPath();
    bool makeDirectory(String dirName);
    bool removeDirectory(String dirName);
    bool deleteFile(String fileName);
    String getFileName(File file);
    String ListDirectory();
    void printDirectory();
    void printDirectory(File dir, int numTabs);
    bool enter(String dirName);
    bool isFile();
    bool isFile(String dirName);
    String readFile(String dirName, int startLine, int endLine);
private:
    //SdFat SD;
    int SD_CS_PIN = SD_ss;
    int MAX_CHAR = 25;
    char fileReadBuffer[50];
    File currentFile;
    String currentDirectory = "/";
    String directories;
};
#endif
