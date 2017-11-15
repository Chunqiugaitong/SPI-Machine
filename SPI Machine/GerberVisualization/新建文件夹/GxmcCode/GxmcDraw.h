
#ifndef GXMC_DRAW_H
#define GXMC_DRAW_H

#pragma once
#include "afx.h"
#include <vector>

#define MAXNUM 9999999.99
#define MINNUM -9999999.99
#define INITI_RECT RectD(MAXNUM,MINNUM,MINNUM,MAXNUM)

namespace Gxmc_Draw
{
	//自定义双精度型型结构
	struct RectD
	{
	public:
		double left;
		double top;
		double right;
		double bottom;
		RectD()
		{
			left = right = top = bottom = 0.000;
		}
		RectD(double Left,double Top,double Right,double Bottom)
		{
			left   = Left;
			right  = Right;
			top    = Top;
			bottom = Bottom;
		}

		void operator= (const RectD& rect)
		{
			left   = rect.left;
			right  = rect.right;
			top    = rect.top;
			bottom = rect.bottom;
		}
		bool operator== (const RectD& rect)
		{
			return (left == rect.left && right  == rect.right&&
				    top == rect.top && bottom == rect.bottom);
		}
		bool IsRectInRect(const RectD& rect)	
		{
			if(right > rect.left && right <= rect.right  &&
			  left >= rect.left  && left  < rect.right   &&
			  top  > rect.bottom && top <= rect.top      &&
			  bottom >= rect.bottom && bottom < rect.top )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		double width()
		{
			return (right - left);
		}
		double height()
		{
			return (top - bottom);
		}
		
	};
	struct PointD
	{
	public:
		double x;
		double y;
		bool isMoveto;
		PointD()
		{
			x=y=0;
			isMoveto = false;
		}
		PointD(double dx,double dy)
		{
			x = dx;
			y = dy;
			isMoveto = false;
		}
// 		PointD(const PointD& point)
// 		{
// 			x = point.x;
// 			y = point.y;
// 			isMoveto = false;
//		}
		void operator= (const PointD& point)
		{
			x = point.x;
			y = point.y;
			isMoveto = point.isMoveto;
		}
		bool IsPointInRect(const RectD& rect)
		{
			if (x >= rect.left && x<= rect.right && y >= rect.bottom && y <= rect.top)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};
	struct ScreenStruct
	{
		double factor;
		double startX;
		double startY;
		double offsetX;
		double offsetY;

		RectD realMarginRect;
		RectD drawBoundaryRect;//与客户区边界对应的实际坐标
	};

	class CGxmcDraw :public CObject
	{
	public:
		CGxmcDraw();
 		CGxmcDraw(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled)
 		{
 			m_penWidth = PenWidth; m_penStyle = PenStyle;  m_penColor = PenColor;
			m_brushColor = BrushColor; m_nLayer = nLayer;  m_bIsFilled = bIsFilled; m_bIsHidden = false;m_marginRect = RectD(MAXNUM,MINNUM,MINNUM,MAXNUM);
 		}
		CGxmcDraw(const CGxmcDraw& gxmcDraw)
		{
			m_penWidth = gxmcDraw.m_penWidth; m_penStyle = gxmcDraw.m_penStyle;  m_penColor = gxmcDraw.m_penColor;
			m_brushColor = gxmcDraw.m_brushColor; m_nLayer = gxmcDraw.m_nLayer;  m_bIsFilled = gxmcDraw.m_bIsFilled; m_bIsHidden = gxmcDraw.m_bIsHidden;
		}
		void operator= (const CGxmcDraw& gxmcDraw)
		{
			m_penWidth = gxmcDraw.m_penWidth; m_penStyle = gxmcDraw.m_penStyle;  m_penColor = gxmcDraw.m_penColor;
			m_brushColor = gxmcDraw.m_brushColor; m_nLayer = gxmcDraw.m_nLayer;  m_bIsFilled = gxmcDraw.m_bIsFilled; m_bIsHidden = gxmcDraw.m_bIsHidden;
		}
		virtual ~CGxmcDraw(void);//继承自CObject析构函数本来就是虚函数

		//void SetScreenStruct(float factor,double startX,double startY);
		void UpdateMargin(RectD rect,RectD& realMarginRect);
		void UpdateMargin(PointD rect,RectD& realMarginRect);
		virtual void GetBoundaryRect(RectD& rect) = 0;
		virtual void Draw(CDC& dc,const ScreenStruct& screen)=0;
		virtual void SetColour(COLORREF PenColor,COLORREF BrushColor) = 0;
		virtual std::string GetShapeStyle() = 0;
		
    public:
		int m_penWidth;
		int m_penStyle;
		COLORREF m_penColor;
		COLORREF m_brushColor;
		int m_nLayer;//图层索引
		//封闭图形是否填充
		bool m_bIsFilled;
		//是否隐藏，遍历时不对该图形作相应处理
		bool m_bIsHidden;
		//封闭区域的边界
		RectD m_marginRect;

		ScreenStruct m_Screen;

		void WorldViewToLogicView(double wX,double wY,int& lX,int& lY);
		void LogicViewToWorldView(int lX,int lY,double& wX,double& wY);

	};
	class CGxmcLine: public CGxmcDraw
	{
	public:
		CGxmcLine();
		//派生类调用基类有参构造函数
		CGxmcLine(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,PointD startPoint,PointD stopPoint)
			:CGxmcDraw(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
			m_startPoint = startPoint;
			m_stopPoint  = stopPoint;
		}

		void operator= (const CGxmcLine& gxmcLine)
		{
			m_penWidth = gxmcLine.m_penWidth; m_penStyle = gxmcLine.m_penStyle;  m_penColor = gxmcLine.m_penColor;
			m_brushColor = gxmcLine.m_brushColor; m_nLayer = gxmcLine.m_nLayer;  m_bIsFilled = gxmcLine.m_bIsFilled;

			m_startPoint = gxmcLine.m_startPoint;
			m_stopPoint  = gxmcLine.m_stopPoint;
		}
		CGxmcLine(const CGxmcLine& gxmcLine)
		{
			m_penWidth = gxmcLine.m_penWidth; m_penStyle = gxmcLine.m_penStyle;  m_penColor = gxmcLine.m_penColor;
			m_brushColor = gxmcLine.m_brushColor; m_nLayer = gxmcLine.m_nLayer;  m_bIsFilled = gxmcLine.m_bIsFilled;

			m_startPoint = gxmcLine.m_startPoint;
			m_stopPoint  = gxmcLine.m_stopPoint;
		}

		~CGxmcLine();

		void Serialize(CArchive& ar);
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "CGxmcLine";}
	protected:
		//继承自CObject的串行化声明
		DECLARE_SERIAL(CGxmcLine);
		PointD m_startPoint;
		PointD m_stopPoint;
	private:
	};
	
	class CGxmcRect: public CGxmcDraw
	{
	public:
		CGxmcRect();
		//派生类调用基类有参构造函数
		CGxmcRect(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,PointD startPoint,PointD stopPoint)
			:CGxmcDraw(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
			m_startPoint = startPoint;
			m_stopPoint  = stopPoint;
		}

		void operator= (const CGxmcRect& gxmcRect)
		{
			m_penWidth = gxmcRect.m_penWidth; m_penStyle = gxmcRect.m_penStyle;  m_penColor = gxmcRect.m_penColor;
			m_brushColor = gxmcRect.m_brushColor; m_nLayer = gxmcRect.m_nLayer;  m_bIsFilled = gxmcRect.m_bIsFilled;

			m_startPoint = gxmcRect.m_startPoint;
			m_stopPoint  = gxmcRect.m_stopPoint;
		}
		CGxmcRect(const CGxmcRect& gxmcRect)
		{
			m_penWidth = gxmcRect.m_penWidth; m_penStyle = gxmcRect.m_penStyle;  m_penColor = gxmcRect.m_penColor;
			m_brushColor = gxmcRect.m_brushColor; m_nLayer = gxmcRect.m_nLayer;  m_bIsFilled = gxmcRect.m_bIsFilled;

			m_startPoint = gxmcRect.m_startPoint;
			m_stopPoint  = gxmcRect.m_stopPoint;
		}

		~CGxmcRect();

		void Serialize(CArchive& ar);
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "CGxmcRect";}
	protected:
		//继承自CObject的串行化声明
		DECLARE_SERIAL(CGxmcRect);
		PointD m_startPoint;//左下
		PointD m_stopPoint;//右上
	private:
	};

	//直线多边形
	class CGxmcPolygon: public CGxmcDraw
	{
	public:
		CGxmcPolygon();
		CGxmcPolygon(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,const std::vector<PointD>& vecPoint)
			:CGxmcDraw(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
			for (unsigned int i = 0;i < vecPoint.size(); i++)
			{
				m_vecPoint.push_back(vecPoint[i]);
			}
		}
		~CGxmcPolygon();

		void operator= (const CGxmcPolygon& gxmcPolygon)
		{
			for (unsigned int i = 0;i < gxmcPolygon.m_vecPoint.size(); i++)
			{
				m_vecPoint.push_back(gxmcPolygon.m_vecPoint[i]);
			}
		}
		CGxmcPolygon(const CGxmcPolygon& gxmcPolygon)
		{
			for (unsigned int i = 0;i < gxmcPolygon.m_vecPoint.size(); i++)
			{
				m_vecPoint.push_back(gxmcPolygon.m_vecPoint[i]);
			}
		}

		void Serialize(CArchive& ar);
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "CGxmcPolygon";}
	protected:
		//继承自CObject的串行化声明
		DECLARE_SERIAL(CGxmcPolygon);
		std::vector<PointD> m_vecPoint; 
	
	private:
	};
	class CGxmcCircle:public CGxmcDraw
	{
	public:
		CGxmcCircle();
		CGxmcCircle(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,PointD Center,double Radius)
			:CGxmcDraw(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
			m_CenterPoint = Center;
			m_Radius      = Radius;
		}
		~CGxmcCircle();

		void operator= (const CGxmcCircle& gxmcCircle)
		{
			m_penWidth = gxmcCircle.m_penWidth; m_penStyle = gxmcCircle.m_penStyle;  m_penColor = gxmcCircle.m_penColor;
			m_brushColor = gxmcCircle.m_brushColor; m_nLayer = gxmcCircle.m_nLayer;  m_bIsFilled = gxmcCircle.m_bIsFilled;
			m_CenterPoint = gxmcCircle.m_CenterPoint;
			m_Radius      = gxmcCircle.m_Radius;
		}
		CGxmcCircle(const CGxmcCircle& gxmcCircle)
		{
			m_penWidth = gxmcCircle.m_penWidth; m_penStyle = gxmcCircle.m_penStyle;  m_penColor = gxmcCircle.m_penColor;
			m_brushColor = gxmcCircle.m_brushColor; m_nLayer = gxmcCircle.m_nLayer;  m_bIsFilled = gxmcCircle.m_bIsFilled;
			m_CenterPoint = gxmcCircle.m_CenterPoint;
			m_Radius      = gxmcCircle.m_Radius;
		}

		void Serialize(CArchive& ar);
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "CGxmcCircle";}
	protected:
		//继承自CObject的串行化声明
		DECLARE_SERIAL(CGxmcCircle);
	public:
		PointD m_CenterPoint;
		double m_Radius;
	private:
	};
	class CGxmcArc: public CGxmcCircle
	{
	public:
		CGxmcArc();
		CGxmcArc(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,PointD Center,double Radius,PointD StartPoint,PointD StopPoint,int nArcDirection)
			:CGxmcCircle(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled,Center,Radius)
		{
			m_startPoint = StartPoint;
			m_stopPoint  = StopPoint;
			m_nArcDirection = nArcDirection;
		}
		~CGxmcArc();

		void operator= (const CGxmcArc& gxmcArc)
		{
			m_penWidth    = gxmcArc.m_penWidth;     m_penStyle = gxmcArc.m_penStyle;  m_penColor = gxmcArc.m_penColor;
			m_brushColor  = gxmcArc.m_brushColor;   m_nLayer   = gxmcArc.m_nLayer;    m_bIsFilled = gxmcArc.m_bIsFilled;
			m_CenterPoint = gxmcArc.m_CenterPoint;
			m_Radius      = gxmcArc.m_Radius;
			m_startPoint  = gxmcArc.m_startPoint;
			m_stopPoint   = gxmcArc.m_stopPoint;
			m_nArcDirection = gxmcArc.m_nArcDirection;

		}
		CGxmcArc(const CGxmcArc& gxmcArc)
		{
			m_penWidth    = gxmcArc.m_penWidth;     m_penStyle = gxmcArc.m_penStyle;  m_penColor = gxmcArc.m_penColor;
			m_brushColor  = gxmcArc.m_brushColor;   m_nLayer   = gxmcArc.m_nLayer;    m_bIsFilled = gxmcArc.m_bIsFilled;
			m_CenterPoint = gxmcArc.m_CenterPoint;
			m_Radius      = gxmcArc.m_Radius;
			m_startPoint  = gxmcArc.m_startPoint;
			m_stopPoint   = gxmcArc.m_stopPoint;
			m_nArcDirection = gxmcArc.m_nArcDirection;
		}

		void Serialize(CArchive& ar);
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "CGxmcArc";}
	protected:
		//继承自CObject的串行化声明
		DECLARE_SERIAL(CGxmcArc);
		PointD m_startPoint;
		PointD m_stopPoint;
		int m_nArcDirection;
	private:
	};
	//带圆弧多边形
	class CGxmcArcPolygon: public CGxmcDraw
	{
	public:
		CGxmcArcPolygon();
		CGxmcArcPolygon(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled)
			:CGxmcDraw(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
		
		}
		~CGxmcArcPolygon();

		void operator =(const CGxmcArcPolygon& gxmcPolygon)
		{
			m_penWidth    = gxmcPolygon.m_penWidth;     m_penStyle = gxmcPolygon.m_penStyle;  m_penColor = gxmcPolygon.m_penColor;
			m_brushColor  = gxmcPolygon.m_brushColor;   m_nLayer   = gxmcPolygon.m_nLayer;    m_bIsFilled = gxmcPolygon.m_bIsFilled;

			for (unsigned int i = 0; i < gxmcPolygon.m_vecLine.size(); i++)
			{
				this->m_vecLine.push_back(gxmcPolygon.m_vecLine[i]);
			}
			for (unsigned int i = 0; i < gxmcPolygon.m_vecArc.size(); i++)
			{
				this->m_vecArc.push_back(gxmcPolygon.m_vecArc[i]);
			}
		}
		CGxmcArcPolygon(const CGxmcArcPolygon& gxmcPolygon)
		{
			m_penWidth    = gxmcPolygon.m_penWidth;     m_penStyle = gxmcPolygon.m_penStyle;  m_penColor = gxmcPolygon.m_penColor;
			m_brushColor  = gxmcPolygon.m_brushColor;   m_nLayer   = gxmcPolygon.m_nLayer;    m_bIsFilled = gxmcPolygon.m_bIsFilled;

			for (unsigned int i = 0; i < gxmcPolygon.m_vecLine.size(); i++)
			{
				this->m_vecLine.push_back(gxmcPolygon.m_vecLine[i]);
			}
			for (unsigned int i = 0; i < gxmcPolygon.m_vecArc.size(); i++)
			{
				this->m_vecArc.push_back(gxmcPolygon.m_vecArc[i]);
			}
		}

		void Serialize(CArchive& ar);
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "CGxmcArcPolygon";}
	protected:
		//继承自CObject的串行化声明
		DECLARE_SERIAL(CGxmcArcPolygon);
		std::vector<CGxmcLine> m_vecLine;
		std::vector<CGxmcArc>  m_vecArc;
		std::vector<PointD>    m_vecPoint;
		//RectD                m_regionMargin;
	private:
	};
}

#endif

