#pragma once

#include "pch.h"

#include "LightGroupData.h"
#include "SirenSystem.h"

class Vehicle {
public:
	static int m_LightIdOffset;

	int hVehicle;
	int modelId;
	CVector position = CVector(0, 0, 0);
	CVector velocity = CVector(0, 0, 0);

	CVehicle* pVehicle = NULL;
	bool gameSirenState = false;
	void* pDriver = NULL;

	bool canBeRemoved = false;

	bool prevLightsState = false;
	bool prevSirenState = false;
	//bool lightsOn = false;
	//bool lightsPaused = false;

	int ledsTime = 0;

	SirenSystem* sirenSystem;

	Vehicle(int hVehicle, int modelId);
	~Vehicle();

	void Init();
	void Destroy();
	void Update(int dt);
	void UpdateLightGroups(int dt);

	void RenderBefore();
	void RenderAfter();

	std::vector<LightGroupData*> GetLightGroupsData();

	void SetGiroflexEnabled(bool enabled, bool forceOn = false);
	void ResetObjectRotation(std::string object);
};