#include "WindowSoundPanel.h"

#include "WindowSoundPanelSettings.h"

#include "menu/Menu.h"
#include "Input.h"
#include "ModConfig.h"
#include "Log.h"
#include "ModelInfos.h"
#include "Globals.h"
#include "SoundSystem.h"
#include "SirenSystem.h"

#include "IModPolicia.h"
extern IModPolicia* modPolicia;

std::vector<SoundPanelButton*> WindowSoundPanel::m_buttons;
std::vector<AudioStreamData> WindowSoundPanel::m_audioStreamData;
//int WindowSoundPanel::m_prevActiveIndex = -1;
bool WindowSoundPanel::m_allowMultipleSounds = false;
bool WindowSoundPanel::m_showOnEnterVehicle = true;
bool WindowSoundPanel::m_showButtonToggleLights = true;
CRGBA WindowSoundPanel::m_buttonDefaultColor = { 109, 167, 100, 255 };
CRGBA WindowSoundPanel::m_buttonActiveColor = { 255, 131, 131, 255 };
CRGBA WindowSoundPanel::m_buttonOutlineColor = { 46, 77, 51, 255 };
CVector2D WindowSoundPanel::m_position = CVector2D(260, 340);
int WindowSoundPanel::m_style = 1;
float WindowSoundPanel::m_buttonSize = 30.0f;

bool WindowSoundPanel::m_visible = false;

SoundPanelButton* WindowSoundPanel::m_buttonSirenTone = NULL;
SoundPanelButton* WindowSoundPanel::m_buttonSirenToggle = NULL;

SoundPanelButton* WindowSoundPanel::m_buttonToggleLights = NULL;

Vehicle* prevVehicle = NULL;
static int prevAudioStreamDataIndex = -1;

void WindowSoundPanel::Toggle(bool state)
{
	if (m_visible == state) return;

	m_visible = state;

	if (m_visible)
	{
		Create();
	}
	else {
		//StopAllSounds();
		DestroyButtons();
	}
}

void WindowSoundPanel::Create()
{
	switch (m_style)
	{
	case 1:
		CreateStyle1();
		break;
	case 2:
		CreateStyle2();
		break;
	case 3:
		CreateStyle3();
		break;
	case 4:
	default:
		CreateStyle4();
		break;
	}
}

void WindowSoundPanel::CreateStyle1()
{
	float buttonSize = m_buttonSize * 1.8f;

	CRGBA buttonDefaultColor = m_buttonDefaultColor;
	CRGBA buttonActiveColor = m_buttonActiveColor;
	CRGBA buttonOutlineColor = m_buttonOutlineColor;
	CRGBA textDefaultColor = { 0, 0, 0, 255 };
	CRGBA white = CRGBA(255, 255, 255);

	auto buttonSirenHorn = AddButton();
	buttonSirenHorn->position = m_position + CVector2D(0, 0);
	buttonSirenHorn->size = CVector2D(buttonSize, buttonSize);
	buttonSirenHorn->sprite.spriteId = 24;
	buttonSirenHorn->text.gxtId = 1;
	buttonSirenHorn->text.num1 = 1;
	buttonSirenHorn->text.color = CRGBA(0, 0, 0, 0);
	buttonSirenHorn->color = white;
	buttonSirenHorn->activeColor = buttonActiveColor;
	buttonSirenHorn->activeOnHold = true;
	//buttonSirenHorn->canBeActivated = false;
	buttonSirenHorn->onIsActiveChange = [](bool isActive) {
		//Menu::ShowPopup(1, isActive ? 1 : 0, 0, 1000.0f);

		auto vehicle = Globals::GetPlayerVehicle();
		vehicle->sirenSystem->ToggleHornSound(isActive);

		//SoundPanelSystem::ToggleHorn(isActive);
	};
	//AddButtonToAudioList(buttonSirenToggle, 1, true);

	auto buttonSirenToggle = AddButton();
	buttonSirenToggle->position = m_position + CVector2D(5.0f + buttonSize, 0);
	buttonSirenToggle->size = CVector2D(buttonSize, buttonSize);
	buttonSirenToggle->sprite.spriteId = 28;
	//buttonSirenToggle->text.gxtId = 1;
	//buttonSirenToggle->text.num1 = 1;
	buttonSirenToggle->text.color = CRGBA(0, 0, 0, 0);
	buttonSirenToggle->color = white;
	buttonSirenToggle->activeColor = buttonActiveColor;
	buttonSirenToggle->onIsActiveChange = [](bool isActive) {
		//Menu::ShowPopup(1, isActive ? 1 : 0, 0, 1000.0f);

		auto vehicle = Globals::GetPlayerVehicle();
		vehicle->sirenSystem->ToggleSiren(isActive);

		//SoundPanelSystem::ToggleSiren(isActive);
	};
	m_buttonSirenToggle = buttonSirenToggle;
	//buttonSirenToggle->canBeActivated = false;
	//AddButtonToAudioList(buttonSirenToggle, 1, true);

	auto buttonSirenTone = AddButton();
	buttonSirenTone->position = m_position + CVector2D(2 * (5.0f + buttonSize), 0);
	buttonSirenTone->size = CVector2D(buttonSize, buttonSize);
	buttonSirenTone->sprite.spriteId = 25;
	buttonSirenTone->text.gxtId = 1;
	//buttonSirenTone->text.num1 = SoundPanelSystem::currentSirenIndex;
	buttonSirenTone->text.color = CRGBA(255, 255, 255);
	buttonSirenTone->color = white;
	//buttonSirenTone->activeColor = m_buttonActiveColor;
	buttonSirenTone->canBeActivated = false;
	buttonSirenTone->onClick = []() {
		auto vehicle = Globals::GetPlayerVehicle();
		vehicle->sirenSystem->ChangeSirenByOne();

		//SoundPanelSystem::ChangeSirenByOne();
	};
	m_buttonSirenTone = buttonSirenTone;
	//AddButtonToAudioList(buttonSirenToggle, 1, true);

	auto buttonSettings = AddButton();
	buttonSettings->position = m_position + CVector2D(3 * (5.0f + buttonSize), 0);
	buttonSettings->size = CVector2D(buttonSize, buttonSize);
	buttonSettings->sprite.spriteId = 27;
	//buttonSettings->text.gxtId = 1;
	buttonSettings->text.color = CRGBA(0, 0, 0, 0);
	buttonSettings->color = white;
	//buttonSirenTone->activeColor = m_buttonActiveColor;
	buttonSettings->canBeActivated = false;
	buttonSettings->onClick = []() {
		WindowSoundPanelSettings::Create();
	};
	//AddButtonToAudioList(buttonSirenToggle, 1, true);
}

void WindowSoundPanel::CreateStyle2()
{
	float buttonSize = m_buttonSize;
	int numOfButtons = 8;
	float margin = 5.0f;
	//CVector2D buttonSize = CVector2D(buttonSize, buttonSize);

	/*
	CVector2D screenSize = Input::GetGTAScreenSize();
	CVector2D startPos = {
		(screenSize.x / 2) - ((numOfButtons + 2) * (buttonSize + margin)) / 2,
		screenSize.y - 45.0f
	};
	*/

	CRGBA buttonDefaultColor = m_buttonDefaultColor;
	CRGBA buttonActiveColor = m_buttonActiveColor;
	CRGBA buttonOutlineColor = m_buttonOutlineColor;
	CRGBA textDefaultColor = { 0, 0, 0, 255 };

	CVector2D currentPos = m_position;

	currentPos = currentPos + CVector2D(0, 67);

	for (int i = 0; i < numOfButtons; i++)
	{
		auto button = AddButton();
		button->position = currentPos;
		button->size = CVector2D(buttonSize, buttonSize);
		button->text.gxtId = 1;
		button->text.num1 = i;
		button->text.color = textDefaultColor;
		button->color = buttonDefaultColor;
		button->drawOutline = true;
		button->outlineColor = buttonOutlineColor;
		button->activeColor = buttonActiveColor;
		button->blinkOnActive = true;

		if (i == 0) //i know this doesnt make sense but why not do it
		{
			AddButtonToAudioList(button, 0);
		}
		else {
			AddButtonToAudioList(button, i);
		}


		currentPos = currentPos + CVector2D(buttonSize + margin, 0);
	}

	auto buttonSettings = AddButton();
	buttonSettings->position = currentPos;
	buttonSettings->size = CVector2D(buttonSize, buttonSize);
	buttonSettings->text.gxtId = 20;
	buttonSettings->text.color = textDefaultColor;
	buttonSettings->color = CRGBA(110, 110, 110);
	buttonSettings->drawOutline = true;
	buttonSettings->outlineColor = CRGBA(80, 80, 80);
	buttonSettings->canBeActivated = false;
	buttonSettings->onClick = []() {
		WindowSoundPanelSettings::Create();
	};

	currentPos = currentPos + CVector2D(buttonSize + margin, 0);

	auto buttonClose = AddButton();
	buttonClose->position = currentPos;
	buttonClose->size = CVector2D(buttonSize, buttonSize);
	buttonClose->text.gxtId = 10;
	buttonClose->text.color = textDefaultColor;
	buttonClose->color = CRGBA(255, 100, 100);
	buttonClose->drawOutline = true;
	buttonClose->outlineColor = CRGBA(178, 70, 70);
	buttonClose->canBeActivated = false;
	buttonClose->onClick = []() {
		Toggle(false);
	};
}

void WindowSoundPanel::CreateStyle3()
{
	CVector2D bgPosition = m_position;
	//CVector2D bgSize = { 235.0f, 103.0f };

	CRGBA buttonDefaultColor = m_buttonDefaultColor;
	CRGBA buttonActiveColor = m_buttonActiveColor;
	CRGBA textDefaultColor = { 0, 0, 0, 255 };

	auto buttonSettings = AddButton();
	buttonSettings->position = bgPosition + CVector2D(3, 3);
	buttonSettings->size = CVector2D(38, 25);
	buttonSettings->sprite.spriteId = 23;
	buttonSettings->text.gxtId = 20;
	buttonSettings->text.color = textDefaultColor;
	buttonSettings->color = CRGBA(128, 128, 128);
	buttonSettings->canBeActivated = false;
	buttonSettings->onClick = []() {
		WindowSoundPanelSettings::Create();
	};

	auto buttonSirenHorn = AddButton();
	buttonSirenHorn->position = bgPosition + CVector2D(3, 29);
	buttonSirenHorn->size = CVector2D(50, 35);
	buttonSirenHorn->sprite.spriteId = 21;
	buttonSirenHorn->text.gxtId = 1;
	buttonSirenHorn->text.num1 = 0;
	buttonSirenHorn->text.color = textDefaultColor;
	buttonSirenHorn->color = buttonDefaultColor;
	buttonSirenHorn->activeColor = buttonActiveColor;
	AddButtonToAudioList(buttonSirenHorn, 0);

	auto buttonSiren1 = AddButton();
	buttonSiren1->position = bgPosition + CVector2D(3, 65);
	buttonSiren1->size = CVector2D(63, 35);
	buttonSiren1->sprite.spriteId = 21;
	buttonSiren1->text.gxtId = 1;
	buttonSiren1->text.num1 = 1;
	buttonSiren1->text.color = textDefaultColor;
	buttonSiren1->color = buttonDefaultColor;
	buttonSiren1->activeColor = buttonActiveColor;
	AddButtonToAudioList(buttonSiren1, 1);

	//

	auto buttonSiren2 = AddButton();
	buttonSiren2->position = bgPosition + CVector2D(67, 29);
	buttonSiren2->size = CVector2D(50, 35);
	buttonSiren2->sprite.spriteId = 23;
	buttonSiren2->text.gxtId = 1;
	buttonSiren2->text.num1 = 2;
	buttonSiren2->text.color = textDefaultColor;
	buttonSiren2->color = buttonDefaultColor;
	buttonSiren2->activeColor = buttonActiveColor;
	AddButtonToAudioList(buttonSiren2, 2);

	auto buttonSiren3 = AddButton();
	buttonSiren3->position = bgPosition + CVector2D(67, 65);
	buttonSiren3->size = CVector2D(50, 35);
	buttonSiren3->sprite.spriteId = 23;
	buttonSiren3->text.gxtId = 1;
	buttonSiren3->text.num1 = 3;
	buttonSiren3->text.color = textDefaultColor;
	buttonSiren3->color = buttonDefaultColor;
	buttonSiren3->activeColor = buttonActiveColor;
	AddButtonToAudioList(buttonSiren3, 3);

	//

	auto buttonSiren4 = AddButton();
	buttonSiren4->position = bgPosition + CVector2D(118, 29);
	buttonSiren4->size = CVector2D(50, 35);
	buttonSiren4->sprite.spriteId = 23;
	buttonSiren4->text.gxtId = 1;
	buttonSiren4->text.num1 = 4;
	buttonSiren4->text.color = textDefaultColor;
	buttonSiren4->color = buttonDefaultColor;
	buttonSiren4->activeColor = buttonActiveColor;
	AddButtonToAudioList(buttonSiren4, 4);

	auto buttonSiren5 = AddButton();
	buttonSiren5->position = bgPosition + CVector2D(118, 65);
	buttonSiren5->size = CVector2D(50, 35);
	buttonSiren5->sprite.spriteId = 23;
	buttonSiren5->text.gxtId = 1;
	buttonSiren5->text.num1 = 5;
	buttonSiren5->text.color = textDefaultColor;
	buttonSiren5->color = buttonDefaultColor;
	buttonSiren5->activeColor = buttonActiveColor;
	AddButtonToAudioList(buttonSiren5, 5);

	//

	auto buttonClose = AddButton();
	buttonClose->position = bgPosition + CVector2D(194, 3);
	buttonClose->size = CVector2D(38, 25);
	buttonClose->sprite.spriteId = 23;
	buttonClose->text.gxtId = 10;
	buttonClose->text.color = textDefaultColor;
	buttonClose->color = CRGBA(220, 145, 145);;
	buttonClose->canBeActivated = false;
	buttonClose->onClick = []() {
		WindowSoundPanel::Toggle(false);
	};

	auto buttonSiren6 = AddButton();
	buttonSiren6->position = bgPosition + CVector2D(182, 29);
	buttonSiren6->size = CVector2D(50, 35);
	buttonSiren6->sprite.spriteId = 22;
	buttonSiren6->text.gxtId = 1;
	buttonSiren6->text.num1 = 6;
	buttonSiren6->text.color = textDefaultColor;
	buttonSiren6->color = buttonDefaultColor;
	buttonSiren6->activeColor = buttonActiveColor;
	AddButtonToAudioList(buttonSiren6, 6);

	auto buttonSiren7 = AddButton();
	buttonSiren7->position = bgPosition + CVector2D(169, 65);
	buttonSiren7->size = CVector2D(63, 35);
	buttonSiren7->sprite.spriteId = 22;
	buttonSiren7->text.gxtId = 1;
	buttonSiren7->text.num1 = 7;
	buttonSiren7->text.color = textDefaultColor;
	buttonSiren7->color = buttonDefaultColor;
	buttonSiren7->activeColor = buttonActiveColor;
	AddButtonToAudioList(buttonSiren7, 7);
}

void WindowSoundPanel::CreateStyle4()
{
	int numButtonsX = 5;
	int numButtonsY = 2;
	float buttonSize = m_buttonSize;
	float margin = 3.0f;

	CRGBA buttonDefaultColor = m_buttonDefaultColor;
	CRGBA buttonActiveColor = m_buttonActiveColor;
	CRGBA buttonOutlineColor = m_buttonOutlineColor;
	CRGBA textDefaultColor = { 0, 0, 0, 255 };

	CVector2D currentPos = m_position;

	currentPos = currentPos + CVector2D(0, 33);

	int i = 0;
	for (int x = 0; x < numButtonsX; x++)
	{
		for (int y = 0; y < numButtonsY; y++)
		{
			CVector2D offset = CVector2D(
				x * (buttonSize + margin),
				y * (buttonSize + margin)
			);

			auto button = AddButton();
			button->position = currentPos + offset;
			button->size = CVector2D(buttonSize, buttonSize);
			button->text.gxtId = 1;
			button->text.num1 = i;
			button->text.color = textDefaultColor;
			button->color = buttonDefaultColor;
			button->drawOutline = true;
			button->outlineColor = buttonOutlineColor;
			button->activeColor = buttonActiveColor;
			button->blinkOnActive = true;

			//0 2 4 6 8
			//1 3 5 7 9

			if (i == 8)
			{
				button->text.gxtId = 10;
				button->canBeActivated = false;
				button->color = CRGBA(255, 100, 100);
				button->outlineColor = CRGBA(178, 70, 70);
				button->onClick = []() {
					Toggle(false);
				};
			} else if (i == 9)
			{
				button->text.gxtId = 20;
				button->canBeActivated = false;
				button->color = CRGBA(110, 110, 110);
				button->outlineColor = CRGBA(80, 80, 80);
				button->onClick = []() {
					WindowSoundPanelSettings::Create();
				};
			}
			else {
				if (i == 0) //i know this doesnt make sense but why not do it
				{
					AddButtonToAudioList(button, 0);
				}
				else {
					AddButtonToAudioList(button, i);
				}
			}

			i++;
		}
	}
}

void WindowSoundPanel::Update(int dt)
{
	if(Globals::m_UsingMultiSiren)
	{
		return;
	}

	if(modPolicia)
	{
		if(modPolicia->IsRadioOpen()) return;
		if(modPolicia->IsMenuOpen()) return;
	}

	if(auto vehicle = Globals::GetPlayerVehicle())
	{
		prevVehicle = vehicle;
	}

	//enable if testing
	//if (!m_visible) Toggle(true);

	if (!m_buttonToggleLights)
	{
		if (Globals::IsPlayerInAnyVehicle() && m_showButtonToggleLights)
		{
			auto vehicle = Globals::GetPlayerVehicle();

			if (ModelInfos::HasModelInfo(vehicle->modelId))
			{
				float buttonSize = m_buttonSize * 1.8f;

				CRGBA buttonActiveColor = m_buttonActiveColor;
				CRGBA white = CRGBA(255, 255, 255);

				auto buttonToggleLights = m_buttonToggleLights = new SoundPanelButton();
				buttonToggleLights->position = m_position + CVector2D(4 * (5.0f + buttonSize), 0);
				buttonToggleLights->size = CVector2D(buttonSize, buttonSize);
				buttonToggleLights->sprite.spriteId = 26;
				//buttonToggleLights->text.gxtId = 1;
				//buttonToggleLights->text.num1 = 1;
				buttonToggleLights->text.color = CRGBA(0, 0, 0, 0);
				buttonToggleLights->color = white;
				buttonToggleLights->activeColor = buttonActiveColor;
				buttonToggleLights->onIsActiveChange = [vehicle](bool isActive) {
					vehicle->SetGiroflexEnabled(!vehicle->prevLightsState);

					SirenSystem::PlaySirenToggleSound();

					//Menu::ShowPopup(1, vehicle->prevLightsState ? 1 : 0, 0, 1000.0f);
				};
			}
		}
	}

	if (m_buttonToggleLights)
	{
		bool canDestroy = false;

		m_buttonToggleLights->Update(dt);
		if (Globals::IsPlayerInAnyVehicle() && m_showButtonToggleLights)
		{
			auto vehicle = Globals::GetPlayerVehicle();
			if (vehicle->prevLightsState != m_buttonToggleLights->isActive)
			{
				m_buttonToggleLights->isActive = vehicle->prevLightsState;
				m_buttonToggleLights->prevIsActive = vehicle->prevLightsState;
				//Menu::ShowPopup(1, m_buttonToggleLights->isActive ? 1 : 0, 0, 1000.0f);
			}

			if (!ModelInfos::HasModelInfo(vehicle->modelId))
			{
				canDestroy = true;
			}
		}
		else {
			canDestroy = true;
		}

		if (canDestroy)
		{
			m_buttonToggleLights->Destroy();
			delete m_buttonToggleLights;
			m_buttonToggleLights = NULL;
		}
	}

	//show pannel if its enabled
	if (m_showOnEnterVehicle && SirenSystem::m_SirenGroups.size() > 0)
	{
		if (!m_visible)
		{
			if (Globals::IsPlayerInAnyVehicle())
			{
				auto modelId = SirenSystem::GetCurrentVehicleModelId();

				auto soundGroup = SirenSystem::GetCurrentVehicleSoundGroup();
				if (soundGroup->IsVehicleModelCopatible(modelId))
				{
					Toggle(true);
				}
			}
		}
	}

	if (!m_visible) return;

	std::for_each(m_buttons.begin(), m_buttons.end(), [dt](SoundPanelButton* button) { button->Update(dt); });

	if(m_buttonSirenTone)
	{
		auto vehicle = Globals::GetPlayerVehicle();
		if(vehicle) m_buttonSirenTone->text.num1 = vehicle->sirenSystem->currentSirenIndex + 1;
	}

	/*
	if(m_buttonSirenToggle)
	{
		auto vehicle = Globals::GetPlayerVehicle();
		if(vehicle)
		{
			if(vehicle->gameSirenState && !m_buttonSirenToggle->isActive)
			{
				m_buttonSirenToggle->isActive = vehicle->prevSirenState;
				m_buttonSirenToggle->prevIsActive = vehicle->prevSirenState;
			}
		}
	}
	*/

	//disable if testing
	if (!Globals::IsPlayerInAnyVehicle())
	{
		Toggle(false);
	}
}

void WindowSoundPanel::Draw()
{
	if(Globals::m_UsingMultiSiren)
	{
		return;
	}

	if(modPolicia)
	{
		if(modPolicia->IsRadioOpen()) return;
		if(modPolicia->IsMenuOpen()) return;
	}
	
	if (m_buttonToggleLights)
	{
		m_buttonToggleLights->Draw();
	}

	if (!m_visible) return;

	if (m_style == 3)
	{
		CVector2D bgPosition = m_position;
		CVector2D bgSize = { 235.0f, 103.0f };

		Draw::DrawBox(bgPosition, bgSize, CRGBA(32, 32, 32, 200));
	}

	std::for_each(m_buttons.begin(), m_buttons.end(), [](SoundPanelButton* button) { button->Draw(); });
}

void WindowSoundPanel::DestroyButtons()
{
	if (m_buttonSirenTone)
	{
		m_buttonSirenTone = NULL;
	}

	if (m_buttonToggleLights)
	{
		m_buttonToggleLights->Destroy();
		delete m_buttonToggleLights;
		m_buttonToggleLights = NULL;
	}

	while (m_buttons.size() > 0)
	{
		auto button = m_buttons[0];
		button->Destroy();
		m_buttons.erase(std::find(m_buttons.begin(), m_buttons.end(), button));
		delete button;
	}

	m_audioStreamData.clear();

	prevAudioStreamDataIndex = -1;
}

SoundPanelButton* WindowSoundPanel::AddButton()
{
	SoundPanelButton* button = new SoundPanelButton();
	m_buttons.push_back(button);
	return button;
}

void WindowSoundPanel::RecreateButtons()
{
	//StopAllSounds();
	DestroyButtons();
	Create();
}

void WindowSoundPanel::AddButtonToAudioList(SoundPanelButton* button, int audioId)
{
	std::string audiosPath = ModConfig::GetConfigFolder() + "/audios/";

	AudioStreamData audioStreamData;
	audioStreamData.audioId = audioId;
	audioStreamData.button = button;

	int dataIndex = m_audioStreamData.size();

	m_audioStreamData.push_back(audioStreamData);

	if (audioId == 0)
	{
		button->onPressBegin = [audioStreamData]() {
			auto vehicle = Globals::GetPlayerVehicle();
			vehicle->sirenSystem->ToggleHornAndStopSiren(true);

			audioStreamData.button->isActive = true;
		};

		button->onPressEnd = [audioStreamData]() {
			auto vehicle = Globals::GetPlayerVehicle();
			vehicle->sirenSystem->ToggleHornAndStopSiren(false);

			audioStreamData.button->isActive = false;
		};
	}
	else {
		if(auto vehicle = Globals::GetPlayerVehicle())
		{
			if(vehicle->sirenSystem->HasSiren(audioId - 1))
			{
				if(vehicle->sirenSystem->IsSirenSoundPlaying(audioId - 1))
				{
					button->isActive = true;

					prevAudioStreamDataIndex = dataIndex;
				}
			}
		}

		button->onClick = [audioId, audioStreamData, dataIndex]() {
			auto vehicle = Globals::GetPlayerVehicle();

			if(prevAudioStreamDataIndex > 0 && prevAudioStreamDataIndex != dataIndex) {
				m_audioStreamData[prevAudioStreamDataIndex].button->isActive = false;
				//prevAudioStreamData = NULL; //not necessary
				vehicle->sirenSystem->ToggleSiren(false);
			}

			if(!vehicle->sirenSystem->sirenState) //&&vehicle->sirenSystem->HasSiren(audioId - 1)
			{
				vehicle->sirenSystem->ToggleSiren(true);
				vehicle->sirenSystem->SetSiren(audioId - 1);

				prevAudioStreamDataIndex = dataIndex;
			} else {
				vehicle->sirenSystem->ToggleSiren(false);
			}
		};
	}
}

void WindowSoundPanel::ToggleAudioButton(int index)
{
	//Menu::ShowPopup(1, index, 0, 1000.0f);

	/*
	AudioStreamData* audioStreamData = &m_audioStreamData[index];

	if (!audioStreamData->audioStream->OK) return;

	if (audioStreamData->audioStream->GetState() == 1) //if playing
	{
		audioStreamData->audioStream->Stop();
	}
	else {
		audioStreamData->audioStream->Play();
		
		if (m_allowMultipleSounds == false)
		{
			if (m_prevActiveIndex != -1 && m_prevActiveIndex != index)
			{
				AudioStreamData* prevAudioStreamData = &m_audioStreamData[m_prevActiveIndex];
				prevAudioStreamData->audioStream->Stop();
				prevAudioStreamData->button->isActive = prevAudioStreamData->audioStream->GetState() == 1;
			}
		}
	}

	audioStreamData->button->isActive = audioStreamData->audioStream->GetState() == 1;

	m_prevActiveIndex = index;
	*/
}

void WindowSoundPanel::StopAllSounds()
{
	//m_prevActiveIndex = -1;

	if(prevVehicle)
	{
		prevVehicle->sirenSystem->ToggleHornSound(false);
		prevVehicle->sirenSystem->ToggleSiren(false);
	}

	/*
	std::for_each(m_audioStreamData.begin(), m_audioStreamData.end(), [](AudioStreamData& audioStreamData) {
		audioStreamData.audioStream->Stop();
		audioStreamData.button->isActive = false;
	});
	*/
}