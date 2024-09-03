#include "Touches.h"

#include "CleoOpcodes.h"
#include "Log.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

extern bool (*CTouchInterface_IsTouchDown)(int param_1);

bool Touches::isTouchPressed;
bool Touches::hasTouchBeenPressedThisFrame;
bool Touches::hasTouchBeenReleasedThisFrame;

std::map<int, TouchData> touchStates;

void Touches::Update()
{
    auto dt = menuVSL->deltaTime;

    hasTouchBeenPressedThisFrame = false;
    hasTouchBeenReleasedThisFrame = false;

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

    for(int i = 1; i <= 9; i++)
    {
        std::string key = "touch_state_" + std::to_string(i);

        auto value = menuVSL->GetGlobalIntVariable(key);

        SetTouchState(i, value == 1);
    }

    int numPresses = 0;

    for (auto pair : touchStates)
    {
        int id = pair.first;
        auto touchData = &touchStates[id];

        if (touchData->isPressed)
        {
            numPresses++;
            touchData->timePressed += dt;

            if(dt > 100)
            {
                Log::Level(eLogLevel::LOG_BOTH) << "touchData->timePressed += dt; dt=" << dt << std::endl;
            }
        }
    }

    if (isTouchPressed)
    {
        if (numPresses == 0)
        {
            isTouchPressed = false;
            Log::Level(eLogLevel::LOG_BOTH) << "Input: Touch released" << std::endl;
            hasTouchBeenReleasedThisFrame = true;
        }
    }
}

void Touches::SetTouchState(int touchId, bool pressed)
{
    if (touchStates.find(touchId) == touchStates.end())
    {
        TouchData data;
        data.isPressed = false;
        data.timePressed = 0;
        touchStates[touchId] = data;
    }

    if (touchStates[touchId].isPressed != pressed)
    {
        if (pressed)
        {
            Log::Level(eLogLevel::LOG_BOTH) << "Touches: Touch " << touchId << " pressed" << std::endl;
        }
        else {
            Log::Level(eLogLevel::LOG_BOTH) << "Touches: Touch " << touchId << " released after " << touchStates[touchId].timePressed << " ms" << std::endl;
        }

        touchStates[touchId].isPressed = pressed;
        touchStates[touchId].timePressed = 0;
    }

    if (pressed)
    {
        if (!isTouchPressed)
        {
            isTouchPressed = true;
            hasTouchBeenPressedThisFrame = true;

            Log::Level(eLogLevel::LOG_BOTH) << "Touches: Touch pressed" << std::endl;
        }
    }
}

bool Touches::GetTouchIdState(int touchId)
{
    if (touchStates.find(touchId) == touchStates.end()) return false;

    return touchStates[touchId].isPressed;
}

int Touches::GetTouchIdPressTime(int touchId)
{
    if (touchStates.find(touchId) == touchStates.end()) return 0;

    return touchStates[touchId].timePressed;
}