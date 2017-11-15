
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once
#include "include/DrawMenu.h"
#include <afxmenubar.h>

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//protected:  // �ؼ���Ƕ���Ա
public:
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CNewMenu          newMenu;

	CView*            m_pInspectView;
	CView*            m_pMeasurementView;
	CView*            m_pPcbView;
// ���ɵ���Ϣӳ�亯��
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


