
// AutoDeviceDoc.cpp : CAutoDeviceDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "AutoDevice.h"
#endif

#include "AutoDeviceDoc.h"
#include <propkey.h>

#include "AutoDeviceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAutoDeviceDoc

IMPLEMENT_DYNCREATE(CAutoDeviceDoc, CDocument)

BEGIN_MESSAGE_MAP(CAutoDeviceDoc, CDocument)
	ON_COMMAND(ID_IMPORT, &CAutoDeviceDoc::OnImport)
END_MESSAGE_MAP()


// CAutoDeviceDoc 构造/析构

CAutoDeviceDoc::CAutoDeviceDoc()
{
	// TODO: 在此添加一次性构造代码

	m_bIsImported = false;
}

CAutoDeviceDoc::~CAutoDeviceDoc()
{
}

BOOL CAutoDeviceDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}


// CAutoDeviceDoc 序列化

void CAutoDeviceDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CAutoDeviceDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CAutoDeviceDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CAutoDeviceDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CAutoDeviceDoc 诊断

#ifdef _DEBUG
void CAutoDeviceDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAutoDeviceDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CAutoDeviceDoc 命令
void CAutoDeviceDoc::GetPadCluster()
{
	return;
}


void CAutoDeviceDoc::OnImport()
{
	// TODO: Add your command handler code here
	if (!m_gxmcGerber.GetPadCluster().empty())
	{
		m_gxmcGerber.ReleasePadCluster();
	}
	if (!m_mapPadCluster.empty())
	{
		std::map<int,Gxmc_Gerber::PadCluster> mapPadCluster;
		m_mapPadCluster.swap(mapPadCluster);
	}

	CFrameWndEx *pMain = (CFrameWndEx *)AfxGetMainWnd();
	CAutoDeviceView *pNewActiveView=(CAutoDeviceView*)pMain->GetActiveView();//取得新视图
// 	if (!pNewActiveView->IsKindOf(RUNTIME_CLASS(CAutoDeviceView)))
// 	{
// 	}
	
	int nFlag = m_gxmcGerber.ReadFromDatabase();
	if (nFlag < 0)
	{
		AfxMessageBox(L"焊盘数据读取失败!");
	}
	else
	{
		m_mapPadCluster = m_gxmcGerber.GetPadCluster();
		m_clustersMargin = m_gxmcGerber.GetClustersMargin();
		m_vecMark = m_gxmcGerber.GetMarkPoints();
		m_bIsImported = true;
		pNewActiveView->m_bIsAutoDeviceViewActive = true;
		pNewActiveView->Invalidate();
		pNewActiveView->UpdateWindow();
	}

	return;
}
