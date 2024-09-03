#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include "utils.h"
#include "Log.h"

#include <dlfcn.h>

#include "Mod.h"
#include "VehicleDummy.h"

// CLEO 2.0.1.5
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// Script Commands
#include "IScriptCommands.h"
IScriptCommands* scriptCommands = NULL;

// Menu VSL
#include "menu/IMenuVSL.h"
IMenuVSL* menuVSL = NULL;

// --------------------------------

MYMODCFG(net.danilo1301.giroflexVSL, GiroflexVSL, 1.0.0, Danilo1301)

// --------------------------------

//rw
RwMatrix* (*RwMatrixCreate)(void);
RwBool (*RwMatrixDestroy)(RwMatrix* mpMat);
RwMatrix* (*RwMatrixTransform)(RwMatrix* matrix, const RwMatrix* transform, RwOpCombineType combineOp);
RwMatrix* (*RwMatrixTranslate)(RwMatrix* matrix, const RwV3d* translation, RwOpCombineType combineOp);
RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);

char* (*GetFrameNodeName)(RwFrame* frame);

void (*RegisterCorona)(unsigned int id, void* attachTo, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, CVector const& posn, float radius, float farClip, int coronaType, int flaretype, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, unsigned char fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);
int (*GetVehicleRef)(int);
void* (*GetVehicleFromRef)(int);

//test (doesnt work)
bool (*CTouchInterface_IsTouchDown)(int param_1);

// --------------------------------

extern "C" void OnModPreLoad()
{
    char logPath[512];
	sprintf(logPath, "%s/giroflexVSL/", aml->GetConfigPath());

    CreateFolder(logPath);

    Log::Open(logPath, "giroflexVSL", true);

    Log::Level(eLogLevel::LOG_BOTH) << "Preload()" << std::endl;
    Log::Level(eLogLevel::LOG_BOTH) << "AML headers: 1.0.3.1" << std::endl;
    Log::Level(eLogLevel::LOG_BOTH) << "Test #1" << std::endl;

    logger->SetTag("Giroflex VSL");

    logger->Info("Preload");

    Log::Level(eLogLevel::LOG_BOTH) << "Preload() END" << std::endl;
}

extern "C" void OnModLoad()
{
    Log::Level(eLogLevel::LOG_BOTH) << "Load()" << std::endl;

    /*
    cfg->Bind("Author", "", "About")->SetString("Danilo1301"); cfg->ClearLast();
    cfg->Bind("Discord", "", "About")->SetString("https://discord.gg/mkCDRf4zJA"); cfg->ClearLast();
    cfg->Bind("GitHub", "", "About")->SetString("https://github.com/Danilo1301"); cfg->ClearLast();
    cfg->Save();
    */

    Log::Level(eLogLevel::LOG_BOTH) << "Loading interfaces..." << std::endl;

    LoadInterface(&cleo, "CLEO");
    LoadInterface(&scriptCommands, "ScriptCommands");
    LoadInterface(&menuVSL, "MenuVSL");

    if(!cleo)
    {
        Log::Level(eLogLevel::LOG_BOTH) << "CLEO was not loaded" << std::endl;
        return;
    }

    if(!scriptCommands)
    {
        Log::Level(eLogLevel::LOG_BOTH) << "ScriptCommands was not loaded" << std::endl;
        return;
    }

    if(!menuVSL)
    {
        Log::Level(eLogLevel::LOG_BOTH) << "MenuVSL was not loaded" << std::endl;
        return;
    }

    Log::Level(eLogLevel::LOG_BOTH) << "Loading symbols..." << std::endl;

    void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t gameAddr = (uintptr_t)(cleo->GetMainLibraryLoadAddress());

    //rw
    SET_TO(RwMatrixCreate, aml->GetSym(hGTASA, "_Z14RwMatrixCreatev"));
    SET_TO(RwMatrixDestroy, aml->GetSym(hGTASA, "_Z15RwMatrixDestroyP11RwMatrixTag"));
    SET_TO(RwMatrixTransform, aml->GetSym(hGTASA, "_Z17RwMatrixTransformP11RwMatrixTagPKS_15RwOpCombineType"));
    SET_TO(RwMatrixTranslate, aml->GetSym(hGTASA, "_Z17RwMatrixTranslateP11RwMatrixTagPK5RwV3d15RwOpCombineType"));
    SET_TO(RpClumpForAllAtomics, aml->GetSym(hGTASA, "_Z20RpClumpForAllAtomicsP7RpClumpPFP8RpAtomicS2_PvES3_"));
    SET_TO(RpGeometryForAllMaterials, aml->GetSym(hGTASA, "_Z25RpGeometryForAllMaterialsP10RpGeometryPFP10RpMaterialS2_PvES3_"));

    SET_TO(GetFrameNodeName, aml->GetSym(hGTASA, "_Z16GetFrameNodeNameP7RwFrame"));

    SET_TO(RegisterCorona, aml->GetSym(hGTASA, "_ZN8CCoronas14RegisterCoronaEjP7CEntityhhhhRK7CVectorffhhhhhfbfbfbb"));
    SET_TO(GetVehicleRef, aml->GetSym(hGTASA, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(GetVehicleFromRef, aml->GetSym(hGTASA, "_ZN6CPools10GetVehicleEi"));

    //test
    SET_TO(CTouchInterface_IsTouchDown, aml->GetSym(hGTASA, "_ZN15CTouchInterface11IsTouchDownEi"));

    Log::Level(eLogLevel::LOG_BOTH) << "Loading MenuVSL functions..." << std::endl;

    menuVSL->AddOnProcessScripts([] () {
        Mod::Update();
    });

    menuVSL->AddOnRender([] () {
        Mod::Render();
    });
    
    Mod::Init();
    
    /*
    menuVSL->AddOnVehicleRenderBefore([] (void* vehicle) {
        return;

        Log::Level(eLogLevel::LOG_BOTH) << "Render before works" << std::endl;

        int lightId = reinterpret_cast<int>(vehicle) + 420;

        RegisterCorona(
            lightId,
            vehicle,
            255,
            0,
            0,
            255,
            {0, 0, 4},
            1.0f,
            1000.0f,
            0,
            0,
            false,
            false,
            0,
            0.0f,
            false,
            0.0f,
            0,
            10.0f,
            false,
            false
        );
    });
    */

    Log::Level(eLogLevel::LOG_BOTH) << "Load() END" << std::endl;
}