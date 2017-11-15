#pragma once
#include "ImageProcess/PcbInspect.h"
//#include "GxmcCode/GxmcGerber.h"


// CAddSolderDlg dialog

class CAddSolderDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddSolderDlg)

public:
	CAddSolderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddSolderDlg();

// Dialog Data
	enum { IDD = IDD_ADDSOLDER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
private:
	CPcbInspect m_PcbInspect;

	CDC  m_MemDC;
	CBitmap m_Bmp;
	CDC* m_pAddWeldingPlateDC;
	CRect m_rect;
	vector<CRect> m_vecRect;
	//´æ´¢ËùÑ¡º¸ÅÌµÄÂß¼­Î»ÖÃ
	CRect m_AddWeldingPlateRect;
	bool m_bIsDrawRect;
	CPoint bPoint;
	CPoint ePoint;

	//Gxmc_Gerber::CGxmcGerber m_gxmcGerber;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonComplete();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

public:
	float nPcbWidth;
	float nPcbHeight;

};
