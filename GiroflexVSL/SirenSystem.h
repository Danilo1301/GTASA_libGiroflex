#pragma once

#include "pch.h"

#include "audiosystem.h"

class SirenGroup {
public:
	std::string id;

	std::string hornDir = "";
	std::vector<std::string> sirensDir;
	std::vector<int> compatibleVehicles;

	SirenGroup(std::string id)
	{
		this->id = id;
	}

	bool IsVehicleModelCopatible(int modelId)
	{
		for (auto compatibleVehicle : compatibleVehicles)
		{
			if (compatibleVehicle == modelId)
			{
				return true;
			}
		}
		return false;
	}
};

class SirenSystem {
public:
	static std::string m_DefaultGroupId;
	static std::map<std::string, SirenGroup*> m_SirenGroups;
	static bool FixLoudSounds;
	static float m_VolumeSirens;
	static float m_VolumeRadio;

	static CAudioStream* m_AudioSirenToggle;

	static void Load();
	static void LoadConfig();
	static void LoadVehicles();

    static SirenGroup* GetSirenGroupById(std::string id);
    static SirenGroup* GetDefaultSirenGroup();
    static SirenGroup* GetSirenGroupForModelId(int modelId);
    static bool ModelIdHasSirenGroup(int modelId);
    static int GetCurrentVehicleModelId();
    static SirenGroup* GetCurrentVehicleSoundGroup();

    static void PlaySirenToggleSound();

	//

	int hVehicle = 0;

	//bool hornState = false;
	bool sirenState = false; //if siren is enabled or not (still true if its paused)
	//bool sirenSoundState = false; //if siren sound is playing or not
	int currentSirenIndex = 0;

	unsigned int timeWithSirenOff = -1;

	CAudioStream* horn = NULL;
	std::vector<CAudioStream*> sirens;

	CAudioStream* radio = NULL;
	int radioIndex = 0;

	unsigned int timeChangeSiren = 0;
	unsigned int maxTimeChangeSiren = 10000;
	int npcChangedSirenTo = -2;

	SirenSystem(int hVehicle);

	void LoadAudios();

	void Init();
	void Update(int dt);
	void Destroy();

	bool IsSirenSoundPlaying(int sirenIndex);
	bool IsHornSoundPlaying();
	bool HasSiren(int sirenIndex);

	void ToggleHornSound(bool enabled);
	void ToggleSirenSound(int sirenIndex, bool enabled);
	void ToggleSiren(bool enabled);
	void SetSiren(int sirenIndex);
	void ToggleHornAndStopSiren(bool enabled);
	void ChangeSirenByOne();

	void ToggleRadio(bool enabled);
	void ChangeRadio(int radioId);
};