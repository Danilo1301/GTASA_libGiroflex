#pragma once

#include <iostream>
#include <functional>

#include "SimpleGTA.h"

enum eItemType {
    ITEM_NONE,
	ITEM_TEXT,
	ITEM_BUTTON,
	ITEM_OPTIONS,
	ITEM_INT_RANGE,
	ITEM_FLOAT_RANGE,
	ITEM_CHECKBOX
};

struct ColorIndicator
{
	CRGBA* color;
};

struct Option
{
	int value = 0;
	std::string name = "";
};

class IItem {
public:
	int m_Id = -1;
    CVector2D m_Position = CVector2D(0, 0);
    CVector2D m_Size = CVector2D(400, 50);
    eItemType m_Type = eItemType::ITEM_NONE;
    std::string m_Text = "Item";
	CRGBA m_TextColor = CRGBA(255, 255, 255);
	CRGBA m_BackgroundColor = CRGBA(0, 0, 0, 0);
	bool m_FitInWindow = true;
	bool m_CanBeSelected = true;
	bool m_DrawTextAtCenter = false;
	float m_OptionsTextWidth = 100.0f;
	bool m_Visible = true;

	std::string* m_StringPtrAtRight = NULL;
	std::string m_StringAtRight = "";
	CVector2D m_StringAtRightOffset = CVector2D(0.0f, 0.0f);
	
	std::function<void()> onClick;
	std::function<void()> onValueChange;
	std::function<void()> onUpdate;

	bool waitingForTouchRelease = false;
	bool hasPressedThisFrame = false;
	bool isPointerOver = false;
	bool isPressed = false;

	bool* pCheckBoxBool = NULL;
	bool tmpBool = false;
	int tmpInt = 0;
	float tmpFloat = 0;

	int m_OptionsSelectedIndex = 0;

	bool m_HoldToChange = false;
	
	std::vector<ColorIndicator> m_ColorIndicators;

	std::vector<Option> m_Options;

	/*1.0.0*/

	/*1.1.0*/
	virtual void AddColorIndicator(CRGBA* color) = 0;
	virtual void AddOption(int value, std::string name) = 0;
	virtual void SetCurrentOption(int value) = 0;
	virtual Option GetCurrentOption() = 0;
	virtual bool GetCheckboxValue() = 0;
};