#pragma once
#include "resource.h"


// CBaselineCorrectView form view

class CBaselineCorrectView : public CFormView
{
	DECLARE_DYNCREATE(CBaselineCorrectView)

public:
	CBaselineCorrectView();           // protected constructor used by dynamic creation
	virtual ~CBaselineCorrectView();

public:
	enum { IDD = IDD_BASELINECORRECT_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


