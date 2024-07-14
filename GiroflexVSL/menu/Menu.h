#pragma once

#include "Item.h"
#include "Window.h"

/*
[1.0.0]
default

[1.0.1]
added canBeRemoved (to fix a crash)

[1.0.2] 27/05/24
added extra num1 and num2 to AddText

[1.1.0] 04/06/24
added opcodes for creating windows, creating buttons and etc

[1.2.0] 07/06/24
added extraTexts on the right of the item

[1.3.0] 18/06/24
added option to change menu style (colors)
added option to show credits
*/

struct MenuPopup
{
	int gfxId = 0;
	int val1 = 0;
	int val2 = 0;
	int timeLeft = 0;
	float width = 200.0f;
};

struct MenuCredits {
	int gfxId = 6;
	bool hasShownCredits = false;
	int time = 0;
	int timeElapsed = 0;
	int fadeTime = 1000;
};

struct MenuStyle {
	CRGBA COLOR_BUTTON_DEFAULT;
	CRGBA COLOR_BUTTON_SECONDARY;
	CRGBA COLOR_TITLE_BOX;
	CRGBA COLOR_BACKGROUND;
};

class Menu {
public:
	static std::string m_Version;

	static CVector2D m_MenuOffset;

	static std::vector<Window*> m_Windows;
	static Window* m_MainWindow;

	static MenuPopup* m_PopUp;
	static MenuCredits* m_Credits;

	static bool m_DrawCursor;

	static int m_StyleId;
	static std::vector<MenuStyle> m_Styles;

	static bool m_Visible;

	static Window* AddWindow(int gxtId);
	static Window* AddWindow(int gxtId, Window* parent);
	static Window* AddPositionWindow(Window* parent, CVector* vec, float min, float max, float addBy, std::function<void()> onChange, std::function<void()> onBack);
	static Window* AddPositionWindow(Window* parent, CVector* vec);
	static Window* AddPosition2DWindow(Window* parent, CVector2D* vec, float min, float max, float addBy, std::function<void()> onChange);
	static Window* AddPosition2DWindow(Window* parent, CVector2D* vec, float min, float max, float addBy);
	static Window* AddColorMenu(Window* parent, CRGBA* color, std::function<void()> onValueChange);
	static Window* AddColorMenu(Window* parent, CRGBA* color);
	static Window* AddConfirmWindow(Window* parent, int textGxtId, std::function<void()> ohYes, std::function<void()> ohNo);

	static void ShowPopup(int gfxId, int val1, int val2, int time, float width = 200.0f);
	static void ShowCredits(int gfxId, int time);

	static void Update(int dt);
	static void Draw();

	static void RemoveWindow(Window* window);
	static void RemoveAllWindows();

	static MenuStyle* GetStyle();
};