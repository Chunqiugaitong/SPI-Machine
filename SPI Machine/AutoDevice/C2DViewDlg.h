#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "include/Label.h"
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkRenderer.h>
#include <vtkWin32OpenGLRenderWindow.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkTextProperty.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataMapper.h>

#include <vtkDelaunay2D.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkScalarBarWidget.h>
#include <vtkScalarBarActor.h>
#include <vtkSimplePointsWriter.h>
#include <vtkPolyDataWriter.h>
#include <vtkMFCWindow.h>




// C2DViewDlg dialog

class C2DViewDlg : public CDialog
{
	DECLARE_DYNAMIC(C2DViewDlg)

public:
	C2DViewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~C2DViewDlg();

// Dialog Data
	enum { IDD = IDD_2DVIEW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_strimageView;
	CStatic m_str3DView;
	CStatic m_str2DView;
	CListCtrl m_strListResult;
	CLabel m_strNoTin;
	CLabel m_strVolumeSmall;
	CLabel m_strVolumeBig;
	CLabel m_strHighToH;
	CLabel m_strHighToL;
	CLabel m_strAreaBig;
	CLabel m_strAreaSmall;
	CLabel m_strXOffset;
	CLabel m_strYOffset;
	CLabel m_strShortC;
	CLabel m_strTTinBad;
	void InitOwnStatic();
	void InitListCtrl();
	void GenerOwnStatic(CLabel& str, long lId);
	void Load3DView();
	void Load2DView();
private:
	vtkMFCWindow          *pvtkMFCWindow;
	vtkPolyDataReader     *pvtkDataSetReader;
	vtkRenderer           *pvtkRenderer;
	vtkPolyDataMapper     *pvtkDataMapper;
	vtkActor              *pvtkActor;
	vtkActor2D            *pvtkActor2D;
	vtkTextMapper         *pvtkTextMapper;
	vtkScalarBarWidget    *scalarWidget;
public:
	BOOL LoadMeshView(CString fileName, float Nbound); 
	void RefreshPIC(CString fileName);
	afx_msg void OnPaint();
};
