#include "LightGroup.h"

#include "Patterns.h"
#include "Log.h"

LightGroup::LightGroup()
{

}

LightGroup::~LightGroup()
{
    Destroy();
}

Point* LightGroup::AddPoint()
{
    Point* point = new Point();
    points.push_back(point);

    Update();

    return point;
}

void LightGroup::SetNumberOfPoints(int amount)
{
    while(points.size() < amount)
    {
        AddPoint();
    }
    while(points.size() > amount)
    {
        RemovePoint(points[points.size() - 1]);
    }
}

void LightGroup::RemovePoint(Point* point)
{
    auto it = std::find(points.begin(), points.end(), point);
    if (it == points.end()) return;
    points.erase(it);
    delete point;
}

void LightGroup::RemoveAllPoints()
{
    while (points.size() > 0)
        RemovePoint(points[0]);
}

void LightGroup::Update()
{
    //patterns.clear();

    auto compatiblePatterns = Patterns::GetCompatiblePatterns(points.size());

    std::vector<std::string> idsToRemove;
    for(auto p : patterns)
    {
        auto id = p.first;

        auto pattern = Patterns::GetPatternById(id);

        auto it = std::find(compatiblePatterns.begin(), compatiblePatterns.end(), pattern);
        if(it == compatiblePatterns.end())
        {
            idsToRemove.push_back(id);
        }
    }

    for(auto id : idsToRemove)
    {
        patterns.erase(id);
    }

    for(auto pattern : compatiblePatterns)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Checking pattern " + pattern->id << std::endl;

        auto it = patterns.find(pattern->id);
        bool hasPattern = it != patterns.end();

        if(!hasPattern)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Doesnt have pattern, adding defualt value" << std::endl;

            patterns[pattern->id] = true;
        }
    }
}

void LightGroup::Destroy()
{
    RemoveAllPoints();
}

Json::Value LightGroup::ToJSON()
{
    Json::Value value = Json::objectValue;

    value["offset"] = CVectorToJSON(offset);

    //points
    value["points"] = Json::arrayValue;
    for (auto point : points) {
        value["points"].append( point->ToJSON() );
    }

    value["distance"] = distance;
    value["curve"] = curve;

    value["color1"] = ColorToJSON(color1);
    value["color2"] = ColorToJSON(color2);
    value["color3"] = ColorToJSON(color3);

    value["patternOffset"] = patternOffset;

    value["radius"] = radius;

    value["renderShadow"] = renderShadow;
    value["shadowIntensity"] = shadowIntensity;
    value["shadowSize"] = shadowSize;
    value["shadowPositionX"] = shadowPositionX;
    value["shadowPositionY"] = shadowPositionY;
    value["shadowTexture"] = shadowTexture;
    value["shadowRotation"] = shadowRotation;
    value["shadowFlipTextures"] = shadowFlipTextures;

    value["renderPointLight"] = renderPointLight;
    value["pointLightIntensity"] = pointLightIntensity;
    value["pointLightDistance"] = pointLightDistance;

    value["nearClip"] = nearClip;

    //value["usePointPositionInsteadOfIndex"] = usePointPositionInsteadOfIndex;

    value["useSmallWhiteCorona"] = useSmallWhiteCorona;
    value["smallWhiteCoronaScale"] = smallWhiteCoronaScale;
    value["smallWhiteCoronaTexture"] = smallWhiteCoronaTexture;
    value["smallWhiteCoronaColor"] = ColorToJSON(smallWhiteCoronaColor);

    value["freezeLights"] = freezeLights;

    value["useFlare"] = useFlare;
    value["flareIntensity"] = flareIntensity;
    value["flareDistance"] = flareDistance;
    value["flareTexture"] = flareTexture;

    value["coronaTexture"] = coronaTexture;

    value["lightSlotId"] = lightSlotId;

    value["alwaysEnabled"] = alwaysEnabled;
    //value["enableOnSpawn"] = enableOnSpawn;
    value["enableWithLights"] = enableWithLights;


    value["rotate"] = rotate;
    value["rotateDistance"] = rotateDistance;
    value["rotateSpeed"] = rotateSpeed;
    value["rotateInverse"] = rotateInverse;

    value["useLightbarLEDs"] = useLightbarLEDs;
    value["lightbarLEDStartIndex"] = lightbarLEDStartIndex;
    value["useNormalLEDs"] = useNormalLEDs;
    value["normalLEDStartIndex"] = normalLEDStartIndex;

    value["ledColor1Enabled"] = ColorToJSON(ledColor1Enabled);
    value["ledColor2Enabled"] = ColorToJSON(ledColor2Enabled);
    value["ledColor3Enabled"] = ColorToJSON(ledColor3Enabled);
    value["ledColorDisabled"] = ColorToJSON(ledColorDisabled);

    Json::Value patternsValue = Json::objectValue;
    for(auto p : patterns)
    {
        auto id = p.first;
        auto value = p.second;

        patternsValue[id] = value;
    }
    value["patterns"] = patternsValue;

    value["direction"] = (int)direction;

    return value;
}

void LightGroup::FromJSON(Json::Value value)
{
    offset = ValidateCVector(value["offset"], offset);

    for (int i = 0; i < (int)value["points"].size(); i++)
    {
        auto point = AddPoint();
        point->FromJSON(value["points"][i]);
    }

    distance = ValidateValue(value["distance"], distance).asFloat();
    curve = ValidateValue(value["curve"], curve).asFloat();

    color1 = ValidateColor(value["color1"], color1);
    color2 = ValidateColor(value["color2"], color2);
    color3 = ValidateColor(value["color3"], color3);

    patternOffset = ValidateValue(value["patternOffset"], patternOffset).asInt();

    radius = ValidateValue(value["radius"], radius).asFloat();

    renderShadow = ValidateValue(value["renderShadow"], renderShadow).asBool();
    shadowIntensity = ValidateValue(value["shadowIntensity"], shadowIntensity).asFloat();
    shadowSize = ValidateValue(value["shadowSize"], shadowSize).asFloat();
    shadowPositionX = ValidateValue(value["shadowPositionX"], shadowPositionX).asFloat();
    shadowPositionY = ValidateValue(value["shadowPositionY"], shadowPositionY).asFloat();
    shadowTexture = ValidateValue(value["shadowTexture"], shadowTexture).asInt();
    shadowRotation = ValidateValue(value["shadowRotation"], shadowRotation).asFloat();
    shadowFlipTextures = ValidateValue(value["shadowFlipTextures"], shadowFlipTextures).asBool();

    renderPointLight = ValidateValue(value["renderPointLight"], renderPointLight).asBool();
    pointLightIntensity = ValidateValue(value["pointLightIntensity"], pointLightIntensity).asFloat();
    pointLightDistance = ValidateValue(value["pointLightDistance"], pointLightDistance).asFloat();

    nearClip = ValidateValue(value["nearClip"], nearClip).asFloat();

    //usePointPositionInsteadOfIndex = ValidateValue(value["usePointPositionInsteadOfIndex"], usePointPositionInsteadOfIndex).asBool();

    useSmallWhiteCorona = ValidateValue(value["useSmallWhiteCorona"], useSmallWhiteCorona).asBool();
    smallWhiteCoronaScale = ValidateValue(value["smallWhiteCoronaScale"], smallWhiteCoronaScale).asFloat();
    smallWhiteCoronaTexture = ValidateValue(value["smallWhiteCoronaTexture"], smallWhiteCoronaTexture).asInt();
    smallWhiteCoronaColor = ValidateColor(value["smallWhiteCoronaColor"], smallWhiteCoronaColor);

    freezeLights = ValidateValue(value["freezeLights"], freezeLights).asBool();

    useFlare = ValidateValue(value["useFlare"], useFlare).asBool();
    flareIntensity = ValidateValue(value["flareIntensity"], flareIntensity).asFloat();
    flareDistance = ValidateValue(value["flareDistance"], flareDistance).asFloat();
    flareTexture = ValidateValue(value["flareTexture"], flareTexture).asInt();

    coronaTexture = ValidateValue(value["coronaTexture"], coronaTexture).asInt();

    lightSlotId = ValidateValue(value["lightSlotId"], lightSlotId).asInt();

    alwaysEnabled = ValidateValue(value["alwaysEnabled"], alwaysEnabled).asBool();
    //enableOnSpawn = ValidateValue(value["enableOnSpawn"], enableOnSpawn).asBool();
    enableWithLights = ValidateValue(value["enableWithLights"], enableWithLights).asBool();

    rotate = ValidateValue(value["rotate"], rotate).asBool();
    rotateDistance = ValidateValue(value["rotateDistance"], rotateDistance).asFloat();
    rotateSpeed = ValidateValue(value["rotateSpeed"], rotateSpeed).asFloat();
    rotateInverse = ValidateValue(value["rotateInverse"], rotateInverse).asBool();

    useLightbarLEDs = ValidateValue(value["useLightbarLEDs"], useLightbarLEDs).asBool();
    lightbarLEDStartIndex = ValidateValue(value["lightbarLEDStartIndex"], lightbarLEDStartIndex).asInt();

    useNormalLEDs = ValidateValue(value["useNormalLEDs"], useNormalLEDs).asBool();
    normalLEDStartIndex = ValidateValue(value["normalLEDStartIndex"], normalLEDStartIndex).asInt();

    ledColor1Enabled = ValidateColor(value["ledColor1Enabled"], ledColor1Enabled);
    ledColor2Enabled = ValidateColor(value["ledColor2Enabled"], ledColor2Enabled);
    ledColor3Enabled = ValidateColor(value["ledColor3Enabled"], ledColor3Enabled);
    ledColorDisabled = ValidateColor(value["ledColorDisabled"], ledColorDisabled);

    Json::Value patternsValue = value["patterns"];
    if(!patternsValue.isNull())
    {
        for(auto id : patternsValue.getMemberNames())
        {
            auto value = patternsValue[id].asBool();

            patterns[id] = value;
        }
    }

    direction = (eSirenDirection)ValidateValue(value["direction"], (int)direction).asInt();
}

std::vector<Pattern*> LightGroup::GetPatterns()
{
    std::vector<Pattern*> compatiblePatterns;

    for(auto p : patterns)
    {
        auto id = p.first;
        auto value = p.second;

        auto pattern = Patterns::GetPatternById(id);

        if(!value) continue;
        
        compatiblePatterns.push_back(pattern);
    }

    return compatiblePatterns;
}