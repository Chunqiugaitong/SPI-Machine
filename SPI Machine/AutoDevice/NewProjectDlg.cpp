// NewProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "NewProjectDlg.h"
#include "afxdialogex.h"
#include "NavigateDlg.h"
#include "AddSolderDlg.h"
#include "include/DMC2410.h"
#include "AutoDeviceDoc.h"
#include "AutoDeviceView.h"
#include "MainFrm.h"

// CNewProjectDlg dialog

IMPLEMENT_DYNAMIC(CNewProjectDlg, CDialog)

CNewProjectDlg::CNewProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewProjectDlg::IDD, pParent)
	, m_nSearchRectWidth(3)
	, m_nSearchRectHeight(3)
	, nPcbWidth(100)
	, nPcbHeight(100)
	, m_dLight(0)
	, m_nGray(0)
{
}

CNewProjectDlg::~CNewProjectDlg()
{
}

void CNewProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RECT_HEIGHT, m_nSearchRectHeight);
	DDX_Text(pDX, IDC_EDIT_RECT_WIDTH, m_nSearchRectWidth);
	DDX_Text(pDX, IDC_EDIT_PCB_WIDTH, nPcbWidth);
	DDX_Text(pDX, IDC_EDIT_PCB_HEIGHT, nPcbHeight);
	DDX_Text(pDX, IDC_EDITLIGHT, m_dLight);
	DDX_Control(pDX, IDC_SLIDERLIGHT, m_strLightControl);
	DDX_Text(pDX, IDC_EDIT_GRAY, m_nGray);
	DDX_Control(pDX, IDC_SLIDER_GRAY, m_SliderCtrlGray);
}


BEGIN_MESSAGE_MAP(CNewProjectDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_NAVIGATE, &CNewProjectDlg::OnBnClickedButtonNavigate)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_LOCATE, &CNewProjectDlg::OnBnClickedButtonAutoLocate)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_BASEPOINT1, &CNewProjectDlg::OnBnClickedButtonSaveBasepoint1)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_BASEPOINT2, &CNewProjectDlg::OnBnClickedButtonSaveBasepoint2)
	ON_BN_CLICKED(IDC_BUTTON_ADD_SOLDERPOINT, &CNewProjectDlg::OnBnClickedButtonAddSolderpoint)
	ON_BN_CLICKED(IDC_BUTTON_SET_SOLDERPARAM, &CNewProjectDlg::OnBnClickedButtonSetSolderparam)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_PCBSIZE, &CNewProjectDlg::OnBnClickedButtonModifyPcbsize)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_MODIFY_SEARCHRECTSIZE, &CNewProjectDlg::OnBnClickedButtonModifySearchrectsize)
	ON_MESSAGE(WM_UPDATEPCBMAP,OnUpdatePcbMap)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDERLIGHT, &CNewProjectDlg::OnNMCustomdrawSliderlight)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_GRAY, &CNewProjectDlg::OnNMCustomdrawSliderGray)
END_MESSAGE_MAP()


// CNewProjectDlg message handlers

BOOL CNewProjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_dLight = GetGain()*10;
	UpdateData();
	m_strLightControl.SetRange(0, 230);
	m_strLightControl.SetPos(m_dLight);

	m_SliderCtrlGray.SetRange(0, 255);
	m_SliderCtrlGray.SetPos(m_nGray);

	//PcbMap初始化
	m_pPcbMapDC = GetDlgItem(IDC_STATIC_PCBMAP)->GetDC();
	m_PcbMapMemDC.CreateCompatibleDC(NULL);
	GetDlgItem(IDC_STATIC_PCBMAP)->GetClientRect(&m_PcbMapRect);
	//GetDlgItem(IDC_STATIC_PCBMAP)->ScreenToClient(&m_PcbMapRect);
	m_PcbMapBmp.CreateCompatibleBitmap(m_pPcbMapDC,m_PcbMapRect.Width(),m_PcbMapRect.Height());
	m_PcbMapMemDC.SelectObject(&m_PcbMapBmp);

	CBrush bbrush(RGB(50,70,50));
	m_PcbMapMemDC.SelectObject(&bbrush);
	m_PcbMapMemDC.Rectangle(&m_PcbMapRect);
// 	CBrush gbrush(RGB(0,255,0));
// 	m_PcbMapMemDC.SelectObject(&gbrush);
// 	m_PcbDrawRect = CRect(m_PcbMapRect.Width()/2 - Struct_Unite.gp.dPcbLen/2, m_PcbMapRect.Height()/2 - Struct_Unite.gp.dPcbWide/2,
// 		m_PcbMapRect.Width()/2 + Struct_Unite.gp.dPcbLen/2, m_PcbMapRect.Height()/2 + Struct_Unite.gp.dPcbWide/2);
// 	m_PcbMapMemDC.Rectangle(&m_PcbDrawRect);

	//获取FOV尺寸并初始化DC
	m_pFovDC = GetDlgItem(IDC_STATIC_FOV)->GetDC();
	GetDlgItem(IDC_STATIC_FOV)->GetWindowRect(&m_FovRect);
	GetDlgItem(IDC_STATIC_FOV)->ScreenToClient(&m_FovRect);

	//比例因子f PCB板视野（mm）/MAP像素(pixel)
	m_fX = Struct_Unite.cp.dLen / m_FovRect.Width();
	m_fY = Struct_Unite.cp.dWide / m_FovRect.Height();

	//图像控件中心位置
	m_FovCenter.x = m_FovRect.left + m_FovRect.Width()/2;
	m_FovCenter.y = m_FovRect.top + m_FovRect.Height()/2;

	m_FovMemDC.CreateCompatibleDC(NULL);
	m_FovMemBmp.CreateCompatibleBitmap(m_pFovDC,m_FovRect.Width(),m_FovRect.Height());
	m_FovMemDC.SelectObject(&m_FovMemBmp);
	//g_pImg = m_PcbInspect.ReadImage("../TestPicture/img2.bmp");

	UpdateFov();
	//初始化定位圆
	m_locateCircle = Vec3f(0,0,0);

	if (g_vecWeldingPlateRect.size() > 0)
	{
		vector<RectF> vecTmp;
		g_vecWeldingPlateRect.swap(vecTmp);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNewProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		//SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		//// Center icon in client rectangle
		//int cxIcon = GetSystemMetrics(SM_CXICON);
		//int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		//GetClientRect(&rect);
		//int x = (rect.Width() - cxIcon + 1) / 2;
		//int y = (rect.Height() - cyIcon + 1) / 2;

		//// Draw the icon
		//dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{

		m_pPcbMapDC->BitBlt(0, 0, m_PcbMapRect.Width(), m_PcbMapRect.Height(), &m_PcbMapMemDC, 0, 0, SRCCOPY);
		m_pFovDC->BitBlt(0, 0, m_FovRect.Width(), m_FovRect.Height(), &m_FovMemDC, 0, 0, SRCCOPY);

		CDialog::OnPaint();
	}
}

//在PcbMap上更新指定视场下所选择的所有焊盘
LRESULT  CNewProjectDlg::OnUpdatePcbMap(WPARAM wParam,LPARAM lParam)
{
	CPen rpen(PS_SOLID,1,RGB(255,0,0));
	CBrush brush(RGB(255,0,0));
	m_PcbMapMemDC.SelectObject(&rpen);
	m_PcbMapMemDC.SelectObject(&brush);
	int flag = wParam;
	//Pcb尺寸 / PcbMap
	float fX = m_PcbMapRect.Width()/Struct_Unite.gp.dPcbLen;
	float fY = m_PcbMapRect.Height()/Struct_Unite.gp.dPcbWide;
	CPoint point,point1;
	
	switch (flag)
	{
	case enumUpdateFov:
		UpdateFov();
		break;
	case enumDrawMark1:
		point.x = /*m_PcbMapRect.Width() - */(m_robotMark1.x - Struct_Unite.axis_par.dLX - Struct_Unite.cp.dLen/2)*fX;
		point.y = /*m_PcbMapRect.Height() - */(m_robotMark1.y - Struct_Unite.axis_par.dLY- Struct_Unite.cp.dWide/2)*fY;
		m_PcbMapMemDC.Ellipse(point.x - 5,point.y - 5,point.x + 5,point.y + 5);
		break;
	case enumDrawMark2:
		point.x = /*m_PcbMapRect.Width() - */(m_robotMark2.x - Struct_Unite.axis_par.dLX - Struct_Unite.cp.dLen/2)*fX;
		point.y = /*m_PcbMapRect.Height() - */(m_robotMark2.y - Struct_Unite.axis_par.dLY- Struct_Unite.cp.dWide/2)*fY;
		m_PcbMapMemDC.Ellipse(point.x - 5,point.y - 5,point.x + 5,point.y + 5);
		break;
	case enumUpdatePcbMap:
		{
			//焊盘逻辑位置转换到PcbMap下的像素位置
			CRect rect;
			CPen bpen(PS_SOLID,1,RGB(0,255,0));
			m_PcbMapMemDC.SelectObject(&bpen);
			m_PcbMapMemDC.SelectStockObject(NULL_BRUSH);
			for (int i = 0;i<g_vecWeldingPlateRect.size();i++)
			{
				rect.left = /*m_PcbMapRect.Width() - */(g_vecWeldingPlateRect[i].left - Struct_Unite.axis_par.dLX - Struct_Unite.cp.dLen/2)*fX;
				rect.right = /*m_PcbMapRect.Width() - */(g_vecWeldingPlateRect[i].right - Struct_Unite.axis_par.dLX - Struct_Unite.cp.dLen/2)*fX;
				rect.top = /*m_PcbMapRect.Height() - */(g_vecWeldingPlateRect[i].top - Struct_Unite.axis_par.dLY - Struct_Unite.cp.dWide/2)*fY;
				rect.bottom = /*m_PcbMapRect.Height() - */(g_vecWeldingPlateRect[i].bottom - Struct_Unite.axis_par.dLY - Struct_Unite.cp.dWide/2)*fY;
				
// 				point.x =  m_PcbMapRect.Width()- (g_vecWeldingPlateRect[i].left - Struct_Unite.axis_par.dLX - Struct_Unite.cp.dLen/2)*fX;
// 				point.y =  m_PcbMapRect.Height()-(g_vecWeldingPlateRect[i].top - Struct_Unite.axis_par.dLY - Struct_Unite.cp.dWide/2)*fY;
// 				point1.x = /*m_PcbMapRect.Width()- */(g_vecWeldingPlateRect[i].right - Struct_Unite.axis_par.dLX - Struct_Unite.cp.dLen/2)*fX;
// 				point1.y = /*m_PcbMapRect.Height()-*/(g_vecWeldingPlateRect[i].bottom - Struct_Unite.axis_par.dLY - Struct_Unite.cp.dWide/2)*fY;
// 				m_PcbMapMemDC.Ellipse(point.x - 10,point.y - 10,point.x + 10,point.y + 10);
// 				m_PcbMapMemDC.Ellipse(point1.x - 3,point1.y - 3,point1.x + 3,point1.y + 3);
 				
				m_PcbMapMemDC.Rectangle(&rect);
			}

			m_pPcbMapDC->BitBlt(0,0,m_PcbMapRect.Width(),m_PcbMapRect.Height(),&m_PcbMapMemDC,0,0,SRCCOPY);

		}

		break;

	}
	m_pPcbMapDC->BitBlt(0,0,m_PcbMapRect.Width(),m_PcbMapRect.Height(),&m_PcbMapMemDC,0,0,SRCCOPY);
	return 1L;
}

void CNewProjectDlg::OnBnClickedButtonNavigate()
{
	//m_navigateDlg.ShowWindow(SW_SHOW);
	CNavigateDlg Dlg;
	Dlg.nPcbHeight = Struct_Unite.gp.dPcbLen;
	Dlg.nPcbWidth = Struct_Unite.gp.dPcbWide;
//	Dlg.m_curPoint = g_curPoint;
	Dlg.DoModal();
}

void CNewProjectDlg::OnBnClickedButtonAutoLocate()
{
	//机械手移动到检测圆心所对应的机械手点位
	if (m_roiMat.empty())
	{
		return;
	}
	m_locateCircle = m_PcbInspect.CircleFit(m_roiMat);
	CPoint circleCenter;
	//圆心在图像中的位置 = 搜索框左上角 + 圆心
	circleCenter.x = m_locateCircle[0]/6 + m_searchRect.left;
	circleCenter.y = m_locateCircle[1]/6 + m_searchRect.top;
	if (m_locateCircle == Vec3f(0,0,0) || !m_searchRect.PtInRect(circleCenter))
	{
		MessageBox(L"mark点定位失败！");
		return;
	}
	double dStartX = 0, dStartY = 0, dZ = 0;
	WORD wRet = 0;
	CString strErr = _T("");

	dZ = Struct_Unite.as.dZHigh;
	Struct_Unite.axis_par.wDirect = 0;
	if (0 == Struct_Unite.axis_par.wDirect)
	{
		//图像中X轴正方向与电机轴正方向相反
		dStartX = Struct_Unite.Real_Coor.dx - m_fX*(circleCenter.x - m_FovCenter.x);
		//图像Y轴正方向（像素坐标）与电机轴正方向（逻辑坐标）原本就是相反的
		dStartY = Struct_Unite.Real_Coor.dy - m_fY*(circleCenter.y - m_FovCenter.y);
	}
	else if (1 == Struct_Unite.axis_par.wDirect)
	{
		//图像中X轴正方向与电机轴正方向相同
		dStartX = Struct_Unite.Real_Coor.dx + m_fX*(circleCenter.x - m_FovCenter.x);
		//图像Y轴正方向（像素坐标）与电机轴正方向（逻辑坐标）原本就是相反的
		dStartY = Struct_Unite.Real_Coor.dy - m_fY*(circleCenter.y - m_FovCenter.y);
	}

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
		Struct_Unite.axis_par.dXTs, 0, dStartX, 1, 0);
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	wRet = Axis_Move(Struct_Unite.axis_par.wYAxis, Struct_Unite.axis_par.dYMinVel, Struct_Unite.axis_par.dYMaxVel,
		Struct_Unite.axis_par.dYTAcc, Struct_Unite.axis_par.dYTDec,
		Struct_Unite.axis_par.dYTs, 0, dStartY, 1, 0);
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
	sType = 1;
	wRet = ExecuteGrab();
	Sleep(200);
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	UpdateFov();
}

void CNewProjectDlg::OnBnClickedButtonSaveBasepoint1()
{
	//保存第一个定位点对应的机械手位置
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CView *pMainView = pMain->GetActiveView();
	((CAutoDeviceView*)pMainView)->GetRealTime();
	m_robotMark1.x = Struct_Unite.Real_Coor.dx;
	m_robotMark1.y = Struct_Unite.Real_Coor.dy;

	CGxmcDatabase::Mark mark;
	mark.ID = 0;
	mark.PosXmm = m_robotMark1.x;
	mark.PosYmm = m_robotMark1.y;
	CGxmcDatabase::MarkElement me;
	m_gxmcGerber.m_dataBase.ConnectAdo(g_strFileName);
	m_gxmcGerber.m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.Mark,CGxmcDatabase::s_markElement,mark);
	//发送消息绘制Mark点1
	::SendMessage(this->m_hWnd, WM_UPDATEPCBMAP ,enumDrawMark1, 0);
}

void CNewProjectDlg::OnBnClickedButtonSaveBasepoint2()
{
	//保存第二个定位点对应的机械手位置
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CView *pMainView = pMain->GetActiveView();
	((CAutoDeviceView*)pMainView)->GetRealTime();
	m_robotMark2.x = Struct_Unite.Real_Coor.dx;
	m_robotMark2.y = Struct_Unite.Real_Coor.dy;

	CGxmcDatabase::Mark mark;
	mark.ID = 1;
	mark.PosXmm = m_robotMark2.x;
	mark.PosYmm = m_robotMark2.y;
	m_gxmcGerber.m_dataBase.ConnectAdo(g_strFileName);
	m_gxmcGerber.m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.Mark,CGxmcDatabase::s_markElement,mark);
	//发送消息绘制Mark点1
	::SendMessage(this->m_hWnd, WM_UPDATEPCBMAP, enumDrawMark2, 0);
}

void CNewProjectDlg::OnBnClickedButtonAddSolderpoint()
{
	//m_addWeldingPlateDlg.ShowWindow(SW_SHOW);
	CAddSolderDlg Dlg;
	Dlg.nPcbWidth = Struct_Unite.gp.dPcbLen;
	Dlg.nPcbWidth = Struct_Unite.gp.dPcbWide;
	Dlg.DoModal();
}

void CNewProjectDlg::OnBnClickedButtonSetSolderparam()
{
	if (g_vecWeldingPlateRect.size() == 0)
	{
		AfxMessageBox(L"还未添加焊盘！");
		return;
	}

	HWND hwnd=::FindWindow(NULL,L"元件编辑");
	if(hwnd==NULL)
	{
		//没有找到接收窗体
		WinExec("EditComponent.exe", SW_SHOW); 
		hwnd=::FindWindow(NULL,L"元件编辑");
	}
	
 	RectF* pRect;
 	int count = g_vecWeldingPlateRect.size();
 	pRect = new RectF[count];
 	for (int i = 0; i < count; i++)
 	{
 		pRect[i] = g_vecWeldingPlateRect[i];
 	}

	COPYDATASTRUCT data1; 
	data1.dwData = enumStruct_Unite;
	data1.cbData = sizeof(STRUCT_UNITE);
	data1.lpData = &Struct_Unite; 
	LRESULT lr = ::SendMessage(hwnd, WM_COPYDATA,0, (LPARAM)&data1);

	COPYDATASTRUCT data2; 
	data2.dwData = enumPadLogicPos;
	data2.cbData = g_vecWeldingPlateRect.size()*sizeof(RectF);
	data2.lpData = pRect;  
	lr = ::SendMessage(hwnd, WM_COPYDATA,0, (LPARAM)&data2);
	//链接数据库，存储焊盘中心点到Pad表
	m_gxmcGerber.m_dataBase.ConnectAdo(g_strFileName);
 	CGxmcDatabase::Pad pad;
 	for (int i = 0; i < g_vecWeldingPlateRect.size(); i++)
 	{
 	   pad.ID = i;
	   pad.PadID = i;
 	   pad.PosXmm = (g_vecWeldingPlateRect[i].left + g_vecWeldingPlateRect[i].right)/2;
 	   pad.PosYmm = (g_vecWeldingPlateRect[i].top + g_vecWeldingPlateRect[i].bottom)/2;
       m_gxmcGerber.m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_padElement,pad);
 	}
	delete[] pRect;
	pRect = NULL;

	COPYDATASTRUCT data3; 
	int nCount = WideCharToMultiByte(CP_ACP,0,g_strFileName,-1,NULL,0,NULL,NULL);
	char ch[256];
	WideCharToMultiByte(CP_ACP,0,g_strFileName,-1,ch,256,NULL,NULL);
	data3.dwData = enumProjectPathName;
	data3.cbData = 256;
	data3.lpData = ch;  
	lr = ::SendMessage(hwnd, WM_COPYDATA,0, (LPARAM)&data3);

}

void CNewProjectDlg::OnBnClickedButtonModifyPcbsize()
{
	UpdateData(TRUE);
	CBrush gbrush(RGB(0,255,0));
	CBrush bbrush(RGB(50,70,50));
	m_pPcbMapDC->SelectObject(&bbrush);
	m_pPcbMapDC->Rectangle(m_PcbMapRect);

	m_pPcbMapDC->SelectObject(&gbrush);
	m_PcbDrawRect = CRect(m_PcbMapRect.Width()/2 - Struct_Unite.gp.dPcbLen/2,m_PcbMapRect.Height()/2 - Struct_Unite.gp.dPcbWide/2,
		m_PcbMapRect.Width()/2 + Struct_Unite.gp.dPcbLen/2,m_PcbMapRect.Height()/2 + Struct_Unite.gp.dPcbWide/2);
	m_pPcbMapDC->Rectangle(&m_PcbDrawRect);

	UpdateData(FALSE);
}

//电机坐标微调后拍摄的FOV
void CNewProjectDlg::UpdateFov()
{
	//绘制FOV图像
	CBrush FovBrush(RGB(0,0,45));
	m_FovMemDC.SelectObject(&FovBrush);
	
	IplImage* pImg = NULL;
	if (bmp1==NULL)
	{
		AfxMessageBox(L"拍照异常");
		return;
	}
	pImg = m_PcbInspect.Bmp2Ipl(bmp1);
	m_PcbInspect.ShowImage(m_FovMemDC.GetSafeHdc(),pImg,m_FovRect,6,6);
	m_PcbInspect.ReleaseImage(pImg);

	//绘制图像搜索区域
	CPen bPen(PS_SOLID,1,RGB(0,0,255));
	m_FovMemDC.SelectObject(&bPen);
	m_FovMemDC.SelectStockObject(NULL_BRUSH);

	//绘制搜索框 逻辑坐标转像素坐标
	int SearchRectWidth = m_nSearchRectWidth /m_fX;
	int SearchRectHeight = m_nSearchRectHeight /m_fY;

	if (SearchRectWidth > m_FovRect.Width() || SearchRectHeight > m_FovRect.Height())
	{
		AfxMessageBox(L"搜索区域超过视场边界");
		return;
	}
	m_searchRect = CRect(m_FovCenter.x - SearchRectWidth/2,m_FovCenter.y - SearchRectHeight/2,
		                 m_FovCenter.x + SearchRectWidth/2,m_FovCenter.y + SearchRectHeight/2);

	//ROI区域阈值化
	m_roiMat = m_PcbInspect.SetRoiRect(pImg,m_searchRect,6,6);
	threshold (m_roiMat, m_roiMat, m_nGray, 255, CV_THRESH_BINARY); 
	m_PcbInspect.ShowImage(m_FovMemDC.GetSafeHdc(),m_roiMat,m_searchRect,6,6);
	m_FovMemDC.Rectangle(m_searchRect);

	//绘制FOV十字叉
	CPen rPen(PS_SOLID,1,RGB(255,0,0));
	m_FovMemDC.SelectObject(&rPen);
	m_FovMemDC.MoveTo(CPoint(0,m_FovRect.Height()/2));
	m_FovMemDC.LineTo(CPoint(m_FovRect.Width(),m_FovRect.Height()/2));
	m_FovMemDC.MoveTo(CPoint(m_FovRect.Width()/2,0));
	m_FovMemDC.LineTo(CPoint(m_FovRect.Width()/2,m_FovRect.Height()));

	m_pFovDC->BitBlt(0,0,m_FovRect.Width(),m_FovRect.Height(),&m_FovMemDC,0,0,SRCCOPY);
}

void CNewProjectDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 	CPoint Point;
	// 	GetCursorPos(&Point);//屏幕坐标
	// 	ScreenToClient(&Point);//屏幕坐标转客户区坐标
	// 	CRect rect;
	// 	GetDlgItem(IDC_STATIC_FOV)->GetWindowRect(rect);//获取控件在屏幕坐标系下的区域坐标
	// 	ScreenToClient(&rect);//控件区域从屏幕坐标转换为客户区坐标
	//point参数为客户区坐标

    WORD wRet = 0; 
	CString strErr = _T("");

	DOUBLE dX = 0, dY = 0, dZ =0;

	if (m_FovRect.PtInRect(point))
	{
		CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		CView *pMainView = pMain->GetActiveView();
		((CAutoDeviceView*)pMainView)->GetRealTime();

		//根据坐标偏离视场中心的偏移量，计算对应机械手的位置
		dX = Struct_Unite.Real_Coor.dx - m_fX*(point.x - m_FovCenter.x);
		dY = Struct_Unite.Real_Coor.dy - m_fY*(point.y - m_FovCenter.y);
		dZ = Struct_Unite.Real_Coor.dz;

		if (dX > Struct_Unite.axis_par.dXTrip || dX < Struct_Unite.axis_par.dXTripNegaTive)
		{
			return;
		}
		if (dY > Struct_Unite.axis_par.dYTrip || dY < Struct_Unite.axis_par.dYTripNegaTive)
		{
			return;
		}

		wRet = Axis_Move(Struct_Unite.axis_par.wXAxis, Struct_Unite.axis_par.dXMinVel, Struct_Unite.axis_par.dXMaxVel,
			Struct_Unite.axis_par.dXTAcc, Struct_Unite.axis_par.dXTDec,
			Struct_Unite.axis_par.dXTs, 0, dX, 1, 0);
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		wRet = Axis_Move(Struct_Unite.axis_par.wYAxis, Struct_Unite.axis_par.dYMinVel, Struct_Unite.axis_par.dYMaxVel,
			Struct_Unite.axis_par.dYTAcc, Struct_Unite.axis_par.dYTDec,
			Struct_Unite.axis_par.dYTs, 0, dY, 1, 0);
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
		//WaitTime(80);
		sType = 1;
		wRet = ExecuteGrab();
		Sleep(200);//cj
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}

		UpdateFov();
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CNewProjectDlg::OnBnClickedButtonModifySearchrectsize()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	UpdateFov();
}

void CNewProjectDlg::OnNMCustomdrawSliderlight(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	int iPos = 0;
	WORD wRet = 0;
	CString strErr = _T("");

	iPos = m_strLightControl.GetPos();
	m_dLight = (DOUBLE)iPos/10;
	UpdateData(FALSE);
	SetGain(m_dLight);
	::DeleteObject(bmp1);//释放资源
	//WaitTime(80);
	sType = 1;
	wRet = ExecuteGrab();
	Sleep(200);//cj
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	UpdateFov();

	*pResult = 0;
}

void CNewProjectDlg::OnNMCustomdrawSliderGray(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int iPos = 0;
	WORD wRet = 0;
	CString strErr = _T("");

	iPos = m_SliderCtrlGray.GetPos();
	m_nGray = (DOUBLE)iPos;
	UpdateData(FALSE);

	::DeleteObject(bmp1);//释放资源
	//WaitTime(80);
	sType = 1;
	wRet = ExecuteGrab();
	Sleep(200);//cj
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
    UpdateFov();

	*pResult = 0;
}
