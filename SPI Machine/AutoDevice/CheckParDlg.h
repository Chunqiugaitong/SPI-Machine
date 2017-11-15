#pragma once


// CCheckParDlg dialog

class CCheckParDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckParDlg)

public:
	CCheckParDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCheckParDlg();

// Dialog Data
	enum { IDD = IDD_CHECK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CString m_strName;
	double m_dLen;
	double m_dWide;
	double m_dHigh;
	double m_dMarkX1;
	double m_dMarkY1;
	double m_dMarkX2;
	double m_dMarkY2;
	CString m_strFilePath;
	WORD  wIsGerber;
	CString strGerberName;
	afx_msg void OnBnClickedBtnWideexec();
	afx_msg void OnEnChangeEditPcbwide();
	afx_msg void OnBnClickedCheckFile();
	afx_msg void OnBnClickedBtnImport();
};
