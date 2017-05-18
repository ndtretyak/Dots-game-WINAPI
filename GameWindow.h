#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <map>
#include "Dot.h"
#include "Graph.h"

struct CConfig {
	int windowSize = 700;
	int boardSize = 610;
	int offset = 10;

	int cellSize = 30;
	int radius = 10;

	int restartButtonLeft = 628;
	int restartButtonTop = 350;
	int restartButtonRight = 710;
	int restartButtonBottom = 317;
	COLORREF restartButtonBgColor = RGB(255, 255, 255);

	int scoreX[2] = {630, 630};
	int scoreY[2] = {100, 400};
	int scoreFontSize = 150;
};


class CGameWindow {
public:
	CGameWindow();
	~CGameWindow();

	static bool RegisterClass();

	bool Create();
	void Show(int cmdShow);

protected:
	void OnDestroy();
	void OnNCCreate(HWND handle);
	void OnCreate();
	void OnPaint();
	void OnLButtonClick(int x, int y);
	void OnRestart();

private:
	static const HCURSOR cursor;
	static const int r = 10;
	CGraph dots[2];
	std::vector<CDot> lostDots;
	int points[2];
	int player_number;
	HWND handle;

	const std::vector<COLORREF> dots_colors;
	const std::vector<COLORREF> lost_dots_colors;
	const std::vector<COLORREF> polygons_colors;

	void DrawRestartButton(HDC & windowDC, int left, int top, int right, int bottom, COLORREF bgColor);
	void DrawScore(HDC & windowDC, int score, int x, int y, COLORREF color);
	void DrawBackground(HDC & windowDC);
	void DrawDots(HDC & windowDC);
	void DrawDot(HDC & windowDC, const CDot & dot, COLORREF color);
	void DrawPolygons(HDC & windowDC);
	void DrawPolygon(HDC & windowDC, const std::vector<CDot> & polygon, COLORREF color);

	bool clickOnIntersection(int x, int y);

	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
