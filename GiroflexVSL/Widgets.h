#pragma once

#include <map>

struct WidgetData {
    bool isPressed = false;
    bool hasJustPressed = false;
    bool hasJustReleased = false;
    unsigned int pressTime = 0;
};

class Widgets {
public:
    static std::map<int, WidgetData> m_Widgets;

    static void Update(int dt);

    static bool IsWidgetPressed(int widgetId);
    static bool IsWidgetJustPressed(int widgetId);
};