#include "folderSelect.h"

folderSelector::folderSelector(HWND hwnd, folderInfo* in) : attachableUI(hwnd,nullptr) {
	this->tabs = new std::vector<tabDim>();
	this->selectedTab = 0;
	this->callFunc = nullptr;
	this->userData = nullptr;
	RECT pos;
	pos.left = in->x;
	pos.top = in->y;
	pos.right = in->x + in->width;
	pos.bottom = in->y + in->height;
	this->setPos(&pos);
	for (int i = 0; i < in->tabNumber; i++) {
		this->addTab(in->tabs[i].name, in->tabs[i].margin);
	}
	this->setSelected(0);
	this->setChangeCallback(this->changedCallback);
	this->setDrawFunc(this->DrawThunk);
	this->setUpdateCallback(this->UpdateThunk);
}

folderSelector::~folderSelector() {
	if (this->tabs != nullptr) {
		for (int i = 0; i < this->tabs->size(); i++) {
			if (this->tabs->at(i).region != nullptr) {
				DeleteObject(this->tabs->at(i).region);
			}
		}
		delete this->tabs;
	}
}

unsigned int folderSelector::fillDrawArray(bool selected, POINT* ptArray, BYTE* typeArray,int x,int y,int width,int height) {
	if (selected) {
		x = x + height / 3;
		ptArray[0].x = x - height / 3;
		ptArray[0].y = y + height;
		typeArray[0] = PT_MOVETO;
		ptArray[1].x = x - height / 3;
		ptArray[1].y = y + height;
		typeArray[1] = PT_BEZIERTO;
		ptArray[2].x = x;
		ptArray[2].y = y + height;
		typeArray[2] = PT_BEZIERTO;
		ptArray[3].x = x;
		ptArray[3].y = y + height * (2.0 / 3.0);
		typeArray[3] = PT_BEZIERTO;
		ptArray[4].x = x;
		ptArray[4].y = y + height / 3;
		typeArray[4] = PT_LINETO;
		ptArray[5].x = x;
		ptArray[5].y = y + height / 3;
		typeArray[5] = PT_BEZIERTO;
		ptArray[6].x = x;
		ptArray[6].y = y;
		typeArray[6] = PT_BEZIERTO;
		ptArray[7].x = x + width / 3;
		ptArray[7].y = y;
		typeArray[7] = PT_BEZIERTO;
		ptArray[8].x = x + width * (2.0 / 3.0);
		ptArray[8].y = y;
		typeArray[8] = PT_LINETO;
		ptArray[9].x = x + width * (2.0 / 3.0);
		ptArray[9].y = y;
		typeArray[9] = PT_BEZIERTO;
		ptArray[10].x = x + width;
		ptArray[10].y = y;
		typeArray[10] = PT_BEZIERTO;
		ptArray[11].x = x + width;
		ptArray[11].y = y + height / 3;
		typeArray[11] = PT_BEZIERTO;
		ptArray[12].x = x + width;
		ptArray[12].y = y + height * (2.0 / 3.0);
		typeArray[12] = PT_LINETO;
		ptArray[13].x = x + width;
		ptArray[13].y = y + height * (2.0 / 3.0);
		typeArray[13] = PT_BEZIERTO;
		ptArray[14].x = x + width;
		ptArray[14].y = y + height;
		typeArray[14] = PT_BEZIERTO;
		ptArray[15].x = x + width + height / 3;
		ptArray[15].y = y + height;
		typeArray[15] = PT_BEZIERTO;
		return 16;
	}
	ptArray[0].x = x;
	ptArray[0].y = y + height;
	typeArray[0] = PT_MOVETO;
	ptArray[1].x = x;
	ptArray[1].y = y + height / 3;
	typeArray[1] = PT_LINETO;
	ptArray[2].x = x;
	ptArray[2].y = y + height / 3;
	typeArray[2] = PT_BEZIERTO;
	ptArray[3].x = x;
	ptArray[3].y = y;
	typeArray[3] = PT_BEZIERTO;
	ptArray[4].x = x + width / 3;
	ptArray[4].y = y;
	typeArray[4] = PT_BEZIERTO;
	ptArray[5].x = x + width * (2.0 / 3.0);
	ptArray[5].y = y;
	typeArray[5] = PT_LINETO;
	ptArray[6].x = x + width * (2.0 / 3.0);
	ptArray[6].y = y;
	typeArray[6] = PT_BEZIERTO;
	ptArray[7].x = x + width;
	ptArray[7].y = y;
	typeArray[7] = PT_BEZIERTO;
	ptArray[8].x = x + width;
	ptArray[8].y = y + height / 3;
	typeArray[8] = PT_BEZIERTO;
	ptArray[9].x = x + width;
	ptArray[9].y = y + height;
	typeArray[9] = PT_LINETO;
	return 10;
}

void folderSelector::setupTextDim(tabDim* io, unsigned int drawWidth) {
	unsigned int middle = (io->drawDim.left + io->drawDim.right) / 2;
	unsigned int left = middle - (drawWidth / 2) + io->margin;
	unsigned int right = middle + (drawWidth / 2) - io->margin;
	unsigned int top =	io->drawDim.top + io->margin;
	unsigned int bottom = top + io->textHeight;
	io->textDrawDim.left = left;
	io->textDrawDim.right = right;
	io->textDrawDim.top = top;
	io->textDrawDim.bottom = bottom;
}

void folderSelector::setSelected(unsigned int select) {
	if (!(select >= 0 && select < this->tabs->size()))
		return;
	if (this->parentWindow == nullptr)
		return;
	RECT pos;
	this->getPosition(&pos);
	unsigned int height = pos.bottom - pos.top;
	unsigned int totalWidth = pos.right - pos.left;
	unsigned int x = pos.left;
	unsigned int y = pos.top;
	this->selectedTab = select;
	unsigned int actualWid = this->tabs->at(this->selectedTab).width;
	unsigned int selectedWid = actualWid + (2 * (height / 3.0));
	unsigned int minWid = 0;
	for (int i = 0; i < this->tabs->size(); i++) {
		if (i != this->selectedTab) {
			minWid += (2 * this->tabs->at(i).margin + 20);
		}
	}
	if (totalWidth < minWid + selectedWid) {
		totalWidth = minWid + selectedWid;
	}
	unsigned int nonSelectedCount = this->tabs->size() - 1;
	if (nonSelectedCount <= 0)
		nonSelectedCount = 1;
	unsigned int nonSelectWid = (totalWidth - selectedWid) / nonSelectedCount;
	POINT ptArray[16];
	BYTE typeArray[16];
	HDC hdc = GetDC(this->parentWindow);
	for (int i = 0; i < this->tabs->size(); i++) {
		if (i != this->selectedTab) {
			unsigned int wid = nonSelectWid;
			if (this->tabs->at(i).width < nonSelectWid)
				wid = this->tabs->at(i).width;
			this->fillDrawArray(false, ptArray, typeArray, x, y, wid, height);
			BeginPath(hdc);
			PolyDraw(hdc, ptArray, typeArray, 10);
			EndPath(hdc);
			if (this->tabs->at(i).region != nullptr)
				DeleteObject(this->tabs->at(i).region);
			this->tabs->at(i).region = PathToRegion(hdc);
			this->tabs->at(i).drawDim.left = x;
			this->tabs->at(i).drawDim.top = y;
			this->tabs->at(i).drawDim.right = x + wid;
			this->tabs->at(i).drawDim.bottom = y + height;
			this->setupTextDim(&this->tabs->at(i),wid);
			x = x + wid;
		}
		else {
			this->fillDrawArray(true, ptArray, typeArray, x,  y, actualWid, height);
			BeginPath(hdc);
			PolyDraw(hdc, ptArray, typeArray, 16);
			EndPath(hdc);
			if (this->tabs->at(i).region != nullptr)
				DeleteObject(this->tabs->at(i).region);
			this->tabs->at(i).region = PathToRegion(hdc);
			this->tabs->at(i).drawDim.left = x;
			this->tabs->at(i).drawDim.top = y;
			this->tabs->at(i).drawDim.right = x + selectedWid;
			this->tabs->at(i).drawDim.bottom = y + height;
			this->setupTextDim(&this->tabs->at(i), actualWid);
			x = x + selectedWid;
		}
	}
	ReleaseDC(this->parentWindow, hdc);
	if (pos.left + totalWidth > pos.right) {
		pos.right = pos.left + totalWidth;
		this->setPos(&pos);
	}
}

void folderSelector::addTab(const char* tabName, unsigned int margin) {
	if (this->parentWindow == nullptr or this->tabs == nullptr)
		return;
	tabDim td;
	td.name = tabName;
	td.textLeng = std::strlen(td.name);
	RECT textBox;
	textBox.left = 0;
	textBox.right = 0;
	textBox.top = 0;
	textBox.bottom = 0;
	HDC hdc = GetDC(this->parentWindow);
	DrawTextA(hdc, td.name, td.textLeng, &textBox, DT_CALCRECT);
	ReleaseDC(this->parentWindow, hdc);
	td.margin = margin;
	td.textWidth = textBox.right - textBox.left;
	td.textHeight = textBox.bottom - textBox.top;
	td.height = td.textHeight + 2 * td.margin;
	td.width = td.textWidth + 2 * td.margin;
	td.region = nullptr;
	RECT pos;
	this->getPosition(&pos);
	unsigned int height = pos.bottom - pos.top;
	if (td.height > height) {
		pos.bottom = pos.top + td.height;
		this->setPos(&pos);
	}
	this->tabs->push_back(td);
	this->setSelected(this->selectedTab);
}

void folderSelector::UpdateThunk(uiElement* in, UINT message, WPARAM wParam, LPARAM lParam) {
	folderSelector* folderSel = dynamic_cast<folderSelector*>(in);
	if (folderSel == nullptr)
		return;
	folderSel->UpdateCallback(message,wParam,lParam);
}

void folderSelector::UpdateCallback(UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_LBUTTONUP:
		this->MouseUp(LOWORD(lParam), HIWORD(lParam));
		break;
	}
}

void folderSelector::MouseUp(int x, int y) {
	for (int i = 0; i < this->tabs->size(); i++) {
		if (PtInRegion(this->tabs->at(i).region, x, y)) {
			if (i != this->selectedTab) {
				if (this->callFunc != nullptr && this->userData != nullptr)
					this->callFunc(x,y,this->userData);
				this->setSelected(i);
				this->setToDraw();
				return;
			}
		}
	}
}

unsigned int folderSelector::getSelected() {
	return this->selectedTab;
}

void folderSelector::changedCallback(attachableUI* obj) {
	folderSelector* fs = dynamic_cast<folderSelector*>(obj);
	if (fs == nullptr)
		return
	fs->actualCallback();
}

void folderSelector::actualCallback() {
	this->setSelected(this->selectedTab);
}

void folderSelector::DrawThunk(HDC hdc,positionUI* in) {
	folderSelector* fs = dynamic_cast<folderSelector*>(in);
	if (fs == nullptr)
		return;
	fs->DrawCall(hdc);
}
	
void folderSelector::DrawCall(HDC hdc) {
	SetDCPenColor(hdc, NDSUGREEN);
	SetTextColor(hdc, NDSUGREEN);
	POINT ptArray[16];
	BYTE typeArray[16];
	SetDCBrushColor(hdc, NDSUGREEN);
	RECT out;
	this->getPosition(&out);
	FillRect(hdc, &out, (HBRUSH)GetStockObject(DC_BRUSH));
	for (int i = 0; i < this->tabs->size(); i++) {
		int wid = this->tabs->at(i).drawDim.right - this->tabs->at(i).drawDim.left;
		int hei = this->tabs->at(i).drawDim.bottom - this->tabs->at(i).drawDim.top;
		int x = this->tabs->at(i).drawDim.left;
		int y = this->tabs->at(i).drawDim.top;
		if (i != this->selectedTab) {
			SetDCBrushColor(hdc, NDSULOWLIGHT2);
			SetBkColor(hdc, NDSULOWLIGHT2);
			FillRgn(hdc,this->tabs->at(i).region,(HBRUSH)GetStockObject(DC_BRUSH));
			int leng = this->fillDrawArray(false, ptArray, typeArray, x, y, wid, hei);
			BeginPath(hdc);
			PolyDraw(hdc,ptArray,typeArray,leng);
			EndPath(hdc);
			StrokePath(hdc);
		}
		else {
			SetDCBrushColor(hdc, NDSUYELLOW); 
			SetBkColor(hdc, NDSUYELLOW);
			FillRgn(hdc, this->tabs->at(i).region, (HBRUSH)GetStockObject(DC_BRUSH));
			int leng = this->fillDrawArray(true, ptArray, typeArray, x, y, this->tabs->at(this->selectedTab).width, hei);
			BeginPath(hdc);
			PolyDraw(hdc,ptArray,typeArray,leng);
			EndPath(hdc);
			StrokePath(hdc);
		}
		SetTextColor(hdc,NDSULOWLIGHT);
		DrawTextA(hdc, this->tabs->at(i).name, this->tabs->at(i).textLeng, &this->tabs->at(i).textDrawDim, DT_LEFT);
	}
}

void folderSelector::setCallback(void (*func)(int x, int y, void* userDat), void* userData) {
	this->callFunc = func;
	this->userData = userData;
}
