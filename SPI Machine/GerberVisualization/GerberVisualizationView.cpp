
// GerberVisualizationView.cpp : implementation of the CGerberVisualizationView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GerberVisualization.h"
#endif

#include "GerberVisualizationDoc.h"
#include "GerberVisualizationView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGerberVisualizationView

IMPLEMENT_DYNCREATE(CGerberVisualizationView, CView)

	BEGIN_MESSAGE_MAP(CGerberVisualizationView, CView)
		// Standard printing commands
		ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
		ON_WM_PAINT()
		ON_COMMAND(IDR_SCALE_FIT, &CGerberVisualizationView::OnIdrScaleFit)
		ON_COMMAND(IDR_SCALE_UP, &CGerberVisualizationView::OnIdrScaleUp)
		ON_COMMAND(IDR_SCALE_DOWN, &CGerberVisualizationView::OnIdrScaleDown)
		ON_COMMAND(IDR_MOVE, &CGerberVisualizationView::OnIdrMove)
		ON_WM_SIZE()
		ON_COMMAND(ID_READ_GERBER, &CGerberVisualizationView::OnReadGerber)
		ON_WM_ERASEBKGND()
		ON_WM_LBUTTONDOWN()
		ON_WM_MOUSEMOVE()
		ON_WM_LBUTTONUP()
		ON_WM_CREATE()
		ON_WM_CLOSE()
		ON_COMMAND(ID_QuitSystem, &CGerberVisualizationView::OnQuitsystem)
		ON_COMMAND(IDR_HIDE, &CGerberVisualizationView::OnIdrHide)
		ON_COMMAND(IDR_OPTIMUMPATH, &CGerberVisualizationView::OnIdrOptimumpath)
		ON_COMMAND(IDR_SELECTITEMS, &CGerberVisualizationView::OnIdrSelectitems)
		ON_COMMAND(IDR_SELECT_MARKPOINTS, &CGerberVisualizationView::OnIdrSelectMarkpoints)
		ON_COMMAND(ID_EXPORT_PROJECT, &CGerberVisualizationView::OnExportProject)
		ON_COMMAND(ID_SELECT_PAD, &CGerberVisualizationView::OnSelectPad)
		ON_WM_RBUTTONDOWN()
		ON_COMMAND(ID_INVERT_SELECT_PAD, &CGerberVisualizationView::OnInvertSelectPad)
		ON_COMMAND(ID_SHOW_PAD, &CGerberVisualizationView::OnShowPad)
		ON_COMMAND(ID_HIDE_PAD, &CGerberVisualizationView::OnHidePad)
		ON_COMMAND(ID_MOVE_PADS, &CGerberVisualizationView::OnMovePads)
		ON_COMMAND(ID_SCALEDOWN_PADS, &CGerberVisualizationView::OnScaledownPads)
		ON_COMMAND(ID_SCALEUP_PADS, &CGerberVisualizationView::OnScaleupPads)
		ON_COMMAND(IDR_TEST, &CGerberVisualizationView::OnIdrTest)
	END_MESSAGE_MAP()

	// CGerberVisualizationView construction/destruction

	CGerberVisualizationView::CGerberVisualizationView()
	{
		// TODO: add construction code here

	}

	CGerberVisualizationView::~CGerberVisualizationView()
	{
	}

#pragma region 打印 诊断相关
	// CGerberVisualizationView printing
	BOOL CGerberVisualizationView::OnPreparePrinting(CPrintInfo* pInfo)
	{
		// default preparation
		return DoPreparePrinting(pInfo);
	}
	void CGerberVisualizationView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		// TODO: add extra initialization before printing
	}
	void CGerberVisualizationView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		// TODO: add cleanup after printing
	}
	// CGerberVisualizationView diagnostics
#ifdef _DEBUG
	void CGerberVisualizationView::AssertValid() const
	{
		CView::AssertValid();
	}

	void CGerberVisualizationView::Dump(CDumpContext& dc) const
	{
		CView::Dump(dc);
	}

	CGerberVisualizationDoc* CGerberVisualizationView::GetDocument() const // non-debug version is inline
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGerberVisualizationDoc)));
		return (CGerberVisualizationDoc*)m_pDocument;
	}
#endif //_DEBUG
#pragma endregion 


	BOOL CGerberVisualizationView::PreCreateWindow(CREATESTRUCT& cs)
	{
		// TODO: Modify the Window class or styles here by modifying
		//  the CREATESTRUCT cs

		//这里默认为白色画刷 WHITE_BRUSH ,现在修改为m_bgcolor
		cs.lpszClass=AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,0,
			(HBRUSH)::GetStockObject(RGB(1,4,1)),0); 
		return CView::PreCreateWindow(cs);
	}

	int CGerberVisualizationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CView::OnCreate(lpCreateStruct) == -1)
			return -1;
		// TODO:  Add your specialized creation code here

		m_realMarginRect.left = -85.000;
		m_realMarginRect.top = 65.000;
		m_realMarginRect.right = 86.000;
		m_realMarginRect.bottom = -130.000;
		m_IsMoveGerberGraph = false;
		m_IsScale = false;
		m_bIsFitShow = false;
		m_bOriginalShow = false;
		m_IsDrawSelectRect = false;
		m_IsDrawRect = false;

		m_backgroundColor = RGB(1,4,1);

		m_Screen.factor = 1;
		m_Screen.offsetX = 0.0;
		m_Screen.offsetY = 0.0;

		//DC参数初始化
		m_penWidth   = 1;
		m_penStyle   = PS_SOLID;
		m_penColor   = RGB(0,255,0);
		m_brushColor = RGB(0,255,0);
		m_nLayer     = 0;//图层索引
		m_bIsFilled  = true;

		return 0;
	}

	// CGerberVisualizationView drawing

	void CGerberVisualizationView::OnDraw(CDC* /*pDC*/)
	{
		CGerberVisualizationDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;

		// TODO: add draw code for native data here
	}

	void CGerberVisualizationView::OnSize(UINT nType, int cx, int cy)
	{
		CView::OnSize(nType, cx, cy);
		// TODO: Add your message handler code here
		GetClientRect(&m_viewRect);
		m_bIsFitShow = true;
		Invalidate();
	}

	CGxmcGerber* g_pGerber;
	//视图界面初始化，同对话框OnInitialDlg
	void CGerberVisualizationView::OnInitialUpdate()
	{
		CView::OnInitialUpdate();

		// TODO: Add your specialized code here and/or call the base class
		m_pDrawThread = NULL;
		g_pGerber = new CGxmcGerber();

	}

	// CGerberVisualizationView message handlers

	//窗口重绘前(调用onpaint()),窗口背景重绘
	BOOL CGerberVisualizationView::OnEraseBkgnd(CDC* pDC)
	{
		// TODO: Add your message handler code here and/or call default
	//	DrawBackGround(); 

		return CView::OnEraseBkgnd(pDC);
	}
	void CGerberVisualizationView::DrawBackGround()
	{
		CBrush   brush(m_backgroundColor);  
		pDC->FillRect(&m_viewRect,&brush); 
	}

	UINT DrawFunction(LPVOID lpParam)
	{
		CGerberVisualizationView* view =(CGerberVisualizationView*)lpParam;
		view->pDC = view->GetDC();
		view->m_viewMemDC.SetBkColor(view->m_backgroundColor);
		view->DrawBackGround();
		if (!view->m_listDataBlock.empty())
		{
			CPen pen(view->m_penStyle,view->m_penWidth,view->m_penColor);
			view->m_viewMemDC.SelectObject(&pen);
			CBrush brush(view->m_brushColor);
			view->m_viewMemDC.SelectObject(&brush);
			g_pGerber->DisplayGerber(view->m_viewMemDC,view->m_Screen,view->m_listDataBlock);
		}
		
		if (!view->m_mapCluster.empty())
		{
			CPen wpen(PS_SOLID,1,RGB(0,0,255));
			CPen* pOldPen = view->m_viewMemDC.SelectObject(&wpen);
			view->m_viewMemDC.SelectStockObject(NULL_BRUSH);
			g_pGerber->DisplayFovPath(view->m_viewMemDC,view->m_Screen,view->m_mapCluster);
		}
		
		if (!view->m_vecSelectRect.empty())
		{
			CPen wpen(PS_SOLID,1,RGB(255,255,255));
			CPen* pOldPen = view->m_viewMemDC.SelectObject(&wpen);
			g_pGerber->DisplaySelectRect(view->m_viewMemDC,view->m_Screen,view->m_vecSelectRect);
		}
		
 		if (!view->m_gxmcGerber.GetPadCluster().empty())
 		{
 			CPen wpen(PS_SOLID,1,RGB(255,255,255));
 			CPen* pOldPen = view->m_viewMemDC.SelectObject(&wpen);
 			g_pGerber->DisplayPadCluster(view->m_viewMemDC,view->m_Screen,view->m_gxmcGerber.GetPadCluster());
 		}

		(view->pDC)->BitBlt(0,0,view->m_viewRect.Width(),view->m_viewRect.Height(),&view->m_viewMemDC,0,0,SRCCOPY);
		view->ReleaseDC(view->pDC);

		view->m_viewMemDC.DeleteDC();
		view->m_viewBmp.DeleteObject();
		return 0;
	}

	void CGerberVisualizationView::OnPaint()
	{
		if (m_viewMemDC.GetSafeHdc() == NULL)
		{
			CPaintDC dc(this); // device context for painting
			// TODO: Add your message handler code here
			// Do not call CView::OnPaint() for painting messages
			m_viewMemDC.CreateCompatibleDC(NULL);
			m_viewBmp.CreateCompatibleBitmap(&dc,m_viewRect.Width(),m_viewRect.Height());
			m_viewMemDC.SelectObject(&m_viewBmp);
		}

		if (m_bIsFitShow)
		{
			double factorX = m_viewRect.Width()/(m_realMarginRect.right - m_realMarginRect.left);
			double factorY = m_viewRect.Height()/(m_realMarginRect.top - m_realMarginRect.bottom);
			m_Screen.factor = factorX < factorY ? factorX : factorY;
			m_Screen.startX = m_realMarginRect.left;
			m_Screen.startY = m_realMarginRect.bottom;
			m_bIsFitShow = false;
		}
		if (m_bOriginalShow)
		{
			m_Screen.factor = 1;
			m_Screen.startX = m_realMarginRect.left;
			m_Screen.startY = m_realMarginRect.bottom;
			m_bOriginalShow = false;
		}

		if (m_pDrawThread)
		{
			m_pDrawThread->ResumeThread();
			//TerminateThread(m_pDrawThread->m_hThread, 0);
			WaitForSingleObject(m_pDrawThread->m_hThread,INFINITE);
			delete m_pDrawThread;
			//m_pDrawThread = NULL;
		}

		if (m_viewMemDC.GetSafeHdc() == NULL)
		{
			CPaintDC dc(this); // device context for painting
			// TODO: Add your message handler code here
			// Do not call CView::OnPaint() for painting messages
			m_viewMemDC.CreateCompatibleDC(NULL);
			m_viewBmp.CreateCompatibleBitmap(&dc,m_viewRect.Width(),m_viewRect.Height());
			m_viewMemDC.SelectObject(&m_viewBmp);
		}

		m_pDrawThread = AfxBeginThread(DrawFunction,this,THREAD_PRIORITY_BELOW_NORMAL,0,CREATE_SUSPENDED);
		m_pDrawThread->m_bAutoDelete = FALSE;
		m_pDrawThread->ResumeThread();

	}
	void CGerberVisualizationView::OnReadGerber()
	{
		// TODO: Add your command handler code here
		CFileDialog fileDlg(true,L"gbr",L"*.gbr",0,L"gerber文件(*.gbr)|*.gbr|所有文件(*.*)|*.*||",this);
		CString strFileName;

		if (IDOK == fileDlg.DoModal())
		{
			strFileName = fileDlg.GetFolderPath() + "\\" + fileDlg.GetFileName();
		} 
		else
		{
			return;
		}
		m_realMarginRect=INITI_RECT;
		m_gxmcGerber.ReleaseADStyleMap();
		if (!m_listDataBlock.empty())
		{
			m_gxmcGerber.ReleaseDataBlock(m_listDataBlock);
		}
		if (!m_mapCluster.empty())
		{
			m_gxmcGerber.ReleaseClusterMap(m_mapCluster);
		}

		m_gxmcGerber.ReadGerberFile(strFileName,m_listDataBlock,m_realMarginRect);

		m_realMarginRect.left -= 15;
		m_realMarginRect.top += 15;
		m_realMarginRect.right += 15;
		m_realMarginRect.bottom -= 15;
		m_Screen.realMarginRect = m_realMarginRect;

		m_MouseOldPoint =m_MouseCurPoint= CPoint(0,0);
		m_delt = 0;
		m_SelectRect = CRect(0,0,0,0);
		m_bIsFitShow = true;
	//	m_bOriginalShow = true;
		m_IsMoveGerberGraph = false;
		Invalidate();
		//释放堆上内存
		return;

	}
	void CGerberVisualizationView::OnClose()
	{
		// TODO: Add your message handler code here and/or call default

		CView::OnClose();
	}
	void CGerberVisualizationView::OnQuitsystem()
	{
		// TODO: Add your command handler code here
		if (MessageBox(L"确定退出系统吗？",L"梅村提示",MB_YESNO | MB_DEFBUTTON2) == IDYES)  
		{  
			if (!m_listDataBlock.empty())
			{
				m_gxmcGerber.ReleaseDataBlock(m_listDataBlock);
			}
			if (!m_mapCluster.empty())
			{
				m_gxmcGerber.ReleaseClusterMap(m_mapCluster);
			}
			m_gxmcGerber.ReleaseADStyleMap();
			if (g_pGerber != NULL)
			{
				delete g_pGerber;
				g_pGerber = NULL;
			}
			
			::PostMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_CLOSE,0,0);  
		} 
	}

	#pragma region 工具栏
    void CGerberVisualizationView::OnIdrScaleFit()
	{
		// TODO: Add your command handler code here
		Invalidate();
	}
	void CGerberVisualizationView::OnIdrScaleUp()
	{
		// TODO: Add your command handler code here
		OnScaleupPads();
	}
	void CGerberVisualizationView::OnIdrScaleDown()
	{
		// TODO: Add your command handler code here
		OnScaledownPads();
	}
	void CGerberVisualizationView::OnIdrMove()
	{
		// TODO: Add your command handler code here
		m_IsMoveGerberGraph = true;
		m_IsScale = false;
		// m_bIsFitShow = true;
	}
    
	void CGerberVisualizationView::OnIdrHide()
	{
		// TODO: Add your command handler code here
		m_IsDrawRect = true;
		m_IsScale = false;
		m_isInvertSelection = false;
	}
	void CGerberVisualizationView::OnIdrSelectitems()
	{
		// TODO: Add your command handler code here
		m_IsDrawRect = true;
		m_IsScale = false;
		m_isInvertSelection = true;
	}
	void CGerberVisualizationView::OnIdrOptimumpath()
	{
		// TODO: Add your command handler code here
		double ViewWidth = 22;
		double ViewHeight = 22;
		m_gxmcGerber.GetFovPath(m_listDataBlock,ViewWidth,ViewHeight,m_mapCluster);
		if (m_mapCluster.empty())
		{
			return;
		}
		Invalidate();
	}

	void CGerberVisualizationView::OnIdrSelectMarkpoints()
	{
		// TODO: Add your command handler code here
		m_gxmcGerber.SelectMarkPoints(m_listDataBlock,m_vecSelectRect,m_Screen,m_mapMarkPoint);

	}
	void CGerberVisualizationView::OnIdrTest()
	{
		// TODO: Add your command handler code here
		m_gxmcGerber.ReadFromDatabase();
		Invalidate();
	}
	#pragma endregion 

	#pragma region 鼠标事件
    void CGerberVisualizationView::OnLButtonDown(UINT nFlags, CPoint point)
	{
		// TODO: Add your message handler code here and/or call default
		if (m_IsMoveGerberGraph)
		{
			m_MouseOldPoint = m_MouseCurPoint = point;
		}

		if (m_IsScale)
		{
			m_Screen.factor *= m_delt;
			Invalidate();
		}
		if (m_IsDrawRect)
		{
			m_bPoint = point;
			m_ePoint = point;

			m_IsDrawRect = false;
			m_IsDrawSelectRect = true;
		}

		CView::OnLButtonDown(nFlags, point);
	}
	void CGerberVisualizationView::OnMouseMove(UINT nFlags, CPoint point)
	{
		// TODO: Add your message handler code here and/or call default
		m_MouseCurPoint = point;

		double px;double py;
		LogicViewToWorldView(point.x,point.y,px,py);

		CString str;  
		str.Format(L"实际坐标   X %.3f   Y %.3f ", px, py);
		CString str1;  
		str1.Format(L"屏幕坐标   X %d   Y %d ", point.x, point.y);
		CString str2;  
		str2.Format(L"缩放倍率   %lf", m_Screen.factor);

		CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->m_wndStatusBar.SetPaneText(0,str);//在状态栏上面设置文本信息 
		pFrame->m_wndStatusBar.SetPaneText(1,str1); 
		pFrame->m_wndStatusBar.SetPaneText(2,str2); 
		pFrame->m_wndStatusBar.SetPaneInfo(0,ID_INDICATOR_EXT,SBPS_NORMAL,220);
		pFrame->m_wndStatusBar.SetPaneInfo(1,ID_INDICATOR_EXT,SBPS_NORMAL,170);
		pFrame->m_wndStatusBar.SetPaneInfo(2,ID_INDICATOR_EXT,SBPS_NORMAL,150);

		if (m_IsDrawSelectRect)
		{
			CDC* pdc = GetDC();
			pdc->SelectStockObject(BLACK_PEN);
			pdc->SelectStockObject(NULL_BRUSH);
			pdc->Rectangle(&m_SelectRect);
			m_SelectRect = CRect(m_bPoint,m_ePoint);
			m_ePoint = point;
			CPen wpen(PS_SOLID,1,RGB(255,255,255));
			pdc->SelectObject(&wpen);
			pdc->SelectStockObject(NULL_BRUSH);
			pdc->Rectangle(&m_SelectRect);
			ReleaseDC(pdc);
		}

		CView::OnMouseMove(nFlags, point);
	}
	void CGerberVisualizationView::OnLButtonUp(UINT nFlags, CPoint point)
	{
		// TODO: Add your message handler code here and/or call default
		if (m_IsDrawSelectRect)
		{
			double left,top,right,bottom;
			PointD startPoint,stopPoint;
			LogicViewToWorldView(m_bPoint.x,m_bPoint.y,     left,top);
			LogicViewToWorldView(m_ePoint.x,m_ePoint.y, right,bottom);
			startPoint = PointD(left,bottom);
			stopPoint  = PointD(right,top);
			GxmcRect rect(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,false,startPoint,stopPoint);

			m_vecSelectRect.push_back(rect);
 			m_IsDrawSelectRect = false;
			m_gxmcGerber.SelectPads(m_listDataBlock,m_vecSelectRect,m_Screen,m_isInvertSelection);

			Invalidate();
		}
		
		if (m_IsMoveGerberGraph)
		{
			int dX = m_MouseCurPoint.x - m_MouseOldPoint.x;
			int dY = m_MouseCurPoint.y - m_MouseOldPoint.y;

			m_Screen.offsetX = dX==0 ? 0 : dX/m_Screen.factor;
			m_Screen.offsetY = dY==0 ? 0 : dY/m_Screen.factor;

			m_Screen.startX  -=m_Screen.offsetX;
			m_Screen.startY  +=m_Screen.offsetY;

			//m_viewPortOriginX += dX;
			//m_viewPortOriginY += dY;
			//m_IsMoveGerberGraph = false;	
			Invalidate();	
		}

		CView::OnLButtonUp(nFlags, point);
	}
	void CGerberVisualizationView::OnRButtonDown(UINT nFlags, CPoint point)
	{
		// TODO: Add your message handler code here and/or call default
		//清除选择矩形框
		if (!m_vecSelectRect.empty())
		{
			m_IsDrawRect = false;
			std::vector<GxmcRect> vecRectTemp;
			m_vecSelectRect.swap(vecRectTemp);
			Invalidate();
		}
		if (m_IsScale)
		{
			m_IsScale = false;
		}

		CView::OnRButtonDown(nFlags, point);
	}
    #pragma endregion 
	
    #pragma region 菜单栏
	//将FOV坐标及其焊盘坐标导出至数据库
	void CGerberVisualizationView::OnExportProject()
	{
		// TODO: Add your command handler code here
		m_gxmcGerber.ExportFovData(m_listDataBlock,m_mapCluster,m_mapMarkPoint);

	}
	//绘制框选矩形
	void CGerberVisualizationView::OnSelectPad()
	{
		// TODO: Add your command handler code here
		m_IsDrawRect = true;
		m_isInvertSelection = false;
	}
	//反选焊盘
	void CGerberVisualizationView::OnInvertSelectPad()
	{
		// TODO: Add your command handler code here
		m_IsDrawRect = true;
		m_isInvertSelection = true;
	}

	//显示所选焊盘
	void CGerberVisualizationView::OnShowPad()
	{
		// TODO: Add your command handler code here
		m_gxmcGerber.HidePads(m_listDataBlock,m_vecSelectRect,m_Screen,m_isInvertSelection);
		if (!m_vecSelectRect.empty())
		{
			m_IsDrawRect = false;
			std::vector<GxmcRect> vecRectTemp;
			m_vecSelectRect.swap(vecRectTemp);
		}
		Invalidate();
	}

    //隐藏所选焊盘
	void CGerberVisualizationView::OnHidePad()
	{
		// TODO: Add your command handler code here
		m_gxmcGerber.HidePads(m_listDataBlock,m_vecSelectRect,m_Screen,m_isInvertSelection);
		if (!m_vecSelectRect.empty())
		{
			m_IsDrawRect = false;
			std::vector<GxmcRect> vecRectTemp;
			m_vecSelectRect.swap(vecRectTemp);
		}
		Invalidate();
	}

	//移动视图
	void CGerberVisualizationView::OnMovePads()
	{
		// TODO: Add your command handler code here
	}

	//缩小视图
	void CGerberVisualizationView::OnScaledownPads()
	{
		// TODO: Add your command handler code here
		m_IsScale = true;
		m_delt = 0.5;
		m_Screen.offsetX = 0;
		m_Screen.offsetY = 0;
	}

	//放大视图
	void CGerberVisualizationView::OnScaleupPads()
	{
		// TODO: Add your command handler code here
		m_IsScale = true;
		m_delt = 2;
		m_Screen.offsetX = 0;
		m_Screen.offsetY = 0;
	}
    #pragma endregion 

	void CGerberVisualizationView::LogicViewToWorldView(int lX,int lY,double& wX,double& wY)
	{
		double realHeight = (m_Screen.realMarginRect.top - m_Screen.realMarginRect.bottom)*m_Screen.factor;
		wX = (double)lX/m_Screen.factor + m_Screen.startX;
		wY = (double)(realHeight - lY)/m_Screen.factor + m_Screen.startY;
	}
	void CGerberVisualizationView::WorldViewToLogicView(double wX,double wY,int& lX,int& lY)
	{
		double realHeight = (m_Screen.realMarginRect.top - m_Screen.realMarginRect.bottom)*m_Screen.factor;

		lX = (wX - m_Screen.startX)*m_Screen.factor;
		lY =  realHeight - (wY - m_Screen.startY)*m_Screen.factor;
	}

	
