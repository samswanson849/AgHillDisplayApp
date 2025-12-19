#include "UIFactory.h"

std::vector<UIFactory*>* UIFactory::instances = nullptr;

UIFactory::UIFactory(HWND hwnd) : factoryFingerPrint() {
	this->window = hwnd;
	this->uiElems = new std::vector<attachableUI*>();
}

UIFactory::~UIFactory() {
	if (this->uiElems != nullptr) {
		for (int i = 0; i < this->uiElems->size(); i++) {
			if (this->uiElems->at(i) != nullptr)
				delete this->uiElems->at(i);
		}
		delete this->uiElems;
	}
}

void UIFactory::SortLayers() {
	std::sort(uiElems->begin(), uiElems->end(),
		[](attachableUI* a, attachableUI* b) {
			int layera = a->getLayer();
			int layerb = b->getLayer();
			return layera < layerb;
		}
	);
}

void UIFactory::layerChangedCallbackThunk(factoryFingerPrint* fingerprint) {
	UIFactory* factory = dynamic_cast<UIFactory*>(fingerprint);
	if (factory == nullptr)
		return;
	factory->SortLayers();
}

void UIFactory::deadCallbackThunk(uiElement* obj, factoryFingerPrint* fingerPrint) {
	UIFactory* factory = dynamic_cast<UIFactory*>(fingerPrint);
	if (factory == nullptr)
		return;
	factory->deadCallback(obj);
}

void UIFactory::deadCallback(uiElement* elem) {
	if (this->uiElems == nullptr)
		return;
	std::vector<attachableUI*>* tempUI = new std::vector<attachableUI*>();
	for (int i = 0; i < this->uiElems->size(); i++) {
		if (this->uiElems->at(i) != elem) {
			tempUI->push_back(this->uiElems->at(i));
		}
	}
	delete this->uiElems;
	this->uiElems = tempUI;
}

void UIFactory::individualUpdate(UINT message, WPARAM wParam, LPARAM lParam) {
	if (this->uiElems == nullptr)
		return;
	if (message == WM_DESTROY) {
		if (UIFactory::instances == nullptr)
			return;
		std::vector<UIFactory*>* tempInstances = new std::vector<UIFactory*>();
		for (int i = 0; i < UIFactory::instances->size(); i++) {
			if (UIFactory::instances->at(i) != this) {
				tempInstances->push_back(UIFactory::instances->at(i));
			}
		}
		delete UIFactory::instances;
		if (tempInstances->size() == 0) {
			delete tempInstances;
			UIFactory::instances = nullptr;
			delete this;
			return;
		}
		UIFactory::instances = tempInstances;
		delete this;
		return;
	}
	for (int i = 0; i < this->uiElems->size(); i++) {
		this->uiElems->at(i)->Update(message, wParam, lParam);
	}
	if (message == WM_PAINT) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(this->window, &ps);
		for (int i = 0; i < this->uiElems->size(); i++) {
			uiElems->at(i)->Draw(hdc);
		}
		EndPaint(this->window, &ps);
	}
}

void UIFactory::Update(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (UIFactory::instances == nullptr)
		return;
	for (int i = 0; i < UIFactory::instances->size(); i++) {
		if (UIFactory::instances->at(i) != nullptr && UIFactory::instances->at(i)->window == hwnd) {
			UIFactory::instances->at(i)->individualUpdate(message, wParam, lParam);
			if (UIFactory::instances == nullptr || UIFactory::instances->size() <= i)
				return;
		}
	}
}

UIFactory* UIFactory::getInstance(HWND window) {
	if (UIFactory::instances == nullptr) {
		UIFactory::instances = new std::vector<UIFactory*>();
	}
	for (int i = 0; i < UIFactory::instances->size(); i++) {
		if (UIFactory::instances->at(i)->window == window)
			return UIFactory::instances->at(i);
	}
	UIFactory::instances->push_back(new UIFactory(window));
	return  UIFactory::instances->back();
}

Button* UIFactory::getButton(buttonInfo* bi) {
	if (this->window == nullptr)
		return nullptr;
	Button* newButton = new Button(this->window, bi);
	newButton->setDeadCallback(UIFactory::deadCallbackThunk, this);
	uiElems->push_back(newButton);
	this->SortLayers();
	return newButton;
}

folderSelector* UIFactory::getFolderSelector(folderInfo* fi) {
	if (this->window == nullptr)
		return nullptr;
	folderSelector* newFolderSelector = new folderSelector(this->window, fi);
	newFolderSelector->setDeadCallback(UIFactory::deadCallbackThunk, this);
	uiElems->push_back(newFolderSelector);
	this->SortLayers();
	return newFolderSelector;
}