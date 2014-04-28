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
	// Оконная процедура
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
	AzGraphCtrl(); // Конструктор
	~AzGraphCtrl(); // Деструктор

	// Создает окно графика в заданном прямоугольнике //
	bool Create(HWND hParent, HMENU hID, RECT Rct);
	// Создает окно графика в заданных координатах с заданными размерами //
	bool Create(HWND hParent, HMENU hID, int x, int y, int cx, int cy);
	// Изменяет позицию и размеры окна //
	BOOL SetWindowPos(int X, int Y, int cx, int cy, UINT uFlags);

	/* Задает количество интервалов с различным цветом фона
	* nItem - количество интервалов
	*/
	void SetCountIntervals(int nItem);

	/* Изменяет атрибуты ранее определенного интервала
	* nItem - индекс интервала
	* min - начальная координата интервала
	* max - конечная координата интревала
	* clr - цвет фона интервала
	* bRedraw (здесь и далее) - флаг перерисовки
	*/
	bool ChangeInterval(int nItem, DOUBLE min, DOUBLE max, COLORREF clr, BOOL bRedraw = TRUE);

	/* Добавляет интервал
	* min - начальная координата интервала
	* max - конечная координата интревала
	* clr - цвет фона интервала
	*/
	bool AddInterval(DOUBLE min, DOUBLE max, COLORREF clr, BOOL bRedraw = TRUE);

	/* Определяет диапозон отображения графика по оси X
	* min - минимальное значение
	* max - максимальное значение
	*/
	void SetMinMaxX(DOUBLE min, DOUBLE max, BOOL bRedraw = TRUE);
	/* Определяет диапозон отображения графика по оси Y
	* min - минимальное значение
	* max - максимальное значение
	*/
	void SetMinMaxY(DOUBLE min, DOUBLE max, BOOL bRedraw = TRUE);

	/* Задает количество линий сетки по оси X
	* nLine - количество линий сетки
	*/
	void SetCountLineGridX(UINT nLine, BOOL bRedraw = TRUE);
	/* Задает количество линий сетки по оси Y
	* nLine - количество линий сетки
	*/
	void SetCountLineGridY(UINT nLine, BOOL bRedraw = TRUE);

	// Задает цвет основного фона графика
	void SetBGColor(COLORREF clr, BOOL bRedraw = TRUE);
	// Задает цвет линий сетки по оси Х
	void SetGridXColor(COLORREF clr, BOOL bRedraw = TRUE);
	// Задает цвет линий сетки по оси Y
	void SetGridYColor(COLORREF clr, BOOL bRedraw = TRUE);
	// Задает цвет оси абсцисс
	void SetAxisXColor(COLORREF clr, BOOL bRedraw = TRUE);
	// Задает цвет оси ординат
	void SetAxisYColor(COLORREF clr, BOOL bRedraw = TRUE);
	// Задает цвет графика
	void SetGraphColor(COLORREF clr, BOOL bRedraw = TRUE);

	/* Передача массивов точек графика. Сразу всех.
	Массивы обязательно должны иметь объявленные в SetCountData размеры.
	Иначе - крах!
	*/
	void SetAllData(vector<vector<DOUBLE>> pDX, vector<vector<DOUBLE>> pDY);

	void SetLegends(vector<string> legnds);
	vector<string> GetLegends();

	void ClearLegends();

	/* Очищает область графика */
	void Clear(BOOL bRedraw = TRUE);

	/* Обновляет область графика
	Полезен при вызовах функци Ploting с флагом bDrawing равным FALSE
	*/
	void Update();

	/* Задает единицы измерения для осей
	* pcX - указатель на строку, содержащую единицу измерения для оси X
	* pcY - указатель на строку, содержащую единицу измерения для оси Y
	*/
	void SetUnits(char* pcX, char* pcY, BOOL bRedraw = TRUE);

	/* Возвращает растровую картинку видимой области графика */
	HBITMAP GetVisualBitmap();

	/* Задает толщину графика */
	void SetCaliber(int nC, BOOL bRedraw = TRUE); // Устанавливает толщину линий графика

	/* Задает тип графика 0 - точками, 1 - линиями */
	void SetGraphType(int type);

	/* Задает размеры точек графика*/
	void SetPointsSize(int w, int h, BOOL bRedraw = TRUE);

	// Копирует растровое изображение
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