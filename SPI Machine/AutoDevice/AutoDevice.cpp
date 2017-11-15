
// AutoDevice.cpp : 定义应用程序的类行为。
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
	// 基于文件的标准文档命令
	//ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CAutoDeviceApp 构造

CAutoDeviceApp::CAutoDeviceApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("AutoDevice.AppID.NoVersion"));

	m_bEnglish = TRUE;//默认的是中文

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CAutoDeviceApp 对象
CAutoDeviceApp theApp;

STRUCT_UNITE Struct_Unite;
BOOL m_bEnglish;
HANDLE m_hCom;

//相机对象
Camera_t* camera1;

//另一种形式的相机对象
CInstantCamera* camera2;
HBITMAP bmp1 = NULL;
HBITMAP bmp2 = NULL;
CPylonImage targetImage1;
CPylonBitmapImage destimage1;
CPylonImage targetImage2;
CPylonBitmapImage destimage2;

//Pylon SDK中的图像转换工具 (bayer转换),黑白相机不会用到。
CImageFormatConverter converter;//转换工具。
short sType = 0;

//IplImage* g_pImg;
//float g_nPcbWidth;
//float g_nPcbHeight;
//Point2f g_robotPoint;

//保存指定视野下所选择的焊盘位置
vector<RectF> g_vecWeldingPlateRect;

// CAutoDeviceApp 初始化

//NONCLIENTMETRICS m_ncmOld;  
//NONCLIENTMETRICS m_ncmNew;  

BOOL CAutoDeviceApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(6);  // 加载标准 INI 文件选项(包括 MRU)


	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAutoDeviceDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CAutoDeviceView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生

	m_hCom = 0;

	//CAxisZeroDlg Dlg;
	//Dlg.bFirst = TRUE;
	//Dlg.bZero = TRUE;
	//Dlg.DoModal();
	
	return TRUE;
}

int CAutoDeviceApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);
	//::SystemParametersInfo(SPI_SETNONCLIENTMETRICS, m_ncmOld.cbSize, &m_ncmOld, 0);  

	return CWinApp::ExitInstance();
}

// CAutoDeviceApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// 用于运行对话框的应用程序命令
void CAutoDeviceApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAutoDeviceApp 消息处理程序


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
//purpose:char to WCHAR 、wchar_t、LPWSTR etc 
void charTowchar(const char *chr, wchar_t *wchar, int size)   
{      
	MultiByteToWideChar( CP_ACP, 0, chr,   
		strlen(chr)+1, wchar, size/sizeof(wchar[0]) );   
}   

//function: wcharTochar 
//purpose:WCHAR 、wchar_t、LPWSTR to char 
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
		Err_Info = _T("未知错误");
		break;
	case 2:
		Err_Info = _T("参数错误");
		break;
	case 3:
		Err_Info = _T("操作超时");
		break;
	case 4:
		Err_Info = _T("控制卡状态忙");
		break;
	case 9:
		Err_Info = _T("卡号错误");
		break;
	case 10:
		Err_Info = _T("数据传输错误,请检查PCI插槽是否松动");
		break;
	case 20:
		Err_Info = _T("控制器返回参数错误");
		break;
	case 22:
		Err_Info = _T("控制器返回当前状态不允许操作");
		break;
	case 24:
		Err_Info = _T("控制器不支持的功能");
		break;
	case OPENCOMERR:
		Err_Info = _T("打开串口失败,请检查串口是否正确或者串口已经正确连接并上电");
		break;
	case CONNECTERR:
		Err_Info = _T("没有找到控制卡,或者控制卡异常");
		break;
	case RUNING:
		StopMoveAxis();
		Err_Info = _T("要操作的轴处于运动状态中");
		break;
	case NOCAMERA:
		Err_Info = _T("没有找到相机,请确认相机是否安装并正确连接");
		break;
	case NOOPEN:
		Err_Info = _T("相机尚未打开,或相机连接故障");
		break;
	case NOGRAB:
		Err_Info = _T("相机没有开始采集");
		break;
	case TIMEOUT:
		Err_Info = _T("相机拍照超时");
		break;
	case ISOPENED:
		Err_Info = _T("要操作的相机已经打开");
		break;
	case LEFTDOWNTOUT:
		Err_Info = _T("左侧挡板气缸下降超时");
		break;
	case LEFTUPTOUT:
		Err_Info = _T("左侧挡板气缸上升超时");
		break;
	case RIGHTDOWNTOUT:
		Err_Info = _T("右侧挡板气缸下降超时");
		break;
	case RIGHTUPTOUT:
		Err_Info = _T("右侧挡板气缸上升超时");
		break;
	case MIDDLEDOWNTOUT:
		Err_Info = _T("夹板板气缸下降超时");
		break;
	case MIDDLEUPTOUT:
		Err_Info = _T("夹板气缸上升超时");
		break;
	case AUTOEXIT:
		Err_Info = _T("退出自动运行");
		break;
	case RAILBARRIER:
		Err_Info = _T("轨道内有板子，请取出轨道内的板子");
		break;
	case DOOROPEN:
		Err_Info = _T("设备门尚未关闭,请先关闭设备门");
		break;
	case SAVEFAIL:
		Err_Info = _T("保存图片失败");
		break;
	case ENTERTIMEOUT:
		Err_Info = _T("等待进板超时,取消进板状态");
		break;
	case Bad_CommOpen:
		Err_Info = _T("光源串口打开失败");
		break;
	case Bad_CommSet:
		Err_Info = _T("光源串口设置失败");
		break;
	case Bad_CommTimeouts:
		Err_Info = _T("光源串口读写超时设置错误");
		break;
	case Bad_CommQueue:
		Err_Info = _T("光源串口缓冲区操作错误");
		break;
	case Bad_CommRead:
		Err_Info = _T("光源串口读数据出错");
		break;
	case Bad_CommWrite:
		Err_Info = _T("光源串口写数据超时");
		break;
	case PACKET_Timeout:
		Err_Info = _T("光源串口读取数据超时");
		break;
	case Bad_CommClose:
		Err_Info = _T("串口关闭失败");
		break;
	case -1:
		Err_Info = _T("光源串口通讯失败");
		break;
	}
}

/* 单轴运动 
* posi_mode = 0 相对运动模式, 1绝对运动模式
* stType = 0 S型截面特性, 1 T型截面特性
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
	if (d2410_check_done(axis) == 0) //已经在运动中
		return RUNING; 
	if (stType == 0)
	{
		//设定S形截面的特性
		wRet = d2410_set_st_profile(axis, dCovMin_Vel, dCovMax_Vel, Tacc, Tdec, Tsacc, Tsdec);
		if (wRet) return wRet;
		wRet = d2410_s_pmove(axis, lCovDist, posi_mode);
		if (wRet) return wRet;
	}
	else if (stType == 1)
	{
		//设定T形截面的特性
		wRet = d2410_set_profile(axis, dCovMin_Vel, dCovMax_Vel, Tacc, Tdec);
		if (wRet) return wRet;
		wRet = d2410_ex_t_pmove(axis, lCovDist, posi_mode);
		if (wRet) return wRet;
	}
	return wRet;
}

/* 单轴回原点 
* stType = 0 S型截面特性, 1 T型截面特性
* org_logic = 0 低电平有效，1 高电平有效  ORG 信号的有效电平
* EZ_logic Ez信号有效电平 0 高有效, 1 低有效
* Mode 回原点的信号模式：
* 0：只计home
* 1：计home 和EZ，计1 个EZ 信号
* 2：一次回零加回找
* 3：二次回零
* 4：EZ 单独回零
* 5：原点捕获回零
* EZ_count EZ 信号出现EZ_count 指定的次数后，轴运动停止。仅当mode=4 时该
* 参数设置有效，取值范围：1－16
* Home_Mode 回原点方式，1：正方向回原点，2：负方向回原点
* Vel_Mode 回原点速度，0：低速回原点，1：高速回原点
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
	if (d2410_check_done(axis) == 0) //已经在运动中
		return RUNING; 
	wRet = d2410_set_HOME_pin_logic(axis, org_logic, 1);
	if (wRet) return wRet;
	if (stType == 0)
	{
		//设定S形截面的特性
		wRet = d2410_set_st_profile(axis, dCovMin_Vel, dCovMax_Vel, Tacc, Tdec, Tsacc, Tsdec);
		if (wRet) return wRet;
	}
	else if (stType == 1)
	{
		//设定T形截面的特性
		wRet = d2410_set_profile(axis, dCovMin_Vel, dCovMax_Vel, Tacc, Tdec);
		if (wRet) return wRet;
	}
	wRet = d2410_config_EZ_PIN(axis, EZ_logic, 0);//设置EZ作用方式
	if (wRet) return wRet;
	wRet = d2410_config_home_mode(axis, Mode, EZ_count);//设置回零方式
	if (wRet) return wRet;
	wRet = d2410_home_move(axis, Home_Mode, Vel_Mode);//回零动作
	if (wRet) return wRet;
	//do 
	//{
	//	DoEvents();
	//} while (d2410_check_done(axis) == 0);//轴正在运行
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

/* 回原点 
* stType = 0 XYZW同时, =1 XY，=2 Z =3 W
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
		//} while (d2410_check_done(Struct_Unite.axis_par.wZAxis) == 0);//轴正在运行
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
			|| d2410_check_done(Struct_Unite.axis_par.wWAxis) == 0);//轴正在运行
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

//检测通道内是否有板卡有的话送到出口位置等待用户取走
WORD CheckPcb()
{
	WORD wRet = 0;
	WORD wML = 0, wMR = 0;
	CMainFrame *pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CAutoDeviceView *pMainView = (CAutoDeviceView*)pMain->GetActiveView();
	//先全部复位挡板气缸，夹板气缸
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
			if (0 == Struct_Unite.axis_par.wDirect)//左侧进板
			{
				if ((!wMR || !wML) && 0 == Struct_Unite.axis_par.wDirect)
				{
					wRet = pMainView->SetOneBitOut(OUT9, LOWLEVEL);//轨道传输正向
					if (wRet) return wRet;
				}
			}
			else if (1 == Struct_Unite.axis_par.wDirect)//右侧进板
			{
				if ((!wMR || !wML) && 1 == Struct_Unite.axis_par.wDirect)
				{
					wRet = pMainView->SetOneBitOut(OUT10, LOWLEVEL);//轨道传输反向
					if (wRet) return wRet;
				}
			}
		} while (!wML || !wMR);
		//板子运行到出口位置停止等待取走
		if (0 == Struct_Unite.axis_par.wDirect)//左侧进板
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
		else if (1 == Struct_Unite.axis_par.wDirect)//右侧进板
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

//XYZ停靠的安全位置
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

//出现操作轴在运动中的错误时,立即停止所有在运动的轴
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

//断开控制卡连接
void CloseBoard()
{
	 d2410_board_close();
}
//打开相机
short OpenCamera()
{
	//初始化Pylon SDK。
	PylonInitialize();

	//CTlFactory用来创建相机对象。
	CTlFactory& tlFactory = CTlFactory::GetInstance();

	//需要转换的图像格式。黑白相机不会用到。
	//converter.OutputPixelFormat = PixelType_RGB8planar ;
	// Get all attached devices and exit application if no device is found.

	//多个相机的集合。类似于一个数组。
	DeviceInfoList_t devices;

	//相机集合数组的长度为0，则代表没有相机被找到。
	//EnumerateDevices = 列出当前能被找到的相机。
	if ( tlFactory.EnumerateDevices(devices) == 0 )
	{
		//throw RUNTIME_EXCEPTION( "No camera present.");
		return NOCAMERA;
	}
	if(devices.size()>0)
	{
		//通过序列号来创建相机对象。
		//CDeviceInfo info;
		//info.SetSerialNumber("21914839");
		//camera1 = new Camera_t(CTlFactory::GetInstance().CreateDevice(info));

		try
		{
			//另一种做法，直接创建相机。camera1对象代表的实际的哪一台相机是随机的。
			//一台电脑配一台相机时可以用该方法。
			camera1 = new Camera_t(CTlFactory::GetInstance().CreateDevice(devices[0]));
			//打开相机。
			camera1->Open();
		}
		catch (const GenericException& e)
		{
			return ISOPENED;
		}

		//设定相机内部缓存图像的最大张数。
		camera1->MaxNumBuffer = 5;

		//注册相机采集到图像时的回调函数。
		camera1->RegisterImageEventHandler( new CSampleImageEventHandler, RegistrationMode_Append, Cleanup_Delete);

		//启用该回调函数。SDK中默认值是true。所以该行代码可以不写。
		//camera1->GrabCameraEvents = true;

		//设置相机的曝光时间。ExposureTime = 白光时间。
		//camera1->ExposureTime.SetValue(1000);

		//设置相机的增益。Gain = 增益。
		//camera1->Gain.SetValue(10);

		//获取相机的曝光时间。该代码在此处没有意义，仅仅演示如何获取相机的属性值。
		//double e = camera1->ExposureTime.GetValue();

		//ExposureTime，Gain等相机的属性名称可以在PylonViewer 中查看。

		//获取相机是否是彩色相机
		PixelFormatEnums e = camera1->PixelFormat.GetValue();
		if (e >= PixelFormat_BayerGR8)
		{
			//需要转换的图像格式。黑白相机不会用到。
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
	//假设电脑同时连接2台相机时，以下代码是在打开第二台相机。可以忽略。
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

//关闭相机。相机使用结束后，必须要关闭相机。否则相机会一直处于InUse 状态，其他程序或该程序下一次运行时将无法Open 相机。
//在你的程序Open相机之后，Close相机之前，你需要预估会出现异常导致程序直接退出的地方，在那些地方加上判断或try...catch，并在catch中Close 相机。
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
	//结束Pylon SDK 的使用。
	PylonTerminate();
	return 0;
}

//开始采集。必须在Open相机之后，才能执行开始采集的函数。
short StartGrab()
{
	if(camera1!=NULL && camera1->IsOpen())
	{
		//开始采集。
		//只需要调用这一行代码就可以了。当相机采集到图像后，会自动的进入回调函数。在回调函数中可以处理获得的图像数据。
		if (!camera1->IsGrabbing())//未开启采集
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
		if (!camera2->IsGrabbing())//未开启采集
		{
			camera2->StartGrabbing(GrabStrategy_OneByOne,GrabLoop_ProvidedByInstantCamera);
		}
	}
	return 0;
}

//停止采集。必须在相机开始采集之后，才能执行停止采集的函数。
short StopGrab()
{
	if(camera1!=NULL && camera1->IsOpen())
	{
		//停止采集。
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

//软触发拍照。必须在Open相机之后，才能执行拍照函数。
short ExecuteGrab(int iTimeOutMs)
{
	if(camera1!=NULL && camera1->IsOpen())
	{
		SetGrabMode(0);//软件触发
		//拍照。
		if (camera1->CanWaitForFrameTriggerReady())
		{
			if (!camera1->IsGrabbing())//未开启采集
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
		SetGrabMode(0);//软件触发
		if (camera2->CanWaitForFrameTriggerReady())
		{
			if (!camera2->IsGrabbing())//未开启采集
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

//设置采集模式 sMode = 0软触发，1=硬件触发，2=连续采集模式
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

//曝光模式，时间等设置
void SetExposure(DOUBLE dTimes)
{
	if (camera1!=NULL && camera1->IsOpen())
	{
		camera1->ShutterMode.SetValue(ShutterMode_Global);//快门模式
		camera1->ExposureAuto.SetValue(ExposureAuto_Off);//曝光控制
		camera1->ExposureMode.SetValue(ExposureMode_Timed);//曝光模式
		camera1->ExposureTime.SetValue(dTimes);//设置曝光时间
	}
}

//增益
void SetGain(DOUBLE dValue)
{
	if (camera1!=NULL && camera1->IsOpen())
	{
		camera1->Gain.SetValue(dValue);
	}
}

//获取当前增益值
DOUBLE GetGain()
{
	DOUBLE dValue = 0;
	if (camera1!=NULL && camera1->IsOpen())
	{
		dValue = camera1->Gain.GetValue();
	}
	return dValue;
}

//XY镜像反转
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

	//CStatic 控件的时候改成自绘模式或者将控件中Simple属性改为true, Picture控件时不需要此行代码
	//pMainView->m_StrReal_Image.ModifyStyle(0, BS_OWNERDRAW);
	BITMAP bm; // 位图结构体，用于接受位图的信息
	hDesDC = pMainView->m_StrReal_Image.GetDC()->m_hDC;
	hSrcDC = CreateCompatibleDC(hDesDC);
	//hSrcDC = CreateCompatibleDC(NULL);
	SelectObject(hSrcDC, hbitmap);
	GetObject(hbitmap, sizeof(bm), &bm);  // 将位图信息填入BITMAP结构体

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


	//pMain->m_ShowImage.GetClientRect(&rect);//获取控件的尺寸
	//HBITMAP hBitmap=(HBITMAP)::LoadImage(0, szSettingFile, IMAGE_BITMAP, rect.right-rect.left, rect.bottom-rect.top, LR_LOADFROMFILE|LR_DEFAULTSIZE);
	
	//pMain->m_ShowImage.ModifyStyle(0xFF, SS_BITMAP|SS_CENTERIMAGE);
	//pMain->m_ShowImage.SetBitmap(hbitmap);//显示图片

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
	//为位图分配内存 
	hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));    
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);    
	*lpbi = bi;  
	//设置调色板 
	hPal = GetStockObject(DEFAULT_PALETTE);        
	if (hPal)    
	{    
		hDC = ::GetDC(NULL);    
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);    
		RealizePalette(hDC);    
	}   
	// 获取该调色板下新的像素值 
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
	//设置位图文件头 
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
	//pMain->m_ShowImage.GetClientRect(&rect);//获取控件的尺寸
	//HBITMAP hBitmap=(HBITMAP)::LoadImage(0, szSettingFile, IMAGE_BITMAP, rect.right-rect.left, rect.bottom-rect.top, LR_LOADFROMFILE|LR_DEFAULTSIZE);

	//pMain->m_ShowImage.ModifyStyle(0xFF, SS_BITMAP|SS_CENTERIMAGE);
	//pMain->m_ShowImage.SetBitmap(hbitmap);//显示图片

	/*
	HDC hSrcDC;
	HDC hDesDC;
	//CBitmap bmp;
	RECT rect;

	//pMain->m_ShowImage.GetClientRect(&rect);//获取控件的尺寸
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
	HDC hMemDC=CreateCompatibleDC(dc->m_hDC);    //创建内存DC
	//HBITMAP hBitmap=(HBITMAP)::LoadImage(0, _T("C:\\123.bmp"), IMAGE_BITMAP, NULL, NULL, LR_LOADFROMFILE|LR_DEFAULTSIZE);
	//bmp1 = hBitmap;
	::SelectObject(hMemDC,bmp1);      //把位图选进内存DC
	BitBlt(dc->m_hDC,rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,hMemDC,0,0,SRCCOPY); //内存DC映射到屏幕DC
	::DeleteDC(hMemDC) ;        //删除内存DC
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
	if (0 == strTemp.Compare(_T("连接板卡")))//控制卡
	{
		wRet = d2410_board_init();
		if (wRet <= 0) return CONNECTERR;
		wRet = EnableLmt();
		if (wRet)
		{
			d2410_board_close();
			return wRet;
		}
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_CONNECT, _T("断开板卡"));
		pFrame->DrawMenuBar();
		pMainView->bConnect = TRUE;
		pMainView->SetTimer(1, 100, NULL);
		pMainView->SetTimer(2, 60, NULL);
	}
	pMenu = pFrame->GetMenu()->GetSubMenu(1);
	pMenu->GetMenuString(ID_OPENCAMERA, strTemp, 1);
	if (0 == strTemp.Compare(_T("打开相机")))//相机
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
		pMenu->ModifyMenu(0, MF_BYPOSITION, ID_OPENCAMERA, _T("关闭相机"));
		pFrame->DrawMenuBar();
	}
	strPath = GetFilePath();
	strPath += INI_FILENAME;
	::GetPrivateProfileString(_T("LightControl"), _T("LightCom"), _T(""), szTemp, 512, strPath);
	strTemp += (CString)szTemp;
	sprintf_s(szCom, "com%d", strTemp.Mid(3));
	m_hCom = Lts_CommOpen(szCom);//光源
	if (0 == m_hCom)
	{
		d2410_board_close();
		CloseCamera();
		return OPENCOMERR;
	}
	//投影机的连接,测高设备的连接
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
	//	//MessageBox(strerr, _T("错误代码"), MB_OK);
	//}
}


/*
void Screen(char filename[])
{
	CDC *pDC;//屏幕DC
	pDC = CDC::FromHandle(GetDC(NULL));//获取当前整个屏幕DC
	int BitPerPixel = pDC->GetDeviceCaps(BITSPIXEL);//获得颜色模式
	int Width = pDC->GetDeviceCaps(HORZRES);
	int Height = pDC->GetDeviceCaps(VERTRES);
	//cout << "当前屏幕色彩模式为" << BitPerPixel << "位色彩" << endl  << "屏幕宽度：" << Width << endl          << "屏幕高度：" << Height << endl << endl;    
	CDC memDC;//内存DC 
	memDC.CreateCompatibleDC(pDC); 
	CBitmap memBitmap, *oldmemBitmap;//建立和屏幕兼容的bitmap 
	memBitmap.CreateCompatibleBitmap(pDC, Width, Height); 
	oldmemBitmap = memDC.SelectObject(&memBitmap);//将memBitmap选入内存DC 
	memDC.BitBlt(0, 0, Width, Height, pDC, 0, 0, SRCCOPY);//复制屏幕图像到内存DC  
	//以下代码保存memDC中的位图到文件 
	BITMAP bmp; 
	memBitmap.GetBitmap(&bmp);//获得位图信息
	FILE *fp = fopen(filename, "w+b"); 
	BITMAPINFOHEADER bih = {0};//位图信息头 
	bih.biBitCount = bmp.bmBitsPixel;//每个像素字节大小 
	bih.biCompression = BI_RGB; 
	bih.biHeight = bmp.bmHeight;//高度 
	bih.biPlanes = 1; 
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//图像数据大小
	bih.biWidth = bmp.bmWidth;//宽度
	BITMAPFILEHEADER bfh = {0};//位图文件头 
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);//到位图数据的偏移量 
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//文件总的大小
	bfh.bfType = (WORD)0x4d42; 

    fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//写入位图文件头 
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//写入位图信息头 
	byte * p = new byte[bmp.bmWidthBytes * bmp.bmHeight];//申请内存保存位图数据 
	GetDIBits(memDC.m_hDC, (HBITMAP) memBitmap.m_hObject, 0, Height, p, (LPBITMAPINFO) &bih, DIB_RGB_COLORS);//获取位图数据
	fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);//写入位图数据 
	delete [] p; 
	fclose(fp);
	memDC.SelectObject(oldmemBitmap);
}
*/
