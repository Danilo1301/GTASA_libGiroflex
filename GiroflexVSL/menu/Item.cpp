#include "Item.h"

#include "../Log.h"
#include "../Input.h"

Item::Item(eItemType_old type)
{
	this->type = type;

	if (type == eItemType_old::ITEM_BUTTON_old)
	{
		drawLabel = false;
	}

	if (type == eItemType_old::ITEM_OPTIONS_old || type == eItemType_old::ITEM_INT_RANGE_old || type == eItemType_old::ITEM_FLOAT_RANGE_old)
	{
		btnLeft = new Item(eItemType_old::ITEM_BUTTON_old);

		btnRight = new Item(eItemType_old::ITEM_BUTTON_old);
	}

	if (type == eItemType_old::CHECKBOX_old)
	{
		auto self = this;

		onClick = [self]() {
			*self->pCheckBoxBool = !*self->pCheckBoxBool;

			if (self->onValueChange) self->onValueChange();
		};
	}
}

void Item::AddColorIndicator(CRGBA* color)
{
	ColorIndicator colorIndicator = { color };
	colorIndicators.push_back(colorIndicator);
}

void Item::AddExtraText(int gxtId, int num1, int num2, CVector2D offsetFromRight)
{
	DrawItem* drawItem = new DrawItem(eDrawType::TEXT);

	drawItem->gxtId = gxtId;
	drawItem->num1 = num1;
	drawItem->num2 = num2;
	drawItem->pos = offsetFromRight;

	extraTexts.push_back(drawItem);
}

void Item::AddOptionBy(int addBy)
{

}

void Item::AddOption(int gxtId, int num1, int num2)
{
	Option option = { gxtId, num1, num2 };
	options.push_back(option);
}

void Item::Update()
{
	hasPressedThisFrame = false;

	isPointerOver = Input::IsPointInsideRect(Input::GetTouchPos(), position, box->size);

	if (isPointerOver)
	{
		//if (Input::hasTouchBeenPressedThisFrame && onClick && type == eItemType_old::ITEM_BUTTON_old) onClick();
		if (Input::hasTouchBeenReleasedThisFrame && !waitingForTouchRelease)
		{
			if (type == eItemType_old::ITEM_BUTTON_old || type == eItemType_old::CHECKBOX_old)
			{	
				hasPressedThisFrame = true;

				if(onClick) onClick();
			}
		}

		if (!isPressed)
		{
			isPressed = true;
		}
	}

	if (!isPointerOver || !Input::isTouchPressed)
	{
		isPressed = false;
	}

	//

	if (btnLeft) btnLeft->Update();
	if (btnRight) btnRight->Update();
	
	//

	if (type == eItemType_old::ITEM_OPTIONS_old || type == eItemType_old::ITEM_INT_RANGE_old)
	{
		if (type == eItemType_old::ITEM_OPTIONS_old)
		{
			intValueRange.min = 0;
			intValueRange.max = options.size() - 1;
		}

		int prevVal = *intValueRange.value;

		if (btnLeft->isPointerOver)
		{
			if (holdToChange)
			{
				if (Input::isTouchPressed) intValueRange.ChangeValueBy(-intValueRange.addBy);
			}
			else {
				if(Input::hasTouchBeenPressedThisFrame) intValueRange.ChangeValueBy(-intValueRange.addBy);
			}
		}

		if (btnRight->isPointerOver)
		{
			if (holdToChange)
			{
				if (Input::isTouchPressed) intValueRange.ChangeValueBy(intValueRange.addBy);
			}
			else {
				if (Input::hasTouchBeenPressedThisFrame) intValueRange.ChangeValueBy(intValueRange.addBy);
			}
		}

		if (*intValueRange.value != prevVal)
		{
			if (onValueChange)
			{
				if (type == eItemType_old::ITEM_OPTIONS_old)
				{
					Log::Level(LOG_LEVEL::LOG_BOTH) << "Option changed from " << prevVal << " to " << *intValueRange.value << " - " << intValueRange.min << " / " << intValueRange.max << std::endl;
				}

				onValueChange();
			}
		}
	}

	//

	if (type == eItemType_old::ITEM_FLOAT_RANGE_old)
	{
		float prevVal = *floatValueRange.value;

		if (btnLeft->isPointerOver)
		{
			if (holdToChange)
			{
				if (Input::isTouchPressed) floatValueRange.ChangeValueBy(-floatValueRange.addBy);
			}
			else {
				if (Input::hasTouchBeenPressedThisFrame) floatValueRange.ChangeValueBy(-floatValueRange.addBy);
			}
		}

		if (btnRight->isPointerOver)
		{
			if (holdToChange)
			{
				if (Input::isTouchPressed) floatValueRange.ChangeValueBy(floatValueRange.addBy);
			}
			else {
				if (Input::hasTouchBeenPressedThisFrame) floatValueRange.ChangeValueBy(floatValueRange.addBy);
			}
		}

		if (*floatValueRange.value != prevVal)
		{
			if (onValueChange) onValueChange();
		}
	}

	//
}

void Item::Draw()
{
	if (type == eItemType_old::ITEM_BUTTON_old)
	{
		CRGBA boxColor = box->color;

		if (isPressed)
		{
			boxColor.r = 50;
			boxColor.g = 50;
			boxColor.b = 50;
		}

		Draw::DrawBoxWithText(text->gxtId, text->num1, text->num2, position, box->size, boxColor, text->color);
	}

	if (type == eItemType_old::ITEM_TEXT_old)
	{
		CVector2D textPos = position;
		textPos.x += box->size.x / 2.0f;
		textPos.y += box->size.y / 2.0f;

		Draw::DrawText(text->gxtId, text->num1, text->num2, textPos, text->color);
	}

	if (type == eItemType_old::ITEM_OPTIONS_old || type == eItemType_old::ITEM_INT_RANGE_old || type == eItemType_old::ITEM_FLOAT_RANGE_old)
	{
		text->gxtId = 1;
		text->color = CRGBA(0, 0, 0);

		if (type == eItemType_old::ITEM_FLOAT_RANGE_old && floatValueRange.value != NULL)
		{
			text->num1 = (int)floor((*floatValueRange.value) * 100.0f);
		}

		if (type == eItemType_old::ITEM_INT_RANGE_old && intValueRange.value != NULL)
		{
			text->num1 = *intValueRange.value;
		}

		if ((type == eItemType_old::ITEM_OPTIONS_old) && intValueRange.value != NULL)
		{
			if (options.size() > 0)
			{
				auto option = options[*intValueRange.value];

				text->gxtId = option.gxtId;
				text->num1 = option.num1;
				text->num2 = option.num2;
			}

			
		}

		Draw::DrawBoxWithText(text->gxtId, text->num1, text->num2, position, box->size, box->color, text->color);
	
		auto btnSize = CVector2D(btnWidth, box->size.y);
		auto btnColor = CRGBA(255, 255, 255);

		if (btnLeft)
		{
			btnLeft->box->size = btnSize;
			btnLeft->text->gxtId = 4;
			btnLeft->text->color = CRGBA(0, 0, 0);
			btnLeft->box->color = btnColor;
			btnLeft->position = position;
			btnLeft->Draw();
		}
		
		if (btnRight)
		{
			btnRight->box->size = btnSize;
			btnRight->text->gxtId = 5;
			btnRight->text->color = CRGBA(0, 0, 0);
			btnRight->box->color = btnColor;
			btnRight->position = CVector2D(position.x + box->size.x - btnSize.x, position.y);
			btnRight->Draw();
		}
	}

	//

	float padding = 2.0f;
	float h = box->size.y - padding * 2;
	float w = h;

	CVector2D pos = position;
	pos.x += box->size.x - w - padding;
	pos.y += padding;

	for (auto colorIndicator : colorIndicators)
	{
		Draw::DrawBox(pos, CVector2D(w, h), *colorIndicator.color);

		pos.x -= w + padding;
	}

	//

	for(auto text : extraTexts)
	{
		CVector2D textPos = position;
		textPos.x += box->size.x;
		textPos.y += box->size.y / 2.0f;

		textPos.x += text->pos.x;
		textPos.y += text->pos.y;

		Draw::DrawText(text->gxtId, text->num1, text->num2, textPos, CRGBA(255, 255, 255), eTextAlign::TEXT_ALIGN_RIGHT);
	}

	//

	if (type == eItemType_old::CHECKBOX_old)
	{
		Draw::DrawBox(pos, CVector2D(w, h), CRGBA(0, 0, 0));

		float b = 2.0f;

		pos.x += b;
		pos.y += b;
		w -= b * 2;
		h -= b * 2;

		Draw::DrawBox(pos, CVector2D(w, h), (*pCheckBoxBool) ? CRGBA(255, 255, 255) : CRGBA(51, 51, 51));
	}
}