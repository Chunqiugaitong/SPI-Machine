// ProjectView.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "ProjectView.h"


// CProjectView

IMPLEMENT_DYNCREATE(CProjectView, CFormView)

CProjectView::CProjectView()
	: CFormView(CProjectView::IDD)
{

}

CProjectView::~CProjectView()
{
}

void CProjectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editBox);
}

BEGIN_MESSAGE_MAP(CProjectView, CFormView)
END_MESSAGE_MAP()


// CProjectView diagnostics

#ifdef _DEBUG
void CProjectView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CProjectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CProjectView message handlers
