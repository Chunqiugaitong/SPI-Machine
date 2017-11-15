#pragma once


// CAppConDlg dialog

class CAppConDlg : public CDialog
{
	DECLARE_DYNAMIC(CAppConDlg)

public:
	CAppConDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAppConDlg();

// Dialog Data
	enum { IDD = IDD_APPCON_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	double m_dXSafe;
	double m_dYSafe;
	double m_dZSafe;
	afx_msg void OnBnClickedBtnUpdate();
	double m_dZHigh;
	afx_msg void OnBnClickedBtnZupdate();
	afx_msg void OnBnClickedBtnZmove();
};
