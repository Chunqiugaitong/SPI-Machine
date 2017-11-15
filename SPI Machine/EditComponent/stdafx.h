
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
//                                �Զ�����
//#include "../Common/GxmcDatabase.h"
#include "../Common/Gerber.h"


struct RectF 
{
	double left;
	double top;
	double right;
	double bottom;
};
typedef struct _REAL_COOR_ //ʵʱ����ֵ
{
	DOUBLE dx;
	DOUBLE dy;
	DOUBLE dz;
	DOUBLE dw;
}REAL_COOR, *LPREAL_COOR;

typedef struct _AXIS_PARAMTER_ //�����������
{
	WORD  wXAxis;          //���
	DOUBLE dXPulsePer;      //ÿת����
	DOUBLE dXPitch;         //�ݾ�
	DOUBLE dXTrip;          //�г� 
	DOUBLE dXTripNegaTive;    //�����г� 
	DOUBLE dXMinVel;         //��ʼ�ٶ�
	DOUBLE dXMaxVel;         //����ٶ�
	DOUBLE dXTAcc;         //����ʱ��
	DOUBLE dXTDec;         //����ʱ��
	DOUBLE dXTs;            //S��ʱ��
	WORD  wYAxis;          //���
	DOUBLE dYPulsePer;      //ÿת����
	DOUBLE dYPitch;         //�ݾ�
	DOUBLE dYTrip;          //�г� 
	DOUBLE dYTripNegaTive;  //�����г� 
	DOUBLE dYMinVel;         //��ʼ�ٶ�
	DOUBLE dYMaxVel;         //����ٶ�
	DOUBLE dYTAcc;         //����ʱ��
	DOUBLE dYTDec;         //����ʱ��
	DOUBLE dYTs;            //S��ʱ��
	WORD  wZAxis;          //���
	DOUBLE dZPulsePer;      //ÿת����
	DOUBLE dZPitch;         //�ݾ�
	DOUBLE dZTrip;          //�г� 
	DOUBLE dZTripNegaTive;   //�����г� 
	DOUBLE dZMinVel;         //��ʼ�ٶ�
	DOUBLE dZMaxVel;         //����ٶ�
	DOUBLE dZTAcc;         //����ʱ��
	DOUBLE dZTDec;         //����ʱ��
	DOUBLE dZTs;            //S��ʱ��
	WORD  wWAxis;          //���
	DOUBLE dWPulsePer;      //ÿת����
	DOUBLE dWPitch;         //�ݾ�
	DOUBLE dWTrip;          //ԭ����
	DOUBLE dWMinTrip;          //��С���
	DOUBLE dWMinVel;         //��ʼ�ٶ�
	DOUBLE dWMaxVel;         //����ٶ�
	DOUBLE dWTAcc;         //����ʱ��
	DOUBLE dWTDec;         //����ʱ��
	DOUBLE dWTs;            //S��ʱ��
	WORD wDirect;  //���巽�������� = 0���Ҳ���� = 1
	DOUBLE dLX;    //�������ԭ��λ��
	DOUBLE dLY;
	DOUBLE dRX;    //�Ҳ�����ԭ��λ��
	DOUBLE dRY;
}AXIS_PARAMTER, *LPAXIS_PARAMTER;

typedef struct _GENERAL_PAR_
{
	CString strName;  //pcb���Ƽ��ļ�����
	DOUBLE dPcbLen;   //pcb��ĳ���
	DOUBLE dPcbWide;  //���
	DOUBLE dPly;      //���Ӻ��(�߶�)
	DOUBLE dMarkX1;    //mark��
	DOUBLE dMarkY1;
	DOUBLE dMarkX2;
	DOUBLE dMarkY2;
	WORD   wIsGerber; //�Ƿ���gerber 1=���� 0=������
	CString strGerberName; //Gerber�ļ���
}GENERAL_PAR, *LPGENERAL_PAR;

typedef struct _APPCON_SYS_
{
	DOUBLE dXSafe;   //X�ᰲȫλ��
	DOUBLE dYSafe;
	DOUBLE dZSafe;
	DOUBLE dZHigh;  //Z�Ṥ���߶�
}APPCON_SYS, *LPAPPCON_SYS;

typedef struct _CAMERA_PAR_
{
	UINT UiLenPixel;   //����ֱ��ʳ�
	UINT UiWidePixel;  //����ֱ��ʿ�
	DOUBLE dCcdSizeL;    //����й�оƬ�ߴ糤
	DOUBLE dCcdSizeW;    //����й�оƬ�ߴ��
	DOUBLE dShotLen;     //�����ͷ����
	DOUBLE dWorkDistance; //�����ﵽ��ͷ�ľ���(��װ����)
	DOUBLE dLen;         //����������볤(mm)
	DOUBLE dWide;         //������������(mm)
	DOUBLE dEpsTime;      //����ع�ʱ��(us)
	DOUBLE dGain;         //�������
	DOUBLE dRatio;        //Զ�ľ�ͷ�Ŵ���
}CAMERA_PAR, *LPCAMERA_PAR;

typedef struct _STRUCT_UNITE_ 
{
	REAL_COOR Real_Coor;
	AXIS_PARAMTER axis_par;
	GENERAL_PAR gp;
	APPCON_SYS  as;
	CAMERA_PAR  cp;
}STRUCT_UNITE, *LPSTRUCT_UNITE;

//��Ԫ�����༭���淢�Ͳ���
enum TRANSLATEPARAMETER
{
	//���̵��߼�����
	enumPadLogicPos,
	//PCB����ز���
	enumStruct_Unite,
	//�����ļ����ݿ�
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


