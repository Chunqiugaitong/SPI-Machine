// AddSolderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "AddSolderDlg.h"
#include "afxdialogex.h"
#include "AutoDeviceDoc.h"
#include "AutoDeviceView.h"
#include "MainFrm.h"

// CAddSolderDlg dialog

IMPLEMENT_DYNAMIC(CAddSolderDlg, CDialog)

CAddSolderDlg::CAddSolderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddSolderDlg::IDD, pParent)
{

}

CAddSolderDlg::~CAddSolderDlg()
{
}

void CAddSolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAddSolderDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDOK, &CAddSolderDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAddSolderDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

// CAddSolderDlg message handlers
BOOL CAddSolderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pAddWeldingPlateDC = GetDlgItem(IDC_STATIC_ADDSOLSER)->GetDC();
	GetDlgItem(IDC_STATIC_ADDSOLSER)->GetWindowRect(m_AddWeldingPlateRect);
	ScreenToClient(m_AddWeldingPlateRect);
	m_MemDC.CreateCompatibleDC(NULL);
	m_Bmp.CreateCompatibleBitmap(m_pAddWeldingPlateDC,m_AddWeldingPlateRect.Width(),m_AddWeldingPlateRect.Height());
	m_MemDC.SelectObject(&m_Bmp);

	if (bmp1==NULL)
	{
		AfxMessageBox(L"拍照异常");
		return TRUE;
	}
	IplImage* pImg = m_PcbInspect.Bmp2Ipl(bmp1);
	m_PcbInspect.ShowImage(m_MemDC.GetSafeHdc(), pImg, m_AddWeldingPlateRect, 5, 5);
	m_PcbInspect.ReleaseImage(pImg);


	m_bIsDrawRect = false; 

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAddSolderDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages

	m_pAddWeldingPlateDC->BitBlt(0, 0, m_AddWeldingPlateRect.Width(), m_AddWeldingPlateRect.Height(), &m_MemDC, 0, 0, SRCCOPY);

}

void CAddSolderDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	GetCursorPos(&point);
	ScreenToClient(&point);

	if (m_AddWeldingPlateRect.PtInRect(point))
	{
		bPoint = point;
		ePoint = point;
		m_bIsDrawRect = true;
		//m_pAddWeldingPlateDC->Rectangle(&m_rect);
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CAddSolderDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_AddWeldingPlateRect.PtInRect(point) && m_bIsDrawRect)
	{
		GetCursorPos(&point);
		ScreenToClient(&point);

		CPen gpen(PS_SOLID,1,RGB(0,255,0));

		//重绘背景和之前画的矩形
		m_MemDC.SelectObject(&gpen);
		m_MemDC.SelectStockObject(NULL_BRUSH);
		for (unsigned int i = 0; i < m_vecRect.size(); i++)
		{
			m_MemDC.Rectangle(&m_vecRect[i]);
		}
		m_pAddWeldingPlateDC->BitBlt(0,0,m_AddWeldingPlateRect.Width(),m_AddWeldingPlateRect.Height(),&m_MemDC,0,0,SRCCOPY);
		m_rect = CRect(bPoint,ePoint);

		//绘制新的矩形框
		m_pAddWeldingPlateDC->SelectObject(&gpen);
		m_pAddWeldingPlateDC->SelectStockObject(NULL_BRUSH);
		m_pAddWeldingPlateDC->Rectangle(&m_rect);
		ePoint = point;	
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CAddSolderDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//左键抬起，将完整的矩形CRect存入Vector

	DOUBLE fX = 0,fY = 0;
	// mm/pixels
	fX = Struct_Unite.cp.dLen / m_AddWeldingPlateRect.Width();
	fY = Struct_Unite.cp.dWide / m_AddWeldingPlateRect.Height();
	/////////////////////////////////////////////////
	//把焊盘位置的像素坐标转换为机械手坐标
	//Point2f robotOriginPoint;
	RectF logicRect;
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CView *pMainView = pMain->GetActiveView();
	((CAutoDeviceView*)pMainView)->GetRealTime();
	logicRect.left = m_rect.left * fX + Struct_Unite.Real_Coor.dx;
	logicRect.right = m_rect.right * fX + Struct_Unite.Real_Coor.dx;
	logicRect.top = m_rect.top * fY + Struct_Unite.Real_Coor.dy;
	logicRect.bottom = m_rect.bottom * fY + Struct_Unite.Real_Coor.dy;

	//存储焊盘的逻辑坐标
	g_vecWeldingPlateRect.push_back(logicRect);
	//存储焊盘的设备坐标，用于保存所绘图形
	m_vecRect.push_back(m_rect);

	m_bIsDrawRect = false;

	CDialog::OnLButtonUp(nFlags, point);
}


void CAddSolderDlg::OnBnClickedOk()
{
	//清空视场下所选焊盘数组
	vector<CRect> vecTempRect;
	m_vecRect.swap(vecTempRect);
	//CDC重新选择初始画布，以清除之前画布上所绘图形
	m_MemDC.SelectObject(&m_Bmp);

	if (bmp1==NULL)
	{
		AfxMessageBox(L"拍照异常");
		CDialog::OnOK();
		return;
	}
	IplImage* pImg = m_PcbInspect.Bmp2Ipl(bmp1);
	m_PcbInspect.ShowImage(m_MemDC.GetSafeHdc(),pImg,m_AddWeldingPlateRect,4,4);
	m_PcbInspect.ReleaseImage(pImg);

	::SendMessage(GetParent()->m_hWnd,WM_UPDATEPCBMAP,enumUpdatePcbMap,0);
	CDialog::OnOK();
}


void CAddSolderDlg::OnBnClickedCancel()
{
	//清空视场下所选焊盘数组
	vector<CRect> vecTempRect;
	m_vecRect.swap(vecTempRect);
	//CDC重新选择初始画布，以清除之前画布上所绘图形
	m_MemDC.SelectObject(&m_Bmp);

	if (bmp1==NULL)
	{
		AfxMessageBox(L"拍照异常");
		CDialog::OnCancel();
		return;
	}
	IplImage* pImg = m_PcbInspect.Bmp2Ipl(bmp1);
	m_PcbInspect.ShowImage(m_MemDC.GetSafeHdc(), pImg, m_AddWeldingPlateRect, 4, 4);
	m_PcbInspect.ReleaseImage(pImg);

	CDialog::OnCancel();
}
