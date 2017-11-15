// C2DViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "C2DViewDlg.h"
#include "afxdialogex.h"


// C2DViewDlg dialog

IMPLEMENT_DYNAMIC(C2DViewDlg, CDialog)

C2DViewDlg::C2DViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C2DViewDlg::IDD, pParent)
{
	this->pvtkMFCWindow     = NULL;
    this->pvtkDataSetReader = vtkPolyDataReader::New(); 
    // Create the the renderer, window and interactor objects.
    this->pvtkRenderer    = vtkRenderer::New();
    // Create the the objects used to form the visualisation.
    this->pvtkDataMapper  = vtkPolyDataMapper::New();
    this->pvtkActor      = vtkActor::New();
    this->pvtkActor2D    = vtkActor2D::New();
    this->pvtkTextMapper  = vtkTextMapper::New();
    this->scalarWidget = vtkScalarBarWidget::New(); 
	this->pvtkActor->RotateX(-60);
	//this->pvtkActor->RotateY(45);
	this->pvtkActor->RotateZ(60);
}

C2DViewDlg::~C2DViewDlg()
{
	 if (this->pvtkDataSetReader)  this->pvtkDataSetReader->Delete();
     // Delete the the renderer, window and interactor objects.
     if (this->pvtkRenderer)      this->pvtkRenderer->Delete();
     // Delete the the objects used to form the visualisation.
     if (this->pvtkDataMapper)  this->pvtkDataMapper->Delete();
     if (this->pvtkActor)      this->pvtkActor->Delete();
     if (this->pvtkActor2D)      this->pvtkActor2D->Delete();
     if (this->pvtkTextMapper)    this->pvtkTextMapper->Delete();
     if(this->scalarWidget) this->scalarWidget->Delete();
     if (this->pvtkMFCWindow)
     {
   		delete this->pvtkMFCWindow;
     }
}

void C2DViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGEVIEW, m_strimageView);
	DDX_Control(pDX, IDC_STATIC_3DVIEW, m_str3DView);
	DDX_Control(pDX, IDC_STATIC_2DVIEW, m_str2DView);
	DDX_Control(pDX, IDC_LIST_RESULT, m_strListResult);
}


BEGIN_MESSAGE_MAP(C2DViewDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// C2DViewDlg message handlers


BOOL C2DViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    InitOwnStatic();
	InitListCtrl();
	//m_strNoTin.SetBkColor(RGB(225, 0, 0));
	LoadMeshView(_T(""),5);
	//RefreshPIC(_T("img.bmp"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void C2DViewDlg::InitOwnStatic()
{
	GenerOwnStatic(m_strNoTin, IDC_STATIC_NOTIN);
	GenerOwnStatic(m_strVolumeSmall, IDC_STATIC_VOLUMESMALL);
	GenerOwnStatic(m_strVolumeBig, IDC_STATIC_VOLUMEBIG);
	GenerOwnStatic(m_strHighToH, IDC_STATIC_HIGHTOH);
	GenerOwnStatic(m_strHighToL, IDC_STATIC_HIGHTOL);
	GenerOwnStatic(m_strAreaBig, IDC_STATIC_AREABIG);
	GenerOwnStatic(m_strAreaSmall, IDC_STATIC_AREASMALL);
	GenerOwnStatic(m_strXOffset, IDC_STATIC_XOFFSET);
	GenerOwnStatic(m_strYOffset, IDC_STATIC_YOFFSET);
	GenerOwnStatic(m_strShortC, IDC_STATIC_SHORTC);
	GenerOwnStatic(m_strTTinBad, IDC_STATIC_TTINBAD);
	m_strVolumeSmall.SetBkColor(RGB(225, 0, 0));
	m_strHighToL.SetBkColor(RGB(225, 0, 0));
}

void C2DViewDlg::InitListCtrl()
{
	m_strListResult.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);//| LVS_EX_SUBITEMIMAGES);// | LVS_EX_TRACKSELECT);
	m_strListResult.InsertColumn(0, _T("焊盘编号"), LVCFMT_LEFT, 70);
	m_strListResult.InsertColumn(1, _T("缺陷"), LVCFMT_LEFT, 70);
	m_strListResult.InsertColumn(2, _T("测试结果"), LVCFMT_LEFT, 70);

	int Iindex = 0;
	CString strTemp = _T("");
	strTemp.Format(_T("%02d"), Iindex+1);
	m_strListResult.InsertItem(Iindex, strTemp);
	m_strListResult.SetItemText(Iindex, 1, _T("体积偏小"));
	m_strListResult.SetItemText(Iindex, 2, _T("NG"));
	Iindex++;
	strTemp.Format(_T("%02d"), Iindex+1);
	m_strListResult.InsertItem(Iindex, strTemp);
	m_strListResult.SetItemText(Iindex, 1, _T("高度过低"));
	m_strListResult.SetItemText(Iindex, 2, _T("NG"));
}

void C2DViewDlg::GenerOwnStatic(CLabel& str, long lId)
{
	str.SubclassDlgItem(lId, this);
	str.SetFontName(_T("Microsoft Sans Serif"));
	//m_strNoTin.SetFontSize(18);
	//str.SetFontBold(true);
	str.ModifyStyle(0, SS_CENTERIMAGE);
	//m_StrLabelName.SetTextColor(RGB(255, 0, 0));
	str.SetBkColor(RGB(0, 225, 0));
}

BOOL C2DViewDlg::LoadMeshView(CString fileName, float Nbound)
{
	this->pvtkRenderer->RemoveActor(this->pvtkActor);
	this->pvtkRenderer->RemoveActor(this->pvtkActor2D);
	this->pvtkRenderer->RemoveActor(scalarWidget->GetScalarBarActor());
	 
      
    //this->pvtkDataSetReader->SetFileName(L"");
    
    // execute object pipeline
#pragma region
	


	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    unsigned int GridSize = 60;
	int bound=GridSize/4;
	int bound0=GridSize/10;
    double xx, yy, zz;
    for(unsigned int x = 0; x < GridSize; x++)
    {
        for(unsigned int y = 0; y < GridSize; y++)
        {
              xx = x + vtkMath::Random(-.2, .2);
              yy = y + vtkMath::Random(-.2, .2);
              zz = vtkMath::Random(0.8, 1.5);
  	        if( y>bound &&y<GridSize-bound)
  	        {
  	            zz = vtkMath::Random(3.7, Nbound);
  	        }
  	        else if( y>bound0 &&y<GridSize-bound0)
  	        {
  	        
  	        }
  	        else
  	        {
  	            zz =0;
  	        }
  	      
              points->InsertNextPoint(xx, yy, zz);
  	   }
     }

	//vtkMFCWindow *pvtkMFCWindow = new vtkMFCWindow(mainWnd);
	if(!this->pvtkMFCWindow) 
	{
		this->pvtkMFCWindow = new vtkMFCWindow(this->GetDlgItem(IDC_STATIC_3DVIEW));
	}
	
	 
	this->pvtkDataMapper = vtkPolyDataMapper::New(); 
	pvtkMFCWindow->GetRenderWindow()->AddRenderer(pvtkRenderer); 
	vtkSmartPointer<vtkPolyData> inputPolyData1 = vtkSmartPointer<vtkPolyData>::New(); 
	inputPolyData1->SetPoints(points);

	  // Triangulate the grid points
  vtkSmartPointer<vtkDelaunay2D> delaunay = 
    vtkSmartPointer<vtkDelaunay2D>::New();
#if VTK_MAJOR_VERSION <= 5
  delaunay->SetInput(inputPolyData1);
#else
  delaunay->SetInputData(inputPolyData1);
#endif
  delaunay->Update();
  vtkPolyData* inputPolyData = delaunay->GetOutput();


	double bounds[6];
	inputPolyData->GetBounds(bounds);

	// Find min and max z
	double minz = bounds[4];
	double maxz = bounds[5];
	// Create the color map
	vtkSmartPointer<vtkLookupTable> colorLookupTable = vtkSmartPointer<vtkLookupTable>::New();
	colorLookupTable->SetTableRange(minz, maxz);
	colorLookupTable->Build();
	// Generate the colors for each point based on the color map
	vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

	for (int i = 0; i < inputPolyData->GetNumberOfPoints(); i++)
	{
		double p[3];
		inputPolyData->GetPoint(i, p);
		double dcolor[3];
		colorLookupTable->GetColor(p[2], dcolor);
		unsigned char color[3];
		for (unsigned int j = 0; j < 3; j++)
		{
			color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
		}
		colors->InsertNextTupleValue(color);
	}
	inputPolyData->GetPointData()->SetScalars(colors); 
	scalarWidget->GetScalarBarActor()->SetLookupTable(colorLookupTable);
	scalarWidget->GetScalarBarActor()->SetWidth(0.1);
	scalarWidget->GetScalarBarActor()->SetHeight(0.55);
	scalarWidget->GetScalarBarActor()->SetDisplayPosition(30, 150);

	pvtkDataMapper->SetInputConnection(inputPolyData->GetProducerPort());
	pvtkActor->SetMapper(pvtkDataMapper);



	//pvtkTextMapper->SetInput(pvtkPolyDataReader->GetFileName());
	pvtkTextMapper->GetTextProperty()->SetFontSize(12);
	pvtkActor2D->SetMapper(pvtkTextMapper);

	pvtkRenderer->SetBackground(1.0, 1.0, 1.0);
	pvtkRenderer->AddActor(pvtkActor);
	pvtkRenderer->AddActor(pvtkActor2D);
	pvtkRenderer->AddActor2D(scalarWidget->GetScalarBarActor());

	pvtkRenderer->ResetCamera();
#pragma endregion
    
    // update window
    if (this->pvtkMFCWindow)
      this->pvtkMFCWindow->RedrawWindow();
	return TRUE;
}
void C2DViewDlg::RefreshPIC(CString fileName)
{
	//CPaintDC dc(this); // device context for painting

	//CFileFind finder;
	//bool ret=finder.FindFile(fileName);
	 
    CImage	image;
    CRect	rect;
	//CString str;
	//str.Format(L"%d",ret);
	//MessageBox(str,L"",MB_OK);
	//fstream f;
	//f.open("test1.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	//f<<"bool:"<<ret<<endl;
	
    //根据路径载入图片
    image.Load(fileName);
    m_str2DView.GetClientRect(&rect);
    //ScreenToClient(&rect);
	if (!image.IsNull())
	{
		image.Draw(m_str2DView.GetDC()->m_hDC, rect);
	}
    //CDC* pDC = m_str2DView.GetWindowDC();
    //image.Draw(pDC->m_hDC, rect);
    //ReleaseDC(pDC);


	//HDC hSrcDC;
	//HDC hDesDC;
	//CRect rect;
	//CDC dcMem;
	//HBITMAP bmpTemp;

	////CStatic 控件的时候改成自绘模式或者将控件中Simple属性改为true, Picture控件时不需要此行代码
	////pMainView->m_StrReal_Image.ModifyStyle(0, BS_OWNERDRAW);
	//BITMAP bm; // 位图结构体，用于接受位图的信息
	//hDesDC = m_str2DView.GetDC()->m_hDC;
	//hSrcDC = CreateCompatibleDC(hDesDC);
	////hSrcDC = CreateCompatibleDC(NULL);
	//bmpTemp = (HBITMAP)::LoadImage(0, fileName, IMAGE_BITMAP, NULL, NULL, LR_LOADFROMFILE|LR_DEFAULTSIZE);

	//SelectObject(hSrcDC, bmpTemp);
	//GetObject(bmpTemp, sizeof(bm), &bm);  // 将位图信息填入BITMAP结构体

	//m_str2DView.GetClientRect(&rect);
	////m_str2DView.GetWindowRect(&rect);
	//ScreenToClient(&rect);


	//::SetStretchBltMode(hDesDC,COLORONCOLOR);
	//StretchBlt(hDesDC, 0, 0, rect.Width() ,rect.Height(), hSrcDC,       
	//	0, 0, bm.bmWidth,bm.bmHeight, SRCCOPY);

	//::DeleteObject(hDesDC);
	//::DeleteObject(hSrcDC);
}

void C2DViewDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CString strPath = _T("");
	strPath = GetFilePath();
	//strPath += '\\';
	strPath += _T("\img.bmp");
	//MessageBox(strPath);
	RefreshPIC(strPath);
	// Do not call CDialog::OnPaint() for painting messages
}
