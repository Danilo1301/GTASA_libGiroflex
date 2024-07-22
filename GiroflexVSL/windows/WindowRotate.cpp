#include "WindowRotate.h"

#include "WindowMain.h"

#include "VehicleDummy.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

void WindowRotate::Create(Window* parent, LightGroup* lightGroup)
{
    auto window = Menu::AddWindow(94, parent);
    window->showPageControls = true;
    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    window->AddCheckbox(94, &lightGroup->rotate);

    window->AddFloatRange(53, &lightGroup->rotateDistance, 0.0f, 1000.0f, 0.01f);

    window->AddFloatRange(95, &lightGroup->rotateSpeed, 0.0f, 50.0f, 0.1f);

    window->AddCheckbox(97, &lightGroup->rotateInverse);
}

static std::vector<std::string> selectObjectStrVec;

void WindowRotate::CreatePointRotate(Window* parent, LightGroup* lightGroup, Point* point)
{
    auto window = menuVSL->AddWindow();
    window->m_Title = "Point - Rotate";
    
    selectObjectStrVec.clear();
    selectObjectStrVec.push_back("[none]");
    auto pVehicle = WindowMain::m_Vehicle->pVehicle;
    auto atomics = VehicleDummy::RpClumpGetAllAtomics(pVehicle->m_pRwClump);
	for (auto atomic : atomics)
	{
		if (!atomic->geometry) continue;

		auto frameAtomic = GetObjectParent((RwObject*)atomic);
		auto name = VehicleDummy::GetFrameName(frameAtomic);

        selectObjectStrVec.push_back(name);
    }

    auto selectObject = window->AddButton("Select object", CRGBA(255, 255, 255));
    selectObject->m_StringAtRight = &point->rotateObject.object;
    selectObject->onClick = [window, point] () {
        auto newWindow = menuVSL->AddWindowOptionsString("Select object", window, &point->rotateObject.object, &selectObjectStrVec);
    };

    auto speed = window->AddFloatRange("Speed", &point->rotateObject.speed, -100.0f, 100.0f, 0.5f);
    speed->m_HoldToChange = true;

    auto axis = window->AddOptions("Axis");
    axis->AddOption((int)eRotateObjectAxis::X, "X");
    axis->AddOption((int)eRotateObjectAxis::Y, "Y");
    axis->AddOption((int)eRotateObjectAxis::Z, "Z");
    axis->SetCurrentOption((int)point->rotateObject.axis);
    axis->onValueChange = [axis, point]() {

        auto vehicle = WindowMain::m_Vehicle;
        vehicle->ResetObjectRotation(point->rotateObject.object);

        point->rotateObject.axis = (eRotateObjectAxis)axis->GetCurrentOption().value;
    };

    window->AddCheckbox("Rotate always", &point->rotateObject.rotateAlways);

    auto close = window->AddButton("> ~r~Close", CRGBA(0, 0, 0, 0));
    close->onClick = [window]() {
        window->SetToBeRemoved();
        Menu::m_Visible = true;
    };
}