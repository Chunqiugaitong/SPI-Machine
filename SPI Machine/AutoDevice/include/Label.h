#if !defined(AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_)
#define AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Label.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLabel window
enum FlashType {None, Text, Background };

class CLabel : public CStatic
{
// Construction
public:
	CLabel();
	CLabel& SetBkColor(COLORREF crBkgnd);	//设置背景的颜色
	CLabel& SetTextColor(COLORREF crText);	//设置文本的颜色
	CLabel& SetText(const CString& strText);	//设置控件的文本
	CLabel& SetFontBold(BOOL bBold);			//是否粗体字的开关
	CLabel& SetFontName(const CString& strFont);	//字体的facename
	CLabel& SetFontUnderline(BOOL bSet);	//下划线的开关
	CLabel& SetFontItalic(BOOL bSet);		//斜体字的开关
	CLabel& SetFontSize(int nSize);
	CLabel& SetSunken(BOOL bSet);		//下沉标志
	CLabel& SetBorder(BOOL bSet);		//边框
	CLabel& FlashText(BOOL bActivate);	//闪动文字的属性
	CLabel& FlashBackground(BOOL bActivate);
	CLabel& SetLink(BOOL bLink);		//是否允许超连接
	CLabel& SetLinkCursor(HCURSOR hCursor);	//超连接的光标

// Attributes
public:
protected:
	void ReconstructFont();
	COLORREF	m_crText;
	HBRUSH		m_hBrush;
	HBRUSH		m_hwndBrush;
	LOGFONT		m_lf;
	CFont		m_font;
	CString		m_strText;
	BOOL		m_bState;
	BOOL		m_bTimer;
	BOOL		m_bLink;
	FlashType	m_Type;		//为枚举类型的变量
	HCURSOR		m_hCursor;
			// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLabel)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_)
