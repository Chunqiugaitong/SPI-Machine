
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "AutoDevice.h"

#include "AutoDeviceDoc.h"
#include "AutoDeviceView.h"
#include "ProjectView.h"
#include "MeasureView.h"
#include "BaselineCorrectView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
	ON_WM_CLOSE()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()
	ON_COMMAND(ID_MEASUREMENT, &CMainFrame::OnMeasurementView)
	ON_COMMAND(ID_INSPECTION, &CMainFrame::OnInspection)
	ON_COMMAND(ID_PCBPROJECT, &CMainFrame::OnPcbproject)
	ON_COMMAND(ID_BASELINECORRECT, &CMainFrame::OnBaselineCorrect)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_COORX,      //X轴坐标
	ID_INDICATOR_COORY,      //Y轴坐标
	ID_INDICATOR_COORZ,      //Z轴坐标
	ID_INDICATOR_COORW,      //W轴坐标
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	m_pInspectView = NULL;
	m_pMeasurementView = NULL;
	m_pPcbView = NULL;
}

CMainFrame::~CMainFrame()
{
}

bool CMainFrame::InitialToolBar()
{
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to create toolbar\n");
		return false;      // fail to create
	}

	m_wndToolBar.SetButtons(NULL,5);
	m_wndToolBar.SetButtonInfo(0,ID_IMPORT,TBSTYLE_BUTTON,0);
	m_wndToolBar.SetButtonText(0,_T("导入"));
	m_wndToolBar.SetButtonInfo(1,ID_INSPECTION,TBSTYLE_BUTTON,1);
	m_wndToolBar.SetButtonText(1,_T("检测"));
	m_wndToolBar.SetButtonInfo(2,ID_MEASUREMENT,TBSTYLE_BUTTON,2);
	m_wndToolBar.SetButtonText(2,_T("测量"));
	m_wndToolBar.SetButtonInfo(3,ID_PCBPROJECT,TBSTYLE_BUTTON,3);
	m_wndToolBar.SetButtonText(3,_T("工程"));
	m_wndToolBar.SetButtonInfo(4,ID_BASELINECORRECT,TBSTYLE_BUTTON,4);
	m_wndToolBar.SetButtonText(4,_T("基准矫正"));

	CRect rectToolBar;
	//设置按钮大小    
	m_wndToolBar.GetToolBarCtrl().SetButtonWidth(60,30);    
	//将按钮和资源图标相关联    
	//m_toolBar.GetToolBarCtrl().SetImageList(&m_ImgList);    
	m_wndToolBar.SetSizes(CSize(60,25),CSize(1,1));    //按钮尺寸，图像尺寸
	//激活鼠标提示功能    
	m_wndToolBar.EnableToolTips(TRUE);    
	//显示工具栏，并根据窗口大小自动调整工具栏的位置   
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0); 
	EnableDocking(CBRS_ALIGN_TOP);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return true;
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitialToolBar();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//状态栏添加项目设置
	m_wndStatusBar.SetPaneInfo(0, 0, SBPS_POPOUT, 280);
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_COORX, SBPS_STRETCH, 50);
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_COORY, SBPS_STRETCH, 10);
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_COORZ, SBPS_STRETCH, 10);
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_COORW, SBPS_STRETCH, 10);
	m_wndStatusBar.SetPaneInfo(5, ID_INDICATOR_CAPS, SBPS_POPOUT, 50);
	m_wndStatusBar.SetPaneInfo(6, ID_INDICATOR_NUM, SBPS_POPOUT, 50);
	m_wndStatusBar.SetPaneInfo(7, ID_INDICATOR_SCRL, SBPS_POPOUT, 50); 
	// TODO: 如果不需要可停靠工具栏，则删除这三行
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	LOGFONT    lf;
	lf.lfWidth          = 0;
	lf.lfEscapement     = 0;
	lf.lfOrientation    = 0;
	lf.lfHeight         = 14;          // 14 point
	lf.lfWeight         = FW_BOLD;      // FW_NORMAL
	lf.lfItalic         = FALSE;
	lf.lfUnderline      = FALSE;
	lf.lfStrikeOut      = 0;
	lf.lfCharSet        = ANSI_CHARSET;
	lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lf.lfQuality        = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	lstrcpy(lf.lfFaceName, TEXT("Tahoma"));
	static CFont sfontStatusBar;
	sfontStatusBar.CreateFontIndirect(&lf); 
	m_wndStatusBar.SetFont(&sfontStatusBar);

	newMenu.LoadMenu(IDR_MAINFRAME);
	SetMenu(&newMenu);
	//只更改下主菜单下的第一项
	//newMenu.ChangeMenuItem(newMenu.GetSubMenu(0));
	//更改全部：
	newMenu.ChangeMenuItem(&newMenu);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	//cs.lpszName="";
	cs.cx = cs.cy = 0;
	cs.cx = GetSystemMetrics(SM_CXSCREEN) - 60;
	cs.cy = GetSystemMetrics(SM_CYSCREEN) - 60;
	cs.style = WS_OVERLAPPED | WS_CAPTION //| FWS_ADDTOTITLE//去掉“无标题”
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZE; 
	cs.style &= ~WS_MAXIMIZEBOX; 
	cs.style &= ~WS_MINIMIZEBOX;

	m_strTitle = L" 锡 膏 印 刷 质 量 检 测 设 备";

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序


LRESULT CMainFrame::OnNcHitTest(CPoint point)//禁止鼠标拖动对话框
{
	int ret = CFrameWnd::OnNcHitTest(point);

	//if语句的前两行是用来禁止改变大小的，最后一行是用来禁止移动的
	if(HTTOP == ret || HTBOTTOM == ret || HTLEFT == ret || HTRIGHT == ret
		|| HTBOTTOMLEFT == ret || HTBOTTOMRIGHT == ret || HTTOPLEFT == ret || HTTOPRIGHT == ret || HTCAPTION == ret)
		return HTCLIENT;

	return ret;

	//	return CFrameWnd::OnNcHitTest(point);
}


void CMainFrame::ActivateFrame(int nCmdShow)
{
	nCmdShow = SW_SHOWMAXIMIZED;   

	CFrameWnd::ActivateFrame(nCmdShow);
}


void CMainFrame::OnClose()
{
 	WORD wRet = 0;
 	WORD wML = 0, wMR = 0;
 	CString strTemp = _T("");
 	CString strErr = _T("");
 	CAutoDeviceView *pView = (CAutoDeviceView*)GetActiveView();
 	if (m_bEnglish)
 	{
 		strTemp = _T("是否要退出程序？");
 	}
 	else
 	{
 		strTemp = _T("Whether to quit the program?");
 	}
 	if (IDOK == MessageBox(strTemp, ALERT_INFOR, ALERT_CONFIRMQ))
 	{
 		StopGrab();//关闭相机采集状态
 		if (pView->bConnect)
 		{
 			//if (pView->bZero)
 			//{
 				wRet = MoveSafeCoor();//移动到安全位置
 				if (wRet)
 				{
 					Err_Info(wRet, strErr);
 					MessageBox(strErr, ALERT_ERR, ALERT_ICON);
 					return;
 				}
 			//}
 			//检测通道内是否有板子，有的话传输到对应的出口并提示用户先拿出板子
 			wRet = CheckPcb();
 			if (wRet)
 			{
 				Err_Info(wRet, strErr);
 				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
 				return;
 			}
 		}
 		CloseBoard();//断开板卡
 		StopGrab();//停止连续采集
 		CloseCamera();//断开相机
 
		CAutoDeviceDoc* pAutoDeviceDoc = (CAutoDeviceDoc*)GetActiveDocument();
		if ( !pAutoDeviceDoc->m_gxmcGerber.GetPadCluster().empty())
		{
			 pAutoDeviceDoc->m_gxmcGerber.ReleasePadCluster();
		}
	   
		CFrameWnd::OnClose();
 	}	
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (lpMeasureItemStruct->CtlType == ODT_MENU)//如果类型是菜单
	{
		newMenu.MeasureItem(lpMeasureItemStruct);//调用CNewMenu类的MeasureItem成员函数
	}

	CFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);

}

void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType == ODT_MENU)
	{
		newMenu.DrawItem(lpDrawItemStruct);
	}

	CFrameWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CMainFrame::OnMeasurementView()
{
	// TODO: Add your command handler code here

	SwitchToForm(IDD_MEASURE_VIEW);
}

void CMainFrame::OnInspection()
{
	// TODO: Add your command handler code here
	
	SwitchToForm(IDD_AUTODEVICE_VIEW);
}

void CMainFrame::OnPcbproject()
{
	// TODO: Add your command handler code here

	SwitchToForm(IDD_PROJECT_VIEW);
}

void CMainFrame::OnBaselineCorrect()
{
	// TODO: Add your command handler code here
	SwitchToForm(IDD_BASELINECORRECT_VIEW);
}


void CMainFrame::SwitchToForm(int nForm)
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	//进行视图切换的代码函数
	if (nForm!=frmID)
	{
		frmID=nForm;//避免多次点击同一按钮时出现的错误，一按钮点击一次后，第二次点击不做任何处理。
		//CDocument* pDoc = GetActiveDocument();

		CView *pOldActiveView=GetActiveView();//保存旧视图
		CView *pNewActiveView=(CView*)GetDlgItem(nForm);//取得新视图
		if(pNewActiveView==NULL)
		{
			switch(nForm)
			{
			case IDD_AUTODEVICE_VIEW://视图资源符号
				pNewActiveView=(CView*)new CAutoDeviceView;
				break;
			case IDD_PROJECT_VIEW://视图资源符号
				pNewActiveView=(CView*)new CProjectView;
				break;
			case IDD_MEASURE_VIEW://视图资源符号
				pNewActiveView=(CView*)new CMeasureView;
				break;
			case IDD_BASELINECORRECT_VIEW://视图资源符号
				pNewActiveView=(CView*)new CBaselineCorrectView;
				break;
			}
			CCreateContext context; //*****将文挡和视图相连
			context.m_pCurrentDoc=pOldActiveView->GetDocument();
			pNewActiveView->Create(NULL, NULL, WS_BORDER|WS_CHILD, CFrameWnd::rectDefault, this, nForm, &context);

			pNewActiveView->OnInitialUpdate();
		}

		//改变活动的视图
		SetActiveView(pNewActiveView); 
		pNewActiveView->ShowWindow(SW_SHOW); //显示新的视图
		pOldActiveView->ShowWindow(SW_HIDE); //隐藏旧的视图
		

		//*********把正在运行的视图保存为旧的激活视图，下次视图切换时用
		if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CAutoDeviceView))
			pOldActiveView->SetDlgCtrlID(IDD_AUTODEVICE_VIEW);
		else if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CProjectView))
			pOldActiveView->SetDlgCtrlID(IDD_PROJECT_VIEW);
		else if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CMeasureView))
			pOldActiveView->SetDlgCtrlID(IDD_MEASURE_VIEW);
		else if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CBaselineCorrectView))
			pOldActiveView->SetDlgCtrlID(IDD_BASELINECORRECT_VIEW);

		//视图是主框架窗口的一个ID为AFX_IDW_PANE_FIRST，带有边框的子窗口，这个主框架窗口是由CFrameWnd类封装并创建的。
		//显然，视图作为其子窗口，也是由CFrameWnd创建的。成员函数CFrameWnd::OnCreateClient()用于创建视图窗口，它是该类的WM_CREATE消息处理函数中被调用的。
		pNewActiveView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);

		delete pOldActiveView; //删除旧视图
		RecalcLayout();//调整框架窗口
	}
}



// 	if (nForm!=frmID)
// 			return;  // already selected
// 	
// 		// Set the child window ID of the active view to AFX_IDW_PANE_FIRST.
// 		// This is necessary so that CFrameWnd::RecalcLayout will allocate
// 		// this "first pane" to that portion of the frame window's client
// 		// area not allocated to control bars.  Set the child ID of
// 		// the previously active view to some other ID; we will use the
// 		// command ID as the child ID.
// 		CView* pOldActiveView = GetActiveView();
// 		::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, frmID);
// 	
// 		CRuntimeClass* pNewViewClass;
// 		switch (nForm)
// 		{
// 		case IDD_AUTODEVICE_FORM://视图资源符号
// 			pNewViewClass=RUNTIME_CLASS(CAutoDeviceView);
// 			break;
// 		case IDD_PROJECTVIEW://视图资源符号
// 			pNewViewClass=RUNTIME_CLASS(CProjectView);
// 			break;
// 		case IDD_MEASUREVIEW://视图资源符号
// 			pNewViewClass=RUNTIME_CLASS(CMeasureView);
// 			break;
// 		default:
// 			ASSERT(0);
// 			return;
// 		}
// 	
// 		// create the new view
// 		CCreateContext context;
// 		context.m_pNewViewClass = pNewViewClass;
// 		context.m_pCurrentDoc = GetActiveDocument();
// 		CView* pNewView = STATIC_DOWNCAST(CView, CreateView(&context));
// 		if (pNewView != NULL)
// 		{
// 			// the new view is there, but invisible and not active...
// 			pNewView->ShowWindow(SW_SHOW);
// 			pNewView->OnInitialUpdate();
// 			SetActiveView(pNewView);
// 			RecalcLayout();
// 			frmID = nForm;
// 	
// 			// finally destroy the old view...
// 			pOldActiveView->ShowWindow(SW_HIDE);
// 		}
// 		return;
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


