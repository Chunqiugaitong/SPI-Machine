
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//                                自定义区
//#include "../Common/GxmcDatabase.h"
#include "../Common/Gerber.h"


struct RectF 
{
	double left;
	double top;
	double right;
	double bottom;
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


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


