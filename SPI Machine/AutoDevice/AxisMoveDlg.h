#pragma once


// CAxisMoveDlg dialog

class CAxisMoveDlg : public CDialog
{
	DECLARE_DYNAMIC(CAxisMoveDlg)

public:
	CAxisMoveDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAxisMoveDlg();

// Dialog Data
	enum { IDD = IDD_AXISMOVE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnSpeed();
	double dRatio;
};
