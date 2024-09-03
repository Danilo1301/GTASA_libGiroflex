#pragma once

#include "pch.h"

struct TouchData
{
	bool isPressed;
	int timePressed;
};

class Touches {
public:
    static bool isTouchPressed;
    static bool hasTouchBeenPressedThisFrame;
	static bool hasTouchBeenReleasedThisFrame;

    static void Update();

    static void SetTouchState(int touchId, bool pressed);

    static bool GetTouchIdState(int touchId);
    static int GetTouchIdPressTime(int touchId);
};