
#include "../Log.h"
#include "../Input.h"
#include "Menu.h"

CVector2D Window::m_DefaultWindowPosition = CVector2D(200, 200);
float Window::m_DefaultWindowWidth = 320.0f;

Window::Window()
{
	Log::Level(LOG_LEVEL::LOG_BOTH) << "[Window] Constructor" << std::endl;

	width = m_DefaultWindowWidth;

	btnLeft = new Item(eItemType_old::ITEM_BUTTON_old);
	btnRight = new Item(eItemType_old::ITEM_BUTTON_old);
	btnBack = new Item(eItemType_old::ITEM_BUTTON_old);

	auto window = this;

	btnRight->onClick = [window]()
	{
		window->page += 1;

		int maxPages = window->GetMaxPages();
		if (window->page >= maxPages-1) window->page = maxPages-1;
	};

	btnLeft->onClick = [window]()
	{
		window->page -= 1;
		if (window->page < 0) window->page = 0;
	};

	//fix issue when accidentally clicking on an item
	if(Input::isTouchPressed)
	{	
		Log::Level(LOG_LEVEL::LOG_BOTH) << "[Window] Blocking buttons while input is not released" << std::endl;
		waitingForTouchRelease = true;
	}
}

void Window::AddItem(Item* item)
{
	item->waitingForTouchRelease = waitingForTouchRelease;

	items.push_back(item);
}

void Window::AddFloatingItem(Item* item)
{
	item->waitingForTouchRelease = waitingForTouchRelease;

	floatingItems.push_back(item);
}

Item* Window::AddButton(int gxtId, int num1, int num2, CRGBA color)
{
	Item* item = new Item(eItemType_old::ITEM_BUTTON_old);
	item->drawLabel = false;

	item->text->gxtId = gxtId;
	item->text->num1 = num1;
	item->text->num2 = num2;
	
	item->box->color = color;
	item->box->size = { 200, 35 };
	item->useFullWidth = true;

	AddItem(item);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddButton" << std::endl;

	return item;
}

Item* Window::AddButton(int gxtId)
{
	return AddButton(gxtId, 0, 0, Menu::GetStyle()->COLOR_BUTTON_DEFAULT);	
}

Item* Window::AddButton(int gxtId, int num1, int num2)
{
	return AddButton(gxtId, num1, num2, Menu::GetStyle()->COLOR_BUTTON_DEFAULT);	
}

Item* Window::AddButton(int gxtId, CRGBA color)
{
	return AddButton(gxtId, 0, 0, color);
}

Item* Window::AddFloatingButton(int gxtId, int num1, int num2, CVector2D position, CVector2D size, CRGBA color)
{
	Item* item = new Item(eItemType_old::ITEM_BUTTON_old);
	item->drawLabel = false;
	//item->useFullWidth = true;
	item->position = position;

	item->text->gxtId = gxtId;
	item->text->num1 = num1;
	item->text->num2 = num2;

	item->box->color = color;
	item->box->size = size;

	AddFloatingItem(item);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddFloatingButton" << std::endl;

	return item;
}

Item* Window::AddFloatingButton(int gxtId, int num1, int num2, CVector2D position, CVector2D size)
{
	return AddFloatingButton(gxtId, num1, num2, position, size, Menu::GetStyle()->COLOR_BUTTON_DEFAULT);
}

Item* Window::AddCheckbox(int gxtId, bool* value)
{
	Item* item = new Item(eItemType_old::CHECKBOX_old);
	item->drawBox = false;
	item->useFullWidth = true;

	item->pCheckBoxBool = value;
	if (value == NULL) item->pCheckBoxBool = &item->tmpCheckBoxBool;

	item->label->gxtId = gxtId;

	item->box->size = { 200, 35 };

	AddItem(item);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddCheckbox" << std::endl;

	return item;
}

Item* Window::AddOptions(int gxtId)
{
	Item* item = new Item(eItemType_old::ITEM_OPTIONS_old);
	item->label->gxtId = gxtId;

	item->intValueRange.value = &item->optionsValue;

	item->box->color = CRGBA(120, 120, 120);
	item->box->size = { 178, 35 };

	AddItem(item);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddOptions" << std::endl;

	return item;
}

Item* Window::AddFloatRange(int gxtId, float* value, float min, float max, float addBy)
{
	Item* item = new Item(eItemType_old::ITEM_FLOAT_RANGE_old);
	item->label->gxtId = gxtId;
	item->floatValueRange.value = value;
	item->floatValueRange.min = min;
	item->floatValueRange.max = max;
	item->floatValueRange.addBy = addBy;
	item->holdToChange = true;

	item->box->color = CRGBA(120, 120, 120);
	item->box->size = { 150, 35 };

	AddItem(item);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddFloatRange" << std::endl;

	return item;
}


Item* Window::AddIntRange(int gxtId, int* value, int min, int max, int addBy)
{
	Item* item = new Item(eItemType_old::ITEM_INT_RANGE_old);
	item->label->gxtId = gxtId;
	item->intValueRange.value = value;
	item->intValueRange.min = min;
	item->intValueRange.max = max;
	item->intValueRange.addBy = addBy;
	item->holdToChange = true;

	item->box->color = CRGBA(120, 120, 120);
	item->box->size = { 150, 35 };

	AddItem(item);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddIntRange" << std::endl;

	return item;
}

Item* Window::AddText(int gxtId, int num1, int num2, CRGBA color)
{
	Item* item = new Item(eItemType_old::ITEM_TEXT_old);

	item->drawLabel = false;

	item->text->gxtId = gxtId;
	item->text->num1 = num1;
	item->text->num2 = num2;
	item->text->color = color;
	item->useFullWidth = true;
	item->box->size.y = 25;

	AddItem(item);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Window: AddText" << std::endl;

	return item;
}

Item* Window::AddText(int gxtId)
{
	return AddText(gxtId, 0, 0, CRGBA(255, 255, 255));
}



void Window::Update()
{
	if (!visible) return;

	if (showPageControls)
	{
		btnLeft->Update();
		btnRight->Update();
		btnBack->Update();
	}

	auto itemsToDraw = GetItemsToDraw();

	if(waitingForTouchRelease)
	{
		if(!Input::isTouchPressed && !Input::hasTouchBeenReleasedThisFrame)
		{
			waitingForTouchRelease = false;
			Log::Level(LOG_LEVEL::LOG_BOTH) << "[Window] You can now press buttons safely" << std::endl;
		}

		for(auto item : GetTotalItems()) item->waitingForTouchRelease = waitingForTouchRelease;
	}

    for (auto item : itemsToDraw)
    {
        item->Update();
    }

	for (auto item : floatingItems)
    {
        item->Update();
    }
}

void Window::Draw()
{
	if (!visible) return;

	CVector2D pos = position;
	auto style = Menu::GetStyle();

	if(showTitle)
		Draw::DrawBoxWithText(titleGtxId, 0, 0, pos, CVector2D(width, titleHeight), style->COLOR_TITLE_BOX, CRGBA(255, 255, 255), eTextAlign::TEXT_ALIGN_LEFT);

	//

	if (showPageControls)
	{
		auto btnSize = CVector2D(40.0f, 40.0f);
		auto btnColor = style->COLOR_BACKGROUND;

		bool canGoLeft = page > 0;
		bool canGoRight = page < GetMaxPages() - 1;

		if (canGoLeft)
		{
			btnLeft->box->size = btnSize;
			btnLeft->text->gxtId = 4; //<
			btnLeft->box->color = btnColor;
			btnLeft->position = CVector2D(position.x - 20.0f - btnSize.x, position.y + 20.0f);
			btnLeft->Draw();
		}
		
		if (canGoRight)
		{
			btnRight->box->size = btnSize;
			btnRight->text->gxtId = 5; //>
			btnRight->box->color = btnColor;
			btnRight->position = CVector2D(position.x + width + 20.0f, position.y + 20.0f);
			btnRight->Draw();
		}
		
		btnSize = CVector2D(80.0f, 40.0f);

		if (parentWindow != NULL)
		{
			btnBack->box->size = btnSize;
			btnBack->text->gxtId = 7; //close
			btnBack->box->color = btnColor;
			btnBack->position = CVector2D(position.x + width + 20.0f, position.y + 80.0f);
			btnBack->Draw();
		}
	}
	//

	pos.y += titleHeight;

	
	auto itemsToDraw = GetItemsToDraw();

    for (auto item : itemsToDraw)
    {
		float padding = 2.0f;

		if (item->useFullWidth) item->box->size.x = width - padding*2;

		Draw::DrawBox(pos, CVector2D(width, item->box->size.y + padding * 2), style->COLOR_BACKGROUND);

		pos.y += padding;

		if (item->drawLabel)
		{
			Draw::DrawText(item->label->gxtId, item->label->num1, item->label->num2, CVector2D(pos.x, pos.y + item->box->size.y / 2), item->label->color, eTextAlign::TEXT_ALIGN_LEFT);
		}


		float totalPadding = width - item->box->size.x;

		//draw at center
		//item->position = CVector2D(pos.x + totalPadding/2, pos.y); 

		//draw at right
		item->position = CVector2D(pos.x + totalPadding - padding, pos.y);

		item->Draw();

		pos.y += item->box->size.y;
		pos.y += padding;
    }

	//draw floating items
	for (auto item : floatingItems)
    {
		item->Draw();
	}
}

void Window::Destroy()
{
	//wait, i just realised that items never get destroyed...
}

std::vector<Item*> Window::GetItemsToDraw()
{
	//int maxPages = (int)std::ceil((float)items.size() / (float)maxItemsPerPage);
	int startIndex = (page * maxItemsPerPage);
	//int endIndex = startIndex + maxItemsPerPage;

	std::vector<Item*> itemsToDraw;

	for (size_t i = 0; i < items.size(); i++)
	{
		if ((int)i >= startIndex && (int)i < (startIndex + maxItemsPerPage))
		{
			auto item = items[i];

			itemsToDraw.push_back(item);
			//totalHeight += item->m_Size.y;
		}
	}

	return itemsToDraw;
}

std::vector<Item*> Window::GetTotalItems()
{
	std::vector<Item*> totalItems;

	for(auto item : items) totalItems.push_back(item);
	for(auto item : floatingItems) totalItems.push_back(item);

	return totalItems;
}

void Window::GoToPrevWindow()
{
	if (!parentWindow) return;
	parentWindow->visible = true;
	RemoveThisWindow();
}

void Window::RemoveThisWindow()
{
	canBeRemoved = true;
	Log::Level(LOG_LEVEL::LOG_BOTH) << "Set window to be removed" << std::endl;
	//Menu::RemoveWindow(this);
}

int Window::GetMaxPages()
{
	return (int)std::ceil((float)items.size() / (float)maxItemsPerPage);
}