#pragma once
#include "resource.h"
#include "afxwin.h"

// CFovPathSelectDlg dialog

class CFovPathSelectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFovPathSelectDlg)

public:
	CFovPathSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFovPathSelectDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_FOVPATHSELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCalculateFovPath();
	CButton m_radio_TopLeft;
	CButton m_radio_TopRight;
	CButton m_radio_BottomLeft;
	CButton m_radio_BottomRight;

	CButton m_radio_Horizon;
	CButton m_radio_Vertical;
	virtual BOOL OnInitDialog();
};
