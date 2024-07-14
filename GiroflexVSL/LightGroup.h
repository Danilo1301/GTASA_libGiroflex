#pragma once

#include "pch.h"

#include "Point.h"
#include "LightGroupRotateObject.h"

class LightGroup {
public:
    CVector offset = CVector(0, 0, 0);
    std::vector<Point*> points;

    float distance = 1.0f;
    float curve = 0.0f;

	CRGBA color1 = CRGBA(255, 0, 0);
	CRGBA color2 = CRGBA(0, 0, 255);
	CRGBA color3 = CRGBA(255, 255, 255);

	int patternOffset = 0;

	float radius = 1.0f;
	
	bool renderShadow = false;
	float shadowIntensity = 0.80f;
	float shadowSize = 5.0f;
	float shadowPositionX = 0.0f;
	float shadowPositionY = 0.0f;
	int shadowTexture = 3;
	float shadowRotation = 0.0f;
	bool shadowFlipTextures = false;

	bool renderPointLight = true;
	float pointLightIntensity = 1.00f;
	float pointLightDistance = 60.0f;

	float nearClip = -0.30f;

	//bool usePointPositionInsteadOfIndex = false;

	bool useSmallWhiteCorona = true;
	float smallWhiteCoronaScale = 0.3f;
	int smallWhiteCoronaTexture = 0;
	CRGBA smallWhiteCoronaColor = CRGBA(255, 255, 255);

	bool freezeLights = false;

	bool useFlare = false;
	float flareIntensity = 1.00f;
	float flareDistance = 100.0f;
	int flareTexture = 1;

	int coronaTexture = 0;

	int lightSlotId = 0;

	bool alwaysEnabled = false;
	//bool enableOnSpawn = false;

	bool rotate = false;
	float rotateDistance = 0.5f;
	float rotateSpeed = 10.0f;
	bool rotateInverse = false;

	float rotateAngle = 0.0f;

	bool useLightbarLEDs = false;
	int lightbarLEDStartIndex = 1;
	bool useNormalLEDs = false;
	int normalLEDStartIndex = 1;
	CRGBA ledColorEnabled = CRGBA(255, 255, 255);
	CRGBA ledColorDisabled = CRGBA(0, 0, 0);

	LightGroupRotateObject rotateObject;

    LightGroup() {}

    ~LightGroup()
    {
        Destroy();
    }

    Point* AddPoint()
	{
		Point* point = new Point();
		points.push_back(point);
		return point;
	}

	void SetNumberOfPoints(int amount)
	{
		while(points.size() < amount)
		{
			AddPoint();
		}
		while(points.size() > amount)
		{
			RemovePoint(points[points.size() - 1]);
		}
	}

	void RemovePoint(Point* point)
	{
		auto it = std::find(points.begin(), points.end(), point);
		if (it == points.end()) return;
		points.erase(it);
		delete point;
	}

	void RemoveAllPoints()
	{
		while (points.size() > 0)
			RemovePoint(points[0]);
	}

	void Destroy()
	{
		RemoveAllPoints();
	}

	Json::Value ToJSON()
	{
		Json::Value value = Json::objectValue;

		value["offset"] = CVectorToJSON(offset);

		//points
		value["points"] = Json::arrayValue;
		for (auto point : points) {
			value["points"].append( point->ToJSON() );
		}

		value["distance"] = distance;
		value["curve"] = curve;

		value["color1"] = ColorToJSON(color1);
		value["color2"] = ColorToJSON(color2);
		value["color3"] = ColorToJSON(color3);

		value["patternOffset"] = patternOffset;

		value["radius"] = radius;

		value["renderShadow"] = renderShadow;
		value["shadowIntensity"] = shadowIntensity;
		value["shadowSize"] = shadowSize;
		value["shadowPositionX"] = shadowPositionX;
		value["shadowPositionY"] = shadowPositionY;
		value["shadowTexture"] = shadowTexture;
		value["shadowRotation"] = shadowRotation;
		value["shadowFlipTextures"] = shadowFlipTextures;

		value["renderPointLight"] = renderPointLight;
		value["pointLightIntensity"] = pointLightIntensity;
		value["pointLightDistance"] = pointLightDistance;

		value["nearClip"] = nearClip;

		//value["usePointPositionInsteadOfIndex"] = usePointPositionInsteadOfIndex;

		value["useSmallWhiteCorona"] = useSmallWhiteCorona;
		value["smallWhiteCoronaScale"] = smallWhiteCoronaScale;
		value["smallWhiteCoronaTexture"] = smallWhiteCoronaTexture;
		value["smallWhiteCoronaColor"] = ColorToJSON(smallWhiteCoronaColor);

		value["freezeLights"] = freezeLights;

		value["useFlare"] = useFlare;
		value["flareIntensity"] = flareIntensity;
		value["flareDistance"] = flareDistance;
		value["flareTexture"] = flareTexture;

		value["coronaTexture"] = coronaTexture;

		value["lightSlotId"] = lightSlotId;

		value["alwaysEnabled"] = alwaysEnabled;
		//value["enableOnSpawn"] = enableOnSpawn;

		value["rotate"] = rotate;
		value["rotateDistance"] = rotateDistance;
		value["rotateSpeed"] = rotateSpeed;
		value["rotateInverse"] = rotateInverse;

		value["useLightbarLEDs"] = useLightbarLEDs;
		value["useNormalLEDs"] = useNormalLEDs;
		value["normalLEDStartIndex"] = normalLEDStartIndex;

		value["ledColorEnabled"] = ColorToJSON(ledColorEnabled);
		value["ledColorDisabled"] = ColorToJSON(ledColorDisabled);

		//rotate object
		Json::Value rotateObjectValue = Json::objectValue;
		rotateObjectValue["speed"] = rotateObject.speed;
		rotateObjectValue["objects"] = Json::arrayValue;
		for(auto object : rotateObject.objects)
		{
			rotateObjectValue["objects"].append(object);
		}
		value["rotateObject"] = rotateObjectValue;

		return value;
	}

	void FromJSON(Json::Value value)
	{
		offset = ValidateCVector(value["offset"], offset);

		for (int i = 0; i < (int)value["points"].size(); i++)
		{
			auto point = AddPoint();
			point->FromJSON(value["points"][i]);
		}

		distance = ValidateValue(value["distance"], distance).asFloat();
		curve = ValidateValue(value["curve"], curve).asFloat();

		color1 = ValidateColor(value["color1"], color1);
		color2 = ValidateColor(value["color2"], color2);
		color3 = ValidateColor(value["color3"], color3);

		patternOffset = ValidateValue(value["patternOffset"], patternOffset).asInt();

		radius = ValidateValue(value["radius"], radius).asFloat();

		renderShadow = ValidateValue(value["renderShadow"], renderShadow).asBool();
		shadowIntensity = ValidateValue(value["shadowIntensity"], shadowIntensity).asFloat();
		shadowSize = ValidateValue(value["shadowSize"], shadowSize).asFloat();
		shadowPositionX = ValidateValue(value["shadowPositionX"], shadowPositionX).asFloat();
		shadowPositionY = ValidateValue(value["shadowPositionY"], shadowPositionY).asFloat();
		shadowTexture = ValidateValue(value["shadowTexture"], shadowTexture).asInt();
		shadowRotation = ValidateValue(value["shadowRotation"], shadowRotation).asFloat();
		shadowFlipTextures = ValidateValue(value["shadowFlipTextures"], shadowFlipTextures).asBool();

		renderPointLight = ValidateValue(value["renderPointLight"], renderPointLight).asBool();
		pointLightIntensity = ValidateValue(value["pointLightIntensity"], pointLightIntensity).asFloat();
		pointLightDistance = ValidateValue(value["pointLightDistance"], pointLightDistance).asFloat();

		nearClip = ValidateValue(value["nearClip"], nearClip).asFloat();

		//usePointPositionInsteadOfIndex = ValidateValue(value["usePointPositionInsteadOfIndex"], usePointPositionInsteadOfIndex).asBool();
	
		useSmallWhiteCorona = ValidateValue(value["useSmallWhiteCorona"], useSmallWhiteCorona).asBool();
		smallWhiteCoronaScale = ValidateValue(value["smallWhiteCoronaScale"], smallWhiteCoronaScale).asFloat();
		smallWhiteCoronaTexture = ValidateValue(value["smallWhiteCoronaTexture"], smallWhiteCoronaTexture).asInt();
		smallWhiteCoronaColor = ValidateColor(value["smallWhiteCoronaColor"], smallWhiteCoronaColor);

		freezeLights = ValidateValue(value["freezeLights"], freezeLights).asBool();

		useFlare = ValidateValue(value["useFlare"], useFlare).asBool();
		flareIntensity = ValidateValue(value["flareIntensity"], flareIntensity).asFloat();
		flareDistance = ValidateValue(value["flareDistance"], flareDistance).asFloat();
		flareTexture = ValidateValue(value["flareTexture"], flareTexture).asInt();

		coronaTexture = ValidateValue(value["coronaTexture"], coronaTexture).asInt();

		lightSlotId = ValidateValue(value["lightSlotId"], lightSlotId).asInt();

		alwaysEnabled = ValidateValue(value["alwaysEnabled"], alwaysEnabled).asBool();
		//enableOnSpawn = ValidateValue(value["enableOnSpawn"], enableOnSpawn).asBool();

		rotate = ValidateValue(value["rotate"], rotate).asBool();
		rotateDistance = ValidateValue(value["rotateDistance"], rotateDistance).asFloat();
		rotateSpeed = ValidateValue(value["rotateSpeed"], rotateSpeed).asFloat();
		rotateInverse = ValidateValue(value["rotateInverse"], rotateInverse).asBool();

		useLightbarLEDs = ValidateValue(value["useLightbarLEDs"], useLightbarLEDs).asBool();
		useNormalLEDs = ValidateValue(value["useNormalLEDs"], useNormalLEDs).asBool();
		normalLEDStartIndex = ValidateValue(value["normalLEDStartIndex"], normalLEDStartIndex).asInt();

		ledColorEnabled = ValidateColor(value["ledColorEnabled"], ledColorEnabled);
		ledColorDisabled = ValidateColor(value["ledColorDisabled"], ledColorDisabled);

		//rotate object
		Json::Value rotateObjectValue = value["rotateObject"];
		rotateObject.speed = ValidateValue(rotateObjectValue["speed"], rotateObject.speed).asInt();

		rotateObject.objects.clear();
		for (int i = 0; i < (int)rotateObjectValue["objects"].size(); i++)
		{
			auto object = rotateObjectValue["objects"][i].asString();
			rotateObject.objects.push_back(object);
		}
	}
};