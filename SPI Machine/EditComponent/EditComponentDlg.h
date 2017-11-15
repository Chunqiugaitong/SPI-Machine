
// EditComponentDlg.h : header file
//
#pragma once
#include "stdafx.h"
#include "PadInspectItemDlg.h"
#include "FovPathSelectDlg.h"
#include "../Common/GxmcDatabase.h"
// CEditComponentDlg dialog
class CEditComponentDlg : public CDialogEx
{
// Construction
public:
	CEditComponentDlg(CWnd* pParent = NULL);	// standard constructor
	~CEditComponentDlg();
// Dialog Data
	enum { IDD = IDD_EDITCOMPONENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	//菜单栏、工具栏、状态栏
	CMenu m_Menu;
	CToolBar m_toolbar;
	CStatusBar m_statusbar;
	CDC m_PcbMapMemDC;
	CDC* m_pPcbMapDC;
	CBitmap m_Bmp;
	CRect m_PcbMapShowRect;
	//fov路径优化对话框
	CFovPathSelectDlg m_FovPathSelectDlg;
	//数据库
	CGxmcDatabase m_dataBase;
	RectF* m_pRect;
	//焊盘数目
	int m_nRectCount;

	//焊盘选择框
	CRect m_SelectRect;
	CPoint m_bPoint;
	CPoint m_ePoint;
	bool m_IsDrawSelectRect;
	//指向PCB板相关信息结构体变量的
	STRUCT_UNITE* pStruct_Unite;
	
public:
	afx_msg void OnBnClickedButtonMoldingplateParameter();
	afx_msg void OnOpenDatabaseFile();

private:
	void InitialToolBar();
	void InitialMenuBar();
	void InitialStatusBar();
	void InitialPcbMapDC();
	void LogicPointToViewPoint(CDC* pDC,float WExtX,float WExtY,float VExtX,
		                       float VExtY, CRect LogicRect, CRect ViewRect);

public:
	afx_msg void OnImportProject();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnScanFov();
	afx_msg void OnClose();
};

extern vector<RectF> g_vecSelectPad;
extern CString g_ProjectPathName;