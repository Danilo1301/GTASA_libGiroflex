#pragma once

#include <iostream>

#include "MenuStructs.h"

#include "IItem.h"

class IWindow {
public:
    int m_Id = -1;
    CVector2D m_Position = CVector2D(400, 400);
    std::vector<IItem*> m_Items;
    std::string m_Title = "Window";
    float m_Width = 500.0f;
    CRGBA m_TitleBackgroundColor = CRGBA(0, 100, 200);
	CRGBA m_ItemBackgroundColor = CRGBA(0, 0, 0, 100);

    virtual void SetPosition(CVector2D position) = 0;
    virtual IItem* AddText(std::string text, CRGBA color) = 0;
    virtual IItem* AddButton(std::string text, CRGBA color) = 0;
    virtual IItem* AddCheckbox(std::string text, bool* value) = 0;
    virtual IItem* AddIntRange(std::string text, int* value, int min, int max, int addBy) = 0;
    virtual IItem* AddFloatRange(std::string text, float* value, float min, float max, float addBy) = 0;
    virtual void SetToBeRemoved() = 0;
};