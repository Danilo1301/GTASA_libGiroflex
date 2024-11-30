#pragma once

#include "pch.h"

enum class eRotateObjectAxis {
	X,
	Y,
	Z
};

struct LightGroupRotateObject {
	bool rotate = true;
	bool rotateAlways = false;
	float speed = 5.0f;
	eRotateObjectAxis axis = eRotateObjectAxis::Z;
	bool flipForward = false;
	std::string object = "[none]";
	RwMatrix* matrix = NULL;
};