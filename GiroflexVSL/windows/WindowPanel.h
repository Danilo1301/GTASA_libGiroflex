#pragma once

#include "../pch.h"

class PanelButton {
public:
	PanelButton() {}
};

class WindowPanel {
public:
	static bool Visible;
	static std::vector<PanelButton*> PanelButtons;
	static int NumOfSlots;

	static void Toggle(bool state);
	static void Create();
	static void Remove();
	static void Update();
	static void Draw();

	/*
	Draws a button, returns whether is pressed or not
	*/
	static bool DrawButton(int gxtId, int num1, int num2, CVector2D position, CVector2D size, bool statePressedDown, CRGBA btnColor);
};