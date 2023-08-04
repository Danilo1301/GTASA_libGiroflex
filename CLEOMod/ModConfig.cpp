#include "ModConfig.h"

#include "ModelInfos.h"
#include "Patterns.h"
#include "Log.h"
#include "Mod.h"
#include "LightGroupDatas.h"

#include "mod/amlmod.h"
#include "mod/logger.h"
#include "mod/config.h"

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

void ModConfig::MakePaths()
{
    Log::file << "ModConfig: MakePaths" << std::endl;

    CreateFolder(GetConfigFolder());
    CreateFolder(GetConfigFolder() + "/vehicles");
    CreateFolder(GetConfigFolder() + "/patterns");
}

bool ModConfig::DirExists(std::string path)
{
    return isDirExist(path);
}

std::string ModConfig::GetConfigFolder()
{
    char path[0xFF];
    snprintf(path, sizeof(path), "%s/giroflex", aml->GetConfigPath());
    return path;
}

void ModConfig::CreateFolder(std::string path)
{
    if (DirExists(path)) return;

    Log::file << "ModConfig: CreateFolder " << path << std::endl;

    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void ModConfig::Save()
{
    Log::file << "ModConfig: Save " << std::endl;

    MakePaths();

    SavePatterns();
    SaveVehicles();
}

void ModConfig::SavePatterns()
{
    Log::file << "ModConfig: SavePatterns " << std::endl;

    for (auto pattern : Patterns::m_Patterns)
    {
        auto path = GetConfigFolder() + "/patterns/" + pattern->id + ".ini";

        Log::file << "ModConfig: Saving pattern ID " << pattern->id + ".ini" << std::endl;
        
        INIFile file;

        /*
        * TEMPORARY FIX
        */
        auto section = file.AddSection("Pattern");
        //section->tmpSaveFix = true;

        for (auto step : pattern->steps)
        {
            std::string line = "";
            for (auto value : step->data)
            {
                line += std::to_string(value);
            }
            line += "|" + std::to_string(step->duration);

            
            section->AddLine(line);
        }

        file.Save(path);
        file.Destroy();
    }
}

void ModConfig::SaveVehicles()
{
    Log::file << "ModConfig: SaveVehicles " << std::endl;

    for (auto pair : ModelInfos::m_ModelInfos)
    {
        auto modelInfo = pair.second;

        auto path = GetConfigFolder() + "/vehicles/" + std::to_string(modelInfo->modelId) + ".ini";

        INIFile file;

        Log::file << "ModConfig: Saving model ID " << modelInfo->modelId << std::endl;
        Log::file << "Saving " << modelInfo->lightGroups.size() << " lightgroups" << std::endl;

        for (auto lightGroup : modelInfo->lightGroups)
        {
            auto section = lightGroup->ToINISection();
            file.sections.push_back(section);
        }

        file.Save(path);
        file.Destroy();
    }
}

void ModConfig::Load()
{
    MakePaths();

    LoadPatterns();
    LoadVehicles();
}

void ModConfig::LoadPatterns()
{
    Log::file << "ModConfig: Load patterns..." << std::endl;

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

        if (name.find(".ini") == std::string::npos) continue;
        
        std::string id = name.substr(0, name.find("."));

        std::string path = patternsPath + name;

        Log::file << "Loading pattern '" << name << "' (" << id << ")" << std::endl;

        //
        auto pattern = Patterns::CreatePattern(id);
        //

        INIFile file;
        file.Read(path);

        auto sections = file.GetSections("Pattern");
        if (sections.size() == 0) continue;
        auto section = sections[0];

        //
        
     
        for (auto value : section->values)
        {
            auto line = value.second;

            //Log::file << line << std::endl;

            auto patternStr = line.substr(0, line.find("|"));
            auto timeStr = line.substr(line.find("|") + 1);

            auto time = std::atoi(timeStr.c_str());

            //Log::file << "pattern=" << patternStr << std::endl;
            //Log::file << "time=" << time << std::endl;

            std::vector<int> data;
            for (char& c : patternStr)
            {
                auto value = c == '1' ? 1 : 0;
                data.push_back(value);

                //Log::file << "value: " << value << std::endl;
            }

            pattern->AddStep(data, time);
        }

        //

        file.Destroy();
    }
}

void ModConfig::LoadVehicles()
{
    Log::file << "ModConfig: Load vehicles..." << std::endl;

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

        //Log::file << name << std::endl;

        int modelId;
        if (sscanf(name.c_str(), "%d.ini", &modelId) == 1)
        {
            Log::file << "ModConfig: Load model ID " << modelId << std::endl;

            std::string path = vehiclesPath + name;

            INIFile file;
            file.Read(path);

            if (!ModelInfos::HasModelInfo(modelId)) ModelInfos::CreateModelInfo(modelId);
            auto modelInfo = ModelInfos::GetModelInfo(modelId);

            Log::file << "Loading " << file.GetSections("LightGroup").size() << " lightgroups" << std::endl;

            auto sections = file.GetSections("LightGroup");
            for (auto section : sections)
            {
                LightGroup* lightGroup = new LightGroup();

                lightGroup->FromINISection(section);

                lightGroup->MakeLightGroup();

                modelInfo->lightGroups.push_back(lightGroup);
            }

            file.Destroy();
        }
    }
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
    std::string currentVersion = Mod::Version;

    Log::file << "ModConfig: [PRE] Updating from " << prevVersion << " to " << currentVersion << std::endl;

    if (prevVersion == currentVersion) return;

    if (prevVersion == "unknown")
    {
        prevVersion = "2.7.0";

        auto patternsPath = GetConfigFolder() + "/patterns/";
        remove((patternsPath + "10lights_1.ini").c_str());
        remove((patternsPath + "10lights_2.ini").c_str());
        remove((patternsPath + "10lights_3.ini").c_str());
    }

    /*
    if (prevVersion == "2.7.0")
    {
        prevVersion = "2.";
    }
    */
}

void ModConfig::ProcessVersionChanges_PostConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::Version;

    Log::file << "ModConfig: [POST] Updating from " << prevVersion << " to " << currentVersion << std::endl;
    
    if (prevVersion == currentVersion) return;

    if (prevVersion == "unknown")
    {
        prevVersion = "2.7.0";

        Patterns::CreateDefaultPatterns();

        /*
        * changing 10 lights type from 3 (old) to 4
        */
        for (auto pairModelInfo : ModelInfos::m_ModelInfos)
        {
            auto modelInfo = pairModelInfo.second;

            for (auto lightGroup : modelInfo->lightGroups)
            {
                if (lightGroup->type == 3) //3 is old TEN_LIGHTS (before 2.7.0)
                {
                    lightGroup->type = eLightGroupType::TEN_LIGHTS;
                    lightGroup->MakeLightGroup();
                }
            }
        }
    }

    /*
    if (prevVersion == "2.7.0")
    {
        prevVersion = "2.";
    }
    */

    //--------------


    Log::file << "ModConfig: Saving version file" << std::endl;

    std::string path = GetConfigFolder() + "/version";

    std::fstream file;
    file.open(path, std::fstream::out);
    file << currentVersion;
    file.close();
}