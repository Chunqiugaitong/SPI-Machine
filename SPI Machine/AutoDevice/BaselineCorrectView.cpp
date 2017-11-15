// BaselineCorrectView.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "BaselineCorrectView.h"


// CBaselineCorrectView

IMPLEMENT_DYNCREATE(CBaselineCorrectView, CFormView)

CBaselineCorrectView::CBaselineCorrectView()
	: CFormView(CBaselineCorrectView::IDD)
{

}

CBaselineCorrectView::~CBaselineCorrectView()
{
}

void CBaselineCorrectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBaselineCorrectView, CFormView)
END_MESSAGE_MAP()


// CBaselineCorrectView diagnostics

#ifdef _DEBUG
void CBaselineCorrectView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaselineCorrectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBaselineCorrectView message handlers
