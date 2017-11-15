
// AutoDeviceDoc.cpp : CAutoDeviceDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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


// CAutoDeviceDoc ����/����

CAutoDeviceDoc::CAutoDeviceDoc()
{
	// TODO: �ڴ����һ���Թ������

	m_bIsImported = false;
}

CAutoDeviceDoc::~CAutoDeviceDoc()
{
}

BOOL CAutoDeviceDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}


// CAutoDeviceDoc ���л�

void CAutoDeviceDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CAutoDeviceDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
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

// ������������֧��
void CAutoDeviceDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:  strSearchContent = _T("point;rectangle;circle;ole object;")��
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

// CAutoDeviceDoc ���

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

// CAutoDeviceDoc ����
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
	CAutoDeviceView *pNewActiveView=(CAutoDeviceView*)pMain->GetActiveView();//ȡ������ͼ
// 	if (!pNewActiveView->IsKindOf(RUNTIME_CLASS(CAutoDeviceView)))
// 	{
// 	}
	
	int nFlag = m_gxmcGerber.ReadFromDatabase();
	if (nFlag < 0)
	{
		AfxMessageBox(L"�������ݶ�ȡʧ��!");
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
