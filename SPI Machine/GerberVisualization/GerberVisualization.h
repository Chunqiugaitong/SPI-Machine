
// GerberVisualization.h : main header file for the GerberVisualization application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CGerberVisualizationApp:
// See GerberVisualization.cpp for the implementation of this class
//

class CGerberVisualizationApp : public CWinApp
{
public:
	CGerberVisualizationApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGerberVisualizationApp theApp;
