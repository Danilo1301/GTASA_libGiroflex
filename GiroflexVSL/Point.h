#pragma once

#include "pch.h"
#include "LightGroupRotateObject.h"

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
};