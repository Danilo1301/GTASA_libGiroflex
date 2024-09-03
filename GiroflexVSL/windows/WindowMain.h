#pragma once

#include "menu/IMenuVSL.h"

class WindowMain {
public:
	static IWindow* m_Window;

	static void Create();
	static void Remove();
};