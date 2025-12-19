#pragma once
#include "uiElement.h"
#include "framework.h"
#include <string>
#include "Colors.h"
#include <vector>

struct tabDim {
	int width;
	int textWidth;
	unsigned int margin;
	int height;
	int textHeight;
	const char* name;
	unsigned int textLeng;
	RECT textDrawDim;
	RECT drawDim;
	HRGN region;
};

struct folderTab {
	const char* name;
	unsigned int margin;
};

struct folderInfo {
	int x;
	int y;
	int width;
	int height;
	folderTab* tabs;
	unsigned int tabNumber;
};

class folderSelector : public attachableUI {
private:
	std::vector<tabDim>* tabs = nullptr;
	unsigned int selectedTab = 0;
	void (*callFunc)(int x, int y, void* userDat);
	void* userData;

	unsigned int fillDrawArray(bool selected, POINT* ptArray, BYTE* typeArray,int x,int y,int width,int height);

	void setupTextDim(tabDim* io, unsigned int drawWidth);

	static void changedCallback(attachableUI* obj);

	void actualCallback();

	static void DrawThunk(HDC hdc,positionUI* in);

	void DrawCall(HDC hdc);
	
	static void UpdateThunk(uiElement* in, UINT message, WPARAM wParam, LPARAM lParam);

	void UpdateCallback(UINT message, WPARAM wParam, LPARAM lParam);
	
	void MouseUp(int x, int y);

public:

	folderSelector(HWND hwnd, folderInfo* in);

	~folderSelector();

	void setCallback(void (*func)(int x, int y, void* userDat), void* userData);
	
	void setSelected(unsigned int select);

	void addTab(const char* tabName, unsigned int margin);
	
	unsigned int getSelected();
};