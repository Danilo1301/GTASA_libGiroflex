#pragma once

#include "pch.h"

enum class eRotateObjectAxis {
	X,
	Y,
	Z
};

struct LightGroupRotateObject {
	bool rotate = true;
	bool rotateAlways = true;
	float speed = 5.0f;
	eRotateObjectAxis axis = eRotateObjectAxis::Z;
	std::vector<std::string> objects;
};