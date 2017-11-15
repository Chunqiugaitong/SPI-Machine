
// AutoDeviceView.cpp : CAutoDeviceView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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

// CAutoDeviceView ����/����

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

#pragma region ���ڳ�ʼ��

BOOL CAutoDeviceView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CFormView::PreCreateWindow(cs);
}
//��ͼ�����ʼ�������ƶԻ���OnInitialDlg
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

	m_strCombAutoType.AddString(_T("�����Զ�����"));
	m_strCombAutoType.AddString(_T("�����ֶ�����"));
	m_strCombAutoType.AddString(_T("�ѻ��Զ����"));
	m_strCombAutoType.AddString(_T("�����Զ����"));
	m_strCombAutoType.AddString(_T("ֱͨ�Զ�����"));
	m_strCombAutoType.AddString(_T("�����ƶ��ɼ�"));
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

	strTemp.Format(_T("���巽��:%s"), Struct_Unite.axis_par.wDirect == 0?(_T("������")):(_T("�Ҳ����")));
	m_StrLabelDirect.SetText(strTemp);

	fclose(pf);
	return 0;
}
void CAutoDeviceView::LoadDefFile()//�������������Ĭ�ϵ����ʹ�õ��ļ�
{
	short sRet = 0;
	CString strPath = _T("");
	CString strTemp = _T("");
	TCHAR szTemp[512] = _T("");
	strPath = GetFilePath();
	strTemp = strPath;
	strTemp += _T("�����ļ�\\");
	strPath += INI_FILENAME;
	::GetPrivateProfileString(_T("FileManage"), _T("CurrentFileName"), _T(""), szTemp, sizeof(szTemp), strPath);
	strTemp += (CString)szTemp;
	if (!LoadConData(strTemp))
	{
		//m_StrLabelName.SetText(_T("��·������:") + Struct_Unite.gp.strName);
		//strPath.Format(_T("%s%.03f * %.03f"), _T("��·��ߴ�:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
		//m_StrLabelSize.SetText(strPath);
		if (1 == Struct_Unite.gp.wIsGerber)
		{
			strTemp = GetFilePath() + _T("Gerber") + '\\' + Struct_Unite.gp.strGerberName.Trim();
	//		OpenGerber(strTemp);
		}
	}
}
#pragma endregion 

#pragma region CAutoDeviceView ���
// CAutoDeviceView ���
#ifdef _DEBUG
void CAutoDeviceView::AssertValid() const
{
	CFormView::AssertValid();
}

void CAutoDeviceView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CAutoDeviceDoc* CAutoDeviceView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAutoDeviceDoc)));
	return (CAutoDeviceDoc*)m_pDocument;
}
#endif //_DEBUG
#pragma endregion

// CAutoDeviceView ��Ϣ�������

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
		//���ػ汳����OnPaint���ھɵı����ϼ�������
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
		m_StrImage.GetClientRect(&m_navigateMargin);//��ȡ�ؼ�����
		m_StrReal_Image.GetClientRect(&m_fovMargin);//��ȡ�ؼ�����

		InitialDraw();
		DrawGerber();
 	}
	return;
}

//�˳�ϵͳ
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
		strTemp = _T("�Ƿ�Ҫ�˳�����");
	}
	else
	{
		strTemp = _T("Whether to quit the program?");
	}
	if (IDOK == MessageBox(strTemp, ALERT_INFOR, ALERT_CONFIRMQ))
	{
		StopGrab();//�ر�����ɼ�״̬
		if (bConnect)
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
	strPath +=_T("�����ļ�");
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

			//m_StrLabelName.SetText(_T("��·������:") + Struct_Unite.gp.strName);
			//strTemp.Format(_T("%s%.03f * %.03f"), _T("��·��ߴ�:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
			//m_StrLabelSize.SetText(strTemp);
			_tcscpy_s(szIniPath, strIniPath.GetString());
			::WritePrivateProfileString(_T("FileManage"), _T("CurrentFileName"), FileDlg.GetFileName(), szIniPath); //��¼��ini�ļ���
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
		//m_StrLabelName.SetText(_T("��·������:") + Struct_Unite.gp.strName);
		//strTemp.Format(_T("%s%.03f * %.03f"), _T("��·��ߴ�:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
		//m_StrLabelSize.SetText(strTemp);
		//CString str = FileDlg.GetFileName();

		//_tcscpy_s(szIniPath, strIniPath.GetString());
		//::WritePrivateProfileString(_T("FileManage"), _T("CurrentFileName"), FileDlg.GetFileName(), szIniPath); //��¼��ini�ļ���
		WriteConData();

//		OpenGerber(Dlg.m_strFilePath);

	}
}

//�Ƿ����ϵͳ�˵�
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
	//AfxGetMainWnd()->DrawMenuBar();//ˢ�²˵�����ʾ״̬
	pFrame->DrawMenuBar();
}
//�ؼ�ʹ��
void CAutoDeviceView::EnableControl(BOOL bActive)
{
	switch (m_strCombAutoType.GetCurSel())
	{
	case 0:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("�����Զ�����"):_T("�����Զ�����״̬"));
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("�����ֶ�����"):_T("�����ֶ�����״̬"));
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("�ѻ��Զ����"):_T("�ѻ��Զ����״̬"));
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("�����Զ����"):_T("�����Զ����״̬"));
		break;
	case 4:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("ֱͨ�Զ�����"):_T("ֱͨ�Զ�����״̬"));
		break;
	case 5:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(TRUE == bActive?_T("�����ƶ��ɼ�"):_T("�����ƶ��ɼ�״̬"));
		break;
	}
	((CButton*)GetDlgItem(IDC_BTN_INTO))->EnableWindow(bActive);
	((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->EnableWindow(bActive);
	((CButton*)GetDlgItem(IDC_BTN_SUSPEND))->EnableWindow(!bActive);
	((CButton*)GetDlgItem(IDC_BTN_RESET))->EnableWindow(!bActive);
	m_strCombAutoType.EnableWindow(bActive);
}
//�������ʱ��ť����
void CAutoDeviceView::IntoEnable(BOOL bActive)
{
	((CButton*)GetDlgItem(IDC_BTN_INTO))->EnableWindow(bActive);
	((CButton*)GetDlgItem(IDC_BTN_INTO))->SetWindowText(TRUE == bActive?_T("����"):_T("�ȴ�����"));
	((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->EnableWindow(bActive);
	m_strCombAutoType.EnableWindow(bActive);
}
//ѡ���Ƿ���ʾʮ�ֻ���
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
	//pFrame->m_wndStatusBar.SetPaneText(1, _T("ģʽ: ����"));
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
	////////////              ��Ҫͬ������
// 	if (m_bIsFovInspected)
// 	{
// 		return;
// 	}

	CRect ImageRect;
	//�ؼ��ߴ�����Ļ����ϵ�µ�����
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(ImageRect);
	//�ؼ��ߴ� ����Ļ����ת�����ͻ�������
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
				//����ƶ���padCluster����λ��	

// 				bool wRet = false;
// 				CString strErr = L"";
// 				//׼ȷ����Ҫ����
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
// 				//�������
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
//����gerber����ͼ
void CAutoDeviceView::InitialDraw()
{
 	m_StrImage.GetClientRect(&m_navigateMargin);//��ȡ�ؼ�����
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
	//Ϊ�߿�����Ե
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
	m_StrImage.GetClientRect(&m_navigateMargin);//��ȡ�ؼ�����

	CAutoDeviceDoc* pAutoDeviceDoc = GetDocument();
// 	//Ϊ�߿�����Ե
 	int nDelt = 10;
	//********************************Fov***********************************************************/
	Gxmc_Gerber::PadCluster padCluster = pAutoDeviceDoc->m_mapPadCluster[m_nClusterIndex];
	m_fovScreen.realMarginRect = RectD(padCluster.clusterMargin->m_startPoint.x,
									padCluster.clusterMargin->m_stopPoint.y,
									padCluster.clusterMargin->m_stopPoint.x,
									padCluster.clusterMargin->m_startPoint.y);
	//Ϊ�߿�����Ե
	m_fovScreen.realMarginRect.left -= nDelt/4;
	m_fovScreen.realMarginRect.right += nDelt/4;
	m_fovScreen.realMarginRect.top += nDelt/4;
	m_fovScreen.realMarginRect.bottom -= nDelt/4;

	m_StrReal_Image.GetClientRect(&m_fovMargin);//��ȡ�ؼ�����
	double fxFov = (m_fovMargin.Width())/m_fovScreen.realMarginRect.width();
	double fyFov = (m_fovMargin.Height())/m_fovScreen.realMarginRect.height();
	m_fovScreen.factor = fxFov < fyFov ? fxFov : fyFov;
	m_fovScreen.startX = m_fovScreen.realMarginRect.left;
	m_fovScreen.startY = m_fovScreen.realMarginRect.bottom;
	m_fovScreen.offsetX = 0;
	m_fovScreen.offsetY = 0;

	m_bShowFov = true;
}
//������Ұ����ǰ�ĳ�ʼ������
void CAutoDeviceView::InitialAllFovMargin()
{
	CRect ImageRect;
	//�ؼ��ߴ�����Ļ����ϵ�µ�����
	GetDlgItem(IDC_STATIC_IMAGE)->GetWindowRect(ImageRect);
	//�ؼ��ߴ� ����Ļ����ת�����ͻ�������
	ScreenToClient(ImageRect);

	//��ռ�������
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
			/**///�õ��߼���������λ��
			/**/circle.WorldViewToLogicView(padCluster.clusterMargin->m_startPoint.x,padCluster.clusterMargin->m_startPoint.y,left,bottom);
			/**/circle.WorldViewToLogicView(padCluster.clusterMargin->m_stopPoint.x,padCluster.clusterMargin->m_stopPoint.y,right,top);
			/**///��ͼ��ؼ����Ͻ�����Ļ����ϵ�µ�����Ϊ��㣨��׼��
			/**/CRect clusterRect = CRect(left,top,right,bottom);
			/**/clusterRect.left += ImageRect.left;
			/**/clusterRect.top += ImageRect.top;
			/**/clusterRect.right += ImageRect.left;
			/**/clusterRect.bottom += ImageRect.top;
			m_vecClusterRect.push_back(clusterRect);
		}
	}
}
//����gerber���ݿ��ļ�����ĺ���
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
		//�ͷ������Դ
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
		//��ͼ��Ӹ���CBitmap
		//m_FovMemBmp.Attach(bmp1);

		m_FovMemDC.CreateCompatibleDC(NULL);		
		m_FovMemBmp.CreateCompatibleBitmap(m_pFovDC, m_fovMargin.Width(), m_fovMargin.Height());
		m_FovMemDC.SelectObject(&m_FovMemBmp);
		CPen pen(PS_SOLID, 1, RGB(0,255,0));
		m_FovMemDC.SelectObject(&pen);
		//��Fovͼ��

		//������
		pAutoDeviceDoc->m_gxmcGerber.DisplayPadCluster(m_FovMemDC,m_fovScreen,pAutoDeviceDoc->m_mapPadCluster[m_nClusterIndex]);
		m_pFovDC->BitBlt(0,0,m_fovMargin.Width(),m_fovMargin.Height(),&m_FovMemDC, 0, 0, SRCCOPY); 

		//��ʮ����
// 		m_pFovDC->SelectObject(&pen);
// 		m_pFovDC->MoveTo(m_fovMargin.Width()/2, 0);
// 		m_pFovDC->LineTo(m_fovMargin.Width()/2, m_fovMargin.Height());
// 		m_pFovDC->MoveTo(0, m_fovMargin.Height()/2);
// 		m_pFovDC->LineTo(m_fovMargin.Width(), m_fovMargin.Height()/2);

		//�ͷ������Դ
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
		StrData +="X�����:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wXAxis);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X��ÿת����:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXPulsePer);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X���ݾ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXPitch);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X���г�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X�������г�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTripNegaTive);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X����ʼ�ٶ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXMinVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X������ٶ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXMaxVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X�����ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTAcc);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X�����ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTDec);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="X��S��ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dXTs);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y�����:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wYAxis);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y��ÿת����:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYPulsePer);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y���ݾ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYPitch);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y���г�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y�������г�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTripNegaTive);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y����ʼ�ٶ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYMinVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y������ٶ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYMaxVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y�����ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTAcc);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y�����ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTDec);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Y��S��ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dYTs);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z�����:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wZAxis);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z��ÿת����:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZPulsePer);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z���ݾ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZPitch);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z���г�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z�������г�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTripNegaTive);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);


		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z����ʼ�ٶ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZMinVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z������ٶ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZMaxVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z�����ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTAcc);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z�����ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTDec);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z��S��ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dZTs);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W�����:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wWAxis);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W��ÿת����:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWPulsePer);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W���ݾ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWPitch);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W��ԭ����:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W����С���:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWMinTrip);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W����ʼ�ٶ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWMinVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W������ٶ�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWMaxVel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W�����ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWTAcc);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W�����ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWTDec);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="W��S��ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dWTs);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="���巽��:";
		sprintf_s(szTemp, "%d", Struct_Unite.axis_par.wDirect);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		strTemp.Format(_T("���巽��:%s"), Struct_Unite.axis_par.wDirect == 0?(_T("������")):(_T("�Ҳ����")));
		m_StrLabelDirect.SetText(strTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="�����X:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dLX);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="�����Y:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dLY);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="�ҽ���X:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dRX);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="�ҽ���Y:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.axis_par.dRY);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="��ȫλ��X:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.as.dXSafe);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);


		memset(szTemp, 0, sizeof(szTemp));
		StrData +="��ȫλ��Y:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.as.dYSafe);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="��ȫλ��Z:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.as.dZSafe);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Z�Ṥ���߶�:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.as.dZHigh);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="����ֱ��ʳ�:";
		sprintf_s(szTemp, "%d", Struct_Unite.cp.UiLenPixel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="����ֱ��ʿ�:";
		sprintf_s(szTemp, "%d", Struct_Unite.cp.UiWidePixel);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="����й�оƬ�ߴ糤:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dCcdSizeL);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="����й�оƬ�ߴ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dCcdSizeW);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="�����ͷ����:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dShotLen);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="��ͷ��������ľ���:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dWorkDistance);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="������ͼ������:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dLen);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="������ͼ�������:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dWide);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="����ع�ʱ��:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dEpsTime);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);
		SetExposure(Struct_Unite.cp.dEpsTime);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="�������:";
		sprintf_s(szTemp, "%.03f", Struct_Unite.cp.dGain);
		strcat_s(szTemp,"\n");
		StrData+=string(szTemp);
		SetGain(Struct_Unite.cp.dGain);

		memset(szTemp, 0, sizeof(szTemp));
		StrData +="Զ�ľ�ͷ�Ŵ���:";
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
	//	MessageBox(_T("���ݱ���ɹ���"), ALERT_INFOR, INFO_ICON);
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

		m_StrLabelName.SetText(_T("��·������:") + Struct_Unite.gp.strName);
		strTemp.Format(_T("%s%.03f * %.03f"), _T("��·��ߴ�:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
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
		strSettingFile +=_T("�����ļ�");
		strSettingFile +='\\';
		wcharTochar(strSettingFile, szPath, 512);
		if ( _access(szPath,00) == -1)
		{
			if (ERROR_SUCCESS != SHCreateDirectoryEx(NULL, strSettingFile, NULL))
			{
				//MessageBox(_T("��������ͼƬĿ¼���ļ���ʧ��"), ALERT_ERR, ALERT_ICON);
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
	::WritePrivateProfileString(_T("FileManage"), _T("CurrentFileName"), Struct_Unite.gp.strName + _T(".dat"), szIniTemp); //��¼��ini�ļ���
	//if (m_bEnglish)
	//{
	//	MessageBox(_T("���ݱ���ɹ���"), ALERT_INFOR, INFO_ICON);
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

	m_StrLabelName.SetText(_T("��·������:") + Struct_Unite.gp.strName);
	strTemp.Format(_T("%s%.03f * %.03f"), _T("��·��ߴ�:"), Struct_Unite.gp.dPcbLen, Struct_Unite.gp.dPcbWide);
	m_StrLabelSize.SetText(strTemp);

	fclose(pf);
	return 0;
}


#pragma region �豸�������̿���

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
		m_StrLabelSts.SetText(_T("�е�·��"));
	}
	else
	{
		m_StrLabelSts.SetText(_T("�޵�·��"));
	}
}
//������ť��Ӧ
void CAutoDeviceView::ResMacBtn()
{
	if (!GetOneBitIn(IN14))//����
	{
		OnBnClickedBtnAutocheck();
	}
	else if (!GetOneBitIn(IN14))//��λ
	{
		OnBnClickedBtnReset();
	}
	else if (!GetOneBitIn(IN14))//��ͣ
	{
		OnBnClickedBtnSuspend();
	}
	else if (!GetOneBitIn(IN14))//��ͣ
	{
		bAutoFlag = FALSE;
	}
}
//������ֹͣ����λ
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
	wRet = MoveSafeCoor();//�ƶ�����ȫλ��
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	if (4 != m_strCombAutoType.GetCurSel())//ֱͨ�Զ����䲻��Ҫ��ѹ
	{
		//if (!GetOneBitIn(IN11))
		//{
		//	MessageBox(_T("δ��⵽��ѹ,��ȷ���Ƿ��Ѿ���ȷ���Ӳ����Ѿ��򿪣�"), ALERT_ERR, ALERT_ICON);
		//	return;
		//}
		//��ȫ����λ�������ף��а�����
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
		strPath +=_T("�ɼ�ͼ��");
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
				MessageBox(_T("��������ͼƬĿ¼���ļ���ʧ��"), ALERT_ERR, ALERT_ICON);
				return;
			}
		}
		if (Struct_Unite.gp.dPcbLen == 0 || Struct_Unite.gp.dPcbWide == 0)
		{
			MessageBox(_T("�ɼ�����ǰ������ȷ��������չ����ĳ��Ϳ��ֵ"), ALERT_ERR, ALERT_ICON);
			return;
		}
		//SetGrabMode(0);//�������
	}
	hAutoDeviceThread = CreateThread(NULL, 0, AutoDeviceThread, this, 0, NULL);
	if (hAutoDeviceThread == NULL)
	{
		CloseHandle(hAutoDeviceThread);
		hAutoDeviceThread = NULL;
		if (m_bEnglish)
		{
			MessageBox(_T("�Զ���������ʧ��!"), ALERT_ERR, ALERT_ICON);
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
//����ģʽѡ��
void CAutoDeviceView::OnCbnSelchangeComboAutotype()
{
	return;
	switch (m_strCombAutoType.GetCurSel())
	{
	case 0:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("�����Զ�����"));
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("�����ֶ�����"));
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("�ѻ��Զ����"));
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("�����Զ����"));
		break;
	case 4:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("ֱͨ�Զ�����"));
		break;
	case 5:
		((CButton*)GetDlgItem(IDC_BTN_AUTOCHECK))->SetWindowText(_T("�����ƶ��ɼ�"));
		break;
	}
}
//�˶����ƿ���ʼ��
void CAutoDeviceView::OnCardOpen()
{
	CString strErr = _T("");
	CString strTemp = _T("");
	WORD wRet = 0;
	CMenu *pMenu;
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMenu = pFrame->GetMenu()->GetSubMenu(0);
	pMenu->GetMenuString(ID_CONNECT, strTemp, 1);
	if (0 == strTemp.Compare(_T("���Ӱ忨")))
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
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_CONNECT, _T("�Ͽ��忨"));
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
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_CONNECT, _T("���Ӱ忨"));
		pFrame->DrawMenuBar();
		bConnect = FALSE;
	}
}
//���˶���������
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
	TCHAR szFilter[] = L"Access�ļ�(*.mdb)|*.mdb|�����ļ�(*.*)|*.*||";
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

	//���ƶ�������ԭ��λ��������һ��
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
	} while (d2410_check_done(Struct_Unite.axis_par.wXAxis) == 0 || d2410_check_done(Struct_Unite.axis_par.wYAxis) == 0 || d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0);//����������
	::DeleteObject(bmp1);//�ͷ���Դ
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


#pragma region �˶����ƿ�IO����
/*
��ȡĳһλ����ڵ�ƽ
* wBit �����λ��
* ���ض�Ӧλ�ĵ�ƽֵ �͵�ƽ��Ч
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
��ȡĳһλ����ڵ�ƽ
* wBit �����λ��
* ���ض�Ӧλ�ŵĵ�ƽֵ �͵�ƽ��Ч
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
����ĳһλ����ڵ�ƽ
* wBit �����λ��
* wBitValue �����Ӧλ�ŵĵ�ƽֵ 0=�͵�ƽ,1=�ߵ�ƽ  �͵�ƽ��Ч
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

/*������׵������½���ʱ��� 0ΪOK
*iTimeout       ��ʱʱ��
*iBitIn         ����λ��
*iBitOut        ���λ��
*iBitOutValue   ���λֵ 0=����, 1=�½�  �͵�ƽ��Ч
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

//��ʱ �����Ƿ��˳���ʱ
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

/*��ɫ�ƿ���
*wColour 0=��ɫ 1=��ɫ 2 =��ɫ
*wBitValue ���Ƶƿ��� 1=�� 0=�ر�
*���غ���ִ�еĴ������
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

/*ѭ���ȴ��������������벢�����ö�Ӧ�����
* wInNo  �����
* wOutNo �����
* wOutValue ���ֵ
* ���غ���ִ�еĽ��
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

/*ѭ���ȴ��������������벢�����ö�Ӧ�������WaitAndControl���������ж��෴
* wInNo  �����
* wOutNo �����
* wOutValue ���ֵ
* ���غ���ִ�еĽ��
*/
WORD CAutoDeviceView::NotWaitAndControl(WORD wInNo, WORD wOutNo, WORD wOutValue)
{
	WORD wRet = 0;
	do 
	{
		//if (GetOneBitIn(IN8))
		//{
		//	wRet = SetOneBitOut(OUT9, HIGHLEVEL);//����λ��û�а�ֹͣ����
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

/*ѭ���ȴ����������Ƿ��⵽��⵽��ʱȻ��ֹͣ����
* wInNo  �����
* wOutNo �����
* wOutValue ���ֵ
* ���غ���ִ�еĽ��
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
*wInNo  ����� ����Ƿ�������
* ���غ���ִ�еĽ��
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
*wInNo  ����� ����Ƿ�û������
* ���غ���ִ�еĽ��
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


#pragma region �������
void CAutoDeviceView::OnCameraOpen()
{
	CString strErr = _T("");
	CString strTemp = _T("");
	WORD wRet = 0;
	CMenu *pMenu;
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMenu = pFrame->GetMenu()->GetSubMenu(1);
	pMenu->GetMenuString(ID_OPENCAMERA, strTemp, 1);
	if (0 == strTemp.Compare(_T("�����")))
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
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_OPENCAMERA, _T("�ر����"));
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
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_OPENCAMERA, _T("�����"));
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

	if (0 == strTemp.Compare(_T("�����ɼ�")))
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
		pMenu->ModifyMenu(4, MF_BYPOSITION, ID_GRAB, _T("ֹͣ�ɼ�"));
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
		pMenu->ModifyMenu(4, MF_BYPOSITION, ID_GRAB, _T("�����ɼ�"));
		pFrame->DrawMenuBar();
	}
}
void CAutoDeviceView::OnCameraExec()
{
	CString strTemp = _T("");
	CString strErr = _T("");
	WORD wRet = 0;
	sType = 1;
	::DeleteObject(bmp1);//�ͷ���Դ
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
	//strPath +=_T("�����ļ�");
	//strPath +='\\';
	CFileDialog FileDlg(FALSE,_T("bmp"),NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,_T("bmp Files(*.bmp)|*.bmp|All Files|*.*||"),this);
	FileDlg.m_ofn.lpstrTitle = _T("Save As File");
	//FileDlg.m_ofn.lpstrInitialDir = strPath;
	if (IDOK==FileDlg.DoModal())
	{
		if (!SaveNewImage(bmp1, FileDlg.GetPathName(), 8))
		{
			MessageBox(_T("����ʧ��"), ALERT_ERR, ALERT_ICON);
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
	//strPath +=_T("�����ļ�");
	//strPath +='\\';
	CFileDialog FileDlg(TRUE,_T("bmp"),NULL,OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,_T("bmp Files(*.bmp)|*.bmp|All Files|*.*||"),this);
	FileDlg.m_ofn.lpstrTitle = _T("Open File");
	//FileDlg.m_ofn.lpstrInitialDir = strPath;
	if (IDOK==FileDlg.DoModal())
	{
		::DeleteObject(bmp1);//�ͷ���Դ
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
//�ɼ�ͼ��
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
			} while (d2410_check_done(Struct_Unite.axis_par.wXAxis) == 0 || d2410_check_done(Struct_Unite.axis_par.wYAxis) == 0 || d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0);//����������
			::DeleteObject(bmp1);//�ͷ���Դ
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

#pragma region ���̼������ʾ
//�����1=OK��0=NG
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
	wRet = MoveSafeCoor();//�ƶ�����ȫλ��
	if (wRet)
	{
		Err_Info(wRet, strErr);
		MessageBox(strErr, ALERT_ERR, ALERT_ICON);
		return;
	}
	//if (!GetOneBitIn(IN11))
	//{
	//	MessageBox(_T("δ��⵽��ѹ,��ȷʵ�Ƿ��Ѿ���ȷ���Ӳ����Ѿ��򿪣�"), ALERT_ERR, ALERT_ICON);
	//	return;
	//}
	//��ȫ����λ�������ף��а�����
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
	if (0 == Struct_Unite.axis_par.wDirect) //������
	{
		//��������Ƿ��а�
		do 
		{
			DoEvents();
		} while (!pView->GetOneBitIn(IN8));
		pView->UserPostMessage(_T("����λ���а�"), 1);
		if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))//������λ��������λ���а����͵�����λ��
		{
			wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);//�����������
			if (wRet)
			{
				goto ErrInfo;
			}
			//�������е�����λ��ֹͣ�ȴ�ȡ��
			wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			pView->UserPostMessage(_T("����λ���а�"), 1);
		}
		do 
		{
			DoEvents();
		} while (!pView->GetOneBitIn(IN8));//������λ�ð����Ƿ�����
		SETTIMEOUT(ENTERTOT); //һ��ʱ����û�м�鵽���ӽ���λ�ã�ȡ�����嶯��
		pView->UserPostMessage(_T("�ȴ�����"), 1);
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
		} while (pView->GetOneBitIn(IN7));//������λ���Ƿ��������
		pView->UserPostMessage(_T("����λ���а�"), 1);
		if (!pView->GetOneBitIn(IN7))//�а�����
		{
			wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			//�������׶���
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
		if (!pView->GetOneBitIn(IN10))//��⵽����
		{
			pView->WaitTime(SLEEPTIME);
			wRet = pView->SetOneBitOut(OUT9, HIGHLEVEL);//ֹͣ����
			if (wRet)
			{
				goto ErrInfo;
			}
			//�а����׶���
			wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			pView->UserPostMessage(_T("�ҵ���λ���а�"), 1);
			goto ErrInfo;
		}
	}
	else if (1 == Struct_Unite.axis_par.wDirect) //�Ҳ����
	{
		//��������Ƿ��а�
		do 
		{
			DoEvents();
		} while (!pView->GetOneBitIn(IN7));
		pView->UserPostMessage(_T("����λ���а�"), 1);
		if (!pView->GetOneBitIn(IN8) || !pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))//������λ��������λ���а����͵�����λ��
		{
			wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);//������䷴��
			if (wRet)
			{
				goto ErrInfo;
			}
			//�������е�����λ��ֹͣ�ȴ�ȡ��
			wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			pView->UserPostMessage(_T("����λ���а�"), 1);
		}
		do 
		{
			DoEvents();
		} while (!pView->GetOneBitIn(IN7));//������λ�ð����Ƿ�����
		SETTIMEOUT(ENTERTOT); //һ��ʱ����û�м�鵽���ӽ���λ�ã�ȡ�����嶯��
		pView->UserPostMessage(_T("�ȴ�����"), 1);
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
		} while (pView->GetOneBitIn(IN8));//������λ���Ƿ��������
		pView->UserPostMessage(_T("����λ���а�"), 1);
		if (!pView->GetOneBitIn(IN8))//�а�����
		{
			wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			//�������׶���
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
		if (!pView->GetOneBitIn(IN9))//��⵽����
		{
			pView->WaitTime(SLEEPTIME);
			wRet = pView->SetOneBitOut(OUT10, HIGHLEVEL);//ֹͣ����
			if (wRet)
			{
				goto ErrInfo;
			}
			//�а����׶���
			wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
			if (wRet)
			{
				goto ErrInfo;
			}
			pView->UserPostMessage(_T("�󵲰�λ���а�"), 1);
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
		if (0 == wAutoType)//�����Զ�����
		{
			if (0 == Struct_Unite.axis_par.wDirect)//�����
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
					pView->WaitTime(50);//������λ����ʱ
				}
				//����λ����û�а�����һֱ�ȴ����ӽ���
				pView->UserPostMessage(_T("�ȴ�����"),1);
				wRet = pView->WaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//��Ӧ�����Ӻ�����ӵ���Ӧλ��
				if (!pView->GetOneBitIn(IN7))
				{
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//����������δ�������𵲰�����
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
				pView->UserPostMessage(_T("�ҵ���λ���а�"),1);
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				pView->WaitTime(1500);
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//�ҽ���
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
					pView->WaitTime(50);//������λ����ʱ
				}
				//����λ����û�а�����һֱ�ȴ����ӽ���
				pView->UserPostMessage(_T("�ȴ�����"),1);
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
				//��Ӧ�����Ӻ�����ӵ���Ӧλ��
				wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
				if (wRet)
				{
					pView->SetOneBitOut(OUT10, HIGHLEVEL);
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����������δ�������𵲰�����
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
				pView->UserPostMessage(_T("�󵲰�λ���а�"),1);
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				pView->WaitTime(1500);
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
		}
		else if (1 == wAutoType)//�����ֶ�����
		{
			if (0 == Struct_Unite.axis_par.wDirect)//�����
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//������λ���а���������ʹ���Ӧ���������·Ű嵽����λ��
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����λ����û�а�����һֱ�ȴ����ӽ���
				pView->UserPostMessage(_T("�ȴ�����"),1);
				wRet = pView->WaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����������δ�������𵲰�����
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
				//��Ӧ�����Ӻ�����ӵ���Ӧλ��
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
				pView->UserPostMessage(_T("�ҵ���λ���а�"),1);
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				pView->WaitTime(1500);
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//�ҽ���
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//������λ���а���������ʹ���Ӧ���������·Ű嵽����λ��
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����λ����û�а�����һֱ�ȴ����ӽ���
				pView->UserPostMessage(_T("�ȴ�����"),1);
				wRet = pView->WaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����������δ�������𵲰�����
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
				//��Ӧ�����Ӻ�����ӵ���Ӧλ��
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
				pView->UserPostMessage(_T("�󵲰�λ���а�"),1);
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				pView->WaitTime(1500);
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
		}
		else if (2 == wAutoType)//�ѻ��Զ����
		{
			if (0 == Struct_Unite.axis_par.wDirect)//�����
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//������λ�õİ����Ƿ�����
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������λ���а���������ʹ���Ӧ���������·Ű嵽����λ��
				pView->UserPostMessage(_T("����λ���а�"),1);
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����λ����û�а�����һֱ�ȴ����ӽ���
				pView->UserPostMessage(_T("�ȴ�����"),1);
				wRet = pView->WaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����������δ�������𵲰�����
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
				//��Ӧ�����Ӻ�����ӵ���Ӧλ��
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
				pView->UserPostMessage(_T("�ҵ���λ���а�"),1);
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				pView->WaitTime(1500);
				//���OK�󵲰������½�
				wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//�ҽ���
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//������λ�õİ����Ƿ�����
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������λ���а���������ʹ���Ӧ���������·Ű嵽����λ��
				pView->UserPostMessage(_T("����λ���а�"),1);
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����λ����û�а�����һֱ�ȴ����ӽ���
				pView->UserPostMessage(_T("�ȴ�����"),1);
				wRet = pView->WaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����������δ�������𵲰�����
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
				//��Ӧ�����Ӻ�����ӵ���Ӧλ��
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
				pView->UserPostMessage(_T("�󵲰�λ���а�"),1);
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				pView->WaitTime(1500);
				//���OK�󵲰������½�
				wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
		}
		else if (3 == wAutoType)//�����Զ����
		{
			if (0 == Struct_Unite.axis_par.wDirect)//�����
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
					//�����ڳ���λ�÷������źţ����ȴ���λ��Ҫ��
					wRet = pView->SetOneBitOut(OUT5, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ���а�Ҫ��"),1);
					wRet = pView->WaitAndControl(IN13, OUT5, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//���Ͱ���
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
				//������λ�õİ����Ƿ�����
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//ͨ����û�а���׼��Ҫ�����
				if (pView->GetOneBitIn(IN7) && pView->GetOneBitIn(IN8)
					&& pView->GetOneBitIn(IN9) && pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);//Ҫ��
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ��Ҫ��"),1);
					wRet = pView->WaitLoop(IN12);//�����λ��Ҫ��
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					if (!pView->GetOneBitIn(IN12))
					{
						wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);//����
						if (wRet)
						{
							pView->SetOneBitOut(OUT9, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->SetOneBitOut(OUT4, HIGHLEVEL);//ֹͣҪ��
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
						//����������δ�������𵲰�����
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
					pView->UserPostMessage(_T("�ҵ���λ���а�"),1);
				}
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				pView->WaitTime(1500);
				//���OK�󵲰������½�
				wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//�ҽ���
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
					//�����ڳ���λ�÷������źţ����ȴ���λ��Ҫ��
					wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ���а�Ҫ��"),1);
					wRet = pView->WaitAndControl(IN12, OUT4, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//���Ͱ���
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
				//������λ�õİ����Ƿ�����
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//ͨ����û�а���׼��Ҫ�����
				if (pView->GetOneBitIn(IN7) && pView->GetOneBitIn(IN8) &&
					pView->GetOneBitIn(IN9) && pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT5, LOWLEVEL);//Ҫ��
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ��Ҫ��"),1);
					wRet = pView->WaitLoop(IN13);//�����λ��Ҫ��
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					if (!pView->GetOneBitIn(IN13))
					{
						wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);//����
						if (wRet)
						{
							pView->SetOneBitOut(OUT10, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->SetOneBitOut(OUT5, HIGHLEVEL);//ֹͣҪ��
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						//����������δ�������𵲰�����
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
					pView->UserPostMessage(_T("�󵲰�λ���а�"),1);
				}
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				pView->WaitTime(1500);
				//���OK�󵲰������½�
				wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
			}
		}
		else if (4 == wAutoType)//ֱͨ�Զ�����
		{
			if (0 == Struct_Unite.axis_par.wDirect)//�����
			{
				//���ȼ��ͨ�����Ƿ��а���
				if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN8) ||
					!pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					//������λ��û�а���,���Ȱ�����λ�õİ��Ӵ��͵�����λ��
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
						pView->UserPostMessage(_T("����λ���а�"),1);
					}
					//�����ڳ���λ�÷������źţ����ȴ���λ��Ҫ��
					wRet = pView->SetOneBitOut(OUT5, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ���а�Ҫ��"),1);
					wRet = pView->WaitAndControl(IN13, OUT5, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//���Ͱ���
					wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT9, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//������λ�õİ����Ƿ�����
					wRet = pView->NotWaitAndControl(IN8, OUT9, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//������λ�õİ����Ƿ�����
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//ͨ����û�а���׼��Ҫ�����
				if (pView->GetOneBitIn(IN7) && pView->GetOneBitIn(IN8) &&
					pView->GetOneBitIn(IN9) && pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);//Ҫ��
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ��Ҫ��"),1);
					wRet = pView->WaitLoop(IN12);//�����λ��Ҫ��
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					if (!pView->GetOneBitIn(IN12))
					{
						wRet = pView->SetOneBitOut(OUT9, LOWLEVEL);//����
						if (wRet)
						{
							pView->SetOneBitOut(OUT9, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);//ֹͣҪ��
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
					}
					wRet = pView->WaitAndControl(IN8, OUT9, HIGHLEVEL);//�����⵽
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ���а�"),1);
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//�ҽ���
			{
				//���ȼ��ͨ�����Ƿ��а���
				if (!pView->GetOneBitIn(IN7) || !pView->GetOneBitIn(IN8) ||
					!pView->GetOneBitIn(IN9) || !pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					//������λ��û�а��ӣ����Ȱ�����λ�õİ��Ӵ��͵�����λ��
					if (pView->GetOneBitIn(IN7))
					{
						wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
						if (wRet)
						{
							pView->SetOneBitOut(OUT10, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);//�������Ƿ񵽳���λ��
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						pView->UserPostMessage(_T("����λ���а�"),1);
					}
					//�����ڳ���λ�÷������źţ����ȴ���λ��Ҫ��
					wRet = pView->SetOneBitOut(OUT4, LOWLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ���а�Ҫ��"),1);
					wRet = pView->WaitAndControl(IN12, OUT4, HIGHLEVEL);//��⵽�а�Ҫ���ź�,�رճ����ź�
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//���Ͱ���
					wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);
					if (wRet)
					{
						pView->SetOneBitOut(OUT10, HIGHLEVEL);
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					//������λ�õİ����Ǵ�����
					wRet = pView->NotWaitAndControl(IN7, OUT10, HIGHLEVEL);
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
				}
				//������λ�õİ����Ƿ�����
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//ͨ����û�а���׼��Ҫ�����
				if (pView->GetOneBitIn(IN7) && pView->GetOneBitIn(IN8) &&
					pView->GetOneBitIn(IN9) && pView->GetOneBitIn(IN10))
				{
					if (!pView->bAutoFlag)
					{
						bLoops = FALSE;
						wRet = AUTOEXIT;
						goto ThreadErrInfo;
					}
					wRet = pView->SetOneBitOut(OUT5, LOWLEVEL);//Ҫ��
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ��Ҫ��"),1);
					wRet = pView->WaitLoop(IN13);//�����λ��Ҫ��
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					if (!pView->GetOneBitIn(IN13))
					{
						wRet = pView->SetOneBitOut(OUT10, LOWLEVEL);//����
						if (wRet)
						{
							pView->SetOneBitOut(OUT10, HIGHLEVEL);
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
						wRet = pView->SetOneBitOut(OUT5, HIGHLEVEL);//ֹͣҪ��
						if (wRet)
						{
							bLoops = FALSE;
							goto ThreadErrInfo;
						}
					}
					wRet = pView->WaitAndControl(IN7, OUT10, HIGHLEVEL);//���䵽����λ�ò�ֹͣ
					if (wRet)
					{
						bLoops = FALSE;
						goto ThreadErrInfo;
					}
					pView->UserPostMessage(_T("����λ���а�"),1);
				}
			}
		}
		else if (5 == wAutoType)//�����ƶ��ɼ�
		{
			if (0 == Struct_Unite.axis_par.wDirect)//�����
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//������λ�õİ����Ƿ�����
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������λ���а���������ʹ���Ӧ���������·Ű嵽����λ��
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("�ȴ�����"),1);
				//����λ����û�а�����һֱ�ȴ����ӽ���
				wRet = pView->WaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����������δ�������𵲰�����
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
				//��Ӧ�����Ӻ�����ӵ���Ӧλ��
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
				pView->UserPostMessage(_T("�ҵ���λ���а�"),1);
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				//pView->WaitTime(1500);
				wRet = pView->CollectImage();
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//���OK�󵲰������½�
				wRet = pView->SetAndGetIO(TIME_OUT, LEFTIN4, LEFTOUT6, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//Ȼ������ӵ�����λ��ֹͣ�Զ�����
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
				pView->UserPostMessage(_T("����λ���а�"),1);
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//�ҽ���
			{
				//���ȼ�������λ�ã���������λ���Ƿ��а�,���аѰ��Ӵ��͵�����λ�ô�
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
					pView->UserPostMessage(_T("����λ���а�"),1);
				}
				if (!pView->bAutoFlag)
				{
					bLoops = FALSE;
					wRet = AUTOEXIT;
					goto ThreadErrInfo;
				}
				//������λ�õİ����Ƿ�����
				wRet = pView->NotWaitLoop(IN7);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������λ���а���������ʹ���Ӧ���������·Ű嵽����λ��
				wRet = pView->NotWaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				pView->UserPostMessage(_T("�ȴ�����"),1);
				//����λ����û�а�����һֱ�ȴ����ӽ���
				wRet = pView->WaitLoop(IN8);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//����������δ�������𵲰�����
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
				//��Ӧ�����Ӻ�����ӵ���Ӧλ��
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
				pView->UserPostMessage(_T("�󵲰�λ���а�"),1);
				//��λ��а����׶���
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, LOWLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//������ʼ����
				pView->WaitTime(1500);
				//wRet = pView->CollectImage();
				//if (wRet)
				//{
				//	bLoops = FALSE;
				//	goto ThreadErrInfo;
				//}
				//���OK�󵲰������½�
				wRet = pView->SetAndGetIO(TIME_OUT, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//���OK��а������½�
				wRet = pView->SetAndGetIO(TIME_OUT, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
				if (wRet)
				{
					bLoops = FALSE;
					goto ThreadErrInfo;
				}
				//Ȼ������ӵ�����λ�ò�ֹͣ�Զ�����
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
				pView->UserPostMessage(_T("����λ���а�"),1);
			}
			bLoops = FALSE;//ִֻ��һ��
		}
		if (!wRet)//�������
		{
			wRet = MoveSafeCoor();//�ƶ�����ȫλ��
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
			pView->bAutoFlag = TRUE;//����������˳����ʼ���Զ����б�־
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

//״̬����ʾ����
void CAutoDeviceView::OnStsBarMsg()
{
	CRect rc;
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if (pFrame->m_wndStatusBar.IsWindowVisible())
	{
		pFrame->ShowControlBar(&pFrame->m_wndStatusBar, FALSE, FALSE);//�ر� 	
	}
	else
	{
		pFrame->ShowControlBar(&pFrame->m_wndStatusBar, TRUE, FALSE);//��ʾ 
	}
	m_StrReal_Image.GetClientRect(&rc);
	m_StrReal_Image.ClientToScreen(&rc);
	InvalidateRect(rc); 
}

//�߳��и���״̬����Ϣ��ʾ
//lParam =0�����Ϣ, 1=ʵʱ��Ϣ��ʾ
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

//�����Զ�����Ϣ
//strMsg��Ϣ����
//��Ϣ���� 0 = �����Ϣ, 1=ʵʱ��Ϣ
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
			pMenu->GetMenuString(ID_GRAB, strTemp, 1);//ϵͳ�˵�����
			//pMenu = pFrame->GetMenu()->GetSubMenu(2);
			pMenu->GetMenuString(ID_OPENCAMERA, strTemp1, 1);//ϵͳ�˵�����

			CMenu menu;
			menu.CreatePopupMenu();
			menu.AppendMenu(MF_STRING, ID_OPENCAMERA, strTemp1);
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_CAMERAPAR, _T("�������"));
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_GRAB, strTemp);
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_EXEC, _T("ִ������"));
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_SAVEIMAGE, _T("����ͼƬ"));
			menu.AppendMenu(MF_SEPARATOR);
			menu.AppendMenu(MF_STRING, ID_LOADIMAGE, _T("����ͼƬ"));
			CPoint pt;
			GetCursorPos(&pt);
			menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
			menu.DestroyMenu();		

			//CMenu menu; //��������Ҫ�õ���cmenu����
			//menu.LoadMenu(IDR_MAINFRAME); //װ���Զ�����Ҽ��˵�
			//CMenu *pContextMenu=menu.GetSubMenu(2); //��ȡ��2�������˵�,���Ե�2���˵��������Ӳ˵�
			//CPoint pt;
			//GetCursorPos(&pt);
			//ClientToScreen(&pt);
			//pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this);
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}
