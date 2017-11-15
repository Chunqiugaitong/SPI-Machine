
// AutoDevice.h : AutoDevice Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������

#define TXT_FILENAME    _T("SetParameter.txt")
#define INI_FILENAME    _T("PwConfig.ini")
#define INFO_ICON           (MB_OK | MB_ICONINFORMATION)
#define ALERT_ERR           _T("Error")
#define ALERT_INFOR         _T("Information")
#define ALERT_ICON          (MB_OK | MB_ICONEXCLAMATION)
#define ALERT_CONFIRM       (MB_OKCANCEL | MB_ICONEXCLAMATION)
#define ALERT_CONFIRMQ      (MB_OKCANCEL | MB_ICONQUESTION)
#define RUNING       100 //���˶���
#define NOCAMERA 101 //û�ҵ����
#define NOOPEN   102 //���δ��
#define NOGRAB   103 //û�п�ʼ�ɼ�  
#define TIMEOUT  104 //��ʱ
#define ISOPENED 106 //����Ѿ���
#define AUTOEXIT 254  //�˳��Զ�����
#define RAILBARRIER 105 //������а�
#define LEFTDOWNTOUT   107//�󵲰������½���ʱ
#define LEFTUPTOUT     108//�󵲰�����������ʱ
#define RIGHTDOWNTOUT  109//�ҵ��������½���ʱ
#define RIGHTUPTOUT    110//�ҵ�������������ʱ
#define MIDDLEDOWNTOUT  111//�а������½���ʱ
#define MIDDLEUPTOUT    112//�а�����������ʱ
#define DOOROPEN        113//��δ�ر�
#define SAVEFAIL        114//����ͼƬʧ��
#define ENTERTIMEOUT    115//�ȴ����峬ʱ
#define CONNECTERR      116//û���ҵ����ƿ�,���߿��ƿ��쳣
#define OPENCOMERR      117//�򿪴���ʧ��,���鴮���Ƿ���ȷ���ߴ����Ѿ���ȷ���Ӳ��ϵ�

#define WM_UPDATE_INFO  WM_USER+101 //�Զ�����Ϣ����״̬����ʾ
#define WM_UPDATEPCBMAP (WM_USER + 1000)//���ͻ���Mark����Ϣ



#define MENUNUM  6    //�˵�����
//ͨ�����������ƽ����
#define LOWLEVEL   0  //�͵�ƽ   ��Ч
#define HIGHLEVEL  1  //�ߵ�ƽ   ��Ч

//��ʱʱ�䶨��
#define TIME_OUT		1000
#define SLEEPTIME       300
#define ENTERTOT        8000 
#define CAMERATIME       50

//IO
#define IN1        1
#define IN2        2
#define IN3        3
#define LEFTIN4     4    //�󵲰�
#define RIGHTIN5    5    //�ҵ���
#define MIDDLEIN6   6    //�а�
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
#define LEFTOUT6     6    //�󵲰�
#define RIGHTOUT7    7    //�ҵ���
#define MIDDLEOUT8   8    //�а�
#define OUT9        9
#define OUT10       10
#define OUT11       11


#include <Pylon/PylonIncludes.h>
using namespace Pylon;
#include <pylon/PylonGUI.h>


#include <pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
using namespace Basler_UsbCameraParams;

//�½����̽��� PcbMap����ö��
enum UPDATEPCBMAP
{
	//����Mark1
	enumDrawMark1,
	//����Mark2
	enumDrawMark2,
	//����Fov
	enumUpdateFov,
	//����PcbMap
	enumUpdatePcbMap
};
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

CString GetFilePath();
void charTowchar(const char *chr, wchar_t *wchar, int size);   
void wcharTochar(const wchar_t *wchar, char *chr, int length);   
void DoEvents(); 
void ReadFileValue(FILE* pf, CString& strValue);
void ReadConFileValue(FILE* pf, CString& strValue);
void Err_Info(int Err_Code, CString& Err_Info);

//���ƿ�����
WORD Axis_Move(WORD axis, double Min_Vel, double Max_Vel, double Tacc, double Tdec
	, long Tsacc, long Tsdec, double Dist, WORD posi_mode, WORD stType);//�����˶�
WORD Axis_Zero(WORD axis, double Min_Vel, double Max_Vel, double Tacc, double Tdec
	, long Tsacc, long Tsdec, WORD stType, WORD org_logic, WORD EZ_logic, WORD Mode,WORD EZ_count, WORD Home_Mode, WORD Vel_Mode);//�����ԭ��
WORD EnableLmt();//��λʹ��,�ߵ͵�ƽ���ƶ���ʽ
WORD BackOrigin(short sType);
WORD CheckPcb();
WORD MoveSafeCoor();
void StopMoveAxis();
void CloseBoard();

//�������
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

//����������ʼ��ȫ������
int InitDevConn();

void DeleteDirectory(LPCTSTR path);

// CAutoDeviceApp:
// �йش����ʵ�֣������ AutoDevice.cpp
//

class CAutoDeviceApp : public CWinApp
{
public:
	CAutoDeviceApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CAutoDeviceApp theApp;
extern STRUCT_UNITE Struct_Unite;
extern BOOL m_bEnglish;
extern HANDLE m_hCom;

//�������
extern Camera_t* camera1;

//��һ����ʽ���������
extern CInstantCamera* camera2;
extern HBITMAP bmp1;
extern HBITMAP bmp2;
extern CPylonImage targetImage1;
extern CPylonBitmapImage destimage1;
extern CPylonImage targetImage2;
extern CPylonBitmapImage destimage2;

//Pylon SDK�е�ͼ��ת������ (bayerת��),�ڰ���������õ���
extern CImageFormatConverter converter;//ת�����ߡ�
extern short sType; //0 = ��ʼֵ 1 = ���� 2 = �����ɼ�

struct RectF 
{
	double left;
	double top;
	double right;
	double bottom;
};

//����ָ����Ұ����ѡ��ĺ���λ��
extern vector<RectF> g_vecWeldingPlateRect;
extern CPoint g_curPoint;
extern CString g_strFileName;