#pragma once

#include "pch.h"
#include "LightGroupRotateObject.h"

extern CCamera* camera;

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

enum class eSirenDirection {
	FRONT,
	BOTH,
	BACK
};

class Point {
public:
	CVector customOffset = { 0, 0, 0 };

    bool useCustomColor = false;
	CRGBA customColor = { 255, 255, 0 };

	LightGroupRotateObject rotateObject;

	Json::Value ToJSON()
	{
		Json::Value value = Json::objectValue;

		value["customOffset"] = CVectorToJSON(customOffset);
		value["useCustomColor"] = useCustomColor;
		value["customColor"] = ColorToJSON(customColor);

		//rotate object
		Json::Value rotateObjectValue = Json::objectValue;
		rotateObjectValue["speed"] = rotateObject.speed;
		rotateObjectValue["object"] = rotateObject.object;
		rotateObjectValue["axis"] = (int)rotateObject.axis;
		value["rotateObject"] = rotateObjectValue;
		

		return value;
	}

	void FromJSON(Json::Value value)
	{
		customOffset = ValidateCVector(value["customOffset"], customOffset);
		useCustomColor = ValidateValue(value["useCustomColor"], useCustomColor).asBool();
		customColor = ValidateColor(value["customColor"], customColor);

		//rotate object
		Json::Value rotateObjectValue = value["rotateObject"];
		if(!rotateObjectValue.isNull())
		{
			rotateObject.speed = ValidateValue(rotateObjectValue["speed"], rotateObject.speed).asInt();
			rotateObject.object = ValidateValue(rotateObjectValue["object"], rotateObject.object).asString();
			rotateObject.axis = (eRotateObjectAxis)ValidateValue(rotateObjectValue["axis"], (int)rotateObject.axis).asInt();
		}
	}

	static double GetAngle(CVehicle* vehicle, CVector position)
	{
        CVector cameraPos = camera->m_matrix->pos;

		//auto vec1 = vehicle->TransformFromObjectSpace(CVector(position.x, 0, position.z));
		auto vec1 = TransformFromObjectSpace(vehicle, CVector(position.x, 0.0001f, position.z));
		auto vec2 = TransformFromObjectSpace(vehicle, position);
		auto vec3 = cameraPos;

		double angle = GetAngleBetweenVectors(vec1, vec2, vec3);

		if (isnan(angle)) {
			angle = 0.0001;
		}
		return angle;
	}

	static double GetRadiusMultiplierByAngle(double currentDir, double startFadeDir, double offsetFadeDir) {
		double end = startFadeDir + offsetFadeDir;

		if (offsetFadeDir < 0) {
			if (currentDir < startFadeDir + offsetFadeDir) currentDir = startFadeDir + offsetFadeDir;
			if (currentDir > startFadeDir) currentDir = startFadeDir;
		}
		else {
			if (currentDir < startFadeDir) currentDir = startFadeDir;
			if (currentDir > startFadeDir + offsetFadeDir) currentDir = startFadeDir + offsetFadeDir;
		}

		return abs((currentDir - end) / offsetFadeDir);
	}

	float GetRadiusMultiplier(double angle, eSirenDirection direction, float positionY) {
		if (direction == eSirenDirection::BOTH) return 1.0f;

		eSirenDirection atDirection = positionY > 0 ? eSirenDirection::FRONT : eSirenDirection::BACK;

		double start_fadeout = 1.0f;
		double fadeout_offset = 0.6f;

		if (atDirection != direction) {
			start_fadeout += fadeout_offset;
			fadeout_offset *= -1;
		}

		return (float)GetRadiusMultiplierByAngle(angle, start_fadeout, fadeout_offset);
	}
};