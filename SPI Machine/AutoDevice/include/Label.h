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
	CLabel& SetBkColor(COLORREF crBkgnd);	//���ñ�������ɫ
	CLabel& SetTextColor(COLORREF crText);	//�����ı�����ɫ
	CLabel& SetText(const CString& strText);	//���ÿؼ����ı�
	CLabel& SetFontBold(BOOL bBold);			//�Ƿ�����ֵĿ���
	CLabel& SetFontName(const CString& strFont);	//�����facename
	CLabel& SetFontUnderline(BOOL bSet);	//�»��ߵĿ���
	CLabel& SetFontItalic(BOOL bSet);		//б���ֵĿ���
	CLabel& SetFontSize(int nSize);
	CLabel& SetSunken(BOOL bSet);		//�³���־
	CLabel& SetBorder(BOOL bSet);		//�߿�
	CLabel& FlashText(BOOL bActivate);	//�������ֵ�����
	CLabel& FlashBackground(BOOL bActivate);
	CLabel& SetLink(BOOL bLink);		//�Ƿ���������
	CLabel& SetLinkCursor(HCURSOR hCursor);	//�����ӵĹ��

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
	FlashType	m_Type;		//Ϊö�����͵ı���
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
