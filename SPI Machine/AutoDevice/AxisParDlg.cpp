// AxisParDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "AxisParDlg.h"
#include "afxdialogex.h"


// CAxisParDlg dialog

IMPLEMENT_DYNAMIC(CAxisParDlg, CDialog)

CAxisParDlg::CAxisParDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAxisParDlg::IDD, pParent)
	, m_dXPulsePer(0)
	, m_dXPitch(0)
	, m_dXTrip(0)
	, m_dXTripNegaTive(0)
	, m_dXMinVel(0)
	, m_dXMaxVel(0)
	, m_dXTAcc(0)
	, m_dXTDec(0)
	, m_dXTs(0)
	, m_dYPulsePer(0)
	, m_dYPitch(0)
	, m_dYTrip(0)
	, m_dYTripNegaTive(0)
	, m_dYMinVel(0)
	, m_dYMaxVel(0)
	, m_dYTAcc(0)
	, m_dYTDec(0)
	, m_dYTs(0)
	, m_dZPulsePer(0)
	, m_dZPitch(0)
	, m_dZTrip(0)
	, m_dZTripNegaTive(0)
	, m_dZMinVel(0)
	, m_dZMaxVel(0)
	, m_dZTAcc(0)
	, m_dZTDec(0)
	, m_dZTs(0)
	, m_dWPulsePer(0)
	, m_dWPitch(0)
	, m_dWMinVel(0)
	, m_dWMaxVel(0)
	, m_dWTAcc(0)
	, m_dWTDec(0)
	, m_dWTs(0)
	, m_dWTrip(0)
	, m_dWMinTrip(0)
	, m_wXAxis(0)
	, m_wYAxis(0)
	, m_wZAxis(0)
	, m_wWAxis(0)
	, m_dLx(0)
	, m_dLy(0)
	, m_dRx(0)
	, m_dRy(0)
	, wDirect(0)
{

}

CAxisParDlg::~CAxisParDlg()
{
}

void CAxisParDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOXNO, m_StrComboXNo);
	DDX_Text(pDX, IDC_EDIT_XPULSEPER, m_dXPulsePer);
	DDX_Text(pDX, IDC_EDIT_XPITCH, m_dXPitch);
	DDX_Text(pDX, IDC_EDIT_XTRIP, m_dXTrip);
	DDX_Text(pDX, IDC_EDIT_XTRIP_NEGATIVE, m_dXTripNegaTive);
	DDX_Text(pDX, IDC_EDIT_XMINVEL, m_dXMinVel);
	DDX_Text(pDX, IDC_EDIT_XMAXVEL, m_dXMaxVel);
	DDX_Text(pDX, IDC_EDIT_XTACC, m_dXTAcc);
	DDX_Text(pDX, IDC_EDIT_XTDEC, m_dXTDec);
	DDX_Text(pDX, IDC_EDIT_XTS, m_dXTs);
	DDV_MinMaxDouble(pDX, m_dXTs, 0, 0.05);
	DDX_Control(pDX, IDC_COMBOYNO, m_StrComboYNo);
	DDX_Text(pDX, IDC_EDIT_YPULSEPER, m_dYPulsePer);
	DDX_Text(pDX, IDC_EDIT_YPITCH, m_dYPitch);
	DDX_Text(pDX, IDC_EDIT_YTRIP, m_dYTrip);
	DDX_Text(pDX, IDC_EDIT_YTRIP_NEGATIVE, m_dYTripNegaTive);
	DDX_Text(pDX, IDC_EDIT_YMINVEL, m_dYMinVel);
	DDX_Text(pDX, IDC_EDIT_YMAXVEL, m_dYMaxVel);
	DDX_Text(pDX, IDC_EDIT_YTACC, m_dYTAcc);
	DDX_Text(pDX, IDC_EDIT_YTDEC, m_dYTDec);
	DDX_Text(pDX, IDC_EDIT_YTS, m_dYTs);
	DDV_MinMaxDouble(pDX, m_dYTs, 0, 0.05);
	DDX_Control(pDX, IDC_COMBOZNO, m_StrComboZNo);
	DDX_Text(pDX, IDC_EDIT_ZPULSEPER, m_dZPulsePer);
	DDX_Text(pDX, IDC_EDIT_ZPITCH, m_dZPitch);
	DDX_Text(pDX, IDC_EDIT_ZTRIP, m_dZTrip);
	DDX_Text(pDX, IDC_EDIT_ZTRIP_NEGATIVE, m_dZTripNegaTive);
	DDX_Text(pDX, IDC_EDIT_ZMINVEL, m_dZMinVel);
	DDX_Text(pDX, IDC_EDIT_ZMAXVEL, m_dZMaxVel);
	DDX_Text(pDX, IDC_EDIT_ZTACC, m_dZTAcc);
	DDX_Text(pDX, IDC_EDIT_ZTDEC, m_dZTDec);
	DDX_Text(pDX, IDC_EDIT_ZTS, m_dZTs);
	DDV_MinMaxDouble(pDX, m_dZTs, 0, 0.05);
	DDX_Control(pDX, IDC_COMBOWNO, m_StrComboWNo);
	DDX_Text(pDX, IDC_EDIT_WPULSEPER, m_dWPulsePer);
	DDX_Text(pDX, IDC_EDIT_WPITCH, m_dWPitch);
	DDX_Text(pDX, IDC_EDIT_WMINVEL, m_dWMinVel);
	DDX_Text(pDX, IDC_EDIT_WMAXVEL, m_dWMaxVel);
	DDX_Text(pDX, IDC_EDIT_WTACC, m_dWTAcc);
	DDX_Text(pDX, IDC_EDIT_WTDEC, m_dWTDec);
	DDX_Text(pDX, IDC_EDIT_WTS, m_dWTs);
	DDV_MinMaxDouble(pDX, m_dWTs, 0, 0.05);
	DDX_Text(pDX, IDC_EDIT_WTRIP, m_dWTrip);
	DDX_Text(pDX, IDC_EDIT_WMINTRIP, m_dWMinTrip);
	DDX_Text(pDX, IDC_EDIT_LX, m_dLx);
	DDX_Text(pDX, IDC_EDIT_LY, m_dLy);
	DDX_Text(pDX, IDC_EDIT_RX, m_dRx);
	DDX_Text(pDX, IDC_EDIT_RY, m_dRy);
}


BEGIN_MESSAGE_MAP(CAxisParDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_LEFTUPDATE, &CAxisParDlg::OnBnClickedBtnLeftupdate)
	ON_BN_CLICKED(IDC_BTN_RIGHTUPDATE, &CAxisParDlg::OnBnClickedBtnRightupdate)
END_MESSAGE_MAP()


// CAxisParDlg message handlers


BOOL CAxisParDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);
	if (0 == wDirect)
	{
		((CButton*)GetDlgItem(IDC_RADIO_LTOR))->SetCheck(1);
	}
	else if (1 == wDirect)
	{
		((CButton*)GetDlgItem(IDC_RADIO_RTOL))->SetCheck(1);
	}
	m_StrComboXNo.SetCurSel(m_wXAxis);
	m_StrComboYNo.SetCurSel(m_wYAxis);
	m_StrComboZNo.SetCurSel(m_wZAxis);
	m_StrComboWNo.SetCurSel(m_wWAxis);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAxisParDlg::OnOK()
{
	UpdateData();
	if (m_dXTripNegaTive > 0 || m_dYTripNegaTive > 0 ||  m_dZTripNegaTive > 0)
	{
		MessageBox(_T("XYZ轴的负向行程值均为小于等于0的值！"), ALERT_ERR, ALERT_ICON);
		return;
	}
	Struct_Unite.axis_par.wXAxis = m_wXAxis = m_StrComboXNo.GetCurSel();
	Struct_Unite.axis_par.wYAxis = m_wYAxis = m_StrComboYNo.GetCurSel();
	Struct_Unite.axis_par.wZAxis = m_wZAxis = m_StrComboZNo.GetCurSel();
	Struct_Unite.axis_par.wWAxis = m_wWAxis = m_StrComboWNo.GetCurSel();
	Struct_Unite.axis_par.dXPulsePer = m_dXPulsePer;
	Struct_Unite.axis_par.dXPitch = m_dXPitch;
	Struct_Unite.axis_par.dXTrip = m_dXTrip;
	Struct_Unite.axis_par.dXTripNegaTive = m_dXTripNegaTive;
	Struct_Unite.axis_par.dXMinVel = m_dXMinVel;
	Struct_Unite.axis_par.dXMaxVel = m_dXMaxVel;
	Struct_Unite.axis_par.dXTAcc = m_dXTAcc;
	Struct_Unite.axis_par.dXTDec = m_dXTDec;
	Struct_Unite.axis_par.dXTs = m_dXTs;

	Struct_Unite.axis_par.dYPulsePer = m_dYPulsePer;
	Struct_Unite.axis_par.dYPitch = m_dYPitch;
	Struct_Unite.axis_par.dYTrip = m_dYTrip;
	Struct_Unite.axis_par.dYTripNegaTive = m_dYTripNegaTive;
	Struct_Unite.axis_par.dYMinVel = m_dYMinVel;
	Struct_Unite.axis_par.dYMaxVel = m_dYMaxVel;
	Struct_Unite.axis_par.dYTAcc = m_dYTAcc;
	Struct_Unite.axis_par.dYTDec = m_dYTDec;
	Struct_Unite.axis_par.dYTs = m_dYTs;

	Struct_Unite.axis_par.dZPulsePer = m_dZPulsePer;
	Struct_Unite.axis_par.dZPitch = m_dZPitch;
	Struct_Unite.axis_par.dZTrip = m_dZTrip;
	Struct_Unite.axis_par.dZTripNegaTive = m_dZTripNegaTive;
	Struct_Unite.axis_par.dZMinVel = m_dZMinVel;
	Struct_Unite.axis_par.dZMaxVel = m_dZMaxVel;
	Struct_Unite.axis_par.dZTAcc = m_dZTAcc;
	Struct_Unite.axis_par.dZTDec = m_dZTDec;
	Struct_Unite.axis_par.dZTs = m_dZTs;

	Struct_Unite.axis_par.dWPulsePer = m_dWPulsePer;
	Struct_Unite.axis_par.dWPitch = m_dWPitch;
	Struct_Unite.axis_par.dWTrip = m_dWTrip;
	Struct_Unite.axis_par.dWMinTrip = m_dWMinTrip;
	Struct_Unite.axis_par.dWMinVel = m_dWMinVel;
	Struct_Unite.axis_par.dWMaxVel = m_dWMaxVel;
	Struct_Unite.axis_par.dWTAcc = m_dWTAcc;
	Struct_Unite.axis_par.dWTDec = m_dWTDec;
	Struct_Unite.axis_par.dWTs = m_dWTs;
	if (((CButton*)GetDlgItem(IDC_RADIO_LTOR))->GetCheck())
	{
		Struct_Unite.axis_par.wDirect = wDirect = 0;
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_RTOL))->GetCheck())
	{
		Struct_Unite.axis_par.wDirect = wDirect = 1;
	}
	Struct_Unite.axis_par.dLX = m_dLx;
	Struct_Unite.axis_par.dLY = m_dLy;
	Struct_Unite.axis_par.dRX = m_dRx;
	Struct_Unite.axis_par.dRY = m_dRy;

	CDialog::OnOK();
}


void CAxisParDlg::OnBnClickedBtnLeftupdate()
{
	m_dLx = Struct_Unite.Real_Coor.dx;
	m_dLy = Struct_Unite.Real_Coor.dy;
	UpdateData(FALSE);
}


void CAxisParDlg::OnBnClickedBtnRightupdate()
{
	m_dRx = Struct_Unite.Real_Coor.dx;
	m_dRy = Struct_Unite.Real_Coor.dy;
	UpdateData(FALSE);
}
