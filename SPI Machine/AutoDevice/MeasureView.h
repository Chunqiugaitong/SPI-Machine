#pragma once
#include "resource.h"


// CMeasureView form view

class CMeasureView : public CFormView
{
	DECLARE_DYNCREATE(CMeasureView)

public:
	CMeasureView();           // protected constructor used by dynamic creation
	virtual ~CMeasureView();

public:
	enum { IDD = IDD_MEASURE_VIEW };
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


