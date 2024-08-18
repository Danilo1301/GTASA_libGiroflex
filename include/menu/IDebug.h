#pragma once

#include <iostream>
#include <vector>

class IDebug {
public:
    std::vector<std::string> lines;
    bool visible = false;

    virtual void AddLine(std::string text) = 0;
    virtual void Clear() = 0;
};