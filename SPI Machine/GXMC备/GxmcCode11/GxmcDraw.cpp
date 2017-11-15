#include "StdAfx.h"
#include "GxmcDraw.h"

namespace Gxmc_Draw
{
	/******************************************* CGxmcDraw ********************************************/
	CGxmcDraw::CGxmcDraw(void)
	{
		m_penWidth = 1; m_penStyle = PS_SOLID;  m_penColor = WHITE_PEN;
		m_brushColor = WHITE_BRUSH; m_nLayer = 1;  m_bIsFilled = true; m_bIsHidden = false;m_bIsHidden = false;
		m_marginRect = RectD(MAXNUM,MINNUM,MINNUM,MAXNUM);
	}

	CGxmcDraw::~CGxmcDraw(void)
	{

	}
	void CGxmcDraw::UpdateMargin(RectD rect,RectD& realMarginRect)
	{
		realMarginRect.left = rect.left < realMarginRect.left ? rect.left : realMarginRect.left;
		realMarginRect.right = rect.right > realMarginRect.right ? rect.right : realMarginRect.right;
		realMarginRect.top = rect.top > realMarginRect.top ? rect.top : realMarginRect.top;
		realMarginRect.bottom = rect.bottom < realMarginRect.bottom ? rect.bottom : realMarginRect.bottom;
	}
	void CGxmcDraw::UpdateMargin(PointD point,RectD& realMarginRect)
	{
		realMarginRect.left = point.x < realMarginRect.left ? point.x : realMarginRect.left;
		realMarginRect.right = point.x > realMarginRect.right ? point.x : realMarginRect.right;
		realMarginRect.top = point.y > realMarginRect.top ? point.y : realMarginRect.top;
		realMarginRect.bottom = point.y < realMarginRect.bottom ? point.y : realMarginRect.bottom;
	}
	void CGxmcDraw::WorldViewToLogicView(double wX,double wY,int& lX,int& lY)
	{
		double realHeight = (m_Screen.realMarginRect.top - m_Screen.realMarginRect.bottom)*m_Screen.factor;

		lX = (wX - m_Screen.startX + m_Screen.offsetX)*m_Screen.factor;
		lY =  realHeight - (wY - m_Screen.startY + m_Screen.offsetY)*m_Screen.factor;
	}
	void CGxmcDraw::LogicViewToWorldView(int lX,int lY,double& wX,double& wY)
	{
		double realHeight = (m_Screen.realMarginRect.top - m_Screen.realMarginRect.bottom)*m_Screen.factor;
		wX = (double)lX/m_Screen.factor + m_Screen.startX + m_Screen.offsetX;
		wY = (double)(realHeight - lY)/m_Screen.factor + m_Screen.startY + m_Screen.offsetY;
	}
	/******************************************* CGxmcLine ********************************************/
	IMPLEMENT_SERIAL(CGxmcLine,CObject,1)
	CGxmcLine::CGxmcLine()
	{
		m_penWidth = 0;   m_penStyle = 0;  m_penColor = 0;
		m_brushColor = 0; m_nLayer = 0;
	}
	CGxmcLine::~CGxmcLine()
	{

	}

	void CGxmcLine::Serialize(CArchive& ar)
	{

	}

	void CGxmcLine::GetBoundaryRect(RectD& rect)
	{

	}

	void CGxmcLine::Draw(CDC& dc,const ScreenStruct& screen)
	{
// 		RectD boundaryRect;
// 		GetBoundaryRect(boundaryRect);
// 		if (!boundaryRect.IsRectInRect(screen.drawBoundaryRect))
// 		{
// 			//return;
// 		}
		//不填充 NA

		if (m_bIsHidden)
		{
			return;
		}
		if (!m_bIsFilled)
		{
// 			CPen pen(m_penStyle,m_penWidth,m_penColor);
// 			dc.SelectObject(&pen);

			m_Screen = screen;
			int startX,startY;
			int stopX,stopY;
			WorldViewToLogicView(m_startPoint.x,m_startPoint.y,startX,startY);
			WorldViewToLogicView(m_stopPoint.x,m_stopPoint.y,stopX,stopY);
 			dc.MoveTo(startX,startY);
 			dc.LineTo(stopX,stopY);
		}	
	}
	void CGxmcLine::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}

	/******************************************* CGxmcRect ********************************************/
	IMPLEMENT_SERIAL(CGxmcRect,CObject,1)
		CGxmcRect::CGxmcRect()
	{
		m_penWidth = 0;   m_penStyle = 0;  m_penColor = 0;
		m_brushColor = 0; m_nLayer = 0;
	}
	CGxmcRect::~CGxmcRect()
	{
	}

	void CGxmcRect::Serialize(CArchive& ar)
	{
	}

	void CGxmcRect::GetBoundaryRect(RectD& rect)
	{
		rect = RectD(m_startPoint.x,m_startPoint.y,m_stopPoint.x,m_stopPoint.y);
	}
	void CGxmcRect::Draw(CDC& dc,const ScreenStruct& screen)
	{
// 		RectD boundaryRect;
// 		GetBoundaryRect(boundaryRect);
// 		if (!boundaryRect.IsRectInRect(screen.drawBoundaryRect))
// 		{
// 			//return;
// 		}
// 		CPen pen(m_penStyle,m_penWidth,m_penColor);
// 		dc.SelectObject(&pen);

		if (m_bIsHidden)
		{
			return;
		}
		
		if (m_bIsFilled)
		{
			CBrush brush(m_brushColor);
			dc.SelectStockObject(NULL_PEN);
			dc.SelectObject(&brush);
		}
		else
		{
// 			CPen pen(m_penStyle,m_penWidth,m_penColor);
// 			dc.SelectObject(&pen);
			dc.SelectStockObject(NULL_BRUSH);
		}
		
		m_Screen = screen;
		int startX,startY;
		int stopX,stopY;
		WorldViewToLogicView(m_startPoint.x,m_startPoint.y,startX,startY);
		WorldViewToLogicView(m_stopPoint.x,m_stopPoint.y,stopX,stopY);
		dc.Rectangle(startX,startY,stopX,stopY);
		
	}
	void CGxmcRect::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}
	/******************************************* CGxmcPolygon ********************************************/
	IMPLEMENT_SERIAL(CGxmcPolygon,CObject,1)
	CGxmcPolygon::CGxmcPolygon()
	{	
	}
	CGxmcPolygon::~CGxmcPolygon()
	{	
	}

	void CGxmcPolygon::Serialize(CArchive& ar)
	{
	}

	void CGxmcPolygon::GetBoundaryRect(RectD& rect)
	{
		rect.left = MAXNUM;
		rect.top = MINNUM;
		rect.right = MINNUM;
		rect.bottom = MAXNUM;
		for (unsigned int i = 0; i < m_vecPoint.size(); i++)
		{
			rect.left = m_vecPoint[i].x < rect.left ? m_vecPoint[i].x : rect.left;
			rect.right = m_vecPoint[i].x > rect.right ? m_vecPoint[i].x : rect.right;
			rect.top = m_vecPoint[i].y > rect.top ? m_vecPoint[i].y : rect.top;
			rect.bottom = m_vecPoint[i].y < rect.bottom ? m_vecPoint[i].y : rect.bottom;
		}
	}

	void CGxmcPolygon::Draw(CDC& dc,const ScreenStruct& screen)
	{
// 		CPen pen(m_penStyle,m_penWidth,m_penColor);
// 		dc.SelectObject(&pen);
// 		CBrush brush(m_brushColor);
// 		dc.SelectObject(&brush);
		if (m_bIsHidden)
		{
			return;
		}
		m_Screen = screen;
		if (m_bIsFilled)
		{
			std::vector<CPoint> vecPoint;
			int pointX,pointY;
			for (unsigned int i = 0; i < m_vecPoint.size(); i++)
			{
				WorldViewToLogicView(m_vecPoint[i].x,m_vecPoint[i].y,pointX,pointY);
				vecPoint.push_back(CPoint(pointX,pointY));
			}
			if (vecPoint.size()>0)
			{
				dc.Polygon(&vecPoint[0],vecPoint.size());
			}
			
		}
		else
		{
			int pointX,pointY;
			//CPoint Point;
			for (unsigned int i = 0; i < m_vecPoint.size(); i++)
			{
				// 			if (!m_vecPoint[i].IsPointInRect(screen.drawBoundaryRect))
				// 			{
				// 				continue;
				// 			}
				WorldViewToLogicView(m_vecPoint[i].x,m_vecPoint[i].y,pointX,pointY);
				if (m_vecPoint[i].isMoveto)
				{
					dc.MoveTo(pointX,pointY);
				}
				else
				{
					dc.LineTo(pointX,pointY);
				}
			}
		}	
	}
	void CGxmcPolygon::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}
	/******************************************* CGxmcCircle ********************************************/
	IMPLEMENT_SERIAL(CGxmcCircle,CObject,1)
	CGxmcCircle::CGxmcCircle()
	{

	}
	CGxmcCircle::~CGxmcCircle()
	{

	}
	void CGxmcCircle::Serialize(CArchive& ar)
	{
		
	}

	void CGxmcCircle::GetBoundaryRect(RectD& rect)
	{
		rect = RectD(m_CenterPoint.x - m_Radius,m_CenterPoint.y + m_Radius,
			m_CenterPoint.x + m_Radius,m_CenterPoint.y - m_Radius);
	}

	void CGxmcCircle::Draw(CDC& dc,const ScreenStruct& screen)
	{
// 		RectD boundaryRect;
// 		GetBoundaryRect(boundaryRect);
// 		if (!boundaryRect.IsRectInRect(screen.drawBoundaryRect))
// 		{
// 			//return;
// 		}
		CPen pen(m_penStyle,m_penWidth,m_penColor);
		CPen* pOldPen = dc.SelectObject(&pen);
		CBrush brush(m_brushColor);
		CBrush* pOldBrush = dc.SelectObject(&brush);
		if (m_bIsHidden)
		{
			return;
		}
		if (m_bIsFilled)
		{
			dc.SelectObject(&brush);
		}
		else
		{
			dc.SelectStockObject(NULL_BRUSH);
		}

		m_Screen = screen;
		int centerX,centerY;
		int radius;

		WorldViewToLogicView(m_CenterPoint.x,m_CenterPoint.y,centerX,centerY);
		radius = m_Radius*m_Screen.factor;
		CRect rect(centerX - radius,centerY - radius,centerX + radius,centerY + radius);
		dc.Ellipse(&rect);
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
	}
	void CGxmcCircle::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}
	/******************************************* CGxmcArc ********************************************/
	IMPLEMENT_SERIAL(CGxmcArc,CObject,1)
	CGxmcArc::CGxmcArc()
	{

	}
	CGxmcArc::~CGxmcArc()
	{

	}
	void CGxmcArc::Serialize(CArchive& ar)
	{

	}

	void CGxmcArc::GetBoundaryRect(RectD& rect)
	{
		//圆弧与X轴正向交点
		//PointD PointMaxX = PointD(m_CenterPoint.x + m_Radius,m_CenterPoint.y);
		////圆弧与X轴负向交点
		//PointD PointMinX = PointD(m_CenterPoint.x - m_Radius,m_CenterPoint.y);
		////圆弧与Y轴正向交点
		//PointD PointMaxY = PointD(m_CenterPoint.x,m_CenterPoint.y + m_Radius);
		////圆弧与Y轴负向交点
		//PointD PointMinY = PointD(m_CenterPoint.x,m_CenterPoint.y - m_Radius);
		////扇形区域在第一象限
		//if (m_startPoint.x > m_CenterPoint.x && m_startPoint.x < PointMaxX.x && 
		//	m_startPoint.y > m_CenterPoint.y && m_startPoint.y < PointMaxY.y &&
		//	m_stopPoint.x > m_CenterPoint.x && m_stopPoint.x < PointMaxX.x && 
		//	m_stopPoint.y > m_CenterPoint.y && m_stopPoint.y < PointMaxY.y &&)
		//{
		//}

		rect = RectD(m_CenterPoint.x - m_Radius,m_CenterPoint.y + m_Radius,
			m_CenterPoint.x + m_Radius,m_CenterPoint.y - m_Radius);

	}

	void CGxmcArc::Draw(CDC& dc,const ScreenStruct& screen)
	{
// 		RectD boundaryRect;
// 		GetBoundaryRect(boundaryRect);
// 		if (!boundaryRect.IsRectInRect(screen.drawBoundaryRect))
// 		{
// 			//return;
// 		}
// 		CPen pen(m_penStyle,m_penWidth,m_penColor);
// 		dc.SelectObject(&pen);
//		CBrush brush(m_brushColor);
		if (m_bIsHidden)
		{
			return;
		}
		m_Screen = screen;
		int startX,startY;
		int stopX,stopY;
		int centerX,centerY;
		int radius;
		WorldViewToLogicView(m_startPoint.x,m_startPoint.y,startX,startY);
		WorldViewToLogicView(m_stopPoint.x,m_stopPoint.y,stopX,stopY);
		WorldViewToLogicView(m_CenterPoint.x,m_CenterPoint.y,centerX,centerY);
		radius = m_Radius*m_Screen.factor;
		//圆弧起点终点相同、或圆心在弦线段外，cj 2017-8-15
		if (startX == stopX && startY == stopY)
		{
			return;
		}
		CRect rect(centerX - radius,centerY - radius,centerX + radius,centerY + radius);
		if (m_bIsFilled)
		{	
			if (m_nArcDirection == AD_COUNTERCLOCKWISE)
			{	
				CPen Pen(m_penStyle,m_penWidth,m_penColor);
				CPen* oldPen = dc.SelectObject(&Pen);
				CBrush Brush(m_brushColor); 
				CBrush* oldBrush = dc.SelectObject(&Brush);
				dc.SetArcDirection(AD_COUNTERCLOCKWISE);
				dc.Pie(&rect,CPoint(startX,startY),CPoint(stopX,stopY));
 				dc.SelectObject(oldBrush);
 				dc.SelectObject(oldPen);	
			}
			else
			{
				CPen Pen(m_penStyle,m_penWidth,dc.GetBkColor());
				CPen* oldPen = dc.SelectObject(&Pen);
				CBrush Brush(dc.GetBkColor()); 
				CBrush* oldBrush = dc.SelectObject(&Brush);
				dc.SetArcDirection(AD_CLOCKWISE);
				dc.Pie(&rect,CPoint(startX,startY),CPoint(stopX,stopY));
				dc.SelectObject(oldBrush);
				dc.SelectObject(oldPen);
			}
		} 
		else
		{
			if (m_nArcDirection == AD_COUNTERCLOCKWISE)
			{
				dc.SetArcDirection(AD_COUNTERCLOCKWISE);
			}
			else
			{
				dc.SetArcDirection(AD_CLOCKWISE);
			}
			dc.Arc(&rect,CPoint(startX,startY),CPoint(stopX,stopY));
		}
	}
	void CGxmcArc::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}
	/******************************************* CGxmcArcPolygon ********************************************/
	IMPLEMENT_SERIAL(CGxmcArcPolygon,CObject,1)
	CGxmcArcPolygon::CGxmcArcPolygon()
	{

	}
	CGxmcArcPolygon::~CGxmcArcPolygon()
	{

	}
	void CGxmcArcPolygon::Serialize(CArchive& ar)
	{

	}

	void CGxmcArcPolygon::GetBoundaryRect(RectD& rect)
	{
		RectD rt;
		for (unsigned int i = 0; i < m_vecArc.size(); i++)
		{
			this->m_vecArc[i].GetBoundaryRect(rt);
			UpdateMargin(rt,rect);
		}
 		for (unsigned int i = 0; i < m_vecLine.size(); i++)
 		{
 			this->m_vecLine[i].GetBoundaryRect(rt);
 			UpdateMargin(rt,rect);
 		}
		for (unsigned int i = 0; i < m_vecPoint.size(); i++)
		{
			UpdateMargin(m_vecPoint[i],rect);
		}
		
	}
	void CGxmcArcPolygon::Draw(CDC& dc,const ScreenStruct& screen)
	{
		if (m_bIsHidden)
		{
			return;
		}
 		std::vector<CPoint> vecPoint;
 		int pointX,pointY;
		CPen pen(m_penStyle,m_penWidth,m_penColor);
		dc.SelectObject(&pen);
		CBrush brush(m_brushColor);
		dc.SelectObject(&brush);
 		m_Screen = screen;
		if (m_bIsFilled)
		{
			CGxmcPolygon polygon(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,m_bIsFilled,m_vecPoint);
			polygon.Draw(dc,screen);	
		}
		else
		{
			for (unsigned int i = 0; i < m_vecLine.size(); i++)
			{
				this->m_vecLine[i].Draw(dc,screen);
			}
		}
		
		for (unsigned int i = 0; i < m_vecArc.size(); i++)
		{
			this->m_vecArc[i].Draw(dc,screen);
		}
			
	}
	void CGxmcArcPolygon::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
		for (unsigned int i = 0; i < m_vecArc.size(); i++)
		{
			this->m_vecArc[i].SetColour(PenColor,BrushColor);
		}
		for (unsigned int i = 0; i < m_vecLine.size(); i++)
		{
			this->m_vecLine[i].SetColour(PenColor,BrushColor);
		}
	}
}

