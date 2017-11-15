// CheckParDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "CheckParDlg.h"
#include "afxdialogex.h"
#include "include/DMC2410.h"


// CCheckParDlg dialog

IMPLEMENT_DYNAMIC(CCheckParDlg, CDialog)

CCheckParDlg::CCheckParDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckParDlg::IDD, pParent)
	, m_strName(_T(""))
	, m_dLen(0)
	, m_dWide(0)
	, m_dHigh(0)
	, m_dMarkX1(0)
	, m_dMarkY1(0)
	, m_dMarkX2(0)
	, m_dMarkY2(0)
	, m_strFilePath(_T(""))
	, wIsGerber(0)
	, strGerberName(_T(""))
{

}

CCheckParDlg::~CCheckParDlg()
{
}

void CCheckParDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PCBNAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_PCBLEN, m_dLen);
	DDX_Text(pDX, IDC_EDIT_PCBWIDE, m_dWide);
	DDX_Text(pDX, IDC_EDIT_PCBHIGH, m_dHigh);
	DDX_Text(pDX, IDC_EDIT_MARK1X, m_dMarkX1);
	DDX_Text(pDX, IDC_EDIT_MARK1Y, m_dMarkY1);
	DDX_Text(pDX, IDC_EDIT_MARK2X, m_dMarkX2);
	DDX_Text(pDX, IDC_EDIT_MARK2Y, m_dMarkY2);
	DDX_Text(pDX, IDC_EDIT_FILEPATH, m_strFilePath);
}


BEGIN_MESSAGE_MAP(CCheckParDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_WIDEEXEC, &CCheckParDlg::OnBnClickedBtnWideexec)
	ON_EN_CHANGE(IDC_EDIT_PCBWIDE, &CCheckParDlg::OnEnChangeEditPcbwide)
	ON_BN_CLICKED(IDC_CHECK_FILE, &CCheckParDlg::OnBnClickedCheckFile)
	ON_BN_CLICKED(IDC_BTN_IMPORT, &CCheckParDlg::OnBnClickedBtnImport)
END_MESSAGE_MAP()


// CCheckParDlg message handlers


BOOL CCheckParDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData();
	if (wIsGerber)
	{
		((CButton*)GetDlgItem(IDC_CHECK_FILE))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_BTN_IMPORT))->ShowWindow(SW_SHOW);
		m_strFilePath = GetFilePath() + _T("Gerber") + '\\' + strGerberName;
	}
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCheckParDlg::OnBnClickedBtnWideexec()
{
	WORD wRet = 0;
	CString strErr = _T("");
	UpdateData();
	if (m_dWide == 0)
	{
		MessageBox(_T("PCB板宽度值不能为空！"), ALERT_ERR, ALERT_ICON);
		return;
	}
	if (m_dWide <Struct_Unite.axis_par.dWMinTrip)
	{
		strErr.Format(_T("宽度设置不能小于最小宽度值，最小宽度为%.03f"), Struct_Unite.axis_par.dWMinTrip);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	if (m_dWide > Struct_Unite.axis_par.dWTrip)
	{
		strErr.Format(_T("宽度设置不能大于轨道原点宽度，原点宽度为%.03f"), Struct_Unite.axis_par.dWTrip);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	wRet = Axis_Move(Struct_Unite.axis_par.wWAxis, Struct_Unite.axis_par.dWMinVel, Struct_Unite.axis_par.dWMaxVel,
		Struct_Unite.axis_par.dWTAcc, Struct_Unite.axis_par.dWTDec,
		Struct_Unite.axis_par.dWTs, 0, Struct_Unite.axis_par.dWTrip - m_dWide, 1, 0);
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
}


void CCheckParDlg::OnOK()
{
	CString strErr = _T("");
	UpdateData();
	if (0 == m_strName.GetLength())
	{
		MessageBox(_T("PCB名称不能为空！"), ALERT_ERR, ALERT_ICON);
		return;
	}
	if (m_dWide <Struct_Unite.axis_par.dWMinTrip)
	{
		m_dWide = 0;
		UpdateData(FALSE);
		strErr.Format(_T("宽度设置不能小于最小宽度值，最小宽度为%.03f"), Struct_Unite.axis_par.dWMinTrip);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	if (m_dWide > Struct_Unite.axis_par.dWTrip)
	{
		m_dWide = 0;
		UpdateData(FALSE);
		strErr.Format(_T("宽度设置不能大于轨道原点宽度，原点宽度为%.03f"), Struct_Unite.axis_par.dWTrip);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	if (((CButton*)GetDlgItem(IDC_CHECK_FILE))->GetCheck())
	{
		if (0 == m_strFilePath.GetLength())
		{
			strErr.Format(_T("请正确选择待导入的Gerber文件"));
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
	}

	Struct_Unite.gp.strName = m_strName;
	Struct_Unite.gp.dPcbLen = m_dLen;
	Struct_Unite.gp.dPcbWide = m_dWide;
	Struct_Unite.gp.dPly = m_dHigh;
	Struct_Unite.gp.dMarkX1 = m_dMarkX1;
	Struct_Unite.gp.dMarkY1 = m_dMarkY1;
	Struct_Unite.gp.dMarkX2 = m_dMarkX2;
	Struct_Unite.gp.dMarkY2 = m_dMarkY2;
	Struct_Unite.gp.wIsGerber = wIsGerber;
	Struct_Unite.gp.strGerberName = strGerberName;

	CDialog::OnOK();
}

void CCheckParDlg::OnEnChangeEditPcbwide()
{
	//CString strErr = _T("");
	//UpdateData();
	//if (m_dWide <Struct_Unite.axis_par.dWMinTrip)
	//{
	//	m_dWide = 0;
	//	UpdateData(FALSE);
	//	strErr.Format(_T("宽度设置不能小于最小宽度值，最小宽度为%.03f"), Struct_Unite.axis_par.dWMinTrip);
	//	MessageBox(strErr, ALERT_ERR, ALERT_ICON);
	//	return;
	//}
	//if (m_dWide > Struct_Unite.axis_par.dWTrip)
	//{
	//	m_dWide = 0;
	//	UpdateData(FALSE);
	//	strErr.Format(_T("宽度设置不能大于轨道原点宽度，原点宽度为%.03f"), Struct_Unite.axis_par.dWTrip);
	//	MessageBox(strErr, ALERT_ERR, ALERT_ICON);
	//	return;
	//}
}


void CCheckParDlg::OnBnClickedCheckFile()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_FILE))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_BTN_IMPORT))->ShowWindow(SW_SHOW);
		wIsGerber = 1;
	}
	else
	{
		((CButton*)GetDlgItem(IDC_BTN_IMPORT))->ShowWindow(SW_HIDE);
		wIsGerber = 0;
	}
}


void CCheckParDlg::OnBnClickedBtnImport()
{
	CString strTemp = _T("");
	CString strPath = _T(""), strIniPath = _T("");
	TCHAR szIniPath[512] = _T("");
	strPath = GetFilePath();
	strPath +=_T("Gerber");
	CFileDialog FileDlg(TRUE,_T("*"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		_T("All Files|*.*||"), this);
	FileDlg.m_ofn.lpstrTitle = _T("Open File");
	FileDlg.m_ofn.lpstrInitialDir = strPath;
	if (IDOK == FileDlg.DoModal())
	{
		m_strFilePath = FileDlg.GetPathName();
		strGerberName = FileDlg.GetFileName();
		UpdateData(FALSE);
	}
}
