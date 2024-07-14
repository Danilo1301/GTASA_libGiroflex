#include "ModConfig.h"

#include <iostream>
#include <fstream>

#include <dirent.h>
#include "dlfcn.h"

#include "mod/amlmod.h"
#include "mod/logger.h"
#include "mod/config.h"

#include "Log.h"
#include "Mod.h"
#include "ModelInfos.h"
#include "Patterns.h"
#include "ConvertOldVersion.h"
#include "SirenSystem.h"

#include "windows/WindowSoundPanel.h"
#include "windows/WindowPanel.h"

#include "iniconfig/INIFile.h"

bool isDirExist(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

std::vector<std::string> get_directories_name(const std::string& s)
{
    std::vector<std::string> r;
    for (auto& p : std::filesystem::recursive_directory_iterator(s))
        if (p.is_directory())
            r.push_back(p.path().filename().string());
    return r;
}

//

bool ModConfig::IgnoreOldModVersionMessage = false;
bool ModConfig::TurnOnLightsWithSiren = true;

void ModConfig::MakePaths()
{
    CreateFolder(GetConfigFolder());
    CreateFolder(GetConfigFolder() + "/vehicles");
    CreateFolder(GetConfigFolder() + "/patterns");
    CreateFolder(GetConfigFolder() + "/audios");

    ConvertOldVersion::MakePaths();
}

bool ModConfig::DirExists(std::string path)
{
    return isDirExist(path);
}

bool ModConfig::FileExists(std::string path)
{
    return file_exists(path);
}

std::vector<std::string> ModConfig::GetDirectoriesName(std::string path)
{
    return get_directories_name(path);
}

void ModConfig::ConfigDeleteFile(std::string path)
{
    try {
        if (std::filesystem::remove(path))
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: file " << path << " deleted" << std::endl;
        else
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: file " << path << " not found" << std::endl;
    }
    catch(const std::filesystem::filesystem_error& err) {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: delete file: filesystem error: " << err.what() << std::endl;
    }
}

std::string ModConfig::GetConfigFolder()
{
    char path[0xFF];
    snprintf(path, sizeof(path), "%s/giroflexVSL", aml->GetConfigPath());
    return path;
}

void ModConfig::CreateFolder(std::string path)
{
    if (DirExists(path)) return;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: CreateFolder " << path << std::endl;

    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void ModConfig::WriteToFile(std::string path, Json::Value value) {
	Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: WriteToFile " << path << std::endl;

	Json::StyledWriter writer;
	std::string strJson = writer.write(value);

	std::ofstream file(path);
	file << strJson;
	file.close();
}

Json::Value ModConfig::ReadFile(std::string path) {
	Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: ReadFile " << path << std::endl;

	std::ifstream file(path);

	Json::Value value;
	Json::Reader reader;

	if (!reader.parse(file, value, true)) {
		//MessageBox(HWND_DESKTOP, std::string("Error loading " + path).c_str(), "GiroflexVSL", MB_ICONERROR);
	}

	return value;
}

void ModConfig::Save()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Save" << std::endl;

    MakePaths();

    SavePatterns();
    SaveVehicles();
    SaveSettings();
}

void ModConfig::SavePatterns()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: SavePatterns " << std::endl;

    for (auto pattern : Patterns::m_Patterns)
    {
        auto path = GetConfigFolder() + "/patterns/" + pattern->id + ".json";

        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Saving pattern ID " << pattern->id + ".json" << std::endl;
        
        Json::Value value = pattern->ToJSON();
        
        WriteToFile(path, value);
    }
}

void ModConfig::SaveVehicles()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: SaveVehicles " << std::endl;

    for (auto pair : ModelInfos::m_ModelInfos)
    {
        auto modelInfo = pair.second;

        auto path = GetConfigFolder() + "/vehicles/" + std::to_string(modelInfo->modelId) + ".json";

        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Saving model ID " << modelInfo->modelId << std::endl;
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Saving " << modelInfo->lightGroups.size() << " lightgroups" << std::endl;

        Json::Value value = Json::objectValue;

        value["lightGroups"] = Json::arrayValue;

        for (auto lightGroup : modelInfo->lightGroups)
        {
            value["lightGroups"].append( lightGroup->ToJSON() );
        }

        value["version"] = Mod::m_Version;

	    WriteToFile(path, value);
    }
}

void ModConfig::SaveSettings()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: SaveSettings  (settings.ini)" << std::endl;

    auto settingsFileDir = GetConfigFolder() + "/settings.ini";

    INIFile file;

    auto generalSection = file.AddSection("General");
    generalSection->AddIntFromBool("ignore_message_old_version", ModConfig::IgnoreOldModVersionMessage);
    generalSection->AddIntFromBool("turn_on_lights_with_siren", ModConfig::TurnOnLightsWithSiren);
    generalSection->AddIntFromBool("enable_deep_log", Log::deepLogEnabled);

    generalSection->AddLine("");
    generalSection->AddLine("; Fixes the loud siren sound when camera gets closer to vehicle (or when your camera is inside the vehicle)");
    generalSection->AddLine("; 0 = disabled | 1 = enabled");
    generalSection->AddIntFromBool("fix_loud_siren_sounds", SirenSystem::FixLoudSounds);
    generalSection->AddLine("");

    generalSection->AddFloat("volume_sirens", SirenSystem::m_VolumeSirens);
    generalSection->AddFloat("volume_radio", SirenSystem::m_VolumeRadio);

    generalSection->AddInt("amount_of_panel_slots", WindowPanel::NumOfSlots);


    auto soundPanelSection = file.AddSection("SoundPanel");
    soundPanelSection->AddIntFromBool("allow_multiple_sound", WindowSoundPanel::m_allowMultipleSounds);
    soundPanelSection->AddIntFromBool("show_on_enter_vehicle", WindowSoundPanel::m_showOnEnterVehicle);
    soundPanelSection->AddIntFromBool("show_button_toggle_lights", WindowSoundPanel::m_showButtonToggleLights);
    soundPanelSection->AddCRGBA("button_color", WindowSoundPanel::m_buttonDefaultColor);
    soundPanelSection->AddCRGBA("button_active_color", WindowSoundPanel::m_buttonActiveColor);
    soundPanelSection->AddCRGBA("button_outline_color", WindowSoundPanel::m_buttonOutlineColor);
    soundPanelSection->AddCVector2D("position", WindowSoundPanel::m_position);
    soundPanelSection->AddFloat("button_size", WindowSoundPanel::m_buttonSize);
    soundPanelSection->AddInt("style", WindowSoundPanel::m_style);
    
    file.Save(settingsFileDir);
    file.Destroy();
}

void ModConfig::Load()
{
    MakePaths();

    LoadPatterns();
    LoadVehicles();
    LoadSettings();

    SirenSystem::Load();

    ConvertOldVersion::Load();
}

void ModConfig::LoadPatterns()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Load patterns..." << std::endl;

    auto patternsPath = GetConfigFolder() + "/patterns/";

    DIR* dirp;
    struct dirent* dp;

    if ((dirp = opendir(patternsPath.c_str())) == NULL) {
        //perror("couldn't open '.'");
        return;
    }

    while ((dp = readdir(dirp)) != NULL)
    {
        std::string name = dp->d_name;

        if (name.find(".json") == std::string::npos) continue;
        
        std::string id = name.substr(0, name.find("."));

        std::string path = patternsPath + name;

        Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading pattern '" << name << "' (id: " << id << ") (" << name << ")" << std::endl;

        Json::Value value = ReadFile(path);

        auto pattern = Patterns::CreatePattern(id);

        pattern->FromJSON(value);
    }   
}

void ModConfig::LoadVehicles()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Load vehicles..." << std::endl;

    auto vehiclesPath = GetConfigFolder() + "/vehicles/";

    DIR* dirp;
    struct dirent* dp;

    if ((dirp = opendir(vehiclesPath.c_str())) == NULL) {
        //perror("couldn't open '.'");
        return;
    }

    while ((dp = readdir(dirp)) != NULL)
    {
        std::string name = dp->d_name;

        if (name.find(".json") == std::string::npos) continue;

        //Log::Level(LOG_LEVEL::LOG_BOTH) << name << std::endl;

        int modelId;
        if (sscanf(name.c_str(), "%d.json", &modelId) == 1)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Load model ID " << modelId << " (" << name << ")" << std::endl;

            std::string path = vehiclesPath + name;

            if (!ModelInfos::HasModelInfo(modelId)) ModelInfos::CreateModelInfo(modelId);
            auto modelInfo = ModelInfos::GetModelInfo(modelId);

            Json::Value value = ReadFile(path);

            for (int lightGroup_i = 0; lightGroup_i < (int)value["lightGroups"].size(); lightGroup_i++)
		    {
                LightGroup* lightGroup = new LightGroup();

                lightGroup->FromJSON(value["lightGroups"][lightGroup_i]);

                modelInfo->lightGroups.push_back(lightGroup);
            }
        }
    }
}

void ModConfig::LoadSettings()
{
    auto settingsFileDir = GetConfigFolder() + "/settings.ini";

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: LoadSettings  (settings.ini)" << std::endl;

    INIFile file;
    if (!file.Read(settingsFileDir))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Error reading settings.ini (Not found)" << std::endl;
        return;
    }

    auto generalSections = file.GetSections("General");
    if (generalSections.size() > 0)
    {
        auto generalSection = generalSections[0];

        generalSection->GetBoolFromInt("ignore_message_old_version", &ModConfig::IgnoreOldModVersionMessage);
        generalSection->GetBoolFromInt("turn_on_lights_with_siren", &ModConfig::TurnOnLightsWithSiren);
        generalSection->GetBoolFromInt("enable_deep_log", &Log::deepLogEnabled);
        generalSection->GetBoolFromInt("fix_loud_siren_sounds", &SirenSystem::FixLoudSounds);
        generalSection->GetFloat("volume_sirens", &SirenSystem::m_VolumeSirens);
        generalSection->GetFloat("volume_radio", &SirenSystem::m_VolumeRadio);
        generalSection->GetInt("amount_of_panel_slots", &WindowPanel::NumOfSlots);
    }

    auto soundPanelSections = file.GetSections("SoundPanel");
    if (soundPanelSections.size() > 0)
    {
        auto soundPanelSection = soundPanelSections[0];

        soundPanelSection->GetBoolFromInt("allow_multiple_sound", &WindowSoundPanel::m_allowMultipleSounds);
        soundPanelSection->GetBoolFromInt("show_on_enter_vehicle", &WindowSoundPanel::m_showOnEnterVehicle);
        soundPanelSection->GetBoolFromInt("show_button_toggle_lights", &WindowSoundPanel::m_showButtonToggleLights);
        soundPanelSection->GetCRGBA("button_color", &WindowSoundPanel::m_buttonDefaultColor);
        soundPanelSection->GetCRGBA("button_active_color", &WindowSoundPanel::m_buttonActiveColor);
        soundPanelSection->GetCRGBA("button_outline_color", &WindowSoundPanel::m_buttonOutlineColor);
        soundPanelSection->GetCVector2D("position", &WindowSoundPanel::m_position);
        soundPanelSection->GetFloat("button_size", &WindowSoundPanel::m_buttonSize);
        soundPanelSection->GetInt("style", &WindowSoundPanel::m_style);
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Success reading settings.ini" << std::endl;
}

std::string ModConfig::ReadVersionFile()
{
    std::string prevVersion = "unknown";
    std::string path = GetConfigFolder() + "/version";

    std::ifstream file;
    file.open(path);
    if (file.good())
    {
        getline(file, prevVersion);
    }
    file.close();

    return prevVersion;
}

void ModConfig::ProcessVersionChanges_PreConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [PRE] Updating from " << prevVersion << " to " << currentVersion << std::endl;

    if (prevVersion == currentVersion) return;

    //-------------

    /*
    if (prevVersion == "2.0.0-example")
    {
        prevVersion = "2.0.1";
    }
    */

    //-------------
}

void ModConfig::ProcessVersionChanges_PostConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [POST] Updating from " << prevVersion << " to " << currentVersion << std::endl;
    
    if (prevVersion == currentVersion) return;

    //-------------

    /*
    if (prevVersion == "2.0.0-example")
    {
        prevVersion = "2.0.1";
    }
    */

    //-------------

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Saving version file" << std::endl;

    std::string path = GetConfigFolder() + "/version";

    std::fstream file;
    file.open(path, std::fstream::out);
    file << currentVersion;
    file.close();
}

std::vector<std::string> ModConfig::FindRandomFileVariations(std::string src, int startNumber, std::string suffix)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Trying to find file variations for " << src << ", suffix: " << suffix << std::endl;

    std::vector<std::string> results;

    int i = startNumber;
    bool canLoop = true;
    do
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Trying to find file variation: " << i << std::endl;

        auto filePath = src + std::to_string(i) + suffix;

        if(FileExists(filePath))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Found" << std::endl;
            results.push_back(filePath);

            i++;
        } else {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Not found" << std::endl;
            canLoop = false;
        }
    } while (canLoop);
    
    int numVariations = i - 1;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Found " << numVariations << " variations" << std::endl;

    return results;
}