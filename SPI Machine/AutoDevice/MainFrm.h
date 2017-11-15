
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "include/DrawMenu.h"
#include <afxmenubar.h>

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//protected:  // 控件条嵌入成员
public:
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CNewMenu          newMenu;

	CView*            m_pInspectView;
	CView*            m_pMeasurementView;
	CView*            m_pPcbView;
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	virtual void ActivateFrame(int nCmdShow = -1);
	afx_msg void OnClose();

	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	bool InitialToolBar();
	afx_msg void OnMeasurementView();
	afx_msg void OnInspection();

	void SwitchToForm(int nForm);
	afx_msg void OnPcbproject();
	afx_msg void OnBaselineCorrect();
};


