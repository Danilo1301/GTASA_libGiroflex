#pragma once

#include "pch.h"

#include "Pattern.h"
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
	bool enableWithLights = true;

	bool rotate = false;
	float rotateDistance = 0.5f;
	float rotateSpeed = 10.0f;
	bool rotateInverse = false;

	float rotateAngle = 0.0f;

	bool useLightbarLEDs = false;
	int lightbarLEDStartIndex = 1;
	bool useNormalLEDs = false;
	int normalLEDStartIndex = 1;
	CRGBA ledColor1Enabled = CRGBA(255, 255, 255);
	CRGBA ledColor2Enabled = CRGBA(255, 255, 255);
	CRGBA ledColor3Enabled = CRGBA(255, 255, 255);
	CRGBA ledColorDisabled = CRGBA(0, 0, 0);

	std::map<std::string, bool> patterns;

    LightGroup();
    ~LightGroup();

    Point* AddPoint();
	void SetNumberOfPoints(int amount);
	void RemovePoint(Point* point);
	void RemoveAllPoints();

	void Update();
	void Destroy();

	Json::Value ToJSON();
	void FromJSON(Json::Value value);

	std::vector<Pattern*> GetPatterns();
};