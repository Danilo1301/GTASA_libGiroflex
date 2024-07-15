#pragma once

#include "../pch.h"

#include "../menu/Menu.h"

#include "Vehicle.h"

class WindowMain {
public:
	static Window* m_Window;
	static Vehicle* m_Vehicle;
	static int m_ModelId;

	static void Create(Vehicle* vehicle);
	static void Remove();
};