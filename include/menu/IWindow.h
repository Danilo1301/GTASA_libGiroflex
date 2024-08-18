#pragma once

#include <iostream>

#include "SimpleGTA.h"

#include "IItem.h"

class IWindow {
public:
    int m_Id = -1;
    CVector2D m_Position = CVector2D(400, 200);
    std::vector<IItem*> m_Items;
    std::string m_Title = "Window";
    float m_Width = 500.0f;
    CRGBA m_TitleBackgroundColor = CRGBA(0, 100, 200);
	CRGBA m_ItemBackgroundColor = CRGBA(0, 0, 0, 100);
    int m_Page = 0;
    int m_MaxItemsPerPage = 7;
    bool m_ShowBackButton = false;
    IWindow* m_Parent = NULL;
    std::function<void()> m_OnCloseWindow;

    /*1.0.0*/
    virtual void SetPosition(CVector2D position) = 0;
    virtual IItem* AddText(std::string text, CRGBA color) = 0;
    virtual IItem* AddButton(std::string text, CRGBA color) = 0;
    virtual IItem* AddCheckbox(std::string text, bool* value) = 0;
    virtual IItem* AddIntRange(std::string text, int* value, int min, int max, int addBy) = 0;
    virtual IItem* AddFloatRange(std::string text, float* value, float min, float max, float addBy) = 0;
    virtual void SetToBeRemoved() = 0;

    /*1.1.0*/
    virtual bool GetIsActive() = 0;
    virtual IItem* AddOptions(std::string text) = 0;
};