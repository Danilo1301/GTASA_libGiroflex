#pragma once

#include "pch.h"

#include "menu/Menu.h"
#include "LightGroup.h"

class WindowLed {
public:
	static void Create(Window* parent, LightGroup* lightGroup);

	static void CreateColor(Window* parent, LightGroup* lightGroup);
};