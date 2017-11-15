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
	BOOL   bZero;//FALSEû�й����˶���TRUE�����˶���
	BOOL   bFirst;//�ж��Ƿ��ǳ���һ�����ʹ򿪴˶Ի���TRUE��
	CLabel  m_StrInfo;

};
