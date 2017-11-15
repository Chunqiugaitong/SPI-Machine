// AxisMoveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "AxisMoveDlg.h"
#include "afxdialogex.h"
#include "include/DMC2410.h"


// CAxisMoveDlg dialog

IMPLEMENT_DYNAMIC(CAxisMoveDlg, CDialog)

CAxisMoveDlg::CAxisMoveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAxisMoveDlg::IDD, pParent)
	, dRatio(1.000)
{

}

CAxisMoveDlg::~CAxisMoveDlg()
{
}

void CAxisMoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAxisMoveDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_SPEED, &CAxisMoveDlg::OnBnClickedBtnSpeed)
END_MESSAGE_MAP()


// CAxisMoveDlg message handlers

void CAxisMoveDlg::OnBnClickedBtnSpeed()
{
	CString strTemp = _T("");
	GetDlgItemText(IDC_BTN_SPEED, strTemp);
	if (0 == strTemp.Compare(_T("高速")))
	{
		SetDlgItemText(IDC_BTN_SPEED, _T("中速"));
		dRatio = 0.0300;
	}
	else if (0 == strTemp.Compare(_T("中速")))
	{
		SetDlgItemText(IDC_BTN_SPEED, _T("低速"));
		dRatio = 0.00300;
	}
	else if (0 == strTemp.Compare(_T("低速")))
	{
		SetDlgItemText(IDC_BTN_SPEED, _T("高速"));
		dRatio = 0.300;
	}
}

BOOL CAxisMoveDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_LBUTTONDOWN == pMsg->message)
	{
		WORD wRet = 0;
		CString strErr = _T("");
		double dEnd = 0.000;
		if ((pMsg->hwnd == GetDlgItem(IDC_BTN_XL)->m_hWnd)||(pMsg->hwnd == GetDlgItem(IDC_BTN_XR)->m_hWnd)||(pMsg->hwnd == GetDlgItem(IDC_BTN_YL)->m_hWnd)||(pMsg->hwnd == GetDlgItem(IDC_BTN_YR)->m_hWnd))
		{
			//wRet = Axis_Move(Struct_Unite.axis_par.wZAxis, Struct_Unite.axis_par.dZMinVel, Struct_Unite.axis_par.dZMaxVel, Struct_Unite.axis_par.dZTAcc, Struct_Unite.axis_par.dZTDec,
			//	Struct_Unite.axis_par.dZTs, 0, dEnd, 1, 0);
			//if (wRet)
			//{
			//	Err_Info(wRet, strErr);
			//	MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			//	return 1;
			//}
		}
		if (pMsg->hwnd == GetDlgItem(IDC_BTN_ZT)->m_hWnd)
		{
			wRet = Axis_Move(Struct_Unite.axis_par.wZAxis, Struct_Unite.axis_par.dZMinVel*dRatio, Struct_Unite.axis_par.dZMaxVel*dRatio,
				Struct_Unite.axis_par.dZTAcc, Struct_Unite.axis_par.dZTDec,
				Struct_Unite.axis_par.dZTs, 0, Struct_Unite.axis_par.dZTripNegaTive, 1, 0);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_ZB)->m_hWnd)
		{
			wRet = Axis_Move(Struct_Unite.axis_par.wZAxis, Struct_Unite.axis_par.dZMinVel*dRatio, Struct_Unite.axis_par.dZMaxVel*dRatio,
				Struct_Unite.axis_par.dZTAcc, Struct_Unite.axis_par.dZTDec,
				Struct_Unite.axis_par.dZTs, 0, Struct_Unite.axis_par.dZTrip, 1, 0);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_XL)->m_hWnd)
		{
			wRet = Axis_Move(Struct_Unite.axis_par.wXAxis, Struct_Unite.axis_par.dXMinVel*dRatio, Struct_Unite.axis_par.dXMaxVel*dRatio,
				Struct_Unite.axis_par.dXTAcc, Struct_Unite.axis_par.dXTDec,
				Struct_Unite.axis_par.dXTs, 0, Struct_Unite.axis_par.dXTripNegaTive, 1, 0);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_XR)->m_hWnd)
		{
			wRet = Axis_Move(Struct_Unite.axis_par.wXAxis, Struct_Unite.axis_par.dXMinVel*dRatio, Struct_Unite.axis_par.dXMaxVel*dRatio,
				Struct_Unite.axis_par.dXTAcc, Struct_Unite.axis_par.dXTDec,
				Struct_Unite.axis_par.dXTs, 0, Struct_Unite.axis_par.dXTrip, 1, 0);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_YL)->m_hWnd)
		{
			wRet = Axis_Move(Struct_Unite.axis_par.wYAxis, Struct_Unite.axis_par.dYMinVel*dRatio, Struct_Unite.axis_par.dYMaxVel*dRatio,
				Struct_Unite.axis_par.dYTAcc, Struct_Unite.axis_par.dYTDec,
				Struct_Unite.axis_par.dYTs, 0, Struct_Unite.axis_par.dYTripNegaTive, 1, 0);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_YR)->m_hWnd)
		{
			wRet = Axis_Move(Struct_Unite.axis_par.wYAxis, Struct_Unite.axis_par.dYMinVel*dRatio, Struct_Unite.axis_par.dYMaxVel*dRatio,
				Struct_Unite.axis_par.dYTAcc, Struct_Unite.axis_par.dYTDec,
				Struct_Unite.axis_par.dYTs, 0, Struct_Unite.axis_par.dYTrip, 1, 0);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_WMAX)->m_hWnd)
		{
			wRet = Axis_Move(Struct_Unite.axis_par.wWAxis, Struct_Unite.axis_par.dWMinVel*dRatio, Struct_Unite.axis_par.dWMaxVel*dRatio,
				Struct_Unite.axis_par.dWTAcc, Struct_Unite.axis_par.dWTDec,
				Struct_Unite.axis_par.dWTs, 0, 0, 1, 0);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_WMIN)->m_hWnd)
		{
			wRet = Axis_Move(Struct_Unite.axis_par.wWAxis, Struct_Unite.axis_par.dWMinVel*dRatio, Struct_Unite.axis_par.dWMaxVel*dRatio,
				Struct_Unite.axis_par.dWTAcc, Struct_Unite.axis_par.dWTDec,
				Struct_Unite.axis_par.dWTs, 0, Struct_Unite.axis_par.dWTrip - Struct_Unite.axis_par.dWMinTrip, 1, 0);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
	}
	else if (WM_LBUTTONUP == pMsg->message)
	{
		WORD wRet = 0;
		CString strErr = _T("");
		if (pMsg->hwnd == GetDlgItem(IDC_BTN_ZT)->m_hWnd||pMsg->hwnd == GetDlgItem(IDC_BTN_ZB)->m_hWnd)
		{
			wRet = d2410_imd_stop(Struct_Unite.axis_par.wZAxis);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_XL)->m_hWnd||pMsg->hwnd == GetDlgItem(IDC_BTN_XR)->m_hWnd)
		{
			wRet = d2410_imd_stop(Struct_Unite.axis_par.wXAxis);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_YL)->m_hWnd||pMsg->hwnd == GetDlgItem(IDC_BTN_YR)->m_hWnd)
		{
			wRet = d2410_imd_stop(Struct_Unite.axis_par.wYAxis);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_BTN_WMAX)->m_hWnd||pMsg->hwnd == GetDlgItem(IDC_BTN_WMIN)->m_hWnd)
		{
			wRet = d2410_imd_stop(Struct_Unite.axis_par.wWAxis);
			if (wRet)
			{
				Err_Info(wRet, strErr);
				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
				return 1;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


