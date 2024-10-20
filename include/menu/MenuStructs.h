#pragma once

#include <iostream>
#include <string>

struct MVehicle
{
   int hVehicle;
   void* pVehicle;
};

static unsigned char ucharIntensity(unsigned char uc, float intensity)
{
    return (unsigned char)std::clamp((int)round(((float)uc) * intensity), 0, 255);
}

static bool StringVectorContainsString(const std::vector<std::string>& vec, const std::string& str)
{
    // Utiliza std::find para procurar o std::string no vetor
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

static void RemoveStringFromVector(std::vector<std::string>& vec, const std::string& str) {
    // Usa std::remove para mover os elementos não removidos para o início do vetor
    auto newEnd = std::remove(vec.begin(), vec.end(), str);

    // Remove os elementos redundantes do vetor
    vec.erase(newEnd, vec.end());
}