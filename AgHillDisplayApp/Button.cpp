#include "Button.h"

Button::Button(HWND hwnd, buttonInfo* dim) : attachableUI(hwnd,nullptr) {
	RECT buttonBox;
	buttonBox.left = dim->x;
	buttonBox.top = dim->y;
	buttonBox.right = dim->x + dim->width;
	buttonBox.bottom = dim->y + dim->height;
	this->setPos(&buttonBox);
	this->radius.x = dim->radiusx;
	this->radius.y = dim->radiusy;
	this->changeName(dim->name,dim->margin);
	this->setColor(NDSUYELLOW, NDSULOWLIGHT, NDSULOWLIGHT);
	this->isTouching = false;
	this->isinside = false;
	this->down = false;
	this->setChangeCallback(this->changedCallback);
	this->setDrawFunc(this->DrawThunk);
	this->setUpdateCallback(this->UpdateThunk);
}

Button::~Button() {
	if (this->buttonRegion != nullptr)
		DeleteObject(this->buttonRegion);
}

void Button::setColor(COLORREF fill, COLORREF border, COLORREF text) {
	this->borderColor = border;
	this->textColor = text;
	this->fillColor = fill;
	int red = GetRValue(fill);
	int green = GetGValue(fill);
	int blue = GetBValue(fill);
	int delta = 20;
	int pRed = red - delta;
	if (pRed < 0)
		pRed = 0;
	int pGreen = green - delta;
	if (pGreen < 0)
		pGreen = 0;
	int pBlue = blue - delta;
	if (pBlue < 0)
		pBlue = 0;
	this->pressedColor = RGB(pRed,pGreen,pBlue);
	pRed = red + delta;
	if (pRed > 255)
		pRed = 255;
	pGreen = green + delta;
	if (pGreen > 255)
		pGreen = 255;
	pBlue = blue + delta;
	if (pBlue > 255)
		pBlue = 255;
	this->hoverColor = RGB(pRed, pGreen, pBlue);
	this->setToDraw();
}

void Button::setCallback(void (*func)(int x, int y, void* userDat), void* userData) {
	this->func = func;
	this->userData = userData;
}

void Button::UpdateThunk(uiElement* in, UINT message, WPARAM wParam, LPARAM lParam) {
	Button* button = dynamic_cast<Button*>(in);
	if (button == nullptr)
		return;
	button->UpdateCallback(message,wParam,lParam);
}

void Button::UpdateCallback(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_MOUSEMOVE:
		this->MouseIn(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		this->MouseDown(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		this->MouseUp(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_POINTERDOWN:
		this->isTouching = true;
		break;
	}
}

void Button::changedCallback(attachableUI* obj) {
	Button* but = dynamic_cast<Button*>(obj);
	if (but != nullptr)
		but->actualCallback();
}

void Button::actualCallback() {
	RECT buttonBox;
	this->getPosition(&buttonBox);
	unsigned int margin = this->textMargin;
	int buttonWidth = buttonBox.right - buttonBox.left;
	int textWidth = this->textBox.right - this->textBox.left;
	bool changedBox = false;
	if (textWidth + margin * 2 > buttonWidth) {
		buttonBox.right = buttonBox.left + textWidth + margin * 2;
		buttonWidth = buttonBox.right - buttonBox.left;
		changedBox = true;
	}
	int buttonHeight = buttonBox.bottom - buttonBox.top;
	int textHeight = this->textBox.bottom - this->textBox.top;
	if (textHeight + margin * 2 > buttonHeight) {
		buttonBox.bottom = buttonBox.top + textHeight + margin * 2;
		buttonHeight = buttonBox.bottom - buttonBox.top;
		changedBox = true;
	}
	int delta = (buttonWidth - textWidth) / 2;
	this->textBox.left = buttonBox.left + delta;
	this->textBox.right = this->textBox.left + textWidth;
	delta = (buttonHeight - textHeight) / 2;
	this->textBox.top = buttonBox.top + delta;
	this->textBox.bottom = this->textBox.top + textHeight;
	if (this->buttonRegion != nullptr)
		DeleteObject(this->buttonRegion);
	this->buttonRegion = CreateRoundRectRgn(buttonBox.left, buttonBox.top, buttonBox.right, buttonBox.bottom, this->radius.x, this->radius.y);
	if (changedBox)
		this->positionUI::setPos(&buttonBox);
}

void Button::MouseUp(int x, int y) {
	if (PtInRegion(this->buttonRegion, x, y)) {
		if (this->down) {
			if (this->func != nullptr)
				this->func(x,y,this->userData);
			if (this->isTouching)
				this->isTouching = false;
				this->isinside = false;
			this->down = false;
			this->setToDraw();
		}
		return;
	}
	this->down = false;
	this->setToDraw();
}

void Button::MouseDown(int x, int y) {
	if (PtInRegion(this->buttonRegion, x, y)) {
		this->down = true;
		this->setToDraw();
		return;
	}
	this->down = false;
	this->setToDraw();
}

void Button::MouseIn(int x, int y) {
	if (this->isTouching)
		return;
	if (PtInRegion(this->buttonRegion, x, y)) {
		if (!this->isinside) {
			this->setToDraw();
		}
		this->isinside = true;
		return;
	}
	if (this->isinside) {
		this->setToDraw();
	}
	this->isinside = false;
}

void Button::changeName(const char* name,unsigned short margin) {
	RECT buttonBox;
	this->getPosition(&buttonBox);
	this->textMargin = margin;
	this->textBox.left = buttonBox.left + margin;
	this->textBox.top = buttonBox.top + margin;
	this->textBox.right = buttonBox.right - margin;
	this->textBox.bottom = this->textBox.top;
	if (this->textBox.right < this->textBox.left)
		this->textBox.right = this->textBox.left;
	this->text = name;
	this->textLeng = std::strlen(name);
	HDC hdc = GetDC(this->parentWindow);
	DrawTextA(hdc, this->text, this->textLeng, &this->textBox, DT_CALCRECT);
	ReleaseDC(this->parentWindow, hdc);
	int buttonWidth = buttonBox.right - buttonBox.left;
	int textWidth = this->textBox.right - this->textBox.left;
	if (textWidth + margin * 2 > buttonWidth) {
		buttonBox.right = buttonBox.left + textWidth + margin * 2;
		buttonWidth = buttonBox.right - buttonBox.left;
	}
	int buttonHeight = buttonBox.bottom - buttonBox.top;
	int textHeight = this->textBox.bottom - this->textBox.top;
	if (textHeight + margin * 2 > buttonHeight) {
		buttonBox.bottom = buttonBox.top + textHeight + margin * 2;
		buttonHeight = buttonBox.bottom - buttonBox.top;
	}
	int delta = (buttonWidth - textWidth) / 2;
	this->textBox.left = buttonBox.left + delta;
	this->textBox.right = this->textBox.left + textWidth;
	delta = (buttonHeight - textHeight) / 2;
	this->textBox.top = buttonBox.top + delta;
	this->textBox.bottom = this->textBox.top + textHeight;
	if (this->buttonRegion != nullptr)
		DeleteObject(this->buttonRegion);
	this->buttonRegion = CreateRoundRectRgn(buttonBox.left, buttonBox.top, buttonBox.right, buttonBox.bottom, this->radius.x, this->radius.y);
	this->positionUI::setPos(&buttonBox);
}

void Button::getBounds(buttonInfo* dimOut) {
	RECT buttonBox;
	this->getPosition(&buttonBox);
	dimOut->x = buttonBox.left;
	dimOut->y = buttonBox.top;
	dimOut->width = buttonBox.right - buttonBox.left;
	dimOut->height = buttonBox.bottom - buttonBox.top;
	dimOut->radiusx = this->radius.x;
	dimOut->radiusy = this->radius.y;
	dimOut->margin = this->textMargin;
}

const char* Button::getName(int* lengthOut) {
	*lengthOut = this->textLeng;
	return this->text;
}

void Button::DrawThunk(HDC hdc,positionUI* in) {
	Button* data = dynamic_cast<Button*>(in);
	if (data == nullptr)
		return;
	data->DrawCall(hdc);
}

void Button::DrawCall(HDC hdc) {
	if (this->down) {
		SetDCBrushColor(hdc, this->pressedColor);
	}
	else if (this->isinside) {
		SetDCBrushColor(hdc, this->hoverColor);
	}
	else {
		SetDCBrushColor(hdc, this->fillColor);
	}
	SetDCPenColor(hdc, this->borderColor);
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SelectObject(hdc, GetStockObject(DC_PEN));
	RECT buttonBox;
	this->getPosition(&buttonBox);
	RoundRect(hdc, buttonBox.left, buttonBox.top, buttonBox.right, buttonBox.bottom, this->radius.x, this->radius.y);
	SetTextColor(hdc, this->textColor);
	if (this->down) {
		SetBkColor(hdc, this->pressedColor);
	}
	else if (this->isinside) {
		SetBkColor(hdc, this->hoverColor);
	}
	else {
		SetBkColor(hdc, this->fillColor);
	}
	DrawTextA(hdc, this->text, this->textLeng, &this->textBox, DT_CENTER);
}