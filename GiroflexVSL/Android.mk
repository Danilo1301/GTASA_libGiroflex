LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_CPP_FEATURES += exceptions
LOCAL_MODULE    := GiroflexVSL
LOCAL_SRC_FILES := main.cpp mod/logger.cpp mod/config.cpp json_reader.cpp json_value.cpp json_writer.cpp Log.cpp Mod.cpp menu/Draw.cpp menu/Menu.cpp menu/Item.cpp menu/Window.cpp menu/CleoMenu.cpp Input.cpp windows/WindowTest.cpp Vehicles.cpp Vehicle.cpp Globals.cpp LightGroup.cpp ModelInfo.cpp ModelInfos.cpp windows/WindowMain.cpp windows/WindowLightGroups.cpp windows/WindowSettings.cpp windows/WindowEditing.cpp Patterns.cpp LightGroupDatas.cpp windows/WindowWhiteCorona.cpp windows/WindowShadow.cpp windows/WindowPointLight.cpp windows/WindowFlare.cpp ModConfig.cpp iniconfig/INIFile.cpp iniconfig/INISection.cpp SoundSystem.cpp windows/WindowSelectPanel.cpp windows/WindowSoundPanel.cpp windows/SoundPanelButton.cpp windows/WindowSoundPanelSettings.cpp windows/WindowPanel.cpp ConvertOldVersion.cpp windows/WindowRotate.cpp audiosystem.cpp SirenSystem.cpp Widgets.cpp GiroflexVSL.cpp VehicleDummy.cpp windows/WindowLed.cpp sdk/Image.cpp windows/WindowPatterns.cpp
LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17
LOCAL_C_INCLUDES += ./include
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)