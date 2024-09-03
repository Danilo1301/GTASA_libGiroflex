#include "Mod.h"

#include "Vehicles.h"
#include "CleoOpcodes.h"
#include "Widgets.h"
#include "Touches.h"

#include "windows/WindowTest.h"
#include "windows/WindowMain.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

static bool firstUpdated = false;
bool canTurnSirenOn = true;

void Mod::Init()
{
    menuVSL->debug->visible = true;
}

void Mod::Update()
{
    firstUpdated = true;

    Widgets::Update(menuVSL->deltaTime);
    Vehicles::Update();

    Touches::Update();

    //Main menu and toggle lights
    if (Touches::GetTouchIdState(6) && Touches::GetTouchIdState(5))
    {
        if (Touches::GetTouchIdPressTime(6) > 500)
        {
            //auto vehicle = Globals::GetPlayerVehicle();

            if (canTurnSirenOn)
            {
                canTurnSirenOn = false;

                //vehicle->SetGiroflexEnabled(!vehicle->prevLightsState);
            }

            if (Touches::GetTouchIdPressTime(6) > 1000)
            {
                if (!WindowMain::m_Window)
                {
                    //vehicle->SetGiroflexEnabled(true, true);

                    WindowMain::Create();
                }
            }
        }
    }
    else {
        canTurnSirenOn = true;
    }

    if(!WindowTest::m_Window)
    {
        if(Widgets::IsWidgetJustPressed(13))
        {
            WindowTest::Create();
        }
    }
}

void Mod::Render()
{   
    if(!firstUpdated) return;

    auto vehicles = Vehicles::GetVehicles();

    //menuVSL->debug->AddLine(std::to_string(vehicles.size()) + " vehicles on giro");

    for(auto vehicle : vehicles)
    {
        vehicle->Render();
    }
}