#pragma once
#include "afxcmn.h"


// CCameraParDlg dialog

class CCameraParDlg : public CDialog
{
	DECLARE_DYNAMIC(CCameraParDlg)

public:
	CCameraParDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCameraParDlg();

// Dialog Data
	enum { IDD = IDD_CAMERAPAR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	UINT m_UICameraLen;
	UINT m_UICameraWide;
	double m_dCcdLen;
	double m_dCcdWide;
	double m_dShotLen;
	double m_dWorkDistance;
	double m_dEpsTime;
	double m_dGain;
	CSliderCtrl m_GainControl;
	double m_dRatio;
	afx_msg void OnDeltaposSpinGain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderGain(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
