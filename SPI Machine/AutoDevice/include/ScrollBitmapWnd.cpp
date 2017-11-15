// ScrollBitmapWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ScrollBitmapWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScrollBitmapWnd

CScrollBitmapWnd::CScrollBitmapWnd()
{
	m_bVScrollShow = false;
	m_bHScrollShow = false;	

	m_bDraw = false;
	m_offsetx = 0;
	m_offsety = 0;
	m_PicSize = CSize(0,0);
	m_clickPoint = CPoint(0,0);

	m_scrollSize = 17;
}

CScrollBitmapWnd::~CScrollBitmapWnd()
{
	m_dcMem.DeleteDC();
	m_BackBitmap.DeleteObject();
}


BEGIN_MESSAGE_MAP(CScrollBitmapWnd, CStatic)
	//{{AFX_MSG_MAP(CScrollBitmapWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollBitmapWnd message handlers

bool CScrollBitmapWnd::setBitmap(HBITMAP bitMap)
{
	m_dcMem.DeleteDC();
	m_BackBitmap.DeleteObject();
	
    
	if(!m_BackBitmap.Attach(bitMap))
	{
		m_BackBitmap.DeleteObject();
		return false;
	}

	m_BackBitmap.GetBitmap(&m_bitMap);
	BITMAP bm;
	m_BackBitmap.GetBitmap(&bm);
	m_PicSize.cx = bm.bmWidth;
	m_PicSize.cy = bm.bmHeight;
 
	if(!m_dcMem.CreateCompatibleDC(NULL))
	{
		m_dcMem.DeleteDC();
		m_BackBitmap.DeleteObject();
		return false;
	}
// 	if (!m_BackBitmap.CreateBitmapIndirect(&m_bitMap))
// 	{
// 		m_BackBitmap.DeleteObject(); 
// 		m_dcMem.DeleteDC(); 
// 		m_bDraw = false;
// 		return false;
// 	}
// 
// 	m_BackBitmap.GetBitmap(&m_bitMap);
// 
// 	int lpLenth = m_bitMap.bmWidthBytes*nHeight*sizeof(BYTE);
// 	BYTE *Templp = (BYTE *) malloc(lpLenth);
// 
// 	bitMap->GetBitmapBits(lpLenth,Templp);
// 	m_BackBitmap.SetBitmapBits(lpLenth,Templp);
// 	
// 	if (Templp)
// 	{
// 		free(Templp);
// 	}	

	m_dcMem.SelectObject(&m_BackBitmap);

	m_bDraw = true;
	refLayOut();
	return true;
}
void CScrollBitmapWnd::refLayOut()
{
	CRect rect;
	GetClientRect(&rect);
	
	m_bHScrollShow = false;
	m_bVScrollShow = false;

	int nPageSize;
	if (m_PicSize.cx>rect.Size().cx)
	{
		m_bHScrollShow = true;
		if (m_PicSize.cy>rect.Size().cy-m_scrollSize)
		{
			m_bVScrollShow = true;
		}
	}
	if(m_PicSize.cy>rect.Size().cy)
	{
		m_bVScrollShow = true;
		if (m_PicSize.cx>rect.Size().cx-m_scrollSize)
		{
			m_bHScrollShow = true;
		}
	}

	m_horz.cbSize = sizeof(SCROLLINFO);
	m_horz.fMask = SIF_ALL;
	m_horz.nMin = 0;
	m_horz.nMax = m_PicSize.cx;
	nPageSize = rect.Size().cx;
	if (m_bVScrollShow)
	{
		nPageSize = rect.Size().cx - m_scrollSize;;
	}
	m_horz.nPage = nPageSize;  
	m_horz.nPos = 0;
	m_horz.nTrackPos=0;

	m_vert.cbSize = sizeof(SCROLLINFO);
	m_vert.fMask = SIF_ALL;
	m_vert.nMin = 0;
	
	m_vert.nMax = m_PicSize.cy;
	nPageSize = rect.Size().cy;
	if (m_bHScrollShow)
	{
		nPageSize = rect.Size().cy - m_scrollSize;
	}
	m_vert.nPage = nPageSize;

	m_vert.nPos = 0;
	m_vert.nTrackPos=0;
	

	m_HScroll.ShowWindow(m_bHScrollShow);
	m_VScroll.ShowWindow(m_bVScrollShow);
	
	if (m_bVScrollShow && m_bHScrollShow)
	{
		rect.right = rect.right - m_scrollSize;
		rect.bottom = rect.bottom - m_scrollSize;
	}
	else if (m_bHScrollShow)
	{
		rect.bottom = rect.bottom - m_scrollSize;
	}
	else if (m_bVScrollShow)
	{
		rect.right = rect.right - m_scrollSize;
	}

	int offsx,offsy;
	offsx = m_rectStaticClient.Size().cx - rect.Size().cx;
	offsy = m_rectStaticClient.Size().cy - rect.Size().cy;

	if (0 > offsx)
	{
		m_offsetx = m_offsetx + offsx;
	}
	if (0 > offsy)
	{
		m_offsety = m_offsety + offsy;
	}	
	m_offsetx = m_offsetx>0 ? m_offsetx:0;
	m_offsety = m_offsety>0 ? m_offsety:0;

	int nMax;

	nMax = (m_PicSize.cx-m_rectStaticClient.Size().cx)>0 ? (m_PicSize.cx-m_rectStaticClient.Size().cx):0;
	m_offsetx = m_offsetx<nMax ? m_offsetx:nMax;

	nMax = (m_PicSize.cy-m_rectStaticClient.Size().cy)>0 ? (m_PicSize.cy-m_rectStaticClient.Size().cy):0;
	m_offsety = m_offsety<nMax ? m_offsety:nMax;
	
	if (!m_bHScrollShow)
	{
		m_offsetx = 0;
	}
	if (!m_bVScrollShow)
	{
		m_offsety = 0;
	}

	m_VScroll.MoveWindow(rect.Size().cx,0,m_scrollSize,rect.Size().cy);
	m_VScroll.SetScrollInfo(&m_vert);
	m_VScroll.SetScrollPos(m_offsety);

	m_HScroll.MoveWindow(0,rect.Size().cy,rect.Size().cx,m_scrollSize);	
	m_HScroll.SetScrollInfo(&m_horz);
	m_HScroll.SetScrollPos(m_offsetx);	

	m_rectStaticClient = rect;
}

void CScrollBitmapWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (m_bDraw)
	{
		dc.BitBlt(0,0, m_rectStaticClient.Size().cx, m_rectStaticClient.Size().cy, &m_dcMem, m_offsetx, m_offsety, SRCCOPY);
	}
	// Do not call CStatic::OnPaint() for painting messages
}

BOOL CScrollBitmapWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	CRect rect;
	GetClientRect(&rect);

 	COLORREF color;
 	color = pDC->GetBkColor();

 	//限制绘图区域
	CRgn rgn,rgnStatic;
	rgn.CreateRectRgn(0,0,m_PicSize.cx,m_PicSize.cy);
	rgnStatic.CreateRectRgnIndirect(&m_rectStaticClient);

	pDC->SelectClipRgn(&rgnStatic);
	pDC->SelectClipRgn(&rgn,RGN_DIFF);

	//滚动条的右下角位置
	if (m_bHScrollShow && m_bVScrollShow)
	{
		CRgn rgnScroll;
		CRect scrollRect;
		scrollRect.left = rect.right - m_scrollSize;
		scrollRect.right = rect.right;
		scrollRect.top = rect.bottom - m_scrollSize;
		scrollRect.bottom = rect.bottom;
		rgnScroll.CreateRectRgnIndirect(&scrollRect);
		pDC->SelectClipRgn(&rgnScroll,RGN_OR);
	}

	pDC->FillSolidRect( &rect,RGB(192,192,192));
	pDC->SelectClipRgn(NULL);		

	pDC->SetBkColor(color);
	return true;
}

void CScrollBitmapWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	switch (nSBCode)
	{
	case SB_LINELEFT:
		m_offsetx -= 5;
		break;
	case SB_LINERIGHT:
		m_offsetx += 5;
		break;
	case SB_TOP:
		m_offsetx = 0;
		break;
	case SB_BOTTOM:
		m_offsetx = INT_MAX;
		break;
	case SB_THUMBTRACK:
		m_offsetx = nPos;
		break;
	}

	m_offsetx = m_offsetx>0 ? m_offsetx:0;	
	int nMax = m_PicSize.cx-m_rectStaticClient.Size().cx;
	m_offsetx = m_offsetx<nMax ? m_offsetx:nMax;

	m_HScroll.SetScrollPos(m_offsetx);
	InvalidateRect(&m_rectStaticClient);
	CStatic::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CScrollBitmapWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	switch (nSBCode)
	{
	case SB_LINEUP:
		m_offsety -= 5;
		break;
	case SB_LINEDOWN:
		m_offsety += 5;
		break;
	case SB_TOP:
		m_offsety = 0;
		break;
	case SB_BOTTOM:
		m_offsety = INT_MAX;
		break;
	case SB_THUMBTRACK:
		m_offsety = nPos;
		break;
	}
	m_offsety = m_offsety>0 ? m_offsety:0;
	int nMax = m_PicSize.cy-m_rectStaticClient.Size().cy;
	m_offsety = m_offsety<nMax ? m_offsety:nMax;

	m_VScroll.SetScrollPos(m_offsety);	
	InvalidateRect(&m_rectStaticClient);

	CStatic::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CScrollBitmapWnd::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	refLayOut();
	Invalidate();
}

int CScrollBitmapWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	VERIFY(m_HScroll.Create(   
              SBS_HORZ | SBS_TOPALIGN | WS_CHILD, CRect(5, 5, 100, 30), this, 100));
	VERIFY(m_VScroll.Create(   
              SBS_VERT | SBS_TOPALIGN | WS_CHILD, CRect(5, 5, 100, 30), this, 101));
	m_VScroll.ShowWindow(false);
	m_HScroll.ShowWindow(false);

	return 0;
}

void CScrollBitmapWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_clickPoint = point;
	if (m_bVScrollShow)
	{
		m_clickPoint.y += m_offsety;
	}
	if (m_bHScrollShow)
	{
		m_clickPoint.x += m_offsetx;
	}

	m_clickPoint.x = m_clickPoint.x<m_PicSize.cx ? m_clickPoint.x:m_PicSize.cx;
	m_clickPoint.y = m_clickPoint.y<m_PicSize.cy ? m_clickPoint.y:m_PicSize.cy;
	CStatic::OnLButtonDown(nFlags, point);
}

CPoint CScrollBitmapWnd::getClickPoint()
{
	CPoint point; 
	point = m_clickPoint;
	m_clickPoint = CPoint(0,0);
	return point; 
}

bool CScrollBitmapWnd::refBitmap(HBITMAP bitMap)
{
	m_dcMem.DeleteDC();
	m_BackBitmap.DeleteObject();
    
	if(!m_BackBitmap.Attach(bitMap))
	{
		m_BackBitmap.DeleteObject();
		return false;
	}

	m_BackBitmap.GetBitmap(&m_bitMap);
	
	if(!m_dcMem.CreateCompatibleDC(NULL))
	{
		m_dcMem.DeleteDC();
		m_BackBitmap.DeleteObject();
		return false;
	}
	m_dcMem.SelectObject(&m_BackBitmap);
	return true;
}
