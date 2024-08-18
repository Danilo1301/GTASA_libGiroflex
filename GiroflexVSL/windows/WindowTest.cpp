#include "WindowTest.h"

extern IMenuVSL* menuVSL;

IWindow* WindowTest::m_Window = NULL;

void WindowTest::Create()
{
	if (m_Window) return;

    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "Test";

    auto test1 = window->AddButton("test 1", CRGBA(255, 255, 255));
    test1->onClick = []()
    {
        Remove();
    };

    auto button_close = window->AddButton("~r~Close", CRGBA(255, 255, 255));
    button_close->onClick = []()
    {
        Remove();
    };
}

void WindowTest::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
}