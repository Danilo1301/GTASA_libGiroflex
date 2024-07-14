#pragma once

#include <iostream>
#include <vector>

#include "IWindow.h"
#include "IDebug.h"

class IMenuVSL {
public:
    IDebug* debug = NULL;
    unsigned int deltaTime = 0;

    virtual IWindow* AddWindow() = 0;
    virtual std::vector<IWindow*> GetWindows() = 0;

    virtual void DrawRect(CRect rect, CRGBA color) = 0;
    virtual void DrawRect(CVector2D position, CVector2D size, CRGBA color) = 0;
    virtual void DrawString(std::string text, CVector2D position, CRGBA color, eFontAlignment align) = 0;
    virtual void DrawRectWithString(std::string text, CVector2D pos, CVector2D boxSize, CRGBA boxColor, CRGBA textColor, eFontAlignment align) = 0;
};