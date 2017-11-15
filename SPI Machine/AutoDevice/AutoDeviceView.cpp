
// AutoDeviceView.cpp : CAutoDeviceView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "AutoDevice.h"
#endif

#include "AutoDeviceDoc.h"
#include "AutoDeviceView.h"
#include "MainFrm.h"
#include "include/Lala.h"
#include "include/DMC2410.h"
#include "AxisParDlg.h"
#include "AxisZeroDlg.h"
#include "CheckParDlg.h"
#include <io.h>
#include "AppConDlg.h"
#include "CameraParDlg.h"
//#include "C2DViewDlg.h"
#include "NewProjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD Tout, Ticks;
#define SETTIMEOUT(x)	Tout=x; Ticks=GetTickCount()
#define TIMEOUT1			((GetTickCount() - Ticks) > Tout)

// CAutoDeviceView

IMPLEMENT_DYNCREATE(CAutoDeviceView, CFormView)

BEGIN_MESSAGE_MAP(CAutoDeviceView, CFormView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_COMMAND(ID_CONNECT, &CAutoDeviceView::OnCardOpen)
	ON_COMMAND(ID_AXIS_PARA, &CAutoDeviceView::OnSetAxisPar)
	ON_COMMAND(ID_GENERAL, &CAutoDeviceView::OnSetCheckPar)
	ON_COMMAND(ID_AXIS_ZERO, &CAutoDeviceView::OnControlAxisZero)
	ON_COMMAND(ID_AXIS_MOVE, &CAutoDeviceView::OnControlAxisMove)
	ON_COMMAND(ID_APPCON, &CAutoDeviceView::OnSetAppCon)
	ON_COMMAND(ID_CAMERAPAR, &CAutoDeviceView::OnSetCameraPar)
	ON_COMMAND(ID_OPENCAMERA, &CAutoDeviceView::OnCameraOpen)
	ON_COMMAND(ID_GRAB, &CAutoDeviceView::OnCameraGrab)
	ON_COMMAND(ID_EXEC, &CAutoDeviceView::OnCameraExec)
	ON_COMMAND(ID_SAVEIMAGE, &CAutoDeviceView::OnSaveImage)
	ON_COMMAND(ID_LOADIMAGE, &CAutoDeviceView::OnLoadImage)
	ON_COMMAND(ID_RETICLE, &CAutoDeviceView::OnIsReticle)
	ON_COMMAND(ID_2DVIEW, &CAutoDeviceView::On2DView)
	ON_COMMAND(ID_VIEW_STATUS_BAR, &CAutoDeviceView::OnStsBarMsg)
	ON_COMMAND(ID_MANUAL_PRO, &CAutoDeviceView::OnManualPro)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_INTO, &CAutoDeviceView::OnBnClickedBtnInto)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTOTYPE, &CAutoDeviceView::OnCbnSelchangeComboAutotype)
	ON_BN_CLICKED(IDC_BTN_AUTOCHECK, &CAutoDeviceView::OnBnClickedBtnAutocheck)
	ON_BN_CLICKED(IDC_BTN_SUSPEND, &CAutoDeviceView::OnBnClickedBtnSuspend)
	ON_BN_CLICKED(IDC_BTN_RESET, &CAutoDeviceView::OnBnClickedBtnReset)
	ON_MESSAGE(WM_UPDATE_INFO, &CAutoDeviceView::OnUpdateInfo)
	ON_COMMAND(ID_AUTO_PRO, &CAutoDeviceView::OnAutoPro)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_EXIT_SYSTEM, &CAutoDeviceView::OnExitSystem)
END_MESSAGE_MAP()

// CAutoDeviceView 构造/析构

CAutoDeviceView::CAutoDeviceView()
	: CFormView(CAutoDeviceView::IDD)
	//, bInit(FALSE)
	, bAutoFlag(TRUE)
	, strXPer(_T("0"))
	, strYPer(_T("0"))
	, strZPer(_T("0"))
	, strWPer(_T("0"))
	, strPath(_T(""))
	, bConnect(FALSE)
	, bZero(FALSE)
	, bCheck(FALSE)
	, m_bIsFileOpen(FALSE)
{
	hAutoDeviceThread = NULL;
	hAutoEnterThread = NULL;
	m_AmDlg = 0;

//  	_CrtSetBreakAlloc(536);
//  	_CrtSetBreakAlloc(591);

}

CAutoDeviceView::~CAutoDeviceView()
{
	if (m_AmDlg) delete m_AmDlg;
}

void CAutoDeviceView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_REAL_IMAGE, m_StrReal_Image);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_StrImage);
	DDX_Control(pDX, IDC_COMBO_AUTOTYPE, m_strCombAutoType);
}

#pragma region 窗口初始化

BOOL CAutoDeviceView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);
}
//视图界面初始化，类似对话框OnInitialDlg
void CAutoDeviceView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	SetFont();
	InitLabel();
	LoadData();
	//LoadDefFile();
	StopMoveAxis();

	m_bIsAutoDeviceViewActive = true;
}
void CAutoDeviceView::InitLabel()
{
	m_StrLabelName.SubclassDlgItem(IDC_STATIC_BOARD_NAME, this);
	m_StrLabelName.SetFontName(_T("Microsoft Sans Serif"));
	m_StrLabelName.SetFontSize(18);
	m_StrLabelName.SetFontBold(true);
	m_StrLabelName.ModifyStyle(0, SS_CENTERIMAGE);
	//m_StrLabelName.SetTextColor(RGB(255, 0, 0));
	m_StrLabelName.SetBkColor(RGB(0, 150, 0));

	m_StrLabelDirect.SubclassDlgItem(IDC_STATIC_BOARD_DIRECT, this);
	m_StrLabelDirect.SetFontName(_T("Microsoft Sans Serif"));
	m_StrLabelDirect.SetFontSize(18);
	m_StrLabelDirect.SetFontBold(true);
	m_StrLabelDirect.ModifyStyle(0, SS_CENTERIMAGE);
	//m_StrLabelDirect.SetTextColor(RGB(255, 0, 0));
	m_StrLabelDirect.SetBkColor(RGB(0, 150, 0));

	m_StrLabelSize.SubclassDlgItem(IDC_STATIC_BOARD_SIZE, this);
	m_StrLabelSize.SetFontName(_T("Microsoft Sans Serif"));
	m_StrLabelSize.SetFontSize(18);
	m_StrLabelSize.SetFontBold(true);
	m_StrLabelSize.ModifyStyle(0, SS_CENTERIMAGE);
	//m_StrLabelSize.SetTextColor(RGB(255, 0, 0));
	m_StrLabelSize.SetBkColor(RGB(0, 150, 0));

	m_StrLabelSts.SubclassDlgItem(IDC_STATIC_BOARD_STATUS, this);
	m_StrLabelSts.SetFontName(_T("Microsoft Sans Serif"));
	m_StrLabelSts.SetFontSize(40);
	m_StrLabelSts.SetFontBold(true);
	m_StrLabelSts.ModifyStyle(0, SS_CENTERIMAGE);
	//m_StrLabelSts.SetTextColor(RGB(255, 0, 0));
	m_StrLabelSts.SetBkColor(RGB(67, 63, 61));

	m_StrLabelOkNg.SubclassDlgItem(IDC_STATIC_BOARD_OKNG, this);
	m_StrLabelOkNg.SetFontName(_T("Microsoft Sans Serif"));
	m_StrLabelOkNg.SetFontSize(60);
	m_StrLabelOkNg.SetFontBold(true);
	m_StrLabelOkNg.ModifyStyle(0, SS_CENTERIMAGE);
	//m_StrLabelOkNg.SetTextColor(RGB(255, 0, 0));
	m_StrLabelOkNg.SetBkColor(RGB(0, 150, 0));
}
void CAutoDeviceView::SetFont()
{
	LOGFONT    lf;
	lf.lfWidth          = 0;
	lf.lfEscapement     = 0;
	lf.lfOrientation    = 0;
	lf.lfHeight         = -26;          // 14 point
	lf.lfWeight         = FW_BOLD;      // FW_NORMAL;    // 
	lf.lfItalic         = FALSE;
	lf.lfUnderline      = FALSE;
	lf.lfStrikeOut      = 0;
	lf.lfCharSet        = ANSI_CHARSET;
	lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lf.lfQuality        = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	lstrcpy(lf.lfFaceName, TEXT("Tahoma"));
	m_font.CreateFontIndirect(&lf);

	((CButton*)GetDlgItem(IDC_BTN_INTO))->SetFont(&m_font);
	((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetFont(&m_font);
	((CButton*)GetDlgItem(IDC_BTN_SUSPEND))->SetFont(&m_font);
	((CButton*)GetDlgItem(IDC_BTN_RESET))->SetFont(&m_font);

	m_strCombAutoType.AddString(_T("单机自动测试"));
	m_strCombAutoType.AddString(_T("单机手动测试"));
	m_strCombAutoType.AddString(_T("脱机自动检测"));
	m_strCombAutoType.AddString(_T("联机自动检测"));
	m_strCombAutoType.AddString(_T("直通自动传送"));
	m_strCombAutoType.AddString(_T("规则制定采集"));
	m_strCombAutoType.SetCurSel(0);
	m_strCombAutoType.SetFont(&m_font);
}
short CAutoDeviceView::LoadData()
{
	TCHAR   szSettingFile[256] = _T("");
	TCHAR   szParse[64] = _T("");
	CHAR    szPath[256] = "";
	CHAR    szTemp[256] = "";
	CString strTemp = _T("");
	CString strSettingFile = _T("");
	strSettingFile=GetFilePath();
	_tcscpy_s(szSettingFile, strSettingFile.GetString());
	if (m_bEnglish)
	{
		_tcscat_s(szSettingFile, TXT_FILENAME);
	}
	else
	{
	}
	wcharTochar(szSettingFile, szPath, sizeof(szPath));
	FILE *pf=NULL;
	pf = fopen(szPath,"rt");
	if (NULL==pf)
	{
		return 1;
	}
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.wXAxis = (WORD)_ttoi(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dXPulsePer = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dXPitch = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dXTrip = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dXTripNegaTive = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dXMinVel = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dXMaxVel = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dXTAcc = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dXTDec = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dXTs = _wtof(strTemp);

	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.wYAxis = (WORD)_ttoi(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dYPulsePer = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dYPitch = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dYTrip = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dYTripNegaTive = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dYMinVel = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dYMaxVel = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dYTAcc = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dYTDec = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dYTs = _wtof(strTemp);

	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.wZAxis = (WORD)_ttoi(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dZPulsePer = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dZPitch = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dZTrip = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dZTripNegaTive = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dZMinVel = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dZMaxVel = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dZTAcc = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dZTDec = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dZTs = _wtof(strTemp);

	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.wWAxis = (WORD)_ttoi(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dWPulsePer = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dWPitch = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dWTrip = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dWMinTrip = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dWMinVel = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dWMaxVel = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dWTAcc = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dWTDec = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dWTs = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.wDirect = (WORD)_ttoi(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dLX = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dLY = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dRX = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.axis_par.dRY = _wtof(strTemp);

	ReadFileValue(pf, strTemp);
	Struct_Unite.as.dXSafe = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.as.dYSafe = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.as.dZSafe = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.as.dZHigh = _wtof(strTemp);

	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.UiLenPixel = _ttoi(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.UiWidePixel = _ttoi(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.dCcdSizeL = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.dCcdSizeW = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.dShotLen = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.dWorkDistance = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.dLen = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.dWide = _wtof(strTemp);
	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.dEpsTime = _wtof(strTemp);
	SetExposure(Struct_Unite.cp.dEpsTime);

	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.dGain = _wtof(strTemp);
	SetGain(Struct_Unite.cp.dGain);

	ReadFileValue(pf, strTemp);
	Struct_Unite.cp.dRatio = _wtof(strTemp);

	strTemp.Format(_T("进板方向:%s"), Struct_Unite.axis_par.wDirect == 0?(_T("左侧进板")):(_T("右侧进板")));
	m_StrLabelDirect.SetText(strTemp);

	fclose(pf);
	return 0;
}
void CAutoDeviceView::LoadDefFile()//程序启动后加载默认的最后使用的文件
{
	short sRet = 0;
	CString strPath = _T("");
	CString strTemp = _T("");
	TCHAR szTemp[512] = _T("");
	strPath = GetFilePath();
	strTemp = strPath;
	strTemp += _T("生产文件\\");
	strPath += INI_FILENAME;
	::GetPrivateProfileString(_T("FileManage"), _T("CurrentFileName"), _T(""), szTemp, sizeof(szTemp), strPath);
	strTemp += (CString)szTemp;
	if (!LoadConData(strTemp))
	{
		//m_StrLabelName.SetText(_T("电路板名称:") + Struct_Unite.gp.strName);
		//strPath.Format(_T("%s%.03f * %.03f"), _T("电路板尺寸:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
		//m_StrLabelSize.SetText(strPath);
		if (1 == Struct_Unite.gp.wIsGerber)
		{
			strTemp = GetFilePath() + _T("Gerber") + '\\' + Struct_Unite.gp.strGerberName.Trim();
	//		OpenGerber(strTemp);
		}
	}
}
#pragma endregion 

#pragma region CAutoDeviceView 诊断
// CAutoDeviceView 诊断
#ifdef _DEBUG
void CAutoDeviceView::AssertValid() const
{
	CFormView::AssertValid();
}

void CAutoDeviceView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CAutoDeviceDoc* CAutoDeviceView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAutoDeviceDoc)));
	return (CAutoDeviceDoc*)m_pDocument;
}
#endif //_DEBUG
#pragma endregion

// CAutoDeviceView 消息处理程序

void CAutoDeviceView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	//if (!bInit)
	//{
	//	GetClientRect(&m_rect);
	//	bInit = TRUE;
	//}
	
	CLala lala;
	lala.Show(m_rect, (CDialog*)this, cx, cy);
	Invalidate();
}

 BOOL CAutoDeviceView::OnEraseBkgnd(CDC* pDC)
 {
 	// TODO: Add your message handler code here and/or call default

 	/* CRect rect;
 	 GetClientRect(&rect);
 	 pDC->FillRect(&rect,&CBrush(RGB(80,160,80)));*/

	if (m_bIsAutoDeviceViewActive)
	{
		return CFormView::OnEraseBkgnd(pDC);
	}

	if (GetDocument()->m_bIsImported)
	{
		//不重绘背景，OnPaint会在旧的背景上继续绘制
		return true;
	}
	
 }
void CAutoDeviceView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
  	CAutoDeviceDoc* pAutoDeviceDoc = GetDocument();
	
  	if (pAutoDeviceDoc->m_bIsImported && m_bIsAutoDeviceViewActive)
  	{
		m_bIsAutoDeviceViewActive = false;
		m_bShowFov = true;
		m_bShovNavigation = true;

		m_nClusterIndex = 0;
		m_StrImage.GetClientRect(&m_navigateMargin);//获取控件坐标
		m_StrReal_Image.GetClientRect(&m_fovMargin);//获取控件坐标

		InitialDraw();
		DrawGerber();
 	}
	return;
}

//退出系统
void CAutoDeviceView::OnExitSystem()
{
	// TODO: Add your command handler code here
	return;

	WORD wRet = 0;
	WORD wML = 0, wMR = 0;
	CString strTemp = _T("");
	CString strErr = _T("");
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
		if (bConnect)
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
	}
	m_gxmcGerber.ReleasePadCluster();

	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CLOSE,0,0);  
}
void CAutoDeviceView::OnSetCheckPar()
{
	CString strTemp = _T("");
	CString strPath = _T(""), strIniPath = _T("");
	TCHAR szIniPath[512] = _T("");
	CCheckParDlg Dlg;
	strPath = GetFilePath();
	strIniPath = strPath;
	strIniPath += INI_FILENAME;
	strPath +=_T("生产文件");
	CFileDialog FileDlg(TRUE,_T("dat"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
		_T("dat Files(*.dat)|*.dat|All Files|*.*||"), this);
	FileDlg.m_ofn.lpstrTitle = _T("Open File");
	FileDlg.m_ofn.lpstrInitialDir = strPath;

//	OpenGerber(Dlg.m_strFilePath);
	return;

	if (IDOK==FileDlg.DoModal())
	{
		if (!LoadConData(FileDlg.GetPathName()))
		{
			Dlg.m_strName = Struct_Unite.gp.strName;
			Dlg.m_dLen = Struct_Unite.gp.dPcbLen;
			Dlg.m_dWide = Struct_Unite.gp.dPcbWide;
			Dlg.m_dHigh = Struct_Unite.gp.dPly;
			Dlg.m_dMarkX1 = Struct_Unite.gp.dMarkX1;
			Dlg.m_dMarkY1 = Struct_Unite.gp.dMarkY1;
			Dlg.m_dMarkX2 = Struct_Unite.gp.dMarkX2;
			Dlg.m_dMarkY2 = Struct_Unite.gp.dMarkY2;
			Dlg.wIsGerber = Struct_Unite.gp.wIsGerber;
			Dlg.strGerberName = Struct_Unite.gp.strGerberName;
			if (0 == Dlg.wIsGerber)
			{
				m_bIsFileOpen = FALSE;
				CRect rc;
				m_StrImage.GetClientRect(&rc);
				m_StrImage.ClientToScreen(&rc);
				InvalidateRect(rc);
			}

			//m_StrLabelName.SetText(_T("电路板名称:") + Struct_Unite.gp.strName);
			//strTemp.Format(_T("%s%.03f * %.03f"), _T("电路板尺寸:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
			//m_StrLabelSize.SetText(strTemp);
			_tcscpy_s(szIniPath, strIniPath.GetString());
			::WritePrivateProfileString(_T("FileManage"), _T("CurrentFileName"), FileDlg.GetFileName(), szIniPath); //记录到ini文件中
		}
	}
	else
	{
		CRect rc;
		m_StrImage.GetClientRect(&rc);
		m_StrImage.ClientToScreen(&rc);
		m_StrLabelName.SetText(_T(""));
		m_StrLabelSize.SetText(_T(""));
		m_bIsFileOpen = FALSE;
		InvalidateRect(rc);
	}
	if (IDOK == Dlg.DoModal())
	{
		//m_StrLabelName.SetText(_T("电路板名称:") + Struct_Unite.gp.strName);
		//strTemp.Format(_T("%s%.03f * %.03f"), _T("电路板尺寸:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
		//m_StrLabelSize.SetText(strTemp);
		//CString str = FileDlg.GetFileName();

		//_tcscpy_s(szIniPath, strIniPath.GetString());
		//::WritePrivateProfileString(_T("FileManage"), _T("CurrentFileName"), FileDlg.GetFileName(), szIniPath); //记录到ini文件中
		WriteConData();

//		OpenGerber(Dlg.m_strFilePath);

	}
}

//是否禁用系统菜单
void CAutoDeviceView::MenuEnable(BOOL bEnable)
{
	CMenu *pMenu;
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMenu = pFrame->GetMenu();
	if (bEnable)
	{
		for (int i=0; i<MENUNUM; i++)
		{
			pMenu->EnableMenuItem(i, MF_BYPOSITION|MF_ENABLED);
		}
	}
	else
	{
		for (int i=0; i<MENUNUM; i++)
		{
			pMenu->EnableMenuItem(i, MF_BYPOSITION|MF_DISABLED);
		}
	}
	//AfxGetMainWnd()->DrawMenuBar();//刷新菜单栏显示状态
	pFrame->DrawMenuBar();
}
//控件使能
void CAutoDeviceView::EnableControl(BOOL bActive)
{
	switch (m_strCombAutoType.GetCurSel())
	{
	case 0:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("单机自动测试"):_T("单机自动测试状态"));
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("单机手动测试"):_T("单机手动测试状态"));
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("脱机自动检测"):_T("脱机自动检测状态"));
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("联机自动检测"):_T("联机自动检测状态"));
		break;
	case 4:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("直通自动传送"):_T("直通自动传送状态"));
		break;
	case 5:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("规则制定采集"):_T("规则制定采集状态"));
		break;
	}
	((CButton*)GetDlgItem(IDC_BTN_INTO))->EnableWindow(bActive);
	((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->EnableWindow(bActive);
	((CButton*)GetDlgItem(IDC_BTN_SUSPEND))->EnableWindow(!bActive);
	((CButton*)GetDlgItem(IDC_BTN_RESET))->EnableWindow(!bActive);
	m_strCombAutoType.EnableWindow(bActive);
}
//进板操作时按钮控制
void CAutoDeviceView::IntoEnable(BOOL bActive)
{
	((CButton*)GetDlgItem(IDC_BTN_INTO))->EnableWindow(bActive);
	((CButton*)GetDlgItem(IDC_BTN_INTO))->SetWindowText(TRUE == bActive?_T("进板"):_T("等待进板"));
	((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->EnableWindow(bActive);
	m_strCombAutoType.EnableWindow(bActive);
}
//选择是否显示十字划线
void CAutoDeviceView::IsShowReticle(BOOL bCheck)
{
	CMenu *pSubMenu;
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pSubMenu = pFrame->GetMenu()->GetSubMenu(3);
	if (bCheck)
	{
		pSubMenu->CheckMenuItem(ID_RETICLE, MF_BYCOMMAND|MF_CHECKED);
	}
	else
	{
		pSubMenu->CheckMenuItem(ID_RETICLE, MF_BYCOMMAND|MF_UNCHECKED);
	}
	//pFrame->m_wndStatusBar.SetPaneText(1, _T("模式: 工作"));
	CRect rc;
	m_StrReal_Image.GetClientRect(&rc);
	m_StrReal_Image.ClientToScreen(&rc);
	InvalidateRect(rc); 
}

//*************************************************************************************************************************************/
//*************************************************************************************************************************************/
void CAutoDeviceView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	//m_gxmcGerber.GetPadCluster()
	CRect ImageRect;
	GetDlgItem(IDC_STATIC_IMAGE)->GetClientRect(ImageRect);
	ClientToScreen(ImageRect);
	if (ImageRect.PtInRect(point))
	{
       point = CPoint(0,0);
	}

	CFormView::OnMouseMove(nFlags, point);
}
void CAutoDeviceView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	///////////////////////////////////////////////////////////////
	////////////              需要同步操作
// 	if (m_bIsFovInspected)
// 	{
// 		return;
// 	}

	CRect ImageRect;
	//控件尺寸在屏幕坐标系下的坐标
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(ImageRect);
	//控件尺寸 从屏幕坐标转换到客户区坐标
	ScreenToClient(ImageRect);
	
	m_nClusterIndex = 0;
	if (ImageRect.PtInRect(point))
	{
		for (unsigned int i = 0; i < m_vecClusterRect.size(); i++)
		{
			if(m_vecClusterRect[i].PtInRect(point))
			{
				m_nClusterIndex = i;
				///////////////////////////////////
				//电机移动到padCluster中心位置	

// 				bool wRet = false;
// 				CString strErr = L"";
// 				//准确度需要调试
// 				double dPosX = padCluster.centerPoint.x;
// 				double dPosY = padCluster.centerPoint.y;
// 
// 				wRet = Axis_Move(Struct_Unite.axis_par.wXAxis, Struct_Unite.axis_par.dXMinVel, Struct_Unite.axis_par.dXMaxVel,
// 					Struct_Unite.axis_par.dXTAcc, Struct_Unite.axis_par.dXTDec,
// 					Struct_Unite.axis_par.dXTs, 0, dPosX, 1, 0);
// 				if (wRet)
// 				{
// 					Err_Info(wRet, strErr);
// 					MessageBox(strErr, ALERT_ERR, ALERT_ICON);
// 					return;
// 				}
// 				wRet = Axis_Move(Struct_Unite.axis_par.wYAxis, Struct_Unite.axis_par.dYMinVel, Struct_Unite.axis_par.dYMaxVel,
// 					Struct_Unite.axis_par.dYTAcc, Struct_Unite.axis_par.dYTDec,
// 					Struct_Unite.axis_par.dYTs, 0, dPosY, 1, 0);
// 				if (wRet)
// 				{
// 					Err_Info(wRet, strErr);
// 					MessageBox(strErr, ALERT_ERR, ALERT_ICON);
// 					return;
// 				}
// 				///////////////////////////////////
// 				//相机拍照
// 				wRet = ExecuteGrab();
// 				Sleep(200);
// 				if (wRet)
// 				{
// 					Err_Info(wRet, strErr);
// 					MessageBox(strErr, ALERT_ERR, ALERT_ICON);
// 					return;
// 				}
				InitialFovScreen();
				DrawGerber();
			}
		}
	}

	CFormView::OnLButtonDown(nFlags, point);
}
//解析gerber并画图
void CAutoDeviceView::InitialDraw()
{
 	m_StrImage.GetClientRect(&m_navigateMargin);//获取控件坐标
	InitialNavigateScreen();
	InitialAllFovMargin();
	InitialFovScreen();
}

void CAutoDeviceView::InitialNavigateScreen()
{
	CAutoDeviceDoc* pAutoDeviceDoc = GetDocument();

	m_bIsFileOpen = TRUE;
	//Struct_Unite.gp.wIsGerber = 1;
	m_navigateScreen.realMarginRect = pAutoDeviceDoc->m_clustersMargin;
	//为边框加外边缘
	int nDelt = 10;
	m_navigateScreen.realMarginRect.left -= nDelt;
	m_navigateScreen.realMarginRect.right += nDelt;
	m_navigateScreen.realMarginRect.top += nDelt;
	m_navigateScreen.realMarginRect.bottom -= nDelt;

	double fxNav = (m_navigateMargin.Width())/m_navigateScreen.realMarginRect.width();
	double fyNav = (m_navigateMargin.Height())/m_navigateScreen.realMarginRect.height();
	m_navigateScreen.factor = fxNav < fyNav ? fxNav : fyNav;
	m_navigateScreen.startX = m_navigateScreen.realMarginRect.left;
	m_navigateScreen.startY = m_navigateScreen.realMarginRect.bottom;
	m_navigateScreen.offsetX = 0;
	m_navigateScreen.offsetY = 0;

	m_bShovNavigation = true;

}
void CAutoDeviceView::InitialFovScreen()
{
	m_StrImage.GetClientRect(&m_navigateMargin);//获取控件坐标

	CAutoDeviceDoc* pAutoDeviceDoc = GetDocument();
// 	//为边框加外边缘
 	int nDelt = 10;
	//********************************Fov***********************************************************/
	Gxmc_Gerber::PadCluster padCluster = pAutoDeviceDoc->m_mapPadCluster[m_nClusterIndex];
	m_fovScreen.realMarginRect = RectD(padCluster.clusterMargin->m_startPoint.x,
									padCluster.clusterMargin->m_stopPoint.y,
									padCluster.clusterMargin->m_stopPoint.x,
									padCluster.clusterMargin->m_startPoint.y);
	//为边框加外边缘
	m_fovScreen.realMarginRect.left -= nDelt/4;
	m_fovScreen.realMarginRect.right += nDelt/4;
	m_fovScreen.realMarginRect.top += nDelt/4;
	m_fovScreen.realMarginRect.bottom -= nDelt/4;

	m_StrReal_Image.GetClientRect(&m_fovMargin);//获取控件坐标
	double fxFov = (m_fovMargin.Width())/m_fovScreen.realMarginRect.width();
	double fyFov = (m_fovMargin.Height())/m_fovScreen.realMarginRect.height();
	m_fovScreen.factor = fxFov < fyFov ? fxFov : fyFov;
	m_fovScreen.startX = m_fovScreen.realMarginRect.left;
	m_fovScreen.startY = m_fovScreen.realMarginRect.bottom;
	m_fovScreen.offsetX = 0;
	m_fovScreen.offsetY = 0;

	m_bShowFov = true;
}
//绘制视野焊盘前的初始化工作
void CAutoDeviceView::InitialAllFovMargin()
{
	CRect ImageRect;
	//控件尺寸在屏幕坐标系下的坐标
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(ImageRect);
	//控件尺寸 从屏幕坐标转换到客户区坐标
	ScreenToClient(ImageRect);

	//清空几个容器
	if (!m_vecClusterRect.empty())
	{
		vector<CRect> vecClusterRect;
		m_vecClusterRect.swap(vecClusterRect);
	}

	if (m_vecClusterRect.empty())
	{
		GxmcCircle circle;
		circle.m_Screen = m_navigateScreen;
		CAutoDeviceDoc* pAutoDeviceDoc = GetDocument();
		for (unsigned int i = 0; i < pAutoDeviceDoc->m_mapPadCluster.size(); i++)
		{
			Gxmc_Gerber::PadCluster padCluster = pAutoDeviceDoc->m_mapPadCluster[i];
			/*************************************************************************************************************************/
			/**/int left,top,right,bottom;
			/**///得到逻辑坐标的相对位置
			/**/circle.WorldViewToLogicView(padCluster.clusterMargin->m_startPoint.x,padCluster.clusterMargin->m_startPoint.y,left,bottom);
			/**/circle.WorldViewToLogicView(padCluster.clusterMargin->m_stopPoint.x,padCluster.clusterMargin->m_stopPoint.y,right,top);
			/**///以图像控件左上角在屏幕坐标系下的坐标为起点（基准）
			/**/CRect clusterRect = CRect(left,top,right,bottom);
			/**/clusterRect.left += ImageRect.left;
			/**/clusterRect.top += ImageRect.top;
			/**/clusterRect.right += ImageRect.left;
			/**/clusterRect.bottom += ImageRect.top;
			m_vecClusterRect.push_back(clusterRect);
		}
	}
}
//绘制gerber数据库文件导入的焊盘
void CAutoDeviceView::DrawGerber()
{
	CAutoDeviceDoc* pAutoDeviceDoc = GetDocument();
	////////////////////////////Navigation/////////////////////////////
	if (m_bShovNavigation)
	{
		m_bShovNavigation = false;	
		m_pNavigateDC = m_StrImage.GetDC();
		CDC m_NavigateMemDC;
		CBitmap m_NavigateMemBmp;
		m_NavigateMemDC.CreateCompatibleDC(NULL);		
		m_NavigateMemBmp.CreateCompatibleBitmap(m_pNavigateDC, m_navigateMargin.Width(), m_navigateMargin.Height());
		m_NavigateMemDC.SelectObject(&m_NavigateMemBmp);
		CPen pen(PS_SOLID, 1, RGB(0,255,0));
		m_NavigateMemDC.SelectObject(&pen);
		pAutoDeviceDoc->m_gxmcGerber.DisplayMarkPoint(m_NavigateMemDC,m_navigateScreen,pAutoDeviceDoc->m_vecMark);
		pAutoDeviceDoc->m_gxmcGerber.DisplayPadCluster(m_NavigateMemDC,m_navigateScreen,pAutoDeviceDoc->m_mapPadCluster);
		m_pNavigateDC->BitBlt(0, 0, m_navigateMargin.Width(), m_navigateMargin.Height(), &m_NavigateMemDC, 0, 0, SRCCOPY); 
		//释放相关资源
		m_NavigateMemDC.DeleteDC();
		ReleaseDC(m_pNavigateDC);
	}	
	////////////////////////////Fov/////////////////////////////
	if (m_bShowFov)
	{
		m_bShowFov = false;
		m_pFovDC = m_StrReal_Image.GetDC();
		CDC m_FovMemDC;
		CBitmap m_FovMemBmp;
		//把图像加附到CBitmap
		//m_FovMemBmp.Attach(bmp1);

		m_FovMemDC.CreateCompatibleDC(NULL);		
		m_FovMemBmp.CreateCompatibleBitmap(m_pFovDC, m_fovMargin.Width(), m_fovMargin.Height());
		m_FovMemDC.SelectObject(&m_FovMemBmp);
		CPen pen(PS_SOLID, 1, RGB(0,255,0));
		m_FovMemDC.SelectObject(&pen);
		//画Fov图像

		//画焊盘
		pAutoDeviceDoc->m_gxmcGerber.DisplayPadCluster(m_FovMemDC,m_fovScreen,pAutoDeviceDoc->m_mapPadCluster[m_nClusterIndex]);
		m_pFovDC->BitBlt(0,0,m_fovMargin.Width(),m_fovMargin.Height(),&m_FovMemDC, 0, 0, SRCCOPY); 

		//画十字线
// 		m_pFovDC->SelectObject(&pen);
// 		m_pFovDC->MoveTo(m_fovMargin.Width()/2, 0);
// 		m_pFovDC->LineTo(m_fovMargin.Width()/2, m_fovMargin.Height());
// 		m_pFovDC->MoveTo(0, m_fovMargin.Height()/2);
// 		m_pFovDC->LineTo(m_fovMargin.Width(), m_fovMargin.Height()/2);

		//释放相关资源
		//DeleteObject(bmp1);
		m_FovMemDC.DeleteDC();
		ReleaseDC(m_pFovDC);
	}
}
//***************************************************************************************************************************************/

void CAutoDeviceView::OnSetAppCon()
{
	CAppConDlg Dlg;
	if (!LoadData())
	{
		Dlg.m_dXSafe = Struct_Unite.as.dXSafe;
		Dlg.m_dYSafe = Struct_Unite.as.dYSafe;
		Dlg.m_dZSafe = Struct_Unite.as.dZSafe;
		Dlg.m_dZHigh = Struct_Unite.as.dZHigh;
	}
	if (IDOK == Dlg.DoModal())
	{
		WriteData();
	}
}
void CAutoDeviceView::OnIsReticle()
{
	bCheck = !bCheck;
	IsShowReticle(bCheck);
}
void CAutoDeviceView::WriteData()
{
	TCHAR   szSettingFile[250] = _T("");
	CHAR    szPath[250]="";
	CHAR    szTemp[250]="";
	CString strSettingFile = _T("");
	CString strTemp = _T("");
	string  StrData = "";
	if (m_bEnglish)
	{
		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X轴轴号:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wXAxis);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X轴每转脉冲:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXPulsePer);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X轴螺距:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXPitch);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X轴行程:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X负向轴行程:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTripNegaTive);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X轴起始速度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXMinVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X轴最大速度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXMaxVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X轴加速时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTAcc);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X轴减速时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTDec);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X轴S段时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTs);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y轴轴号:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wYAxis);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y轴每转脉冲:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYPulsePer);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y轴螺距:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYPitch);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y轴行程:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y负向轴行程:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTripNegaTive);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y轴起始速度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYMinVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y轴最大速度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYMaxVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y轴加速时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTAcc);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y轴减速时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTDec);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y轴S段时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTs);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴轴号:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wZAxis);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴每转脉冲:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZPulsePer);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴螺距:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZPitch);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴行程:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z负向轴行程:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTripNegaTive);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);


		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴起始速度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZMinVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴最大速度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZMaxVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴加速时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTAcc);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴减速时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTDec);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴S段时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTs);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴轴号:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wWAxis);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴每转脉冲:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWPulsePer);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴螺距:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWPitch);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴原点宽度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴最小宽度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWMinTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴起始速度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWMinVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴最大速度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWMaxVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴加速时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWTAcc);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴减速时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWTDec);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W轴S段时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWTs);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="进板方向:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wDirect);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		strTemp.Format(_T("进板方向:%s"), Struct_Unite.axis_par.wDirect == 0?(_T("左侧进板")):(_T("右侧进板")));
		m_StrLabelDirect.SetText(strTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="左进板X:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dLX);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="左进板Y:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dLY);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="右进板X:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dRX);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="右进板Y:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dRY);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="安全位置X:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.as.dXSafe);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);


		memset(szTemp, 0, sizeof(szTemp));
		StrData +="安全位置Y:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.as.dYSafe);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="安全位置Z:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.as.dZSafe);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z轴工作高度:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.as.dZHigh);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="相机分辨率长:";
		sprintf_s(szTemp, "%d", Struct_Unite.cp.UiLenPixel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="相机分辨率宽:";
		sprintf_s(szTemp, "%d", Struct_Unite.cp.UiWidePixel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="相机感光芯片尺寸长:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dCcdSizeL);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="相机感光芯片尺寸宽:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dCcdSizeW);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="相机镜头焦距:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dShotLen);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="镜头到拍摄物的距离:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dWorkDistance);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="计算后的图像物理长:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dLen);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="计算后的图像物理宽:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dWide);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="相机曝光时间:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dEpsTime);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);
		SetExposure(Struct_Unite.cp.dEpsTime);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="相机增益:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dGain);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);
		SetGain(Struct_Unite.cp.dGain);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="远心镜头放大倍率:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dRatio);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		strSettingFile=GetFilePath();
		_tcscpy_s(szSettingFile, strSettingFile.GetString());
		_tcscat_s(szSettingFile, TXT_FILENAME);
	}
	else
	{

	}
	wcharTochar(szSettingFile, szPath, sizeof(szPath));
	FILE *pf = NULL;
	pf = fopen(szPath,"wt");
	fwrite(StrData.c_str(), StrData.length(), 1, pf);
	fclose(pf);

	//if (m_bEnglish)
	//{
	//	MessageBox(_T("数据保存成功！"), ALERT_INFOR, INFO_ICON);
	//}
	//else
	//{
	//	MessageBox(_T("Data is successfully saved!"), ALERT_INFOR, INFO_ICON);
	//}
	return;
}
void CAutoDeviceView::WriteConData()
{
	TCHAR   szSettingFile[512] = _T("");
	CHAR    szPath[512] ="";
	CHAR    szTemp[512] ="";
	CHAR    szTmp[512] = "";
	TCHAR    szIniTemp[512] = _T("");
	CString strSettingFile = _T("");
	CString strTemp = _T("");
	string  StrData = "";
	if (m_bEnglish)
	{
		memset(szTemp, 0, sizeof(szTemp));
		wcharTochar(Struct_Unite.gp.strName, szTemp, Struct_Unite.gp.strName.GetLength()*2);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf_s(szTemp, "%.03f", Struct_Unite.gp.dPcbLen);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf_s(szTemp, "%.03f", Struct_Unite.gp.dPcbWide);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		m_StrLabelName.SetText(_T("电路板名称:") + Struct_Unite.gp.strName);
		strTemp.Format(_T("%s%.03f * %.03f"), _T("电路板尺寸:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
		m_StrLabelSize.SetText(strTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf_s(szTemp, "%.03f", Struct_Unite.gp.dPly);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf_s(szTemp, "%.03f", Struct_Unite.gp.dMarkX1);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf_s(szTemp, "%.03f", Struct_Unite.gp.dMarkY1);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf_s(szTemp, "%.03f", Struct_Unite.gp.dMarkX2);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf_s(szTemp, "%.03f", Struct_Unite.gp.dMarkY2);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		sprintf_s(szTemp, "%d", Struct_Unite.gp.wIsGerber);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		wcharTochar(Struct_Unite.gp.strGerberName, szTemp, Struct_Unite.gp.strGerberName.GetLength()*2);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		strSettingFile = GetFilePath();
		strTemp = strSettingFile;
		strTemp += INI_FILENAME;
		strSettingFile +=_T("生产文件");
		strSettingFile +='\\';
		wcharTochar(strSettingFile, szPath, 512);
		if ( _access(szPath,00) == -1)
		{
			if (ERROR_SUCCESS != SHCreateDirectoryEx(NULL, strSettingFile, NULL))
			{
				//MessageBox(_T("创建保存图片目录的文件夹失败"), ALERT_ERR, ALERT_ICON);
				return;
			}
		}
		strSettingFile += Struct_Unite.gp.strName;
		strSettingFile += _T(".dat");
	}
	else
	{

	}
	wcharTochar(strSettingFile, szPath, sizeof(szPath));
	FILE *pf=NULL;
	pf = fopen(szPath,"wt");
	fwrite(StrData.c_str(), StrData.length(), 1, pf);
	fclose(pf);
	_tcscpy_s(szIniTemp, strTemp.GetString());
	::WritePrivateProfileString(_T("FileManage"), _T("CurrentFileName"), Struct_Unite.gp.strName + _T(".dat"), szIniTemp); //记录到ini文件中
	//if (m_bEnglish)
	//{
	//	MessageBox(_T("数据保存成功！"), ALERT_INFOR, INFO_ICON);
	//}
	//else
	//{
	//	MessageBox(_T("Data is successfully saved!"), ALERT_INFOR, INFO_ICON);
	//}
	return;
}
short CAutoDeviceView::LoadConData(CString strPathName)
{
	CHAR    szPath[512] = "";
	CString strTemp = _T("");
	wcharTochar(strPathName, szPath, sizeof(szPath));
	FILE *pf=NULL;
	pf = fopen(szPath,"rt");
	if (NULL==pf)
	{
		return 1;
	}
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.strName = strTemp;
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.dPcbLen = _wtof(strTemp);
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.dPcbWide = _wtof(strTemp);
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.dPly = _wtof(strTemp);
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.dMarkX1 = _wtof(strTemp);
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.dMarkY1 = _wtof(strTemp);
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.dMarkX2 = _wtof(strTemp);
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.dMarkY2 = _wtof(strTemp);
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.wIsGerber = _wtoi(strTemp);
	ReadConFileValue(pf, strTemp);
	Struct_Unite.gp.strGerberName = strTemp;

	m_StrLabelName.SetText(_T("电路板名称:") + Struct_Unite.gp.strName);
	strTemp.Format(_T("%s%.03f * %.03f"), _T("电路板尺寸:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
	m_StrLabelSize.SetText(strTemp);

	fclose(pf);
	return 0;
}


#pragma region 设备运行流程控制

void CAutoDeviceView::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent)
	{
		GetRealTime();
	}
	else if (2 == nIDEvent)
	{
		ResMacBtn();
	}

	CFormView::OnTimer(nIDEvent);
}
void CAutoDeviceView::GetRealTime()
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	long lx = 0, ly = 0, lz =0, lw = 0;
	CString strXTemp = _T("");
	CString strYTemp = _T("");
	CString strZTemp = _T("");
	CString strWTemp = _T("");
	CString strTemp = _T("");
	lx = d2410_get_position(Struct_Unite.axis_par.wXAxis);
	ly = d2410_get_position(Struct_Unite.axis_par.wYAxis);
	lz = d2410_get_position(Struct_Unite.axis_par.wZAxis);
	lw = d2410_get_position(Struct_Unite.axis_par.wWAxis);
	strXTemp.Format(_T("%.03f"), lx*(Struct_Unite.axis_par.dXPitch/Struct_Unite.axis_par.dXPulsePer));
	strYTemp.Format(_T("%.03f"), ly*(Struct_Unite.axis_par.dYPitch/Struct_Unite.axis_par.dYPulsePer));
	strZTemp.Format(_T("%.03f"), lz*(Struct_Unite.axis_par.dZPitch/Struct_Unite.axis_par.dZPulsePer));
	strWTemp.Format(_T("%.03f"), Struct_Unite.axis_par.dWTrip - lw*(Struct_Unite.axis_par.dWPitch/Struct_Unite.axis_par.dWPulsePer));
	Struct_Unite.Real_Coor.dx = _wtof(strXTemp);
	Struct_Unite.Real_Coor.dy = _wtof(strYTemp);
	Struct_Unite.Real_Coor.dz = _wtof(strZTemp);
	Struct_Unite.Real_Coor.dw = _wtof(strWTemp);
	strTemp = _T("X:") + strXTemp + _T("  ") + _T("Y:") + strYTemp + _T("  ")
		+ _T("Z:") + strZTemp + _T("  ") + _T("W:") + strWTemp;
	if (strXPer != strXTemp)
	{
		strXPer = strXTemp;
		pFrame->m_wndStatusBar.SetPaneText(1, strTemp);
	}
	if (strYPer != strYTemp)
	{
		strYPer = strYTemp;
		pFrame->m_wndStatusBar.SetPaneText(1, strTemp);
	}
	if (strZPer != strZTemp)
	{
		strZPer = strZTemp;
		pFrame->m_wndStatusBar.SetPaneText(1, strTemp);
	}
	if (strWPer != strWTemp)
	{
		strWPer = strWTemp;
		pFrame->m_wndStatusBar.SetPaneText(1, strTemp);
	}
	if (!GetOneBitIn(IN7) || !GetOneBitIn(IN8) || !GetOneBitIn(IN9) || !GetOneBitIn(IN10))
	{
		m_StrLabelSts.SetText(_T("有电路板"));
	}
	else
	{
		m_StrLabelSts.SetText(_T("无电路板"));
	}
}
//机器按钮响应
void CAutoDeviceView::ResMacBtn()
{
	if (!GetOneBitIn(IN14))//启动
	{
		OnBnClickedBtnAutocheck();
	}
	else if (!GetOneBitIn(IN14))//复位
	{
		OnBnClickedBtnReset();
	}
	else if (!GetOneBitIn(IN14))//暂停
	{
		OnBnClickedBtnSuspend();
	}
	else if (!GetOneBitIn(IN14))//急停
	{
		bAutoFlag = FALSE;
	}
}
//启动、停止、复位
void CAutoDeviceView::OnBnClickedBtnAutocheck()
{
	WORD wRet = 0;
	CString strErr = _T("");
	UserPostMessage(_T(""), 0);
	//if (!GetOneBitIn(IN3))
	//{
	//	Err_Info(DOOROPEN, strErr);
	//	MessageBox(strErr, ALERT_ERR, ALERT_ICON);
	//	return;
	//}
	wRet = MoveSafeCoor();//移动到安全位置
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	if (4 != m_strCombAutoType.GetCurSel())//直通自动传输不需要气压
	{
		//if (!GetOneBitIn(IN11))
		//{
		//	MessageBox(_T("未检测到气压,请确认是否已经正确连接并且已经打开！"), ALERT_ERR, ALERT_ICON);
		//	return;
		//}
		//先全部复位挡板气缸，夹板气缸
		wRet = SetAndGetIO(SLEEPTIME, LEFTIN4, LEFTOUT6, HIGHLEVEL);
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		wRet = SetAndGetIO(SLEEPTIME, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		wRet = SetAndGetIO(SLEEPTIME, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
	}
	if (5 == m_strCombAutoType.GetCurSel())
	{
		CHAR szPath[512];
		char szTemp[512];
		CString strTemp = _T("");
		memset(szPath, 0, sizeof(szPath));
		memset(szTemp, 0, sizeof(szTemp));
		strPath = GetFilePath();
		strPath +=_T("采集图像");
		strPath +='\\';
		//strTemp.Format(_T("%s\\*.*"), strPath);
		wcharTochar(strPath, szPath, strPath.GetLength()*2);
		//sprintf_s(szTemp, "del   /s/q  %s", szPath);
		//system(szTemp); 
		DeleteDirectory(strPath);
		if ( _access(szPath,00) == -1)
		{
			if (ERROR_SUCCESS != SHCreateDirectoryEx(NULL,strPath, NULL))
			{
				MessageBox(_T("创建保存图片目录的文件夹失败"), ALERT_ERR, ALERT_ICON);
				return;
			}
		}
		if (Struct_Unite.gp.dPcbLen == 0 || Struct_Unite.gp.dPcbWide == 0)
		{
			MessageBox(_T("采集拍照前必须正确输入待拍照工件的长和宽度值"), ALERT_ERR, ALERT_ICON);
			return;
		}
		//SetGrabMode(0);//软件触发
	}
	hAutoDeviceThread = CreateThread(NULL, 0, AutoDeviceThread, this, 0, NULL);
	if (hAutoDeviceThread == NULL)
	{
		CloseHandle(hAutoDeviceThread);
		hAutoDeviceThread = NULL;
		if (m_bEnglish)
		{
			MessageBox(_T("自动运行启动失败!"), ALERT_ERR, ALERT_ICON);
		}
		else
		{
			MessageBox(_T("Automatic startup failed!"), ALERT_ERR, ALERT_ICON);
		}
		return;
	}
	else
	{
		bAutoFlag = TRUE;
	}
	EnableControl(FALSE);
}
void CAutoDeviceView::OnBnClickedBtnSuspend()
{
	bAutoFlag = FALSE;
}
void CAutoDeviceView::OnBnClickedBtnReset()
{
	// TODO: Add your control notification handler code here
}
#pragma endregion 
//运行模式选择
void CAutoDeviceView::OnCbnSelchangeComboAutotype()
{
	return;
	switch (m_strCombAutoType.GetCurSel())
	{
	case 0:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("单机自动测试"));
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("单机手动测试"));
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("脱机自动检测"));
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("联机自动检测"));
		break;
	case 4:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("直通自动传送"));
		break;
	case 5:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("规则制定采集"));
		break;
	}
}
//运动控制卡初始化
void CAutoDeviceView::OnCardOpen()
{
	CString strErr = _T("");
	CString strTemp = _T("");
	WORD wRet = 0;
	CMenu *pMenu;
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMenu = pFrame->GetMenu()->GetSubMenu(0);
	pMenu->GetMenuString(ID_CONNECT, strTemp, 1);
	if (0 == strTemp.Compare(_T("连接板卡")))
	{
		wRet = d2410_board_init();
		if (wRet <= 0)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		wRet = EnableLmt();
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_CONNECT, _T("断开板卡"));
		pFrame->DrawMenuBar();
		bConnect = TRUE;
		SetTimer(1, 100, NULL);
		//SetTimer(2, 60, NULL);
	}
	else
	{
		KillTimer(1);
		//KillTimer(2);
		wRet = d2410_board_close();
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_CONNECT, _T("连接板卡"));
		pFrame->DrawMenuBar();
		bConnect = FALSE;
	}
}
//轴运动参数设置
void CAutoDeviceView::OnSetAxisPar()
{
	CAxisParDlg Dlg;
	if (!LoadData())
	{
		Dlg.m_wXAxis = Struct_Unite.axis_par.wXAxis;
		Dlg.m_wYAxis = Struct_Unite.axis_par.wYAxis;
		Dlg.m_wZAxis = Struct_Unite.axis_par.wZAxis;
		Dlg.m_wWAxis = Struct_Unite.axis_par.wWAxis; 
		Dlg.m_dXPulsePer = Struct_Unite.axis_par.dXPulsePer;
		Dlg.m_dXPitch = Struct_Unite.axis_par.dXPitch;
		Dlg.m_dXTrip = Struct_Unite.axis_par.dXTrip;
		Dlg.m_dXTripNegaTive = Struct_Unite.axis_par.dXTripNegaTive;
		Dlg.m_dXMinVel = Struct_Unite.axis_par.dXMinVel;
		Dlg.m_dXMaxVel = Struct_Unite.axis_par.dXMaxVel;
		Dlg.m_dXTAcc = Struct_Unite.axis_par.dXTAcc;
		Dlg.m_dXTDec = Struct_Unite.axis_par.dXTDec;
		Dlg.m_dXTs = Struct_Unite.axis_par.dXTs;

		Dlg.m_dYPulsePer = Struct_Unite.axis_par.dYPulsePer;
		Dlg.m_dYPitch = Struct_Unite.axis_par.dYPitch;
		Dlg.m_dYTrip = Struct_Unite.axis_par.dYTrip;
		Dlg.m_dYTripNegaTive = Struct_Unite.axis_par.dYTripNegaTive;
		Dlg.m_dYMinVel = Struct_Unite.axis_par.dYMinVel;
		Dlg.m_dYMaxVel = Struct_Unite.axis_par.dYMaxVel;
		Dlg.m_dYTAcc = Struct_Unite.axis_par.dYTAcc;
		Dlg.m_dYTDec = Struct_Unite.axis_par.dYTDec;
		Dlg.m_dYTs = Struct_Unite.axis_par.dYTs;

		Dlg.m_dZPulsePer = Struct_Unite.axis_par.dZPulsePer; 
		Dlg.m_dZPitch = Struct_Unite.axis_par.dZPitch; 
		Dlg.m_dZTrip = Struct_Unite.axis_par.dZTrip;
		Dlg.m_dZTripNegaTive = Struct_Unite.axis_par.dZTripNegaTive;
		Dlg.m_dZMinVel = Struct_Unite.axis_par.dZMinVel;
		Dlg.m_dZMaxVel = Struct_Unite.axis_par.dZMaxVel;
		Dlg.m_dZTAcc = Struct_Unite.axis_par.dZTAcc;
		Dlg.m_dZTDec = Struct_Unite.axis_par.dZTDec;
		Dlg.m_dZTs = Struct_Unite.axis_par.dZTs;

		Dlg.m_dWPulsePer = Struct_Unite.axis_par.dWPulsePer;
		Dlg.m_dWPitch = Struct_Unite.axis_par.dWPitch;
		Dlg.m_dWTrip = Struct_Unite.axis_par.dWTrip;
		Dlg.m_dWMinTrip = Struct_Unite.axis_par.dWMinTrip;
		Dlg.m_dWMinVel = Struct_Unite.axis_par.dWMinVel;
		Dlg.m_dWMaxVel = Struct_Unite.axis_par.dWMaxVel;
		Dlg.m_dWTAcc = Struct_Unite.axis_par.dWTAcc;
		Dlg.m_dWTDec = Struct_Unite.axis_par.dWTDec;
		Dlg.m_dWTs = Struct_Unite.axis_par.dWTs;
		Dlg.wDirect = Struct_Unite.axis_par.wDirect;
		Dlg.m_dLx = Struct_Unite.axis_par.dLX;
		Dlg.m_dLy = Struct_Unite.axis_par.dLY;
		Dlg.m_dRx = Struct_Unite.axis_par.dRX;
		Dlg.m_dRy = Struct_Unite.axis_par.dRY;
	}
	if (IDOK == Dlg.DoModal())
	{
		WriteData();
	}
}
CPoint g_curPoint;
CString g_strFileName;
void CAutoDeviceView::OnManualPro()
{
	TCHAR szFilter[] = L"Access文件(*.mdb)|*.mdb|所有文件(*.*)|*.*||";
	CFileDialog fileDlg(true,L"mdb",NULL,0,szFilter,this);
	//CString strFilePath;
	if (fileDlg.DoModal() == IDOK)
	{
		g_strFileName = fileDlg.GetFolderPath()+ "\\" + fileDlg.GetFileName();
	}
	else
	{
		return;
	}
/*
	WORD wRet = 0;
	CString strTemp = _T(""), strErr = _T("");
	int ixCount = 0, iyCount = 0, ixNum = 0, iyNum = 0;
	double dOffX = 22, dOffY = 22, dx = 0, dy = 0, dStartX = 0, dStartY = 0, dZ = 0;
	dOffX = Struct_Unite.cp.dLen;
	dOffY = Struct_Unite.cp.dWide;

	//先移动到板子原点位置先拍照一次
	ixNum = Struct_Unite.gp.dPcbLen/dOffX + 1;
	iyNum = Struct_Unite.gp.dPcbWide/dOffY + 1;
	dZ = Struct_Unite.as.dZHigh;
	if (0 == Struct_Unite.axis_par.wDirect)
	{
		dStartX = Struct_Unite.axis_par.dLX + Struct_Unite.cp.dLen/2;
		dStartY = Struct_Unite.axis_par.dLY + Struct_Unite.cp.dWide/2;
	}
	else if (1 == Struct_Unite.axis_par.wDirect)
	{
		dStartX = Struct_Unite.axis_par.dRX - Struct_Unite.gp.dPcbLen  - Struct_Unite.cp.dLen/2;
		dStartY = Struct_Unite.axis_par.dRY + Struct_Unite.cp.dWide/2;
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
		Struct_Unite.axis_par.dZTs, 0, Struct_Unite.as.dZHigh, 1, 0);
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
	WaitTime(200);//cj
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	*/
    CNewProjectDlg Dlg;
	g_curPoint = CPoint(0,0);
	//Dlg.nPcbWidth = Struct_Unite.gp.dPcbLen;
	//Dlg.nPcbHeight = Struct_Unite.gp.dPcbWide;
	Dlg.DoModal();
}
void CAutoDeviceView::OnAutoPro()
{
	// TODO: Add your command handler code here
	WinExec("../Debug/EditComponent.exe",SW_SHOW);
}


#pragma region 运动控制卡IO操作
/*
获取某一位输入口电平
* wBit 输入口位号
* 返回对应位的电平值 低电平有效
*/
WORD CAutoDeviceView::GetOneBitIn(WORD wBit)
{
	int iBitValue = 0;
	//if (!bAutoFlag) return AUTOEXIT;
	iBitValue = d2410_read_inbit(0, wBit);
	return (WORD)iBitValue;
	//if (!bAutoFlag) return AUTOEXIT;
}

/*
获取某一位输出口电平
* wBit 输出口位号
* 返回对应位号的电平值 低电平有效
*/
WORD CAutoDeviceView::GetOneBitOut(WORD wBit)
{
	int iBitValue = 0;
	//if (!bAutoFlag) return AUTOEXIT;
	iBitValue = d2410_read_outbit(0, wBit);
	return (WORD)iBitValue;
	//if (!bAutoFlag) return AUTOEXIT;
}

/*
设置某一位输出口电平
* wBit 输出口位号
* wBitValue 输出对应位号的电平值 0=低电平,1=高电平  低电平有效
*/
WORD CAutoDeviceView::SetOneBitOut(WORD wBit, WORD wBitValue)
{
	WORD wRet = 0;
	//if (!bAutoFlag) return AUTOEXIT;
	wRet = d2410_write_outbit(0, wBit, wBitValue);
	if (wRet) return wRet;
	return wRet;
	//if (!bAutoFlag) return AUTOEXIT;
}

/*检测气缸的上升下降超时与否 0为OK
*iTimeout       超时时间
*iBitIn         输入位号
*iBitOut        输出位号
*iBitOutValue   输出位值 0=上升, 1=下降  低电平有效
*/
WORD CAutoDeviceView::SetAndGetIO(int iTimeout, WORD iBitIn, WORD iBitOut, WORD iBitOutValue)
{
	int wRes = 0, wRet = 0;
	if (!bAutoFlag) return AUTOEXIT;
	wRet = SetOneBitOut(iBitOut, iBitOutValue);
	if (wRet) return wRet;
	if (!iBitOutValue)
	{
		SETTIMEOUT(iTimeout);
		do 
		{
			if (!bAutoFlag) return AUTOEXIT;
			wRes = GetOneBitIn(iBitIn);
			if (wRes) break;
			if (!bAutoFlag) return AUTOEXIT;
			DoEvents();
		}while(!TIMEOUT1);
		if (TIMEOUT1) 
		{
			if (iBitIn == LEFTIN4)
			{
				return LEFTUPTOUT;
			}
			else if (iBitIn == RIGHTIN5)
			{
				return RIGHTUPTOUT;
			}
			else if (iBitIn == MIDDLEIN6)
			{
				return MIDDLEUPTOUT;
			}
		}
	}
	else
	{
		wRet = WaitTime(iTimeout/10);
		if (wRet) return wRet;
	}
	return wRet;
}

//延时 返回是否退出延时
WORD CAutoDeviceView::WaitTime(int iMilliSecond)
{
	SETTIMEOUT(iMilliSecond);
	do 
	{
		if (!bAutoFlag) return AUTOEXIT;
		DoEvents();
	} while (!TIMEOUT1);
	return 0;
}

/*三色灯控制
*wColour 0=红色 1=黄色 2 =绿色
*wBitValue 控制灯开关 1=打开 0=关闭
*返回函数执行的错误代码
*/
WORD CAutoDeviceView::LightControl(WORD wColour, WORD wBitValue)
{
	WORD wRet = 0;
	WORD wBit = 0;
	switch (wColour)
	{
	case 0:
		wBit = OUT1;
		break;
	case 1:
		wBit = OUT2;
		break;
	case 2:
		wBit = OUT3;
		break;
	}
	wRet = SetOneBitOut(wBit, wBitValue);
	if (wRet) return wRet;
	return wRet;
}

/*循环等待符合条件的输入并且设置对应的输出
* wInNo  输入号
* wOutNo 输出号
* wOutValue 输出值
* 返回函数执行的结果
*/
WORD CAutoDeviceView::WaitAndControl(WORD wInNo, WORD wOutNo, WORD wOutValue)
{
	WORD wRet = 0;
	do 
	{
		DoEvents();
		if (!bAutoFlag)
		{
			return AUTOEXIT;
		}
	} while (GetOneBitIn(wInNo));
	if (!GetOneBitIn(wInNo))
	{
		wRet = SetOneBitOut(wOutNo, wOutValue);
		if (wRet)
		{
			return wRet;
		}
	}
	return wRet;
}

/*循环等待符合条件的输入并且设置对应的输出与WaitAndControl函数输入判断相反
* wInNo  输入号
* wOutNo 输出号
* wOutValue 输出值
* 返回函数执行的结果
*/
WORD CAutoDeviceView::NotWaitAndControl(WORD wInNo, WORD wOutNo, WORD wOutValue)
{
	WORD wRet = 0;
	do 
	{
		//if (GetOneBitIn(IN8))
		//{
		//	wRet = SetOneBitOut(OUT9, HIGHLEVEL);//出板位置没有板停止传送
		//	if (wRet)
		//	{
		//		return AUTOEXIT;
		//	}
		//}
		DoEvents();
		if (!bAutoFlag)
		{
			return AUTOEXIT;
		}
	} while (!GetOneBitIn(IN8));
	if (GetOneBitIn(IN8))
	{
		wRet = SetOneBitOut(OUT9, HIGHLEVEL);
		if (wRet)
		{
			return AUTOEXIT;
		}
	}
	return wRet;
}

/*循环等待挡板气缸是否检测到检测到延时然后停止传输
* wInNo  输入号
* wOutNo 输出号
* wOutValue 输出值
* 返回函数执行的结果
*/
WORD CAutoDeviceView::WaitAndControlSleep(WORD wInNo, WORD wOutNo, WORD wOutValue)
{
	WORD wRet = 0;
	do 
	{
		if (!bAutoFlag)
		{
			return AUTOEXIT;
		}
		DoEvents();
	} while (GetOneBitIn(wInNo));
	if (!GetOneBitIn(wInNo))
	{
		wRet = WaitTime(SLEEPTIME);
		if (wRet)
		{
			return wRet;
		}
		wRet = SetOneBitOut(wOutNo, wOutValue);
		if (wRet)
		{
			return wRet;
		}
	}
	return wRet;
}

/*
*wInNo  输入号 检查是否有输入
* 返回函数执行的结果
*/
WORD CAutoDeviceView::WaitLoop(WORD wInNo)
{
	WORD wRet = 0;
	do 
	{
		if (!bAutoFlag)
		{
			return AUTOEXIT;
		}
		DoEvents();
	} while (GetOneBitIn(wInNo));
	return wRet;
}

/*
*wInNo  输入号 检查是否没有输入
* 返回函数执行的结果
*/
WORD CAutoDeviceView::NotWaitLoop(WORD wInNo)
{
	WORD wRet = 0;
	do 
	{
		if (!bAutoFlag)
		{
			return AUTOEXIT;
		}
		DoEvents();
	} while (!GetOneBitIn(wInNo));
	return wRet;
}
#pragma endregion 
void CAutoDeviceView::OnControlAxisZero()
{
	CAxisZeroDlg Dlg;
	Dlg.DoModal();
}
void CAutoDeviceView::OnControlAxisMove()
{
	if (0 != m_AmDlg) delete m_AmDlg;
	m_AmDlg = new CAxisMoveDlg;
	m_AmDlg->Create(IDD_AXISMOVE_DLG, this);
	m_AmDlg->ShowWindow(SW_SHOW);
}


#pragma region 相机控制
void CAutoDeviceView::OnCameraOpen()
{
	CString strErr = _T("");
	CString strTemp = _T("");
	WORD wRet = 0;
	CMenu *pMenu;
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMenu = pFrame->GetMenu()->GetSubMenu(1);
	pMenu->GetMenuString(ID_OPENCAMERA, strTemp, 1);
	if (0 == strTemp.Compare(_T("打开相机")))
	{
		wRet = OpenCamera();
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		ImageReverseXY();
		SetExposure(Struct_Unite.cp.dEpsTime);
		SetGain(Struct_Unite.cp.dGain);
		SetGrabMode(2);
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_OPENCAMERA, _T("关闭相机"));
		pFrame->DrawMenuBar();
	}
	else
	{
		wRet = CloseCamera();
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_OPENCAMERA, _T("打开相机"));
		pFrame->DrawMenuBar();
	}
}
void CAutoDeviceView::OnCameraGrab()
{
	CString strErr = _T("");
	CString strTemp = _T("");
	WORD wRet = 0;
	CMenu *pMenu;
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMenu = pFrame->GetMenu()->GetSubMenu(1);
	pMenu->GetMenuString(ID_GRAB, strTemp, 1);

	if (0 == strTemp.Compare(_T("启动采集")))
	{
		::DeleteObject(bmp1);
		sType = 2;
		SetGrabMode(2);
		wRet = StartGrab();
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		pMenu->ModifyMenu(4, MF_BYPOSITION, ID_GRAB, _T("停止采集"));
		pFrame->DrawMenuBar();
	}
	else
	{
		sType = 0;
		wRet = StopGrab();
		if (wRet)
		{
			Err_Info(wRet, strErr);
			MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			return;
		}
		pMenu->ModifyMenu(4, MF_BYPOSITION, ID_GRAB, _T("启动采集"));
		pFrame->DrawMenuBar();
	}
}
void CAutoDeviceView::OnCameraExec()
{
	CString strTemp = _T("");
	CString strErr = _T("");
	WORD wRet = 0;
	sType = 1;
	::DeleteObject(bmp1);//释放资源
	wRet = ExecuteGrab();
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
}
void CAutoDeviceView::OnSaveImage()
{
	CString strPath = _T("");
	CString strTemp = _T("");
	TCHAR   szSettingFile[512] = _T("");
	strPath = GetFilePath();
	//strTemp = strPath;
	//strPath +=_T("生产文件");
	//strPath +='\\';
	CFileDialog FileDlg(FALSE,_T("bmp"),NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,_T("bmp Files(*.bmp)|*.bmp|All Files|*.*||"),this);
	FileDlg.m_ofn.lpstrTitle = _T("Save As File");
	//FileDlg.m_ofn.lpstrInitialDir = strPath;
	if (IDOK==FileDlg.DoModal())
	{
		if (!SaveNewImage(bmp1, FileDlg.GetPathName(), 8))
		{
			MessageBox(_T("保存失败"), ALERT_ERR, ALERT_ICON);
			return;
		}
	}
}
void CAutoDeviceView::OnLoadImage()
{
	CString strPath = _T("");
	CString strTemp = _T("");
	TCHAR   szSettingFile[512] = _T("");
	CRect rc;
	strPath = GetFilePath();
	//strTemp = strPath;
	//strPath +=_T("生产文件");
	//strPath +='\\';
	CFileDialog FileDlg(TRUE,_T("bmp"),NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,_T("bmp Files(*.bmp)|*.bmp|All Files|*.*||"),this);
	FileDlg.m_ofn.lpstrTitle = _T("Open File");
	//FileDlg.m_ofn.lpstrInitialDir = strPath;
	if (IDOK==FileDlg.DoModal())
	{
		::DeleteObject(bmp1);//释放资源
		LoadImageX(FileDlg.GetPathName());
		m_StrReal_Image.GetClientRect(&rc);
		m_StrReal_Image.ClientToScreen(&rc);
		InvalidateRect(rc);
	}
}
#pragma endregion 
void CAutoDeviceView::OnSetCameraPar()
{
	CCameraParDlg Dlg;
	if (!LoadData())
	{
		Dlg.m_UICameraLen = Struct_Unite.cp.UiLenPixel;
		Dlg.m_UICameraWide = Struct_Unite.cp.UiWidePixel; 
		Dlg.m_dCcdLen = Struct_Unite.cp.dCcdSizeL;
		Dlg.m_dCcdWide = Struct_Unite.cp.dCcdSizeW;
		Dlg.m_dShotLen = Struct_Unite.cp.dShotLen;
		Dlg.m_dWorkDistance = Struct_Unite.cp.dWorkDistance;
		Dlg.m_dEpsTime = Struct_Unite.cp.dEpsTime;
		Dlg.m_dGain = Struct_Unite.cp.dGain;
		Dlg.m_dRatio = Struct_Unite.cp.dRatio;
	}
	if (IDOK == Dlg.DoModal())
	{
		WriteData();
	}
}
//采集图像
WORD CAutoDeviceView::CollectImage()
{
	WORD wRet = 0;
	CString strTemp = _T(""), strErr = _T("");
	int ixCount = 0, iyCount = 0, ixNum = 0, iyNum = 0;
	double dOffX = 22, dOffY = 22, dx = 0, dy = 0, dStartX = 0, dStartY = 0, dZ = 0;
	dOffX = Struct_Unite.cp.dLen;
	dOffY = Struct_Unite.cp.dWide;

	int iCarame = 0;
	ixNum = Struct_Unite.gp.dPcbLen/dOffX + 1;
	iyNum = Struct_Unite.gp.dPcbWide/dOffY + 1;
	//GetRealTime();
	dZ = Struct_Unite.as.dZHigh;
	if (0 == Struct_Unite.axis_par.wDirect)
	{
		dStartX = Struct_Unite.axis_par.dLX + Struct_Unite.cp.dLen/2;
		dStartY = Struct_Unite.axis_par.dLY + Struct_Unite.cp.dWide/2;
	}
	else if (1 == Struct_Unite.axis_par.wDirect)
	{
		dStartX = Struct_Unite.axis_par.dRX - Struct_Unite.gp.dPcbLen  - Struct_Unite.cp.dLen/2;
		dStartY = Struct_Unite.axis_par.dRY + Struct_Unite.cp.dWide/2;
	}
	for (ixCount = 0; ixCount < ixNum; ixCount++)
	{
		if (bAutoFlag) return wRet;
		for (iyCount = 0; iyCount < iyNum; iyCount++)
		{
			if (bAutoFlag) return wRet;

			dx = dOffX * ixCount;
			if ((ixCount%2)==0)
				dy = dOffY * iyCount;
			else 
				dy = dOffY * (iyNum - iyCount - 1);

			wRet = Axis_Move(Struct_Unite.axis_par.wXAxis, Struct_Unite.axis_par.dXMinVel, Struct_Unite.axis_par.dXMaxVel,
				Struct_Unite.axis_par.dXTAcc, Struct_Unite.axis_par.dXTDec,
				Struct_Unite.axis_par.dXTs, 0, dStartX + dx, 1, 0);
			if (wRet) return wRet;
			wRet = Axis_Move(Struct_Unite.axis_par.wYAxis, Struct_Unite.axis_par.dYMinVel, Struct_Unite.axis_par.dYMaxVel,
				Struct_Unite.axis_par.dYTAcc, Struct_Unite.axis_par.dYTDec,
				Struct_Unite.axis_par.dYTs, 0, dStartY + dy, 1, 0);
			if (wRet) return wRet;
			wRet = Axis_Move(Struct_Unite.axis_par.wZAxis, Struct_Unite.axis_par.dZMinVel, Struct_Unite.axis_par.dZMaxVel,
				Struct_Unite.axis_par.dZTAcc, Struct_Unite.axis_par.dZTDec,
				Struct_Unite.axis_par.dZTs, 0, Struct_Unite.Real_Coor.dz, 1, 0);
			if (wRet) return wRet;
			do 
			{
				DoEvents();
				if (bAutoFlag) return wRet;
			} while (d2410_check_done(Struct_Unite.axis_par.wXAxis) == 0 || d2410_check_done(Struct_Unite.axis_par.wYAxis) == 0 || d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0);//轴正在运行
			::DeleteObject(bmp1);//释放资源
			WaitTime(50);
			wRet = ExecuteGrab();
			if (wRet) return wRet;
			sType = 1;
			WaitTime(200);
			//iCarame++;
			//strTemp.Format(_T("%s%d"), strPath, iCarame);
			//strTemp += _T(".bmp");
			SYSTEMTIME st;
			GetLocalTime(&st);
			strTemp.Format(_T("%s%04d-%02d-%02d_%02d-%02d-%02d-%06d%s"), strPath, st.wYear, st.wMonth, st.wDay
				, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, _T(".bmp"));
			if (!SaveNewImage(bmp1, strTemp, 8))
			{
				return SAVEFAIL;
			}
		}
	}
	return wRet;
}

#pragma region 焊盘检测结果显示
//检测结果1=OK，0=NG
void CAutoDeviceView::CheckResust(WORD wType)
{
	if (wType)
	{
		m_StrLabelOkNg.SetTextColor(RGB(18, 250, 57));
		m_StrLabelOkNg.SetText(_T("OK"));
	}
	else
	{
		m_StrLabelOkNg.SetTextColor(RGB(252, 33, 16));
		m_StrLabelOkNg.SetText(_T("NG"));
	}
}
void CAutoDeviceView::On2DView()
{
	// 	C2DViewDlg Dlg;
	// 	Dlg.DoModal();
}
#pragma endregion 

void CAutoDeviceView::OnBnClickedBtnInto()
{
	WORD wRet = 0;
	CString strErr = _T("");
	UserPostMessage(_T(""), 0);
	wRet = MoveSafeCoor();//移动到安全位置
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	//if (!GetOneBitIn(IN11))
	//{
	//	MessageBox(_T("未检测到气压,请确实是否已经正确连接并且已经打开！"), ALERT_ERR, ALERT_ICON);
	//	return;
	//}
	//先全部复位挡板气缸，夹板气缸
	wRet = SetAndGetIO(SLEEPTIME, LEFTIN4, LEFTOUT6, HIGHLEVEL);
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	wRet = SetAndGetIO(SLEEPTIME, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	wRet = SetAndGetIO(SLEEPTIME, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	IntoEnable(FALSE);
	hAutoDeviceThread = CreateThread(NULL, 0, AutoEnterThread, this, 0, NULL);
}

DWORD WINAPI AutoEnterThread(LPVOID lpParame)
{
	CString strErr = _T("");
	WORD wRet = 0;
	CAutoDeviceView* pView = (CAutoDeviceView*)lpParame;
	if (0 == Struct_Unite.axis_par.wDirect) //左侧进板
	{
		//检测轨道内是否有板
		do 
		{
			DoEvents();
		} while (!pView->GetOneBitIn(IN8));
		pView->UserPostMessage(_T("出板位置有板"), 1);
		if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))//除出口位置外其他位置有板则送到出口位置
		{
			wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);//轨道传输正向
			if (wRet)
			{
				goto ErrInfo;
			}
			//板子运行到出口位置停止等待取走
			wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			pView->UserPostMessage(_T("出板位置有板"), 1);
		}
		do 
		{
			DoEvents();
		} while (!pView->GetOneBitIn(IN8));//检查出板位置板子是否拿走
		SETTIMEOUT(ENTERTOT); //一定时间内没有检查到板子进板位置，取消进板动作
		pView->UserPostMessage(_T("等待进板"), 1);
		do 
		{
			do 
			{
				if (!pView->GetOneBitIn(IN7))
				{
					break;
				}
				DoEvents();
			}while(!TIMEOUT1);
			if (TIMEOUT1) 
			{
				wRet = ENTERTIMEOUT;
				pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, HIGHLEVEL);
				goto ErrInfo;
			}
			DoEvents();
		} while (pView->GetOneBitIn(IN7));//检查进板位置是否进来板子
		pView->UserPostMessage(_T("进板位置有板"), 1);
		if (!pView->GetOneBitIn(IN7))//有板则传送
		{
			wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			//挡板气缸顶起
			wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
		}
		do 
		{
			DoEvents();
		} while (pView->GetOneBitIn(IN10));
		if (!pView->GetOneBitIn(IN10))//检测到板子
		{
			pView->WaitTime(SLEEPTIME);
			wRet = pView->SetOneBitOut(OUT9, HIGHLEVEL);//停止传送
			if (wRet)
			{
				goto ErrInfo;
			}
			//夹板气缸顶起
			wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			pView->UserPostMessage(_T("右挡板位置有板"), 1);
			goto ErrInfo;
		}
	}
	else if (1 == Struct_Unite.axis_par.wDirect) //右侧进板
	{
		//检测轨道内是否有板
		do 
		{
			DoEvents();
		} while (!pView->GetOneBitIn(IN7));
		pView->UserPostMessage(_T("出板位置有板"), 1);
		if (!pView->GetOneBitIn(IN8) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))//除出口位置外其他位置有板则送到出口位置
		{
			wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);//轨道传输反向
			if (wRet)
			{
				goto ErrInfo;
			}
			//板子运行到出口位置停止等待取走
			wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			pView->UserPostMessage(_T("出板位置有板"), 1);
		}
		do 
		{
			DoEvents();
		} while (!pView->GetOneBitIn(IN7));//检查出板位置板子是否拿走
		SETTIMEOUT(ENTERTOT); //一定时间内没有检查到板子进板位置，取消进板动作
		pView->UserPostMessage(_T("等待进板"), 1);
		do 
		{
			do 
			{
				if (!pView->GetOneBitIn(IN8))
				{
					break;
				}
				DoEvents();
			}while(!TIMEOUT1);
			if (TIMEOUT1) 
			{
				wRet = ENTERTIMEOUT;
				pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
				goto ErrInfo;
			}
			DoEvents();
		} while (pView->GetOneBitIn(IN8));//检查进板位置是否进来板子
		pView->UserPostMessage(_T("进板位置有板"), 1);
		if (!pView->GetOneBitIn(IN8))//有板则传送
		{
			wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			//挡板气缸顶起
			wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
		}
		do 
		{
			DoEvents();
		} while (pView->GetOneBitIn(IN9));
		if (!pView->GetOneBitIn(IN9))//检测到板子
		{
			pView->WaitTime(SLEEPTIME);
			wRet = pView->SetOneBitOut(OUT10, HIGHLEVEL);//停止传送
			if (wRet)
			{
				goto ErrInfo;
			}
			//夹板气缸顶起
			wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			pView->UserPostMessage(_T("左挡板位置有板"), 1);
			goto ErrInfo;
		}
	}
ErrInfo:
	pView->IntoEnable(TRUE);
	if (wRet)
	{
		Err_Info(wRet, strErr);
		//pView->MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		pView->UserPostMessage(strErr, 1);
	}
	return 1L;
}
DWORD WINAPI AutoDeviceThread(LPVOID lpParame)
{
	WORD wRet = 0;
	CString strErr = _T("");
	BOOL bLoops = TRUE;
	WORD wAutoType;
	CAutoDeviceView* pView = (CAutoDeviceView*)lpParame;
	wAutoType = pView->m_strCombAutoType.GetCurSel();
	while (bLoops)
	{
		if (!pView->bAutoFlag)
		{
			bLoops = FALSE;
			wRet = AUTOEXIT;
			goto ThreadErrInfo;
		}
		if (0 == wAutoType)//单机自动测试
		{
			if (0 == Struct_Unite.axis_par.wDirect)//左进板
			{
				//首先检测除进板位置，其他几个位置是否有板,若有把板子传送到进板位置处
				if (!pView->GetOneBitIn(IN8) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT10, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("进板位置有板"),1);
					pView->WaitTime(50);//到进板位置延时
				}
				//进板位置若没有板子则一直等待板子进来
				pView->UserPostMessage(_T("等待进板"),1);
				wRet = pView->WaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//感应到板子后传输板子到相应位置
				if (!pView->GetOneBitIn(IN7))
				{
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//挡板气缸若未顶起则顶起挡板气缸
					if (pView->GetOneBitIn(LEFTIN4))
					{
						if (!pView->bAutoFlag)
						{
							bLoops = FALSE;
							wRet = AUTOEXIT;
							goto ThreadErrInfo;
						}
						wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, LOWLEVEL);
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
					}
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				wRet = pView->WaitAndControlSleep(IN10, OUT9, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("右挡板位置有板"),1);
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				pView->WaitTime(1500);
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//右进板
			{
				//首先检测除进板位置，其他几个位置是否有板,若有把板子传送到进板位置处
				if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("进板位置有板"),1);
					pView->WaitTime(50);//到进板位置延时
				}
				//进板位置若没有板子则一直等待板子进来
				pView->UserPostMessage(_T("等待进板"),1);
				wRet = pView->WaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//感应到板子后传输板子到相应位置
				wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT10, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//挡板气缸若未顶起则顶起挡板气缸
				if (pView->GetOneBitIn(RIGHTIN5))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				wRet = pView->WaitAndControlSleep(IN9, OUT10, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("左挡板位置有板"),1);
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				pView->WaitTime(1500);
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
		}
		else if (1 == wAutoType)//单机手动测试
		{
			if (0 == Struct_Unite.axis_par.wDirect)//左进板
			{
				//首先检测除进板位置，其他几个位置是否有板,若有把板子传送到进板位置处
				if (!pView->GetOneBitIn(IN8) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT10, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("进板位置有板"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//若进板位置有板则先拿起使其感应不到再重新放板到进板位置
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//进板位置若没有板子则一直等待板子进来
				pView->UserPostMessage(_T("等待进板"),1);
				wRet = pView->WaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//挡板气缸若未顶起则顶起挡板气缸
				if (pView->GetOneBitIn(LEFTIN4))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//感应到板子后传输板子到相应位置
				wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT9, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				wRet = pView->WaitAndControlSleep(IN10, OUT9, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("右挡板位置有板"),1);
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				pView->WaitTime(1500);
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//右进板
			{
				//首先检测除进板位置，其他几个位置是否有板,若有把板子传送到进板位置处
				if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("进板位置有板"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//若进板位置有板则先拿起使其感应不到再重新放板到进板位置
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//进板位置若没有板子则一直等待板子进来
				pView->UserPostMessage(_T("等待进板"),1);
				wRet = pView->WaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//挡板气缸若未顶起则顶起挡板气缸
				if (pView->GetOneBitIn(RIGHTIN5))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//感应到板子后传输板子到相应位置
				wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT10, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				wRet = pView->WaitAndControlSleep(IN9, OUT10, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("左挡板位置有板"),1);
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				pView->WaitTime(1500);
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
		}
		else if (2 == wAutoType)//脱机自动检测
		{
			if (0 == Struct_Unite.axis_par.wDirect)//左进板
			{
				//首先检测除出板位置，其他几个位置是否有板,若有把板子传送到出板位置处
				if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//检测出板位置的板子是否拿走
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//若进板位置有板则先拿起使其感应不到再重新放板到进板位置
				pView->UserPostMessage(_T("进板位置有板"),1);
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//进板位置若没有板子则一直等待板子进来
				pView->UserPostMessage(_T("等待进板"),1);
				wRet = pView->WaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//挡板气缸若未顶起则顶起挡板气缸
				if (pView->GetOneBitIn(LEFTIN4))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//感应到板子后传输板子到相应位置
				wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT9, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				wRet = pView->WaitAndControlSleep(IN10, OUT9, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("右挡板位置有板"),1);
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				pView->WaitTime(1500);
				//完成OK后挡板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//右进板
			{
				//首先检测除出板位置，其他几个位置是否有板,若有把板子传送到出板位置处
				if (!pView->GetOneBitIn(IN8) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT10, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//检测出板位置的板子是否拿走
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//若进板位置有板则先拿起使其感应不到再重新放板到进板位置
				pView->UserPostMessage(_T("进板位置有板"),1);
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//进板位置若没有板子则一直等待板子进来
				pView->UserPostMessage(_T("等待进板"),1);
				wRet = pView->WaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//挡板气缸若未顶起则顶起挡板气缸
				if (pView->GetOneBitIn(RIGHTIN5))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//感应到板子后传输板子到相应位置
				wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT10, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				wRet = pView->WaitAndControlSleep(IN9, OUT10, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("左挡板位置有板"),1);
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				pView->WaitTime(1500);
				//完成OK后挡板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
		}
		else if (3 == wAutoType)//联机自动检测
		{
			if (0 == Struct_Unite.axis_par.wDirect)//左进板
			{
				//首先检测除出板位置，其他几个位置是否有板,若有把板子传送到出板位置处
				if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板"),1);
					//板子在出板位置发出板信号，并等待下位机要板
					wRet = pView->SetOneBitOut(OUT5, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板要出"),1);
					wRet = pView->WaitAndControl(IN13, OUT5, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//传送板子
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->NotWaitAndControl(IN8, OUT9, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//检测出板位置的板子是否传送走
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//通道内没有板子准备要板进板
				if (pView->GetOneBitIn(IN7) && pView->GetOneBitIn(IN8)
					&& pView->GetOneBitIn(IN9) && pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);//要板
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("进板位置要板"),1);
					wRet = pView->WaitLoop(IN12);//检测下位机要板
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					if (!pView->GetOneBitIn(IN12))
					{
						wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);//传输
						if (wRet)
						{
							pView->SetOneBitOut(OUT9, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->SetOneBitOut(OUT4, HIGHLEVEL);//停止要板
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						if (!pView->bAutoFlag)
						{
							bLoops = FALSE;
							wRet = AUTOEXIT;
							goto ThreadErrInfo;
						}
						//挡板气缸若未顶起则顶起挡板气缸
						if (pView->GetOneBitIn(RIGHTIN5))
						{
							if (!pView->bAutoFlag)
							{
								bLoops = FALSE;
								wRet = AUTOEXIT;
								goto ThreadErrInfo;
							}
							wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, LOWLEVEL);
							if (wRet)
							{
								bLoops = FALSE;
								goto ThreadErrInfo;
							}
						}
					}
					wRet = pView->WaitAndControlSleep(IN10, OUT9, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("右挡板位置有板"),1);
				}
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				pView->WaitTime(1500);
				//完成OK后挡板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//右进板
			{
				//首先检测除出板位置，其他几个位置是否有板,若有把板子传送到出板位置处
				if (!pView->GetOneBitIn(IN8) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT10, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板"),1);
					//板子在出板位置发出板信号，并等待下位机要板
					wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板要出"),1);
					wRet = pView->WaitAndControl(IN12, OUT4, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//传送板子
					wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT10, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->NotWaitAndControl(IN7, OUT10, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//检测出板位置的板子是否传送走
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//通道内没有板子准备要板进板
				if (pView->GetOneBitIn(IN7) && pView->GetOneBitIn(IN8) &&
					pView->GetOneBitIn(IN9) && pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT5, LOWLEVEL);//要板
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("进板位置要板"),1);
					wRet = pView->WaitLoop(IN13);//检测下位机要板
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					if (!pView->GetOneBitIn(IN13))
					{
						wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);//传输
						if (wRet)
						{
							pView->SetOneBitOut(OUT10, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->SetOneBitOut(OUT5, HIGHLEVEL);//停止要板
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						//挡板气缸若未顶起则顶起挡板气缸
						if (pView->GetOneBitIn(RIGHTIN5))
						{
							if (!pView->bAutoFlag)
							{
								bLoops = FALSE;
								wRet = AUTOEXIT;
								goto ThreadErrInfo;
							}
							wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, LOWLEVEL);
							if (wRet)
							{
								bLoops = FALSE;
								goto ThreadErrInfo;
							}
						}
					}
					wRet = pView->WaitAndControlSleep(IN9, OUT10, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("左挡板位置有板"),1);
				}
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				pView->WaitTime(1500);
				//完成OK后挡板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
		}
		else if (4 == wAutoType)//直通自动传送
		{
			if (0 == Struct_Unite.axis_par.wDirect)//左进板
			{
				//首先检测通道内是否有板子
				if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN8) ||
					!pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					//若出板位置没有板子,则先把其他位置的板子传送到出板位置
					if (pView->GetOneBitIn(IN8))
					{
						wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
						if (wRet)
						{
							pView->SetOneBitOut(OUT9, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						pView->UserPostMessage(_T("出板位置有板"),1);
					}
					//板子在出板位置发出板信号，并等待下位机要板
					wRet = pView->SetOneBitOut(OUT5, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板要出"),1);
					wRet = pView->WaitAndControl(IN13, OUT5, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//传送板子
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//检测出板位置的板子是否传送走
					wRet = pView->NotWaitAndControl(IN8, OUT9, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//检测出板位置的板子是否传送走
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//通道内没有板子准备要板进板
				if (pView->GetOneBitIn(IN7) && pView->GetOneBitIn(IN8) &&
					pView->GetOneBitIn(IN9) && pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);//要板
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("进板位子要板"),1);
					wRet = pView->WaitLoop(IN12);//检测下位机要板
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					if (!pView->GetOneBitIn(IN12))
					{
						wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);//传输
						if (wRet)
						{
							pView->SetOneBitOut(OUT9, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);//停止要板
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
					}
					wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);//出板检测到
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板"),1);
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//右进板
			{
				//首先检测通道内是否有板子
				if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN8) ||
					!pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					//若出板位置没有板子，则先把其他位置的板子传送到出板位置
					if (pView->GetOneBitIn(IN7))
					{
						wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
						if (wRet)
						{
							pView->SetOneBitOut(OUT10, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);//检测板子是否到出板位置
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						pView->UserPostMessage(_T("出板位置有板"),1);
					}
					//板子在出板位置发出板信号，并等待下位机要板
					wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板要出"),1);
					wRet = pView->WaitAndControl(IN12, OUT4, HIGHLEVEL);//检测到有板要出信号,关闭出板信号
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//传送板子
					wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT10, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//检测出板位置的板子是传送走
					wRet = pView->NotWaitAndControl(IN7, OUT10, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//检测出板位置的板子是否传送走
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//通道内没有板子准备要板进板
				if (pView->GetOneBitIn(IN7) && pView->GetOneBitIn(IN8) &&
					pView->GetOneBitIn(IN9) && pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT5, LOWLEVEL);//要板
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("进板位子要板"),1);
					wRet = pView->WaitLoop(IN13);//检测下位机要板
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					if (!pView->GetOneBitIn(IN13))
					{
						wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);//传输
						if (wRet)
						{
							pView->SetOneBitOut(OUT10, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->SetOneBitOut(OUT5, HIGHLEVEL);//停止要板
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
					}
					wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);//传输到出板位置并停止
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板"),1);
				}
			}
		}
		else if (5 == wAutoType)//规则制定采集
		{
			if (0 == Struct_Unite.axis_par.wDirect)//左进板
			{
				//首先检测除出板位置，其他几个位置是否有板,若有把板子传送到出板位置处
				if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//检测出板位置的板子是否拿走
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//若进板位置有板则先拿起使其感应不到再重新放板到进板位置
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("等待进板"),1);
				//进板位置若没有板子则一直等待板子进来
				wRet = pView->WaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//挡板气缸若未顶起则顶起挡板气缸
				if (pView->GetOneBitIn(LEFTIN4))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//感应到板子后传输板子到相应位置
				wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT9, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				wRet = pView->WaitAndControlSleep(IN10, OUT9, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("右挡板位置有板"),1);
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				//pView->WaitTime(1500);
				wRet = pView->CollectImage();
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//完成OK后挡板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//然后传输板子到出板位置停止自动运行
				wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT9, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("出板位置有板"),1);
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//右进板
			{
				//首先检测除出板位置，其他几个位置是否有板,若有把板子传送到出板位置处
				if (!pView->GetOneBitIn(IN8) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT10, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("出板位置有板"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//检测出板位置的板子是否拿走
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//若进板位置有板则先拿起使其感应不到再重新放板到进板位置
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("等待进板"),1);
				//进板位置若没有板子则一直等待板子进来
				wRet = pView->WaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//挡板气缸若未顶起则顶起挡板气缸
				if (pView->GetOneBitIn(RIGHTIN5))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//感应到板子后传输板子到相应位置
				wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT10, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				wRet = pView->WaitAndControlSleep(IN9, OUT10, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("左挡板位置有板"),1);
				//到位后夹板气缸顶起
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//就绪后开始工作
				pView->WaitTime(1500);
				//wRet = pView->CollectImage();
				//if (wRet)
				//{
				//	bLoops = FALSE;
				//	goto ThreadErrInfo;
				//}
				//完成OK后挡板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//完成OK后夹板气缸下降
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//然后传输板子到出板位置并停止自动运行
				wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT10, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("出板位置有板"),1);
			}
			bLoops = FALSE;//只执行一次
		}
		if (!wRet)//正常完成
		{
			wRet = MoveSafeCoor();//移动到安全位置
			if (wRet)
			{
				bLoops = FALSE;
				goto ThreadErrInfo;
			}
		}
ThreadErrInfo:
		if (wRet)
		{
			for (WORD wOut = 4; wOut < 11; wOut++)
			{
				pView->SetOneBitOut(wOut, HIGHLEVEL);
			}
			pView->bAutoFlag = TRUE;//出错或正常退出后初始化自动运行标志
			bLoops = FALSE;
			Err_Info(wRet, strErr);
			//pView->MessageBox(strErr, ALERT_ERR, ALERT_ICON);
			pView->UserPostMessage(strErr, 1);
		}
		if (!bLoops)
		{
			pView->EnableControl(TRUE);
		}
	}
	return 1L;
}

//状态栏显示隐藏
void CAutoDeviceView::OnStsBarMsg()
{
	CRect rc;
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if (pFrame->m_wndStatusBar.IsWindowVisible())
	{
		pFrame->ShowControlBar(&pFrame->m_wndStatusBar, FALSE, FALSE);//关闭 	
	}
	else
	{
		pFrame->ShowControlBar(&pFrame->m_wndStatusBar, TRUE, FALSE);//显示 
	}
	m_StrReal_Image.GetClientRect(&rc);
	m_StrReal_Image.ClientToScreen(&rc);
	InvalidateRect(rc); 
}

//线程中更新状态栏消息提示
//lParam =0清空消息, 1=实时消息提示
LRESULT CAutoDeviceView::OnUpdateInfo(WPARAM wParam,LPARAM lParam)
{
	CString strTemp = _T("");
	TCHAR   szTemp[1024] = _T("");
	CHAR    szNarrow[1024];
	memset(szNarrow, 0, sizeof(szNarrow));
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if (lParam)
	{
		memcpy(szNarrow, (char*)wParam, sizeof(szNarrow));
		charTowchar(szNarrow, szTemp, sizeof(szTemp));
		strTemp = (CString)szTemp;
		pFrame->m_wndStatusBar.SetPaneText(2, strTemp.Trim());
	}
	else
	{
		pFrame->m_wndStatusBar.SetPaneText(2, strTemp);
	}
	return 0;
}

//发送自定义消息
//strMsg消息内容
//消息类型 0 = 清空消息, 1=实时消息
void CAutoDeviceView::UserPostMessage(CString strMsg, WORD wType)
{
	CHAR szNarrow[1024];
	memset(szNarrow, 0, sizeof(szNarrow));
	if (wType)
	{
		wcharTochar(strMsg, szNarrow, strMsg.GetLength()*2);
		::SendMessage(GetSafeHwnd(), WM_UPDATE_INFO, (UINT)szNarrow, wType);
	}
	else
	{
		::SendMessage(GetSafeHwnd(), WM_UPDATE_INFO, 0, wType);
	}
}
BOOL CAutoDeviceView::PreTranslateMessage(MSG* pMsg)
{
	if (WM_RBUTTONDOWN == pMsg->message)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_STATIC_REAL_IMAGE)->m_hWnd)
		{
			CString strTemp = _T(""), strTemp1 = _T("");
			CMenu *pMenu;
			CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
			pMenu = pFrame->GetMenu()->GetSubMenu(1);
			pMenu->GetMenuString(ID_GRAB, strTemp, 1);//系统菜单名字
			//pMenu = pFrame->GetMenu()->GetSubMenu(2);
			pMenu->GetMenuString(ID_OPENCAMERA, strTemp1, 1);//系统菜单名字

			CMenu menu;
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_STRING, ID_OPENCAMERA, strTemp1);
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_CAMERAPAR, _T("相机参数"));
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_GRAB, strTemp);
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_EXEC, _T("执行拍照"));
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_SAVEIMAGE, _T("保存图片"));
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_LOADIMAGE, _T("加载图片"));
			CPoint pt;
			GetCursorPos(&pt);
			menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
			menu.DestroyMenu();		

			//CMenu menu; //定义下面要用到的cmenu对象
			//menu.LoadMenu(IDR_MAINFRAME); //装载自定义的右键菜单
			//CMenu *pContextMenu=menu.GetSubMenu(2); //获取第2个弹出菜单,所以第2个菜单必须有子菜单
			//CPoint pt;
			//GetCursorPos(&pt);
			//ClientToScreen(&pt);
			//pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}
