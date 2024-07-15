#include "Patterns.h"
#include "Log.h"

std::vector<Pattern*> Patterns::m_Patterns;
int Patterns::m_TimeBetweenPatterns = 7000;

Pattern* Patterns::CreatePattern(std::string id)
{
	Pattern* pattern = new Pattern(id);
	m_Patterns.push_back(pattern);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Patterns: Create pattern" << std::endl;

	return pattern;
}

bool Patterns::HasPattern(std::string id)
{
    return GetPatternById(id) != NULL;
}

Pattern* Patterns::GetPatternById(std::string id)
{
    for (auto pattern : m_Patterns)
    {
        if (pattern->id.compare(id) == 0) return pattern;
    }
    return NULL;
}

void Patterns::CreateDefaultPatterns()
{
    /*
    if (!Patterns::HasPattern("4_lights-1"))
    {
        auto PATTERN_4_lights_1 = Patterns::CreatePattern("4_lights-1");
        PATTERN_4_lights_1->AddStep({ 1, 0, 0, 0 }, 300);
        PATTERN_4_lights_1->AddStep({ 1, 1, 0, 0 }, 300);
        PATTERN_4_lights_1->AddStep({ 0, 1, 1, 0 }, 300);
        PATTERN_4_lights_1->AddStep({ 0, 0, 1, 1 }, 300);
        PATTERN_4_lights_1->AddStep({ 0, 0, 0, 1 }, 300);
        PATTERN_4_lights_1->AddStep({ 0, 0, 0, 0 }, 300);
    }
    */
}

std::vector<Pattern*> Patterns::GetCompatiblePatterns(int amountOfLights)
{
    std::vector<Pattern*> patterns;

    for (auto pattern : m_Patterns)
    {
        if (pattern->steps[0]->data.size() != amountOfLights) continue;
        patterns.push_back(pattern);
    }

    return patterns;
}