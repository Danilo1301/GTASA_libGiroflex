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

    auto color = window->AddButton(75);
    color->onClick = [lightGroup, window]() {
        CreateColor(window, lightGroup);
    };
}

void WindowLed::CreateColor(Window* parent, LightGroup* lightGroup)
{
    auto window = Menu::AddWindow(100, parent);
    window->showPageControls = true;
    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    auto colorEnabled1 = window->AddButton(48);
    colorEnabled1->AddColorIndicator(&lightGroup->ledColor1Enabled);
    colorEnabled1->onClick = [lightGroup, window]() {
        Menu::AddColorMenu(window, &lightGroup->ledColor1Enabled);
    };

    auto colorEnabled2 = window->AddButton(49);
    colorEnabled2->AddColorIndicator(&lightGroup->ledColor2Enabled);
    colorEnabled2->onClick = [lightGroup, window]() {
        Menu::AddColorMenu(window, &lightGroup->ledColor2Enabled);
    };

    auto colorEnabled3 = window->AddButton(50);
    colorEnabled3->AddColorIndicator(&lightGroup->ledColor3Enabled);
    colorEnabled3->onClick = [lightGroup, window]() {
        Menu::AddColorMenu(window, &lightGroup->ledColor3Enabled);
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