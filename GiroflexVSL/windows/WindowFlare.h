#pragma once

#include "../pch.h"

#include "../menu/Menu.h"
#include "../LightGroup.h"

class WindowFlare {
public:
	static void Create(Window* parent, LightGroup* lightGroup);
};