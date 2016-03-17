// lab2-next.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "lab2-next.h"
#include <iostream>
#include <strstream>
#include "string"


#define MAX_LOADSTRING 100
#define STEP_SIZE 20
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

const UINT SHAPE_POINT_COUNT = 512;
const UINT SHAPE_LINE_COUNT = 512;

typedef struct tagShapePoint
{
	RECT rect;
	UINT shapeID;
	COLORREF color;
	UINT size;
} ShapePoint;

typedef struct tagShapeLine
{
	RECT rect;
	UINT shapeID;
	COLORREF color;
	UINT size;
} ShapeLine;

// Глобальные переменные:
ShapePoint ShapePoints[SHAPE_POINT_COUNT];
ShapeLine ShapeLines[SHAPE_LINE_COUNT];
UINT nCurrentLineIndex = 0;
UINT nCurrentPointIndex = 0;
int xMaxPos;
int yMaxPos;
int xBMP = 0;
int yBMP = 0;
int xMPos, yMPos;
int red, green, blue;
int size = 8;
HBITMAP img_bmp;

HWND hWnd;
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
WNDCLASSEXW wcex;
HDC img_dc;
HBITMAP old_bmp;
HDC gHDC;
COLORREF color;
bool leftBnDown = false;

bool drawWithLine = false;
bool drawWithDot = true;
bool inFirst = true;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SizeDialog(HWND, UINT, WPARAM, LPARAM);
void				DrawPoint(HDC gHDC, int x1, int y1, int size, int color);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2NEXT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
	

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	gHDC = GetDC(hWnd);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2NEXT));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize			= sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2NEXT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB2NEXT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной
   HDC hDCScreen = GetDC(NULL);
   int Horres = GetDeviceCaps(hDCScreen, HORZRES);
   int Vertres = GetDeviceCaps(hDCScreen, VERTRES);

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   GetDeviceCaps(hDCScreen, HORZRES) / 2 - WINDOW_WIDTH / 2,
	   GetDeviceCaps(hDCScreen, VERTRES) / 2 - WINDOW_HEIGHT / 2,
	   WINDOW_WIDTH,
	   WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT OnPaint(HWND hWnd);
LRESULT OnCommand(HWND hWnd, int iID, int iEvent, HWND hWndControl, bool &isHandled);

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND — обработать меню приложения
//  WM_PAINT — отрисовать главное окно
//  WM_DESTROY — отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int wmId, wmEvent;
	static PAINTSTRUCT ps;
	static HDC hdc;

	static int xPos = 0;
	static int yPos = 0;
	

    switch (message)
    {

	case WM_CREATE:
		
		break;

	case WM_MOUSEMOVE:
		xMPos = LOWORD(lParam);
		yMPos = HIWORD(lParam);

		if (leftBnDown)
		{
			if (drawWithDot)
			{
				ShapePoints[nCurrentPointIndex].rect.left = xMPos;
				ShapePoints[nCurrentPointIndex].rect.top = yMPos;
				ShapePoints[nCurrentPointIndex].color = RGB(red, green, blue);
				ShapePoints[nCurrentPointIndex].size = size;
				nCurrentPointIndex++;
				InvalidateRect(hWnd, 0, 0);
			}
			if (drawWithLine)
			{
				if (inFirst)
				{
					nCurrentLineIndex++;
					inFirst = false;
					ShapeLines[nCurrentLineIndex].rect.left = xMPos;
					ShapeLines[nCurrentLineIndex].rect.top = yMPos;
					ShapeLines[nCurrentLineIndex].color = RGB(red, green, blue);
				}
				ShapeLines[nCurrentLineIndex].rect.right = xMPos;
				ShapeLines[nCurrentLineIndex].rect.bottom = yMPos;
				
			}
		}
		break;

	case WM_LBUTTONDOWN:
	{
		leftBnDown = true;
	}
	break;

	case WM_LBUTTONUP:
	{
		leftBnDown = false;
		inFirst = true;
		InvalidateRect(hWnd, 0, 0);
	}
	break;


	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		bool isHandled = true;
		LRESULT lResult = OnCommand(hWnd, wmId, wmEvent, (HWND)lParam, isHandled);
		if (!isHandled)
		{
			lResult = DefWindowProc(hWnd, message, wParam, lParam);
		}

		return lResult;
	}
	break;

    case WM_PAINT:
	{
		return OnPaint(hWnd);
	}
	break;

	case WM_HSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINERIGHT:
			if (xPos < xMaxPos) 
			{
				xPos += STEP_SIZE;
				xBMP += STEP_SIZE;
			}
			break;
		case SB_LINELEFT:
			if (xPos > 0) 
			{
				xPos -= STEP_SIZE;
				xBMP -= STEP_SIZE;
			}
			break;
		case SB_THUMBTRACK:
			xPos = HIWORD(wParam);
			xBMP = HIWORD(wParam);
			break;
		}
		SetScrollPos(hWnd, SB_HORZ, xPos, TRUE);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			if (yPos > 0) {
				yPos -= STEP_SIZE;
				yBMP -= STEP_SIZE;
			}
			break;
		case SB_LINEDOWN:
			if (yPos < yMaxPos) {
				yPos += STEP_SIZE;
				yBMP += STEP_SIZE;
			}
			break;
		case SB_THUMBTRACK:
			yPos = HIWORD(wParam);
			yBMP = HIWORD(wParam);
			break;
		}
		SetScrollPos(hWnd, SB_VERT, yPos, TRUE);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK SizeDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			TCHAR s[255];
			GetDlgItemText(hDlg, IDC_EDIT, s, 9);
			size = _ttoi(s);
			EndDialog(hDlg, LOWORD(wParam));
			
			return (INT_PTR)TRUE;
		}


        
		
        break;
    }
    return (INT_PTR)FALSE;
}

void DrawPoint(HDC gHDC, int x1, int y1, int size, int color)
{

	static HPEN hpen = NULL;
	static HPEN oldPen = NULL;

	hpen = CreatePen(PS_SOLID, size, color);
	oldPen = (HPEN)SelectObject(gHDC, hpen);

	MoveToEx(gHDC, x1, y1, NULL);
	LineTo(gHDC, x1, y1);
	
	SelectObject(gHDC, oldPen);
	
	DeleteObject(hpen);
}

void DrawLine(HDC gHDC, int x1, int y1, int x2, int y2, int size, int color)
{

	static HPEN hpen = NULL;
	static HPEN oldPen = NULL;

	hpen = CreatePen(PS_SOLID, size, color);
	oldPen = (HPEN)SelectObject(gHDC, hpen);

	MoveToEx(gHDC, x1, y1, NULL);
	LineTo(gHDC, x2, y2);

	SelectObject(gHDC, oldPen);

	DeleteObject(hpen);
}

LRESULT OnPaint(HWND hWnd)
{
	static HDC hdc, compitibledc;
	static BITMAP bmp;
	static RECT rect;
	static PAINTSTRUCT ps;

	img_bmp = (HBITMAP)LoadImage(NULL, "test.bmp", IMAGE_BITMAP, 1024, 768, LR_LOADFROMFILE);


	hdc = ::BeginPaint(hWnd, &ps);

	if (!img_bmp)
		return EXIT_FAILURE;

	GetObject(img_bmp, sizeof(BITMAP), &bmp);
	compitibledc = CreateCompatibleDC(hdc);
	SelectObject(compitibledc, img_bmp);
	GetClientRect(hWnd, &rect);
	BitBlt(hdc, NULL, NULL, rect.right, rect.bottom, compitibledc, xBMP, yBMP, SRCCOPY);

	if ((bmp.bmWidth - rect.right) > 0)
	{
		xMaxPos = bmp.bmWidth - rect.right;
		SetScrollRange(hWnd, SB_HORZ, NULL, xMaxPos, TRUE);
	}

	if ((bmp.bmHeight - rect.bottom) > 0)
	{
		yMaxPos = bmp.bmHeight - rect.bottom;
		SetScrollRange(hWnd, SB_VERT, NULL, yMaxPos, TRUE);
	}


	static UINT index;
	static UINT xPos;
	static UINT yPos;

	
	for (index = 0; index < SHAPE_POINT_COUNT; index++)
	{
		if (ShapePoints[index].rect.left) {
			DrawPoint(hdc, ShapePoints[index].rect.left, ShapePoints[index].rect.top, ShapePoints[index].size, ShapePoints[index].color);
		}
	}

	for (index = 0; index < SHAPE_LINE_COUNT; index++)
	{
		if (ShapeLines[index].rect.right) {
			DrawLine(hdc, ShapeLines[index].rect.left, ShapeLines[index].rect.top, ShapeLines[index].rect.right, ShapeLines[index].rect.bottom, size, ShapeLines[index].color);
		}
	}

	

	::EndPaint(hWnd, &ps);
	
	DeleteObject(img_bmp);
	DeleteDC(compitibledc);

	return LRESULT();
}
LRESULT OnCommand(HWND hWnd, int iID, int iEvent, HWND hWndControl, bool & isHandled)
{
	// Разобрать выбор в меню:
	switch (iID)
	{
		case IDM_ABOUT:
		    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		    break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_BLACK:
			red = 0;
			green = 0;
			blue = 0;
			break;
		case IDM_RED:
			red = 255;
			green = 0;
			blue = 0;
			break;
		case IDM_GREEN:
			red = 0;
			green = 255;
			blue = 0;
			break;
		case IDM_BLUE:
			red = 0;
			green = 0;
			blue = 255;
			break; 
		case IDM_DOT:
			drawWithDot = true;
			drawWithLine = false;
			break; 
		case IDM_LINE:
			drawWithDot = false;
			drawWithLine = true;
			break;
		case ID_SIZE_SMALL:
			size = 8;
			break;
		case ID_SIZE_MIDDLE:
			size = 16;
			break;
		case ID_SIZE_BIG:
			size = 32;
			break;
		case ID_SIZE_ENTER:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, SizeDialog);
			break;

		default:
			isHandled = false;
	}
	return 0;
};
