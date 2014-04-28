/////////////////////////////////////////////////////
/// Author:			Belenkov SV (AZote).
/// Description:	Class for displaying graphes
/////////////////////////////////////////////////////
/// AzGraphCtrl.cpp 
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

// INCLUDES /////////////////////////////////////////
#pragma once
#include <windows.h>

#include "AzGraphCtrl.h"

/// NEW TYPES ///////////////////////////////////////

/// DESCRIPTIONS ////////////////////////////////////

AzGraphCtrl::AzGraphCtrl()
{
	db.hDC = db.hDBDC = NULL;
	db.hOldDBBmp = db.hDBBmp = NULL;
	db.clrBody = RGB(250,250,250);//RGB(233,236,216);
	db.clrBGGraph = RGB(255,255,255);//RGB(0,0,0);
	db.clrRulers =  RGB(255,255,255);//RGB(233,236,216);
	db.clrXGrid =  RGB(255,255,255);//RGB(128,128,128);
	db.clrYGrid =  RGB(255,255,255);//RGB(128,128,128);
	db.dMinX = -1;
	db.dMaxX =  1;
	db.dMinY = -1;
	db.dMaxY =  1;
	db.nLineGridX = 5;
	db.nLineGridY = 5;
	db.nZoom = 1;

	db.clrGraph[0] = RGB(74,126,187);//RGB(0,255,0);
	db.clrGraph[1] = RGB(192,80,77);
	db.clrGraph[2] = RGB(155,187,89);
	db.clrGraph[3] = RGB(128,100,162);
	db.clrGraph[4] = RGB(75,172,198);

	db.hSI.cbSize = sizeof(db.hSI);
	db.hSI.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
	db.hSI.nMin = 0;
	db.hSI.nPos = 0;
	db.hSI.nTrackPos = 0;

	db.nFixWidthBody = -1;
	db.nPutBodyX = 0;
	db.nPutBodyY = 0;
	db.nIncWinX = 0;

	db.nLimIncWinX = 0;
	db.nLimIncWinY = 0;


	// initializing of font parameters
	db.sLF.lfHeight = 14;
	db.sLF.lfWidth = 0;
	db.sLF.lfEscapement = 0;
	db.sLF.lfOrientation = 0;
	db.sLF.lfWeight = 400;
	db.sLF.lfItalic = FALSE;
	db.sLF.lfUnderline = FALSE;
	db.sLF.lfStrikeOut = FALSE;
	db.sLF.lfCharSet = 1;
	db.sLF.lfOutPrecision = 0;
	db.sLF.lfClipPrecision = 0;
	db.sLF.lfQuality = 4;
	db.sLF.lfPitchAndFamily = 0;
//	db.sLF.lfFaceName = NULL;

	db.pIC = NULL;
	db.nInterval = 0;
	db.nInitInterval = 0;

	db.bData = FALSE;
	db.bCopyData = FALSE;
	db.nDataItem = 0;
	db.nInitDataItem = 0;

	db.clrXAxis = RGB(192, 192, 192);
	db.clrYAxis = RGB(192, 192, 192);
	db.bDrawAxes = TRUE;

	db.hGraphRgn = NULL;
	db.hGraphPen = NULL;

	strcpy(db.strUnitX, "Единица");
	strcpy(db.strUnitY, "Единица");
	db.bUnitX = TRUE;
	db.bUnitY = TRUE;

	db.nCaliber = 5;

	db.lineGraph = false;
	db.pointWidth = 5;
	db.pointHeight = 5;
	db.legends.reserve(5);

	db.useLegends = false;
}

AzGraphCtrl::~AzGraphCtrl()
{
	
}

bool AzGraphCtrl::Create(HWND hParent, HMENU hID, int x, int y, int cx, int cy)
{
	m_hInst = GetModuleHandle(NULL);
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = m_hInst;
	wcex.hCursor = LoadCursor(0, (LPCTSTR)IDC_ARROW);
	wcex.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wcex.lpfnWndProc = WinProc;
	wcex.lpszClassName = "AzGraphCtrl";
	int atom = RegisterClassEx(&wcex);

	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, "AzGraphCtrl", NULL, 
		WS_VISIBLE | WS_CHILD | WS_HSCROLL, 
					x, y, cx, cy, hParent, hID, m_hInst, this);

	if (m_hWnd) return true; else return false;
}

bool AzGraphCtrl::Create(HWND hParent, HMENU hID, RECT Rct)
{
	return Create(hParent, hID, Rct.left, Rct.top, 
						Rct.right - Rct.left, Rct.bottom - Rct.top);
}

LRESULT AzGraphCtrl::OnMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_ERASEBKGND: return 0;
	case WM_CREATE:			OnCreate(hWnd, wParam, lParam);		break;
	case WM_SIZE: 			OnSize(wParam, lParam);				break;
	case WM_PAINT:			OnPaint();							break;
	case WM_DESTROY:		OnDestroy();						break;
	case WM_MOUSEWHEEL:		OnMouseWheel(wParam, lParam);		break;
	case WM_INITDIALOG:		OnInitDialog();						break;
	case WM_HSCROLL:		OnHScroll(wParam, lParam);			break;
	case WM_LBUTTONDOWN:	OnLButtonDown(wParam, lParam);		break;
	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void AzGraphCtrl::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;
	db.hDC = GetDC(hWnd);

	db.hFont = CreateFontIndirect(&db.sLF);
	db.hGraphPen = CreatePen(PS_SOLID, 1, db.clrGraph[0]);

}

void AzGraphCtrl::OnInitDialog()
{
}

void AzGraphCtrl::OnSize(WPARAM wParam, LPARAM lParam)
{
	BOOL bSizeBody = FALSE;

	GetClientRect(m_hWnd, &db.rctWnd);
	db.hSI.nPage = db.rctWnd.right;

	if(db.nFixWidthBody <= -1) {
		db.rctBody = db.rctWnd;
		bSizeBody = TRUE;
	}
	else
	{
		if(db.rctWnd.right > db.nFixWidthBody) {
			db.rctBody.right = db.rctWnd.right;
			bSizeBody = TRUE;
		}
		else
			bSizeBody = FALSE;
	}
	if(db.rctWnd.bottom != db.rctBody.bottom) {
		db.rctBody.bottom = db.rctWnd.bottom;
		bSizeBody = TRUE;
	}

	// if changed size of body, redraw him
	if(bSizeBody) { 
		OnSizeBody();
		SetScrollInfo(m_hWnd, SB_HORZ, &db.hSI, FALSE);
		Draw();
	}
	else {
		SetScrollInfo(m_hWnd, SB_HORZ, &db.hSI, TRUE);
		db.nLimIncWinX = db.rctWnd.right - db.rctBody.right;

		if(db.nIncWinX < db.nLimIncWinX) {
			db.nIncWinX = db.nLimIncWinX;
			db.nPutBodyX = db.nIncWinX;
			db.hSI.nPos = -db.nIncWinX;
		}

	}
}

void AzGraphCtrl::OnSizeBody() // Заходим сюда, только когда изменяем размеры rctBody!!!
{
	///////////////////// 
	if (db.hDBDC)
	{
		SelectObject(db.hDBDC, db.hOldDBBmp);
		DeleteObject(db.hDBBmp);
	}
	else
	{
		db.hDBDC = CreateCompatibleDC(db.hDC);
		db.hOldFont = (HFONT)SelectObject(db.hDBDC, db.hFont);
		GetTextMetrics(db.hDBDC, &db.sTM);
		OnTextMetrics();
	}
	db.hDBBmp = CreateCompatibleBitmap(db.hDC, db.rctBody.right - db.rctBody.left, 
		db.rctBody.bottom - db.rctBody.top);
	db.hOldDBBmp = (HBITMAP)SelectObject(db.hDBDC, db.hDBBmp);
	/////////////////////

	db.rctXRulerU.left = db.rctBody.left;
	db.rctXRulerU.right = db.rctBody.right - 1;
	db.rctXRulerU.top = db.rctBody.top;
	db.rctXRulerU.bottom = db.rctXRulerU.top + db.szTMY.cy + 6;

	db.rctYRulerL.left = db.rctBody.left;
	db.rctYRulerL.right = db.szTMY.cx + 17;
	db.rctYRulerL.top = db.rctXRulerU.bottom + 1;
	db.rctYRulerL.bottom = db.rctBody.bottom - 1;

	db.rctXRulerD.left = db.rctYRulerL.right + 1;
	db.rctXRulerD.right = db.rctBody.right - 1;
	db.rctXRulerD.bottom = db.rctBody.bottom - 1;
	db.rctXRulerD.top = db.rctXRulerD.bottom - db.szTMX.cy - 10;

	db.rctYRulerR.right = db.rctBody.right - 1;
	db.rctYRulerR.left = db.rctYRulerR.right - db.szTMX.cx - 6;
	db.rctYRulerR.top = db.rctBody.top;
	db.rctYRulerR.bottom = db.rctXRulerD.top - 1;

	db.rctXRulerU.right = db.rctYRulerR.left - 1;

	OnSizeRulers();

	db.nLimIncWinX = db.rctWnd.right - db.rctBody.right;

	db.hSI.nMax = db.rctBody.right - 1;
	SetScrollInfo(m_hWnd, SB_HORZ, &db.hSI, TRUE);

}

void AzGraphCtrl::OnPaint()
{
	PAINTSTRUCT PS;
	HDC hDC = BeginPaint(m_hWnd, &PS);
		BitBlt(hDC, db.nPutBodyX, db.nPutBodyY, db.rctBody.right, db.rctBody.bottom, 
			db.hDBDC, 0, 0, SRCCOPY);
	EndPaint(m_hWnd, &PS);
}

void AzGraphCtrl::OnDestroy()
{
	SelectObject(db.hDBDC, db.hOldDBBmp);
	DeleteObject(db.hDBBmp);
	DeleteDC(db.hDBDC);
	ReleaseDC(m_hWnd, db.hDC);
	if(db.hGraphPen)
		DeleteObject(db.hGraphPen);
	if(db.hGraphRgn)
		DeleteObject(db.hGraphRgn);

	DestroyWindow(m_hWnd);

}


BOOL AzGraphCtrl::SetWindowPos(int X, int Y, int cx, int cy, UINT uFlags)
{
	return ::SetWindowPos(m_hWnd, NULL, X, Y, cx, cy, uFlags);
}


void AzGraphCtrl::Draw()
{
	DrawGraph();
	DrawXRulerU();
	DrawXRulerD();
	DrawYRulerL();
	DrawYRulerR();
	DrawLegend();
}

void AzGraphCtrl::DrawGraph()
{
	HBRUSH hBr = CreateSolidBrush(db.clrBGGraph);
	FillRect(db.hDBDC, &db.rctGraph, hBr);
	DeleteObject(hBr);

	RECT rct;

	rct.top = db.rctGraph.top + 1;
	rct.bottom = db.rctGraph.bottom - 1;
	for(int j=0; j<(int)db.nInitInterval; j++) {
		rct.left = (int)(db.iOX + db.dAspectX * db.pIC[j].start);
		rct.right = (int)(db.iOX + db.dAspectX * db.pIC[j].end);
		if(rct.left < db.rctGraph.left) rct.left = db.rctGraph.left + 1;
		if(rct.right > db.rctGraph.right) rct.right = db.rctGraph.right - 1;
		if(rct.left < rct.right)
		{
			hBr = CreateSolidBrush(db.pIC[j].clr);
			FillRect(db.hDBDC, &rct, hBr);
			DeleteObject(hBr);
		}
	}

	HPEN hPenX = CreatePen(PS_SOLID, 1, db.clrXGrid);
	HPEN hPenY = CreatePen(PS_SOLID, 1, db.clrYGrid);

	HPEN hOldPen = (HPEN)SelectObject(db.hDBDC, hPenX);
	for(UINT i=1; i<=(db.nLineGridX * db.nZoom); i++) {
		MoveToEx(db.hDBDC, (int)(db.rctGraph.left + 1 + (i * db.dGradMarkX) * db.dAspectX), 
			db.rctGraph.top, NULL);
		LineTo(db.hDBDC, (int)(db.rctGraph.left + 1 + (i * db.dGradMarkX) * db.dAspectX), 
			db.rctGraph.bottom);
	}

	SelectObject(db.hDBDC, hPenY);
	for(int i=1; i<=db.nLineGridY; i++) {
		MoveToEx(db.hDBDC, db.rctGraph.left, 
			(int)(db.rctGraph.top + 1 + (i * db.dGradMarkY) * db.dAspectY), NULL);
		LineTo(db.hDBDC, db.rctGraph.right, 
			(int)(db.rctGraph.top + 1 + (i * db.dGradMarkY) * db.dAspectY));
	}

	DrawAxes();
	/////// Draw graphics ////////
	if(db.pDataY.size()>0)
	{
		HRGN hRgn = CreateRectRgnIndirect(&db.rctGraph);
		SelectClipRgn(db.hDBDC, hRgn);	

		for(int i=0; i<db.pDataY.size(); ++i)
		{
			HPEN hPenG = CreatePen(PS_SOLID, db.nCaliber, db.clrGraph[i]);
			SelectObject(db.hDBDC, hPenG);
			int x, y;

			if(db.lineGraph)
			{
				x = db.rctGraph.left + 1 + (int)((db.pDataX[i][0] - db.dMinX) * db.dAspectX); 
				y = db.rctGraph.top + 1 + (int)((db.dMaxY - db.pDataY[i][0]) * db.dAspectY);
				MoveToEx(db.hDBDC, x, y, NULL);

				for(int j=0; j<db.pDataY[i].size(); ++j)
				{
					x = db.rctGraph.left + 1 + (int)((db.pDataX[i][j] - db.dMinX) * db.dAspectX); 
					y = db.rctGraph.top + 1 + (int)((db.dMaxY - db.pDataY[i][j]) * db.dAspectY);
					LineTo(db.hDBDC, x, y);
				}
			}
			else
			{
				RECT rct;
				HBRUSH hBr = CreateSolidBrush(db.clrGraph[i]);

				for(int j=0; j<db.pDataY[i].size(); ++j)
				{
					x = db.rctGraph.left + 1 + (int)((db.pDataX[i][j] - db.dMinX) * db.dAspectX); 
					y = db.rctGraph.top + 1 + (int)((db.dMaxY - db.pDataY[i][j]) * db.dAspectY);
					rct.left = x; rct.top = y; rct.bottom = y+db.pointHeight; rct.right = x + db.pointWidth;
					FillRect(db.hDBDC,&rct,hBr);
				}
				DeleteObject(hBr);
			}
			DeleteObject(hPenG);
		}
		SelectClipRgn(db.hDBDC, NULL);
		DeleteObject(hRgn);
		SelectObject(db.hDBDC, hOldPen);
		
	}
	
	//////////////////////////////

	hBr = CreateSolidBrush(RGB(0,0,0));
		FrameRect(db.hDBDC, &db.rctGraph, hBr);
	DeleteObject(hBr);

	SelectObject(db.hDBDC, hOldPen); DeleteObject(hPenX); DeleteObject(hPenY);
}

void AzGraphCtrl::DrawLegend()
{
	if(! db.useLegends) return;

	int lgNum = db.legends.size();

	if(lgNum > 0)
	{
		HPEN hPenX = CreatePen(PS_SOLID, 1, db.clrXGrid);
	

		RECT rct;
		int textLineDist = 5;
		int legDist = 25;
		int dy = (db.rctYRulerR.bottom - db.rctYRulerR.top - db.sTM.tmHeight - legDist*(lgNum-1) - (db.sTM.tmHeight + textLineDist)*lgNum)/2;
		rct.top = db.rctYRulerR.top + dy;

		for(int i=0; i<lgNum; ++i)
		{
			HPEN hPenX1 = CreatePen(PS_SOLID, 3, db.clrGraph[i]);
			//draw text
			rct.left = db.rctYRulerR.left + 7;
			rct.right = db.rctYRulerR.right - 7;
			rct.top = db.rctYRulerR.top + dy + i*(legDist + textLineDist);
			SelectObject(db.hDBDC, hPenX1);
			MoveToEx(db.hDBDC, rct.right, rct.top, NULL);
			LineTo(db.hDBDC, rct.left, rct.top); 

			//draw line
			rct.top += textLineDist;
			rct.bottom = rct.top + db.sTM.tmHeight;
			rct.left = db.rctYRulerR.left + (db.rctYRulerR.right - db.rctYRulerR.left - db.szTMLeg[i].cx)/2;
			rct.right = db.rctYRulerR.right - (db.rctYRulerR.right - db.rctYRulerR.left -  db.szTMLeg[i].cx)/2;

			SelectObject(db.hDBDC, hPenX);
			DrawText(db.hDBDC, db.legends[i].data(), db.legends[i].size(), &rct, 
			DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			DeleteObject(hPenX1);
		}

		DeleteObject(hPenX);
		
	}
}

void AzGraphCtrl::DrawXRulerU()
{
	DrawButton(db.hDBDC, &db.rctXRulerU, db.clrRulers, 0);

	RECT rct;
	rct.left = db.rctXRulerU.left + 5;
	rct.top = db.rctXRulerU.top + 3;
	rct.right = rct.left + db.szUnits.cy;
	rct.bottom = rct.top + db.sTM.tmHeight;
	DrawText(db.hDBDC, db.strUnitY, strlen(db.strUnitY), &rct, 
		DT_LEFT | DT_VCENTER | DT_SINGLELINE);

}

void AzGraphCtrl::DrawXRulerD()
{
	DOUBLE dNote;
	char cNote[32]; RECT rct;

	DrawButton(db.hDBDC, &db.rctXRulerD, db.clrRulers, 0);

	rct.top = db.rctXRulerD.top + 6;
	rct.bottom = db.rctXRulerD.top + 6 + db.sTM.tmHeight;

	SetBkColor(db.hDBDC, db.clrRulers);
	for(int i = 0; i < (int)(db.nLineGridX * db.nZoom); i++) {
		DrawVMark(db.hDBDC, (int)(db.rctGraph.left + 1 + ((i+1) * db.dGradMarkX) * db.dAspectX), 
			db.rctXRulerD.top + 8, db.rctXRulerD.top);
	}
	for(int i = 0; i <= (int)(db.nLineGridX * db.nZoom); i++) {
		dNote = db.dMinX + i * db.dGradMarkX;
		gcvt(dNote, 10, cNote);
		rct.left = (int)(db.rctXRulerD.left + 3 + (i * db.dGradMarkX) * db.dAspectX);
		rct.right = (int)(rct.left + db.dGradMarkX * db.dAspectX - 5);
		DrawText(db.hDBDC, cNote, strlen(cNote), &rct, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

}

void AzGraphCtrl::DrawYRulerL()
{
	DOUBLE dNote;
	char cNote[32]; RECT rct;

	DrawButton(db.hDBDC, &db.rctYRulerL, db.clrRulers, 0);
	
	rct.left = db.rctYRulerL.left + 5;
	rct.right = db.rctYRulerL.right - 5;

	SetBkColor(db.hDBDC, db.clrRulers);
	for(int i = 0; i < (int)db.nLineGridY; i++) {
		DrawHMark(db.hDBDC, (int)(db.rctGraph.top + 1 + ((i+1) * db.dGradMarkY) * db.dAspectY), 
			db.rctYRulerL.right - 8, db.rctYRulerL.right);
	}
	for(int i = 0; i <= (int)db.nLineGridY; i++) {
		dNote = db.dMaxY - i * db.dGradMarkY;
		gcvt(dNote, 5, cNote);
		rct.top = (int)(db.rctGraph.top + 1 + (i * db.dGradMarkY * db.dAspectY) + 3);
		rct.bottom = (int)(rct.top + db.sTM.tmHeight);
		DrawText(db.hDBDC, cNote, strlen(cNote), &rct, DT_RIGHT | DT_TOP | DT_SINGLELINE);
	}
}

void AzGraphCtrl::DrawYRulerR()
{
	RECT rct;

	DrawButton(db.hDBDC, &db.rctYRulerR, db.clrRulers, 0);

	rct.left = db.rctYRulerR.left + 3;
	rct.right = db.rctYRulerR.right - 3;
	rct.bottom = db.rctYRulerR.bottom - 3;
	rct.top = rct.bottom - db.sTM.tmHeight;
	DrawText(db.hDBDC, db.strUnitX, strlen(db.strUnitX), &rct, 
		DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void AzGraphCtrl::DrawHMark(HDC hDC, int y, int x1, int x2)
{
	HPEN hPen1 = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	HPEN hPen2 = CreatePen(PS_SOLID, 1, RGB(255,255,255));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen1);
	MoveToEx(hDC, x1, y, NULL);
	LineTo(hDC, x2, y); 
	SelectObject(hDC, hPen2);
	MoveToEx(hDC, x1, y+1, NULL);
	LineTo(hDC, x2, y+1); 

	SelectObject(hDC, hOldPen); DeleteObject(hPen1); 
	DeleteObject(hPen2);

}

void AzGraphCtrl::DrawVMark(HDC hDC, int x, int y1, int y2)
{
	HPEN hPen1 = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	HPEN hPen2 = CreatePen(PS_SOLID, 1, RGB(255,255,255));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen1);
	MoveToEx(hDC, x, y1, NULL);
	LineTo(hDC, x, y2); 
	SelectObject(hDC, hPen2);
	MoveToEx(hDC, x+1, y1, NULL);
	LineTo(hDC, x+1, y2); 

	SelectObject(hDC, hOldPen); DeleteObject(hPen1); 
	DeleteObject(hPen2);

}

void AzGraphCtrl::DrawButton(HDC hDC, RECT* rct, COLORREF clr, int nState)
{
	COLORREF clr1, clr2;
	int R, G, B, r, g, b;

	R = clr & 0xFF;
	G = (clr >> 8) & 0xFF;
	B = (clr >> 16) & 0xFF;

	r = R + 63; if(r>255) r=255;
	g = G + 63; if(g>255) g=255;
	b = B + 63; if(b>255) b=255;
	if(!nState) clr1 = RGB(r,g,b); else clr2 = RGB(r,g,b);

	r = R - 63; if(r<0) r=0;
	g = G - 63; if(g<0) g=0;
	b = B - 63; if(b<0) b=0;
	if(!nState) clr2 = RGB(r,g,b); else clr1 = RGB(r,g,b);

	HPEN hPen1, hPen2, hOldPen;	HBRUSH hBr;
	int x1 = rct->left; 
	int y1 = rct->top;
	int x2 = rct->right;
	int y2 = rct->bottom;
	hPen1 = CreatePen(PS_SOLID, 1, clr1);
	hPen2 = CreatePen(PS_SOLID, 1, clr2);
	hBr = CreateSolidBrush(clr);
	FillRect(hDC, rct, hBr);
	hOldPen = (HPEN)SelectObject(hDC, hPen1);
	MoveToEx(hDC, x1, y2, NULL);
	LineTo(hDC, x1, y1); LineTo(hDC, x2, y1);
	SelectObject(hDC, hPen2);
	LineTo(hDC, x2,y2); LineTo(hDC, x1,y2);

	SelectObject(hDC, hOldPen); DeleteObject(hPen1); 
	DeleteObject(hPen2); DeleteObject(hBr);
}

void AzGraphCtrl::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int zDelta = (short)HIWORD(wParam);

	if(zDelta > 0 && db.nZoom < 16) {
		db.nZoom *= 2;
		db.rctBody.right = db.rctBody.left + (db.rctBody.right - db.rctBody.left) * 2;
		
		if(db.rctBody.right > 6400) db.rctBody.right = 6400;
		db.nFixWidthBody = db.rctBody.right - db.rctBody.left;
	
		OnSizeBody();

		Draw();
		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}
	else if(zDelta < 0 && db.nZoom > 1) {
		db.nZoom /= 2;
		db.rctBody.right = db.rctBody.left + (db.rctBody.right - db.rctBody.left) / 2;
		if(db.rctBody.right <= db.rctWnd.right) {
			db.nFixWidthBody = -1;
			db.nZoom = 1;
			db.rctBody.right = db.rctWnd.right;
			db.nPutBodyX = 0;
			db.nIncWinX = 0;
			db.hSI.nPos = 0;
		}
		else {
			db.nFixWidthBody = db.rctBody.right - db.rctBody.left;
		}
		db.nLimIncWinX = db.rctWnd.right - db.rctBody.right;
		
		if(db.nIncWinX < db.nLimIncWinX) {
			db.nIncWinX = db.nLimIncWinX;
			db.nPutBodyX = db.nIncWinX;
			db.hSI.nPos = -db.nIncWinX;
		}

		OnSizeBody();

		Draw();
		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}

}

void AzGraphCtrl::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	int nSBCode = (int)LOWORD(wParam);
	int xInc = 0;
	switch (nSBCode)
	{
	case SB_LINEUP:
		if (db.nIncWinX < 0) {
			if (db.nIncWinX + 20 <= 0)
				xInc = 20;
			else 
				xInc = -db.nIncWinX;
		}
		else xInc = 0;
		break;
	case SB_LINEDOWN:
		if (db.nIncWinX > db.nLimIncWinX) {
			if (db.nIncWinX - 20 >= db.nLimIncWinX)
				xInc = -20;
			else 
				xInc = db.nLimIncWinX - db.nIncWinX;
		}
		else xInc = 0;
		break;
	case SB_PAGEUP:
		if (db.nIncWinX < 0) {
			int sum = db.nIncWinX + db.hSI.nPage;
			if (sum <= 0)
				xInc = db.hSI.nPage;
			else 
				xInc = -db.nIncWinX;
		}
		else xInc = 0;
		break;
	case SB_PAGEDOWN:
		if (db.nIncWinX > db.nLimIncWinX) {
			if ((db.nIncWinX - (int)db.hSI.nPage) >= db.nLimIncWinX)
				xInc = -(int)db.hSI.nPage;
			else 
				xInc = db.nLimIncWinX - db.nIncWinX;
		}
		else xInc = 0;
		break;
	case SB_THUMBTRACK:
		xInc = -(HIWORD(wParam) - db.hSI.nPos);;
		break;
	default: xInc = 0; break;
	}

	if(xInc != 0) {
		db.nIncWinX+= xInc;
		db.nPutBodyX = db.nIncWinX;
		db.hSI.nPos-= xInc;
		
		SetScrollInfo(m_hWnd, SB_HORZ, &db.hSI, TRUE);

//		OnMoveGraph();

		InvalidateRect(m_hWnd, NULL, TRUE);
		UpdateWindow(m_hWnd);
	}

}

void AzGraphCtrl::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
/////////////////////////////////////
	SetFocus(m_hWnd);	/////////////
/////////////////////////////////////
	int fwKeys = wParam; 
	int xPos = LOWORD(lParam); 
	int yPos = HIWORD(lParam);
}

void AzGraphCtrl::OnTextMetrics()
{
	char text[32];
	SIZE sizeX, sizeY;

	gcvt((db.dMaxX - db.dMinX) / db.nLineGridX * db.nZoom, 5, text);
	GetTextExtentPoint32(db.hDBDC, text, strlen(text), &db.szTMX);

	gcvt((db.dMaxY - db.dMinY) / db.nLineGridY, 5, text);
	GetTextExtentPoint32(db.hDBDC, text, strlen(text), &db.szTMY);
	
	GetTextExtentPoint32(db.hDBDC, db.strUnitX, strlen(db.strUnitX), &sizeX);
	GetTextExtentPoint32(db.hDBDC, db.strUnitY, strlen(db.strUnitY), &sizeY);

	SIZE s;
	
	db.szTMLeg.resize(db.legends.size());
	for(int i=0; i<db.legends.size();++i)
	{
		GetTextExtentPoint32(db.hDBDC, db.legends[i].data(), strlen(db.legends[i].data()), &s);
		db.szTMLeg[i] = s; 
		if(s.cx > db.szTMX.cx)
			db.szTMX.cx = s.cx;

		db.szTMX.cx += 10;
	}
	

	db.szUnits.cx = sizeX.cx + 6;
	db.szUnits.cy = sizeY.cx;
	db.szTMY.cy = sizeY.cy;

	if(db.szTMX.cx < db.szUnits.cx) db.szTMX.cx = db.szUnits.cx;

	if(db.szTMX.cx < 10) db.szTMX.cx = 20;
	if(db.szTMY.cy < 10) db.szTMY.cy = 20;

	db.rctXRulerU.left = db.rctBody.left;
	db.rctXRulerU.right = db.rctBody.right - 1;
	db.rctXRulerU.top = db.rctBody.top;
	db.rctXRulerU.bottom = db.rctXRulerU.top + db.szTMY.cy + 6;

	db.rctYRulerL.left = db.rctBody.left;
	db.rctYRulerL.right = db.szTMY.cx + 25;
	db.rctYRulerL.top = db.rctXRulerU.bottom + 1;
	db.rctYRulerL.bottom = db.rctBody.bottom - 1;

	db.rctXRulerD.left = db.rctYRulerL.right + 1;
	db.rctXRulerD.right = db.rctBody.right - 1;
	db.rctXRulerD.bottom = db.rctBody.bottom - 1;
	db.rctXRulerD.top = db.rctXRulerD.bottom - db.szTMX.cy - 10;

	db.rctYRulerR.right = db.rctBody.right - 1;
	db.rctYRulerR.left = db.rctYRulerR.right - db.szTMX.cx;
	db.rctYRulerR.top = db.rctBody.top;
	db.rctYRulerR.bottom = db.rctXRulerD.top - 1;

	db.rctXRulerU.right = db.rctYRulerR.left - 1;

	OnSizeRulers();
}

void AzGraphCtrl::OnSizeRulers()
{
	db.rctGraph.left = db.rctYRulerL.right + 1;
	db.rctGraph.right = db.rctYRulerR.left;
	db.rctGraph.top = db.rctXRulerU.bottom + 1;
	db.rctGraph.bottom = db.rctXRulerD.top;

	OnSizeGraph();

}

void AzGraphCtrl::OnSizeGraph()
{
	////////////////////////
	if(db.hGraphRgn) {
		DeleteObject(db.hGraphRgn);
	}
	db.hGraphRgn = CreateRectRgnIndirect(&db.rctGraph);
	////////////////////////

	db.nLenXAxis = db.rctGraph.right - db.rctGraph.left - 2;
	db.nLenYAxis = db.rctGraph.bottom - db.rctGraph.top - 2;

	db.dGradMarkX = (db.dMaxX - db.dMinX) / (db.nLineGridX * db.nZoom);
	db.dGradMarkY = (db.dMaxY - db.dMinY) / db.nLineGridY;

	db.dAspectX = db.nLenXAxis / (db.dMaxX - db.dMinX);
	db.iOX = db.rctGraph.left + (int)((0 - db.dMinX) * db.dAspectX) + 1;

	db.dAspectY = db.nLenYAxis / (db.dMaxY - db.dMinY);
	db.iOY = (int)(db.rctGraph.top + (db.dMaxY * db.dAspectY) + 1);
}

void AzGraphCtrl::SetCountIntervals(int nItem)
{
	if(db.pIC) {
		delete[] db.pIC;
		db.nInitInterval = 0;
	}

	db.pIC = new INTERVALCOLOR[nItem];
	db.nInterval = nItem;
}

bool AzGraphCtrl::ChangeInterval(int nItem, DOUBLE min, DOUBLE max, COLORREF clr, BOOL bRedraw)
{
	if(nItem <= (int)db.nInitInterval) {
		db.pIC[nItem].start = min;
		db.pIC[nItem].end = max;
		db.pIC[nItem].clr = clr;

		return true;
	}
	return false;
}

bool AzGraphCtrl::AddInterval(DOUBLE min, DOUBLE max, COLORREF clr,  BOOL bRedraw)
{
	if(db.nInitInterval < db.nInterval) {
		db.pIC[db.nInitInterval].start = min;
		db.pIC[db.nInitInterval].end = max;
		db.pIC[db.nInitInterval].clr = clr;
		db.nInitInterval+= 1;

		if(bRedraw) {
			Draw();
			OnUpdate();
		}
		return true;
	}
	return false;
}

void AzGraphCtrl::OnUpdate()
{
	InvalidateRect(m_hWnd, NULL, TRUE);
	UpdateWindow(m_hWnd);
}

void AzGraphCtrl::SetMinMaxX(DOUBLE min, DOUBLE max, BOOL bRedraw)
{
	db.dMinX = min;
	db.dMaxX = max;

	OnTextMetrics();

	if(bRedraw) {
		Draw();
		OnUpdate();
	}
}

void AzGraphCtrl::SetMinMaxY(DOUBLE min, DOUBLE max, BOOL bRedraw)
{
	db.dMinY = min;
	db.dMaxY = max;

	OnTextMetrics();

	if(bRedraw) {
		Draw();
		OnUpdate();
	}
}

void AzGraphCtrl::SetCountLineGridX(UINT nLine, BOOL bRedraw)
{
	db.nLineGridX = nLine;
	db.dGradMarkX = (db.dMaxX - db.dMinX) / (db.nLineGridX * db.nZoom);

	OnTextMetrics();

	if(bRedraw) {
		Draw();
		OnUpdate();
	}
}

void AzGraphCtrl::SetCountLineGridY(UINT nLine, BOOL bRedraw)
{
	db.nLineGridY = nLine;
	db.dGradMarkX = (db.dMaxY - db.dMinY) / db.nLineGridY;

	OnTextMetrics();

	if(bRedraw) {
		Draw();
		OnUpdate();
	}
}

void AzGraphCtrl::OnMinMaxY()
{
/*	char text[32];
	SIZE size;

	gcvt((db.dMaxY - db.dMinY) / db.nLineGridY, 10, text);
	GetTextExtentPoint32(db.hDBDC, text, strlen(text), &size);

	if(size.cx < db.szUnits.cy) size.cx = db.szUnits.cy;

	db.rctYRuler.right = size.cx + 20;
	db.rctXRuler.left = db.rctBody.left + db.rctYRuler.right + 1;

	OnSizeRulers();
*/
}

void AzGraphCtrl::SetAllData(vector<vector<DOUBLE>> pDX, vector<vector<DOUBLE>> pDY)
{
	db.pDataX = pDX;
	db.pDataY = pDY;

	DrawGraph();
	OnUpdate();
}

void AzGraphCtrl::SetLegends(vector<string> legnds)
{
	db.legends = legnds;
	db.useLegends = true;
	OnTextMetrics();
}

vector<string> AzGraphCtrl::GetLegends()
{
	return db.legends;
}

void AzGraphCtrl::ClearLegends()
{
	db.legends.resize(0);
}

//void AzGraphCtrl::DrawGraphAndCopy()
//{
//	if(db.nInitDataItem > 0)
//	{
//		HPEN hPenG = CreatePen(PS_SOLID, db.nCaliber, db.clrGraph);
//		HPEN hOldPen = (HPEN)SelectObject(db.hDBDC, hPenG);
//		HRGN hRgn = CreateRectRgnIndirect(&db.rctGraph);
//		SelectClipRgn(db.hDBDC, hRgn);
//
//		int x, y;
//
//		if(db.lineGraph)
//		{
//			db.pDataX[0] = db.pDataX2[0];
//			db.pDataY[0] = db.pDataY2[0];
//
//			x = db.rctGraph.left + 1 + (int)((db.pDataX[0] - db.dMinX) * db.dAspectX); 
//			y = db.rctGraph.top + 1 + (int)((db.dMaxY - db.pDataY[0]) * db.dAspectY);
//
//			MoveToEx(db.hDBDC, x, y, NULL);
//	
//			for(UINT i = 1; i<db.nInitDataItem; i++) {
//				db.pDataX[i] = db.pDataX2[i];
//				db.pDataY[i] = db.pDataY2[i];
//	
//				x = db.rctGraph.left + 1 + (int)((db.pDataX[i] - db.dMinX) * db.dAspectX); 
//				y = db.rctGraph.top + 1 + (int)((db.dMaxY - db.pDataY[i]) * db.dAspectY);
//				LineTo(db.hDBDC, x, y);
//			}
//		}
//		else
//		{
//			RECT rct;
//			HBRUSH hBr = CreateSolidBrush(db.clrGraph);
//	
//			for(UINT i = 0; i<db.nInitDataItem; i++) {
//				db.pDataX[i] = db.pDataX2[i];
//				db.pDataY[i] = db.pDataY2[i];
//	
//				x = db.rctGraph.left + 1 + (int)((db.pDataX[i] - db.dMinX) * db.dAspectX); 
//				y = db.rctGraph.top + 1 + (int)((db.dMaxY - db.pDataY[i]) * db.dAspectY);
//				rct.left = x; rct.top = y; rct.bottom = y+db.pointHeight; rct.right = x+db.pointWidth;
//				FillRect(db.hDBDC,&rct,hBr);
//			}
//
//			DeleteObject(hBr);
//		}
//
//		SelectClipRgn(db.hDBDC, NULL);
//		DeleteObject(hRgn);
//		SelectObject(db.hDBDC, hOldPen);
//		DeleteObject(hPenG);
//	}
//	db.bCopyData = FALSE;
//	db.pDataX2 = db.pDataX;
//	db.pDataY2 = db.pDataY;
//}

void AzGraphCtrl::DrawAxes()
{
	if(db.bDrawAxes) {
		// Draw X-axis
		if((db.dMinY < 0 && db.dMaxY > 0) || (db.dMinY > 0 && db.dMaxY < 0)) 
		{
			HPEN hPen = CreatePen(PS_SOLID, 2, db.clrXAxis);
			HPEN hOldPen = (HPEN)SelectObject(db.hDBDC, hPen);

			MoveToEx(db.hDBDC, db.rctGraph.left + 1, db.iOY, NULL);
			LineTo(db.hDBDC, db.rctGraph.right - 1, db.iOY);

			SelectObject(db.hDBDC, hOldPen);
			DeleteObject(hPen);
		}
		// Draw Y-axis
		if((db.dMinX < 0 && db.dMaxX > 0) || (db.dMinX > 0 && db.dMaxX < 0)) 
		{
			HPEN hPen = CreatePen(PS_SOLID, 2, db.clrYAxis);
			HPEN hOldPen = (HPEN)SelectObject(db.hDBDC, hPen);

			MoveToEx(db.hDBDC, db.iOX, db.rctGraph.top + 1, NULL);
			LineTo(db.hDBDC, db.iOX, db.rctGraph.bottom - 1);

			SelectObject(db.hDBDC, hOldPen);
			DeleteObject(hPen);
		}
	}
}

void AzGraphCtrl::SetBGColor(COLORREF clr, BOOL bRedraw)
{
	db.clrBGGraph = clr;

	if(bRedraw) {
		Draw();
		OnUpdate();
	}
}

void AzGraphCtrl::SetGridXColor(COLORREF clr, BOOL bRedraw)
{
	db.clrXGrid = clr;

	if(bRedraw) {
		Draw();
		OnUpdate();
	}

}

void AzGraphCtrl::SetGridYColor(COLORREF clr, BOOL bRedraw)
{
	db.clrYGrid = clr;

	if(bRedraw) {
		Draw();
		OnUpdate();
	}

}

void AzGraphCtrl::SetAxisXColor(COLORREF clr, BOOL bRedraw)
{
	db.clrXAxis = clr;

	if(bRedraw) {
		Draw();
		OnUpdate();
	}

}

void AzGraphCtrl::SetAxisYColor(COLORREF clr, BOOL bRedraw)
{
	db.clrYAxis = clr;

	if(bRedraw) {
		Draw();
		OnUpdate();
	}
}

void AzGraphCtrl::SetGraphColor(COLORREF clr, BOOL bRedraw)
{
	db.clrGraph[0] = clr;
	if(db.hGraphPen)
		DeleteObject(db.hGraphPen);
	db.hGraphPen = CreatePen(PS_SOLID, 1, clr);

	if(bRedraw) {
		Draw();
		OnUpdate();
	}
}

void AzGraphCtrl::Update()
{
	Draw();
	OnUpdate();
}

void AzGraphCtrl::Clear(BOOL bRedraw)
{
	db.nInitDataItem = 0;

	if(bRedraw) {
		DrawGraph();
		OnUpdate();
	}
}

void AzGraphCtrl::OnMoveGraph()
{
	////////////////////////
	if(db.hGraphRgn) {
		DeleteObject(db.hGraphRgn);
	}
	RECT rct = db.rctGraph;
	rct.left+= db.nPutBodyX;
	rct.right+= db.nPutBodyX;
	db.hGraphRgn = CreateRectRgnIndirect(&rct);
	////////////////////////
}

void AzGraphCtrl::SetUnits(char* pcX, char* pcY, BOOL bRedraw)
{
	strcpy(db.strUnitX, pcX);
	strcpy(db.strUnitY, pcY);

	OnTextMetrics();

	if(bRedraw) Update();
}

HBITMAP AzGraphCtrl::GetVisualBitmap()
{
    BITMAP bm;
    GetObject(db.hDBBmp, sizeof(bm), &bm);
	
	HBITMAP hBMP = CreateBitmap(db.rctWnd.right, db.rctWnd.bottom, 
		bm.bmPlanes, bm.bmBitsPixel, NULL);

	HDC hDC = CreateCompatibleDC(NULL);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBMP);

	BitBlt(hDC, 0, 0, db.rctWnd.right, db.rctWnd.bottom, db.hDBDC, 
		-db.nPutBodyX, -db.nPutBodyY, SRCCOPY);

	SelectObject(hDC, hOldBmp);
	DeleteDC(hDC);

	return hBMP;
}

void AzGraphCtrl::SetCaliber(int nC, BOOL bRedraw)
{
	db.nCaliber = nC;

	if(bRedraw) Update();
}

void AzGraphCtrl::SetGraphType(int type)
{
	db.lineGraph = type;
	//if(bRedraw) Update();
}

void AzGraphCtrl::SetPointsSize(int w, int h, BOOL bRedraw)
{
	db.pointWidth = w;
	db.pointHeight = h;
	if(bRedraw) Update();
}

HBITMAP AzGraphCtrl::CopyBitmap(HBITMAP hbm)
{
    HDC hdcSrc = CreateCompatibleDC(NULL);
    HDC hdcDst = CreateCompatibleDC(NULL);
    HBITMAP hbmOld, hbmOld2, hbmNew;
    BITMAP bm;
    GetObject(hbm, sizeof(bm), &bm);
    hbmOld = (HBITMAP)SelectObject(hdcSrc, hbm);
    hbmNew = CreateBitmap( bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel,NULL);
    hbmOld2 = (HBITMAP)SelectObject(hdcDst, hbmNew);
    BitBlt(hdcDst, 0, 0, bm.bmWidth, bm.bmHeight, hdcSrc, 0, 0, SRCCOPY);
    SelectObject(hdcSrc, hbmOld);
    DeleteDC(hdcSrc);
    DeleteDC(hdcDst);
    return hbmNew;
}

BOOL  AzGraphCtrl::SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	HDC hDC;

	int iBits;

	WORD wBitCount;

	DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0;

	BITMAP Bitmap;

	BITMAPFILEHEADER bmfHdr;

	BITMAPINFOHEADER bi;

	LPBITMAPINFOHEADER lpbi;

	HANDLE fh, hDib, hPal,hOldPal=NULL;
  
	hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
	wBitCount = 1;
	else if (iBits <= 4)
	wBitCount = 4;
	else if (iBits <= 8)
	wBitCount = 8;
	else
	wBitCount = 24;
	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{ 
	hDC = GetDC(NULL);
	hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
	RealizePalette(hDC);
	}
  

	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
	+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal)
	{
	SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
	RealizePalette(hDC);
	ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
	FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
  
	if (fh == INVALID_HANDLE_VALUE)
	return FALSE; 

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}