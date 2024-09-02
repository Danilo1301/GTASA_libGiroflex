#include "Vehicle.h"

#include "Log.h"

#include "RenderCorona.h"
#include "Vehicles.h"
#include "ModelInfos.h"
#include "LightGroupDatas.h"
#include "Patterns.h"
#include "Globals.h"
#include "ModConfig.h"
#include "VehicleDummy.h"

#include "windows/WindowEditing.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

extern void* (*GetVehicleFromRef)(int);
extern RwMatrix* (*RwMatrixRotate)(RwMatrix* matrix, const RwV3d* axis, RwReal angle, RwOpCombineType combineOp);
extern RwMatrix* (*RwMatrixTranslate)(RwMatrix* matrix, const RwV3d* translation, RwOpCombineType combineOp);

extern void (*RegisterCorona)(unsigned int id, void* attachTo, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, CVector const& posn, float radius, float farClip, int coronaType, int flaretype, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, unsigned char fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);

static std::list<std::pair<unsigned int *, unsigned int>> resetEntries;

int Vehicle::m_LightIdOffset = 1000;

int LightIdOffset = 10000;

Vehicle::Vehicle(int hVehicle, int modelId)
{
    this->hVehicle = hVehicle;
    this->modelId = modelId;
    pVehicle = (CVehicle*)GetVehicleFromRef(hVehicle);

    this->sirenSystem = new SirenSystem(hVehicle);

    std::string vehicleIdString = "Vehicle " + std::to_string(hVehicle) + ": ";

    Log::Level(LOG_LEVEL::LOG_BOTH) << vehicleIdString << "Created" << std::endl;

    //removing this update will case another issue SOME FUCKING WHERE ELSE I DON'T REMEMBER
    //Update(0);
    

    /*
    auto lightGroupDataList = GetLightGroupsData();
    for (auto lightGroupData : lightGroupDataList)
    {
        if(lightGroupData->lightGroup->enableOnSpawn)
            lightGroupData->lightsOn = true;
    }
    */
}

Vehicle::~Vehicle()
{
    Destroy();
}

void Vehicle::Init()
{
    this->sirenSystem->Init();
}

void Vehicle::Destroy()
{
    std::string vehicleIdString = "Vehicle " + std::to_string(hVehicle) + ": ";

    Log::Level(LOG_LEVEL::LOG_BOTH) << vehicleIdString << "Destroy" << std::endl;

    this->sirenSystem->Destroy();
}

void Vehicle::Update(int dt)
{
    std::string vehicleIdString = "Vehicle " + std::to_string(hVehicle) + ": ";

    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << vehicleIdString << "Update" << std::endl;

    ledsTime += dt;
    if(ledsTime >= 2000) ledsTime = 0;

    this->sirenSystem->Update(dt);

    UpdateLightGroups(dt);

    //-----------

    //Log::Level(LOG_LEVEL::LOG_BOTH) << vehicleIdString << "Getting siren state: " << std::endl;

    //void* vehicleEntity = Mod::ModGetVehicleFromRef(hVehicle);
    //bool sirenOn = *(uint8_t*)((uintptr_t)vehicleEntity + 0x42D + 4) >> 7;

    //Log::Level(LOG_LEVEL::LOG_BOTH) << vehicleIdString << "siren state: " << sirenOn << std::endl;

    //if (prevLightsState != gameSirenState)
    if (prevSirenState != gameSirenState)
    {
        prevSirenState = gameSirenState;

        if (pDriver)
        {
            if(Globals::hPlayerVehicle == hVehicle)
            {
                //player vehicle
                if(ModConfig::TurnOnLightsWithSiren)
                {
                    SirenSystem::PlaySirenToggleSound();
                    
                    SetGiroflexEnabled(gameSirenState);
                }
            } else {
                //npc vehicle
                SetGiroflexEnabled(gameSirenState);
            }
        }

        if(!Globals::m_UsingMultiSiren)
        {
            //siren audio
            if(SirenSystem::ModelIdHasSirenGroup(modelId))
                sirenSystem->ToggleSiren(gameSirenState);
        }
    }

    //Log::Level(LOG_LEVEL::LOG_BOTH) << vehicleIdString << "Update end" << std::endl;
}

void Vehicle::UpdateLightGroups(int dt)
{
    std::string vehicleIdString = "Vehicle " + std::to_string(hVehicle) + ": ";
    
    if (!ModelInfos::HasModelInfo(modelId)) return;

    auto modelInfo = ModelInfos::GetModelInfo(modelId);

    auto lightId = hVehicle + Vehicle::m_LightIdOffset;

    for (auto lightGroup : modelInfo->lightGroups)
    {
        if (lightGroup->points.size() == 0)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << vehicleIdString << "Warning: lightGroup (from id: " << modelInfo->modelId << ") has 0 points" << std::endl;
            continue;
        }

        //

        if (!LightGroupDatas::HasLightGroupData(lightGroup, hVehicle))
        {   
            std::vector<Pattern*> compatiblePatterns = lightGroup->GetPatterns();
            
            if(compatiblePatterns.size() == 0)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << vehicleIdString << "Warning: lightGroup (from id: " << modelInfo->modelId << ") has 0 compatible patterns" << std::endl;
                continue;
            }

            LightGroupData* lightGroupData = new LightGroupData(lightGroup, hVehicle);

            for (auto pattern : compatiblePatterns)
                lightGroupData->patterns.push_back(pattern);
           
            LightGroupDatas::AddLightGroupData(lightGroupData);

            Log::Level(LOG_LEVEL::LOG_BOTH) << vehicleIdString << "Created LightGroupData with " << lightGroupData->patterns.size() << " patterns [global: " << LightGroupDatas::m_LightGroupDatas.size() << "]" << std::endl;
        }

        //if (!lightsOn) continue;

        LightGroupData* lightGroupData = LightGroupDatas::GetLightGroupData(lightGroup, hVehicle);

        if (!lightGroupData)
        {
            continue;
        }

        //

        if (lightGroupData->patternLoop->HasNoSteps())
        {
            for (auto pattern : lightGroupData->patterns)
            {
                std::cout << pattern << std::endl;

                lightGroupData->patternLoop->AddStep(&LightGroupDatas::m_ChangePatternTime);
            }

            int patternOffset = lightGroup->patternOffset;
            while (patternOffset > 0)
            {
                lightGroupData->patternLoop->StepIndex++;
                if (lightGroupData->patternLoop->StepIndex >= lightGroupData->patternLoop->Steps.size()) lightGroupData->patternLoop->StepIndex = 0;
                patternOffset--;
            }

        }

        //

        //if (!lightsPaused)
        {
            lightGroupData->patternLoop->Update(dt);

            if (lightGroupData->patternLoop->HasStepChanged())
            {
                //std::cout << "patternLoop step changed" << std::endl;
                lightGroupData->stepLoop->Clear();
            }

            //

            Pattern* pattern = lightGroupData->GetCurrentPattern();

            if (pattern == NULL)
            {
                //std::cout << "Pattern not found" << std::endl;
                continue;
            }

            if (pattern->steps.size() == 0)
            {
                //std::cout << "Pattern has no steps" << std::endl;
                continue;
            }

            if (lightGroupData->stepLoop->HasNoSteps())
            {
                Pattern* pattern = lightGroupData->GetCurrentPattern();

                //std::cout << "add " << pattern->steps.size() << " steps to stepLoop" << std::endl;

                for (auto step : pattern->steps)
                {
                    lightGroupData->stepLoop->AddStep(&step->duration);
                }
            }

            lightGroupData->stepLoop->Update(dt);
        }

        //

        if(lightGroup->rotate)
        {
            lightGroup->rotateAngle += 0.001f * dt * lightGroup->rotateSpeed;   
            if(lightGroup->rotateAngle >= 2 * M_PI) lightGroup->rotateAngle = 0;

            //Log::Level(LOG_LEVEL::LOG_BOTH) << "rotateAngle: " << lightGroup->rotateAngle << std::endl;
        }
        //

        for (int i = 0; i < (int)lightGroup->points.size(); i++)
        {
            auto point = lightGroup->points[i];

            auto amountOfPoints = lightGroup->points.size();
            auto distance = lightGroup->distance;
            auto curve = lightGroup->curve;

            //position
            float x = (i * distance) - ((amountOfPoints - 1) * distance / 2);
			float y = (float)arch_fn_parabola((float)i, curve, (float)(amountOfPoints - 1));

            //rotate position
            if(lightGroup->rotate)
            {
                auto angle = lightGroup->rotateAngle;

                x += std::sin(angle) * lightGroup->rotateDistance * (lightGroup->rotateInverse ? -1 : 1);
                y += std::cos(angle) * lightGroup->rotateDistance;
            }

            auto coronaOffset = CVector(x, y, 0) + lightGroup->offset + point->customOffset;

            //color
            CRGBA color = lightGroup->color1;
            if((double)i < ((double)amountOfPoints)/(double)2)
            {
                color = lightGroup->color1;

                if(amountOfPoints%2 == 1 && i == std::round(amountOfPoints/2) && amountOfPoints > 2)            
                {
                    color = lightGroup->color3;
                }
            } else {
                color = lightGroup->color2;
            }

            //pattern color
            auto step = lightGroupData->GetCurrenetStep();
            if(step->useCustomColor)
            {
                color = step->customColor;
            }

            //custom color
            if(point->useCustomColor)
            {
                color = point->customColor;
            }
            
            //
            int index = i;

            bool enabled = lightGroupData->GetPointIsEnabled(point, index);
            
            if (lightGroup->freezeLights) enabled = true;

            if (!lightGroupData->lightsOn && !lightGroup->alwaysEnabled) enabled = false;

            if (WindowEditing::FreezeLights) enabled = true;

            if (WindowEditing::ShowCurrentEditingLightGroup)
            {
                if (WindowEditing::LightGroupToShow != lightGroup) enabled = false;
            }

            //menuVSL->debug->m_Visible = true;

            auto radius = lightGroup->radius;

            //auto positionY = lightGroup->offset.y + point->customOffset.y;
            
            //---------------------------------------
            // get angle and direction
            // and I hate this, cuz its hard, or im just too stupid for this, idk

            auto vehiclePos = pVehicle->m_matrix->pos;
            CVector cameraPos = camera->m_matrix->pos;
            float coronaOffsetX = coronaOffset.x;

            auto vec1 = TransformFromObjectSpace(pVehicle, CVector(-coronaOffsetX, 0, 0));
            vec1.z = vehiclePos.z;

            auto vec2 = vehiclePos;

            auto vec3 = cameraPos;
            vec3.z = vehiclePos.z;

            auto vec1_2d = CVector2D(vec1.x, vec1.y);
            auto vec2_2d = CVector2D(vec2.x, vec2.y);
            auto vec3_2d = CVector2D(vec3.x, vec3.y);

            auto angle = calculateAngleVec2D(vec1_2d, vec2_2d, vec3_2d);
            
            if(hVehicle == Globals::hPlayerVehicle)
            {
                //menuVSL->debug->m_Visible = true;
                //menuVSL->debug->AddLine("angle: " + std::to_string(angle));
                //menuVSL->debug->AddLine("x: " + std::to_string(coronaOffsetX));
            }

            eSirenDirection direction = lightGroup->direction;

            float radiusMult = 1.0f;
            bool isCoronaAtLeft = coronaOffsetX < 0;
            bool isCoronaAtRight = !isCoronaAtLeft;

            if(direction == eSirenDirection::SIDES)
            {
                //bool isHidden = (angle < 270.0f && angle > 90.0f);
                bool isHidden = (angle < 280.0f && angle > 80.0f);

                radiusMult = isHidden ? 0.0f : 1.0f;
            }

            if(direction == eSirenDirection::FRONT || direction == eSirenDirection::BACK)
            {
                auto newAngle = angle;
                if(isCoronaAtRight) newAngle -= 180;
                if (newAngle < 0) newAngle += 360.0f;

                bool visibleBack = (newAngle >= 0 && newAngle < 180.0f);
                bool visibleFront = !visibleBack;

                if(direction == eSirenDirection::FRONT)
                    radiusMult = visibleFront ? 1.0f : 0.0f;

                if(direction == eSirenDirection::BACK)
                    radiusMult = visibleBack ? 1.0f : 0.0f;
            }

            radius = lightGroup->radius * radiusMult;

            //---------------------------------------

            if(color.a == 0) enabled = false;
            
            RenderCorona corona;
            corona.car = hVehicle;
            corona.pVehicle = pVehicle;
            corona.id = lightId++;
            corona.color = color;
            corona.offset = coronaOffset;
            corona.radius = enabled ? radius : 0.0f;
            corona.renderShadow = enabled ? lightGroup->renderShadow : false;
            corona.renderPointLight = enabled ? lightGroup->renderPointLight : false;

            corona.shadowIntensity = lightGroup->shadowIntensity;
            corona.shadowSize = lightGroup->shadowSize;

            //shadow offset X
            //corona.shadowOffsetX = lightGroup->shadowPositionX;
            auto dir = (coronaOffset.x > 0) ? 1 : -1;
            float margin = 0.15f;
            float sdistance = abs(coronaOffset.x);

            corona.shadowOffsetX = coronaOffset.x + (dir * lightGroup->shadowPositionX);
            if (sdistance > margin)
            {
                //corona.shadowOffsetX += (dir * lightGroup->shadowSize / 2);
            }

            if(lightGroup->rotate) corona.shadowOffsetX = lightGroup->shadowPositionX;

            //

            corona.shadowOffsetY = lightGroup->shadowPositionY;
            corona.shadowTexture = lightGroup->shadowTexture;

            corona.shadowRotation = lightGroup->shadowRotation;
            if(lightGroup->rotate) corona.shadowRotation = -lightGroup->rotateAngle *  (lightGroup->rotateInverse ? -1 : 1);

            corona.shadowFlipTextures = lightGroup->shadowFlipTextures;

            corona.pointLightDistance = lightGroup->pointLightDistance;
            corona.pointLightIntensity = lightGroup->pointLightIntensity;

            corona.nearClip = lightGroup->nearClip;
            corona.useFlare = enabled ? lightGroup->useFlare : false;
            corona.flareIntensity = lightGroup->flareIntensity;
            corona.flareDistance = lightGroup->flareDistance;
            corona.flareTexture = lightGroup->flareTexture;
            corona.coronaTexture = lightGroup->coronaTexture;
            Vehicles::AddCoronaToRender(corona);

            if(corona.useFlare) lightId++;

            if (lightGroup->useSmallWhiteCorona)
            {
                RenderCorona corona2;
                corona2.car = hVehicle;
                corona2.pVehicle = pVehicle;
                corona2.id = lightId++;
                corona2.color = lightGroup->smallWhiteCoronaColor;
                corona2.offset = corona.offset;
                corona2.radius = corona.radius * lightGroup->smallWhiteCoronaScale;
                corona2.nearClip = corona.nearClip;
                corona2.coronaTexture = lightGroup->smallWhiteCoronaTexture;
                Vehicles::AddCoronaToRender(corona2);
            }
        }
    }
}

void Vehicle::OnUpdateGameLogic()
{   
   
}

void Vehicle::RenderBefore()
{
    if (!ModelInfos::HasModelInfo(modelId)) return;

    auto modelInfo = ModelInfos::GetModelInfo(modelId);

    auto atomics = VehicleDummy::RpClumpGetAllAtomics(pVehicle->m_pRwClump);
	for (auto atomic : atomics)
	{
		if (!atomic->geometry) continue;

		atomic->geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;

		auto frameAtomic = GetObjectParent((RwObject*)atomic);
		auto name = VehicleDummy::GetFrameName(frameAtomic);
		//CVector position = m_Vehicle->TransformFromObjectSpace(GetFrameNodePosition(frameAtomic));

        //Log::Level(LOG_LEVEL::LOG_BOTH) << "name: " << name << std::endl;

        //rotate objects
        for (auto lightGroup : modelInfo->lightGroups)
        {
            LightGroupData* lightGroupData = LightGroupDatas::GetLightGroupData(lightGroup, hVehicle);

            if (!lightGroupData)
            {
                continue;
            }

            for (int i = 0; i < (int)lightGroup->points.size(); i++)
            {
                auto point = lightGroup->points[i];
                auto index = i;

                if(to_lower(point->rotateObject.object) != to_lower(name))
                {
                    continue;
                }

                //
                bool enabled = lightGroupData->GetPointIsEnabled(point, index);
                
                if (lightGroup->freezeLights) enabled = true;

                if (!lightGroupData->lightsOn && !lightGroup->alwaysEnabled) enabled = false;

                if (WindowEditing::FreezeLights) enabled = true;

                if (WindowEditing::ShowCurrentEditingLightGroup)
                {
                    if (WindowEditing::LightGroupToShow != lightGroup) enabled = false;
                }

                //

                if(!point->rotateObject.rotateAlways)
                {
                    if(!lightGroupData->lightsOn) continue;
                }

                auto axisVal = point->rotateObject.axis;

                RwV3d axis = {
                    (float)(axisVal == eRotateObjectAxis::X ? 1 : 0),
                    (float)(axisVal == eRotateObjectAxis::Y ? 1 : 0),
                    (float)(axisVal == eRotateObjectAxis::Z ? 1 : 0)
                };
                RwReal angle = point->rotateObject.speed;

                RwMatrixRotate(&frameAtomic->modelling, &axis, angle, rwCOMBINEPRECONCAT);
            }
        }

        //leds
        for (auto lightGroup : modelInfo->lightGroups)
        {
            if(!lightGroup->useLightbarLEDs && !lightGroup->useNormalLEDs) continue;

            LightGroupData* lightGroupData = LightGroupDatas::GetLightGroupData(lightGroup, hVehicle);

            if (!lightGroupData)
            {
                continue;
            }

            for (int i = 0; i < (int)lightGroup->points.size(); i++)
            {
                auto point = lightGroup->points[i];
                auto amountOfPoints = lightGroup->points.size();

                if(lightGroup->useLightbarLEDs)
                {
                    int ledIndex = i + lightGroup->lightbarLEDStartIndex;

                    if (to_lower(name).compare(to_lower("lightbar-led-" + std::to_string(ledIndex))) != 0) continue;
                }

                if(lightGroup->useNormalLEDs)
                {
                    int ledIndex = i + lightGroup->normalLEDStartIndex;

                    if (to_lower(name).compare(to_lower("led-" + std::to_string(ledIndex))) != 0) continue;
                }

                //color
                CRGBA color = lightGroup->ledColor1Enabled;
                if((double)i < ((double)amountOfPoints)/(double)2)
                {
                    color = lightGroup->ledColor1Enabled;

                    if(amountOfPoints%2 == 1 && i == std::round(amountOfPoints/2) && amountOfPoints > 2)            
                    {
                        color = lightGroup->ledColor3Enabled;
                    }
                } else {
                    color = lightGroup->ledColor2Enabled;
                }

                //pattern color
                auto step = lightGroupData->GetCurrenetStep();
                if(step->useCustomLedColor)
                {
                    color = step->customLedColor;
                }
               
                //
                int index = i;

                bool enabled = lightGroupData->GetPointIsEnabled(point, index);
                
                if (lightGroup->freezeLights) enabled = true;

                if (!lightGroupData->lightsOn && !lightGroup->alwaysEnabled) enabled = false;

                if (WindowEditing::FreezeLights) enabled = true;

                if (WindowEditing::ShowCurrentEditingLightGroup)
                {
                    if (WindowEditing::LightGroupToShow != lightGroup) enabled = false;
                }

                if (!enabled)
				{
					color = lightGroup->ledColorDisabled;
				}

                auto materials = VehicleDummy::RpGeometryGetAllMaterials(atomic->geometry);
				for (auto material : materials)
				{
					//if (!material) continue;

					material->color = { color.r, color.g, color.b, color.a };
					
					material->surfaceProps.ambient = 10;
					material->surfaceProps.diffuse = 10;
					material->surfaceProps.specular = 10;
				}
            }
        }
    }
}

void Vehicle::RenderAfter()
{
    for (auto &p : resetEntries)
    *p.first = p.second;

    resetEntries.clear();
}

std::vector<LightGroupData*> Vehicle::GetLightGroupsData()
{
	std::vector<LightGroupData*> lightGroupDataList;

	if (!ModelInfos::HasModelInfo(this->modelId)) return lightGroupDataList;

	auto modelInfo = ModelInfos::GetModelInfo(this->modelId);

	for (auto lightGroup : modelInfo->lightGroups)
	{
        auto lightGroupData = LightGroupDatas::GetLightGroupData(lightGroup, this->hVehicle);

        if(lightGroupData)
            lightGroupDataList.push_back(lightGroupData);
	}

	return lightGroupDataList;
}

void Vehicle::SetGiroflexEnabled(bool enabled, bool forceOn)
{
    prevLightsState = enabled;
    //lightsPaused = !enabled;
    //lightsOn = enabled;

    auto lightGroupDataList = GetLightGroupsData();
    for (auto lightGroupData : lightGroupDataList)
    {
        if(!forceOn && !lightGroupData->lightsOn && !lightGroupData->lightGroup->enableWithLights)
        {
            continue;
        }

        lightGroupData->lightsOn = enabled;
    }

	/*
	std::cout << "lightGroupDataList " << lightGroupDataList.size() << std::endl;

	for (auto lightGroupData : lightGroupDataList)
	{
		lightGroupData->stepLoop->DontChangeSteps = !enabled;
	}
	*/
}

void Vehicle::ResetObjectRotation(std::string object)
{
	auto frame = VehicleDummy::FindDummy(pVehicle, object);

	if (!frame) return;

	RwV3d axis = {0, 0, 0};
	RwReal angle = 0.0f;
	RwV3d pos = (RwV3d)frame->modelling.pos;
	
	RwMatrixRotate(&frame->modelling, &axis, angle, rwCOMBINEREPLACE);
	RwMatrixTranslate(&frame->modelling, &pos, rwCOMBINEREPLACE);
}

void Vehicle::RegisterTestCorona(int lightId, CVector position, CRGBA color, float radius)
{
    RegisterCorona(
        lightId++,
        NULL,
        color.r,
        color.g,
        color.b,
        255,
        { position.x, position.y, position.z },
        radius,
        1000.0f,
        0,
        0,
        true,
        false,
        0,
        0.0f,
        false,
        0.0f,
        0,
        10000.0f,
        false,
        false
    );
}