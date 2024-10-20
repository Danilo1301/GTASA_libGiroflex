#pragma once

#include <iostream>

#include "SimpleGTA.h"

#include "IItem.h"

class IPopup {
public:
    CVector2D m_Position = CVector2D(400, 200);
    std::vector<IItem*> m_Items;
    std::string m_Title = "Popup";
    std::string m_Text = "Text";
};