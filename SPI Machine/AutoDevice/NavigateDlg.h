#pragma once

#include "ImageProcess/PcbInspect.h"

// CNavigateDlg dialog

class CNavigateDlg : public CDialog
{
	DECLARE_DYNAMIC(CNavigateDlg)

public:
	CNavigateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNavigateDlg();

// Dialog Data
	enum { IDD = IDD_NAVIGATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	HDC m_hPcbMap;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

private:
	CPcbInspect m_PcbInspect;

	//FOV����
	CDC* m_pPcbMapDC;
	CDC  m_PcbMapMemDC;
	CBitmap m_PcbMapBmp;
	//ͼ��ؼ��ߴ�
	CRect m_pcbMapRect;
	//Map�ؼ�������ϴ����ʱ������
	CPoint m_prePoint;
	
	//Pcb��ߴ�
	CRect m_PcbRect;

	//FOV
	CRect m_FovRect;
	CBitmap m_FovMemBmp;
	CDC m_FovMemDC;
	CDC* m_pFovDC; 
	IplImage* m_pFovImg;

public:
	float nPcbWidth;
	float nPcbHeight;
//	CPoint m_curPoint;


	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
