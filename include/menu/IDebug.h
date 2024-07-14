#pragma once

#include <iostream>
#include <vector>

class IDebug {
public:
    std::vector<std::string> m_Lines;
    bool m_Visible = false;

    virtual void AddLine(std::string text) = 0;
    virtual void Clear() = 0;
};