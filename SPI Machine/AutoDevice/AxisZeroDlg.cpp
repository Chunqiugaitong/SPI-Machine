// AxisZeroDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "AxisZeroDlg.h"
#include "afxdialogex.h"
#include "AutoDeviceDoc.h"
#include "AutoDeviceView.h"
#include "MainFrm.h"


// CAxisZeroDlg dialog

IMPLEMENT_DYNAMIC(CAxisZeroDlg, CDialog)

CAxisZeroDlg::CAxisZeroDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAxisZeroDlg::IDD, pParent)
	, sType(0)
	, bZero(FALSE)
	, bFirst(FALSE)
{

}

CAxisZeroDlg::~CAxisZeroDlg()
{
}

void CAxisZeroDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAxisZeroDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_XYZW, &CAxisZeroDlg::OnBnClickedRadioXyzw)
	ON_BN_CLICKED(IDC_RADIO_XY, &CAxisZeroDlg::OnBnClickedRadioXy)
	ON_BN_CLICKED(IDC_RADIO_Z, &CAxisZeroDlg::OnBnClickedRadioZ)
	ON_BN_CLICKED(IDC_RADIO_W, &CAxisZeroDlg::OnBnClickedRadioW)
	ON_BN_CLICKED(IDC_BTN_ZERO, &CAxisZeroDlg::OnBnClickedBtnZero)
END_MESSAGE_MAP()


// CAxisZeroDlg message handlers


BOOL CAxisZeroDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_StrInfo.SubclassDlgItem(IDC_STATIC_ZEROINFO, this);
	m_StrInfo.SetFontName(_T("Microsoft Sans Serif"));
	m_StrInfo.SetFontSize(18);
	m_StrInfo.SetFontBold(true);
	m_StrInfo.ModifyStyle(0, SS_CENTERIMAGE);
	//m_StrInfo.SetTextColor(RGB(255, 0, 0));
	//m_StrInfo.SetBkColor(RGB(0, 150, 0));

	((CButton*)GetDlgItem(IDC_RADIO_XYZW))->SetCheck(1);
	sType = 0;
	if (bFirst)
	{
		OnBnClickedBtnZero();
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CAxisZeroDlg::OnBnClickedRadioXyzw()
{
	sType = 0;
}


void CAxisZeroDlg::OnBnClickedRadioXy()
{
	sType = 1;
}


void CAxisZeroDlg::OnBnClickedRadioZ()
{
	sType = 2;
}


void CAxisZeroDlg::OnBnClickedRadioW()
{
	sType = 3;
}


void CAxisZeroDlg::OnBnClickedBtnZero()
{
	WORD wRet = 0;
	CString strErr = _T("");
	//SetDlgItemText(IDC_STATIC_ZEROINFO, _T("归零运动中.........."));
	m_StrInfo.SetTextColor(RGB(0, 128, 0));
	m_StrInfo.SetText(_T("归零运动中.........."));
	bZero = TRUE;//归零中....
	EnableControl(!bZero);
	wRet = BackOrigin(sType);
	if (wRet)
	{
		bZero = FALSE;
		EnableControl(!bZero);
		//SetDlgItemText(IDC_STATIC_ZEROINFO, _T("归零失败"));
		m_StrInfo.SetTextColor(RGB(255, 0, 0));
		m_StrInfo.SetText(_T("归零失败!"));
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return; 
	}
	bZero = FALSE;
	EnableControl(!bZero);
	//SetDlgItemText(IDC_STATIC_ZEROINFO, _T("归零运动完成"));
	m_StrInfo.SetTextColor(RGB(0, 0, 0));
	m_StrInfo.SetText(_T("归零运动完成"));
}

void CAxisZeroDlg::EnableControl(BOOL bSts)
{
	((CButton*)GetDlgItem(IDC_RADIO_XYZW))->EnableWindow(bSts);
	((CButton*)GetDlgItem(IDC_RADIO_XY))->EnableWindow(bSts);
	((CButton*)GetDlgItem(IDC_RADIO_Z))->EnableWindow(bSts);
	((CButton*)GetDlgItem(IDC_RADIO_W))->EnableWindow(bSts);
	((CButton*)GetDlgItem(IDC_BTN_ZERO))->EnableWindow(bSts);
}


void CAxisZeroDlg::OnCancel()
{
	if (!bZero)
	{
		CDialog::OnCancel();
	}
}



