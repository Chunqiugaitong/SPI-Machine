// MeasureView.cpp : implementation file
//

#include "stdafx.h"
#include "AutoDevice.h"
#include "MeasureView.h"


// CMeasureView

IMPLEMENT_DYNCREATE(CMeasureView, CFormView)

CMeasureView::CMeasureView()
	: CFormView(CMeasureView::IDD)
{

}

CMeasureView::~CMeasureView()
{
}

void CMeasureView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMeasureView, CFormView)
END_MESSAGE_MAP()


// CMeasureView diagnostics

#ifdef _DEBUG
void CMeasureView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMeasureView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMeasureView message handlers
