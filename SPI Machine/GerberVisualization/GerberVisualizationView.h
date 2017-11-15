
// GerberVisualizationView.h : interface of the CGerberVisualizationView class
//

#pragma once
#include "../GxmcCode/GxmcGerber.h"

using namespace Gxmc_Gerber;

class CGerberVisualizationView : public CView
{
protected: // create from serialization only
	CGerberVisualizationView();
	DECLARE_DYNCREATE(CGerberVisualizationView)

// Attributes
public:
	CGerberVisualizationDoc* GetDocument() const;

	CGxmcGerber m_gxmcGerber;
	std::list<DataBlock> m_listDataBlock;
	int m_backgroundColor;
	//存储gerber文件坐标的边界值
	RectD m_realMarginRect;
	CDC m_viewMemDC;
	CBitmap m_viewBmp;
	CRect m_viewRect;
	float m_factor;

	CPoint m_MouseOldPoint;
	CPoint m_MouseCurPoint;
	bool m_IsMoveGerberGraph;
	bool m_IsScale;
	float m_delt;
	//框选元件
	CRect m_SelectRect;
	std::vector<GxmcRect> m_vecSelectRect;
	bool m_IsDrawSelectRect;
	bool m_IsDrawRect;
	CPoint m_bPoint;
	CPoint m_ePoint;
	bool m_isInvertSelection;

	float m_wWidth;
	float m_wHeight;
	int m_viewPortOriginX;
	int m_viewPortOriginY;

	ScreenStruct m_Screen;
	ScreenStruct m_FitScreen;
	bool m_bIsFitShow;
	bool m_bOriginalShow;
	CWinThread* m_pDrawThread;
	CDC* pDC;

	//绘图参数
	int m_penWidth;
	int m_penStyle;
	COLORREF m_penColor;
	COLORREF m_brushColor;
	int m_nLayer;//图层索引
	bool m_bIsFilled;

	std::map<int,Cluster> m_mapCluster;
	std::map<int,_Polygon> m_mapMarkPoint;
// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CGerberVisualizationView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnIdrScaleFit();
	afx_msg void OnIdrScaleUp();
	afx_msg void OnIdrScaleDown();
	afx_msg void OnIdrMove();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg void OnReadGerber();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void DrawBackGround();
	void LogicViewToWorldView(int lX,int lY,double& wX,double& wY);
	void WorldViewToLogicView(double wX,double wY,int& lX,int& lY);
	afx_msg void OnClose();
	afx_msg void OnQuitsystem();
	afx_msg void OnIdrHide();
	afx_msg void OnIdrOptimumpath();
	afx_msg void OnIdrSelectitems();
	afx_msg void OnIdrSelectMarkpoints();
	afx_msg void OnExportProject();
	afx_msg void OnSelectPad();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnInvertSelectPad();
	afx_msg void OnShowPad();
	afx_msg void OnHidePad();
	afx_msg void OnMovePads();
	afx_msg void OnScaledownPads();
	afx_msg void OnScaleupPads();
	afx_msg void OnIdrTest();
};

#ifndef _DEBUG  // debug version in GerberVisualizationView.cpp
inline CGerberVisualizationDoc* CGerberVisualizationView::GetDocument() const
   { return reinterpret_cast<CGerberVisualizationDoc*>(m_pDocument); }
#endif

