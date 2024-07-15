#pragma once

#include <iostream>
#include <vector>

#include "SimpleGTA.h"

#include "IWindow.h"
#include "IDebug.h"

class IMenuVSL {
public:
    IDebug* debug = NULL;
    unsigned int deltaTime = 0;

    /*1.0.0*/
    virtual IWindow* AddWindow() = 0;
    virtual std::vector<IWindow*> GetWindows() = 0;

    virtual void DrawRect(CRect rect, CRGBA color) = 0;
    virtual void DrawRect(CVector2D position, CVector2D size, CRGBA color) = 0;
    virtual void DrawString(std::string text, CVector2D position, CRGBA color, eFontAlignment align) = 0;
    virtual void DrawRectWithString(std::string text, CVector2D pos, CVector2D boxSize, CRGBA boxColor, CRGBA textColor, eFontAlignment align) = 0;
    
    /*1.1.0*/
    virtual void DrawSprite(CSprite2d* sprite, CVector2D pos, CVector2D size) = 0;
    virtual void* LoadRwTextureFromFile(std::string file, std::string textureName, bool flipHorizontal = false);
    /*
    * Load RwTexture* from a file (tested with a .png)
    */
    virtual IWindow* AddWindowOptionsString(std::string title, IWindow* parent, std::string* pString, std::vector<std::string>* options) = 0;
    virtual IWindow* AddWindowMultiOptionsString(std::string title, IWindow* parent, std::vector<std::string>* selectedOptions, std::vector<std::string>* allOptions) = 0;
    virtual IWindow* AddColorWindow(IWindow* parent, CRGBA* color, std::function<void()> onValueChange) = 0;
};