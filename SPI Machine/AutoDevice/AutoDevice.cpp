
// AutoDevice.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "AutoDevice.h"
#include "MainFrm.h"

#include "AutoDeviceDoc.h"
#include "AutoDeviceView.h"
#include "include/DMC2410.h"
#include "include/Lts_Dll.h"
#include "AxisZeroDlg.h"
#include "MeasureView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoDeviceApp

BEGIN_MESSAGE_MAP(CAutoDeviceApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CAutoDeviceApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	//ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CAutoDeviceApp ����

CAutoDeviceApp::CAutoDeviceApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("AutoDevice.AppID.NoVersion"));

	m_bEnglish = TRUE;//Ĭ�ϵ�������

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CAutoDeviceApp ����
CAutoDeviceApp theApp;

STRUCT_UNITE Struct_Unite;
BOOL m_bEnglish;
HANDLE m_hCom;

//�������
Camera_t* camera1;

//��һ����ʽ���������
CInstantCamera* camera2;
HBITMAP bmp1 = NULL;
HBITMAP bmp2 = NULL;
CPylonImage targetImage1;
CPylonBitmapImage destimage1;
CPylonImage targetImage2;
CPylonBitmapImage destimage2;

//Pylon SDK�е�ͼ��ת������ (bayerת��),�ڰ���������õ���
CImageFormatConverter converter;//ת�����ߡ�
short sType = 0;

//IplImage* g_pImg;
//float g_nPcbWidth;
//float g_nPcbHeight;
//Point2f g_robotPoint;

//����ָ����Ұ����ѡ��ĺ���λ��
vector<RectF> g_vecWeldingPlateRect;

// CAutoDeviceApp ��ʼ��

//NONCLIENTMETRICS m_ncmOld;  
//NONCLIENTMETRICS m_ncmNew;  

BOOL CAutoDeviceApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(6);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAutoDeviceDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CAutoDeviceView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����

	m_hCom = 0;

	//CAxisZeroDlg Dlg;
	//Dlg.bFirst = TRUE;
	//Dlg.bZero = TRUE;
	//Dlg.DoModal();
	
	return TRUE;
}

int CAutoDeviceApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);
	//::SystemParametersInfo(SPI_SETNONCLIENTMETRICS, m_ncmOld.cbSize, &m_ncmOld, 0);  

	return CWinApp::ExitInstance();
}

// CAutoDeviceApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()

};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CAutoDeviceApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAutoDeviceApp ��Ϣ�������


CString GetFilePath()
{
	TCHAR   szSettingFile[256] = _T("");
	CString strSettingFile;
	::GetModuleFileName(NULL, szSettingFile, sizeof(szSettingFile));
	strSettingFile = CString(szSettingFile);
	int nPos = strSettingFile.ReverseFind('\\');
	return strSettingFile = strSettingFile.Mid(0, nPos + 1);
}

//function: charTowchar 
//purpose:char to WCHAR ��wchar_t��LPWSTR etc 
void charTowchar(const char *chr, wchar_t *wchar, int size)   
{      
	MultiByteToWideChar( CP_ACP, 0, chr,   
		strlen(chr)+1, wchar, size/sizeof(wchar[0]) );   
}   

//function: wcharTochar 
//purpose:WCHAR ��wchar_t��LPWSTR to char 
void wcharTochar(const wchar_t *wchar, char *chr, int length)   
{   
	WideCharToMultiByte( CP_ACP, 0, wchar, -1,   
		chr, length, NULL, NULL );   
}

void DoEvents() 
{ 
	MSG msg; 
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
	{ 
		{ 
			GetMessage(&msg, NULL, 0, 0); 
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		} 
	} 
} 

void ReadFileValue(FILE* pf, CString& strValue)
{
	CHAR szTmp[128] = "";
	TCHAR szWTmp[128] = _T("");
	CString strTemp1 = _T("");

	memset(szTmp, 0, sizeof(szTmp));
	memset(szWTmp, 0, sizeof(szWTmp));
	fgets(szTmp, sizeof(szTmp), pf);
	charTowchar(szTmp, szWTmp, sizeof(szWTmp));
	strTemp1 = CString(szWTmp);
	strValue=strTemp1.Mid(strTemp1.Find(_T(":"))+1).Trim();
}

void ReadConFileValue(FILE* pf, CString& strValue)
{
	CHAR szTmp[128] = "";
	TCHAR szWTmp[128] = _T("");
	CString strTemp1 = _T("");

	memset(szTmp, 0, sizeof(szTmp));
	memset(szWTmp, 0, sizeof(szWTmp));
	fgets(szTmp, sizeof(szTmp), pf);
	charTowchar(szTmp, szWTmp, sizeof(szWTmp));
	strTemp1 = CString(szWTmp);
	strValue=strTemp1.Trim();
}

void Err_Info(int Err_Code, CString& Err_Info)
{
	switch (Err_Code)
	{
	case 1:
		Err_Info = _T("δ֪����");
		break;
	case 2:
		Err_Info = _T("��������");
		break;
	case 3:
		Err_Info = _T("������ʱ");
		break;
	case 4:
		Err_Info = _T("���ƿ�״̬æ");
		break;
	case 9:
		Err_Info = _T("���Ŵ���");
		break;
	case 10:
		Err_Info = _T("���ݴ������,����PCI����Ƿ��ɶ�");
		break;
	case 20:
		Err_Info = _T("���������ز�������");
		break;
	case 22:
		Err_Info = _T("���������ص�ǰ״̬���������");
		break;
	case 24:
		Err_Info = _T("��������֧�ֵĹ���");
		break;
	case OPENCOMERR:
		Err_Info = _T("�򿪴���ʧ��,���鴮���Ƿ���ȷ���ߴ����Ѿ���ȷ���Ӳ��ϵ�");
		break;
	case CONNECTERR:
		Err_Info = _T("û���ҵ����ƿ�,���߿��ƿ��쳣");
		break;
	case RUNING:
		StopMoveAxis();
		Err_Info = _T("Ҫ�������ᴦ���˶�״̬��");
		break;
	case NOCAMERA:
		Err_Info = _T("û���ҵ����,��ȷ������Ƿ�װ����ȷ����");
		break;
	case NOOPEN:
		Err_Info = _T("�����δ��,��������ӹ���");
		break;
	case NOGRAB:
		Err_Info = _T("���û�п�ʼ�ɼ�");
		break;
	case TIMEOUT:
		Err_Info = _T("������ճ�ʱ");
		break;
	case ISOPENED:
		Err_Info = _T("Ҫ����������Ѿ���");
		break;
	case LEFTDOWNTOUT:
		Err_Info = _T("��൲�������½���ʱ");
		break;
	case LEFTUPTOUT:
		Err_Info = _T("��൲������������ʱ");
		break;
	case RIGHTDOWNTOUT:
		Err_Info = _T("�Ҳ൲�������½���ʱ");
		break;
	case RIGHTUPTOUT:
		Err_Info = _T("�Ҳ൲������������ʱ");
		break;
	case MIDDLEDOWNTOUT:
		Err_Info = _T("�а�������½���ʱ");
		break;
	case MIDDLEUPTOUT:
		Err_Info = _T("�а�����������ʱ");
		break;
	case AUTOEXIT:
		Err_Info = _T("�˳��Զ�����");
		break;
	case RAILBARRIER:
		Err_Info = _T("������а��ӣ���ȡ������ڵİ���");
		break;
	case DOOROPEN:
		Err_Info = _T("�豸����δ�ر�,���ȹر��豸��");
		break;
	case SAVEFAIL:
		Err_Info = _T("����ͼƬʧ��");
		break;
	case ENTERTIMEOUT:
		Err_Info = _T("�ȴ����峬ʱ,ȡ������״̬");
		break;
	case Bad_CommOpen:
		Err_Info = _T("��Դ���ڴ�ʧ��");
		break;
	case Bad_CommSet:
		Err_Info = _T("��Դ��������ʧ��");
		break;
	case Bad_CommTimeouts:
		Err_Info = _T("��Դ���ڶ�д��ʱ���ô���");
		break;
	case Bad_CommQueue:
		Err_Info = _T("��Դ���ڻ�������������");
		break;
	case Bad_CommRead:
		Err_Info = _T("��Դ���ڶ����ݳ���");
		break;
	case Bad_CommWrite:
		Err_Info = _T("��Դ����д���ݳ�ʱ");
		break;
	case PACKET_Timeout:
		Err_Info = _T("��Դ���ڶ�ȡ���ݳ�ʱ");
		break;
	case Bad_CommClose:
		Err_Info = _T("���ڹر�ʧ��");
		break;
	case -1:
		Err_Info = _T("��Դ����ͨѶʧ��");
		break;
	}
}

/* �����˶� 
* posi_mode = 0 ����˶�ģʽ, 1�����˶�ģʽ
* stType = 0 S�ͽ�������, 1 T�ͽ�������
*/
WORD Axis_Move(WORD axis, double Min_Vel, double Max_Vel, double Tacc, double Tdec
	, long Tsacc, long Tsdec, double Dist, WORD posi_mode, WORD stType)
{
	long lCovDist = 0;
	double dCovMin_Vel = 0.000, dCovMax_Vel = 0.000;
	WORD wRet = 0;
	switch (axis)
	{
	case 0:
		lCovDist = Dist * Struct_Unite.axis_par.dXPulsePer/Struct_Unite.axis_par.dXPitch;
		dCovMin_Vel = Min_Vel * Struct_Unite.axis_par.dXPulsePer/Struct_Unite.axis_par.dXPitch;
		dCovMax_Vel = Max_Vel * Struct_Unite.axis_par.dXPulsePer/Struct_Unite.axis_par.dXPitch;
		break;
	case 1:
		lCovDist = Dist * Struct_Unite.axis_par.dYPulsePer/Struct_Unite.axis_par.dYPitch;
		dCovMin_Vel = Min_Vel * Struct_Unite.axis_par.dYPulsePer/Struct_Unite.axis_par.dYPitch;
		dCovMax_Vel = Max_Vel * Struct_Unite.axis_par.dYPulsePer/Struct_Unite.axis_par.dYPitch;
		break;
	case 2:
		lCovDist = Dist * Struct_Unite.axis_par.dZPulsePer/Struct_Unite.axis_par.dZPitch;
		dCovMin_Vel = Min_Vel * Struct_Unite.axis_par.dZPulsePer/Struct_Unite.axis_par.dZPitch;
		dCovMax_Vel = Max_Vel * Struct_Unite.axis_par.dZPulsePer/Struct_Unite.axis_par.dZPitch;
		break;
	case 3:
		lCovDist = Dist * Struct_Unite.axis_par.dWPulsePer/Struct_Unite.axis_par.dWPitch;
		dCovMin_Vel = Min_Vel * Struct_Unite.axis_par.dWPulsePer/Struct_Unite.axis_par.dWPitch;
		dCovMax_Vel = Max_Vel * Struct_Unite.axis_par.dWPulsePer/Struct_Unite.axis_par.dWPitch;
		break;
	}
	if (d2410_check_done(axis) == 0) //�Ѿ����˶���
		return RUNING; 
	if (stType == 0)
	{
		//�趨S�ν��������
		wRet = d2410_set_st_profile(axis, dCovMin_Vel, dCovMax_Vel, Tacc, Tdec, Tsacc, Tsdec);
		if (wRet) return wRet;
		wRet = d2410_s_pmove(axis, lCovDist, posi_mode);
		if (wRet) return wRet;
	}
	else if (stType == 1)
	{
		//�趨T�ν��������
		wRet = d2410_set_profile(axis, dCovMin_Vel, dCovMax_Vel, Tacc, Tdec);
		if (wRet) return wRet;
		wRet = d2410_ex_t_pmove(axis, lCovDist, posi_mode);
		if (wRet) return wRet;
	}
	return wRet;
}

/* �����ԭ�� 
* stType = 0 S�ͽ�������, 1 T�ͽ�������
* org_logic = 0 �͵�ƽ��Ч��1 �ߵ�ƽ��Ч  ORG �źŵ���Ч��ƽ
* EZ_logic Ez�ź���Ч��ƽ 0 ����Ч, 1 ����Ч
* Mode ��ԭ����ź�ģʽ��
* 0��ֻ��home
* 1����home ��EZ����1 ��EZ �ź�
* 2��һ�λ���ӻ���
* 3�����λ���
* 4��EZ ��������
* 5��ԭ�㲶�����
* EZ_count EZ �źų���EZ_count ָ���Ĵ��������˶�ֹͣ������mode=4 ʱ��
* ����������Ч��ȡֵ��Χ��1��16
* Home_Mode ��ԭ�㷽ʽ��1���������ԭ�㣬2���������ԭ��
* Vel_Mode ��ԭ���ٶȣ�0�����ٻ�ԭ�㣬1�����ٻ�ԭ��
*/
WORD Axis_Zero(WORD axis, double Min_Vel, double Max_Vel, double Tacc, double Tdec
	, long Tsacc, long Tsdec, WORD stType, WORD org_logic, WORD EZ_logic, WORD Mode,WORD EZ_count, WORD Home_Mode, WORD Vel_Mode)
{
	double dCovMin_Vel = 0.000, dCovMax_Vel = 0.000;
	WORD wRet = 0;
	switch (axis)
	{
	case 0:
		dCovMin_Vel = Min_Vel * Struct_Unite.axis_par.dXPulsePer/Struct_Unite.axis_par.dXPitch;
		dCovMax_Vel = Max_Vel * Struct_Unite.axis_par.dXPulsePer/Struct_Unite.axis_par.dXPitch;
		break;
	case 1:
		dCovMin_Vel = Min_Vel * Struct_Unite.axis_par.dYPulsePer/Struct_Unite.axis_par.dYPitch;
		dCovMax_Vel = Max_Vel * Struct_Unite.axis_par.dYPulsePer/Struct_Unite.axis_par.dYPitch;
		break;
	case 2:
		dCovMin_Vel = Min_Vel * Struct_Unite.axis_par.dZPulsePer/Struct_Unite.axis_par.dZPitch;
		dCovMax_Vel = Max_Vel * Struct_Unite.axis_par.dZPulsePer/Struct_Unite.axis_par.dZPitch;
		break;
	case 3:
		dCovMin_Vel = Min_Vel * Struct_Unite.axis_par.dWPulsePer/Struct_Unite.axis_par.dWPitch;
		dCovMax_Vel = Max_Vel * Struct_Unite.axis_par.dWPulsePer/Struct_Unite.axis_par.dWPitch;
		break;
	}
	if (d2410_check_done(axis) == 0) //�Ѿ����˶���
		return RUNING; 
	wRet = d2410_set_HOME_pin_logic(axis, org_logic, 1);
	if (wRet) return wRet;
	if (stType == 0)
	{
		//�趨S�ν��������
		wRet = d2410_set_st_profile(axis, dCovMin_Vel, dCovMax_Vel, Tacc, Tdec, Tsacc, Tsdec);
		if (wRet) return wRet;
	}
	else if (stType == 1)
	{
		//�趨T�ν��������
		wRet = d2410_set_profile(axis, dCovMin_Vel, dCovMax_Vel, Tacc, Tdec);
		if (wRet) return wRet;
	}
	wRet = d2410_config_EZ_PIN(axis, EZ_logic, 0);//����EZ���÷�ʽ
	if (wRet) return wRet;
	wRet = d2410_config_home_mode(axis, Mode, EZ_count);//���û��㷽ʽ
	if (wRet) return wRet;
	wRet = d2410_home_move(axis, Home_Mode, Vel_Mode);//���㶯��
	if (wRet) return wRet;
	//do 
	//{
	//	DoEvents();
	//} while (d2410_check_done(axis) == 0);//����������
	//if (axis != 3)
	//{
	//	wRet = d2410_set_position(axis, 0);
	//	if (wRet) return wRet;
	//}
	//else
	//{
	//	wRet = d2410_set_position(axis, axis_par.dWTrip*axis_par.dWPulsePer/axis_par.dWPitch);
	//	if (wRet) return wRet;
	//}

	return wRet;
}

WORD EnableLmt()
{
	WORD wRet = 0;
	wRet = d2410_set_pulse_outmode(Struct_Unite.axis_par.wXAxis, 1);
	if (wRet) return wRet;
	wRet = d2410_Enable_EL_PIN(Struct_Unite.axis_par.wXAxis, 1);
	if (wRet) return wRet;
	wRet = d2410_config_EL_MODE(Struct_Unite.axis_par.wXAxis, 2);
	if (wRet) return wRet;

	wRet = d2410_set_pulse_outmode(Struct_Unite.axis_par.wYAxis, 3);
	if (wRet) return wRet;
	wRet = d2410_Enable_EL_PIN(Struct_Unite.axis_par.wYAxis, 1);
	if (wRet) return wRet;
	wRet = d2410_config_EL_MODE(Struct_Unite.axis_par.wYAxis, 2);
	if (wRet) return wRet;

	wRet = d2410_set_pulse_outmode(Struct_Unite.axis_par.wZAxis, 3);
	if (wRet) return wRet;
	wRet = d2410_Enable_EL_PIN(Struct_Unite.axis_par.wZAxis, 1);
	if (wRet) return wRet;
	wRet = d2410_config_EL_MODE(Struct_Unite.axis_par.wZAxis, 0);
	if (wRet) return wRet;

	wRet = d2410_set_pulse_outmode(Struct_Unite.axis_par.wWAxis, 3);
	if (wRet) return wRet;
	wRet = d2410_Enable_EL_PIN(Struct_Unite.axis_par.wWAxis, 1);
	if (wRet) return wRet;
	wRet = d2410_config_EL_MODE(Struct_Unite.axis_par.wWAxis, 0);
	if (wRet) return wRet;

	return wRet;
}

/* ��ԭ�� 
* stType = 0 XYZWͬʱ, =1 XY��=2 Z =3 W
*/
WORD BackOrigin(short sType)
{
	WORD wRet = 0;
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pMain->GetActiveView();

	//if (0 == sType || 3 == sType)
	//{
	//	wRet = MoveSafeCoor();
	//	if (wRet) return wRet;
	//	wRet = CheckPcb();
	//	if (wRet) return wRet;
	//}
	pMainView->bZero = TRUE;
	if (/*0 == sType || */2 == sType)
	{
		wRet = Axis_Zero(Struct_Unite.axis_par.wZAxis, Struct_Unite.axis_par.dZMinVel, 
			Struct_Unite.axis_par.dZMaxVel, Struct_Unite.axis_par.dZTAcc, Struct_Unite.axis_par.dZTDec,
			Struct_Unite.axis_par.dZTs, 0, 0, 1, 0, 2, 1, 1, 1);
		if (wRet) return wRet;
		//do 
		//{
		//	DoEvents();
		//} while (d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0);//����������
		//wRet = d2410_set_position(Struct_Unite.axis_par.wZAxis, 0);
		//if (wRet) return wRet;
	}
	if (0 == sType)
	{
		wRet = Axis_Zero(Struct_Unite.axis_par.wZAxis, Struct_Unite.axis_par.dZMinVel, 
			Struct_Unite.axis_par.dZMaxVel, Struct_Unite.axis_par.dZTAcc, Struct_Unite.axis_par.dZTDec,
			Struct_Unite.axis_par.dZTs, 0, 0, 1, 0, 2, 1, 1, 1);
		if (wRet) return wRet;
	}
	if (0 == sType || 1 == sType)
	{
		wRet = Axis_Zero(Struct_Unite.axis_par.wXAxis, Struct_Unite.axis_par.dXMinVel, Struct_Unite.axis_par.dXMaxVel,
			Struct_Unite.axis_par.dXTAcc, Struct_Unite.axis_par.dXTDec,
			Struct_Unite.axis_par.dXTs, 0, 0, 1, 0, 2, 1, 1, 1);
		if (wRet) return wRet;
	}
	if (0 == sType || 1 == sType)
	{
		wRet = Axis_Zero(Struct_Unite.axis_par.wYAxis, Struct_Unite.axis_par.dYMinVel, Struct_Unite.axis_par.dYMaxVel,
			Struct_Unite.axis_par.dYTAcc, Struct_Unite.axis_par.dYTDec,
			Struct_Unite.axis_par.dYTs, 0, 0, 1, 0, 2, 1, 1, 1);
		if (wRet) return wRet;
	}
	if (0 == sType || 3 == sType)
	{
		wRet = Axis_Zero(Struct_Unite.axis_par.wWAxis, Struct_Unite.axis_par.dWMinVel, Struct_Unite.axis_par.dWMaxVel,
			Struct_Unite.axis_par.dWTAcc, Struct_Unite.axis_par.dWTDec,
			Struct_Unite.axis_par.dWTs, 0, 0, 1, 0, 2, 1, 1, 0);
		if (wRet) return wRet;
	}
	if (0 == sType)
	{
		do 
		{
			DoEvents();
		} while (d2410_check_done(Struct_Unite.axis_par.wXAxis) == 0
			|| d2410_check_done(Struct_Unite.axis_par.wYAxis) == 0
			|| d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0
			|| d2410_check_done(Struct_Unite.axis_par.wWAxis) == 0);//����������
	}
	else if (1 == sType)
	{
		do 
		{
			DoEvents();
		} while (d2410_check_done(Struct_Unite.axis_par.wXAxis) == 0
			|| d2410_check_done(Struct_Unite.axis_par.wYAxis) == 0);
	}
	else if (2 == sType)
	{
		do 
		{
			DoEvents();
		} while (d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0);
	}
	else if (3 == sType)
	{
		do 
		{
			DoEvents();
		} while (d2410_check_done(Struct_Unite.axis_par.wWAxis) == 0);
	}
	if (0 == sType || 1 == sType)
	{
		wRet = d2410_set_position(Struct_Unite.axis_par.wXAxis, 0);
		if (wRet) return wRet;
	}
	if (0 == sType || 1 == sType)
	{
		wRet = d2410_set_position(Struct_Unite.axis_par.wYAxis, 0);
		if (wRet) return wRet;
	}
	if (0 == sType || 3 == sType)
	{
		wRet = d2410_set_position(Struct_Unite.axis_par.wWAxis, 0);
		if (wRet) return wRet;
	}
	if (0 == sType || 2 == sType)
	{
		wRet = d2410_set_position(Struct_Unite.axis_par.wZAxis, 0);
		if (wRet) return wRet;
	}
	return wRet;
}

//���ͨ�����Ƿ��а忨�еĻ��͵�����λ�õȴ��û�ȡ��
WORD CheckPcb()
{
	WORD wRet = 0;
	WORD wML = 0, wMR = 0;
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pMain->GetActiveView();
	//��ȫ����λ�������ף��а�����
	wRet = pMainView->SetAndGetIO(SLEEPTIME, LEFTIN4, LEFTOUT6, HIGHLEVEL);
	if (wRet) return wRet;
	wRet = pMainView->SetAndGetIO(SLEEPTIME, RIGHTIN5, RIGHTOUT7, HIGHLEVEL);
	if (wRet) return wRet;
	wRet = pMainView->SetAndGetIO(SLEEPTIME, MIDDLEIN6, MIDDLEOUT8, HIGHLEVEL);
	if (wRet) return wRet;
	if (!pMainView->GetOneBitIn(IN7) || !pMainView->GetOneBitIn(IN8))
	{
		return RAILBARRIER;
	}
	if (!pMainView->GetOneBitIn(IN9) || !pMainView->GetOneBitIn(IN10))
	{
		do 
		{
			wML = pMainView->GetOneBitIn(IN9);
			wMR = pMainView->GetOneBitIn(IN10);
			DoEvents();
			if (0 == Struct_Unite.axis_par.wDirect)//������
			{
				if ((!wMR || !wML) && 0 == Struct_Unite.axis_par.wDirect)
				{
					wRet = pMainView->SetOneBitOut(OUT9, LOWLEVEL);//�����������
					if (wRet) return wRet;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//�Ҳ����
			{
				if ((!wMR || !wML) && 1 == Struct_Unite.axis_par.wDirect)
				{
					wRet = pMainView->SetOneBitOut(OUT10, LOWLEVEL);//������䷴��
					if (wRet) return wRet;
				}
			}
		} while (!wML || !wMR);
		//�������е�����λ��ֹͣ�ȴ�ȡ��
		if (0 == Struct_Unite.axis_par.wDirect)//������
		{
			do 
			{
			} while (pMainView->GetOneBitIn(IN8));
			if (!pMainView->GetOneBitIn(IN8))
			{
				wRet = pMainView->SetOneBitOut(OUT9, HIGHLEVEL);
				if (wRet) return wRet;
			}
		}
		else if (1 == Struct_Unite.axis_par.wDirect)//�Ҳ����
		{
			do 
			{
			} while (pMainView->GetOneBitIn(IN7));
			if (!pMainView->GetOneBitIn(IN7))
			{
				wRet = pMainView->SetOneBitOut(OUT10, HIGHLEVEL);
				if (wRet) return wRet;
			}
		}
		return RAILBARRIER;
	}
	return wRet;
}

//XYZͣ���İ�ȫλ��
WORD MoveSafeCoor()
{
	WORD wRet = 0;
	wRet = Axis_Move(Struct_Unite.axis_par.wXAxis, Struct_Unite.axis_par.dXMinVel, Struct_Unite.axis_par.dXMaxVel,
		Struct_Unite.axis_par.dXTAcc, Struct_Unite.axis_par.dXTDec,
		Struct_Unite.axis_par.dXTs, 0, Struct_Unite.as.dXSafe, 1, 0);
	if (wRet) return wRet;
	wRet = Axis_Move(Struct_Unite.axis_par.wYAxis, Struct_Unite.axis_par.dYMinVel, Struct_Unite.axis_par.dYMaxVel,
		Struct_Unite.axis_par.dYTAcc, Struct_Unite.axis_par.dYTDec,
		Struct_Unite.axis_par.dYTs, 0, Struct_Unite.as.dYSafe, 1, 0);
	if (wRet) return wRet;
	do 
	{
		DoEvents();
	} while (d2410_check_done(Struct_Unite.axis_par.wXAxis) == 0
		|| d2410_check_done(Struct_Unite.axis_par.wYAxis) == 0
		|| d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0);

	//wRet = Axis_Move(Struct_Unite.axis_par.wZAxis, Struct_Unite.axis_par.dZMinVel, Struct_Unite.axis_par.dZMaxVel,
	//	Struct_Unite.axis_par.dZTAcc, Struct_Unite.axis_par.dZTDec,
	//	Struct_Unite.axis_par.dZTs, 0, Struct_Unite.as.dZSafe, 1, 0);
	//if (wRet) return wRet;

	return wRet;
}

//���ֲ��������˶��еĴ���ʱ,����ֹͣ�������˶�����
void StopMoveAxis()
{
	if (d2410_check_done(Struct_Unite.axis_par.wXAxis) == 0)
	{
		d2410_imd_stop(Struct_Unite.axis_par.wXAxis);
	}
	else if (d2410_check_done(Struct_Unite.axis_par.wYAxis) == 0)
	{
		d2410_imd_stop(Struct_Unite.axis_par.wYAxis);
	}
	else if (d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0)
	{
		d2410_imd_stop(Struct_Unite.axis_par.wZAxis);
	}
	else if (d2410_check_done(Struct_Unite.axis_par.wWAxis) == 0)
	{
		d2410_imd_stop(Struct_Unite.axis_par.wXAxis);
	}
}

//�Ͽ����ƿ�����
void CloseBoard()
{
	 d2410_board_close();
}
//�����
short OpenCamera()
{
	//��ʼ��Pylon SDK��
	PylonInitialize();

	//CTlFactory���������������
	CTlFactory& tlFactory = CTlFactory::GetInstance();

	//��Ҫת����ͼ���ʽ���ڰ���������õ���
	//converter.OutputPixelFormat = PixelType_RGB8planar ;
	// Get all attached devices and exit application if no device is found.

	//�������ļ��ϡ�������һ�����顣
	DeviceInfoList_t devices;

	//�����������ĳ���Ϊ0�������û��������ҵ���
	//EnumerateDevices = �г���ǰ�ܱ��ҵ��������
	if ( tlFactory.EnumerateDevices(devices) == 0 )
	{
		//throw RUNTIME_EXCEPTION( "No camera present.");
		return NOCAMERA;
	}
	if(devices.size()>0)
	{
		//ͨ�����к��������������
		//CDeviceInfo info;
		//info.SetSerialNumber("21914839");
		//camera1 = new Camera_t(CTlFactory::GetInstance().CreateDevice(info));

		try
		{
			//��һ��������ֱ�Ӵ��������camera1��������ʵ�ʵ���һ̨���������ġ�
			//һ̨������һ̨���ʱ�����ø÷�����
			camera1 = new Camera_t(CTlFactory::GetInstance().CreateDevice(devices[0]));
			//�������
			camera1->Open();
		}
		catch (const GenericException& e)
		{
			return ISOPENED;
		}

		//�趨����ڲ�����ͼ������������
		camera1->MaxNumBuffer = 5;

		//ע������ɼ���ͼ��ʱ�Ļص�������
		camera1->RegisterImageEventHandler( new CSampleImageEventHandler, RegistrationMode_Append, Cleanup_Delete);

		//���øûص�������SDK��Ĭ��ֵ��true�����Ը��д�����Բ�д��
		//camera1->GrabCameraEvents = true;

		//����������ع�ʱ�䡣ExposureTime = �׹�ʱ�䡣
		//camera1->ExposureTime.SetValue(1000);

		//������������档Gain = ���档
		//camera1->Gain.SetValue(10);

		//��ȡ������ع�ʱ�䡣�ô����ڴ˴�û�����壬������ʾ��λ�ȡ���������ֵ��
		//double e = camera1->ExposureTime.GetValue();

		//ExposureTime��Gain��������������ƿ�����PylonViewer �в鿴��

		//��ȡ����Ƿ��ǲ�ɫ���
		PixelFormatEnums e = camera1->PixelFormat.GetValue();
		if (e >= PixelFormat_BayerGR8)
		{
			//��Ҫת����ͼ���ʽ���ڰ���������õ���
			converter.OutputPixelFormat = PixelType_RGB8planar ;

			//if(e == PixelFormat_BayerGR8)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGR8;
			//}
			//else if (e == PixelFormat_BayerRG8)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerRG8;
			//}
			//else if (e == PixelFormat_BayerGB8)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGB8;
			//}
			//else if (e == PixelFormat_BayerBG8)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerBG8;
			//}
			//else if (e == PixelFormat_BayerGR10)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGR10;
			//}
			//else if (e == PixelFormat_BayerGR10p)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGR10p;
			//}
			//else if (e == PixelFormat_BayerRG10)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerRG10;
			//}
			//else if (e == PixelFormat_BayerRG10p)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerRG10p;
			//}
			//else if (e == PixelFormat_BayerGB10)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGB10;
			//}
			//else if (e == PixelFormat_BayerGB10p)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGB10p;
			//}
			//else if (e == PixelFormat_BayerBG10)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerBG10;
			//}
			//else if (e == PixelFormat_BayerBG10p)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerBG10p;
			//}
			//else if (e == PixelFormat_BayerGR12)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGR12;
			//}
			//else if (e == PixelFormat_BayerGR12p)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGR12p;
			//}
			//else if (e == PixelFormat_BayerRG12)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerRG12;
			//}
			//else if (e == PixelFormat_BayerRG12p)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerRG12p;
			//}
			//else if (e == PixelFormat_BayerGB12)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGB12;
			//}
			//else if (e == PixelFormat_BayerGB12p)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerGB12p;
			//}
			//else if (e == PixelFormat_BayerBG12)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerBG12;
			//}
			//else if (e == PixelFormat_BayerBG12p)
			//{
			//	converter.OutputPixelFormat = PixelType_BayerBG12p;
			//}
			//else if (e == PixelFormat_RGB8)
			//{
			//	converter.OutputPixelFormat = PixelType_RGB8packed;
			//}
			//else if (e == PixelFormat_BGR8)
			//{
			//	converter.OutputPixelFormat = PixelType_BGR8packed;
			//}
			//else if (e == PixelFormat_YCbCr422_8)
			//{
			//	converter.OutputPixelFormat = PixelType_YUV422_YUYV_Packed;
			//}
			// Get all attached devices and exit application if no device is found.
		}

		//camera.PixelFormat.SetValue(PixelFormat_BayerBG8);

	}
	//�������ͬʱ����2̨���ʱ�����´������ڴ򿪵ڶ�̨��������Ժ��ԡ�
	if(devices.size()>1)
	{
		camera2 = new CInstantCamera(CTlFactory::GetInstance().CreateDevice(devices[1]));
		camera2->Open();
		camera2->MaxNumBuffer = 5;
		camera2->RegisterImageEventHandler( new CSampleImageEventHandler, RegistrationMode_Append, Cleanup_Delete);
		camera2->GrabCameraEvents = true;
	}
	return 0;
}

//�ر���������ʹ�ý����󣬱���Ҫ�ر���������������һֱ����InUse ״̬�����������ó�����һ������ʱ���޷�Open �����
//����ĳ���Open���֮��Close���֮ǰ������ҪԤ��������쳣���³���ֱ���˳��ĵط�������Щ�ط������жϻ�try...catch������catch��Close �����
short CloseCamera()
{
	if(camera1!=NULL && camera1->IsOpen())
	{
		camera1->Close();
		camera1 = NULL;
	}
	else
	{
		return NOOPEN;
	}

	if(camera2!=NULL && camera2->IsOpen())
	{
		camera2->Close();
		camera2 = NULL;
	}
	//����Pylon SDK ��ʹ�á�
	PylonTerminate();
	return 0;
}

//��ʼ�ɼ���������Open���֮�󣬲���ִ�п�ʼ�ɼ��ĺ�����
short StartGrab()
{
	if(camera1!=NULL && camera1->IsOpen())
	{
		//��ʼ�ɼ���
		//ֻ��Ҫ������һ�д���Ϳ����ˡ�������ɼ���ͼ��󣬻��Զ��Ľ���ص��������ڻص������п��Դ����õ�ͼ�����ݡ�
		if (!camera1->IsGrabbing())//δ�����ɼ�
		{
			camera1->StartGrabbing(GrabStrategy_OneByOne,GrabLoop_ProvidedByInstantCamera);
		}
	}
	else
	{
		return NOOPEN;
	}
	if(camera2!=NULL && camera2->IsOpen())
	{
		if (!camera2->IsGrabbing())//δ�����ɼ�
		{
			camera2->StartGrabbing(GrabStrategy_OneByOne,GrabLoop_ProvidedByInstantCamera);
		}
	}
	return 0;
}

//ֹͣ�ɼ��������������ʼ�ɼ�֮�󣬲���ִ��ֹͣ�ɼ��ĺ�����
short StopGrab()
{
	if(camera1!=NULL && camera1->IsOpen())
	{
		//ֹͣ�ɼ���
		camera1->StopGrabbing();
	}
	else
	{
		return NOGRAB;
	}
	if(camera2!=NULL && camera2->IsOpen())
	{
		camera2->StopGrabbing();
	}
	return 0;
}

//�������ա�������Open���֮�󣬲���ִ�����պ�����
short ExecuteGrab(int iTimeOutMs)
{
	if(camera1!=NULL && camera1->IsOpen())
	{
		SetGrabMode(0);//�������
		//���ա�
		if (camera1->CanWaitForFrameTriggerReady())
		{
			if (!camera1->IsGrabbing())//δ�����ɼ�
			{
				camera1->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
			}

			if (camera1->WaitForFrameTriggerReady(iTimeOutMs, TimeoutHandling_Return))
			{
				camera1->ExecuteSoftwareTrigger();
			}
			else
			{
				return TIMEOUT;
			}

		}
	}
	else
	{
		return NOOPEN;
	}
	if(camera2!=NULL && camera2->IsOpen())
	{
		SetGrabMode(0);//�������
		if (camera2->CanWaitForFrameTriggerReady())
		{
			if (!camera2->IsGrabbing())//δ�����ɼ�
			{
				camera2->StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
			}

			if ( camera2->WaitForFrameTriggerReady(iTimeOutMs, TimeoutHandling_Return))
			{
				camera2->ExecuteSoftwareTrigger();
			}
			else
			{
				return TIMEOUT;
			}
		}
	}
	return 0;
}

//���òɼ�ģʽ sMode = 0������1=Ӳ��������2=�����ɼ�ģʽ
void SetGrabMode(short sMode)
{
	if (camera1!=NULL && camera1->IsOpen())
	{
		if (0 == sMode)
		{
			camera1->TriggerMode.SetValue(TriggerMode_On);
			camera1->TriggerSource.SetValue(TriggerSource_Software);
		}
		else if (1 == sMode)
		{
			camera1->TriggerMode.SetValue(TriggerMode_On);
			camera1->TriggerSource.SetValue(TriggerSource_Line1);
		}
		else if (2 == sMode)
		{
			camera1->TriggerMode.SetValue(TriggerMode_Off);
		}
	}
}

//�ع�ģʽ��ʱ�������
void SetExposure(DOUBLE dTimes)
{
	if (camera1!=NULL && camera1->IsOpen())
	{
		camera1->ShutterMode.SetValue(ShutterMode_Global);//����ģʽ
		camera1->ExposureAuto.SetValue(ExposureAuto_Off);//�ع����
		camera1->ExposureMode.SetValue(ExposureMode_Timed);//�ع�ģʽ
		camera1->ExposureTime.SetValue(dTimes);//�����ع�ʱ��
	}
}

//����
void SetGain(DOUBLE dValue)
{
	if (camera1!=NULL && camera1->IsOpen())
	{
		camera1->Gain.SetValue(dValue);
	}
}

//��ȡ��ǰ����ֵ
DOUBLE GetGain()
{
	DOUBLE dValue = 0;
	if (camera1!=NULL && camera1->IsOpen())
	{
		dValue = camera1->Gain.GetValue();
	}
	return dValue;
}

//XY����ת
void ImageReverseXY()
{
	if (camera1!=NULL && camera1->IsOpen())
	{
		camera1->ReverseX.SetValue(true);
		camera1->ReverseY.SetValue(true);
	}
}

void ShowImage(HBITMAP hbitmap)
{
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	//CView *pMainView = pMain->GetActiveView();
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pMain->GetActiveView();

	HDC hSrcDC;
	HDC hDesDC;
	CRect rect;
	CDC dcMem;

	//CStatic �ؼ���ʱ��ĳ��Ի�ģʽ���߽��ؼ���Simple���Ը�Ϊtrue, Picture�ؼ�ʱ����Ҫ���д���
	//pMainView->m_StrReal_Image.ModifyStyle(0, BS_OWNERDRAW);
	BITMAP bm; // λͼ�ṹ�壬���ڽ���λͼ����Ϣ
	hDesDC = pMainView->m_StrReal_Image.GetDC()->m_hDC;
	hSrcDC = CreateCompatibleDC(hDesDC);
	//hSrcDC = CreateCompatibleDC(NULL);
	SelectObject(hSrcDC, hbitmap);
	GetObject(hbitmap, sizeof(bm), &bm);  // ��λͼ��Ϣ����BITMAP�ṹ��

	pMainView->m_StrReal_Image.GetClientRect(&rect);
	//pMainView->m_StrReal_Image.GetWindowRect(&rect);
	pMainView->ScreenToClient(&rect);

	::SetStretchBltMode(hDesDC,COLORONCOLOR);
	StretchBlt(hDesDC, 0, 0, rect.Width() ,rect.Height(), hSrcDC,       
		0, 0, bm.bmWidth,bm.bmHeight, SRCCOPY);
	//::StretchBlt(hDesDC, rect.left, rect.top, rect.right, rect.bottom, 
	//	hSrcDC, 0, 0, rect.right, rect.bottom,SRCCOPY);
	//::StretchBlt(hDesDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hSrcDC, rect.right - rect.left, rect.bottom - rect.top, rect.right, rect.bottom,SRCCOPY); 

	//dcMem.Attach(hDesDC);

	//CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
	//dcMem.SelectObject(&pen);
	//CBrush pBrush(RGB(255, 0, 0));
	//dcMem.SelectObject(&pBrush);
	//CPen *oldPen=dcMem.SelectObject(&pen);

	//dcMem.MoveTo(rect.Width()/2, 0);
	//dcMem.LineTo(rect.Width()/2, rect.Height());

	//dcMem.MoveTo(0, rect.Height()/2);
	//dcMem.LineTo(rect.Width(), rect.Height()/2);

	//dcMem.SelectObject(oldPen);
	//dcMem.DeleteDC();

	DrawLine(hDesDC, rect);

	::DeleteObject(hDesDC);
	::DeleteObject(hSrcDC);
	//::DeleteObject(hbitmap);
}

void DrawLine(HDC hdc, CRect rc)
{
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	//CView *pMainView = pMain->GetActiveView();
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pMain->GetActiveView();
	CDC dcMem;
	//dcMem.FromHandle(hdc);
    if (!pMainView->bCheck) return;

	dcMem.Attach(hdc);
	//CDC* pDC=CDC::FromHandle(hdc);


	CPen pen(PS_SOLID, 1, RGB(0, 0, 255));
	dcMem.SelectObject(&pen);
	CBrush pBrush(RGB(0, 0, 255));
	dcMem.SelectObject(&pBrush);
	CPen *oldPen=dcMem.SelectObject(&pen);

	dcMem.MoveTo(rc.Width()/2, 0);
	dcMem.LineTo(rc.Width()/2, rc.Height());

	dcMem.MoveTo(0, rc.Height()/2);
	dcMem.LineTo(rc.Width(), rc.Height()/2);

	dcMem.SelectObject(oldPen);
	dcMem.DeleteDC();
}

void DrawLine()
{
	CDC *pDC;
	CDC dcMem;
	CRect rc;
	CPen Pen;
	CBitmap bm;
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	//CView *pMainView = pMain->GetActiveView();
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pMain->GetActiveView();

	int x = 0, y = 0;
	x = int(Struct_Unite.cp.dLen + 0.5);
	y = int(Struct_Unite.cp.dWide + 0.5);
	pDC = pMainView->m_StrReal_Image.GetDC();
	pMainView->m_StrReal_Image.GetClientRect(&rc);
	dcMem.CreateCompatibleDC(pDC);	

	bm.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	CBitmap *oldbm=dcMem.SelectObject(&bm);
	dcMem.FillSolidRect(0,0,rc.Width(), rc.Height(),dcMem.GetBkColor());
	//dcMem.FillSolidRect(rc.left, rc.top, rc.Width(), rc.Height(),RGB(255,255,255));
	CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
	dcMem.SelectObject(&pen);
	CBrush pBrush(RGB(255, 0, 0));
	dcMem.SelectObject(&pBrush);
	CPen *oldPen=dcMem.SelectObject(&Pen);

	//dcMem.MoveTo(x/2, 0);
	//dcMem.LineTo(x/2, y);

	//dcMem.MoveTo(0, y/2);
	//dcMem.LineTo(x, y/2);
	dcMem.MoveTo(rc.Width()/2, 0);
	dcMem.LineTo(rc.Width()/2, rc.Height());

	dcMem.MoveTo(0, rc.Height()/2);
	dcMem.LineTo(rc.Width(), rc.Height()/2);

	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(oldPen);

	dcMem.DeleteDC();
	bm.DeleteObject();
}


void ShowVideo(HBITMAP hbitmap)
{
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pMain->GetActiveView();


	//pMain->m_ShowImage.GetClientRect(&rect);//��ȡ�ؼ��ĳߴ�
	//HBITMAP hBitmap=(HBITMAP)::LoadImage(0, szSettingFile, IMAGE_BITMAP, rect.right-rect.left, rect.bottom-rect.top, LR_LOADFROMFILE|LR_DEFAULTSIZE);
	
	//pMain->m_ShowImage.ModifyStyle(0xFF, SS_BITMAP|SS_CENTERIMAGE);
	//pMain->m_ShowImage.SetBitmap(hbitmap);//��ʾͼƬ

	HDC hSrcDC;
	HDC hDesDC;
	RECT rect;

	hDesDC = pMainView->m_StrReal_Image.GetDC()->m_hDC;
	hSrcDC = CreateCompatibleDC(hDesDC);
	SelectObject(hSrcDC, hbitmap);
	//pMain->m_ShowImage.GetClientRect(&rect);
	pMainView->m_StrReal_Image.GetWindowRect(&rect);
	pMainView->ScreenToClient(&rect);

	::SetStretchBltMode(hDesDC,COLORONCOLOR);
	//::StretchBlt(hDesDC, rect.left, rect.top, rect.right, rect.bottom, hSrcDC, 0, 0, rect.right, rect.bottom,SRCCOPY);
	::StretchBlt(hDesDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hSrcDC, rect.right - rect.left, rect.bottom - rect.top, rect.right, rect.bottom,SRCCOPY); 
	::DeleteObject(hDesDC);
	::DeleteObject(hSrcDC);
	::DeleteObject(hbitmap);
}



BOOL SaveNewImage(HBITMAP hbitmap, CString strFilename, int nColor) 
{ 
	BITMAP Bitmap; 
	HDC    hDC;                          
	DWORD  dwPaletteSize=0, dwBmBitsSize = 0,dwDIBSize = 0, dwWritten = 0;      
	BITMAPFILEHEADER       bmfHdr;                    
	BITMAPINFOHEADER       bi;                            
	LPBITMAPINFOHEADER   lpbi;            
	HANDLE  fh = NULL, hDib = NULL, hPal = NULL, hOldPal = NULL;    
	if (nColor <= 8)    
		dwPaletteSize = (1<<nColor)*sizeof(RGBQUAD);    
	GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&Bitmap); 
	bi.biSize = sizeof(BITMAPINFOHEADER); 
	bi.biWidth = Bitmap.bmWidth; 
	bi.biHeight = Bitmap.bmHeight; 
	bi.biPlanes = 1; 
	bi.biBitCount = nColor; 
	bi.biCompression = BI_RGB; 
	bi.biSizeImage = 0; 
	bi.biXPelsPerMeter = 0; 
	bi.biYPelsPerMeter = 0; 
	bi.biClrUsed = 0; 
	bi.biClrImportant = 0;  
	dwBmBitsSize = ((Bitmap.bmWidth*nColor+31)/32*4)*Bitmap.bmHeight;   
	//Ϊλͼ�����ڴ� 
	hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));    
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);    
	*lpbi = bi;  
	//���õ�ɫ�� 
	hPal = GetStockObject(DEFAULT_PALETTE);        
	if (hPal)    
	{    
		hDC = ::GetDC(NULL);    
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);    
		RealizePalette(hDC);    
	}   
	// ��ȡ�õ�ɫ�����µ�����ֵ 
	GetDIBits(hDC, hbitmap, 0, Bitmap.bmHeight, 
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,    
		(BITMAPINFO*)lpbi, DIB_RGB_COLORS);    
	if (hOldPal)    
	{    
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);    
		RealizePalette(hDC);    
		::ReleaseDC(NULL,hDC);    
	}    

	fh = CreateFile(strFilename, GENERIC_WRITE,      
		0,//not   be   shared    
		NULL,   //cannot   be   inherited    
		CREATE_ALWAYS,    
		FILE_ATTRIBUTE_NORMAL |FILE_FLAG_SEQUENTIAL_SCAN,      
		NULL);    
	if (fh == INVALID_HANDLE_VALUE)    
		return FALSE;    
	//����λͼ�ļ�ͷ 
	bmfHdr.bfType = 0x4D42;     //   "BM"    
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;        
	bmfHdr.bfSize = dwDIBSize;    
	bmfHdr.bfReserved1 = 0;    
	bmfHdr.bfReserved2 = 0;    
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;    
	//write   file   header    
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);    
	//write   bmp   data    
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);    

	GlobalUnlock(hDib);    
	GlobalFree(hDib);    
	CloseHandle(fh);    
	//::DeleteObject(hbitmap);  
	return   TRUE;   
}

void LoadImageX(CString strPath)
{
	//char szPath[1024];
	//memset(szPath, 0, sizeof(szPath));
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pMain->GetActiveView();
	//wcharTochar(strPath, szPath, strPath.GetLength());

	//CRect rect;
	//pMain->m_ShowImage.GetClientRect(&rect);//��ȡ�ؼ��ĳߴ�
	//HBITMAP hBitmap=(HBITMAP)::LoadImage(0, szSettingFile, IMAGE_BITMAP, rect.right-rect.left, rect.bottom-rect.top, LR_LOADFROMFILE|LR_DEFAULTSIZE);

	//pMain->m_ShowImage.ModifyStyle(0xFF, SS_BITMAP|SS_CENTERIMAGE);
	//pMain->m_ShowImage.SetBitmap(hbitmap);//��ʾͼƬ

	/*
	HDC hSrcDC;
	HDC hDesDC;
	//CBitmap bmp;
	RECT rect;

	//pMain->m_ShowImage.GetClientRect(&rect);//��ȡ�ؼ��ĳߴ�
	pMain->m_ShowImage.GetWindowRect(&rect);
	pMain->ScreenToClient(&rect);

	hDesDC = pMain->m_ShowImage.GetDC()->m_hDC;
	hSrcDC = CreateCompatibleDC(hDesDC);
	*/

	//HBITMAP hBitmap=(HBITMAP)::LoadImage(0, strPath, IMAGE_BITMAP, rect.right-rect.left, rect.bottom-rect.top, LR_LOADFROMFILE|LR_DEFAULTSIZE);
	bmp1 = (HBITMAP)::LoadImage(0, strPath, IMAGE_BITMAP, NULL, NULL, LR_LOADFROMFILE|LR_DEFAULTSIZE);
	//SendMessage(pMain->m_hWnd, WM_PAINT, NULL, NULL);

	/*
	SelectObject(hSrcDC, hBitmap);
	::SetStretchBltMode(hDesDC,COLORONCOLOR);
//	::StretchBlt(hDesDC, rect.left, rect.top, rect.right, rect.bottom, hSrcDC, 0, 0, rect.right, rect.bottom,SRCCOPY); 
	::StretchBlt(hDesDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hSrcDC, rect.right - rect.left, rect.bottom - rect.top, rect.right, rect.bottom,SRCCOPY); 
	::DeleteObject(hDesDC);
	::DeleteObject(hSrcDC);
	::DeleteObject(hBitmap);
	*/
//	bmp1 = (HBITMAP)::LoadImage(0, strPath, IMAGE_BITMAP, NULL, NULL, LR_LOADFROMFILE|LR_DEFAULTSIZE);
}

void NewShow(CDC* dc)
{
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pMain->GetActiveView();
	CRect rect;
	pMainView->m_StrReal_Image.GetWindowRect(&rect);
	//m_StrReal_Image.GetClientRect(&rect);
	pMainView->ScreenToClient(rect);
	HDC hMemDC=CreateCompatibleDC(dc->m_hDC);    //�����ڴ�DC
	//HBITMAP hBitmap=(HBITMAP)::LoadImage(0, _T("C:\\123.bmp"), IMAGE_BITMAP, NULL, NULL, LR_LOADFROMFILE|LR_DEFAULTSIZE);
	//bmp1 = hBitmap;
	::SelectObject(hMemDC,bmp1);      //��λͼѡ���ڴ�DC
	BitBlt(dc->m_hDC,rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,hMemDC,0,0,SRCCOPY); //�ڴ�DCӳ�䵽��ĻDC
	::DeleteDC(hMemDC) ;        //ɾ���ڴ�DC
}

int InitDevConn()
{
	CString strErr = _T("");
	CString strTemp = _T(""), strPath = _T("");
	TCHAR szTemp[512] = _T("");
	char  szCom[12];
	memset(szCom, 0, sizeof(szCom));
	WORD wRet = 0;
	CMenu *pMenu;
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pFrame->GetActiveView();

	pMenu = pFrame->GetMenu()->GetSubMenu(0);
	pMenu->GetMenuString(ID_CONNECT, strTemp, 1);
	if (0 == strTemp.Compare(_T("���Ӱ忨")))//���ƿ�
	{
		wRet = d2410_board_init();
		if (wRet <= 0) return CONNECTERR;
		wRet = EnableLmt();
		if (wRet)
		{
			d2410_board_close();
			return wRet;
		}
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_CONNECT, _T("�Ͽ��忨"));
		pFrame->DrawMenuBar();
		pMainView->bConnect = TRUE;
		pMainView->SetTimer(1, 100, NULL);
		pMainView->SetTimer(2, 60, NULL);
	}
	pMenu = pFrame->GetMenu()->GetSubMenu(1);
	pMenu->GetMenuString(ID_OPENCAMERA, strTemp, 1);
	if (0 == strTemp.Compare(_T("�����")))//���
	{
		wRet = OpenCamera();
		if (wRet)
		{
			d2410_board_close();
			return wRet;
		}
		ImageReverseXY();
		SetExposure(Struct_Unite.cp.dEpsTime);
		SetGain(Struct_Unite.cp.dGain);
		SetGrabMode(2);
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_OPENCAMERA, _T("�ر����"));
		pFrame->DrawMenuBar();
	}
	strPath = GetFilePath();
	strPath += INI_FILENAME;
	::GetPrivateProfileString(_T("LightControl"), _T("LightCom"), _T(""), szTemp, 512, strPath);
	strTemp += (CString)szTemp;
	sprintf_s(szCom, "com%d", strTemp.Mid(3));
	m_hCom = Lts_CommOpen(szCom);//��Դ
	if (0 == m_hCom)
	{
		d2410_board_close();
		CloseCamera();
		return OPENCOMERR;
	}
	//ͶӰ��������,����豸������
	return wRet;
}


void DeleteDirectory(LPCTSTR path)
{
	CFileFind findfile;
	CString str = _T("");
	str=path;
	if(str.Right(1)!=_T("\\"))
		str.Format(_T("%s\\*.*"), path);
	else
		str.Format(_T("%s*.*"), path);
	BOOL find=findfile.FindFile(str);

	while(find)
	{
		find=findfile.FindNextFile();
		if(findfile.IsDirectory())
		{
			if(!findfile.IsDots())
			{
				DeleteDirectory(findfile.GetFilePath());
			}
		}
		else
		{
			DeleteFile(findfile.GetFilePath());
		}
	}
	findfile.Close();
	//if(!RemoveDirectory(path))
	//{
	//	DWORD ret=::GetLastError();
	//	CString strerr = _T("");
	//	strerr.Format(_T("%d"), ret);
	//	//MessageBox(strerr, _T("�������"), MB_OK);
	//}
}


/*
void Screen(char filename[])
{
	CDC *pDC;//��ĻDC
	pDC = CDC::FromHandle(GetDC(NULL));//��ȡ��ǰ������ĻDC
	int BitPerPixel = pDC->GetDeviceCaps(BITSPIXEL);//�����ɫģʽ
	int Width = pDC->GetDeviceCaps(HORZRES);
	int Height = pDC->GetDeviceCaps(VERTRES);
	//cout << "��ǰ��Ļɫ��ģʽΪ" << BitPerPixel << "λɫ��" << endl  << "��Ļ��ȣ�" << Width << endl          << "��Ļ�߶ȣ�" << Height << endl << endl;    
	CDC memDC;//�ڴ�DC 
	memDC.CreateCompatibleDC(pDC); 
	CBitmap memBitmap, *oldmemBitmap;//��������Ļ���ݵ�bitmap 
	memBitmap.CreateCompatibleBitmap(pDC, Width, Height); 
	oldmemBitmap = memDC.SelectObject(&memBitmap);//��memBitmapѡ���ڴ�DC 
	memDC.BitBlt(0, 0, Width, Height, pDC, 0, 0, SRCCOPY);//������Ļͼ���ڴ�DC  
	//���´��뱣��memDC�е�λͼ���ļ� 
	BITMAP bmp; 
	memBitmap.GetBitmap(&bmp);//���λͼ��Ϣ
	FILE *fp = fopen(filename, "w+b"); 
	BITMAPINFOHEADER bih = {0};//λͼ��Ϣͷ 
	bih.biBitCount = bmp.bmBitsPixel;//ÿ�������ֽڴ�С 
	bih.biCompression = BI_RGB; 
	bih.biHeight = bmp.bmHeight;//�߶� 
	bih.biPlanes = 1; 
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//ͼ�����ݴ�С
	bih.biWidth = bmp.bmWidth;//���
	BITMAPFILEHEADER bfh = {0};//λͼ�ļ�ͷ 
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ���� 
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//�ļ��ܵĴ�С
	bfh.bfType = (WORD)0x4d42; 

    fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//д��λͼ�ļ�ͷ 
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//д��λͼ��Ϣͷ 
	byte * p = new byte[bmp.bmWidthBytes * bmp.bmHeight];//�����ڴ汣��λͼ���� 
	GetDIBits(memDC.m_hDC, (HBITMAP) memBitmap.m_hObject, 0, Height, p, (LPBITMAPINFO) &bih, DIB_RGB_COLORS);//��ȡλͼ����
	fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);//д��λͼ���� 
	delete [] p; 
	fclose(fp);
	memDC.SelectObject(oldmemBitmap);
}
*/
