#include "uiElement.h"

uiElement::uiElement(HWND hwnd) {
	this->setRegionLast = false;
	this->validSetup = false;
	this->shouldDraw = true;
	this->parentWindow = hwnd;
	this->DeadCallback = nullptr;
	this->obj = nullptr;
	this->UpdateCallback = nullptr;	
}

uiElement::~uiElement() {
	if (this->DeadCallback == nullptr || this->obj == nullptr)
		return;
	this->DeadCallback(this,this->obj);
}

void uiElement::setUpdateCallback(void (update)(uiElement*, UINT , WPARAM , LPARAM )) {
	if (update == nullptr)
		return;
	this->UpdateCallback = update;
}

void uiElement::Update(UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_PAINT:
		this->updateDraw();
	}
	if (this->UpdateCallback == nullptr)
		return;
	this->UpdateCallback(this, msg, wParam, lParam);
}

bool uiElement::setDeadCallback(void (dead)(uiElement*,factoryFingerPrint*),factoryFingerPrint* inObj) {
	if (this->DeadCallback == nullptr && inObj != nullptr) {
		this->DeadCallback = dead;
		this->obj = inObj;
		return true;
	}
	return false;
}

uiElement::RectComparison uiElement::InvalidCompare(RECT* a) {
	if (   this->lastInvalid.left == a->left
		&& this->lastInvalid.right == a->right
		&& this->lastInvalid.top == a->top
		&& this->lastInvalid.bottom == a->bottom)
		return uiElement::RectComparison::EQUAL;
	if (   this->lastInvalid.left <= a->left
		&& this->lastInvalid.right >= a->right
		&& this->lastInvalid.top <= a->top
		&& this->lastInvalid.bottom >= a->bottom)
		return uiElement::RectComparison::CONTAINS;
	POINT p;
	p.x = a->left;
	p.y = a->top;
	if (this->InvalidCompare(&p) == uiElement::RectContains::CONTAINED)
		return uiElement::RectComparison::INTERSECTS;
	p.x = a->left;
	p.y = a->bottom;
	if (this->InvalidCompare(&p) == uiElement::RectContains::CONTAINED)
		return uiElement::RectComparison::INTERSECTS;
	p.x = a->right;
	p.y = a->top;
	if (this->InvalidCompare(&p) == uiElement::RectContains::CONTAINED)
		return uiElement::RectComparison::INTERSECTS;
	p.x = a->right;
	p.y = a->bottom;
	if (this->InvalidCompare(&p) == uiElement::RectContains::CONTAINED)
		return uiElement::RectComparison::INTERSECTS;
	return uiElement::RectComparison::OUTSIDE;
}

uiElement::RectContains uiElement::InvalidCompare(POINT* a) {
	if ((this->lastInvalid.left == a->x && this->lastInvalid.top == a->y)
		|| (this->lastInvalid.right == a->x && this->lastInvalid.top == a->y)
		|| (this->lastInvalid.left == a->x && this->lastInvalid.bottom == a->y)
		|| (this->lastInvalid.right == a->x && this->lastInvalid.bottom == a->y))
		return uiElement::RectContains::CORNER;
	if ((
		(this->lastInvalid.left == a->x || this->lastInvalid.right == a->x)
		&& this->lastInvalid.top < a->y
		&& this->lastInvalid.bottom > a->y
		)
		|| 
		(
		(this->lastInvalid.top == a->y || this->lastInvalid.bottom == a->y)
		&& this->lastInvalid.left < a->x
		&& this->lastInvalid.right > a->x
		)) {
		return uiElement::RectContains::EDGE;
	}
	if (this->lastInvalid.left < a->x
		&& this->lastInvalid.right > a->x
		&& this->lastInvalid.top < a->y
		&& this->lastInvalid.bottom > a->y)
		return uiElement::RectContains::CONTAINED;
	return uiElement::RectContains::NOTCONTAINED;
}

void uiElement::updateDraw() {
	if (this->parentWindow == nullptr)
		return;
	if (this->setRegionLast) {
		InvalidateRect(this->parentWindow, &this->lastInvalid, TRUE);
		this->setRegionLast = false;
	}
	memcpy(&this->lastInvalid, &this->newInvalid, sizeof(RECT));
}

void uiElement::setToDraw(RECT* a) {
	if (!this->validSetup) {
		this->validSetup = true;
		memcpy(&this->lastInvalid,a,sizeof(RECT));
		memcpy(&this->newInvalid, a, sizeof(RECT));
		this->shouldDraw = true;
		this->setRegionLast = false;
		InvalidateRect(this->parentWindow, &this->newInvalid, FALSE);
		return;
	}
	switch (this->InvalidCompare(a)) {
	case uiElement::RectComparison::EQUAL:
		if (!this->shouldDraw) {
			memcpy(&this->newInvalid, a, sizeof(RECT));
			this->shouldDraw = true;
			this->setRegionLast = false;
			InvalidateRect(this->parentWindow, &this->newInvalid, FALSE);
			return;
		}
		this->shouldDraw = true;
		this->setRegionLast = false;
		
		return;
	case uiElement::RectComparison::CONTAINS:
		memcpy(&this->newInvalid, a, sizeof(RECT));
		this->shouldDraw = true;
		this->setRegionLast = true;
		InvalidateRect(this->parentWindow, &this->newInvalid, FALSE);
		return;
	}
	memcpy(&this->newInvalid, a, sizeof(RECT));
	this->shouldDraw = true;
	this->setRegionLast = true;
	InvalidateRect(this->parentWindow, &this->newInvalid, FALSE);
	return;
}

bool uiElement::runDraw() {
	if (this->parentWindow == nullptr)
		return false;
	if (this->shouldDraw) {
		this->shouldDraw = false;
		return true;
	}
	return false;
}

void positionUI::anchorRelative(positionUI::AnchorLocation loc, POINT* io) {
	if (positionUI::AnchorLocation::NORTHWEST == loc)
		return;
	int width = this->position.right - this->position.left;
	int height = this->position.bottom - this->position.top;
	switch (loc) {
	case positionUI::AnchorLocation::CENTER:
		io->x = io->x - width / 2;
		io->y = io->y - height / 2;
		return;
	case positionUI::AnchorLocation::NORTH:
		io->x = io->x - width / 2;
		return;
	case positionUI::AnchorLocation::NORTHEAST:
		io->x = io->x - width;
		return;
	case positionUI::AnchorLocation::EAST:
		io->x = io->x - width;
		io->y = io->y - height / 2;
		return;
	case positionUI::AnchorLocation::SOUTHEAST:
		io->x = io->x - width;
		io->y = io->y - height;
		return;
	case positionUI::AnchorLocation::SOUTH:
		io->x = io->x - width / 2;
		io->y = io->y - height;
		return;
	case positionUI::AnchorLocation::SOUTHWEST:
		io->y = io->y - height;
		return;
	case positionUI::AnchorLocation::WEST:
		io->y = io->y - height / 2;
		return;
	}
}

void positionUI::GetAnchor(positionUI::AnchorLocation loc, POINT* out) {
	int width = this->position.right - this->position.left;
	int height = this->position.bottom - this->position.top;
	int x = this->position.left;
	int y = this->position.top;
	switch (loc) {
	case positionUI::AnchorLocation::CENTER:
		out->x = x + width / 2;
		out->y = y + height / 2;
		return;
	case positionUI::AnchorLocation::NORTH:
		out->x = x + width / 2;
		out->y = y;
		return;
	case positionUI::AnchorLocation::NORTHEAST:
		out->x = x + width;
		out->y = y;
		return;
	case positionUI::AnchorLocation::EAST:
		out->x = x + width;
		out->y = y + height / 2;
		return;
	case positionUI::AnchorLocation::SOUTHEAST:
		out->x = x + width;
		out->y = y + height;
		return;
	case positionUI::AnchorLocation::SOUTH:
		out->x = x + width / 2;
		out->y = y + height;
		return;
	case positionUI::AnchorLocation::SOUTHWEST:
		out->x = x;
		out->y = y + height;
		return;
	case positionUI::AnchorLocation::WEST:
		out->x = x;
		out->y = y + height / 2;
		return;
	case positionUI::AnchorLocation::NORTHWEST:
		out->x = x;
		out->y = y;
		return;
	}
}

positionUI::positionUI(HWND hwnd, RECT* positionin) : uiElement(hwnd) {
	this->layer = 0;
	this->drawFunction = nullptr;
	this->customDraw = nullptr;
	this->layerchangeCallbac = nullptr;
	this->factroyFingerPrint = nullptr;
	if (positionin == nullptr) {
		this->position.left = 0;
		this->position.right = 0;
		this->position.top = 0;
		this->position.bottom = 0;
		return;
	}
	memcpy(&this->position, positionin, sizeof(RECT));
	if (this->position.left > this->position.right)
		this->position.right = this->position.left;
	if (this->position.top > this->position.bottom)
		this->position.bottom = this->position.top;
	this->uiElement::setToDraw(&this->position);
}

positionUI::~positionUI() {

}

bool positionUI::setLayerChangeCallback(void (*layerChanged)(factoryFingerPrint*), factoryFingerPrint* factoryFingerPrint) {
	if (layerChanged == nullptr || factoryFingerPrint == nullptr)
		return false;
	if (this->layerchangeCallbac == nullptr && this->factroyFingerPrint == nullptr) {
		this->layerchangeCallbac = layerChanged;
		this->factroyFingerPrint = factoryFingerPrint;
		return true;
	}
	return false;
}

void positionUI::setPos(POINT* loc, positionUI::AnchorLocation anchor) {
	POINT temp;
	temp.x = loc->x;
	temp.y = loc->y;
	this->anchorRelative(anchor,&temp);
	int deltaX = temp.x - this->position.left;
	int deltaY = temp.y - this->position.top;
	this->position.left += deltaX;
	this->position.right += deltaX;
	this->position.top += deltaY;
	this->position.bottom += deltaY;
	this->uiElement::setToDraw(&this->position);
}

void positionUI::getPosition(RECT* out) {
	memcpy(out, &this->position, sizeof(RECT));
}

void positionUI::scale(float scale, positionUI::AnchorLocation loc) {
	POINT scaleCenter;
	this->GetAnchor(loc, &scaleCenter);
	
	POINT northW;
	northW.x = this->position.left - scaleCenter.x;
	northW.y = this->position.top - scaleCenter.y;
	POINT northE;
	northE.x = this->position.right - scaleCenter.x;
	northE.y = this->position.top - scaleCenter.y;
	POINT southE;
	southE.x = this->position.right - scaleCenter.x;
	southE.y = this->position.bottom - scaleCenter.y;
	POINT southW;
	southW.x = this->position.left - scaleCenter.x;
	southW.y = this->position.bottom - scaleCenter.y;

	northW.x = northW.x * scale;
	northW.y = northW.y * scale;
	northE.x = northE.x * scale;
	northE.y = northE.y * scale;
	southW.x = southW.x * scale;
	southW.y = southW.y * scale;
	southE.x = southE.x * scale;
	southE.y = southE.y * scale;

	northW.x += scaleCenter.x;
	northW.y += scaleCenter.y;
	northE.x += scaleCenter.x;
	northE.y += scaleCenter.y;
	southW.x += scaleCenter.x;
	southW.y += scaleCenter.y;
	southE.x += scaleCenter.x;
	southE.y += scaleCenter.y;

	int left = min(northW.x, southW.x);
	int right = max(northE.x, southE.x);
	int top = min(northW.y, northE.y);
	int bottom max(southW.y, southE.y);

	this->position.left = left;
	this->position.right = right;
	this->position.top = top;
	this->position.bottom = bottom;
	this->uiElement::setToDraw(&this->position);
}

void positionUI::setPos(RECT* in) {
	if (in->left > in->right || in->top > in->bottom)
		return;
	memcpy(&this->position, in, sizeof(RECT));
	this->uiElement::setToDraw(&this->position);
}

void positionUI::setLayer(unsigned int in) {
	if (in == this->layer || this->factroyFingerPrint == nullptr || this->layerchangeCallbac == nullptr)
		return;
	this->layer = in;
	this->layerchangeCallbac(this->factroyFingerPrint);
}

unsigned int positionUI::getLayer() {
	return this->layer;
}

void positionUI::setToDraw() {
	this->uiElement::setToDraw(&this->position);
}

void positionUI::setDrawFunc(void (*drawFunc)(HDC,positionUI*)) {
	this->drawFunction = drawFunc;
}

void positionUI::Draw(HDC hdc) {
	if (this->drawFunction == nullptr)
		return;
	if (!this->runDraw())
		return;
	this->drawFunction(hdc,this);
	if (this->customDraw != nullptr) {
		HRGN last = CreateRectRgn(this->position.left, this->position.top, this->position.right, this->position.bottom);
		GetClipRgn(hdc, last);
		HRGN current = CreateRectRgn(this->position.left, this->position.top, this->position.right, this->position.bottom);
		SelectClipRgn(hdc, current);
		this->customDraw(hdc,&this->position);
		SelectClipRgn(hdc, last);
		DeleteObject(current);
		DeleteObject(last);
	}
}

void positionUI::setCustomDraw(void (*drawFunc)(HDC,RECT*)) {
	this->customDraw = drawFunc;
}

attachableUI::attachableUI(HWND hwnd, RECT* positionin) : positionUI(hwnd,positionin) {
	this->attachments = new std::vector<attachment>();
	this->parentChangedCallback = nullptr;
}

attachableUI::~attachableUI() {
	if (this->attachments != nullptr)
		delete this->attachments;
}

void attachableUI::getDirection(attachment* att, attachDir* dir) {
	positionUI* con = (positionUI*)(att->connected);
	POINT c1;
	con->GetAnchor(positionUI::AnchorLocation::CENTER,&c1);
	POINT loc1;
	con->GetAnchor(att->to, &loc1);
	POINT c2;
	this->GetAnchor(positionUI::AnchorLocation::CENTER, &c2);
	POINT loc2;
	this->GetAnchor(att->from, &loc2);
	POINT dir1;
	dir1.x = c1.x - loc1.x;
	dir1.y = c1.y - loc1.y;
	POINT dir2;
	dir2.x = loc2.x - c2.x;
	dir2.y = loc2.y - c2.y;
	POINT dirO;
	dirO.x = dir1.x + dir2.x;
	dirO.y = dir1.y + dir2.y;

	long mag = sqrt(dirO.x*dirO.x + dirO.y*dirO.y);

	dir->x = ((float)dirO.x) / ((float)mag);
	dir->y = ((float)dirO.y) / ((float)mag);
}

void attachableUI::attach(attachableUI* connected, positionUI::AnchorLocation to, positionUI::AnchorLocation from, unsigned int margin) {
	if (this->attachments == nullptr)
		return;
	for (int i = 0; i < this->attachments->size(); i++) {
		if (this->attachments->at(i).connected == connected) {
			this->attachments->at(i).from = from;
			this->attachments->at(i).to = to;
			attachDir dir;
			this->getDirection(&this->attachments->at(i), &dir);
			dir.x = dir.x * margin;
			dir.y = dir.y * margin;
			this->attachments->at(i).margin.x = (LONG)dir.x;
			this->attachments->at(i).margin.y = (LONG)dir.y;
			return;
		}
	}
	attachment temp;
	temp.connected = connected;
	temp.from = from;
	temp.to = to;
	attachDir dir;
	this->getDirection(&temp, &dir);
	dir.x = dir.x * margin;
	dir.y = dir.y * margin;
	temp.margin.x = (LONG)dir.x;
	temp.margin.y = (LONG)dir.y;
	this->attachments->push_back(temp);
}

void attachableUI::attach(attachableUI* connected, positionUI::AnchorLocation to, positionUI::AnchorLocation from, int offx, int offy) {
	if (this->attachments == nullptr)
		return;
	for (int i = 0; i < this->attachments->size(); i++) {
		if (this->attachments->at(i).connected == connected) {
			this->attachments->at(i).from = from;
			this->attachments->at(i).to = to;
			this->attachments->at(i).margin.x = offx;
			this->attachments->at(i).margin.y = offy;
			return;
		}
	}
	attachment temp;
	temp.connected = connected;
	temp.from = from;
	temp.to = to;
	temp.margin.x = offx;
	temp.margin.y = offy;
	this->attachments->push_back(temp);
}

void attachableUI::detach(attachableUI* connected) {
	if (this->attachments == nullptr)
		return;
	for (int i = 0; i < this->attachments->size(); i++) {
		if (this->attachments->at(i).connected == connected) {
			this->attachments->erase(this->attachments->begin() + i);
		}
	}
}

void attachableUI::setPos(POINT* loc, positionUI::AnchorLocation anchor) {
	this->positionUI::setPos(loc,anchor);
	for (int i = 0; i < this->attachments->size(); i++) {
		attachment* selected = &this->attachments->at(i);
		POINT pos;
		this->GetAnchor(selected->from, &pos);
		pos.x += selected->margin.x;
		pos.y += selected->margin.y;
		selected->connected->setPos(&pos,selected->to);
	}
	if (this->parentChangedCallback != nullptr)
		this->parentChangedCallback(this);
}

void attachableUI::setPos(RECT* in) {
	this->positionUI::setPos(in);
	for (int i = 0; i < this->attachments->size(); i++) {
		attachment* selected = &this->attachments->at(i);
		POINT pos;
		this->GetAnchor(selected->from, &pos);
		pos.x += selected->margin.x;
		pos.y += selected->margin.y;
		selected->connected->setPos(&pos, selected->to);
	}
	if (this->parentChangedCallback != nullptr)
		this->parentChangedCallback(this);
}

void attachableUI::scale(float scale, positionUI::AnchorLocation loc) {
	this->positionUI::scale(scale,loc);
	for (int i = 0; i < this->attachments->size(); i++) {
		attachment* selected = &this->attachments->at(i);
		POINT pos;
		this->GetAnchor(selected->from, &pos);
		pos.x += selected->margin.x;
		pos.y += selected->margin.y;
		selected->connected->setPos(&pos, selected->to);
	}
	if (this->parentChangedCallback != nullptr)
		this->parentChangedCallback(this);
}

void attachableUI::setChangeCallback(void (*parentCallback)(attachableUI* obj)) {
	if (parentCallback == nullptr)
		return;
	this->parentChangedCallback = parentCallback;
}