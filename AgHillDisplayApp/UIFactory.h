#pragma once

#include <algorithm>
#include "Button.h"
#include "folderSelect.h"
#include "UIfactoryFingerprint.h"

class UIFactory : public factoryFingerPrint {
private:

	static std::vector<UIFactory*>* instances;
	std::vector<attachableUI*>* uiElems = nullptr;
	HWND window = nullptr;

	UIFactory(HWND hwnd);

	~UIFactory();

	void SortLayers();

	static void layerChangedCallbackThunk(factoryFingerPrint* fingerprint);
	
	static void deadCallbackThunk(uiElement* obj, factoryFingerPrint* fingerPrint);

	void deadCallback(uiElement* elem);

	void individualUpdate(UINT message, WPARAM wParam, LPARAM lParam);
	
public:

	static void Update(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	UIFactory(UIFactory& factory) = delete;

	static UIFactory* getInstance(HWND window);
		
	Button* getButton(buttonInfo* bi);

	folderSelector* getFolderSelector(folderInfo* fi);

};


