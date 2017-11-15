
//头文件GxmcGerber.h

#ifndef GXMC_GERBER_H
#define GXMC_GERBER_H

#pragma once
#include <string>
#include <regex>
#include <vector>
#include <list>
#include <map>
#include <math.h>
//#include <gl/glut.h>  
#include "GxmcDraw.h"
#include "GxmcDatabase.h"

#define MAXNUM 9999999.99
#define MINNUM -9999999.99
const int MaxNum = 99999;
const int MinNum = -99999;

#define BUFFERLENGTH 256
#define INITI_RECT RectD(MAXNUM,MINNUM,MINNUM,MAXNUM)
#define MAX_RECT   RectD(MINNUM,MAXNUM,MAXNUM,MINNUM)

using namespace Gxmc_Draw;
using namespace std;

typedef CGxmcDraw GxmcShape;
typedef CGxmcArcPolygon GxmcPolygon;
typedef CGxmcLine GxmcLine;
typedef CGxmcPolygon GxmcPLine;
typedef CGxmcCircle GxmcCircle;
typedef CGxmcArc GxmcArc;
typedef CGxmcRect GxmcRect;

class CGxmcDatabase;

namespace Gxmc_Gerber
{
	#pragma region 各种光圈形状结构体  AM  

	struct AM_Circle 
	{
		int    exposure;
		double diameter;
		PointD* centerPoint;
		double  rotationAngle;
	};

	struct AM_VectorLine 
	{
		int    exposure;
		double lineWidth;
		PointD* startPoint;
		PointD* stopPoint;
		double  rotationAngle;
	};

	struct AM_CenterLine 
	{
		int    exposure;
		double rectangleWidth;
		double rectangleHeight;
		PointD* centerPoint;
		double  rotationAngle;
	};

	struct AM_OutLine 
	{
		int    exposure;
		int    numVertex;
		PointD* pointList;
		double  rotationAngle;

	};

	struct AM_Polygon 
	{
		int    exposure;
		int    numVertex;
		PointD* centerPoint;
		double diameter;
		double  rotationAngle;
	};

	struct AM_Moire
	{
		PointD* centerPoint;
		double outerDiameter;
		double ringThickness;
		double ringGap;
		int    ringMaxNum;
        double crosshairThickness;
		double crosshairLength;
		double  rotationAngle;
	};

	struct AM_Thermal
	{
		PointD* centerPoint;
		double outerDiameter;
		double innerDiameter;
		double gapThickness;
		double  rotationAngle;
	};

	enum eAmName
	{
		CircleAM,
		VectorLine,
		CenterLine ,
		OutLine,
		Polygon, 
		Moire,
		Thermal
	};
	 class AD_AmInfo
	{
	public:
		AD_AmInfo()
		{

		}
		~AD_AmInfo()
		{

		}
		std::vector<AM_Circle> vecAmCircle;
		std::vector<AM_VectorLine> vecAmVectorLine;
		std::vector<AM_CenterLine> vecAmCenterLine;
		std::vector<AM_OutLine> vecAmOutLine;
		std::vector<AM_Polygon> vecAmPolygon;
		std::vector<AM_Moire> vecAmMoire;
		std::vector<AM_Thermal> vecAmThermal;
		void Initialize()
		{
		}
	};
	typedef struct  tagAmStyle
	{
		eAmName amName;
		AD_AmInfo amInfo; 
	}AmStyle;

	
	#pragma endregion

    #pragma region 各种光圈形状结构体  AD
	struct  AD_Circle
	{
		double Diameter;
		double HoleDiameter;
		void Initialize()
		{
			Diameter = HoleDiameter = 0.0000;
		}
	};
	struct  AD_Rectangle
	{
		double Width;
		double Height;
		double HoleDiameter;
		void Initialize()
		{
			Width = Height = HoleDiameter = 0.0000;
		}
	};
	struct  AD_Obround
	{
		double Width;
		double Height;
		double HoleDiameter;
		void Initialize()
		{
			Width = Height = HoleDiameter = 0.0000;
		}
	};
	struct  AD_RegularPolygon
	{
		double OuterDiameter;
		double numVertice;
		double RotateAngle;
		double HoleDiameter;
		void Initialize()
		{
			OuterDiameter = numVertice = RotateAngle = HoleDiameter = 0.0000;
		}
	};
	enum eAdName
	{
		NomName,
		Circle,
		Rectangle,
		Obround,
		RegularPolygon,
		Am
	};
	typedef union tagAdInfo
	{
		AD_Circle adCircle;
		AD_Rectangle	adRectangle;
		AD_Obround adObround;
		AD_RegularPolygon adRegularPolygon;
		AD_AmInfo* adAmInfo;
	}AdInfo;
	typedef struct  tagAdStyle
	{
		eAdName adName;
		AdInfo adInfo; 
		void Initialize()
		{
			adInfo.adCircle.Initialize();
			adInfo.adRectangle.Initialize();
			adInfo.adObround.Initialize();
			adInfo.adRectangle.Initialize();
			adInfo.adRegularPolygon.Initialize();
			adInfo.adAmInfo->Initialize();
		}
	}AdStyle;

    #pragma endregion 

    #pragma region Gerbera信息

	enum eDirection
	{
		Clockwise,
		CounterClockwise
	};
	enum eArcStyle
	{
		MajorArc,
		MinorArc
	};
	enum eUnit
	{
		Millimetre,
		Inch
	};
	enum eZeroOmit
	{
		LeadingOmit,
		TailOmit,
		NotOmit
	};
	enum eApertureShape
	{
		CircleShape,
		RectangleShape,
		ObroundShape,
		PolygonShape,
		CustomShape
	};
	enum eInterpolationMode
	{
		NoMode,
		//G01
		LinearMode,
		//G02
		ClockwiseCircularMode,
		//G03
		CounterClockwiseCircularMode
	};
	enum eQuadrantMode
	{
		NoQuadrant,
		SingleQuadrant,
		MultiplyQuadrant
	};
	struct _ApertureInfo
	{
		eApertureShape ApertureShape;
		int DCode;
		union _Parameter
		{
			float Diameter;
			struct  _RectangleEx
			{
				float LongAxis;
				float ShortAxis;
				//_Point CenterPoint;
			}RectAngle;

		}Parameter;
	};
	struct _NumberInfo
	{
		int IntegerBits;
		int DecimalBits;
	};
	struct _TipInfo 
	{
		eUnit Unit;
		eZeroOmit ZeroOmit;
		_NumberInfo XNumber;
		_NumberInfo YNumber;
	};
    #pragma endregion

	#pragma region 光圈形状
    class ACircle :public GxmcShape
	{
	public:
		AD_Circle m_adCircle;
		PointD m_CenterPoint;
		string m_strDCode;
		ACircle();
		ACircle(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,PointD Center,AD_Circle adCircle,string strDCode)
			:GxmcShape(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
			m_CenterPoint = Center;
			m_adCircle = adCircle;
			m_strDCode = strDCode;
		}
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "ACircle";}
	};
	class ARectangle :public GxmcShape
	{ 
	public:
		AD_Rectangle m_adRectangle;
		PointD m_CenterPoint;
		string m_strDCode;
		ARectangle(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,PointD Center,AD_Rectangle adRectangle,string strDCode)
			:GxmcShape(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
			m_CenterPoint = Center;
			m_adRectangle = adRectangle;
			m_strDCode = strDCode;
		}
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "ARectangle";}
	};
	class AObround :public GxmcShape
	{
    public:
		AD_Obround m_adObround;
		PointD m_CenterPoint;
		string m_strDCode;
		AObround(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,PointD Center,AD_Obround adObround,string strDCode)
			:GxmcShape(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
			m_CenterPoint = Center;
			m_adObround = adObround;
			m_strDCode = strDCode;
		}
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "AObround";}
	};
	class ARegularPolygon :public GxmcShape
	{
	public:
		AD_RegularPolygon m_adRegularPolygon;
		PointD m_CenterPoint;  
		string m_strDCode;
		ARegularPolygon(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,PointD Center,AD_RegularPolygon adRegularPolygon,string strDCode)
			:GxmcShape(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
			m_CenterPoint = Center;
			m_adRegularPolygon = adRegularPolygon;
			m_strDCode = strDCode;
		}
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "ARegularPolygon";}
	};

	class AMShape: public GxmcShape
	{
	public:
		AD_AmInfo m_adAmInfo;
		PointD m_CenterPoint;  
		string m_strDCode;
		AMShape(int PenWidth,int PenStyle,COLORREF PenColor,COLORREF BrushColor,int nLayer,bool bIsFilled,PointD Center,AD_AmInfo adAmInfo,string strDCode)
			:GxmcShape(PenWidth, PenStyle, PenColor, BrushColor, nLayer,bIsFilled)
		{
			m_CenterPoint = Center;
			m_adAmInfo = adAmInfo;
			m_strDCode = strDCode;
		}
		void GetBoundaryRect(RectD& rect);
		void Draw(CDC& dc,const ScreenStruct& screen);
		void SetColour(COLORREF PenColor,COLORREF BrushColor);
		std::string GetShapeStyle(){return "AMShape";}
	};
    #pragma endregion 

	//数据块,描述一个完整的封闭图形
	typedef std::list<GxmcShape*> DataBlock;
	struct Regex_Pattern
	{
		std::string prefix_ADD;
		std::string prefix_AM;
		std::string prefix_AMDescription;
		std::string prefix_FS;
		std::string prefix_MO;
		std::string prefix_LP;
	};
	struct DrawingPara 
	{
		float startX;//视图起始点
		float startY;//视图起始点
		int widthScreen; //视图宽度
		int heightScreen;//视图高度
		float factor;//一个逻辑单位所表示的实际尺寸
	};

	#pragma region FOV路径计算
	//描述焊盘 Pad
	struct  _Polygon
	{
		DataBlock dataBlock;
		RectD     margin;
		//bool     isAllocated;//判断该图形是否被划分到指定的FOV里
		bool operator == (const _Polygon& polygon)
		{
			return margin == polygon.margin;
		}
	};
	//路径优化
	struct Cluster 
	{
		//Fov里的所有图形元素
		std::vector<_Polygon> vecPolygon;
		//视野中心点
		PointD centerPoint;
		//视野边框
		GxmcRect* clusterMargin;
	};
	struct PadCluster 
	{
		//Fov里的所有图形元素
		std::map<int,_Polygon> mapPolygon;
		//视野中心点
		PointD centerPoint;
		//视野边框
		GxmcRect* clusterMargin;
	};
	class map_finder
	{
	public:
		map_finder(const _Polygon &cmp_polygon):m_cmp_polygon(cmp_polygon){}
		bool operator ()(const std::map<int,_Polygon>::value_type &pair)
			{
					return (_Polygon)pair.second == m_cmp_polygon;
			}
	private:
		const _Polygon &m_cmp_polygon;

	};

	class vector_finder
	{
	public:
		vector_finder(const _Polygon &cmp_polygon):m_cmp_polygon(cmp_polygon){}
		bool operator ()(const std::vector<_Polygon>::value_type &polygon)
		{
			return (_Polygon)polygon == m_cmp_polygon;
		}
	private:
		const _Polygon &m_cmp_polygon;

	};
	struct vector_PointLessByY 
	{
	public:
		vector_PointLessByY(){}
		bool operator() (const std::vector<_Polygon>::value_type& polygon1,const std::vector<_Polygon>::value_type& polygon2)
		{
			return polygon1.margin.bottom < polygon2.margin.bottom;
		}
	};
	struct vector_PointLessByX
	{
	public:
		vector_PointLessByX(){}
		bool operator() (const std::vector<_Polygon>::value_type& polygon1,const std::vector<_Polygon>::value_type& polygon2)
		{
			return polygon1.margin.left < polygon2.margin.left;
		}
	};

	struct map_PointLessByX 
	{
	public:
		map_PointLessByX(){}
		bool operator() (const std::map<int,_Polygon>::value_type& pair1,const std::map<int,_Polygon>::value_type& pair2)
		{
			return pair1.second.margin.left < pair2.second.margin.left;
		}
	};

	struct map_PointLessByY 
	{
	public:
		map_PointLessByY(){}
		bool operator() (const std::map<int,_Polygon>::value_type& pair1,const std::map<int,_Polygon>::value_type& pair2)
		{
			return pair1.second.margin.bottom < pair2.second.margin.bottom;
		}
	};
	struct map_PointLessByY_ 
	{
	public:
		map_PointLessByY_(){}
		bool operator() (const std::vector<_Polygon>::value_type& polygon1,const std::vector<_Polygon>::value_type& polygon2)
		{
			return polygon1.margin.bottom < polygon2.margin.bottom;
		}
	};

	//到矩形边界最近的多边形的边界

	struct map_LessDistanceTop 
	{
	public:
		map_LessDistanceTop(const Cluster& cluster):map_cluster(cluster){}
		bool operator() (const _Polygon& polygon1,const _Polygon& polygon2)
		{
			double dist1 = polygon1.margin.top - map_cluster.clusterMargin->m_stopPoint.y;
			double dist2 = polygon1.margin.top - map_cluster.clusterMargin->m_stopPoint.y;
			return dist1 < dist2;
		}

	private:
		const Cluster& map_cluster;
	};

	struct map_LessDistanceLeft 
	{
	public:
		map_LessDistanceLeft(const Cluster& cluster):map_cluster(cluster){}
		bool operator() (const _Polygon& polygon1,const _Polygon& polygon2)
		{
			double dist1 = polygon1.margin.left - map_cluster.clusterMargin->m_startPoint.x;
			double dist2 = polygon1.margin.left - map_cluster.clusterMargin->m_startPoint.x;
			return dist1 < dist2;
		}

	private:
		const Cluster& map_cluster;
	};
    #pragma endregion

	class CGxmcGerber
	{
	public:
		CGxmcGerber(void);
		~CGxmcGerber(void);
		
		void ReadGerberFile(CString fileFullName,std::list<DataBlock>& dataBlockList,RectD& realMarginRect);
		void DisplayGerber(CDC& dc,ScreenStruct screen,const std::list<DataBlock>& dataBlockList);
		void SetDrawingPara(const DrawingPara& drawpara);
		void ReleaseDataBlock(std::list<DataBlock>& dataBlockList);
		void ReleaseADStyleMap();
		void ReleaseClusterMap(std::map<int,Cluster>& mapCluster);
		void DisplayFovPath(CDC& dc,ScreenStruct screen,const std::map<int,Cluster>& mapCluster);
		//将封闭图形编码存储到map
		void GetFovPath(const std::list<DataBlock>& dataBlockList,double ViewWidth,double ViewHeight,std::map<int,Cluster>& mapCluster);
		//框选焊盘
		void SelectPads(std::list<DataBlock>& dataBlockList,std::vector<GxmcRect> vecSelectRect,ScreenStruct screen,bool isInvertSelection);
		//隐藏焊盘
		void HidePads(std::list<DataBlock>& dataBlockList,std::vector<GxmcRect> vecSelectRect,ScreenStruct screen,bool isInvertSelection);
		//选择Mark点
		void SelectMarkPoints(std::list<DataBlock>& dataBlockList,std::vector<GxmcRect> vecSelectRect,ScreenStruct screen,std::map<int,_Polygon>& mapMarkPoint);
		//显示选择框
		void DisplaySelectRect(CDC& dc,ScreenStruct screen,std::vector<GxmcRect> vecSelectRect);
		//把计算好的FOV、Mark数据导出至数据库
		void ExportFovData(const std::list<DataBlock>& dataBlockList,const std::map<int,Cluster>& mapCluster,const std::map<int,_Polygon>& mapMarkPoint);
	    //加载数据库
		void ReadFromDatabase();
		//获取数据库中提取的Fov数据集
		map<int,PadCluster> GetPadCluster();
		//显示Fov数据集
		void DisplayPadCluster(CDC& dc,ScreenStruct screen,const map<int,PadCluster>& mapPadCluster);
	private:
		void UpdateMargin(PointD point,RectD& realMarginRect);
		void UpdateMargin(RectD rect,RectD& realMarginRect);
		bool RegexSearch(std::string source,std::string pattern,std::vector<std::string>& vecMatch);
		bool RegexReplace(std::string source,std::string pattern,std::string fmt,std::string& dest);
		void ReadLine(std::ifstream& infile,bool& isReadLine,std::string& srcExpression);
		//从字符串中提取坐标值
		void GetValueFromStr(std::string strX,std::string strY,double& dPreviousX,double& dPreviousY,double& dX,double& dY);
		//读取gerber文件提示信息
		void ReadTipInfo(std::ifstream& infile,char* strBuf,_TipInfo &tipInfo,std::map<std::string,AdStyle>& ADStyleMap);
		//读取直线模式下的插值操作D01
		void ReadLinearIterpolateOperationD01(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD& point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect,bool& isD01Read,bool isFilled,bool isIncludeLine);
		//读取圆弧模式下的插值操作D01
		void ReadCircularIterpolateOperationD01(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect,bool& isD01Read,bool isFilled);
		//读取移动操作D02
		void ReadMoveOpreationD02(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect);
		//读取G75
		void ReadMultiQuadrantModeG75(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect);
        //读取AM参数
		void ReadAmPara(std::string source,std::string chToken,std::vector<std::string>& vecMatch);
		//将光圈的图元添加到数据块列表
		void AddAperture(std::map<std::string,AdStyle> ADStyleMap,std::string DCode,PointD point,DataBlock dataBlock,std::list<DataBlock>& dataBlockList);
		
		//Fov路径计算
		void CalculateFovPath(std::map<int,Cluster>& mapCluster,int& clusterIndex,std::map<int,_Polygon>& mapPolygon,double ViewWidth,double ViewHeight);
		//正则表达式 匹配模式变量
		Regex_Pattern m_RegexPattern;
		//提示性信息
		_TipInfo m_TipInfo;
		//象限模式
		eQuadrantMode m_QuadrantMode;
		DrawingPara m_drawingPara;

		//绘图参数
		int m_penWidth;
		int m_penStyle;
		COLORREF m_penColor;
		COLORREF m_brushColor;
		int m_nLayer;//图层索引
		bool m_bIsFilled;

		std::map<std::string,AdStyle> m_adStyleMap;
		//std::list<DataBlock> m_dataBlockList;
		//将封闭图形编码存储到map
		std::map<int,_Polygon> m_mapPolygon;
		//存放计算后的Fov数据
		std::map<int,Cluster> m_mapCluster;
		//存放选取的Mark
		std::map<int,_Polygon> m_mapMarkPoint;
		std::map<int,PadCluster> m_mapPadCluster;

		CGxmcDatabase m_dataBase;

	};
}

#endif

