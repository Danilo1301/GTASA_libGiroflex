#include "WindowLed.h"

void WindowLed::Create(Window* parent, LightGroup* lightGroup)
{
    auto window = Menu::AddWindow(100, parent);
    window->showPageControls = true;
    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    window->AddCheckbox(101, &lightGroup->useLightbarLEDs);

    window->AddCheckbox(102, &lightGroup->useNormalLEDs);

    auto ledStartIndex = window->AddIntRange(103, &lightGroup->normalLEDStartIndex, 1, 10000, 1);
    ledStartIndex->holdToChange = false;
}