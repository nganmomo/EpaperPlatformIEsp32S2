//Author: Leo Yan
#ifndef RENDERER_CTRL
#define RENDERER_CTRL

#include "epdpaint.h"
#include <Arduino.h>
//#include "utility.h"

#define COLORED     0
#define UNCOLORED   1
#define MENU_SIZE	5

class Renderer{
public:
    //Variables
    enum currentPage {main = 0, settings = 1, files = 2, note = 3};
    //Functions
    Renderer(){};
    Paint newEmptyPaint(int width, int height);
    Paint infoBar(const bool & sdStatus, const String & currentPath);
    Paint quickMenu(String options, int selected);
    //Paint MainContent(int selectedPage);
    Paint mainPage();
    Paint settingsPage();
    Paint filesPage(String directories, int selected);
    Paint notesPage(String content);
    Paint notesPage(String title, String content);
    void splitStringBy(String raw, String * parameters, int paraSize, char deliminater);
    
    unsigned char image[4000/4 + 50];
private:
    String allMenuOptions = "";
};

#endif //RENDERER_CTRL
