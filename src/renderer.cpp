//Author: Leo Yan
#include "renderer.h"
#include "parasetup.h"

Paint Renderer::newEmptyPaint(int height, int width){
    //Force height to be multiple of eight, named height instead of width since
    //the display will be in landscape as opposed to in default portrait mode
    uint8_t h = (height % 8) ? height : (height - height % 8);

    Paint paint(image, h, width);
    paint.SetRotate(ROTATE_270);
    paint.Clear(COLORED);
    paint.Clear(UNCOLORED);

    return paint;
}

Paint Renderer::infoBar(const bool & sdStatus, const String & currentPath){
    Paint paint = newEmptyPaint(12, 250);
    String sdText = sdStatus ? "SD" : "--";
    String shortenedPath = currentPath;
    if(currentPath.length() > 20){
        shortenedPath = ".." + currentPath.substring(currentPath.length() - 20);
    }
    paint.DrawStringAt(0, 0, sdText, &Font12, COLORED);
    paint.DrawStringAt(120, 0, shortenedPath, &Font12, COLORED);
    return paint;
}
Paint Renderer::quickMenu(String options, int selected){
    Paint paint = newEmptyPaint(48, 128);
    paint.DrawRectangle(0, 0, 128, 48, COLORED);
    
    const uint8_t fontSize = 8;

    String commands[MENU_SIZE];

    splitStringBy(options, commands, MENU_SIZE, '\n');

    for(uint8_t i = 0; i < MENU_SIZE; i++){
        paint.DrawStringAt(8, fontSize * i, commands[i], &Font8, COLORED);
        paint.DrawHorizontalLine(0, fontSize * i + 1, 128, COLORED);
    }
    paint.DrawStringAt(8, fontSize * selected, "-", &Font8, COLORED);

    return paint;
}

Paint Renderer::mainPage(){
    Paint paint = newEmptyPaint(112, 250);
    //TODO: I don't think this is necessary
    return paint;
}
Paint Renderer::settingsPage(){
    Paint paint = newEmptyPaint(112, 250);
    //TODO: I don't think this is necessary
    //Maybe choosing fonts?
    return paint;
}
Paint Renderer::filesPage(String directories, int selected){

    Paint paint = newEmptyPaint(112, 250);
    
    const uint8_t fontSize = 12;
    const uint8_t FILE_DISPLAY_COUNT = 9;
    const uint8_t maxChar = 20;

    String options[FILE_DISPLAY_COUNT];
    splitStringBy(directories, options, FILE_DISPLAY_COUNT, '\n');

    for(uint8_t i = 0; i < FILE_DISPLAY_COUNT; i++){
        if(options[i].length() >= 20){
            options[i] = options[i].substring(0, maxChar - 2) + "..";
        }
    }

    for(uint8_t i = 0; i < FILE_DISPLAY_COUNT; i++){
        paint.DrawStringAt(24, fontSize * i, options[i], &Font12, COLORED);
        paint.DrawHorizontalLine(0, fontSize * i + 1, 256, COLORED);
    }
    paint.DrawStringAt(8, 8 * selected, "-", &Font8, COLORED);

    return paint;
}
Paint Renderer::notesPage(String rawContent){
    Paint paint = newEmptyPaint(112, 250);

    const uint8_t maxLines = 9;
    const uint8_t maxChar = 21;
    const uint8_t fontSize = 12;

    uint8_t charCounter = 0;
    String content = "";

    for(uint8_t i = 0; i < rawContent.length(); i++){
        if(rawContent[i] == '\n'){
            charCounter = 0;
        }
        if(charCounter == maxChar){
            charCounter = 0;
            content += "\n";
        }
        charCounter += 1;
        if(i > fontSize * maxChar){
            break;
        }
    }

    content = content.substring(0, fontSize * maxChar);

    String toPrint[maxLines];
    splitStringBy(content, toPrint, maxLines, '\n');


    for(uint8_t i = 0; i < maxLines; i++){
        paint.DrawStringAt(0, fontSize * i, toPrint[i], &Font12, COLORED,POSITIVE);
    }

    
    return paint;
}

Paint Renderer::notesPage(String title, String content){
    Paint paint = newEmptyPaint(112, 250);
    //TODO: I don't think this is necessary
    return paint;
}

void Renderer::splitStringBy(String raw, String * parameters, int paraSize, char deliminater){
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
