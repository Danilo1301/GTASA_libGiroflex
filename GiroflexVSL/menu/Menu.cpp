#include "Menu.h"

#include "../Log.h"
#include "../Input.h"

#include "pch.h"

std::string Menu::m_Version = "1.3.0";

CVector2D Menu::m_MenuOffset = CVector2D(0, 0);

std::vector<Window*> Menu::m_Windows;
Window* Menu::m_MainWindow = new Window();

MenuPopup* Menu::m_PopUp = new MenuPopup();
MenuCredits* Menu::m_Credits = new MenuCredits();

bool Menu::m_DrawCursor = false;

int Menu::m_StyleId = 0;
std::vector<MenuStyle> Menu::m_Styles = {
    { //black
        CRGBA(30, 30, 30),
        CRGBA(100, 100, 100),
        CRGBA(58, 58, 58),
        CRGBA(0, 0, 0)
    },
    { //blue
        CRGBA(0, 0, 50),
        CRGBA(96, 125, 219),
        CRGBA(23, 30, 53),
        CRGBA(54, 70, 124)
    }
};

bool Menu::m_Visible = true;

Window* Menu::AddWindow(int gxtId)
{
    Window* window = new Window();
    window->titleGtxId = gxtId;
    window->position = Window::m_DefaultWindowPosition;

    m_Windows.push_back(window);

   Log::Level(LOG_LEVEL::LOG_BOTH) << "Menu: AddWindow" << std::endl;

    return window;
}

Window* Menu::AddWindow(int gxtId, Window* parent)
{
    Window* window = AddWindow(gxtId);
    window->parentWindow = parent;
    window->position = parent->position;

    parent->visible = false;

    return window;
}

Window* Menu::AddPositionWindow(Window* parent, CVector* vec, float min, float max, float addBy, std::function<void()> onChange, std::function<void()> onBack)
{
    Window* window = AddWindow(9, parent);
    window->width = 200.0f;
    window->position = { 10, 200 };
    window->showPageControls = true;

    auto floatx = window->AddFloatRange(10, &vec->x, min, max, addBy);
    floatx->onValueChange = [onChange]() {onChange(); };

    auto floaty = window->AddFloatRange(11, &vec->y, min, max, addBy);
    floaty->onValueChange = [onChange]() {onChange(); };

    auto floatz = window->AddFloatRange(12, &vec->z, min, max, addBy);
    floatz->onValueChange = [onChange]() {onChange(); };

    window->btnBack->onClick = [window, onBack]()
    {
        window->GoToPrevWindow();
        onBack();
    };

    return window;
}

Window* Menu::AddPositionWindow(Window* parent, CVector* vec)
{
    return AddPositionWindow(parent, vec, 1000.0f, -1000.0f, 0.01f, []() {}, []() {});
}

Window* Menu::AddPosition2DWindow(Window* parent, CVector2D* vec, float min, float max, float addBy, std::function<void()> onChange)
{   
    Window* window = NULL;
    if(parent)
    {
        window = AddWindow(9, parent);
    } else {
        window = AddWindow(9);
    }
    window->width = 200.0f;
    window->position = { 10, 200 };
    window->showPageControls = true;

    auto floatx = window->AddFloatRange(10, &vec->x, min, max, addBy);
    floatx->onValueChange = [onChange]() {onChange(); };

    auto floaty = window->AddFloatRange(11, &vec->y, min, max, addBy);
    floaty->onValueChange = [onChange]() {onChange(); };

    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    return window;
}

Window* Menu::AddPosition2DWindow(Window* parent, CVector2D* vec, float min, float max, float addBy)
{
    return Menu::AddPosition2DWindow(parent, vec, min, max, addBy, []() {});
}

static int colormenu_r = 0;
static int colormenu_g = 0;
static int colormenu_b = 0;
static int colormenu_a = 0;

Window* Menu::AddColorMenu(Window* parent, CRGBA* color, std::function<void()> onValueChange)
{
    Window* window = AddWindow(13, parent);
    window->width = 200.0f;
    window->position = { 10, 200 };
    window->showPageControls = true;

    colormenu_r = color->r;
    colormenu_g = color->g;
    colormenu_b = color->b;
    colormenu_a = color->a;

    auto option_r = window->AddIntRange(14, &colormenu_r, 0, 255, 1);
    auto option_g = window->AddIntRange(15, &colormenu_g, 0, 255, 1);
    auto option_b = window->AddIntRange(16, &colormenu_b, 0, 255, 1);
    auto option_a = window->AddIntRange(17, &colormenu_a, 0, 255, 1);

    option_r->onValueChange = [color, onValueChange]() { color->r = colormenu_r; onValueChange(); };
    option_g->onValueChange = [color, onValueChange]() { color->g = colormenu_g; onValueChange(); };
    option_b->onValueChange = [color, onValueChange]() { color->b = colormenu_b; onValueChange(); };
    option_a->onValueChange = [color, onValueChange]() { color->a = colormenu_a; onValueChange(); };

    window->btnBack->onClick = [window]()
    {
        window->GoToPrevWindow();
    };

    return window;
}

Window* Menu::AddColorMenu(Window* parent, CRGBA* color)
{
    return AddColorMenu(parent, color, []() {});
}

Window* Menu::AddConfirmWindow(Window* parent, int textGxtId, std::function<void()> ohYes, std::function<void()> ohNo)
{
    auto screenSize = Input::GetGTAScreenSize();

    Window* window = AddWindow(22, parent);
    window->width = 200.0f;
    window->position = { screenSize.x/2 - window->width/2, 200 };

    window->AddText(textGxtId);

    auto button_yes = window->AddButton(24);
    button_yes->onClick = [window, ohYes]()
    {
        window->GoToPrevWindow();
        ohYes();
    };

    auto button_no = window->AddButton(25, CRGBA(170, 70, 70));
    button_no->onClick = [window, ohNo]()
    {
        window->GoToPrevWindow();
        ohNo();
    };

    return window;
}

void Menu::ShowPopup(int gfxId, int val1, int val2, int time, float width)
{
    m_PopUp->gfxId = gfxId;
    m_PopUp->val1 = val1;
    m_PopUp->val2 = val2;
    m_PopUp->timeLeft = time;
    m_PopUp->width = width;
}

void Menu::ShowCredits(int gfxId, int time)
{
    m_Credits->gfxId = gfxId;
    m_Credits->time = time;
    m_Credits->timeElapsed = 0;
    m_Credits->hasShownCredits = true;
}

void Menu::Update(int dt)
{
    if(!m_Visible) return;

    //popup
    m_PopUp->timeLeft -= dt;
    if (m_PopUp->timeLeft < 0) m_PopUp->timeLeft = 0;

    //credits
    if(m_Credits->time > 0) {
        m_Credits->timeElapsed += dt;
        if(m_Credits->timeElapsed >= m_Credits->time)
        {
            m_Credits->timeElapsed = 0;
            m_Credits->time = 0;
        }
    }
    

    m_MainWindow->Update();

    std::vector<Window*> windowsToRemove; //to fix a crash that i still cant believe it never happened before

    for (auto window : m_Windows)
    {
        window->Update();

        if(window->canBeRemoved) windowsToRemove.push_back(window);
    }

    for (auto window : windowsToRemove)
    {
        RemoveWindow(window);
    }

    if(windowsToRemove.size() > 0)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Removed " << windowsToRemove.size() << " windows" << std::endl;
    }
}

void Menu::Draw()
{
    if(!m_Visible) return;
    
    m_MainWindow->showTitle = false;
    m_MainWindow->Draw();

    for (auto window : m_Windows)
    {
        window->Draw();
    }

    if (m_PopUp->timeLeft > 0)
    {
        float x = 20;
        float y = 200;

        float titleH = 20;
        float hoxH = 60;

        float w = m_PopUp->width;

        CRGBA white = { 255, 255, 255, 255 };

        Draw::DrawBoxWithText(22, 100, 200, { x, y }, { w, titleH }, { 0, 77, 132, 255 }, white);
        y += 20;
        Draw::DrawBoxWithText(m_PopUp->gfxId, m_PopUp->val1, m_PopUp->val2, { x, y }, { w, hoxH }, { 0, 119, 204, 255 }, white);
    }

    if (m_Credits->time > 0)
    {
        auto screenSize = Input::GetGTAScreenSize();

        float boxW = 300;
        float hoxH = 50;

        float x = screenSize.x/2 - boxW/2;
        float y = 80;

        unsigned char alpha = 255;

        if(m_Credits->timeElapsed >= 0 && m_Credits->timeElapsed < m_Credits->fadeTime)
        {
            float in = ((float)m_Credits->fadeTime - (float)m_Credits->timeElapsed) / (float)m_Credits->fadeTime;
            alpha = 255 - ucharIntensity(255, in);
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "in: " << in << std::endl;
        }
        if(m_Credits->timeElapsed >= m_Credits->time - m_Credits->fadeTime && m_Credits->timeElapsed <= m_Credits->time)
        {
            float out = ((float)m_Credits->time - (float)m_Credits->timeElapsed) / (float)m_Credits->fadeTime;
            alpha = ucharIntensity(255, out);
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "out: " << out << std::endl;
        }

        CRGBA textColor = { 255, 255, 255, alpha };
        auto boxColor = GetStyle()->COLOR_BACKGROUND;
        boxColor.a = alpha;

        Draw::DrawBoxWithText(m_Credits->gfxId, 0, 0, { x, y }, { boxW, hoxH }, boxColor, textColor);
    }
    
    //cursor
    if (m_DrawCursor)
    {
        CVector2D pos = Input::GetTouchPos();
        CRGBA colorOn = { 0, 255, 0, 255 };
        CRGBA colorOff = { 255, 0, 0, 255 };

        Draw::DrawBox({ pos.x - 3, pos.y - 3 }, { 6, 6 }, (Input::isTouchPressed ? colorOn : colorOff));
    }
}

void Menu::RemoveWindow(Window* window)
{
    if (window == m_MainWindow) return;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Menu: RemoveWindow" << std::endl;

    auto it = std::find(m_Windows.begin(), m_Windows.end(), window);
    if (it == m_Windows.end()) return;

    //if (m_ActiveWindow == window) m_ActiveWindow = nullptr;

    window->Destroy();
    m_Windows.erase(it);
    delete window;
}

void Menu::RemoveAllWindows()
{
    for(auto window : m_Windows)
    {
        if(window == m_MainWindow) continue;
        window->canBeRemoved = true;
    }
}

MenuStyle* Menu::GetStyle()
{
    return &m_Styles[m_StyleId];
}