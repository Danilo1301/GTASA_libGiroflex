#include "Widgets.h"
#include "CleoOpcodes.h"
#include "Log.h"

std::map<int, WidgetData> Widgets::m_Widgets;

void Widgets::Update(int dt)
{
    for(auto& pair : m_Widgets)
    {
        int widgetId = pair.first;
        WidgetData* data = &m_Widgets[widgetId]; //imporant, using pointer instead

        if(data->isPressed)
        {
            data->pressTime += dt;
            Log::Level(eLogLevel::LOG_UPDATE) << "Widget " << widgetId << " pressTime: " << data->pressTime << std::endl;
        }

        data->hasJustPressed = false;
        data->hasJustReleased = false;
    }
}

bool Widgets::IsWidgetPressed(int widgetId)
{
    //Log::file << "[Widgets] IsWidgetPressed " << widgetId << "" << std::endl;

    if (m_Widgets.find(widgetId) == m_Widgets.end())
    {
        WidgetData data;
        data.isPressed = false;
        data.hasJustPressed = false;
        data.hasJustReleased = false;
        data.pressTime = 0;
        m_Widgets[widgetId] = data;

        //Log::file << "[Widgets] Created new widgetData, pressed " << (m_Widgets[widgetId].isPressed ? "1" : "0") << "" << std::endl;
    }

    bool pressed = IS_WIDGET_PRESSED(widgetId);

    WidgetData* data = &m_Widgets[widgetId];

    //Log::file << "[Widgets] Widget pressed: " << (pressed ? "1" : "0" ) << std::endl;
    //Log::file << "[Widgets] Widget data pressed: " << (data->isPressed ? "1" : "0" ) << std::endl;

    if(pressed)
    {
        if(!data->isPressed)
        {
            data->isPressed = true;
            data->hasJustPressed = true;
            data->pressTime = 0;
            Log::Level(eLogLevel::LOG_BOTH) << "[Widgets] Widget " << widgetId << " hasJustPressed" << std::endl;
        }
    } else {

        //if(data.isPressed) //for some fucking reason it runs the true statement even if its value is false, or I am just dumb?
        if(data->isPressed == true)
        {
            data->isPressed = false;
            data->hasJustReleased = true;
            data->pressTime = 0;
            Log::Level(eLogLevel::LOG_BOTH) << "[Widgets] Widget " << widgetId << " hasJustReleased" << std::endl;
        } 
    }
    return pressed;
}

bool Widgets::IsWidgetJustPressed(int widgetId)
{
    IsWidgetPressed(widgetId);

    WidgetData& data = m_Widgets[widgetId];
    return data.hasJustPressed;
}