#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "SimpleGTA.h"
#include "MenuStructs.h"

#include "IWindow.h"
#include "IDebug.h"

#define _debug menuVSL->debug
#define GetLanguageLine menuVSL->GetLanguageLineFormatted

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

    /*
    * Load RwTexture* from a file (tested with a .png)
    */
    virtual void* LoadRwTextureFromFile(std::string file, std::string textureName, bool flipHorizontal = false);

    virtual IWindow* AddWindowOptionsString(std::string title, IWindow* parent, std::string* pString, std::vector<std::string>* options) = 0;
    virtual IWindow* AddWindowMultiOptionsString(std::string title, IWindow* parent, std::vector<std::string>* selectedOptions, std::vector<std::string>* allOptions) = 0;
    virtual IWindow* AddColorWindow(IWindow* parent, CRGBA* color, std::function<void()> onValueChange) = 0;

    /*1.2.0*/

    virtual void SetGlobalIntVariable(std::string key, int value) = 0;
    virtual int GetGlobalIntVariable(std::string key) = 0;

    virtual CVector2D ConvertWorldPositionToScreenPosition(CVector worldPosition) = 0;
    virtual void DrawWorldText(std::string text, CVector position, CRGBA color, eFontAlignment align) = 0;
    virtual void AddOnRender(std::function<void()> fn) = 0;
    virtual void SetFontScale(float sx, float sy) = 0;
    virtual CVector2D GetFontScale() = 0;
    virtual void ResetFontScale() = 0;

    /*1.3.0*/

    virtual void AddOnVehicleRenderBefore(std::function<void(void*)> fn) = 0;
    virtual void AddOnVehicleRenderAfter(std::function<void(void*)> fn) = 0;
    virtual void AddOnUpdate(std::function<void()> fn) = 0;
    virtual void AddOnProcessScripts(std::function<void()> fn) = 0;
    virtual std::vector<MVehicle> GetVehicles() = 0;

    virtual void DrawRectWithStringMultiline(std::string text, CVector2D pos, float width, CVector2D padding, CRGBA boxColor, CRGBA textColor, eFontAlignment align) = 0;
    virtual void ShowPopup(std::string title, std::string text, CVector2D pos, int time) = 0;

    // default value for drawWithFixedScale is true
    // using false is usefull with ConvertWorldPositionToScreenPosition
    virtual void SetDrawWithFixedScale(bool enabled) = 0;

    virtual IWindow* AddVector2WindowEx(IWindow* parent, CVector2D* vec, float min, float max, float addBy, std::function<void()> onChange, std::function<void()> onBack) = 0;
    virtual IWindow* AddVector2Window(IWindow* parent, CVector2D* vec, float min, float max, float addBy) = 0;
    
    virtual IWindow* AddVectorWindowEx(IWindow* parent, CVector* vec, float min, float max, float addBy, std::function<void()> onChange, std::function<void()> onBack) = 0;
    virtual IWindow* AddVectorWindow(IWindow* parent, CVector* vec, float min, float max, float addBy) = 0;

    /* 1.4.0 */

    virtual IWindow* ShowSelectLanguageWindow(IWindow* parent = NULL) = 0;
    virtual void LoadLanguagesFolder(std::string folder) = 0;

    virtual std::string GetLanguageLineFormatted(std::string key, ...) = 0;

    virtual void ShowMessage(std::string key, int time) = 0;

    virtual void AddModCredits(std::string key) = 0;
};