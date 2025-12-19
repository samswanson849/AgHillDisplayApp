#pragma once 
#include "framework.h"
#include <vector>
#include "UIfactoryFingerprint.h"

class uiElement
{
private:
	
	RECT lastInvalid;
	RECT newInvalid;
	bool validSetup = false;
	bool shouldDraw = false;
	bool setRegionLast = false;
	void (*DeadCallback)(uiElement*,factoryFingerPrint*) = nullptr;
	factoryFingerPrint* obj;
	void (*UpdateCallback)(uiElement*, UINT , WPARAM , LPARAM );

protected:
	
	enum RectComparison { EQUAL, CONTAINS, INTERSECTS, OUTSIDE };
	enum RectContains { CONTAINED, EDGE, CORNER, NOTCONTAINED };
	HWND parentWindow = nullptr;
	
protected:
	
	uiElement::RectComparison InvalidCompare(RECT* a);
	
	uiElement::RectContains InvalidCompare(POINT* a);

	uiElement(HWND hwnd);
	
	~uiElement();

	bool runDraw();

	virtual void setToDraw(RECT* a);

	void setUpdateCallback(void (update)(uiElement*, UINT , WPARAM , LPARAM ));
	
	void updateDraw();

public:

	void Update(UINT msg, WPARAM wParam, LPARAM lParam);
	
	bool setDeadCallback(void (dead)(uiElement*,factoryFingerPrint*),factoryFingerPrint* inObj);

};

class attachableUI;

class positionUI : public uiElement {
private:
	
	RECT position;
	unsigned int layer;
	void (*drawFunction)(HDC,positionUI*) = nullptr;
	void (*customDraw)(HDC,RECT*) = nullptr;
	void (*layerchangeCallbac)(factoryFingerPrint*) = nullptr;
	factoryFingerPrint* factroyFingerPrint = nullptr;

public:

	enum AnchorLocation { CENTER, NORTH, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST };

private:
	
	void anchorRelative(positionUI::AnchorLocation loc, POINT* io);

protected:

	positionUI(HWND hwnd,RECT* positionin);

	~positionUI();

	void setToDraw();
	
	bool setLayerChangeCallback(void (*layerChanged)(factoryFingerPrint*),factoryFingerPrint* factoryFingerPrint);

	void setDrawFunc(void (*drawFunc)(HDC,positionUI*));

public:

	void Draw(HDC);

	void setCustomDraw(void (*drawFunc)(HDC,RECT*));

	void setLayer(unsigned int in);

	unsigned int getLayer();

	virtual void setPos(POINT* loc, positionUI::AnchorLocation anchor);

	void GetAnchor(positionUI::AnchorLocation loc, POINT* out);

	void getPosition(RECT* out);

	virtual void scale(float scale, positionUI::AnchorLocation loc);

	virtual void setPos(RECT* in);

};

struct attachment {
	positionUI::AnchorLocation to;
	positionUI::AnchorLocation from;
	attachableUI* connected;
	POINT margin;
};

struct attachDir {
	float x;
	float y;
};

class attachableUI : public positionUI {
private:

	std::vector<attachment>* attachments = nullptr;
	void (*parentChangedCallback)(attachableUI* obj) = nullptr;

private:

	void getDirection(attachment* att,attachDir* dir);

protected:

	attachableUI(HWND hwnd, RECT* positionin);

	void setChangeCallback(void (*parentCallback)(attachableUI* obj));

public:

	~attachableUI();

	void setPos(POINT* loc, positionUI::AnchorLocation anchor) override;

	void setPos(RECT* in) override;

	void scale(float scale, positionUI::AnchorLocation loc) override;

	void attach(attachableUI* connected, positionUI::AnchorLocation to, positionUI::AnchorLocation from, int offx, int offy);

	void attach(attachableUI* connected, positionUI::AnchorLocation to, positionUI::AnchorLocation from,unsigned int margin);

	void detach(attachableUI* connected);

};