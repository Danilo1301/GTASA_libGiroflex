#include "WindowLightGroups.h"

#include "WindowMain.h"
#include "WindowEditing.h"
#include "WindowWhiteCorona.h"
#include "WindowShadow.h"
#include "WindowPointLight.h"
#include "WindowFlare.h"
#include "WindowRotate.h"
#include "WindowLed.h"
#include "WindowPanel.h"
#include "WindowPatterns.h"

#include "ModelInfos.h"
#include "LightGroupDatas.h"
#include "Globals.h"
#include "Vehicles.h"
#include "Log.h"

void WindowLightGroups::Create(Window* parent)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "WindowLightGroups: Create" << std::endl;

    auto modelId = WindowMain::m_ModelId;

    auto window = Menu::AddWindow(6, parent);
    window->showPageControls = true;
    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    int numLightgroups = 0;
    if(ModelInfos::HasModelInfo(modelId))
    {
        numLightgroups = ModelInfos::GetModelInfo(modelId)->lightGroups.size();
    }

    auto text_lightgroups = window->AddText(36);
    text_lightgroups->text->num1 = numLightgroups;

    auto button_add = window->AddButton(37);
    button_add->onClick = [modelId, window, parent]() {
        if (!ModelInfos::HasModelInfo(modelId))
        {
            ModelInfos::CreateModelInfo(modelId);
        }

        auto lightGroup = new LightGroup();
        lightGroup->AddPoint();
        lightGroup->AddPoint();
        //lightGroup->type = eLightGroupType::TWO_LIGHTS;
        //lightGroup3->usePointPositionInsteadOfIndex = true;
        lightGroup->offset.z = 1.0f;
        //lightGroup->MakeLightGroup();
        ModelInfos::GetModelInfo(modelId)->AddLightGroup(lightGroup);

        //enable all lights
        auto vehicle = Globals::GetPlayerVehicle();
        if (vehicle)
        {
            vehicle->Update(0);
            vehicle->SetGiroflexEnabled(true);
        }

        window->RemoveThisWindow();
        Create(parent);
    };

    if (ModelInfos::HasModelInfo(modelId))
    {
        int i = 0;
        for (auto lightGroup : ModelInfos::GetModelInfo(modelId)->lightGroups)
        {
            auto button_edit = window->AddButton(38, Menu::GetStyle()->COLOR_BUTTON_SECONDARY);
            button_edit->text->num1 = i+1;

            button_edit->AddColorIndicator(&lightGroup->color3);
            button_edit->AddColorIndicator(&lightGroup->color2);
            button_edit->AddColorIndicator(&lightGroup->color1);

            button_edit->onClick = [window, lightGroup]() {
                CreateEditLightGroup(window, lightGroup);
            };

            i++;
        }
    }
}


void WindowLightGroups::CreateEditLightGroup(Window* parent, LightGroup* lightGroup)
{
    auto modelId = WindowMain::m_ModelId;

    auto window = Menu::AddWindow(6, parent);
    window->showPageControls = true;
    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    auto option_giroflex = window->AddOptions(31);
    option_giroflex->optionsValue = (int)lightGroup->points.size();
    for(int i = 0; i <= 50; i++)
    {
        option_giroflex->AddOption(32, i, 0);
    }
    option_giroflex->onValueChange = [option_giroflex, lightGroup]() {
        //LightGroupDatas::DeleteLightGroupRerefences(lightGroup);

        lightGroup->SetNumberOfPoints(option_giroflex->optionsValue);

        //reset lightGroupData
        for (auto pair : Vehicles::m_Vehicles)
        {
            if (pair.second->modelId != WindowMain::m_ModelId) continue;
            LightGroupDatas::RemoveLightGroupDataFromVehicle(pair.second->hVehicle);
        }

        //enable again
        auto vehicle = Globals::GetPlayerVehicle();
        if (vehicle)
        {
            vehicle->Update(0);
            vehicle->SetGiroflexEnabled(true);
        }
    };

    auto edit_poits = window->AddButton(40);
    edit_poits->onClick = [window, lightGroup]()
    {
        CreateEditPoints(window, lightGroup);
    };

    window->AddFloatRange(51, &lightGroup->radius, 0.0f, 5.0f, 0.01f);

    auto button_position = window->AddButton(9);
    button_position->onClick = [lightGroup, window]() {
        /*
        MoveLightsSpeed from 
        * 0.01
        0.0f  0.00
        1.0f  0.01
        2.0f  0.02
        */
        float moveSpeed = WindowEditing::MoveLightsSpeed * 0.01f;

        auto posWindow = Menu::AddPositionWindow(window, &lightGroup->offset, -1000.0f, 1000.0f, moveSpeed, []() {}, []() {});

        posWindow->width += 40.0f;

        auto distance = posWindow->AddFloatRange(53, &lightGroup->distance, -10.0f, 10.0f, moveSpeed);
        distance->onValueChange = [lightGroup]() {
            /*
            lightGroup->MakeLightGroup();

            auto vehicle = Mod::GetPlayerVehicle();
            if (vehicle)
            {
                vehicle->Update(0);
                vehicle->SetGiroflexEnabled(true);
            }
            */
        };

        auto curve = posWindow->AddFloatRange(54, &lightGroup->curve, -1.0f, 1.0f, 0.0005f);
        curve->onValueChange = [lightGroup]() {
            /*
            lightGroup->MakeLightGroup();

            auto vehicle = Mod::GetPlayerVehicle();
            if (vehicle)
            {
                vehicle->Update(0);
                vehicle->SetGiroflexEnabled(true);
            }
            */
        };
    };

    auto button_editing_options = window->AddButton(41);
    button_editing_options->onClick = [lightGroup, window]() {
        WindowEditing::Create(window, lightGroup);
    };

    auto button_white_corona = window->AddButton(52);
    button_white_corona->onClick = [lightGroup, window]() {
        WindowWhiteCorona::Create(window, lightGroup);
    };

    auto button_leds = window->AddButton(100);
    button_leds->onClick = [lightGroup, window]() {
        //Menu::m_Visible = false;
        WindowLed::Create(window, lightGroup);
    };

    auto patterns = window->AddButton(108);
    patterns->onClick = [window, lightGroup]() {
        Menu::m_Visible = false;
        WindowPatterns::Create(window, lightGroup);
    };

    //auto lightIdOffset = window->AddIntRange(23, &Vehicle::m_LightIdOffset, -100000, 100000, 1);

    auto patternOffset = window->AddIntRange(76, &lightGroup->patternOffset, 0, 10, 1);
    patternOffset->holdToChange = false;
    patternOffset->onValueChange = [lightGroup]() {
        LightGroupDatas::DeleteLightGroupRerefences(lightGroup);

        auto vehicle = Globals::GetPlayerVehicle();
        if (vehicle)
        {
            vehicle->Update(0);
            vehicle->SetGiroflexEnabled(true);
        }
    };

    window->AddFloatRange(77, &lightGroup->nearClip, -2.0f, 5.0f, 0.01f);

    auto coronaTexture = window->AddIntRange(55, &lightGroup->coronaTexture, 0, 9, 1);
    coronaTexture->holdToChange = false;

    auto lightSlotId = window->AddIntRange(66, &lightGroup->lightSlotId, 0, WindowPanel::NumOfSlots-1, 1);
    lightSlotId->holdToChange = false;

    auto button_shadow = window->AddButton(56);
    button_shadow->onClick = [lightGroup, window]() {
        WindowShadow::Create(window, lightGroup);
    };

    auto button_pointLight = window->AddButton(59);
    button_pointLight->onClick = [lightGroup, window]() {
        WindowPointLight::Create(window, lightGroup);
    };

    auto button_color1 = window->AddButton(48);
    button_color1->AddColorIndicator(&lightGroup->color1);
    button_color1->onClick = [lightGroup, window]() {
        Menu::AddColorMenu(window, &lightGroup->color1);
    };

    auto button_color2 = window->AddButton(49);
    button_color2->AddColorIndicator(&lightGroup->color2);
    button_color2->onClick = [lightGroup, window]() {
        Menu::AddColorMenu(window, &lightGroup->color2);
    };

    auto button_color3 = window->AddButton(50);
    button_color3->AddColorIndicator(&lightGroup->color3);
    button_color3->onClick = [lightGroup, window]() {
        Menu::AddColorMenu(window, &lightGroup->color3);
    };

    auto button_flare = window->AddButton(63);
    button_flare->onClick = [lightGroup, window]() {
        WindowFlare::Create(window, lightGroup);
    };

    auto button_rotate = window->AddButton(94);
    button_rotate->onClick = [lightGroup, window]() {
        WindowRotate::Create(window, lightGroup);
    };

    window->AddCheckbox(67, &lightGroup->alwaysEnabled);

    window->AddCheckbox(42, &lightGroup->freezeLights);

    window->AddCheckbox(109, &lightGroup->enableWithLights);

    //window->AddCheckbox(93, &lightGroup->enableOnSpawn);

    auto button_duplicate = window->AddButton(68);
    button_duplicate->onClick = [window, lightGroup, modelId]()
    {
        Menu::AddConfirmWindow(window, 69, [lightGroup, modelId]() {
            ModelInfos::GetModelInfo(WindowMain::m_ModelId)->DuplicateLightGroup(lightGroup);

            //reset lightGroupData
            for (auto pair : Vehicles::m_Vehicles)
            {
                if (pair.second->modelId != WindowMain::m_ModelId) continue;

                LightGroupDatas::RemoveLightGroupDataFromVehicle(pair.second->hVehicle);
            }

            //enable again
            auto vehicle = Globals::GetPlayerVehicle();
            if (vehicle)
            {
                vehicle->Update(0);
                vehicle->SetGiroflexEnabled(true);
            }

            Menu::RemoveAllWindows();

            WindowMain::Remove();
            WindowMain::Create(vehicle);
        }, []() {

        });
    };

    auto button_delete = window->AddButton(70, CRGBA(170, 70, 70));
    button_delete->onClick = [window, lightGroup, modelId]()
    {
        Menu::AddConfirmWindow(window, 71, [lightGroup, modelId]() {
            ModelInfos::GetModelInfo(WindowMain::m_ModelId)->RemoveLightGroup(lightGroup);

            //idk if this works
            if (ModelInfos::GetModelInfo(WindowMain::m_ModelId)->lightGroups.size() == 0)
            {
                ModelInfos::RemoveModelInfo(WindowMain::m_ModelId);
            }

            //enable lights again
            auto vehicle = Globals::GetPlayerVehicle();
            if (vehicle)
            {
                vehicle->Update(0);
                vehicle->SetGiroflexEnabled(true);
            }

            Menu::RemoveAllWindows();

            WindowMain::Remove();
            WindowMain::Create(vehicle);
        }, []() {

        });
    };
}

void WindowLightGroups::CreateEditPoints(Window* parent, LightGroup* lightGroup)
{
    auto window = Menu::AddWindow(40, parent);
    window->showPageControls = true;
    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    for(int i = 0; i < lightGroup->points.size(); i++)
    {
        auto point = lightGroup->points[i];

        auto button_edit = window->AddButton(38, CRGBA(96, 125, 219));
        button_edit->text->num1 = i+1;

        if(point->useCustomColor)
            button_edit->AddColorIndicator(&point->customColor);

        button_edit->onClick = [window, lightGroup, point]() {
            CreateEditPoint(window, lightGroup, point);
        };
    }
}

void WindowLightGroups::CreateEditPoint(Window* parent, LightGroup* lightGroup, Point* point)
{
    auto window = Menu::AddWindow(40, parent);
    window->showPageControls = true;
    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    auto button_position = window->AddButton(9);
    button_position->onClick = [point, window]() {
        float moveSpeed = WindowEditing::MoveLightsSpeed * 0.01f;

        auto posWindow = Menu::AddPositionWindow(window, &point->customOffset, -1000.0f, 1000.0f, moveSpeed, []() {}, []() {});

        posWindow->width += 40.0f;
    };

    window->AddCheckbox(46, &point->useCustomColor);

    auto button_color = window->AddButton(47);
    button_color->AddColorIndicator(&point->customColor);
    button_color->onClick = [point, window]() {
        Menu::AddColorMenu(window, &point->customColor);
    };

    auto rotateObject = window->AddButton(94);
    rotateObject->onClick = [window, lightGroup, point]() {
        Menu::m_Visible = false;
        WindowRotate::CreatePointRotate(window, lightGroup, point);
    };
}