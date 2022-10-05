//Author: Leo Yan
#include "SDCard.h"

SDCard::SDCard(int cspin){
    SD_CS_PIN = cspin;
}
bool SDCard::initialize(){
  SPIClass SDSPI(HSPI);
  SDSPI.begin(SD_sck, SD_miso, SD_mosi, -1);
  pinMode(SD_ss, OUTPUT);        //HSPI SS  set slave select pins as output

  if (!SD.begin(SD_ss, SDSPI)) {
    Serial1.println("Card Mount Failed");
    return false;
  }
  //------------------------------------

  File profile = SD.open("/foo.txt", FILE_READ);

  if (!profile) {
    Serial1.println("Opening file to read failed");
    return false;
  }

  Serial1.println("File Content:");

  while (profile.available()) {

    while (profile.available()) {
      Serial1.write(profile.read());
      delay(50);
    }
  }
  return true;
}



String SDCard::getPath(){
    return currentDirectory;
}
bool SDCard::makeDirectory(String dirName){
    String dir = currentDirectory;
    currentFile = SD.open(currentDirectory);
    if(currentDirectory == "/"){
        dir = dirName;
    }else{
        dir = currentDirectory + "/" + dirName;
    }
    Serial1.println("Trying to make a new directory at ");
    Serial1.print(dir);
    if(SD.mkdir(dir)){
        Serial1.println("...successful");
        return true;
    }else{
        Serial1.println("...failed");
        return false;
    }
}
bool SDCard::removeDirectory(String dirName){
    String dir = currentDirectory;
    currentFile = SD.open(currentDirectory);
    if(currentDirectory == "/"){
        dir = dirName;
    }else{
        dir = currentDirectory + "/" + dirName;
    }
    Serial1.println("Trying to remove a directory at ");
    Serial1.print(dir);
    if(SD.rmdir(dir)){
        Serial1.println("...successful");
        return true;
    }else{
        Serial1.println("...failed");
        return false;
    }
}
bool SDCard::deleteFile(String fileName){
    String dir = currentDirectory;
    currentFile = SD.open(currentDirectory);
    if(currentDirectory == "/"){
        dir = fileName;
    }else{
        dir = currentDirectory + "/" + fileName;
    }
    Serial1.println("SDCard: Trying to delete a file at ");
    Serial1.print(dir);
    if(SD.remove(dir)){
        Serial1.println("SDCard: delete successful");
        return true;
    }else{
        Serial1.println("SDCard: delete failed");
        return false;
    }
}
String SDCard::getFileName(File file){
    //char FILE_NAME[MAX_CHAR];    
    //file.getName(FILE_NAME, MAX_CHAR);          
    //return String(FILE_NAME);
    return file.name();   
}
String SDCard::ListDirectory() {
    String temp = "..\n";
    currentFile = SD.open(currentDirectory);
    while (true) {
        File entry =  currentFile.openNextFile();
        if (! entry) {
            // no more files
            //Serial1.print("No File to print");
            //temp += "..";
            break;
        }
        //Serial1.print(entry.name());
        temp += getFileName(entry);
        if (entry.isDirectory()) {
            //temp += "/";
            //Serial1.println("/");
            //printDirectory(entry, numTabs + 1);
        } else {
            // files have sizes, directories do not
            //Serial1.print("\t\t");
            //temp += "\t\t";
            //Serial1.println(entry.size(), DEC);
        }
        temp += "\n";
        entry.close();
    }
    return temp;
}
void SDCard::printDirectory() {
    currentFile = SD.open(currentDirectory);
    Serial1.println("SDCard: Printing Directory");
    while (true) {
        File entry =  currentFile.openNextFile();
        if (! entry) {
            // no more files
            Serial1.println("SDCard: No more Files to print");
            break;
        }
        //Serial1.print(entry.name());
        Serial1.print(getFileName(entry));
        
        if (entry.isDirectory()) {
            Serial1.println("/");
            //printDirectory(entry, numTabs + 1);
        } else {
            // files have sizes, directories do not
            //Serial1.print("\t\t");
            //Serial1.println(entry.size(), DEC);
        }
        entry.close();
    }
}
void SDCard::printDirectory(File dir, int numTabs) {
    while (true) {
        File entry =  dir.openNextFile();
        if (! entry) {
            // no more files
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++) {
            Serial1.print('\t');
        }
       // Serial1.print(entry.name());
        
        if (entry.isDirectory()) {
            Serial1.println("/");
            //printDirectory(entry, numTabs + 1);
        } else {
            // files have sizes, directories do not
            Serial1.print("\t\t");
            //Serial1.println(entry.size(), DEC);
        }
        entry.close();
    }
}
bool SDCard::enter(String dirName){
    String dir = currentDirectory;
    //Serial1.println(currentDirectory);
    if(currentDirectory == "/"){
        dir = currentDirectory + dirName;
    }else if(dirName == ".."){
        int count = 0;
        int stringMaxIndex = currentDirectory.length() - 1;
        for(int i = stringMaxIndex; i >= 0; i--){
            if(currentDirectory[i] == '/'){
                count++;
            }
            if(count >= 1){
                dir.remove(i, stringMaxIndex + 1 - i);
                if(dir == ""){
                    dir = "/";
                }
                break;
            }
        }
    }
    else{
        dir = currentDirectory + "/" + dirName;
    }
    //Serial1.print("Now trying to access directory: ");
    //Serial1.println(dir);
    currentFile = SD.open(dir);
    if(!currentFile){
        return false;
    }
    currentDirectory = dir;
    if(currentFile.isDirectory()){
        //printDirectory(currentFile, 0);
        ;
    }else{
        //Serial1.println(getFileName(currentFile));
        //Serial1.println(" Open Successful");
        ;
    }
    return true;//??
}
bool SDCard::isFile(){
    bool isDir = currentFile.isDirectory();
    return !isDir;
}
bool SDCard::isFile(String dirName){
    String dir = currentDirectory;
    if(currentDirectory == "/"){
        dir = currentDirectory + dirName;
    }else if(dirName == ".."){
        //This is obviously not a file
        return false;
    }
    else{
        dir = currentDirectory + "/" + dirName;
    }
    File tempFile = SD.open(dir);
    bool isDir = tempFile.isDirectory();
    tempFile.close();
    return !isDir;
}
String SDCard::readFile(String dirName, int startLine, int endLine){
    //startLine inclusive, endLine should be exclusive
    String results;
    if(startLine < 0 || endLine < 0){
        return "";
    }
    String dir = currentDirectory;
    if(currentDirectory == "/"){
        dir = currentDirectory + dirName;
    }else if(dirName == ".."){
        //This is obviously not a file
        return "";
    }
    else{
        dir = currentDirectory + "/" + dirName;
    }
    File tempFile = SD.open(dir);
    int lineNumber = 0;
    while(tempFile.available()){
        lineNumber++;
        if(lineNumber >= endLine){
            break;
        }
        if(results != ""){
            results += "\n";
        }
//        int n = tempFile.fgets(fileReadBuffer, sizeof(fileReadBuffer));
        int n = tempFile.readBytes(fileReadBuffer,sizeof(fileReadBuffer));
        if(lineNumber >= startLine){
            results += String(fileReadBuffer);
        }
    }
    return results;
}
