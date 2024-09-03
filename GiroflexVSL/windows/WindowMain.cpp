#include "WindowMain.h"

extern IMenuVSL* menuVSL;

IWindow* WindowMain::m_Window = NULL;

void WindowMain::Create()
{
	if (m_Window) return;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "Main";

    auto giroflex = window->AddButton("Giroflex", CRGBA(255, 255, 255));
    giroflex->onClick = []()
    {
        
    };

    auto options = window->AddButton("Options", CRGBA(255, 255, 255));
    options->onClick = []()
    {
        
    };

    auto close = window->AddButton("~r~Close", CRGBA(255, 255, 255));
    close->onClick = []()
    {
        Remove();
    };
}

void WindowMain::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
}