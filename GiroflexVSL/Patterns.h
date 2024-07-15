#pragma once

#include "pch.h"

#include "Pattern.h"

class Patterns {
public:
	static std::vector<Pattern*> m_Patterns;
	static int m_TimeBetweenPatterns;

	static Pattern* CreatePattern(std::string id);
	static bool HasPattern(std::string name);
	static Pattern* GetPatternById(std::string name);

	static void CreateDefaultPatterns();

	static std::vector<Pattern*> GetCompatiblePatterns(int amountOfLights);
};