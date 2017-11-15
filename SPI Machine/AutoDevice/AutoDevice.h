
// AutoDevice.h : AutoDevice 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号

#define TXT_FILENAME    _T("SetParameter.txt")
#define INI_FILENAME    _T("PwConfig.ini")
#define INFO_ICON           (MB_OK | MB_ICONINFORMATION)
#define ALERT_ERR           _T("Error")
#define ALERT_INFOR         _T("Information")
#define ALERT_ICON          (MB_OK | MB_ICONEXCLAMATION)
#define ALERT_CONFIRM       (MB_OKCANCEL | MB_ICONEXCLAMATION)
#define ALERT_CONFIRMQ      (MB_OKCANCEL | MB_ICONQUESTION)
#define RUNING       100 //轴运动中
#define NOCAMERA 101 //没找到相机
#define NOOPEN   102 //相机未打开
#define NOGRAB   103 //没有开始采集  
#define TIMEOUT  104 //超时
#define ISOPENED 106 //相机已经打开
#define AUTOEXIT 254  //退出自动运行
#define RAILBARRIER 105 //轨道内有板
#define LEFTDOWNTOUT   107//左挡板气缸下降超时
#define LEFTUPTOUT     108//左挡板气缸上升超时
#define RIGHTDOWNTOUT  109//右挡板气缸下降超时
#define RIGHTUPTOUT    110//右挡板气缸上升超时
#define MIDDLEDOWNTOUT  111//夹板气缸下降超时
#define MIDDLEUPTOUT    112//夹板气缸上升超时
#define DOOROPEN        113//门未关闭
#define SAVEFAIL        114//保存图片失败
#define ENTERTIMEOUT    115//等待进板超时
#define CONNECTERR      116//没有找到控制卡,或者控制卡异常
#define OPENCOMERR      117//打开串口失败,请检查串口是否正确或者串口已经正确连接并上电

#define WM_UPDATE_INFO  WM_USER+101 //自定义消息更新状态栏提示
#define WM_UPDATEPCBMAP (WM_USER + 1000)//发送绘制Mark点消息



#define MENUNUM  6    //菜单项数
//通用输入输出电平定义
#define LOWLEVEL   0  //低电平   有效
#define HIGHLEVEL  1  //高电平   无效

//超时时间定义
#define TIME_OUT		1000
#define SLEEPTIME       300
#define ENTERTOT        8000 
#define CAMERATIME       50

//IO
#define IN1        1
#define IN2        2
#define IN3        3
#define LEFTIN4     4    //左挡板
#define RIGHTIN5    5    //右挡板
#define MIDDLEIN6   6    //夹板
#define IN7        7
#define IN8        8
#define IN9        9
#define IN10        10
#define IN11        11
#define IN12        12
#define IN13        13
#define IN14        14

#define OUT1        1
#define OUT2        2
#define OUT3        3
#define OUT4        4
#define OUT5        5
#define LEFTOUT6     6    //左挡板
#define RIGHTOUT7    7    //右挡板
#define MIDDLEOUT8   8    //夹板
#define OUT9        9
#define OUT10       10
#define OUT11       11


#include <Pylon/PylonIncludes.h>
using namespace Pylon;
#include <pylon/PylonGUI.h>


#include <pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
using namespace Basler_UsbCameraParams;

//新建工程界面 PcbMap更新枚举
enum UPDATEPCBMAP
{
	//绘制Mark1
	enumDrawMark1,
	//绘制Mark2
	enumDrawMark2,
	//更新Fov
	enumUpdateFov,
	//更新PcbMap
	enumUpdatePcbMap
};
//向元器件编辑界面发送参数
enum TRANSLATEPARAMETER
{
	//焊盘的逻辑坐标
	enumPadLogicPos,
	//PCB板相关参数
	enumStruct_Unite,
	//工程文件数据库
	enumProjectPathName
};


typedef struct _REAL_COOR_ //实时坐标值
{
	DOUBLE dx;
	DOUBLE dy;
	DOUBLE dz;
	DOUBLE dw;
}REAL_COOR, *LPREAL_COOR;

typedef struct _AXIS_PARAMTER_ //各轴参数设置
{
	WORD  wXAxis;          //轴号
	DOUBLE dXPulsePer;      //每转脉冲
	DOUBLE dXPitch;         //螺距
	DOUBLE dXTrip;          //行程 
	DOUBLE dXTripNegaTive;    //负向行程 
	DOUBLE dXMinVel;         //起始速度
	DOUBLE dXMaxVel;         //最大速度
	DOUBLE dXTAcc;         //加速时间
	DOUBLE dXTDec;         //减速时间
	DOUBLE dXTs;            //S段时间
	WORD  wYAxis;          //轴号
	DOUBLE dYPulsePer;      //每转脉冲
	DOUBLE dYPitch;         //螺距
	DOUBLE dYTrip;          //行程 
	DOUBLE dYTripNegaTive;  //负向行程 
	DOUBLE dYMinVel;         //起始速度
	DOUBLE dYMaxVel;         //最大速度
	DOUBLE dYTAcc;         //加速时间
	DOUBLE dYTDec;         //减速时间
	DOUBLE dYTs;            //S段时间
	WORD  wZAxis;          //轴号
	DOUBLE dZPulsePer;      //每转脉冲
	DOUBLE dZPitch;         //螺距
	DOUBLE dZTrip;          //行程 
	DOUBLE dZTripNegaTive;   //负向行程 
	DOUBLE dZMinVel;         //起始速度
	DOUBLE dZMaxVel;         //最大速度
	DOUBLE dZTAcc;         //加速时间
	DOUBLE dZTDec;         //减速时间
	DOUBLE dZTs;            //S段时间
	WORD  wWAxis;          //轴号
	DOUBLE dWPulsePer;      //每转脉冲
	DOUBLE dWPitch;         //螺距
	DOUBLE dWTrip;          //原点宽度
	DOUBLE dWMinTrip;          //最小宽度
	DOUBLE dWMinVel;         //起始速度
	DOUBLE dWMaxVel;         //最大速度
	DOUBLE dWTAcc;         //加速时间
	DOUBLE dWTDec;         //减速时间
	DOUBLE dWTs;            //S段时间
	WORD wDirect;  //进板方向，左侧进板 = 0，右侧进板 = 1
	DOUBLE dLX;    //左侧进板的原点位置
	DOUBLE dLY;
	DOUBLE dRX;    //右侧进板的原点位置
	DOUBLE dRY;
}AXIS_PARAMTER, *LPAXIS_PARAMTER;

typedef struct _GENERAL_PAR_
{
	CString strName;  //pcb名称即文件名称
	DOUBLE dPcbLen;   //pcb板的长度
	DOUBLE dPcbWide;  //宽度
	DOUBLE dPly;      //板子厚度(高度)
	DOUBLE dMarkX1;    //mark点
	DOUBLE dMarkY1;
	DOUBLE dMarkX2;
	DOUBLE dMarkY2;
	WORD   wIsGerber; //是否导入gerber 1=导入 0=不导入
	CString strGerberName; //Gerber文件名
}GENERAL_PAR, *LPGENERAL_PAR;

typedef struct _APPCON_SYS_
{
	DOUBLE dXSafe;   //X轴安全位置
	DOUBLE dYSafe;
	DOUBLE dZSafe;
	DOUBLE dZHigh;  //Z轴工作高度
}APPCON_SYS, *LPAPPCON_SYS;

typedef struct _CAMERA_PAR_
{
	UINT UiLenPixel;   //相机分辨率长
	UINT UiWidePixel;  //相机分辨率宽
	DOUBLE dCcdSizeL;    //相机感光芯片尺寸长
	DOUBLE dCcdSizeW;    //相机感光芯片尺寸宽
	DOUBLE dShotLen;     //相机镜头焦距
	DOUBLE dWorkDistance; //拍摄物到镜头的距离(安装距离)
	DOUBLE dLen;         //画面物理距离长(mm)
	DOUBLE dWide;         //画面物理距离宽(mm)
	DOUBLE dEpsTime;      //相机曝光时间(us)
	DOUBLE dGain;         //相机增益
	DOUBLE dRatio;        //远心镜头放大倍率
}CAMERA_PAR, *LPCAMERA_PAR;

typedef struct _STRUCT_UNITE_ 
{
	REAL_COOR Real_Coor;
	AXIS_PARAMTER axis_par;
	GENERAL_PAR gp;
	APPCON_SYS  as;
	CAMERA_PAR  cp;
}STRUCT_UNITE, *LPSTRUCT_UNITE;

CString GetFilePath();
void charTowchar(const char *chr, wchar_t *wchar, int size);   
void wcharTochar(const wchar_t *wchar, char *chr, int length);   
void DoEvents(); 
void ReadFileValue(FILE* pf, CString& strValue);
void ReadConFileValue(FILE* pf, CString& strValue);
void Err_Info(int Err_Code, CString& Err_Info);

//控制卡操作
WORD Axis_Move(WORD axis, double Min_Vel, double Max_Vel, double Tacc, double Tdec
	, long Tsacc, long Tsdec, double Dist, WORD posi_mode, WORD stType);//单轴运动
WORD Axis_Zero(WORD axis, double Min_Vel, double Max_Vel, double Tacc, double Tdec
	, long Tsacc, long Tsdec, WORD stType, WORD org_logic, WORD EZ_logic, WORD Mode,WORD EZ_count, WORD Home_Mode, WORD Vel_Mode);//单轴回原点
WORD EnableLmt();//限位使能,高低电平及制动方式
WORD BackOrigin(short sType);
WORD CheckPcb();
WORD MoveSafeCoor();
void StopMoveAxis();
void CloseBoard();

//相机操作
short OpenCamera();
short CloseCamera();
short StartGrab();
short StopGrab();
short ExecuteGrab(int iTimeOutMs = 500);
void SetGrabMode(short sMode);
void SetExposure(DOUBLE dTimes);
void SetGain(DOUBLE dValue);
DOUBLE GetGain();
void ImageReverseXY();
void ShowImage(HBITMAP hbitmap);
void ShowVideo(HBITMAP hbitmap);
void DrawLine();
void DrawLine(HDC hdc, CRect rc);
BOOL SaveNewImage(HBITMAP hbitmap, CString strFilename, int nColor);
void LoadImageX(CString strPath);
void NewShow(CDC* dc);

//程序启动初始化全部连接
int InitDevConn();

void DeleteDirectory(LPCTSTR path);

// CAutoDeviceApp:
// 有关此类的实现，请参阅 AutoDevice.cpp
//

class CAutoDeviceApp : public CWinApp
{
public:
	CAutoDeviceApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAutoDeviceApp theApp;
extern STRUCT_UNITE Struct_Unite;
extern BOOL m_bEnglish;
extern HANDLE m_hCom;

//相机对象
extern Camera_t* camera1;

//另一种形式的相机对象
extern CInstantCamera* camera2;
extern HBITMAP bmp1;
extern HBITMAP bmp2;
extern CPylonImage targetImage1;
extern CPylonBitmapImage destimage1;
extern CPylonImage targetImage2;
extern CPylonBitmapImage destimage2;

//Pylon SDK中的图像转换工具 (bayer转换),黑白相机不会用到。
extern CImageFormatConverter converter;//转换工具。
extern short sType; //0 = 初始值 1 = 拍照 2 = 连续采集

struct RectF 
{
	double left;
	double top;
	double right;
	double bottom;
};

//保存指定视野下所选择的焊盘位置
extern vector<RectF> g_vecWeldingPlateRect;
extern CPoint g_curPoint;
extern CString g_strFileName;