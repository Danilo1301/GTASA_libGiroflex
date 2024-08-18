#include "Mod.h"

#include "Vehicles.h"
#include "CleoOpcodes.h"
#include "Widgets.h"

#include "windows/WindowTest.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

extern bool (*CTouchInterface_IsTouchDown)(int param_1);

static bool firstUpdated = false;

void Mod::Init()
{
    menuVSL->debug->visible = true;
}

void Mod::Update()
{
    firstUpdated = true;

    Widgets::Update(menuVSL->deltaTime);
    Vehicles::Update();

    UpdateTestTouches();

    if(!WindowTest::m_Window)
    {
        if(Widgets::IsWidgetJustPressed(13))
        {
            WindowTest::Create();
        }
    }
}

void Mod::UpdateTestTouches()
{
    //menuVSL->debug->AddLine("checking touches:");

    for(int i = 1; i <= 9; i++)
    {
        auto state = GET_TOUCH_POINT_STATE(i, 0);
        if(state != 0)
        {
            menuVSL->debug->AddLine("touch " + std::to_string(i) + " state: " + std::to_string(state));
        }

        auto isDown = CTouchInterface_IsTouchDown(i);
        if(isDown)
        {
            menuVSL->debug->AddLine("touch " + std::to_string(i) + " is DOWN");
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