
// AutoDeviceView.h : CAutoDeviceView 类的接口
//

#pragma once

#include "resource.h"
#include "include/Label.h"
#include "afxwin.h"
#include "AxisMoveDlg.h"
#include "include/Gerber.h"
#include "../GxmcCode/GxmcGerber.h"
#include "ImageProcess/PcbInspect.h"

DWORD WINAPI AutoDeviceThread(LPVOID);
DWORD WINAPI AutoEnterThread(LPVOID);

class CAutoDeviceView : public CFormView
{
public: // 仅从序列化创建
	CAutoDeviceView();
	DECLARE_DYNCREATE(CAutoDeviceView)

public:
	enum{ IDD = IDD_AUTODEVICE_VIEW };

// 特性
public:
	CAutoDeviceDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CAutoDeviceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCardOpen();
	afx_msg void OnSetAxisPar();
	afx_msg void OnSetCheckPar();
	afx_msg void OnControlAxisZero();
	afx_msg void OnControlAxisMove();
	afx_msg void OnSetAppCon();
	afx_msg void OnSetCameraPar();
	afx_msg void OnCameraOpen();
	afx_msg void OnCameraGrab();
	afx_msg void OnCameraExec();
	afx_msg void OnSaveImage();
	afx_msg void OnLoadImage();
	afx_msg void OnBnClickedBtnInto();
	afx_msg void OnCbnSelchangeComboAutotype();
	afx_msg void OnBnClickedBtnAutocheck();
	afx_msg void OnBnClickedBtnSuspend();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnIsReticle();
	afx_msg void On2DView();
	afx_msg void OnStsBarMsg();
	afx_msg LRESULT OnUpdateInfo(WPARAM wParam,LPARAM lParam);
	afx_msg void OnManualPro();
	afx_msg void OnAutoPro();
	virtual BOOL PreTranslateMessage(MSG* pMsg);


	void WriteData();//参数设置写入文件
	short LoadData();//读取参数文件
	void InitLabel();
	void GetRealTime();
	void ResMacBtn();
	void WriteConData();//检测参数写入文件
	short LoadConData(CString strPathName);//读取检测参数文件
	void  LoadDefFile();
	void  SetFont();//设置控件字体
	void  EnableControl(BOOL bActive);
	void  IntoEnable(BOOL bActive);
	WORD  CollectImage();
	void  UserPostMessage(CString strMsg, WORD wType);
	void  IsShowReticle(BOOL bCheck);
	void  MenuEnable(BOOL bEnable);
	void  CheckResust(WORD wType);
	//void  DrawLine();

	//控制卡IO操作相关

	WORD GetOneBitIn(WORD wBit);
	WORD GetOneBitOut(WORD wBit);
	WORD SetOneBitOut(WORD wBit, WORD wBitValue);
	WORD  SetAndGetIO(int iTimeout, WORD iBitIn, WORD iBitOut, WORD iBitOutValue);
	WORD WaitTime(int iMilliSecond);
	WORD LightControl(WORD wColour, WORD wBitValue);

	WORD WaitAndControl(WORD wInNo, WORD wOutNo, WORD wOutValue);
	WORD NotWaitAndControl(WORD wInNo, WORD wOutNo, WORD wOutValue);
	WORD WaitAndControlSleep(WORD wInNo, WORD wOutNo, WORD wOutValue);
	WORD WaitLoop(WORD wInNo);
	WORD NotWaitLoop(WORD wInNo);


	CRect m_rect;
	//BOOL bInit;
	BOOL bAutoFlag;//自动运行标志 true为自动运行
	CFont      m_font;
	CLabel  m_StrLabelName;
	CLabel  m_StrLabelDirect;
	CLabel  m_StrLabelSize;
	CLabel  m_StrLabelSts;
	CLabel  m_StrLabelOkNg;
	CStatic m_StrReal_Image;
	CStatic m_StrImage;
	CString strXPer;
	CString strYPer;
	CString strZPer;
	CString strWPer;
	CComboBox m_strCombAutoType;
	HANDLE hAutoDeviceThread;
	HANDLE hAutoEnterThread;
	CString strPath;
	BOOL   bConnect; //控制卡 true=连接上，false=未连接
	BOOL   bZero;   //控制卡是否做过归零操作，true = 归零 false = 未归零
	BOOL   bCheck;  //是否显示十字线，true = 显示 false = 不显示
	CAxisMoveDlg* m_AmDlg;

	//****************************读取gerber数据库******************************************/
	Gxmc_Gerber::CGxmcGerber m_gxmcGerber;
	ScreenStruct m_navigateScreen;
	ScreenStruct m_fovScreen;
	bool m_bIsFileOpen;
	CRect m_navigateMargin;
	CRect m_fovMargin;
	int m_nClusterIndex;

	bool m_bShowFov;
	bool m_bShovNavigation;
	CDC* m_pNavigateDC;
	CDC* m_pFovDC;
	
	vector<CRect> m_vecClusterRect;
//	std::map<int,Gxmc_Gerber::PadCluster> m_mapPadCluster;
	
	//视图是否处于激活状态
	bool m_bIsAutoDeviceViewActive;
	//Fov里的焊盘是否检测完成
	bool m_bIsFovInspected;

	void InitialAllFovMargin();
	void InitialDraw();
	void InitialFovScreen();
	void InitialNavigateScreen();
	void DrawGerber();
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnExitSystem();
};

class CSampleImageEventHandler : public CImageEventHandler
{
	//相机采集图像的回调函数。
	//每当相机采集到一张图像，就会进入一次该回调函数。
public:
	virtual void OnImageGrabbed( CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
	{
		bmp1 = NULL;
		bmp2 = NULL;

		//CPcbInspect pcbInspect;

		//ptrGrabResult为相机采集图像的结果。
		//判断采集是否成功。
		if(false == ptrGrabResult->GrabSucceeded())
			//string str = ptrGrabResult->GetErrorDescription();
		 //   int  iCode = 0;
			//iCode = ptrGrabResult->GetErrorCode();
			return;
		try
		{
			//该if中的第三个条件，用于多相机使用同一个回调函数，判断当前的采集结果是哪一个相机给出的。可以忽略。
			if(camera1!=NULL && camera1->IsOpen()&&
				camera.GetDeviceInfo().GetSerialNumber() == camera1->GetDeviceInfo().GetSerialNumber())
			{

				//ptrGrabResult->GetBuffer() 为图像的原始数据。
				//如果相机是黑白模式，该数据可以直接使用。
				//如果相机是彩色Bayer模式，则需要进行convert转换。
				//uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();

				//Convert 转换。
				converter.Convert(targetImage1, ptrGrabResult);//进行图像转换。

				//Pylon::DisplayImage(1, ptrGrabResult);//pylon显示实时采集画面

				//uint8_t* targetImagebuffer = (uint8_t*) targetImage1.GetBuffer();

				//测试代码，可以忽略。
				//for (int i = 0; i < 2592 * 2048*4; i++)
				//{
				//	targetImagebuffer[i+10];
				//}

				destimage1.CopyImage(targetImage1);
				bmp1 =  destimage1.Detach();//可供显示的图像。

				/////////////////////////////////////////////////////////////////////////////
				//IplImage* img = pcbInspect.Bmp2Ipl(bmp1);
			//	imshow(L"hh",Mat(img));

				if (1 == sType)
				{
					ShowImage(bmp1);
				}
				else if (2 == sType)
				{
					ShowImage(bmp1);
					::DeleteObject(bmp1);//释放资源
				}
				//ShowVideo(bmp1);
				//SaveNewImage(bmp1, _T("C:\\111.bmp"), 8);

				//将bmp显示在 图1 上。
			}
			//多相机时，区分采集结果是哪一个相机给出的。可以忽略。
			if(camera2!=NULL && camera2->IsOpen()&&
				camera.GetDeviceInfo().GetSerialNumber() == camera2->GetDeviceInfo().GetSerialNumber())
			{
				//将bmp显示在 图2 上。
				converter.Convert(targetImage2,ptrGrabResult);//进行图像转换。

				destimage2.CopyImage(targetImage2);

				bmp2 =  destimage2.Detach();//可供显示的图像。
			}
		}

		catch (const GenericException &e)
		{
			// Error handling
			cerr << "An exception occurred." << endl
				<< e.GetDescription() << endl;
		}
	}
};


#ifndef _DEBUG  // AutoDeviceView.cpp 中的调试版本
inline CAutoDeviceDoc* CAutoDeviceView::GetDocument() const
   { return reinterpret_cast<CAutoDeviceDoc*>(m_pDocument); }
#endif

