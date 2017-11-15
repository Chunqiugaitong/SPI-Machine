// FovPathSelectDlg.cpp : implementation file
//
#include "stdafx.h"
#include "EditComponent.h"
#include "FovPathSelectDlg.h"
#include "afxdialogex.h"


// CFovPathSelectDlg dialog

IMPLEMENT_DYNAMIC(CFovPathSelectDlg, CDialogEx)

CFovPathSelectDlg::CFovPathSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFovPathSelectDlg::IDD, pParent)
{

}

CFovPathSelectDlg::~CFovPathSelectDlg()
{
}

void CFovPathSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_TOPLEFT, m_radio_TopLeft);
	DDX_Control(pDX, IDC_RADIO_TOPRIGHT, m_radio_TopRight);
	DDX_Control(pDX, IDC_RADIO_BOTTOMLEFT, m_radio_BottomLeft);
	DDX_Control(pDX, IDC_RADIO_BOTTOMRIGHT, m_radio_BottomRight);

	DDX_Control(pDX, IDC_RADIO_HORIZON, m_radio_Horizon);
	DDX_Control(pDX, IDC_RADIO_VERTICAL, m_radio_Vertical);
}


BEGIN_MESSAGE_MAP(CFovPathSelectDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE_FOV_PATH, &CFovPathSelectDlg::OnBnClickedButtonCalculateFovPath)
END_MESSAGE_MAP()

//vector<RectF> g_vecSelectPad;
// CFovPathSelectDlg message handlers

BOOL CFovPathSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  Add extra initialization here

	m_radio_TopLeft.SetCheck(1);
	m_radio_Horizon.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFovPathSelectDlg::OnBnClickedButtonCalculateFovPath()
{
	// TODO: Add your control notification handler code here
	if (m_radio_TopLeft.GetCheck())
	{
		AfxMessageBox(L"haha");
	}
	if (m_radio_TopRight.GetCheck())
	{
		AfxMessageBox(L"haha");
	}
	if (m_radio_BottomLeft.GetCheck())
	{
		AfxMessageBox(L"haha");
	}
	if (m_radio_BottomRight.GetCheck())
	{
		AfxMessageBox(L"haha");
	}

// 	Gerber gerber;
// 	vector<_Polygon> vecPolygon;
// 	_Rectangle RealPolygonMargin;
// 	for (int i = 0; i<g_vecSelectPad.size(); i++)
// 	{
// 
// 	}
// 	
// 	//°ÑPolygon´æµ½MapÀï
// 	map<int,_Polygon> mapPolygon;
// 	int polygonIndex = 0;
// 	vector<_Polygon>:: iterator itorPolygon = vecPolygon.begin();
// 	for ( ; itorPolygon != vecPolygon.end();itorPolygon++)
// 	{
// 		mapPolygon.insert(make_pair<int,_Polygon>(polygonIndex++,*itorPolygon));
// 	}
// 	int rate = 1.5;
// 	int clusterIndex = 0;
// 	int ViewWidth  = 4*rate;
// 	int ViewHeight = 4*rate;
// 	map<int,_Cluster> mapCluster;
// 	gerber.SplitGerberRegion(mapCluster,clusterIndex,mapPolygon,ViewWidth,ViewHeight);

}

