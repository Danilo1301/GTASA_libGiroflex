#pragma once

#include <iostream>
#include <functional>

#include "MenuStructs.h"

enum eItemType {
    ITEM_NONE,
	ITEM_TEXT,
	ITEM_BUTTON,
	ITEM_OPTIONS,
	ITEM_INT_RANGE,
	ITEM_FLOAT_RANGE,
	ITEM_CHECKBOX
};

class IItem {
public:
	int m_Id = -1;
    CVector2D m_Position = CVector2D(0, 0);
    CVector2D m_Size = CVector2D(400, 50);
    eItemType m_Type = eItemType::ITEM_NONE;
    std::string m_Text = "Item";
	CRGBA m_TextColor = CRGBA(255, 255, 255);
	CRGBA m_BackgroundColor = CRGBA(50, 50, 50);
	bool m_FitInWindow = true;
	bool m_CanBeSelected = true;
	bool m_DrawTextAtCenter = false;
	float m_OptionsTextWidth = 100.0f;
	
	std::function<void()> onClick;
	std::function<void()> onValueChange;

	bool waitingForTouchRelease = false;
	bool hasPressedThisFrame = false;
	bool isPointerOver = false;
	bool isPressed = false;

	//checkbox
	bool* pCheckBoxBool = NULL;
	bool tmpBool = false;
	int tmpInt = 0;
	float tmpFloat = 0;

	bool m_HoldToChange = false;
};