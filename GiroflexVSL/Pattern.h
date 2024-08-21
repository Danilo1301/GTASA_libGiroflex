#pragma once

#include "pch.h"

#include "Log.h"

#include <vector>

struct PatternStep {
	std::vector<int> data;
	int duration = 0;

	bool useCustomColor = false;
	CRGBA customColor = CRGBA(255, 0, 0);

	bool useCustomLedColor = false;
	CRGBA customLedColor = CRGBA(255, 0, 0);
};

class Pattern {
public:
	std::string id = "";
	std::vector<PatternStep*> steps;

	Pattern(std::string id)
	{
		this->id = id;
	}

	PatternStep* AddStep(std::vector<int> data, int duration)
	{
		PatternStep* step = new PatternStep();
		step->data = data;
		step->duration = duration;

		steps.push_back(step);
		return step;
	}

	void RemoveStep(PatternStep* step)
	{
		auto it = std::find(steps.begin(), steps.end(), step);
		if (it == steps.end()) return;
		steps.erase(it);
		delete step;
	}

	void RemoveAllSteps()
	{
		while (steps.size() > 0)
			RemoveStep(steps[0]);
	}

	Json::Value ToJSON()
	{
		Json::Value value = Json::objectValue;

		value["steps"] = Json::arrayValue;

		for (auto step : steps)
		{
			Json::Value stepValue = Json::objectValue;

			stepValue["duration"] = step->duration;

			stepValue["data"] = Json::arrayValue;

			for (auto d : step->data)
			{
				stepValue["data"].append(d);
			}

			stepValue["useCustomColor"] = step->useCustomColor;
			stepValue["customColor"] = ColorToJSON(step->customColor);

			stepValue["useCustomLedColor"] = step->useCustomLedColor;
			stepValue["customLedColor"] = ColorToJSON(step->customLedColor);

			value["steps"].append( stepValue );
		}

		return value;
	}

	void FromJSON(Json::Value value)
	{
		for (int step_i = 0; step_i < (int)value["steps"].size(); step_i++)
		{
			Json::Value stepValue = value["steps"][step_i];

			int duration = stepValue["duration"].asInt();

			if (duration <= 0) continue;
			
			std::vector<int> data;
			for (int val_i = 0; val_i < (int)stepValue["data"].size(); val_i++)
			{
				data.push_back(stepValue["data"][val_i].asInt());
			}

			auto step = AddStep(data, duration);

			step->useCustomColor = ValidateValue(stepValue["useCustomColor"], step->useCustomColor).asBool();
			step->customColor = ValidateColor(stepValue["customColor"], step->customColor);

			step->useCustomLedColor = ValidateValue(stepValue["useCustomLedColor"], step->useCustomLedColor).asBool();
			step->customLedColor = ValidateColor(stepValue["customLedColor"], step->customColor);
		}
	}
};
