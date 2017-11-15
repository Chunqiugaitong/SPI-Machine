
//ͷ�ļ�GxmcGerber.h

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
	#pragma region ���ֹ�Ȧ��״�ṹ��  AM  

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

    #pragma region ���ֹ�Ȧ��״�ṹ��  AD
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

    #pragma region Gerbera��Ϣ

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

	#pragma region ��Ȧ��״
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

	//���ݿ�,����һ�������ķ��ͼ��
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
		float startX;//��ͼ��ʼ��
		float startY;//��ͼ��ʼ��
		int widthScreen; //��ͼ���
		int heightScreen;//��ͼ�߶�
		float factor;//һ���߼���λ����ʾ��ʵ�ʳߴ�
	};

	#pragma region FOV·������
	//�������� Pad
	struct  _Polygon
	{
		DataBlock dataBlock;
		RectD     margin;
		//bool     isAllocated;//�жϸ�ͼ���Ƿ񱻻��ֵ�ָ����FOV��
		bool operator == (const _Polygon& polygon)
		{
			return margin == polygon.margin;
		}
	};
	//·���Ż�
	struct Cluster 
	{
		//Fov�������ͼ��Ԫ��
		std::vector<_Polygon> vecPolygon;
		//��Ұ���ĵ�
		PointD centerPoint;
		//��Ұ�߿�
		GxmcRect* clusterMargin;
	};
	struct PadCluster 
	{
		//Fov�������ͼ��Ԫ��
		std::map<int,_Polygon> mapPolygon;
		//��Ұ���ĵ�
		PointD centerPoint;
		//��Ұ�߿�
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

	//�����α߽�����Ķ���εı߽�

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
		//�����ͼ�α���洢��map
		void GetFovPath(const std::list<DataBlock>& dataBlockList,double ViewWidth,double ViewHeight,std::map<int,Cluster>& mapCluster);
		//��ѡ����
		void SelectPads(std::list<DataBlock>& dataBlockList,std::vector<GxmcRect> vecSelectRect,ScreenStruct screen,bool isInvertSelection);
		//���غ���
		void HidePads(std::list<DataBlock>& dataBlockList,std::vector<GxmcRect> vecSelectRect,ScreenStruct screen,bool isInvertSelection);
		//ѡ��Mark��
		void SelectMarkPoints(std::list<DataBlock>& dataBlockList,std::vector<GxmcRect> vecSelectRect,ScreenStruct screen,std::map<int,_Polygon>& mapMarkPoint);
		//��ʾѡ���
		void DisplaySelectRect(CDC& dc,ScreenStruct screen,std::vector<GxmcRect> vecSelectRect);
		//�Ѽ���õ�FOV��Mark���ݵ��������ݿ�
		void ExportFovData(const std::list<DataBlock>& dataBlockList,const std::map<int,Cluster>& mapCluster,const std::map<int,_Polygon>& mapMarkPoint);
	    //�������ݿ�
		void ReadFromDatabase();
		//��ȡ���ݿ�����ȡ��Fov���ݼ�
		map<int,PadCluster> GetPadCluster();
		//��ʾFov���ݼ�
		void DisplayPadCluster(CDC& dc,ScreenStruct screen,const map<int,PadCluster>& mapPadCluster);
	private:
		void UpdateMargin(PointD point,RectD& realMarginRect);
		void UpdateMargin(RectD rect,RectD& realMarginRect);
		bool RegexSearch(std::string source,std::string pattern,std::vector<std::string>& vecMatch);
		bool RegexReplace(std::string source,std::string pattern,std::string fmt,std::string& dest);
		void ReadLine(std::ifstream& infile,bool& isReadLine,std::string& srcExpression);
		//���ַ�������ȡ����ֵ
		void GetValueFromStr(std::string strX,std::string strY,double& dPreviousX,double& dPreviousY,double& dX,double& dY);
		//��ȡgerber�ļ���ʾ��Ϣ
		void ReadTipInfo(std::ifstream& infile,char* strBuf,_TipInfo &tipInfo,std::map<std::string,AdStyle>& ADStyleMap);
		//��ȡֱ��ģʽ�µĲ�ֵ����D01
		void ReadLinearIterpolateOperationD01(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD& point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect,bool& isD01Read,bool isFilled,bool isIncludeLine);
		//��ȡԲ��ģʽ�µĲ�ֵ����D01
		void ReadCircularIterpolateOperationD01(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect,bool& isD01Read,bool isFilled);
		//��ȡ�ƶ�����D02
		void ReadMoveOpreationD02(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect);
		//��ȡG75
		void ReadMultiQuadrantModeG75(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect);
        //��ȡAM����
		void ReadAmPara(std::string source,std::string chToken,std::vector<std::string>& vecMatch);
		//����Ȧ��ͼԪ��ӵ����ݿ��б�
		void AddAperture(std::map<std::string,AdStyle> ADStyleMap,std::string DCode,PointD point,DataBlock dataBlock,std::list<DataBlock>& dataBlockList);
		
		//Fov·������
		void CalculateFovPath(std::map<int,Cluster>& mapCluster,int& clusterIndex,std::map<int,_Polygon>& mapPolygon,double ViewWidth,double ViewHeight);
		//������ʽ ƥ��ģʽ����
		Regex_Pattern m_RegexPattern;
		//��ʾ����Ϣ
		_TipInfo m_TipInfo;
		//����ģʽ
		eQuadrantMode m_QuadrantMode;
		DrawingPara m_drawingPara;

		//��ͼ����
		int m_penWidth;
		int m_penStyle;
		COLORREF m_penColor;
		COLORREF m_brushColor;
		int m_nLayer;//ͼ������
		bool m_bIsFilled;

		std::map<std::string,AdStyle> m_adStyleMap;
		//std::list<DataBlock> m_dataBlockList;
		//�����ͼ�α���洢��map
		std::map<int,_Polygon> m_mapPolygon;
		//��ż�����Fov����
		std::map<int,Cluster> m_mapCluster;
		//���ѡȡ��Mark
		std::map<int,_Polygon> m_mapMarkPoint;
		std::map<int,PadCluster> m_mapPadCluster;

		CGxmcDatabase m_dataBase;

	};
}

#endif

