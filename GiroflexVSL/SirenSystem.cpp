#include "SirenSystem.h"

#include "Log.h"
#include "ModConfig.h"
#include "Globals.h"
#include "SoundSystem.h"
#include "Vehicles.h"
#include "Widgets.h"

#include "windows/WindowSoundPanel.h"

#include "iniconfig/INIFile.h"

std::string SirenSystem::m_DefaultGroupId = "";
std::map<std::string, SirenGroup*> SirenSystem::m_SirenGroups;
bool SirenSystem::FixLoudSounds = true;
float SirenSystem::m_VolumeSirens = 1.0f;
float SirenSystem::m_VolumeRadio = 3.0f;

CAudioStream* SirenSystem::m_AudioSirenToggle = NULL;

extern IBASS* BASS;
extern CCamera* camera;

const int CHANGED_SIREN_TO_NONE = -2;
const int CHANGED_SIREN_TO_HORN = -1;

void SirenSystem::Load()
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Load" << std::endl;

	if (!BASS)
	{
		Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: not loading (BASS was not loaded)" << std::endl;
		return;
	}

	std::string audiosFolder = ModConfig::GetConfigFolder() + "/audios";
	std::string vehiclesFolder = audiosFolder + "/vehicles";

	if (!ModConfig::DirExists(audiosFolder)) return;
	if (!ModConfig::DirExists(vehiclesFolder)) return;

	LoadConfig();
	LoadVehicles();

	auto defaultSirenGroup = GetDefaultSirenGroup();

	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: GetdefaultSirenGroup = " << defaultSirenGroup->id << std::endl;

	auto soundGroupFor416 = GetSirenGroupForModelId(416);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: GetSoundGroupForModelId 416 = " << soundGroupFor416->id << std::endl;
}

void SirenSystem::LoadConfig()
{
	std::string audiosFolder = ModConfig::GetConfigFolder() + "/audios";
	std::string vehiclesFolder = audiosFolder + "/vehicles";

	auto configFileDir = vehiclesFolder + "/config.ini";

	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: LoadConfig (audios/vehicles/config.ini)" << std::endl;

	INIFile file;
	if (!file.Read(configFileDir))
	{
		Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Error reading config.ini (Not found)" << std::endl;
		return;
	}

	auto sections = file.GetSections("Config");
	if (sections.size() == 0) return;
	auto section = sections[0];

	m_DefaultGroupId = section->GetString("defaultGroup");
}

void SirenSystem::LoadVehicles()
{
	std::string audiosFolder = ModConfig::GetConfigFolder() + "/audios";
	std::string vehiclesFolder = audiosFolder + "/vehicles";

	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: LoadVehicles" << std::endl;

	auto directories = ModConfig::GetDirectoriesName(vehiclesFolder);

	for (auto directory : directories)
	{
		std::string id = directory;
		std::string dir = vehiclesFolder + "/" + id;

		Log::Level(LOG_LEVEL::LOG_BOTH) << std::endl;
		Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Loading " << id << std::endl;

		auto sirenGroup = new SirenGroup(id);

		Log::Level(LOG_LEVEL::LOG_BOTH) << "1" << std::endl;

		m_SirenGroups[id] = sirenGroup;

		//
	
		//vehicles.ini

		auto vehiclesFileDir = dir + "/vehicles.ini";

		Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Loading vehicles.ini" << std::endl;

		INIFile file;
		if (!file.Read(vehiclesFileDir))
		{
			Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Error reading vehicles.ini (Not found)" << std::endl;
			return;
		}

		auto sections = file.GetSections("Compatible_Vehicles");
		if (sections.size() == 0) return;
		auto section = sections[0];

		for (auto value : section->values)
		{
			Log::Level(LOG_LEVEL::LOG_BOTH) << "Found value (" << value.first << "|" << value.second << ")" << std::endl;

			int vehicleModelId = std::atoi(value.second.c_str());
			sirenGroup->compatibleVehicles.push_back(vehicleModelId);
		}

		//

		Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Loading horn.wav" << std::endl;

		sirenGroup->hornDir = dir + "/horn.wav";
		
		int i = 1;
		bool searching = true;
		while (searching)
		{
			std::string audioFileName = "siren" + std::to_string(i) + ".wav";

			Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Trying to load: " << audioFileName << std::endl;

			if (!ModConfig::FileExists(dir + "/" + audioFileName))
			{
				searching = false;

				Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Could not find: " << audioFileName << std::endl;

				break;
			}

			sirenGroup->sirensDir.push_back(dir + "/" + audioFileName);

			i++;
		}

        Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Siren group has " << sirenGroup->sirensDir.size() << " sirens" << std::endl;
	}
}

SirenGroup* SirenSystem::GetSirenGroupById(std::string id)
{
	return m_SirenGroups[id];
}

SirenGroup* SirenSystem::GetDefaultSirenGroup()
{
	SirenGroup* defaultSirenGroup = NULL;

	for (auto pair : m_SirenGroups)
	{
		defaultSirenGroup = pair.second;
		break;
	}

	auto find = GetSirenGroupById(m_DefaultGroupId);
	if (find) defaultSirenGroup = find;

	return defaultSirenGroup;
}

SirenGroup* SirenSystem::GetSirenGroupForModelId(int modelId)
{
	//Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: GetSoundGroupForModelId " << modelId << std::endl;

	SirenGroup* retSoundGroup = GetDefaultSirenGroup();

	for (auto pair : m_SirenGroups)
	{
		bool hasCompatible = pair.second->IsVehicleModelCopatible(modelId);
		
		if (hasCompatible)
		{
			retSoundGroup = pair.second;
		}
	}

	return retSoundGroup;
}

bool SirenSystem::ModelIdHasSirenGroup(int modelId)
{
	for(auto pair : m_SirenGroups)
	{
		auto sirenGroup = pair.second;
		
		if(sirenGroup->IsVehicleModelCopatible(modelId)) return true;
	}
	return false;
}

int SirenSystem::GetCurrentVehicleModelId()
{
	//Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: GetCurrentVehicleModelId" << std::endl;

	int modelId = -1;

	if (auto veh = Globals::GetPlayerVehicle())
	{
		modelId = veh->modelId;
	}

	return modelId;
}

SirenGroup* SirenSystem::GetCurrentVehicleSoundGroup()
{
	//Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: GetCurrentVehicleSoundGroup" << std::endl;

	return GetSirenGroupForModelId(GetCurrentVehicleModelId());
}

void SirenSystem::PlaySirenToggleSound()
{
	if(m_AudioSirenToggle->GetState() != AudioStreamState::STREAM_STOPPED)
		m_AudioSirenToggle->Stop();

	m_AudioSirenToggle->Play();
}

//--------------------------------------

SirenSystem::SirenSystem(int hVehicle)
{
	this->hVehicle = hVehicle;
}

void SirenSystem::LoadAudios()
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: LoadAudios" << std::endl;

	// load siren toggle sounds

	if(!m_AudioSirenToggle)
	{
		m_AudioSirenToggle = SoundSystem::LoadStreamFromAudiosFolder("/siren_toggle.wav", false);
	}

	//

	auto vehicle = Vehicles::GetVehicleByHandle(hVehicle);
	auto modelId = vehicle->modelId;

	auto sirenGroup = SirenSystem::GetSirenGroupForModelId(modelId);

	if(!horn)
	{
		horn = SoundSystem::LoadStream(sirenGroup->hornDir, true);
		horn->Loop(true);
		horn->Link(vehicle->pVehicle);
		horn->SetVolume(m_VolumeRadio);
	}

	if(sirens.size() == 0)
	{
		for(auto sirenDir : sirenGroup->sirensDir)
		{
			auto siren = SoundSystem::LoadStream(sirenDir, true);
			siren->Loop(true);
			siren->Link(vehicle->pVehicle);
			siren->SetVolume(m_VolumeSirens);

			sirens.push_back(siren);
		}
	}

	//radio

	if(!radio)
	{
		std::string audiosFolder = ModConfig::GetConfigFolder() + "/audios";

		radio = SoundSystem::LoadStream(audiosFolder + "/radio/radio" + std::to_string(radioIndex + 1) + ".wav", true);
		radio->Loop(true);
		radio->Link(vehicle->pVehicle);
	}
}

void SirenSystem::Init()
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: Init" << std::endl;

	auto vehicle = Vehicles::GetVehicleByHandle(hVehicle);
	auto modelId = vehicle->modelId;

	if(SirenSystem::ModelIdHasSirenGroup(modelId))
	{
		LoadAudios();
	}
}

void SirenSystem::Update(int dt)
{
	Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "SirenSystem: Update" << std::endl;

	auto vehicle = Vehicles::GetVehicleByHandle(hVehicle);
	auto modelId = vehicle->modelId;
	auto playerVehicle = Globals::GetPlayerVehicle();

	if(sirenState) timeWithSirenOff = 0;
	if(timeWithSirenOff != -1 && timeWithSirenOff < 10000)
	{
		timeWithSirenOff += dt;
	}

	if(vehicle == playerVehicle)
	{
		if(Widgets::IsWidgetDoubleClicked(7) && timeWithSirenOff >= 0 && timeWithSirenOff < 300)
		{
			ChangeSirenByOne();
		}

		if(horn && sirenState == true)
		{
			if(Widgets::IsWidgetPressed(7))
			{
				auto timePressed = Widgets::m_Widgets[7].timePressed;

				if(timePressed > 100)
				{
					if(!IsHornSoundPlaying())
					{
						ToggleHornAndStopSiren(true);
					}
				}
			}
		}

		if(horn && IsHornSoundPlaying())
		{
			if(Widgets::IsWidgetJustReleased(7))
			{
				ToggleHornAndStopSiren(false);
			}
		}
	}

	if(vehicle != playerVehicle && vehicle->pDriver && (sirenState || IsHornSoundPlaying()))
	{
		timeChangeSiren += dt;

		if(timeChangeSiren >= maxTimeChangeSiren)
		{
			timeChangeSiren = 0;
			
			if(npcChangedSirenTo == CHANGED_SIREN_TO_NONE)
			{
				npcChangedSirenTo = GetRandomNumber(-1, sirens.size() - 1);
				maxTimeChangeSiren = 4000;

				//Log::Level(LOG_LEVEL::LOG_BOTH) << "Changed siren to " << npcChangedSirenTo << std::endl;
				
				if(npcChangedSirenTo == CHANGED_SIREN_TO_HORN)
				{
					ToggleHornAndStopSiren(true);
				} else {
					if(currentSirenIndex != npcChangedSirenTo)
						SetSiren(npcChangedSirenTo);
				}
			} else {
				//Log::Level(LOG_LEVEL::LOG_BOTH) << "Siren was " << npcChangedSirenTo << std::endl;

				if(npcChangedSirenTo == CHANGED_SIREN_TO_HORN)
				{
					ToggleHornAndStopSiren(false);
				} else {
					if(currentSirenIndex != 0)
						SetSiren(0);
				}

				npcChangedSirenTo = CHANGED_SIREN_TO_NONE;
				maxTimeChangeSiren = 10000;
			}
		}
	}

	bool playerUsedThisCar = Globals::hPrevUsedPlayerVehicle == vehicle->hVehicle;

	if(vehicle != playerVehicle && !vehicle->pDriver && currentSirenIndex != 0 && !playerUsedThisCar)
	{
		if(npcChangedSirenTo == CHANGED_SIREN_TO_HORN)
		{
			ToggleHornAndStopSiren(false);
		} else {
			SetSiren(0);
		}

		npcChangedSirenTo = CHANGED_SIREN_TO_NONE;
		maxTimeChangeSiren = 10000;
	}

	if(radio) radio->SetVolume(m_VolumeRadio);

	CMatrix* pMatrix = camera->GetMatSA();
    CVector cameraPosition = pMatrix->pos;
	CVector vehiclePositon = vehicle->position;

	std::vector<CAudioStream*> streams;

	if(horn) streams.push_back(horn);
	for(auto siren : sirens) streams.push_back(siren);

	for(auto stream : streams)
	{
		stream->SetVolume(m_VolumeSirens);
	}

	auto distanceFromCamera = DistanceBetween(vehiclePositon, cameraPosition);

	auto maxFarDistance = 100.0f;
	auto farMult = 1 - (distanceFromCamera / maxFarDistance);
	if(farMult < 0) farMult = 0;
	if(farMult > 1) farMult = 1;

	if(FixLoudSounds)
	{
		auto maxNearDistance = 5.0f;

		auto nearMult = 1.0f;
		if(distanceFromCamera < maxNearDistance)
		{
			nearMult = distanceFromCamera/maxNearDistance;;
		}
		if(distanceFromCamera < 2.0f)
		{
			nearMult = 0.05f;
		}

		auto mult = nearMult * farMult;

		//Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "nearMult: " << nearMult << std::endl;
		//Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "farMult: " << farMult << std::endl;
		//Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "mult: " << mult << "| d: " << distanceFromCamera << std::endl;

		for(auto stream : streams)
		{
			stream->SetVolume(m_VolumeSirens * mult);
		}
	} 

	//boomark1
	if(WindowSoundPanel::m_turnOffSirenWhenLeaveCar)
	{
		if(vehicle->pDriver == NULL && sirenState)
		{
			ToggleSiren(false);
		}
	}

	/*
	//if(!SirenSystem::ModelIdHasSirenGroup(modelId)) return;

	auto siren = sirens[0];
	if(siren->GetState() == -1)
	{
		siren->Play();
	}
	*/
}

void SirenSystem::Destroy()
{
	if(horn)
	{
		horn->Stop();
		SoundSystem::UnloadStream(horn);
	}

	for(auto siren : sirens)
	{
		siren->Stop();
		SoundSystem::UnloadStream(siren);
	}

	if(radio)
	{
		radio->Stop();
		SoundSystem::UnloadStream(radio);
	}
}

bool SirenSystem::IsSirenSoundPlaying(int sirenIndex)
{
	if(!HasSiren(sirenIndex)) return false;
	auto siren = sirens[sirenIndex];
	return siren->GetState() == AudioStreamState::STREAM_PLAYING_OR_STALLED;
}

bool SirenSystem::IsHornSoundPlaying()
{
	if(!horn) return false;
	return horn->GetState() == AudioStreamState::STREAM_PLAYING_OR_STALLED;
}

bool SirenSystem::HasSiren(int sirenIndex)
{
	return sirenIndex >= 0 && sirenIndex < sirens.size();
}

void SirenSystem::ToggleHornSound(bool enabled)
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: ToggleHornSound " << enabled << std::endl;

	if(!horn) LoadAudios();

	if(!horn) return;

	//hornState = enabled;

	if (enabled)
	{
		horn->Play();
	}
	else {
		horn->Pause();
	}
}

void SirenSystem::ToggleSirenSound(int sirenIndex, bool enabled)
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: ToggleSirenSound " << sirenIndex << ", " << enabled << std::endl;

	if(sirens.size() == 0) LoadAudios();

	if(!HasSiren(sirenIndex)) return;

	auto siren = sirens[sirenIndex];
	
	//sirenSoundState = enabled;

	if (enabled)
	{
		siren->Play();
	}
	else {
		siren->Pause();
	}
}

void SirenSystem::ToggleSiren(bool enabled)
{
	sirenState = enabled;

	ToggleSirenSound(currentSirenIndex, enabled);
}

void SirenSystem::SetSiren(int sirenIndex)
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: SetSiren " << sirenIndex << std::endl;
	
	auto prevSirenIndex = currentSirenIndex;

	ToggleSirenSound(prevSirenIndex, false);

	currentSirenIndex = sirenIndex;

	ToggleSirenSound(sirenIndex, true);
}

void SirenSystem::ToggleHornAndStopSiren(bool enabled)
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: ToggleHornAndStopSiren " << enabled << std::endl;

	ToggleHornSound(enabled);

	if(sirenState)
		ToggleSirenSound(currentSirenIndex, !enabled);
}

void SirenSystem::ChangeSirenByOne()
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: ChangeSirenByOne" << std::endl;

	int max = sirens.size();

	auto sirenIndex = currentSirenIndex + 1;
	if (sirenIndex > max - 1) sirenIndex = 0;

	SetSiren(sirenIndex);
}

void SirenSystem::ToggleRadio(bool enabled)
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "SirenSystem: ToggleRadio " << (enabled ? "TRUE" : "FALSE") << std::endl;

	if(!radio)
	{
		LoadAudios();
	}

	if (enabled)
	{
		radio->Play();
	}
	else {
		radio->Pause();
	}
}

void SirenSystem::ChangeRadio(int radioId)
{
	ToggleRadio(false);

	if(radio)
	{
		radio->Stop();
		SoundSystem::UnloadStream(radio);
		radio = NULL;
	}

	radioIndex = radioId;

	LoadAudios();
}