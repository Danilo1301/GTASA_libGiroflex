#pragma once

#include "../pch.h"

#include "../menu/Menu.h"
#include "../LightGroup.h"

class WindowRotate {
public:
	static void Create(Window* parent, LightGroup* lightGroup);

	static void CreatePointRotate(Window* parent, LightGroup* lightGroup, Point* point);
};