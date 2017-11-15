#if !defined(AFX_SCROLLBITMAPWND_H__B2A4B5F6_77F3_4C09_89F7_E82E36B2A341__INCLUDED_)
#define AFX_SCROLLBITMAPWND_H__B2A4B5F6_77F3_4C09_89F7_E82E36B2A341__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScrollBitmapWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScrollBitmapWnd window
/*--------------------------------------------------------------------------
	�Ի������������ʾͼƬ�ؼ�����ҪOnLButtonDown��Ϣ��ҪWS_CHILD | WS_VISIBLE | SS_NOTIFY
	������ʽ

  ʹ�÷�����
	DWORD lStyle = WS_CHILD | WS_VISIBLE | SS_NOTIFY;	
	m_PicShowWnd.Create(_T(""), lStyle, CRect(5, 5, 100, 30),this,102);
	m_PicShowWnd.ShowWindow(true);
	ͨ��������ƴ��ڵĴ�С��ͼƬ���ݼ����������Զ���Ӧ��
---------------------------------------------------------------------------*/

class CScrollBitmapWnd : public CStatic
{
// Construction
public:
	CScrollBitmapWnd();

private:
	CBitmap		m_BackBitmap;	//������λͼ
	BITMAP		m_bitMap;
	CRect		m_rectStaticClient;

	CSize		m_PicSize;
	CScrollBar  m_VScroll;
	CScrollBar  m_HScroll;

	CDC			m_dcMem;

	SCROLLINFO	m_horz,m_vert;
	bool		m_bVScrollShow;
	bool		m_bHScrollShow;

	int			m_offsetx,m_offsety;
	bool		m_bDraw;

	int			m_scrollSize;
	CPoint		m_clickPoint;

	void refLayOut();
	

// Attributes
public:
	bool setBitmap(HBITMAP bitMap);				//������ʾ��λͼ
	bool refBitmap(HBITMAP bitMap);				//ˢ����ʾ��λͼ����Ҫ��setBitmapͬ����С��λͼ
	CPoint getClickPoint();						//�����굥����λ��
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrollBitmapWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScrollBitmapWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CScrollBitmapWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCROLLBITMAPWND_H__B2A4B5F6_77F3_4C09_89F7_E82E36B2A341__INCLUDED_)
