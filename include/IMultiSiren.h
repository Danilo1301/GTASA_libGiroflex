#pragma once

//Interface version: 1.1.0

class IMultiSiren {
public:
    virtual void ToggleAudioPanel() = 0;
    virtual void ToggleAudioPanelSettings() = 0;
};