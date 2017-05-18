#include "GameWindow.h"
#include <Windows.h>
#include <Windowsx.h>
#include <iostream>
#include <algorithm>

const CConfig config;
const HCURSOR CGameWindow::cursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));

inline int isLeft(CDot a, CDot b, CDot c) {
	return ((b.x - a.x) * (c.y - a.y) - (b.x - a.x) * (a.y - b.y));
}

bool dotIsInPolygon(CDot p, const std::vector<CDot> & polygon) {
	int count = 0;   
	for (int i = 0; i < polygon.size() - 1; i++) {    
		if (((polygon[i].y <= p.y) && (polygon[i + 1].y > p.y))    
			|| ((polygon[i].y > p.y) && (polygon[i + 1].y <= p.y))) { 
			float vt = static_cast<float>((p.y - polygon[i].y)) / (polygon[i + 1].y - polygon[i].y);
			if (p.x <  polygon[i].x + vt * (polygon[i + 1].x - polygon[i].x))
				++count; 
		}
	}
	return (count & 1);
}

CGameWindow::CGameWindow() : dots_colors({ RGB(0, 0, 255), RGB(255, 0, 0) }), points{0, 0},
polygons_colors({ RGB(0, 0, 100), RGB(100, 0, 0) }), lost_dots_colors({ RGB(170, 40, 170), RGB(100, 40, 70) }) {
	player_number = 0;
	handle = 0;
}

CGameWindow::~CGameWindow() {
}

bool CGameWindow::RegisterClass() {
	WNDCLASSEX windowClass;
	::ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = CGameWindow::windowProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.lpszClassName = (LPCSTR) "GameWindow";
	windowClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	windowClass.hCursor = cursor;
	return (::RegisterClassEx(&windowClass) != 0);
}

bool CGameWindow::Create() {
	CreateWindowEx(0,
		"GameWindow",
		"My Window",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		config.windowSize,
		config.windowSize,
		0,
		0,
		GetModuleHandle(0),
		this);
	return (handle != 0);
}

bool CGameWindow::clickOnIntersection(int x, int y) {
	int cellSize = config.cellSize;
	int radius = config.radius;
	return (x % cellSize < radius || x % cellSize > cellSize - radius) 
		&& (y % cellSize < radius || y % cellSize > cellSize - radius)
		&& (x <= config.boardSize) && (y <= config.boardSize);
}

void CGameWindow::OnLButtonClick(int x, int y) {
	if (clickOnIntersection(x, y)) {
		x = x / config.cellSize + (x % config.cellSize > config.cellSize - config.radius);
		y = y / config.cellSize + (y % config.cellSize > config.cellSize - config.radius);
		CDot dot(x, y);
		if (!dots[0].contains(dot) && !dots[1].contains(dot)) {
			dots[player_number].addDot(dot);
			for (auto dot : dots[player_number ^ 1].getDots()) {
				for (auto polygon : dots[player_number].getCycles()) {
					if (dotIsInPolygon(dot, polygon)) {
						dots[player_number ^ 1].removeDot(dot);
						lostDots.push_back(dot);
						points[player_number]++;
						break;
					}
				}
			}
			if (points[player_number] > 10) {
				MessageBox(handle, "Game Over", "Game Over", 0);
				OnRestart();
			}
			player_number ^= 1;
			RECT rect;
			GetClientRect(handle, &rect);
			InvalidateRect(handle, &rect, TRUE);
		}
	} else if (x >= config.restartButtonLeft && x <= config.restartButtonRight 
		&& y >= config.restartButtonBottom && y <= config.restartButtonTop) {
		OnRestart();
	}
}

void CGameWindow::OnRestart() {
	for (int i = 0; i < 2; i++) {
		dots[i] = CGraph();
		lostDots = std::vector<CDot>();
		points[i] = 0;
		player_number = 0;
	}
	RECT rect;
	GetClientRect(handle, &rect);
	InvalidateRect(handle, &rect, TRUE);
}

void CGameWindow::Show(int cmdShow) {
	ShowWindow(handle, cmdShow);
}

void CGameWindow::OnDestroy() {
	PostQuitMessage(0);
}

void CGameWindow::OnNCCreate(HWND _handle) {
	handle = _handle;
}

void CGameWindow::OnCreate() {
	RECT oldWindowRect;
	GetWindowRect(handle, &oldWindowRect);
	RECT newWindowRect = oldWindowRect;
	newWindowRect.right = newWindowRect.left + config.windowSize + 2 * config.offset;
	newWindowRect.bottom = newWindowRect.top + config.boardSize + 2 * config.offset;
	AdjustWindowRect(&newWindowRect, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), FALSE);
	SetWindowPos(handle, HWND_TOP, newWindowRect.left, newWindowRect.top, newWindowRect.right - newWindowRect.left, newWindowRect.bottom - newWindowRect.top, 0);
}

void CGameWindow::DrawScore(HDC & windowDC, int score, int x, int y, COLORREF color) {
	HFONT font = CreateFont(config.scoreFontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	SelectObject(windowDC, font);
	SetTextColor(windowDC, color);
	SetBkMode(windowDC, TRANSPARENT);
	char text[2];
	sprintf_s(text, 2, "%d", score);
	TextOut(windowDC, x, y, text, sizeof(text));
	DeleteObject(font);
}

void CGameWindow::DrawRestartButton(HDC & windowDC, int left, int top, int right, int bottom, COLORREF bgColor) {
	RECT rect;
	rect.left = left;
	rect.top = top;
	rect.right = right;
	rect.bottom = bottom;
	HBRUSH brush = CreateSolidBrush(bgColor);
	FillRect(windowDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	HFONT font = CreateFont((top - bottom) * 0.75, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
	SelectObject(windowDC, font);
	SetTextColor(windowDC, RGB(0, 0, 0));
	SetBkColor(windowDC, RGB(255, 255, 255));
	char text[] = "Restart";
	TextOut(windowDC, (left + right) / 2 - config.offset * 3, (top + bottom) / 2 - config.offset, text, sizeof(text));
	DeleteObject(font);
	DeleteObject(brush);
}

void CGameWindow::DrawBackground(HDC & windowDC) {
	for (int i = 0; i < 2; i++) {
		DrawScore(windowDC, points[i], config.scoreX[i], config.scoreY[i], dots_colors[i]);
	}
	HPEN pen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
	SelectObject(windowDC, pen);
	int x = config.offset;
	while (x <= config.boardSize) {
		MoveToEx(windowDC, x, config.offset, NULL);
		LineTo(windowDC, x, config.boardSize);

		MoveToEx(windowDC, config.offset, x, NULL);
		LineTo(windowDC, config.boardSize, x);
		x += config.cellSize;
	}
	DeleteObject(pen);
	DrawRestartButton(windowDC, config.restartButtonLeft, config.restartButtonTop, config.restartButtonRight,
		config.restartButtonBottom, config.restartButtonBgColor);
}

void CGameWindow::DrawDot(HDC & windowDC, const CDot & dot, COLORREF color) {
	int x = dot.x * config.cellSize;
	int y = dot.y * config.cellSize;
	HPEN pen = CreatePen(PS_SOLID, 1, color);
	HBRUSH brush = CreateSolidBrush(color);
	SelectObject(windowDC, pen);
	SelectObject(windowDC, brush);
	Ellipse(windowDC, x - r + config.offset, y - r + config.offset, x + r + config.offset, 
		y + r + config.offset);
	DeleteObject(brush);
	DeleteObject(pen);
}

void CGameWindow::DrawPolygons(HDC & windowDC) {
	for (int i = 0; i < 2; i++) {
		std::vector<std::vector<CDot>> cycles = dots[i].getCycles();
		for (auto polygon : cycles) {
			DrawPolygon(windowDC, polygon, polygons_colors[i]);
		}
	}
}

void CGameWindow::DrawPolygon(HDC & windowDC, const std::vector<CDot>& polygon, COLORREF color) {
	POINT * points = new POINT[polygon.size()];
	for (int i = 0; i < polygon.size() - 1; i++) {
		points[i].x = polygon[i].x * config.cellSize + config.offset;
		points[i].y = polygon[i].y * config.cellSize + config.offset;
	}
	HPEN pen = CreatePen(PS_SOLID, 1, color);
	HBRUSH brush = CreateSolidBrush(color);
	SelectObject(windowDC, pen);
	SelectObject(windowDC, brush);
	Polygon(windowDC, points, polygon.size() - 1);
	DeleteObject(pen);
	DeleteObject(brush);
	delete[] points;
}

void CGameWindow::DrawDots(HDC & windowDC) {
	for (int i = 0; i < 2; i++) {
		for (auto dot : dots[i].getDots()) {
			DrawDot(windowDC, dot, dots_colors[i]);
		}
		for (auto dot : lostDots) {
			DrawDot(windowDC, dot, lost_dots_colors[i]);
		}
	}
}

void CGameWindow::OnPaint() {
	PAINTSTRUCT paintStruct;
	HDC windowDC = ::BeginPaint(handle, &paintStruct);
	DrawBackground(windowDC);
	DrawPolygons(windowDC);
	DrawDots(windowDC);
	::EndPaint(handle, &paintStruct);
}

LRESULT CGameWindow::windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_NCCREATE) {
		CGameWindow* window = (CGameWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
		SetLastError(0);
		SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)window);
		if (GetLastError() != 0) {
			return GetLastError();
		}
		window->OnNCCreate(handle);
		return DefWindowProc(handle, message, wParam, lParam);
	}
	CGameWindow* window = (CGameWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);
	switch (message) {
	case WM_CREATE:
	{
		window->OnCreate();
		return DefWindowProc(handle, message, wParam, lParam);
	}
	case WM_SETCURSOR:
	{
		SetCursor(window->cursor);
		return 0;
	}
	case WM_DESTROY:
	{
		window->OnDestroy();
		return 0;
	}
	case WM_PAINT:
	{
		window->OnPaint();
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		window->OnLButtonClick(GET_X_LPARAM(lParam) - config.offset,
			GET_Y_LPARAM(lParam) - config.offset);
		return 0;
	}
	default:
		return DefWindowProc(handle, message, wParam, lParam);
	}
}
