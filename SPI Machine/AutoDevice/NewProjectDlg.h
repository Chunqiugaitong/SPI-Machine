#pragma once
#include "ImageProcess/PcbInspect.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "../GxmcCode/GxmcGerber.h"

// CNewProjectDlg dialog

class CNewProjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewProjectDlg)

public:
	CNewProjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewProjectDlg();

// Dialog Data
	enum { IDD = IDD_NEWPROJECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	//ͼ�����㷨�����
	CPcbInspect m_PcbInspect;
	//PcbMap��ر���
	CDC* m_pPcbMapDC; 
	CDC  m_PcbMapMemDC;
	CBitmap m_PcbMapBmp;
	CRect m_PcbMapRect;
	//Pcb���̻��ƾ�������
	CRect m_PcbDrawRect;

	//�ӳ���ر���
	CBitmap m_FovMemBmp;
	CDC m_FovMemDC;
	CDC* m_pFovDC; 
	CRect m_FovRect;
	//ROI������
	CRect m_searchRect;
	//ͼ��ؼ�FOV����
	CPoint m_FovCenter;
	float m_fX;
	float m_fY;

	Point2f m_robotMark1;
	Point2f m_robotMark2;
	Vec3f m_locateCircle;
	Mat m_roiMat;

	//���ݿ�
	Gxmc_Gerber::CGxmcGerber m_gxmcGerber;
private:
	void UpdateFov();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonModifySearchrectsize();
	afx_msg void OnBnClickedButtonNavigate();
	afx_msg void OnBnClickedButtonAutoLocate();
	afx_msg void OnBnClickedButtonSaveBasepoint1();
	afx_msg void OnBnClickedButtonSaveBasepoint2();
	afx_msg void OnBnClickedButtonAddSolderpoint();
	afx_msg void OnBnClickedButtonSetSolderparam();
	afx_msg void OnBnClickedButtonModifyPcbsize();
	afx_msg LRESULT  OnUpdatePcbMap(WPARAM wParam,LPARAM lParam);
	int m_nSearchRectWidth;
	int m_nSearchRectHeight;
	float nPcbWidth;
	float nPcbHeight;
	DOUBLE m_dLight;
	int m_nGray;
	CSliderCtrl m_strLightControl;
	CSliderCtrl m_SliderCtrlGray;
	afx_msg void OnNMCustomdrawSliderlight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderGray(NMHDR *pNMHDR, LRESULT *pResult);
	
	
};
