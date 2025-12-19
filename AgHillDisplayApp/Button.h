#pragma once
#include <string>
#include "Colors.h"
#include "uiElement.h"

struct buttonInfo {
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	unsigned int radiusx;
	unsigned int radiusy;
	unsigned int margin;
	const char* name;
};

class Button : public attachableUI {
private:
	RECT textBox;
	POINT radius;
	const char* text = "";
	int textLeng = 0;
	HRGN buttonRegion = nullptr;
	bool isinside = false;
	bool down = true;
	COLORREF textColor = 0;
	COLORREF borderColor = 0;
	COLORREF fillColor = 0;
	COLORREF pressedColor = 0;
	COLORREF hoverColor = 0;
	void (*func)(int x, int y, void* userDat) = nullptr;
	void* userData = nullptr;
	bool isTouching;
	unsigned int textMargin;

private:

	static void changedCallback(attachableUI* obj);

	void actualCallback();

	static void DrawThunk(HDC hdc,positionUI* in);

	void DrawCall(HDC hdc);

	static void UpdateThunk(uiElement* in, UINT message, WPARAM wParam, LPARAM lParam);

	void UpdateCallback(UINT message, WPARAM wParam, LPARAM lParam);

public:
	
	Button(HWND hwnd, buttonInfo* dim);

	~Button();

	void setCallback(void (*func)(int x, int y, void* userDat),void* userData);

	void setColor(COLORREF fill,COLORREF border,COLORREF text);

	void getBounds(buttonInfo* dimOut);

	const char* getName(int* lengthOut);

	void changeName(const char* name, unsigned short margin);

	void MouseIn(int x, int y);

	void MouseDown(int x, int y);

	void MouseUp( int x, int y);
};