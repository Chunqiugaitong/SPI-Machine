// NavigateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "NavigateDlg.h"
#include "afxdialogex.h"
#include "include/DMC2410.h"
#include "AutoDeviceDoc.h"
#include "AutoDeviceView.h"
#include "MainFrm.h"


// CNavigateDlg dialog

IMPLEMENT_DYNAMIC(CNavigateDlg, CDialog)

CNavigateDlg::CNavigateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNavigateDlg::IDD, pParent)
{

}

CNavigateDlg::~CNavigateDlg()
{
}

void CNavigateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNavigateDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDOK, &CNavigateDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CNavigateDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CNavigateDlg message handlers

BOOL CNavigateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//初始化导航地图
	m_pPcbMapDC = GetDlgItem(IDC_STATIC_NAVIGATE_PCBMAP)->GetDC();
	GetDlgItem(IDC_STATIC_NAVIGATE_PCBMAP)->GetClientRect(&m_pcbMapRect);
	m_prePoint = CPoint(m_pcbMapRect.right,m_pcbMapRect.bottom);
	//控制鼠标当前点及机械手实际坐标 重要 CJ 3.23
	if (g_curPoint == CPoint(0,0))
	{
		g_curPoint = m_prePoint;
	}
	else
	{
		 m_prePoint = g_curPoint;
	}
	
	m_PcbMapMemDC.CreateCompatibleDC(NULL);
	m_PcbMapBmp.CreateCompatibleBitmap(m_pPcbMapDC,m_pcbMapRect.Width(),m_pcbMapRect.Height());
	m_PcbMapMemDC.SelectObject(&m_PcbMapBmp);

	CPen pen(PS_DOT,2,RGB(0,205,0));
	CPen rpen(PS_DOT,2,RGB(255,0,0));
	CBrush brush(RGB(0,205,0));
	CBrush rbrush(RGB(255,0,0));

	//pcb在图像控件中的范围
	//比例因子f PCB机械手坐标（mm）/MAP像素坐标(pixel)
	float fX = nPcbWidth/m_pcbMapRect.Width();
	float fY = nPcbHeight/m_pcbMapRect.Height();
	CRect rect(m_pcbMapRect.right - nPcbWidth/fX,m_pcbMapRect.bottom - nPcbHeight/fY,m_pcbMapRect.right,m_pcbMapRect.bottom);
	//画背景
	m_PcbMapMemDC.SelectObject(&pen);
	m_PcbMapMemDC.SelectStockObject(NULL_BRUSH);
	m_PcbMapMemDC.Rectangle(&rect);
	m_pPcbMapDC->BitBlt(0,0,m_pcbMapRect.Width(), m_pcbMapRect.Height(), &m_PcbMapMemDC, 0, 0, SRCCOPY);
	//画机械手当前位置指示点
	m_pPcbMapDC->SelectObject(&rpen);
	m_pPcbMapDC->SelectObject(&rbrush);
	m_pPcbMapDC->Rectangle(g_curPoint.x - 5, g_curPoint.y - 5, g_curPoint.x +5, g_curPoint.y +5);

	//初始化FOV
	m_pFovDC = GetDlgItem(IDC_STATIC_NAVIGATE_FOV)->GetDC();

	GetDlgItem(IDC_STATIC_NAVIGATE_FOV)->GetClientRect(&m_FovRect);
	m_FovMemDC.CreateCompatibleDC(NULL);
	m_FovMemBmp.CreateCompatibleBitmap(m_pFovDC, m_FovRect.Width(), m_FovRect.Height());
	m_FovMemDC.SelectObject(&m_FovMemBmp);

	CBrush FovBrush(RGB(0, 0, 45));
	m_FovMemDC.SelectObject(&FovBrush);
	m_FovMemDC.Rectangle(m_FovRect);

	//IplImage* pImg = m_PcbInspect.ReadImage("../TestPicture/img2.bmp");
	m_pFovImg = NULL;
	if (bmp1==NULL)
	{
		AfxMessageBox(L"拍照异常");
		return TRUE;
	}
	m_pFovImg = m_PcbInspect.Bmp2Ipl(bmp1);
    m_PcbInspect.ShowImage(m_FovMemDC.GetSafeHdc(), m_pFovImg, m_FovRect, 6, 6);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNavigateDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages

	m_pPcbMapDC->BitBlt(0,0,m_pcbMapRect.Width(), m_pcbMapRect.Height(), &m_PcbMapMemDC, 0, 0, SRCCOPY);
	m_pPcbMapDC->Rectangle(g_curPoint.x - 5, g_curPoint.y - 5, g_curPoint.x +5, g_curPoint.y +5);

	m_pFovDC->BitBlt(0,0,m_FovRect.Width(), m_FovRect.Height(), &m_FovMemDC, 0, 0, SRCCOPY);
}

void CNavigateDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	WORD wRet = 0;
	CString strTemp = _T(""), strErr = _T("");
	double dOffX = 22, dOffY = 22, dx = 0, dy = 0, dStartX = 0, dStartY = 0, dZ = 0;
	if (m_pcbMapRect.PtInRect(point))
	{
		//比例因子f PCB机械手坐标（mm）/MAP像素坐标(pixel)
		float fX = nPcbWidth/m_pcbMapRect.Width();
		float fY = nPcbHeight/m_pcbMapRect.Height();

		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		CView *pMainView = pMain->GetActiveView();
		((CAutoDeviceView*)pMainView)->GetRealTime();

		dStartX = Struct_Unite.Real_Coor.dx - fX*(point.x - m_prePoint.x);
		dStartY = Struct_Unite.Real_Coor.dy - fY*(point.y - m_prePoint.y);
		dZ = Struct_Unite.Real_Coor.dz;
		if (dStartX > Struct_Unite.axis_par.dXTrip || dStartX < Struct_Unite.axis_par.dXTripNegaTive)
		{
			return;
		}
		if (dStartY > Struct_Unite.axis_par.dYTrip || dStartY < Struct_Unite.axis_par.dYTripNegaTive)
		{
			return;
		}

		wRet = Axis_Move(Struct_Unite.axis_par.wXAxis, Struct_Unite.axis_par.dXMinVel, Struct_Unite.axis_par.dXMaxVel,
			Struct_Unite.axis_par.dXTAcc, Struct_Unite.axis_par.dXTDec,
			Struct_Unite.axis_par.dXTs, 0, dStartX + dx, 1, 0);
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		wRet = Axis_Move(Struct_Unite.axis_par.wYAxis, Struct_Unite.axis_par.dYMinVel, Struct_Unite.axis_par.dYMaxVel,
			Struct_Unite.axis_par.dYTAcc, Struct_Unite.axis_par.dYTDec,
			Struct_Unite.axis_par.dYTs, 0, dStartY + dy, 1, 0);
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		wRet = Axis_Move(Struct_Unite.axis_par.wZAxis, Struct_Unite.axis_par.dZMinVel, Struct_Unite.axis_par.dZMaxVel,
			Struct_Unite.axis_par.dZTAcc, Struct_Unite.axis_par.dZTDec,
			Struct_Unite.axis_par.dZTs, 0, dZ, 1, 0);
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		do 
		{
			DoEvents();
			//	if (bAutoFlag) return wRet;
		} while (d2410_check_done(Struct_Unite.axis_par.wXAxis) == 0 || d2410_check_done(Struct_Unite.axis_par.wYAxis) == 0 || d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0);//轴正在运行
		::DeleteObject(bmp1);//释放资源
		Sleep(50);
		sType = 1;
		wRet = ExecuteGrab();
		Sleep(200);
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		g_curPoint = point;
		//记录上次鼠标坐标
		m_prePoint = point;
		//在Map上指示当前位置
		m_pPcbMapDC->BitBlt(0,0,m_pcbMapRect.Width(), m_pcbMapRect.Height(), &m_PcbMapMemDC, 0, 0, SRCCOPY);
		m_pPcbMapDC->Rectangle(point.x - 5, point.y - 5, point.x +5, point.y +5);
		//显示当前视场
	    m_pFovImg = m_PcbInspect.Bmp2Ipl(bmp1);
		m_PcbInspect.ShowImage(m_FovMemDC.GetSafeHdc(), m_pFovImg, m_FovRect, 6, 6);
		m_pFovDC->BitBlt(0,0,m_FovRect.Width(), m_FovRect.Height(), &m_FovMemDC, 0, 0, SRCCOPY);
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CNavigateDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

//	g_curPoint = m_curPoint;

	HWND hWnd = ::FindWindow(NULL,L"NewProject");
	if (hWnd != NULL)
	{
		::SendMessage(hWnd, WM_UPDATEPCBMAP, enumUpdateFov, 0);
	}
	
	CDialog::OnOK();
}


void CNavigateDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
//	g_curPoint = m_curPoint;
	CDialog::OnCancel();
}
