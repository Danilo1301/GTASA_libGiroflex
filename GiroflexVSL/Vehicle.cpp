#include "Vehicle.h"

#include "Log.h"
#include "CleoOpcodes.h"
#include "VehicleDummy.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

Vehicle::Vehicle(int hVehicle, CVehicle* pVehicle)
{
    this->hVehicle = hVehicle;
    this->pVehicle = pVehicle;
    this->modelId = GET_CAR_MODEL(hVehicle);
}

void Vehicle::Render()
{
    if(!PlayerIsUsingThisVehicle()) return;

    auto vehiclePosition = GetCarPosition(hVehicle);

    menuVSL->DrawWorldText(std::to_string(modelId), vehiclePosition, CRGBA(0, 255, 0), eFontAlignment::ALIGN_LEFT);

    DrawFrames();
    RenderCoronas();
}

void Vehicle::DrawFrames()
{
    RwFrame* froot = (RwFrame*)pVehicle->m_pRwClump->object.parent;

    auto frames = VehicleDummy::GetFramesOnVehicle(pVehicle);
    for (auto frame : frames)
    {
        if (frame == froot) continue;

        CVector position = VehicleDummy::GetTransformedDummyPosition(pVehicle, frame, CVector(0, 0, 0));

        std::string name = GetFrameNodeName(frame);

        menuVSL->DrawWorldText(name, position, CRGBA(255, 255, 0), eFontAlignment::ALIGN_LEFT);
    }
}

void Vehicle::Update()
{
    if(!PlayerIsUsingThisVehicle()) return;

    auto vehiclePosition = GetCarPosition(hVehicle);

    //flash light
    //Command< 0x09E5 >(fposition.x, fposition.y, fposition.z, (int)reflectColor.r, (int)reflectColor.g, (int)reflectColor.b, lightGroup->reflectionDistance);
    CREATE_FLASH_LIGHT(vehiclePosition.x, vehiclePosition.y, vehiclePosition.z, 0, 0, 255, 20.0f);

    //shadow
    CREATE_PARTICLE(3, vehiclePosition.x, vehiclePosition.y, vehiclePosition.z, 0.0f, 10.0f, 255, 255, 255, 255);
}

void Vehicle::RenderCoronas()
{
    int lightId = reinterpret_cast<int>(pVehicle) + 420;

    //Vehicle::RegisterTestCorona(lightId++, pVehicle, CVector(0, 0, 2), CRGBA(0, 255, 0));

    RwFrame* froot = (RwFrame*)pVehicle->m_pRwClump->object.parent;

    auto frames = VehicleDummy::GetFramesOnVehicle(pVehicle);
    for (auto frame : frames)
    {
        if (frame == froot) continue;

        //CVector position = VehicleDummy::GetTransformedDummyPosition(pVehicle, frame, CVector(0, 0, 0));
        //CVector vehiclePosition = GetCarPosition(hVehicle);
        CVector offset = VehicleDummy::GetDummyOffset_KindaFixed(pVehicle, frame, CVector(0, 0, 0));

        std::string name = GetFrameNodeName(frame);

        if (name.find("corona") == std::string::npos) continue;

        if (name.find("corona1") != std::string::npos)
        {
            Vehicle::RegisterTestCorona(lightId++, pVehicle, offset, CRGBA(255, 0, 0));
        }

        if (name.find("corona2") != std::string::npos)
        {
            Vehicle::RegisterTestCorona(lightId++, pVehicle, offset, CRGBA(0, 255, 0));
        }

        if (name.find("corona3") != std::string::npos)
        {
            Vehicle::RegisterTestCorona(lightId++, pVehicle, offset, CRGBA(0, 0, 255));
        }
    }
}

bool Vehicle::PlayerIsUsingThisVehicle()
{
    auto playerActor = GetPlayerActor();
    auto car = ACTOR_USED_CAR(playerActor);
    return car == hVehicle;
}

void Vehicle::RegisterTestCorona(int lightId, CEntity* entity, CVector position, CRGBA color)
{
    RegisterCorona(
        lightId,
        entity,
        color.r,
        color.g,
        color.b,
        color.a,
        position,
        0.5f,
        1000.0f,
        0,
        0,
        false,
        false,
        0,
        0.0f,
        false,
        0.0f,
        0,
        10.0f,
        false,
        false
    );
}