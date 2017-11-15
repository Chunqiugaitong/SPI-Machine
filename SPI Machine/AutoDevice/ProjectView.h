#pragma once
#include "afxwin.h"



// CProjectView form view

class CProjectView : public CFormView
{
	DECLARE_DYNCREATE(CProjectView)

public:
	CProjectView();           // protected constructor used by dynamic creation
	virtual ~CProjectView();

public:
	enum { IDD = IDD_PROJECT_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editBox;
};


