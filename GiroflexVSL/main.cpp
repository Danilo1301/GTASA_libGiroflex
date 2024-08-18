#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include "Log.h"
#include "Mod.h"
#include "ModelInfos.h"
#include "Patterns.h"
#include "ModConfig.h"
#include "audiosystem.h"

#include "windows/WindowSettings.h"

#include "opcodes.h"

// ---------------------------------------

//MYMODCFG(net.danilo1301.giroflexVSL, GiroflexVSL, Mod::m_Version, Danilo1301) //whoops
MYMODCFG(net.danilo1301.giroflexVSL, GiroflexVSL, 3.7.3, Danilo1301)

// ---------------------------------------

// CLEO 2.0.1.3
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// BASS
IBASS* BASS = NULL;

// SAUtils 1.6 (but it says 1.5.2)
#include "isautils.h"
ISAUtils* sautils = NULL;

#include "audiosystem.h"
static CSoundSystem soundsysLocal;
CSoundSystem* soundsys = &soundsysLocal;

#include "IModPolicia.h"
IModPolicia* modPolicia = NULL;

#include "menu/IMenuVSL.h"
IMenuVSL* menuVSL = NULL;

#include "IMultiSiren.h"
IMultiSiren* multiSiren = NULL;

#include "GiroflexVSL.h"

// ---------------------------------------

CVector2D *m_vecCachedPos;
uintptr_t* pVehiclePool = 0;

int (*ScreenGetWidth)();
int (*ScreenGetHeight)();
int (*GetVehicleRef)(int);
void* (*GetVehicleFromRef)(int);
void (*RegisterCorona)(unsigned int id, void* attachTo, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, CVector const& posn, float radius, float farClip, int coronaType, int flaretype, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, unsigned char fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);

RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);
char* (*GetFrameNodeName)(RwFrame* frame);
RwRaster* (*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwRaster* (*RwRasterUnlock)(RwRaster* raster);
RwBool (*RwTextureSetMipmapping)(RwBool enable);
RwBool (*RwTextureSetAutoMipmapping)(RwBool enable);
RwTexture* (*RwTextureCreate)(RwRaster* raster);
RwUInt8* (*RwRasterLock)(RwRaster* raster, RwUInt8 level, RwInt32 lockMode);
RwMatrix* (*RwMatrixRotate)(RwMatrix* matrix, const RwV3d* axis, RwReal angle, RwOpCombineType combineOp);
RwMatrix* (*RwMatrixTranslate)(RwMatrix* matrix, const RwV3d* translation, RwOpCombineType combineOp);
RwReal (*RwV3dNormalize)(RwV3d* out, const RwV3d* in);
void (*CMatrix_CopyToRwMatrix)(CMatrix*, RwMatrix *matrix);
RwMatrix* (*RwMatrixCreate)(void);
RwBool (*RwMatrixDestroy)(RwMatrix* mpMat);

void (*CFont_PrintString)(float x, float y, unsigned short* text);
void (*AsciiToGxtChar)(const char* txt, unsigned short* ret);
void (*RenderFontBuffer)(void);
void (*CSprite2d_DrawRect)(CRect  const& posn, CRGBA  const& color);
void (*DrawSprite)(CSprite2d*, CRect const&, CRGBA const&);

CCamera* camera;
bool* userPaused;
bool* codePaused;
int nGameLoaded = -1;
// ---------------------------------------

ConfigEntry* cfgMenuOffsetX = NULL;
ConfigEntry* cfgTimeBetweenPatterns = NULL;

void SaveCfg()
{
    cfgMenuOffsetX->SetInt((int)Menu::m_MenuOffset.x);
    cfgTimeBetweenPatterns->SetInt(Patterns::m_TimeBetweenPatterns);

    cfg->Save();
}

//

DECL_HOOK(void*, UpdateGameLogic, uintptr_t a1)
{
    if(BASS) {
        soundsys->Update();
    }

    for(auto p : Vehicles::m_Vehicles)
    {
        auto vehicle = p.second;

        vehicle->OnUpdateGameLogic();
    }

    return UpdateGameLogic(a1);
}

DECL_HOOKv(RenderVehicle, void* self)
{
    CVehicle* vehicle = (CVehicle*)self;

    Vehicles::RenderBefore(vehicle);
    RenderVehicle(self);
    Vehicles::RenderAfter(vehicle);
}

#include "sdk/Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb/stb_image_write.h"

bool CreateImageFromFile(std::string const& path, Image*& img) {
    int32_t w, h, c;
    uint8_t* p = stbi_load(path.c_str(), &w, &h, &c, 4);

    if (p) {
        if (!img)
            img = new Image();

        img->width = w;
        img->height = h;
        img->channels = c;
        img->pixels = p;

        return true;
    }

    return false;
}

RwTexture* LoadSpriteFromFolder(std::string const& file) {
    Image* img = nullptr;
    if (CreateImageFromFile(file, img)) {
        uint32_t w = img->width;
        uint32_t h = img->height;
        uint8_t* p = img->pixels;

        bool mipMap = false;

        RwRaster* raster = RwRasterCreate(w, h, 0, rwRASTERTYPETEXTURE | rwRASTERFORMAT8888 | (mipMap ? rwRASTERFORMATMIPMAP | rwRASTERFORMATAUTOMIPMAP : 0));
        RwUInt32* pixels = (RwUInt32*)RwRasterLock(raster, 0, rwRASTERLOCKWRITE);

        for (uint32_t i = 0; i < w * h * 4; i += 4) {
            uint8_t r = p[i + 2];
            uint8_t g = p[i + 1];
            uint8_t b = p[i];

            p[i + 2] = b;
            p[i + 1] = g;
            p[i] = r;
        }

        memcpy(pixels, p, w * h * 4);
        RwRasterUnlock(raster);

        auto tex = RwTextureCreate(raster);
        std::string newName = "test";
        //std::string fileNoExt = RemovePath(file);
        //fileNoExt = RemoveExtension(fileNoExt);

        if (mipMap)
            RwTextureSetFilterMode(tex, rwFILTERMIPLINEAR);
        else
            RwTextureSetFilterMode(tex, rwFILTERLINEAR);

        RwTextureSetMipmapping(mipMap);
        RwTextureSetAutoMipmapping(mipMap);
        memset(tex->name, 0, 32);
        newName.copy(tex->name, 32);

        //spritesMap.insert({ fileNoExt, tex });
        //spritesMapIndex.insert({ Index++, tex });

        img->Release();

        return tex;
    }

    return nullptr;
}

char text[24];
unsigned short* textGxt = new unsigned short[0xFF];

CSprite2d sprite;

DECL_HOOKv(CHud_Draw, void* self)
{
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "CHud_Draw" << std::endl;

    CHud_Draw(self);

    /*
    CSprite2d_DrawRect(CRect(300, 300, 400, 400), CRGBA(0, 0, 255));

    if (!sprite.m_pTexture)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading texture" << std::endl;
        char path[512];
	    sprintf(path, "%s/giroflexVSL/test.png", aml->GetConfigPath());
        Log::Level(LOG_LEVEL::LOG_BOTH) << "path: " << path << std::endl;

        sprite.m_pTexture = LoadSpriteFromFolder(path);
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Texture: " << sprite.m_pTexture << std::endl;
    }

    DrawSprite(&sprite, CRect(300, 300, 350, 350), CRGBA(255, 255, 255, 255));

    sprintf(text, "%dm", 52183);

    AsciiToGxtChar(text, textGxt);

    CFont_PrintString(300, 300, textGxt);
    RenderFontBuffer();
    */
}


//

std::string CheckModVersion(std::vector<std::string> GUIDs, std::vector<std::string> versions)
{
    for (int ii = GUIDs.size() - 1; ii >= 0; ii--)
    {
        auto GUID = GUIDs[ii];

        for (int i = versions.size() - 1; i >= 0; i--)
        {
            std::string version = versions[i];

            Log::Level(LOG_LEVEL::LOG_BOTH) << "Checking " << GUID << " " << version << "..." << std::endl;

            if (aml->HasModOfVersion(GUID.c_str(), version.c_str()))
                return version;
        }
    }
    return "";
}

//---------------------------------------------------------------------------------------------------
//test

const char* optionsGiroflexEditMode[] = {
    "OFF",
    "ON"
};
void OnGiroflexEditModeChanged(int oldVal, int newVal, void* data)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "OnGiroflexEditModeChanged - changed to " << newVal << std::endl;

    if (newVal == 1)
    {
        WindowSettings::ToggleEditScreenPos(true);
    }
    else {
        WindowSettings::ToggleEditScreenPos(false);

        SaveCfg();
    }
}

//---------------------------------------------------------------------------------------------------

void LoadSymbols()
{
    //void* hGTASA = aml->GetLibHandle("libGTASA.so"); crashes the game
    //void* pGameHandle = aml->GetLibHandle("libGTASA.so");

    void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t gameAddr = (uintptr_t)(cleo->GetMainLibraryLoadAddress());

    Log::Level(LOG_LEVEL::LOG_BOTH) << "hGTASA: " << hGTASA << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Getting Syms 1..." << std::endl;

    SET_TO(camera, cleo->GetMainLibrarySymbol("TheCamera"));
    SET_TO(userPaused, cleo->GetMainLibrarySymbol("_ZN6CTimer11m_UserPauseE"));
    SET_TO(codePaused, cleo->GetMainLibrarySymbol("_ZN6CTimer11m_CodePauseE"));

    if((uintptr_t)camera == gameAddr + 0x951FA8) nGameLoaded = 0; // SA 2.00
    else if((uintptr_t)camera == gameAddr + 0x595420) nGameLoaded = 1; // VC 1.09
    else
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "The loaded game is not GTA:SA v2.00 or GTA:VC v1.09. Aborting..." << std::endl;
        return;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Getting Syms 2..." << std::endl;

    SET_TO(m_vecCachedPos, aml->GetSym(hGTASA, "_ZN15CTouchInterface14m_vecCachedPosE"));
    SET_TO(pVehiclePool, aml->GetSym(hGTASA, "_ZN6CPools15ms_pVehiclePoolE"));

    SET_TO(ScreenGetWidth, aml->GetSym(hGTASA, "_Z17OS_ScreenGetWidthv"));
    SET_TO(ScreenGetHeight, aml->GetSym(hGTASA, "_Z18OS_ScreenGetHeightv"));
    SET_TO(GetVehicleRef, aml->GetSym(hGTASA, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(GetVehicleFromRef, aml->GetSym(hGTASA, "_ZN6CPools10GetVehicleEi"));
    SET_TO(RegisterCorona, aml->GetSym(hGTASA, "_ZN8CCoronas14RegisterCoronaEjP7CEntityhhhhRK7CVectorffhhhhhfbfbfbb"));
    
    SET_TO(RpClumpForAllAtomics, aml->GetSym(hGTASA, "_Z20RpClumpForAllAtomicsP7RpClumpPFP8RpAtomicS2_PvES3_"));
    SET_TO(RpGeometryForAllMaterials, aml->GetSym(hGTASA, "_Z25RpGeometryForAllMaterialsP10RpGeometryPFP10RpMaterialS2_PvES3_"));
    SET_TO(GetFrameNodeName, aml->GetSym(hGTASA, "_Z16GetFrameNodeNameP7RwFrame"));
    SET_TO(RwRasterCreate, aml->GetSym(hGTASA, "_Z14RwRasterCreateiiii"));
    SET_TO(RwRasterUnlock, aml->GetSym(hGTASA, "_Z14RwRasterUnlockP8RwRaster"));
    SET_TO(RwTextureSetMipmapping, aml->GetSym(hGTASA, "_Z22RwTextureSetMipmappingi"));
    SET_TO(RwTextureSetAutoMipmapping, aml->GetSym(hGTASA, "_Z26RwTextureSetAutoMipmappingi"));
    SET_TO(RwTextureCreate, aml->GetSym(hGTASA, "_Z15RwTextureCreateP8RwRaster"));
    SET_TO(RwRasterLock, aml->GetSym(hGTASA, "_Z12RwRasterLockP8RwRasterhi"));
    SET_TO(RwMatrixRotate, aml->GetSym(hGTASA, "_Z14RwMatrixRotateP11RwMatrixTagPK5RwV3df15RwOpCombineType"));    
    SET_TO(RwMatrixTranslate, aml->GetSym(hGTASA, "_Z17RwMatrixTranslateP11RwMatrixTagPK5RwV3d15RwOpCombineType"));
    SET_TO(RwV3dNormalize, aml->GetSym(hGTASA, "_Z14RwV3dNormalizeP5RwV3dPKS_"));
    SET_TO(CMatrix_CopyToRwMatrix, aml->GetSym(hGTASA, "_ZNK7CMatrix14CopyToRwMatrixEP11RwMatrixTag"));
    SET_TO(RwMatrixCreate, aml->GetSym(hGTASA, "_Z14RwMatrixCreatev"));
    SET_TO(RwMatrixDestroy, aml->GetSym(hGTASA, "_Z15RwMatrixDestroyP11RwMatrixTag"));

    SET_TO(CFont_PrintString, aml->GetSym(hGTASA, "_ZN5CFont11PrintStringEffPt"));
    SET_TO(AsciiToGxtChar, aml->GetSym(hGTASA, "_Z14AsciiToGxtCharPKcPt"));
    SET_TO(RenderFontBuffer, aml->GetSym(hGTASA, "_ZN5CFont16RenderFontBufferEv"));
    SET_TO(CSprite2d_DrawRect, aml->GetSym(hGTASA, "_ZN9CSprite2d8DrawRectERK5CRectRK5CRGBA"));
    SET_TO(DrawSprite, aml->GetSym(hGTASA, "_ZN9CSprite2d4DrawERK5CRectRK5CRGBA"));

    //

    HOOKPLT(UpdateGameLogic, gameAddr + 0x66FE58);

    HOOK(RenderVehicle, aml->GetSym(hGTASA, "_ZN8CVehicle6RenderEv"));
    //HOOK(CHud_Draw, aml->GetSym(hGTASA, "_ZN4CHud4DrawEv"));

    //
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "vecCachedPos: x " << m_vecCachedPos->x << ", y " << m_vecCachedPos->y << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "pVehiclePool: " << pVehiclePool << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ScreenGetWidth: " << ScreenGetWidth() << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ScreenGetHeight: " << ScreenGetHeight() << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "RegisterCorona: " << (void*)RegisterCorona << std::endl;
}

extern "C" void OnModPreLoad()
{
    ModConfig::MakePaths();

    char logPath[512];
	sprintf(logPath, "%s/giroflexVSL/", aml->GetConfigPath());
    Log::Open(logPath, "giroflexVSL");

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Preload()" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "AML headers: 1.1" << std::endl;

    logger->SetTag("GiroflexVSL");

    logger->Info("Preloading");

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Registering interface 'GiroflexVSL'..." << std::endl;

    RegisterInterface("GiroflexVSL", giroflexVSL);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Preload() END" << std::endl;
}

extern "C" void OnModLoad()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load()" << std::endl;

    cfgMenuOffsetX = cfg->Bind("menu_offset_x", -195, "General");
    cfgTimeBetweenPatterns = cfg->Bind("time_between_patterns", Patterns::m_TimeBetweenPatterns, "General");

    Menu::m_MenuOffset.x = (float)cfgMenuOffsetX->GetInt();
    Patterns::m_TimeBetweenPatterns = cfgTimeBetweenPatterns->GetInt();

    cfg->Bind("Author", "", "About")->SetString("Danilo1301"); cfg->ClearLast();
    cfg->Bind("Discord", "", "About")->SetString("https://discord.gg/mkCDRf4zJA"); cfg->ClearLast();
    cfg->Bind("GitHub", "", "About")->SetString("https://github.com/Danilo1301/GTASA_libGiroflexVSL"); cfg->ClearLast();
    //cfg->Save();

    SaveCfg();
    
    //Menu VSL
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading MenuVSL..." << std::endl;
    menuVSL = (IMenuVSL*)GetInterface("MenuVSL");
    if (menuVSL) Log::Level(LOG_LEVEL::LOG_BOTH) << "MenuVSL loaded" << std::endl;
    else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "MenuVSL was not loaded" << std::endl;
        return;
    }

    //Mod Policia
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading ModPolicia..." << std::endl;
    modPolicia = (IModPolicia*)GetInterface("ModPolicia");
    if (modPolicia) Log::Level(LOG_LEVEL::LOG_BOTH) << "ModPolicia loaded" << std::endl;
    else Log::Level(LOG_LEVEL::LOG_BOTH) << "ModPolicia was not loaded" << std::endl;

    //Multi Siren
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading MultiSiren..." << std::endl;
    multiSiren = (IMultiSiren*)GetInterface("MultiSiren");
    if (multiSiren) Log::Level(LOG_LEVEL::LOG_BOTH) << "MultiSiren loaded" << std::endl;
    else Log::Level(LOG_LEVEL::LOG_BOTH) << "MultiSiren was not loaded" << std::endl;

    if(multiSiren)
    {
        Globals::m_UsingMultiSiren = true;
    }

    //CLEO
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading CLEO..." << std::endl;
    cleo = (cleo_ifs_t*)GetInterface("CLEO");
    if (cleo) Log::Level(LOG_LEVEL::LOG_BOTH) << "CLEO loaded" << std::endl;
    else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "CLEO was not loaded" << std::endl;
        return;
    }

    //BASS
    //https://github.com/AndroidModLoader/GTASA_CLEO_AudioStreams
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading BASS..." << std::endl;
    if (!(BASS = (IBASS*)GetInterface("BASS")))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "BASS was not loaded" << std::endl;
    }
    else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "BASS loaded: " << BASS << std::endl;

        soundsys->Init();

        /*
        std::string audiosPath = ModConfig::GetConfigFolder() + "/audios/";

        auto audioStream = new AudioStream(audiosPath + "/funk_raca_negra.mp3");
        audioStream->Loop(true);
        audioStream->Play();
        */

        /*
        auto audioStream = SoundSystem::LoadStream(audiosPath + "/siren1.wav", false);
        audioStream->SetVolume(0.5f);
        audioStream->Loop(false);
        audioStream->Play();
        */

        /*
        auto audioStream = new AudioStream(audiosPath + "/loli_dancando.mp3");
        audioStream->Loop(false);
        audioStream->Play();
        */
    }

    //SAUtils
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading SAUtils..." << std::endl;
    if (!(sautils = (ISAUtils*)GetInterface("SAUtils")))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "SAUtils was not loaded" << std::endl;
    }
    else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "SAUtils loaded" << std::endl;

        sautils->AddClickableItem(eTypeOfSettings::SetType_Mods, "Giroflex VSL - Edit mode", 0, 0, sizeofA(optionsGiroflexEditMode) - 1, optionsGiroflexEditMode, OnGiroflexEditModeChanged);

        if (aml->HasModOfVersion("net.rusjj.gtasa.utils", "1.4.0"))
        {
            /*
            sautils->AddOnRWInitListener([]() {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "RWInit" << std::endl;

                std::string imageFile = std::string(aml->GetConfigPath()) + "/giroflex/textures/1.png";

                Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading texture: " << imageFile << std::endl;

                //its returning 0x0 for some reason
                auto texture = sautils->LoadRwTextureFromPNG(imageFile.c_str());

                Log::Level(LOG_LEVEL::LOG_BOTH) << "Texture: " << texture << std::endl;
            });
            */
        }
        else {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "SAUtils 1.4 or superior not found" << std::endl;
        }
    }

    //

    std::string cleoVersion = CheckModVersion(
        { "net.rusjj.cleolib", "net.rusjj.cleomod" },
        { "2.0.1", "2.0.1.1", "2.0.1.2", "2.0.1.3", "2.0.1.4", "2.0.1.5" }
    );

    std::string sautilsVersion = CheckModVersion(
        { "net.rusjj.gtasa.utils" },
        { "1.1", "1.2", "1.2.1", "1.3.0", "1.3.1", "1.4", "1.4.1", "1.5.1", "1.6 "}
    );
    
    std::string amlVersion = CheckModVersion(
        { "net.rusjj.aml" },
        { "1.0.0.0", "1.0.0.1", "1.0.0.2", "1.0.0.3", "1.0.0.4", "1.0.0.5", "1.0.0.6","1.0.1", "1.0.2", "1.0.2.1", "1.0.2.2", "1.0.3", "1.0.3.1", "1.1", "1.2", "1.2.1", "1.2.2" }
    );

    Log::Level(LOG_LEVEL::LOG_BOTH) << "----------------------------" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Game: " << aml->GetCurrentGame() << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "GiroflexVSL: " << Mod::m_Version << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "CLEO version: " << cleoVersion << " (recommended 2.0.1.3)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "SAUtils version: " << sautilsVersion << " (recommended 1.3.1)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "AML version: " << amlVersion << " (recommended 1.2.2)" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "----------------------------" << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Config: " << aml->GetConfigPath() << std::endl;

    //

    LoadSymbols();

    //
    
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Registering opcodes..." << std::endl;

    __reg_op_func(PROCESS_GIROFLEX_VSL_LIB, PROCESS_GIROFLEX_VSL_LIB);
    __reg_op_func(GET_DRAW_ITEM_INFO, GET_DRAW_ITEM_INFO);
    __reg_op_func(SEND_TOUCH_STATE, SEND_TOUCH_STATE);
    __reg_op_func(SEND_PLAYER_POSITION, SEND_PLAYER_POSITION);
    __reg_op_func(SEND_CURRENT_VEHICLE, SEND_CURRENT_VEHICLE);
    __reg_op_func(GET_MOD_INFO, GET_MOD_INFO);
    __reg_op_func(SEND_CAR_VELOCITY, SEND_CAR_VELOCITY);
    __reg_op_func(CREATE_NEW_VEHICLE, CREATE_NEW_VEHICLE);
    __reg_op_func(REGISTER_GIROFLEX_CORONA, REGISTER_GIROFLEX_CORONA);
    __reg_op_func(SEND_CAR_POSITION, SEND_CAR_POSITION);
    __reg_op_func(ADD_LOG_MESSAGE, ADD_LOG_MESSAGE);
    __reg_op_func(SEND_WIDGET_STATE, SEND_WIDGET_STATE);

    //

    ModConfig::ProcessVersionChanges_PreConfigLoad();

    ModConfig::Load();

    if (Patterns::m_Patterns.size() == 0)
    {
        Patterns::CreateDefaultPatterns();
    }

    ModConfig::ProcessVersionChanges_PostConfigLoad();

    /*
    if(!ModelInfos::HasModelInfo(522))
    {
        auto modelInfo = ModelInfos::CreateModelInfo(522);

        auto lightGroup = new LightGroup();
        lightGroup->offset = CVector(0, 0, 2);
        lightGroup->AddPoint();
        lightGroup->AddPoint();
        lightGroup->AddPoint();
        lightGroup->AddPoint();

        modelInfo->AddLightGroup(lightGroup);
    }
    */

    ModConfig::Save();

    for(auto p : ModelInfos::m_ModelInfos)
    {
        auto modelInfo = p.second;

        for(auto lightGroup : modelInfo->lightGroups)
        {
            lightGroup->Update();
        }
    }

    //

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load() END" << std::endl;
}