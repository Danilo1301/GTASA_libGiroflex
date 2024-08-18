#pragma once

#include "pch.h"

#include "CleoOpcodes.h"

class Vehicle {
public:
    int hVehicle;
    CVehicle* pVehicle;
    int modelId;

    Vehicle(int hVehicle, CVehicle* pVehicle);

    void Render();
    void DrawFrames();
    
    void Update();
    
    void RenderCoronas();

    bool PlayerIsUsingThisVehicle();

    static void RegisterTestCorona(int lightId, CEntity* entity, CVector position, CRGBA color);
};