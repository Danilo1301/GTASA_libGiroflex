#include "WindowPatterns.h"

#include "Patterns.h"
#include "LightGroupDatas.h"
#include "Globals.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

static std::vector<std::string> allOptions;
static std::vector<std::string> selectedOptions;

void UpdateOptions(LightGroup* lightGroup)
{
    allOptions.clear();
    auto patterns = Patterns::GetCompatiblePatterns(lightGroup->points.size());
    for(auto pattern : patterns)
    {
        allOptions.push_back(pattern->id);
    }

    selectedOptions.clear();
    for(auto p : lightGroup->patterns)
    {
        auto id = p.first;
        auto value = p.second;

        if(value) selectedOptions.push_back(id);
    }
}

void WindowPatterns::Create(Window* parent, LightGroup* lightGroup)
{
    auto window = menuVSL->AddWindow();
    window->m_Title = "Patterns";

    UpdateOptions(lightGroup);
    
    auto selectPatterns = window->AddButton("> Select patterns", CRGBA(0, 0, 0, 0));
    selectPatterns->onClick = [window, lightGroup]() {
        
        auto newWindow = menuVSL->AddWindowMultiOptionsString("Select patterns", window, &selectedOptions, &allOptions);
        newWindow->m_OnCloseWindow = [lightGroup]() {
            menuVSL->debug->AddLine("Closed, now update");

            lightGroup->Update();

            for(auto option : allOptions)
            {
                lightGroup->patterns[option] = false;
            }
            for(auto option : selectedOptions)
            {
                lightGroup->patterns[option] = true;
            }

            UpdateOptions(lightGroup);

            LightGroupDatas::DeleteLightGroupRerefences(lightGroup);

            auto vehicle = Globals::GetPlayerVehicle();
            if (vehicle)
            {
                vehicle->Update(0);
                vehicle->SetGiroflexEnabled(true);
            }   
        };
    };

    auto itemInfo = window->AddText("", CRGBA(255, 255, 255));
    itemInfo->onUpdate = [itemInfo]() {
        itemInfo->m_Text = "Selected patterns: " + std::to_string(selectedOptions.size()) + " / " + std::to_string(allOptions.size());
    };

    auto close = window->AddButton("> ~r~Close", CRGBA(0, 0, 0, 0));
    close->onClick = [window]() {
        window->SetToBeRemoved();
        Menu::m_Visible = true;
    };
}