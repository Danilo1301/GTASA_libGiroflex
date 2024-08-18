#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

#include "menu/SimpleGTA.h"
#include "SimpleGTA.h"

extern char* (*GetFrameNodeName)(RwFrame* frame);

extern void (*RegisterCorona)(unsigned int id, void* attachTo, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, CVector const& posn, float radius, float farClip, int coronaType, int flaretype, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, unsigned char fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);
extern int (*GetVehicleRef)(int);
extern void* (*GetVehicleFromRef)(int);

static std::string to_upper(std::string data) {
    std::for_each(data.begin(), data.end(), [](char& c) {
        c = ::toupper(c);
        });
    return data;
}

static std::string to_lower(std::string data) {
    std::for_each(data.begin(), data.end(), [](char& c) {
        c = ::tolower(c);
        });
    return data;
}