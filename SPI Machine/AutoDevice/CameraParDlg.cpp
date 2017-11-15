// CameraParDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "CameraParDlg.h"
#include "afxdialogex.h"


// CCameraParDlg dialog

IMPLEMENT_DYNAMIC(CCameraParDlg, CDialog)

CCameraParDlg::CCameraParDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraParDlg::IDD, pParent)
	, m_UICameraLen(0)
	, m_UICameraWide(0)
	, m_dCcdLen(0)
	, m_dCcdWide(0)
	, m_dShotLen(0)
	, m_dWorkDistance(0)
	, m_dEpsTime(9000)
	, m_dGain(10)
	, m_dRatio(0.5)
{

}

CCameraParDlg::~CCameraParDlg()
{
}

void CCameraParDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CAMERALEN, m_UICameraLen);
	DDV_MinMaxUInt(pDX, m_UICameraLen, 0, 2048);
	DDX_Text(pDX, IDC_EDIT_CAMERAWIDE, m_UICameraWide);
	DDV_MinMaxUInt(pDX, m_UICameraWide, 0, 2048);
	DDX_Text(pDX, IDC_EDIT_CCDLEN, m_dCcdLen);
	DDX_Text(pDX, IDC_EDIT_CCDWIDE, m_dCcdWide);
	DDX_Text(pDX, IDC_EDIT_SHOTLEN, m_dShotLen);
	DDX_Text(pDX, IDC_EDIT_WORKDISTANCE, m_dWorkDistance);
	DDX_Text(pDX, IDC_EDIT_EPSTIME, m_dEpsTime);
	DDX_Text(pDX, IDC_EDIT_GAIN, m_dGain);
	DDV_MinMaxDouble(pDX, m_dGain, 0, 23);
	DDX_Control(pDX, IDC_SLIDER_GAIN, m_GainControl);
	DDX_Text(pDX, IDC_EDIT_RATIO, m_dRatio);
}


BEGIN_MESSAGE_MAP(CCameraParDlg, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GAIN, &CCameraParDlg::OnDeltaposSpinGain)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_GAIN, &CCameraParDlg::OnNMCustomdrawSliderGain)
END_MESSAGE_MAP()


// CCameraParDlg message handlers


BOOL CCameraParDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData();
	m_GainControl.SetRange(0, 230);
	m_GainControl.SetPos(m_dGain*10);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CCameraParDlg::OnOK()
{
	DOUBLE dTemp = 0.000, dPerTemp = 0.000;
	UpdateData();
	Struct_Unite.cp.UiLenPixel = m_UICameraLen;
	Struct_Unite.cp.UiWidePixel = m_UICameraWide; 
	Struct_Unite.cp.dCcdSizeL = m_dCcdLen;
	Struct_Unite.cp.dCcdSizeW = m_dCcdWide;
	Struct_Unite.cp.dShotLen = m_dShotLen;
	Struct_Unite.cp.dWorkDistance = m_dWorkDistance;
	Struct_Unite.cp.dEpsTime = m_dEpsTime;
	Struct_Unite.cp.dGain = m_dGain;
	Struct_Unite.cp.dRatio = m_dRatio;

	//普通镜头
	//dTemp = m_dCcdLen/2048*m_UICameraLen*m_dWorkDistance/m_dShotLen;
	//dPerTemp = dTemp/m_UICameraLen;
	//Struct_Unite.cp.dLen = dPerTemp*m_UICameraLen;
	//Struct_Unite.cp.dWide = dPerTemp*m_UICameraWide;

	//远心镜头
	dTemp = m_dCcdLen/m_dRatio;
	dPerTemp = dTemp/2048;
	Struct_Unite.cp.dLen = dPerTemp*m_UICameraLen;
	Struct_Unite.cp.dWide = dPerTemp*m_UICameraWide;


	CDialog::OnOK();
}


void CCameraParDlg::OnDeltaposSpinGain(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString strTemp = _T("");
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int iTemp = 0;
	UpdateData();
	iTemp = m_dGain*10;
	if(pNMUpDown-> iDelta == 1) // 如果此值为1 , 说明点击了Spin的往下箭头 
	{
		if (iTemp >0)
		{
			iTemp--;
		}
	} 
	else if(pNMUpDown-> iDelta == -1) // 如果此值为-1 , 说明点击了Spin的往上箭头 
	{ 
		if (iTemp < 230)
		{
			iTemp++;
		}
	}
	m_GainControl.SetPos(iTemp);
	m_dGain = (DOUBLE)iTemp/10;
	UpdateData(FALSE);
	SetGain(m_dGain);
	*pResult = 0;
}


void CCameraParDlg::OnNMCustomdrawSliderGain(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int iPos = 0;
	iPos = m_GainControl.GetPos();
	m_dGain = (DOUBLE)iPos/10;
	UpdateData(FALSE);
	SetGain(m_dGain);
	*pResult = 0;
}




BOOL CCameraParDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_LBUTTONDOWN == pMsg->message)
	{
		if (pMsg->hwnd != GetDlgItem(IDC_EDIT_GAIN)->m_hWnd && ((CButton*)GetDlgItem(IDC_EDIT_GAIN))->GetFocus())
		{
			UpdateData();
			m_GainControl.SetPos(m_dGain*10);
			SetGain(m_dGain);
		}
		if (pMsg->hwnd != GetDlgItem(IDC_EDIT_EPSTIME)->m_hWnd && ((CButton*)GetDlgItem(IDC_EDIT_EPSTIME))->GetFocus())
		{
			UpdateData();
			SetExposure(m_dEpsTime);
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
