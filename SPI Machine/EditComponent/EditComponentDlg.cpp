// EditComponentDlg.cpp : implementation file
//
#include "stdafx.h"
#include "EditComponent.h"
#include "EditComponentDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEditComponentDlg dialog

CEditComponentDlg::CEditComponentDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditComponentDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	pStruct_Unite = NULL;
}

CEditComponentDlg::~CEditComponentDlg()
{
	if (m_nRectCount > 1)
	{
		if (m_pRect != NULL) 
		{
			delete[] m_pRect;
			m_pRect = NULL;
		}
	}
	else
	{
		if (m_pRect != NULL) 
		{
			delete m_pRect;
			m_pRect = NULL;
		}
	}
	
	if (pStruct_Unite != NULL)
	{
		delete pStruct_Unite;
		pStruct_Unite = NULL;
	}
	
}

void CEditComponentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEditComponentDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_MOLDINGPLATE_PARAMETER, &CEditComponentDlg::OnBnClickedButtonMoldingplateParameter)
	ON_COMMAND(ID_OPEN_DATABASE_FILE, &CEditComponentDlg::OnOpenDatabaseFile)
	ON_COMMAND(ID_IMPORT_PROJECT, &CEditComponentDlg::OnImportProject)
	ON_WM_COPYDATA()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_SCAN_FOV, &CEditComponentDlg::OnScanFov)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT BASED_CODE indicators[] = {IDS_STRING_WELDINGPLATE_INFO};
// CEditComponentDlg message handlers

BOOL CEditComponentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog 
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//添加菜单栏
	InitialMenuBar();
	//添加工具栏
//	InitialToolBar();
	//添加状态栏
	InitialStatusBar();
	//初始化PcbMapShow
	InitialPcbMapDC();
	//创建焊盘检测项目对话框对象
	//m_PadInspectItemDlg.Create(IDD_DIALOG_PADINSPECTITEM);

	m_nRectCount = 0;
	m_pRect = NULL;
//	m_pPcbMapDC->SetWindowOrg(249,71);
	m_IsDrawSelectRect = false;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

#define ID_BUTTON_CJ                  500

void CEditComponentDlg::InitialToolBar()
{
	RECT btnRect;(60,60,110,110);
	btnRect.left = 0;
	btnRect.top = 0;
	btnRect.right = 60;
	btnRect.bottom = 60;

	//CButton m_Button;
	//m_Button.Create(L"打开",WS_CHILD,btnRect,this,ID_BUTTON_CJ);
	//m_Button.ShowWindow(SW_SHOW);

	m_toolbar.CreateEx(this);                     //创建空白工具栏  

	//m_toolbar.LoadBitmapW(IDB_BITMAP1);           //加载图标
	m_toolbar.LoadToolBar(IDR_TOOLBAR1);          //加载工具栏资源  
	m_toolbar.LoadToolBar(IDR_TOOLBAR_OPEN_FILE);
	m_toolbar.LoadToolBar(IDR_TOOLBAR_SAVE_FILE);          //加载工具栏资源  
	m_toolbar.LoadToolBar(IDR_TOOLBAR_SCALEDOWN);
	m_toolbar.LoadToolBar(IDR_TOOLBAR_SCALEUP);
	// 	m_toolbar.ModifyStyle(0, BTNS_SHOWTEXT | TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_LIST);
	//  m_toolbar.SetButtonText(IDR_TOOLBAR1,L"打开");
	m_toolbar.SetSizes(CSize(60,60),CSize(50,50));

	CRect rect;   
	GetClientRect(&rect);                         //计算客户区大小  
	m_toolbar.MoveWindow(0,0,rect.Width(), 25);   //必须设置工具栏显示位置 
}
void CEditComponentDlg::InitialMenuBar()
{
	m_Menu.LoadMenu(IDR_MENU);
	this->SetMenu(&m_Menu);
}
void CEditComponentDlg::InitialStatusBar()
{
	m_statusbar.Create(this);
	m_statusbar.SetIndicators(indicators,1);
	CRect rect1;
	GetClientRect(&rect1);
	m_statusbar.SetPaneInfo(0, IDS_STRING_WELDINGPLATE_INFO, SBPS_NORMAL, rect1.Width() - 100);
	//m_statusbar.GetStatusBarCtrl().SetBkColor(RGB(10, 255, 10));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, AFX_IDW_CONTROLBAR_FIRST);
}

void CEditComponentDlg::InitialPcbMapDC()
{
	m_pPcbMapDC = GetDlgItem(IDC_STATIC_PCBMAPSHOW)->GetDC();

	//m_pPcbMapDC->SetViewportExt(249,71);
	//SetViewportOrgEx(m_pPcbMapDC->GetSafeHdc(),10,10,NULL);
	//m_pPcbMapDC->SetViewportOrg(-249,-71);

	//获取整个对话框的客户区
//	GetClientRect(&m_PcbMapShowRect);

	GetDlgItem(IDC_STATIC_PCBMAPSHOW)->GetClientRect(&m_PcbMapShowRect);

// 	GetDlgItem(IDC_STATIC_PCBMAPSHOW)->GetWindowRect(&m_PcbMapShowRect);
// 	ScreenToClient(&m_PcbMapShowRect);

	int width = m_PcbMapShowRect.Width();
	int height = m_PcbMapShowRect.Height();

//	m_pPcbMapDC->SetWindowOrg(249,72);

	m_PcbMapMemDC.CreateCompatibleDC(NULL);
	m_Bmp.CreateCompatibleBitmap(m_pPcbMapDC,m_PcbMapShowRect.Width(),m_PcbMapShowRect.Height());
	m_PcbMapMemDC.SelectObject(&m_Bmp);

	CPen pen(PS_SOLID,1,RGB(50,90,50));
	CBrush brush(RGB(50,90,50));
	m_PcbMapMemDC.SelectObject(&pen);
	m_PcbMapMemDC.SelectObject(&brush);
	m_PcbMapMemDC.Rectangle(m_PcbMapShowRect);

	m_pPcbMapDC->BitBlt(0,0,m_PcbMapShowRect.Width(),m_PcbMapShowRect.Height(),&m_PcbMapMemDC,0,0,SRCCOPY);
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEditComponentDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		m_pPcbMapDC->BitBlt(0,0,m_PcbMapShowRect.Width(),m_PcbMapShowRect.Height(),&m_PcbMapMemDC,0,0,SRCCOPY);

		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEditComponentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEditComponentDlg::OnBnClickedButtonMoldingplateParameter()
{
	// TODO: Add your control notification handler code here
	m_statusbar.SetPaneText(0, _T("我是谁：我是陈杰"));
	//焊盘检测项目对话框
	CPadInspectItemDlg m_PadInspectItemDlg;
	m_PadInspectItemDlg.DoModal();
	return;
}

void CEditComponentDlg::OnOpenDatabaseFile()
{
	// TODO: Add your command handler code here
	{
		m_dataBase.ConnectAdo(L"ARM-MDL-V1.3(WD658)-A.mdb");
		vector<CString> vecStrItems;
		m_dataBase.ReadItemsFromTable(CGxmcDatabase::s_padElement.PosXmm,CGxmcDatabase::tableList.Pad,vecStrItems);
 		CString Item;
 		try
 		{
 			m_dataBase.GetItemByKey(CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_padElement.ID,L"45",CGxmcDatabase::s_padElement.PosXmm,Item);
 			m_dataBase.GetItemByKey(CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_padElement.ID,L"10",CGxmcDatabase::s_padElement.PosXmm,Item);
 		}
 		catch (_com_error e)
 		{
 			AfxMessageBox(e.ErrorMessage());
 		}
 		m_dataBase.SetItemByKey(CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_padElement.ID,L"37",CGxmcDatabase::s_padElement.PosXmm,L"333.333");
 
		CGxmcDatabase::Pad pad;
		pad.ID = 100;
		pad.PosXmm = 1234.579;
		pad.PosYmm = 6234.568;

		if (!m_dataBase.FindRecordByKey(CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_padElement.ID,L"100"))
		{
			try
			{
				m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_padElement,pad);
			}
			catch (_com_error e)
			{
				AfxMessageBox(e.ErrorMessage());
			}
		}

		m_dataBase.DeleteRecordFromTable(CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_padElement.ID,L"35");

// 		vector<float> vecFloat;
// 		for (int i = 0; i< vecStrItems.size(); i++)
// 		{
// 			//vecFloat.push_back(atof((vecStrItems[i]).GetBuffer(vecStrItems[i].));
// 		}
	}
}


void CEditComponentDlg::OnImportProject()
{
	// TODO: Add your command handler code here

	WinExec("MyPrj",SW_SHOW);


	TCHAR szFilter[] = L"Access文件(*.mdb)|*.mdb|所有文件(*.*)|*.*||";
	CFileDialog fileDlg(true,L"mdb",L"*.mdb",0,szFilter,this);
	CString strFilePath;
	if (fileDlg.DoModal() == IDOK)
	{
		strFilePath = fileDlg.GetFolderPath() + "\\" + fileDlg.GetFileName();
	}
	else
	{
		return;
	}

	vector<CGxmcDatabase::TableStyle> vecTableStyle;
	vecTableStyle.push_back(CGxmcDatabase::enumPad);
	m_dataBase.CreateTable(strFilePath,CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_vecTableStyle);

	//WinExec("GerberX2.exe",WS_CHILD);
}

BOOL CEditComponentDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: Add your message handler code here and/or call default
	int flag = pCopyDataStruct->dwData;
	switch (flag)
	{
	case enumStruct_Unite:
		{
			pStruct_Unite = new STRUCT_UNITE();
			memcpy(pStruct_Unite,pCopyDataStruct->lpData,pCopyDataStruct->cbData); 
		}
		break;
	case enumPadLogicPos:
		{
			m_nRectCount = pCopyDataStruct->cbData/sizeof(RectF);
			m_pRect = new RectF[m_nRectCount]();
			memcpy(m_pRect,pCopyDataStruct->lpData,pCopyDataStruct->cbData); 
			CPen pen(PS_SOLID,1,RGB(0,255,0));
			m_PcbMapMemDC.SelectObject(&pen);
			m_PcbMapMemDC.SelectStockObject(NULL_BRUSH);

			//Pcb尺寸 / PcbMap
			if (NULL == pStruct_Unite || pStruct_Unite->gp.dPcbLen == 0 || pStruct_Unite->gp.dPcbWide == 0)
			{
				AfxMessageBox(L"Pcb板相关参数传输指针为空，或Pcb板尺寸中有0值！");
				break;
			}
			double fX = m_PcbMapShowRect.Width()/pStruct_Unite->gp.dPcbLen;
			double fY = m_PcbMapShowRect.Height()/pStruct_Unite->gp.dPcbWide;
			CRect rect;
			for (int i = 0; i < m_nRectCount; i++)
			{
				rect.left = /*m_PcbMapRect.Width() - */(m_pRect[i].left - pStruct_Unite->axis_par.dLX - pStruct_Unite->cp.dLen/2)*fX;
				rect.right = /*m_PcbMapRect.Width() - */(m_pRect[i].right - pStruct_Unite->axis_par.dLX - pStruct_Unite->cp.dLen/2)*fX;
				rect.top = /*m_PcbMapRect.Height() - */(m_pRect[i].top - pStruct_Unite->axis_par.dLY - pStruct_Unite->cp.dWide/2)*fY;
				rect.bottom = /*m_PcbMapRect.Height() - */(m_pRect[i].bottom - pStruct_Unite->axis_par.dLY - pStruct_Unite->cp.dWide/2)*fY;
				m_PcbMapMemDC.Rectangle(&rect);
			}
			m_pPcbMapDC->BitBlt(0,0,m_PcbMapShowRect.Width(),m_PcbMapShowRect.Height(),&m_PcbMapMemDC,0,0,SRCCOPY);
		}
		break;
	case enumProjectPathName:
		{
			int nLength =  pCopyDataStruct->cbData;
			char chTmp[256];
			memcpy(chTmp,pCopyDataStruct->lpData,nLength);
			int nCount = MultiByteToWideChar(CP_ACP,0,chTmp,-1,NULL,0);
			//wchar_t* wchTmp = new wchar_t[nCount];
			MultiByteToWideChar(CP_ACP,0,chTmp,-1,g_ProjectPathName.GetBuffer(nCount + 1),nCount + 1);
			g_ProjectPathName.ReleaseBuffer(nCount);
		}
		break;
	}

	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}

void LogicPointToViewPoint(CDC* pDC,float WExtX,float WExtY,float VExtX,float VExtY, CRect LogicRect, CRect ViewRect)
{
	int nVOX = 0;
	int nVOY = 0;
	int nWOX = 0;
	int nWOY = 0;

// 	int VExtX = rect.Width();
// 	int VExtY = rect.Height(); 
// 	int WExtX = 500;
// 	int WExtY = 500;
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetViewportOrg(nVOX,nVOY);
	pDC->SetViewportExt(VExtX,VExtY);

	pDC->SetWindowOrg(nWOX,nWOY);
	pDC->SetWindowExt(WExtX,WExtY);

	//视口坐标转换为窗口坐标
	LogicRect.left = (ViewRect.left - nVOX)*(WExtX/VExtX) + nWOX;
	LogicRect.right = (ViewRect.right - nVOX)*(WExtX/VExtX) + nWOX;

	LogicRect.top = (ViewRect.top - nVOY)*(WExtY/VExtY) + nWOY;
	LogicRect.bottom = (ViewRect.bottom - nVOY)*(WExtY/VExtY) + nWOY;
}

void CEditComponentDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_PcbMapShowRect.PtInRect(point))
	{
		m_bPoint = point;
		m_ePoint = point;

		m_IsDrawSelectRect = true;
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CEditComponentDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_IsDrawSelectRect)
	{
		m_pPcbMapDC->BitBlt(0,0,m_PcbMapShowRect.Width(),m_PcbMapShowRect.Height(),&m_PcbMapMemDC,0,0,SRCCOPY);
		CPen pen(PS_DOT,0.5,RGB(50,90,50));
		m_pPcbMapDC->SelectObject(&pen);
		m_pPcbMapDC->SelectStockObject(NULL_BRUSH);
		m_SelectRect = CRect(m_bPoint,m_ePoint);
		//m_PcbMapMemDC.Rectangle(&m_SelectRect);
		m_pPcbMapDC->Rectangle(&m_SelectRect);
		m_ePoint = point;
	}
	
	CDialogEx::OnMouseMove(nFlags, point);
}

void CEditComponentDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_IsDrawSelectRect = false;

	//选中在选择框中的焊盘
	CPen wpen(PS_DASH,1,RGB(255,255,255));
	m_pPcbMapDC->SelectObject(&wpen);
	m_pPcbMapDC->SelectStockObject(NULL_BRUSH);

	//Pcb尺寸 / PcbMap
	if (NULL != pStruct_Unite && pStruct_Unite->gp.dPcbLen != 0 && pStruct_Unite->gp.dPcbWide != 0)
	{
		float fX = m_PcbMapShowRect.Width()/pStruct_Unite->gp.dPcbLen;
		float fY = m_PcbMapShowRect.Height()/pStruct_Unite->gp.dPcbWide;

		CRect rect;
		for (int i = 0; i < m_nRectCount; i++)
		{
			rect.left = /*m_PcbMapRect.Width() - */(m_pRect[i].left - pStruct_Unite->axis_par.dLX - pStruct_Unite->cp.dLen/2)*fX;
			rect.right = /*m_PcbMapRect.Width() - */(m_pRect[i].right - pStruct_Unite->axis_par.dLX - pStruct_Unite->cp.dLen/2)*fX;
			rect.top = /*m_PcbMapRect.Height() - */(m_pRect[i].top - pStruct_Unite->axis_par.dLY - pStruct_Unite->cp.dWide/2)*fY;
			rect.bottom = /*m_PcbMapRect.Height() - */(m_pRect[i].bottom - pStruct_Unite->axis_par.dLY - pStruct_Unite->cp.dWide/2)*fY;
			if (m_SelectRect.PtInRect(rect.TopLeft()) && m_SelectRect.PtInRect(rect.BottomRight()))
			{
				m_pPcbMapDC->Rectangle(&rect);
				g_vecSelectPad.push_back(m_pRect[i]);
			}	
		}
	}
	
	CPen pen(PS_SOLID,1,RGB(50,90,50));
	m_pPcbMapDC->SelectObject(&pen);
	m_pPcbMapDC->Rectangle(&m_SelectRect);

	CDialogEx::OnLButtonUp(nFlags, point);
}

//视场扫描
void CEditComponentDlg::OnScanFov()
{
	// TODO: Add your command handler code here
	m_FovPathSelectDlg.DoModal();
}


void CEditComponentDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CDialogEx::OnClose();
}
