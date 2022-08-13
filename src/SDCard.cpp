//Author: Leo Yan
#include "SDCard.h"

SDCard::SDCard(int cspin){
    SD_CS_PIN = cspin;
}
bool SDCard::initialize(){
    if(!SD.begin(SD_CS_PIN)){
        return false;
    }else{
        currentFile = SD.open("/");
        return true;
    }
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
    Serial.println("Trying to make a new directory at ");
    Serial.print(dir);
    if(SD.mkdir(dir)){
        Serial.println("...successful");
        return true;
    }else{
        Serial.println("...failed");
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
    Serial.println("Trying to remove a directory at ");
    Serial.print(dir);
    if(SD.rmdir(dir)){
        Serial.println("...successful");
        return true;
    }else{
        Serial.println("...failed");
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
    Serial.println("SDCard: Trying to delete a file at ");
    Serial.print(dir);
    if(SD.remove(dir)){
        Serial.println("SDCard: delete successful");
        return true;
    }else{
        Serial.println("SDCard: delete failed");
        return false;
    }
}
String SDCard::getFileName(File file){
    char FILE_NAME[MAX_CHAR];
    file.getName(FILE_NAME, MAX_CHAR);
    return String(FILE_NAME);
}
String SDCard::ListDirectory() {
    String temp = "..\n";
    currentFile = SD.open(currentDirectory);
    while (true) {
        File entry =  currentFile.openNextFile();
        if (! entry) {
            // no more files
            //Serial.print("No File to print");
            //temp += "..";
            break;
        }
        //Serial.print(entry.name());
        temp += getFileName(entry);
        if (entry.isDirectory()) {
            //temp += "/";
            //Serial.println("/");
            //printDirectory(entry, numTabs + 1);
        } else {
            // files have sizes, directories do not
            //Serial.print("\t\t");
            //temp += "\t\t";
            //Serial.println(entry.size(), DEC);
        }
        temp += "\n";
        entry.close();
    }
    return temp;
}
void SDCard::printDirectory() {
    currentFile = SD.open(currentDirectory);
    Serial.println("SDCard: Printing Directory");
    while (true) {
        File entry =  currentFile.openNextFile();
        if (! entry) {
            // no more files
            Serial.println("SDCard: No more Files to print");
            break;
        }
        //Serial.print(entry.name());
        Serial.print(getFileName(entry));
        
        if (entry.isDirectory()) {
            Serial.println("/");
            //printDirectory(entry, numTabs + 1);
        } else {
            // files have sizes, directories do not
            //Serial.print("\t\t");
            //Serial.println(entry.size(), DEC);
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
            Serial.print('\t');
        }
       // Serial.print(entry.name());
        
        if (entry.isDirectory()) {
            Serial.println("/");
            //printDirectory(entry, numTabs + 1);
        } else {
            // files have sizes, directories do not
            Serial.print("\t\t");
            //Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}
bool SDCard::enter(String dirName){
    String dir = currentDirectory;
    //Serial.println(currentDirectory);
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
    //Serial.print("Now trying to access directory: ");
    //Serial.println(dir);
    currentFile = SD.open(dir);
    if(!currentFile){
        return false;
    }
    currentDirectory = dir;
    if(currentFile.isDirectory()){
        //printDirectory(currentFile, 0);
        ;
    }else{
        //Serial.println(getFileName(currentFile));
        //Serial.println(" Open Successful");
        ;
    }
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
        int n = tempFile.fgets(fileReadBuffer, sizeof(fileReadBuffer));
        if(lineNumber >= startLine){
            results += String(fileReadBuffer);
        }
    }
    return results;
}
