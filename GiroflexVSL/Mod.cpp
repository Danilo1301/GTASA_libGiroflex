#include "Mod.h"

#include "menu/Draw.h"
#include "menu/Menu.h"

#include "Input.h"
#include "Vehicles.h"
#include "Globals.h"
#include "ModConfig.h"
#include "Log.h"
#include "Widgets.h"

#include "windows/WindowTest.h"
#include "windows/WindowMain.h"
#include "windows/WindowSettings.h"
#include "windows/WindowSelectPanel.h"
#include "windows/WindowPanel.h"
#include "windows/WindowSoundPanel.h"

#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

extern CVector2D *m_vecCachedPos;
extern RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
extern RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);
extern char* (*GetFrameNodeName)(RwFrame* frame);

const char* Mod::m_Version = "3.5.0";

bool canTurnSirenOn = true;
bool canTurnPanelOn = true;

Vehicle* testedVehicle = NULL;

void Mod::Update(int dt)
{
    while (Draw::m_DrawItems.size() > 0)
    {
        auto dw = Draw::m_DrawItems[0];
        Draw::m_DrawItems.erase(std::find(Draw::m_DrawItems.begin(), Draw::m_DrawItems.end(), dw));
        delete dw;
    }

    //

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "> Vehicles --------------" << std::endl;

    Vehicles::TryFindNewVehicles();

    Vehicles::Update(dt);
    
    if(auto vehicle = Globals::GetPlayerVehicle())
    {
        if(testedVehicle != vehicle)
        {
            testedVehicle = vehicle;

            Log::Level(LOG_LEVEL::LOG_UPDATE) << "Player vehicle: " << vehicle->pVehicle << std::endl;
            
            auto pVehicle = vehicle->pVehicle;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "placement: " << CVectorToString(pVehicle->GetPlacement()->pos) << std::endl;
            Log::Level(LOG_LEVEL::LOG_BOTH) << "matrix: " << CVectorToString(pVehicle->GetMatrix()->at) << std::endl;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "pos: " << CVectorToString(pVehicle->m_placement.pos) << std::endl;
            Log::Level(LOG_LEVEL::LOG_BOTH) << "at: " << CVectorToString(pVehicle->m_matrix->at) << std::endl;

            Log::Level(LOG_LEVEL::LOG_BOTH) << "m_pRwClump: " << pVehicle->m_pRwClump << std::endl;

            auto clump = pVehicle->m_pRwClump;

            RpClumpForAllAtomics(clump, [](RpAtomic* atomic, void* data) {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "atomic: " << atomic << std::endl;

                RwFrame* frameAtomic = GetObjectParent((RwObject*)atomic);
		        std::string name = GetFrameNodeName(frameAtomic);

                Log::Level(LOG_LEVEL::LOG_BOTH) << "name: " << name << std::endl;

                if(name.find("lightbar-led-") == std::string::npos) return atomic;

                if (!atomic->geometry) return atomic;

                //atomic->geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;

                auto geometry = atomic->geometry;

                RpGeometryForAllMaterials(geometry, [](RpMaterial* material, void* data) {
                    
                    Log::Level(LOG_LEVEL::LOG_BOTH) << "material: " << material << std::endl;

                    /*
                    material->color = { 255, 255, 255, 255 };

                    material->surfaceProps.ambient = 10;
					material->surfaceProps.diffuse = 10;
					material->surfaceProps.specular = 10;
                    */

                    return material;
                }, 0);

                return atomic;
            }, (void*)((uint32_t)(0)));
        }
    }

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "SoundPanelSystem" << std::endl;

    //SoundPanelSystem::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "> Menu" << std::endl;

    Menu::Update(dt);

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "> WindowSettings" << std::endl;

    WindowSettings::Update(dt);
    WindowSettings::Draw();

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "> WindowSoundPanel" << std::endl;
    
    WindowSoundPanel::Draw();
    WindowSoundPanel::Update(dt);

    WindowPanel::Update();
    WindowPanel::Draw();
    
    Menu::Draw();

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "> Input" << std::endl;

    Input::Update(dt);
    Widgets::Update(dt);

    Mod::ProcessMenuButtons(dt);

    //

    //check old giroflex.csa
    //theres a bug when displaying two popups at same time (when theres two .csa files)
    
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "Check 1 '" + std::string("/storage/emulated/0/cleo/sa/giroflex.csa") + "'" << std::endl;
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "Check 2 '" + std::string("/storage/emulated/0/Android/data/" + std::string(aml->GetCurrentGame()) + "/giroflex.csa") + "'" << std::endl;
    if (!ModConfig::IgnoreOldModVersionMessage)
    {
        if (
            ModConfig::FileExists("/storage/emulated/0/cleo/sa/giroflex.csa") ||
            ModConfig::FileExists("/storage/emulated/0/Android/data/" + std::string(aml->GetCurrentGame()) + "/giroflex.csa")
            )
        {
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "Found" << std::endl;
            Menu::ShowPopup(92, 0, 0, 1000);
        }
    }

    //

    //Draw::DrawBoxWithText(2, 1, 2, {0, 0}, {50, 50}, {255, 0, 0, 255}, {0, 0, 0, 255});

    //

    if (Menu::m_DrawCursor)
    {
        Draw::DrawText(2, m_vecCachedPos->x, m_vecCachedPos->y, CVector2D(50, 300), CRGBA(255, 180, 0));
        Draw::DrawText(1, Draw::m_DrawItems.size(), 0, CVector2D(20, 320), CRGBA(255, 255, 0));
        Draw::DrawText(1, dt, 0, CVector2D(20, 340), CRGBA(255, 255, 0));
        Draw::DrawText(1, Vehicles::m_CoronasToRender.size(), 0, CVector2D(20, 360), CRGBA(255, 255, 0));
        //Draw::DrawText(1, Input::GetTouchIdPressTime(6), 0, CVector2D(20, 400), CRGBA(255, 255, 0));

        //Draw::DrawText(2, (int)Menu::m_MenuOffset.x, (int)Menu::m_MenuOffset.y, CVector2D(20, 320), CRGBA(255, 255, 0));
    }

    if(!Menu::m_Credits->hasShownCredits)
    {
        Menu::ShowCredits(6, 5000);
    }

    Log::Level(LOG_LEVEL::LOG_UPDATE) << "> end" << std::endl;
}

void Mod::ProcessMenuButtons(int dt)
{
    //Mod::ProcessTestMenuButtons(dt);

    //Main menu and toggle lights
    if (Input::GetTouchIdState(6) && Input::GetTouchIdState(5))
    {
        if (Input::GetTouchIdPressTime(6) > 500)
        {
            if (!Globals::IsPlayerInAnyVehicle())
            {
                Menu::ShowPopup(39, 0, 0, 1000);
                return;
            }

            auto vehicle = Globals::GetPlayerVehicle();

            if (canTurnSirenOn)
            {
                canTurnSirenOn = false;

                vehicle->SetGiroflexEnabled(!vehicle->prevLightsState);
            }

            if (Input::GetTouchIdPressTime(6) > 1000)
            {
                if (!WindowMain::m_Window)
                {
                    vehicle->SetGiroflexEnabled(true);

                    WindowMain::Create(vehicle->modelId);
                }
            }
        }
    }
    else {
        canTurnSirenOn = true;
    }

    //Panel
    if (Input::GetTouchIdState(5) && Input::GetTouchIdState(9))
    {
        if (Input::GetTouchIdPressTime(5) > 500 && Input::GetTouchIdPressTime(9) > 500)
        {
            if (Globals::IsPlayerInAnyVehicle())
            {
                auto veh = Globals::GetPlayerVehicle();

                if (canTurnPanelOn)
                {
                    canTurnPanelOn = false;

                    WindowSelectPanel::Create();
                    
                    //WindowPanel::Toggle(!WindowPanel::Visible);
                }
            }
            else {
                Menu::ShowPopup(39, 0, 0, 1000);
            }
        }
    }
    else {
        canTurnPanelOn = true;
    }
}

void Mod::ProcessTestMenuButtons(int dt)
{
    //test menu, 5 and 6
    if (Input::GetTouchIdState(6) && Input::GetTouchIdState(5))
    {
        if (Input::GetTouchIdPressTime(6) > 500)
        {
            WindowTest::Create();
        }
    }
}