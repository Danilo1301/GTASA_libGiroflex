#include "WindowLed.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

void WindowLed::Create(Window* parent, LightGroup* lightGroup)
{
    auto window = Menu::AddWindow(100, parent);
    window->showPageControls = true;
    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    window->AddCheckbox(101, &lightGroup->useLightbarLEDs);

    auto lightbarLEDStartIndex = window->AddIntRange(107, &lightGroup->lightbarLEDStartIndex, 1, 10000, 1);
    lightbarLEDStartIndex->holdToChange = false;
    
    window->AddCheckbox(102, &lightGroup->useNormalLEDs);

    auto normalLEDStartIndex = window->AddIntRange(103, &lightGroup->normalLEDStartIndex, 1, 10000, 1);
    normalLEDStartIndex->holdToChange = false;

    auto colorEnabled = window->AddButton(105);
    colorEnabled->AddColorIndicator(&lightGroup->ledColorEnabled);
    colorEnabled->onClick = [lightGroup, window]() {
        Menu::AddColorMenu(window, &lightGroup->ledColorEnabled);
    };

    auto colorDisabled = window->AddButton(106);
    colorDisabled->AddColorIndicator(&lightGroup->ledColorDisabled);
    colorDisabled->onClick = [lightGroup, window]() {
        Menu::AddColorMenu(window, &lightGroup->ledColorDisabled);
    };
}

/*
NEW MENU

void WindowLed::Create(Window* parent, LightGroup* lightGroup)
{
    auto window = menuVSL->AddWindow();
    window->m_Title = "Edit LEDs";
    window->m_Position = CVector2D(400, 200);

    window->AddCheckbox("Enable ~y~lightbar ~w~LEDs", &lightGroup->useLightbarLEDs);
    window->AddIntRange("Start ~y~lightbar ~w~ID", &lightGroup->lightbarLEDStartIndex, 1, 10000, 1);

    window->AddCheckbox("Enable ~g~led ~w~LEDs", &lightGroup->useNormalLEDs);
    window->AddIntRange("Start ~g~led ~w~ID", &lightGroup->normalLEDStartIndex, 1, 10000, 1);

    auto close = window->AddButton("> ~r~Close", CRGBA(0, 0, 0, 0));
    close->onClick = [window]() {
        window->SetToBeRemoved();
        Menu::m_Visible = true;
    };
}
*/