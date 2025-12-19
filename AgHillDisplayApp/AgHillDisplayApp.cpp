// AgHillDisplayApp.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "AgHillDisplayApp.h"
#include "UIFactory.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance            // the main window class name

class colorPallete;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance,colorPallete* col);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{    // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(L"AgHillDispClass", L"AgHillDisplay", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return nullptr;
    }

    return hWnd;
}

void b1Func(int x, int y,void* userDat) {
    Button* b = (Button*)userDat;
    POINT temp;
    temp.x = x;
    temp.y = y;
    b->setPos(&temp, positionUI::AnchorLocation::CENTER);
}

void b2Func(int x, int y,void* userDat) {
    Button* b = (Button*)userDat;
    POINT temp;
    temp.x = x;
    temp.y = y;
    b->setPos(&temp, positionUI::AnchorLocation::CENTER);
}

class colorPallete {
public:
    HBRUSH ndsuGreen = nullptr;
    HBRUSH ndsuYellow = nullptr;
    HBRUSH lowlight = nullptr;
    HBRUSH highLight = nullptr;

    colorPallete() {
        COLORREF ndsuGreen = RGB(0, 88, 61);
        this->ndsuGreen = CreateSolidBrush(ndsuGreen);
        COLORREF ndsuYellow = RGB(255, 196, 37);
        this->ndsuYellow = CreateSolidBrush(ndsuYellow);
        COLORREF lowlight = RGB(0, 34, 27);
        this->lowlight = CreateSolidBrush(lowlight);
        COLORREF highLight = RGB(255, 244, 214);
        this->highLight = CreateSolidBrush(highLight);
    }

    ~colorPallete() {
        if (this->ndsuGreen != nullptr) {
            DeleteObject(this->ndsuGreen);
        }
        if (this->ndsuYellow != nullptr) {
            DeleteObject(this->ndsuYellow);
        }
        if (this->lowlight != nullptr) {
            DeleteObject(this->lowlight);
        }
        if (this->highLight != nullptr) {
            DeleteObject(this->highLight);
        }
    }
};

void drawSomething(HDC hdc,RECT* pos) {
    
    BeginPath(hdc);
    Rectangle(hdc, 10, 10, 20, 20);
    Rectangle(hdc, 30, 10, 40, 20);
    Rectangle(hdc, 10, 30, 40, 35);
    EndPath(hdc);
    HRGN rgn = PathToRegion(hdc);
    OffsetRgn(rgn, pos->left, pos->top);
    SetDCBrushColor(hdc, NDSUGREEN);
    HBRUSH dcBrush = (HBRUSH)GetStockObject(DC_BRUSH);
    FillRgn(hdc, rgn, dcBrush);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    colorPallete* colP = new colorPallete();
    
    // Initialize global strings
    MyRegisterClass(hInstance,colP);

    // Perform application initialization:
    HWND hwnd = InitInstance(hInstance, nCmdShow);
    
    if (hwnd == nullptr){
        return FALSE;
    }

    UIFactory* factory = UIFactory::getInstance(hwnd);
    UIFactory* factory2 = UIFactory::getInstance(hwnd);

    folderInfo fI;
    fI.x = 20;
    fI.y = 30;
    fI.width = 10;
    fI.height = 10;
    folderTab tabSetup[4] = { {"hello",10} , {"test",10}, {"this",10}, {"data",10} };
    fI.tabs = tabSetup;
    fI.tabNumber = 4;
    folderSelector* fs = factory->getFolderSelector(&fI);
    buttonInfo bI;
    bI.x = 100;
    bI.y = 100;
    bI.height = 10;
    bI.width = 10;
    bI.radiusx = 10;
    bI.radiusy = 10;
    bI.margin = 20;
    bI.name = "test1";
    Button* b1 = factory->getButton(&bI);
    b1->setCallback(b1Func,b1);
    bI.x = 100;
    bI.y = 300;
    bI.height = 10;
    bI.width = 10;
    bI.radiusx = 10;
    bI.radiusy = 10;
    bI.margin = 20;
    bI.name = "test2";
    Button* b2 = factory->getButton(&bI);
    b2->setCallback(b2Func,b2);
    b1->attach(b2,positionUI::NORTH,positionUI::SOUTHEAST,0,20);
    bI.name = "";
    Button* b3 = factory->getButton(&bI);
    b3->setCustomDraw(drawSomething);
    b1->attach(b3, positionUI::NORTH, positionUI::SOUTHWEST, 0, 20);
    POINT pos;
    pos.x = 100;
    pos.y = 100;
    b1->setPos(&pos, positionUI::CENTER);

    MSG msg;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance, colorPallete* col)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = NULL;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = col->ndsuGreen;
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = L"AgHillDispClass";
    wcex.hIconSm        = NULL;

    HRESULT hr = RegisterClassExW(&wcex);
    
    return hr;
     
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UIFactory::Update(hWnd,message,wParam,lParam);
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

