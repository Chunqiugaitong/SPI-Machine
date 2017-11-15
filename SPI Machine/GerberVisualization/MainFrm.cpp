
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "GerberVisualization.h"

#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()

	ON_WM_CLOSE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitialMenu();
	InitialToolBar();
	InitialStatusBar();

	//���ز˵���
	//SetMenu(NULL);  
	//���ùرհ�ť
	//CMenu *pSysMenu = GetSystemMenu(FALSE);
	//ASSERT(pSysMenu != NULL);
	//pSysMenu->EnableMenuItem(SC_CLOSE,MF_DISABLED);
	// ������󻯣���С�����رհ�ť    
	ModifyStyle(WS_SYSMENU, 0);
	// ���ر�����  
	//ModifyStyle(WS_CAPTION, 0);  
	// ȫ����ʾ  
	RECT rc;  
	::GetWindowRect(::GetDesktopWindow(), &rc);    
	this->MoveWindow(&rc);  
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	m_strTitle = L"Gerber���ӻ�Ӧ�ó���";
	return TRUE;
}
bool CMainFrame::InitialMenu()
{
	m_myMenu.LoadMenu(IDR_MENU1); 
	SetMenu(&m_myMenu);
	return true;
}
bool CMainFrame::InitialStatusBar()
{
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return false;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	//����״̬��λ��
	CRect rect;
	GetClientRect(&rect);
	//rect.bottom - 20��״̬���ĸ߶�λ��,20��״̬�ĸ߶�
	m_wndStatusBar.MoveWindow(0, rect.bottom - 40, rect.right,40);

	return true;
}

bool CMainFrame::InitialToolBar()
{
	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to create toolbar\n");
		return false;      // fail to create
	}

	m_toolBar.SetButtons(NULL,9);
	m_toolBar.SetButtonInfo(0,IDR_SCALE_FIT,TBSTYLE_BUTTON,0);
	m_toolBar.SetButtonText(0,_T("��Ӧ"));
	m_toolBar.SetButtonInfo(1,IDR_SCALE_UP,TBSTYLE_BUTTON,1);
	m_toolBar.SetButtonText(1,_T("�Ŵ�"));
	m_toolBar.SetButtonInfo(2,IDR_SCALE_DOWN,TBSTYLE_BUTTON,2);
	m_toolBar.SetButtonText(2,_T("��С"));
	m_toolBar.SetButtonInfo(3,IDR_MOVE,TBSTYLE_BUTTON,3);
	m_toolBar.SetButtonText(3,_T("�ƶ�"));
	m_toolBar.SetButtonInfo(4,IDR_HIDE,TBSTYLE_BUTTON,4);
	m_toolBar.SetButtonText(4,_T("������ѡԪ��"));
	m_toolBar.SetButtonInfo(5,IDR_SELECTITEMS,TBSTYLE_BUTTON,5);
	m_toolBar.SetButtonText(5,_T("���ط�ѡԪ��"));
	m_toolBar.SetButtonInfo(6,IDR_OPTIMUMPATH,TBSTYLE_BUTTON,6);
	m_toolBar.SetButtonText(6,_T("FOV·������"));
	m_toolBar.SetButtonInfo(7,IDR_SELECT_MARKPOINTS,TBSTYLE_BUTTON,7);
	m_toolBar.SetButtonText(7,_T("ѡ��Mark��"));
	m_toolBar.SetButtonInfo(8,IDR_TEST,TBSTYLE_BUTTON,7);
	m_toolBar.SetButtonText(8,_T("TEST"));
	
	CRect rectToolBar;
	//���ð�ť��С    
	m_toolBar.GetToolBarCtrl().SetButtonWidth(50,30);    
	//����ť����Դͼ�������    
	//m_toolBar.GetToolBarCtrl().SetImageList(&m_ImgList);    
	m_toolBar.SetSizes(CSize(50,25),CSize(1,1));    //��ť�ߴ磬ͼ��ߴ�
	//���������ʾ����    
	m_toolBar.EnableToolTips(TRUE);    
	//��ʾ�������������ݴ��ڴ�С�Զ�������������λ��   
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0); 
	EnableDocking(CBRS_ALIGN_TOP);

	m_toolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_toolBar);

	return true;
}

// CMainFrame diagnostics

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


// CMainFrame message handlers


void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	
	CFrameWnd::OnClose();
}
