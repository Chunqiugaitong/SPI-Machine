#pragma once
#include "include/Label.h"


// CAxisZeroDlg dialog

class CAxisZeroDlg : public CDialog
{
	DECLARE_DYNAMIC(CAxisZeroDlg)

public:
	CAxisZeroDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAxisZeroDlg();

// Dialog Data
	enum { IDD = IDD_AXISZERO_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioXyzw();
	afx_msg void OnBnClickedRadioXy();
	afx_msg void OnBnClickedRadioZ();
	afx_msg void OnBnClickedRadioW();
	afx_msg void OnBnClickedBtnZero();
	void   EnableControl(BOOL bSts);
	virtual void OnCancel();
	short  sType;
	BOOL   bZero;//FALSE没有归零运动，TRUE归零运动中
	BOOL   bFirst;//判断是否是程序一起来就打开此对话框，TRUE是
	CLabel  m_StrInfo;

};
