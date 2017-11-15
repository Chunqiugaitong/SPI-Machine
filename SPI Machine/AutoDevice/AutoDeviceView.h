
// AutoDeviceView.h : CAutoDeviceView ��Ľӿ�
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
public: // �������л�����
	CAutoDeviceView();
	DECLARE_DYNCREATE(CAutoDeviceView)

public:
	enum{ IDD = IDD_AUTODEVICE_VIEW };

// ����
public:
	CAutoDeviceDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CAutoDeviceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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


	void WriteData();//��������д���ļ�
	short LoadData();//��ȡ�����ļ�
	void InitLabel();
	void GetRealTime();
	void ResMacBtn();
	void WriteConData();//������д���ļ�
	short LoadConData(CString strPathName);//��ȡ�������ļ�
	void  LoadDefFile();
	void  SetFont();//���ÿؼ�����
	void  EnableControl(BOOL bActive);
	void  IntoEnable(BOOL bActive);
	WORD  CollectImage();
	void  UserPostMessage(CString strMsg, WORD wType);
	void  IsShowReticle(BOOL bCheck);
	void  MenuEnable(BOOL bEnable);
	void  CheckResust(WORD wType);
	//void  DrawLine();

	//���ƿ�IO�������

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
	BOOL bAutoFlag;//�Զ����б�־ trueΪ�Զ�����
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
	BOOL   bConnect; //���ƿ� true=�����ϣ�false=δ����
	BOOL   bZero;   //���ƿ��Ƿ��������������true = ���� false = δ����
	BOOL   bCheck;  //�Ƿ���ʾʮ���ߣ�true = ��ʾ false = ����ʾ
	CAxisMoveDlg* m_AmDlg;

	//****************************��ȡgerber���ݿ�******************************************/
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
	
	//��ͼ�Ƿ��ڼ���״̬
	bool m_bIsAutoDeviceViewActive;
	//Fov��ĺ����Ƿ������
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
	//����ɼ�ͼ��Ļص�������
	//ÿ������ɼ���һ��ͼ�񣬾ͻ����һ�θûص�������
public:
	virtual void OnImageGrabbed( CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult)
	{
		bmp1 = NULL;
		bmp2 = NULL;

		//CPcbInspect pcbInspect;

		//ptrGrabResultΪ����ɼ�ͼ��Ľ����
		//�жϲɼ��Ƿ�ɹ���
		if(false == ptrGrabResult->GrabSucceeded())
			//string str = ptrGrabResult->GetErrorDescription();
		 //   int  iCode = 0;
			//iCode = ptrGrabResult->GetErrorCode();
			return;
		try
		{
			//��if�еĵ��������������ڶ����ʹ��ͬһ���ص��������жϵ�ǰ�Ĳɼ��������һ����������ġ����Ժ��ԡ�
			if(camera1!=NULL && camera1->IsOpen()&&
				camera.GetDeviceInfo().GetSerialNumber() == camera1->GetDeviceInfo().GetSerialNumber())
			{

				//ptrGrabResult->GetBuffer() Ϊͼ���ԭʼ���ݡ�
				//�������Ǻڰ�ģʽ�������ݿ���ֱ��ʹ�á�
				//�������ǲ�ɫBayerģʽ������Ҫ����convertת����
				//uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();

				//Convert ת����
				converter.Convert(targetImage1, ptrGrabResult);//����ͼ��ת����

				//Pylon::DisplayImage(1, ptrGrabResult);//pylon��ʾʵʱ�ɼ�����

				//uint8_t* targetImagebuffer = (uint8_t*) targetImage1.GetBuffer();

				//���Դ��룬���Ժ��ԡ�
				//for (int i = 0; i < 2592 * 2048*4; i++)
				//{
				//	targetImagebuffer[i+10];
				//}

				destimage1.CopyImage(targetImage1);
				bmp1 =  destimage1.Detach();//�ɹ���ʾ��ͼ��

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
					::DeleteObject(bmp1);//�ͷ���Դ
				}
				//ShowVideo(bmp1);
				//SaveNewImage(bmp1, _T("C:\\111.bmp"), 8);

				//��bmp��ʾ�� ͼ1 �ϡ�
			}
			//�����ʱ�����ֲɼ��������һ����������ġ����Ժ��ԡ�
			if(camera2!=NULL && camera2->IsOpen()&&
				camera.GetDeviceInfo().GetSerialNumber() == camera2->GetDeviceInfo().GetSerialNumber())
			{
				//��bmp��ʾ�� ͼ2 �ϡ�
				converter.Convert(targetImage2,ptrGrabResult);//����ͼ��ת����

				destimage2.CopyImage(targetImage2);

				bmp2 =  destimage2.Detach();//�ɹ���ʾ��ͼ��
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


#ifndef _DEBUG  // AutoDeviceView.cpp �еĵ��԰汾
inline CAutoDeviceDoc* CAutoDeviceView::GetDocument() const
   { return reinterpret_cast<CAutoDeviceDoc*>(m_pDocument); }
#endif

