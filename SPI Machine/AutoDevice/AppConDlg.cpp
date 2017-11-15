// AppConDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "AppConDlg.h"
#include "afxdialogex.h"
#include "include/DMC2410.h"


// CAppConDlg dialog

IMPLEMENT_DYNAMIC(CAppConDlg, CDialog)

CAppConDlg::CAppConDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppConDlg::IDD, pParent)
	, m_dXSafe(0)
	, m_dYSafe(0)
	, m_dZSafe(0)
	, m_dZHigh(0)
{

}

CAppConDlg::~CAppConDlg()
{
}

void CAppConDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_XSAFE, m_dXSafe);
	DDX_Text(pDX, IDC_EDIT_YSAFE, m_dYSafe);
	DDX_Text(pDX, IDC_EDIT_ZSAFE, m_dZSafe);
	DDX_Text(pDX, IDC_EDIT_ZHIGH, m_dZHigh);
}


BEGIN_MESSAGE_MAP(CAppConDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CAppConDlg::OnBnClickedBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_ZUPDATE, &CAppConDlg::OnBnClickedBtnZupdate)
	ON_BN_CLICKED(IDC_BTN_ZMOVE, &CAppConDlg::OnBnClickedBtnZmove)
END_MESSAGE_MAP()


// CAppConDlg message handlers


BOOL CAppConDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAppConDlg::OnBnClickedBtnUpdate()
{
	m_dXSafe = Struct_Unite.Real_Coor.dx;
	m_dYSafe = Struct_Unite.Real_Coor.dy;
	m_dZSafe = Struct_Unite.Real_Coor.dz;
	UpdateData(FALSE);
}


void CAppConDlg::OnBnClickedBtnZupdate()
{
	m_dZHigh = Struct_Unite.Real_Coor.dz;
	UpdateData(FALSE);
}


void CAppConDlg::OnBnClickedBtnZmove()
{
	WORD wRet = 0;
	CString strErr = _T("");
	UpdateData();
	wRet = Axis_Move(Struct_Unite.axis_par.wZAxis, Struct_Unite.axis_par.dZMinVel, Struct_Unite.axis_par.dZMaxVel,
		Struct_Unite.axis_par.dZTAcc, Struct_Unite.axis_par.dZTDec,
		Struct_Unite.axis_par.dZTs, 0, m_dZHigh, 1, 0);
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
	}
}


void CAppConDlg::OnOK()
{
	CString strErr = _T("");
	UpdateData();

	if (m_dXSafe > 0)
	{
		if (m_dXSafe > Struct_Unite.axis_par.dXTrip)
		{
			strErr.Format(_T("X轴坐标不能大于最大正向行程%.03f"), Struct_Unite.axis_par.dXTrip);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
	}
	else if (m_dXSafe < 0)
	{
		if (m_dXSafe < Struct_Unite.axis_par.dXTrip)
		{
			strErr.Format(_T("X轴坐标不能小于最大负向行程%.03f"), Struct_Unite.axis_par.dXTripNegaTive);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
	}

	if (m_dYSafe > 0)
	{
		if (m_dYSafe > Struct_Unite.axis_par.dYTrip)
		{
			strErr.Format(_T("Y轴坐标不能大于最大正向行程%.03f"), Struct_Unite.axis_par.dYTrip);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
	}
	else if (m_dYSafe < 0)
	{
		if (m_dYSafe < Struct_Unite.axis_par.dYTrip)
		{
			strErr.Format(_T("Y轴坐标不能小于最大负向行程%.03f"), Struct_Unite.axis_par.dYTripNegaTive);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
	}

	if (m_dZSafe > 0)
	{
		if (m_dZSafe > Struct_Unite.axis_par.dZTrip)
		{
			strErr.Format(_T("Z轴坐标不能大于最大正向行程%.03f"), Struct_Unite.axis_par.dZTrip);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		if (m_dZHigh > Struct_Unite.axis_par.dZTrip)
		{
			strErr.Format(_T("Z轴工作高度不能大于最大正向行程%.03f"), Struct_Unite.axis_par.dZTrip);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
	}
	else if (m_dZSafe < 0)
	{
		if (m_dZSafe < Struct_Unite.axis_par.dZTrip)
		{
			strErr.Format(_T("Z轴坐标不能小于最大负向行程%.03f"), Struct_Unite.axis_par.dZTripNegaTive);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		if (m_dZHigh < Struct_Unite.axis_par.dZTrip)
		{
			strErr.Format(_T("Z轴工作高度不能小于最大负向行程%.03f"), Struct_Unite.axis_par.dZTripNegaTive);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
	}
	Struct_Unite.as.dXSafe = m_dXSafe;
	Struct_Unite.as.dYSafe = m_dYSafe;
	Struct_Unite.as.dZSafe = m_dZSafe;
	Struct_Unite.as.dZHigh = m_dZHigh;

	CDialog::OnOK();
}
