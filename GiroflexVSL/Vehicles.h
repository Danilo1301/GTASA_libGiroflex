#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "RenderCorona.h"
#include "Vehicle.h"

#include <map>
#include <vector>

class Vehicles {
public:
	
	static std::map<int, Vehicle*> m_Vehicles;
	static std::vector<RenderCorona> m_CoronasToRender;
	static std::vector<int> m_NewVehiclesRef;

	static void TryCreateVehicle(int hVehicle, int modelId);
	static bool HasVehicleHandle(int hVehicle);
	static Vehicle* GetVehicleByHandle(int hVehicle);
	static Vehicle* GetVehicleByVecIndex(int index);
	static void RemoveVehicle(int hVehicle);
	static void CheckStreamedOutVehicles();

	static void Update(int dt);

	static void RenderBefore(CVehicle* pVehicle);
	static void RenderAfter(CVehicle* pVehicle);

	static void AddCoronaToRender(RenderCorona corona);

	static void TryFindNewVehicles();
};