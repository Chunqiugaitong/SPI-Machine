
// MainFrm.cpp : CMainFrame ���ʵ��
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
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_COORX,      //X������
	ID_INDICATOR_COORY,      //Y������
	ID_INDICATOR_COORZ,      //Z������
	ID_INDICATOR_COORW,      //W������
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

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
	m_wndToolBar.SetButtonText(0,_T("����"));
	m_wndToolBar.SetButtonInfo(1,ID_INSPECTION,TBSTYLE_BUTTON,1);
	m_wndToolBar.SetButtonText(1,_T("���"));
	m_wndToolBar.SetButtonInfo(2,ID_MEASUREMENT,TBSTYLE_BUTTON,2);
	m_wndToolBar.SetButtonText(2,_T("����"));
	m_wndToolBar.SetButtonInfo(3,ID_PCBPROJECT,TBSTYLE_BUTTON,3);
	m_wndToolBar.SetButtonText(3,_T("����"));
	m_wndToolBar.SetButtonInfo(4,ID_BASELINECORRECT,TBSTYLE_BUTTON,4);
	m_wndToolBar.SetButtonText(4,_T("��׼����"));

	CRect rectToolBar;
	//���ð�ť��С    
	m_wndToolBar.GetToolBarCtrl().SetButtonWidth(60,30);    
	//����ť����Դͼ�������    
	//m_toolBar.GetToolBarCtrl().SetImageList(&m_ImgList);    
	m_wndToolBar.SetSizes(CSize(60,25),CSize(1,1));    //��ť�ߴ磬ͼ��ߴ�
	//���������ʾ����    
	m_wndToolBar.EnableToolTips(TRUE);    
	//��ʾ�������������ݴ��ڴ�С�Զ�������������λ��   
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
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//״̬�������Ŀ����
	m_wndStatusBar.SetPaneInfo(0, 0, SBPS_POPOUT, 280);
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_COORX, SBPS_STRETCH, 50);
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_COORY, SBPS_STRETCH, 10);
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_COORZ, SBPS_STRETCH, 10);
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_COORW, SBPS_STRETCH, 10);
	m_wndStatusBar.SetPaneInfo(5, ID_INDICATOR_CAPS, SBPS_POPOUT, 50);
	m_wndStatusBar.SetPaneInfo(6, ID_INDICATOR_NUM, SBPS_POPOUT, 50);
	m_wndStatusBar.SetPaneInfo(7, ID_INDICATOR_SCRL, SBPS_POPOUT, 50); 
	// TODO: �������Ҫ��ͣ������������ɾ��������
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
	//ֻ���������˵��µĵ�һ��
	//newMenu.ChangeMenuItem(newMenu.GetSubMenu(0));
	//����ȫ����
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
	cs.style = WS_OVERLAPPED | WS_CAPTION //| FWS_ADDTOTITLE//ȥ�����ޱ��⡱
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZE; 
	cs.style &= ~WS_MAXIMIZEBOX; 
	cs.style &= ~WS_MINIMIZEBOX;

	m_strTitle = L" �� �� ӡ ˢ �� �� �� �� �� ��";

	return TRUE;
}

// CMainFrame ���

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


// CMainFrame ��Ϣ�������


LRESULT CMainFrame::OnNcHitTest(CPoint point)//��ֹ����϶��Ի���
{
	int ret = CFrameWnd::OnNcHitTest(point);

	//if����ǰ������������ֹ�ı��С�ģ����һ����������ֹ�ƶ���
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
 		strTemp = _T("�Ƿ�Ҫ�˳�����");
 	}
 	else
 	{
 		strTemp = _T("Whether to quit the program?");
 	}
 	if (IDOK == MessageBox(strTemp, ALERT_INFOR, ALERT_CONFIRMQ))
 	{
 		StopGrab();//�ر�����ɼ�״̬
 		if (pView->bConnect)
 		{
 			//if (pView->bZero)
 			//{
 				wRet = MoveSafeCoor();//�ƶ�����ȫλ��
 				if (wRet)
 				{
 					Err_Info(wRet, strErr);
 					MessageBox(strErr, ALERT_ERR, ALERT_ICON);
 					return;
 				}
 			//}
 			//���ͨ�����Ƿ��а��ӣ��еĻ����䵽��Ӧ�ĳ��ڲ���ʾ�û����ó�����
 			wRet = CheckPcb();
 			if (wRet)
 			{
 				Err_Info(wRet, strErr);
 				MessageBox(strErr, ALERT_ERR, ALERT_ICON);
 				return;
 			}
 		}
 		CloseBoard();//�Ͽ��忨
 		StopGrab();//ֹͣ�����ɼ�
 		CloseCamera();//�Ͽ����
 
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
	if (lpMeasureItemStruct->CtlType == ODT_MENU)//��������ǲ˵�
	{
		newMenu.MeasureItem(lpMeasureItemStruct);//����CNewMenu���MeasureItem��Ա����
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
	//������ͼ�л��Ĵ��뺯��
	if (nForm!=frmID)
	{
		frmID=nForm;//�����ε��ͬһ��ťʱ���ֵĴ���һ��ť���һ�κ󣬵ڶ��ε�������κδ���
		//CDocument* pDoc = GetActiveDocument();

		CView *pOldActiveView=GetActiveView();//�������ͼ
		CView *pNewActiveView=(CView*)GetDlgItem(nForm);//ȡ������ͼ
		if(pNewActiveView==NULL)
		{
			switch(nForm)
			{
			case IDD_AUTODEVICE_VIEW://��ͼ��Դ����
				pNewActiveView=(CView*)new CAutoDeviceView;
				break;
			case IDD_PROJECT_VIEW://��ͼ��Դ����
				pNewActiveView=(CView*)new CProjectView;
				break;
			case IDD_MEASURE_VIEW://��ͼ��Դ����
				pNewActiveView=(CView*)new CMeasureView;
				break;
			case IDD_BASELINECORRECT_VIEW://��ͼ��Դ����
				pNewActiveView=(CView*)new CBaselineCorrectView;
				break;
			}
			CCreateContext context; //*****���ĵ�����ͼ����
			context.m_pCurrentDoc=pOldActiveView->GetDocument();
			pNewActiveView->Create(NULL, NULL, WS_BORDER|WS_CHILD, CFrameWnd::rectDefault, this, nForm, &context);

			pNewActiveView->OnInitialUpdate();
		}

		//�ı�����ͼ
		SetActiveView(pNewActiveView); 
		pNewActiveView->ShowWindow(SW_SHOW); //��ʾ�µ���ͼ
		pOldActiveView->ShowWindow(SW_HIDE); //���ؾɵ���ͼ
		

		//*********���������е���ͼ����Ϊ�ɵļ�����ͼ���´���ͼ�л�ʱ��
		if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CAutoDeviceView))
			pOldActiveView->SetDlgCtrlID(IDD_AUTODEVICE_VIEW);
		else if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CProjectView))
			pOldActiveView->SetDlgCtrlID(IDD_PROJECT_VIEW);
		else if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CMeasureView))
			pOldActiveView->SetDlgCtrlID(IDD_MEASURE_VIEW);
		else if(pOldActiveView->GetRuntimeClass() ==RUNTIME_CLASS(CBaselineCorrectView))
			pOldActiveView->SetDlgCtrlID(IDD_BASELINECORRECT_VIEW);

		//��ͼ������ܴ��ڵ�һ��IDΪAFX_IDW_PANE_FIRST�����б߿���Ӵ��ڣ��������ܴ�������CFrameWnd���װ�������ġ�
		//��Ȼ����ͼ��Ϊ���Ӵ��ڣ�Ҳ����CFrameWnd�����ġ���Ա����CFrameWnd::OnCreateClient()���ڴ�����ͼ���ڣ����Ǹ����WM_CREATE��Ϣ�������б����õġ�
		pNewActiveView->SetDlgCtrlID(AFX_IDW_PANE_FIRST);

		delete pOldActiveView; //ɾ������ͼ
		RecalcLayout();//������ܴ���
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
// 		case IDD_AUTODEVICE_FORM://��ͼ��Դ����
// 			pNewViewClass=RUNTIME_CLASS(CAutoDeviceView);
// 			break;
// 		case IDD_PROJECTVIEW://��ͼ��Դ����
// 			pNewViewClass=RUNTIME_CLASS(CProjectView);
// 			break;
// 		case IDD_MEASUREVIEW://��ͼ��Դ����
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


