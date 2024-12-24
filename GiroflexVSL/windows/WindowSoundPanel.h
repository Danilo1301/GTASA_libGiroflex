#pragma once

#include "../pch.h"

#include "audiosystem.h"

#include "SoundPanelButton.h"

struct AudioStreamData
{
	int audioId = 0;
	SoundPanelButton* button = NULL;
};

class WindowSoundPanel {
public:
	static std::vector<SoundPanelButton*> m_buttons;
	static std::vector<AudioStreamData> m_audioStreamData;
	//static int m_prevActiveIndex;
	static bool m_allowMultipleSounds;
	static bool m_showOnEnterVehicle;
	static bool m_showButtonToggleLights;
	static bool m_turnOffSirenWhenLeaveCar;
	static CRGBA m_buttonDefaultColor;
	static CRGBA m_buttonActiveColor;
	static CRGBA m_buttonOutlineColor;
	static CVector2D m_position;
	static int m_style;
	static float m_buttonSize;

	static bool m_visible;

	static SoundPanelButton* m_buttonSirenTone;
	static SoundPanelButton* m_buttonSirenToggle;

	static SoundPanelButton* m_buttonToggleLights;

	static void Toggle(bool state);
	static void Create();
	static void CreateStyle1();
	static void CreateStyle2();
	static void CreateStyle3();
	static void CreateStyle4();
	static void Update(int dt);
	static void Draw();
	static void DestroyButtons();

	static SoundPanelButton* AddButton();
	static void RecreateButtons();

	static void AddButtonToAudioList(SoundPanelButton* button, int audioId);
	static void ToggleAudioButton(int index);
	static void StopAllSounds();
};