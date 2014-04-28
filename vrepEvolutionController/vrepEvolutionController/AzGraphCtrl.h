/////////////////////////////////////////////////////
/// Author:			Belenkov SV (AZote).
/// Description:	Class for displaying graphes
/////////////////////////////////////////////////////
/// AzGraphCtrl.h 
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

/// NEW TYPES ///////////////////////////////////////
#ifndef AZ_GRAPH_CTRL_H
#define AZ_GRAPH_CTRL_H

#include <vector>
#include <string>

using namespace std;

struct INTERVALCOLOR
{
	DOUBLE start;
	DOUBLE end;
	COLORREF clr;
};


struct DATABASEFORGRAPH
{
	// definition of fields
	RECT rctWnd;
	RECT rctBody;
	RECT rctGraph;
	RECT rctXRulerU;
	RECT rctXRulerD;
	RECT rctYRulerL;
	RECT rctYRulerR;
	// definition of colors
	COLORREF clrBody;
	COLORREF clrBGGraph;
	COLORREF clrXGrid;
	COLORREF clrYGrid;
	COLORREF clrXAxis;
	COLORREF clrYAxis;
	COLORREF clrRulers;

	COLORREF clrGraph[5];
	// definition of parameters
	UINT nInterval;
	UINT nInitInterval;
	UINT nDataItem;
	UINT nInitDataItem;
	UINT nLineGridX;
	UINT nLineGridY;
	int nLenXAxis;
	int nLenYAxis;
	int nFixWidthBody;
	int nFixHeightBody;
	int nPutBodyX;
	int nPutBodyY;
	int nIncWinX;
	int nLimIncWinX;
	int nLimIncWinY;
	int pointWidth;
	int pointHeight;
	UINT nZoom;
	
	BOOL lineGraph;

	DOUBLE dMinX, dMaxX;
	DOUBLE dMinY, dMaxY;
	DOUBLE dAspectX, dAspectY;
	int iOX, iOY;

	DOUBLE dGradMarkX, dGradMarkY;

	int nCaliber;
	// definiton of resource
	vector<vector<DOUBLE>> pDataX;
	vector<vector<DOUBLE>> pDataY;
	// definition of states
	BOOL bData;
	BOOL bCopyData;
	BOOL bDrawAxes;
	BOOL bUnitX;
	BOOL bUnitY;
	// definition of handlers
	HDC hDC, hDBDC;	HBITMAP hOldDBBmp, hDBBmp;
	HFONT hFont, hOldFont;
	HRGN hGraphRgn;
	HPEN hGraphPen;
	// definition of other types
	INTERVALCOLOR* pIC;
	SCROLLINFO hSI;
	LOGFONT sLF;
	TEXTMETRIC sTM;
	SIZE szTMY;
	SIZE szTMX;
	SIZE szUnits;
	// definition of strings
	char strUnitX[20];
	char strUnitY[20];

	//
	std::vector <std::string> legends;
	std::vector<SIZE> szTMLeg;
	BOOL useLegends;
};


/// DESCRIPTIONS ////////////////////////////////////
class AzGraphCtrl
{
private:
	// ������� ���������
	static LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LONG dwNewLong;
		AzGraphCtrl* pwin = NULL;
		if(uMsg == WM_NCCREATE) 
		{
			dwNewLong = (long)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			::SetWindowLong(hWnd, GWL_USERDATA, dwNewLong);
			pwin = (AzGraphCtrl*)dwNewLong;
		}
		else
		{
			pwin = (AzGraphCtrl*) ::GetWindowLong(hWnd, GWL_USERDATA);
		}
		if(pwin)
		{
			return pwin->OnMessage(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}
	// ==================
public: // Public functions
	AzGraphCtrl(); // �����������
	~AzGraphCtrl(); // ����������

	// ������� ���� ������� � �������� �������������� //
	bool Create(HWND hParent, HMENU hID, RECT Rct);
	// ������� ���� ������� � �������� ����������� � ��������� ��������� //
	bool Create(HWND hParent, HMENU hID, int x, int y, int cx, int cy);
	// �������� ������� � ������� ���� //
	BOOL SetWindowPos(int X, int Y, int cx, int cy, UINT uFlags);

	/* ������ ���������� ���������� � ��������� ������ ����
	* nItem - ���������� ����������
	*/
	void SetCountIntervals(int nItem);

	/* �������� �������� ����� ������������� ���������
	* nItem - ������ ���������
	* min - ��������� ���������� ���������
	* max - �������� ���������� ���������
	* clr - ���� ���� ���������
	* bRedraw (����� � �����) - ���� �����������
	*/
	bool ChangeInterval(int nItem, DOUBLE min, DOUBLE max, COLORREF clr, BOOL bRedraw = TRUE);

	/* ��������� ��������
	* min - ��������� ���������� ���������
	* max - �������� ���������� ���������
	* clr - ���� ���� ���������
	*/
	bool AddInterval(DOUBLE min, DOUBLE max, COLORREF clr, BOOL bRedraw = TRUE);

	/* ���������� �������� ����������� ������� �� ��� X
	* min - ����������� ��������
	* max - ������������ ��������
	*/
	void SetMinMaxX(DOUBLE min, DOUBLE max, BOOL bRedraw = TRUE);
	/* ���������� �������� ����������� ������� �� ��� Y
	* min - ����������� ��������
	* max - ������������ ��������
	*/
	void SetMinMaxY(DOUBLE min, DOUBLE max, BOOL bRedraw = TRUE);

	/* ������ ���������� ����� ����� �� ��� X
	* nLine - ���������� ����� �����
	*/
	void SetCountLineGridX(UINT nLine, BOOL bRedraw = TRUE);
	/* ������ ���������� ����� ����� �� ��� Y
	* nLine - ���������� ����� �����
	*/
	void SetCountLineGridY(UINT nLine, BOOL bRedraw = TRUE);

	// ������ ���� ��������� ���� �������
	void SetBGColor(COLORREF clr, BOOL bRedraw = TRUE);
	// ������ ���� ����� ����� �� ��� �
	void SetGridXColor(COLORREF clr, BOOL bRedraw = TRUE);
	// ������ ���� ����� ����� �� ��� Y
	void SetGridYColor(COLORREF clr, BOOL bRedraw = TRUE);
	// ������ ���� ��� �������
	void SetAxisXColor(COLORREF clr, BOOL bRedraw = TRUE);
	// ������ ���� ��� �������
	void SetAxisYColor(COLORREF clr, BOOL bRedraw = TRUE);
	// ������ ���� �������
	void SetGraphColor(COLORREF clr, BOOL bRedraw = TRUE);

	/* �������� �������� ����� �������. ����� ����.
	������� ����������� ������ ����� ����������� � SetCountData �������.
	����� - ����!
	*/
	void SetAllData(vector<vector<DOUBLE>> pDX, vector<vector<DOUBLE>> pDY);

	void SetLegends(vector<string> legnds);
	vector<string> GetLegends();

	void ClearLegends();

	/* ������� ������� ������� */
	void Clear(BOOL bRedraw = TRUE);

	/* ��������� ������� �������
	������� ��� ������� ������ Ploting � ������ bDrawing ������ FALSE
	*/
	void Update();

	/* ������ ������� ��������� ��� ����
	* pcX - ��������� �� ������, ���������� ������� ��������� ��� ��� X
	* pcY - ��������� �� ������, ���������� ������� ��������� ��� ��� Y
	*/
	void SetUnits(char* pcX, char* pcY, BOOL bRedraw = TRUE);

	/* ���������� ��������� �������� ������� ������� ������� */
	HBITMAP GetVisualBitmap();

	/* ������ ������� ������� */
	void SetCaliber(int nC, BOOL bRedraw = TRUE); // ������������� ������� ����� �������

	/* ������ ��� ������� 0 - �������, 1 - ������� */
	void SetGraphType(int type);

	/* ������ ������� ����� �������*/
	void SetPointsSize(int w, int h, BOOL bRedraw = TRUE);

	// �������� ��������� �����������
	HBITMAP CopyBitmap(HBITMAP hbm);

	BOOL  AzGraphCtrl::SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName);


protected: // Protected functions
	LRESULT OnMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private: // Private functions
	void OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnDestroy();
	void OnPaint();
	void OnSize(WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam);
	void OnInitDialog();
	void OnHScroll(WPARAM wParam, LPARAM lParam);
	void OnSizeBody();
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void OnTextMetrics();
	void OnSizeRulers();
	void OnSizeGraph();
	void OnUpdate();
	void OnMinMaxY();
	void OnMoveGraph();

	void Draw();
	void DrawGraph();
	void DrawXRulerU();
	void DrawXRulerD();
	void DrawYRulerL();
	void DrawYRulerR();
	//void DrawGraphAndCopy();
	void DrawAxes();
	void DrawLegend();

	void DrawButton(HDC hDC, RECT* rct, COLORREF clr, int nState = 0);
	void DrawHMark(HDC hDC, int y, int x1, int x2);
	void DrawVMark(HDC hDC, int x, int y1, int y2);


public: // Public data
	HWND m_hWnd;

private: // Private data
	HINSTANCE m_hInst;

	DATABASEFORGRAPH db;
};

#endif;