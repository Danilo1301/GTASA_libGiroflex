#pragma once

#include "pch.h"

#include "Vehicle.h"

class Vehicles {
public:
    static std::map<int, Vehicle*> m_Vehicles;

    static bool HasVehicleHandle(int hVehicle);
    static Vehicle* TryCreateVehicle(int hVehicle);
    static Vehicle* GetVehicleByHandle(int hVehicle);
    static void RemoveVehicle(int hVehicle);

    static void Update();

    static std::vector<Vehicle*> GetVehicles();
};