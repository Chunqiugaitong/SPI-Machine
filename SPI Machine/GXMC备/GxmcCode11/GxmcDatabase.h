#pragma once
#include "stdafx.h"
#include <vector>
#include <map>
#include "GxmcGerber.h"

#import "C:/Program Files/Common Files/System/ado/msadox.dll"   
#import "C:/Program Files/Common Files/System/ado/msado15.dll" \
	no_namespace                                           \
	rename("BOF","adoBOF")                                  \
	rename("EOF","adoEOF")

using namespace std;
//using namespace ADOX;

namespace Gxmc_Gerber
{
  typedef struct  tagAdStyle AdStyle;
  struct AM_Circle;
  struct AM_CenterLine;
  struct AM_VectorLine;
  struct AM_OutLine;
  struct AM_Polygon;
  struct AM_Moire;
  struct AM_Thermal;

  class ACircle;
  class ARectangle;
  class AObround;
  class ARegularPolygon;
  class AMShape;
 
}

class CGxmcDatabase
{
public:
	typedef struct PadElement 
	{
		CString ID;
		CString PadID;
		CString BadMarkID;
		CString BoardID;
		CString BlockID;
		CString ComponentID;

		CString PackageType;
		CString SpecIndex;
		CString PinNumber;

		CString ShapeID;
		CString ExtXum;
		CString ExtYum;

		CString FovNo;
		CString PosXmm;
		CString PosYmm;
		CString Rotation;

		CString BaseType;

		CString Check;
		CString CheckVol;
		CString CheckArea;
		CString CheckHeight;
		CString CheckShape;
		CString CheckBridge;
		CString CheckOffset;
		CString CheckCoplanarity;

		CString HeightU;
		CString HeightL;
		CString AreaU;
		CString AreaL;
		CString VolU;
		CString VolL;
		CString ShiftXU;
		CString ShiftYU;

		CString ShapeDeltaU;
		CString OffsetMark;
		CString StencilHeight;
		CString BridgeWidth;
		CString BridgeLength;
		CString BridgeHeight;
		CString PadGroup;
		CString UserAdjustMeasuredHeight;
	} PadElement;
	typedef struct Pad 
	{
		int ID;
		int PadID;
		int BadMarkID;
		CString BoardID;
		CString BlockID;
		CString ComponentID;

		CString PackageType;
		CString SpecIndex;
		CString PinNumber;

		int ShapeID;
		double ExtXum;
		double ExtYum;

		int FovNo;
		double PosXmm;
		double PosYmm;
		int Rotation;

		int BaseType;

		bool Check;
		bool CheckVol;
		bool CheckArea;
		bool CheckHeight;
		bool CheckShape;
		bool CheckBridge;
		bool CheckOffset;
		bool CheckCoplanarity;

		double HeightU;
		double HeightL;
		double AreaU;
		double AreaL;
		double VolU;
		double VolL;
		double ShiftXU;
		double ShiftYU;

		double ShapeDeltaU;
		int OffsetMark;
		double StencilHeight;
		double BridgeWidth;
		double BridgeLength;
		double BridgeHeight;
		CString PadGroup;
		double UserAdjustMeasuredHeight;

	}Pad;

	typedef struct BareBoardElement
	{
		CString ID;
		CString PadID;
		CString VenderName;
		CString VenderID;
		CString Height;

	}BareBoardElement;
	typedef struct BareBoard
	{
		int ID;
		int PadID;
		CString VenderName;
		CString VenderID;
		int Height;
	}BareBoard;

	typedef struct FovPathElement
	{
		CString FovNo;
		CString PosX;
		CString PosY;
		CString SizeX;
		CString SizeY;
		CString CaptureType;
		CString LumiPara0;
		CString LumiPara1;
		CString LumiPara2;
		CString LumiPara3;
		CString LumiPara4;
		CString LumiPara5;
		CString LumiPara6;
		CString LumiPara7;
		CString BlockID;
	}FovPathElement;
	typedef struct FovPath
	{
		int FovNo;
		double PosX;
		double PosY;
		double SizeX;
		double SizeY;
		int CaptureType;
		int LumiPara0;
		int LumiPara1;
		int LumiPara2;
		int LumiPara3;
		int LumiPara4;
		int LumiPara5;
		int LumiPara6;
		int LumiPara7;
		int BlockID;
	}FovPath;

	typedef struct GeneralElement
	{
		CString CreatDateTime;
		CString LastSaveTime;
		CString BoardWidth;
		CString BoardLength;
		CString CreatBy;
		CString JobFileVersion;
		CString Rotation;
		CString BoardOffsetX;
		CString BoardOffsetY;
		CString ID;
		CString PCBType;
		CString MeasureRange;
		CString HeightType;

	}GeneralElement;
	typedef struct General
	{
		COleDateTime CreatDateTime;
		COleDateTime LastSaveTime;
		double BoardWidth;
		double BoardLength;
		CString CreatBy;
		CString JobFileVersion;
		double Rotation;
		double BoardOffsetX;
		double BoardOffsetY;
		int ID;
		int PCBType;
		double MeasureRange;
		int HeightType;
	}General;

	typedef struct MarkElement
	{
		CString ID;
		CString Type;
		CString Shape;
		CString GlbOffsetXmm;
		CString GlbOffsetYmm;
		CString PosXmm;
		CString PosYmm;
		CString SearchSizeXmm;
		CString SearchSizeYmm;
		CString SizeXmm;
		CString SizeYmm;
		CString MeasureRange;
		CString IsBright;

		CString GrayThresh;
		CString LumiType;
		CString LumiPara0;
		CString LumiPara1;
		CString LumiPara2;
		CString LumiPara3;
		CString LumiPara4;
		CString LumiPara5;
	//	CString TemplateImage;
		CString ImgScale;
		CString ImgWidth;
		CString ImgHeight;
		CString ImgPara0;
		CString ImgPara1;
		CString ImgPara2;
		CString ImgPara3;
		CString ImgPara4;
		CString ScoreMin;
		CString BlockID;

	}MarkElement;
	typedef struct Mark
	{
		int  ID;
		int Type;
		int Shape;
		double GlbOffsetXmm;
		double GlbOffsetYmm;
		double PosXmm;
		double PosYmm;
		double SearchSizeXmm;
		double SearchSizeYmm;
		double SizeXmm;
		double SizeYmm;
		double MeasureRange;
		bool IsBright;

		int GrayThresh;
		int LumiType;
		int LumiPara0;
		int LumiPara1;
		int LumiPara2;
		int LumiPara3;
		int LumiPara4;
		int LumiPara5;
		//COleDataObject TemplateImage;
		double ImgScale;
		int ImgWidth;
		int ImgHeight;
		int ImgPara0;
		int ImgPara1;
		int ImgPara2;
		int ImgPara3;
		int ImgPara4;
		double ScoreMin;
		int BlockID;
	}Mark;

	typedef struct ShapeElement
	{
		CString ShapeID;
		CString ShapeStyle;
		CString SizeXmm;
		CString SizeYmm;
		CString StencilAreaPer;
		CString CShiftXmm;
		CString CShiftYmm;
		CString UseMask;
		CString MaskPattern;
		CString MaskScale;
		CString PixLength;
		CString PixWidth;
	}ShapeElement;
	typedef struct Shape
	{
		int ShapeID;
		int ShapeStyle;
		double SizeXmm;
		int SizeYmm;
		double StencilAreaPer;
		int CShiftXmm;
		int CShiftYmm;
		bool UseMask;
		COleDataObject MaskPattern;
		double MaskScale;
		double PixLength;
		double PixWidth;
	}Shape;

	typedef struct Shape_CircleElement
	{
		CString circleID;
		CString centerX;
		CString centerY;
		CString radius;
	}Shape_CircleElement;
	typedef struct Shape_Circle
	{
		int circleID;
		double centerX;
		double centerY;
		double radius;
	}Shape_Circle;

	typedef struct Shape_RectangleElement
	{
		CString rectID;
		CString left;
		CString top;
		CString right;
		CString bottom;
	}Shape_RectangleElement;
	typedef struct Shape_Rectangle
	{
		int rectID;
		double left;
		double top;
		double right;
		double bottom;
	}Shape_Rectangle;

	typedef struct Shape_ArcElement
	{
		CString nIndex;
		CString arcID;
		CString arcDirection;
		CString startPointX,startPointY;
		CString stopPointX,stopPointY;
		CString centerPointX,centerPointY;
		CString radius;
		
	}Shape_ArcElement;
	typedef struct Shape_Arc
	{
		int nIndex; 
		int arcID;
		int arcDirection;
		double startPointX,startPointY;
		double stopPointX,stopPointY;
		double centerPointX,centerPointY;
		double radius;
		
	}Shape_Arc;

	typedef struct Shape_PolygonElement
	{
		CString nIndex;
		CString polygonID;
		CString pointX;
		CString pointY;

	}Shape_PolygonElement;
	typedef struct Shape_Polygon
	{
		int nIndex;
		int polygonID;
		double pointX;
		double pointY;
	}Shape_Polygon;

	typedef struct ShapeItemElement
	{
		CString nIndex;
		CString shapeID;
		CString polygonID;
		CString arcID;
		CString DCode;

	}ShapeItemElement;
	typedef struct ShapeItem
	{
		int nIndex;
		int shapeID;
		int polygonID;
		int arcID;
		string DCode;
	}ShapeItem;

	typedef struct AdRectangleElement
	{
		CString DCode;
		CString Width;
		CString Height;
		CString HoleDiameter;
	}AdRectangleElement;
	typedef struct AdRectangle
	{
		string DCode;
		double Width;
		double Height;
		double HoleDiameter;
	}AdRectangle;
	typedef struct AdCircleElement
	{
		CString DCode;
		CString Diameter;
		CString HoleDiameter;
	}AdCircleElement;
	typedef struct AdCircle
	{
		string DCode;
		double Diameter;
		double HoleDiameter;
	}AdCircle;

	typedef struct AdObroundElement
	{
		CString DCode;
		CString Width;
		CString Height;
		CString HoleDiameter;
	}AdObroundElement;
	typedef struct AdObround
	{
		string DCode;
		double Width;
		double Height;
		double HoleDiameter;
	}AdObround;

	typedef struct AdRegularPolygonElement
	{
		CString DCode;
	}AdRegularPolygonElement;
	typedef struct AdRegularPolygon
	{
		string DCode;
	}AdRegularPolygon;

	typedef struct AdAmElement
	{
		CString DCode;
		CString CircleID;
		CString VectorLineID;
		CString CenterLineID;
		CString OutLineID;
		CString PolygonID;
		CString MoireID;
		CString ThermalID;
	}AdAmElement;
	typedef struct AdAm
	{
		string DCode;
		int CircleID;
		int VectorLineID;
		int CenterLineID;
		int OutLineID;
		int PolygonID;
		int MoireID;
		int ThermalID;
	}AdAm;

	typedef struct AmCircleElement
	{
		CString CircleID;
		CString exposure;
		CString diameter;
		CString centerX;
		CString centerY;
		CString rotAngle;
	}AmCircleElement;
	typedef struct AmCircle
	{
		int CircleID;
		int    exposure;
		double diameter;
		double centerX;
		double centerY;
		double  rotAngle;
	}AmCircle;

	typedef struct AmVectorLineElement
	{
		CString VectorLineID;
		CString    exposure;
		CString lineWidth;
		CString startX;
		CString startY;
		CString stopX;
		CString stopY;
		CString rotAngle;
	}AmVectorLineElement;
	typedef struct AmVectorLine
	{
		int VectorLineID;
		int    exposure;
		double lineWidth;
		double startX;
		double startY;
		double stopX;
		double stopY;
		double rotAngle;
	}AmVectorLine;

	typedef struct AmCenterLineElement
	{
		CString CenterLineID;
		CString exposure;
		CString rectWidth;
		CString rectHeight;
		CString centerX;
		CString centerY;
		CString rotAngle;
	}AmCenterLineElement;
	typedef struct AmCenterLine
	{
		int CenterLineID;
		int    exposure;
		double rectWidth;
		double rectHeight;
		double centerX;
		double centerY;
		double rotAngle;
	}AmCenterLine;

	typedef struct AmOutLineElement
	{
		CString OutLineID;
		CString exposure;
		CString numVertex;
		CString pointX;
		CString pointY;
		CString rotAngle;
	}AmOutLineElement;
	typedef struct AmOutLine
	{
		int    OutLineID;
		int    exposure;
		int    numVertex;
		double pointX;
		double pointY;
		double rotAngle;
	}AmOutLine;
	typedef struct AmPolygonElement
	{
		CString PolygonID; 
		CString exposure;
		CString numVertex;
		CString centerX;
		CString centerY;
		CString diameter;
		CString rotAngle;
	}AmPolygonElement;
	typedef struct AmPolygon 
	{
		int    PolygonID; 
		int    exposure;
		int    numVertex;
		double centerX;
		double centerY;
		double diameter;
		double rotAngle;
	}AmPolygon;
	typedef struct AmMoireElement
	{
		CString MoireID;
		CString centerX;
		CString centerY;
		CString outerDiameter;
		CString ringThickness;
		CString ringGap;
		CString ringMaxNum;
		CString crosshairThickness;
		CString crosshairLength;
		CString  rotAngle;
	}AmMoireElement;
	typedef struct AmMoire 
	{
		int MoireID;
		double centerX;
		double centerY;
		double outerDiameter;
		double ringThickness;
		double ringGap;
		int    ringMaxNum;
		double crosshairThickness;
		double crosshairLength;
		double  rotAngle;
	}AmMoire;
	typedef struct ThermalElement
	{
		CString ThermalID;
		CString centerX;
		CString centerY;
		CString outerDiameter;
		CString innerDiameter;
		CString gapThickness;
		CString  rotAngle;
	}AmThermalElement;
	typedef struct AmThermal
	{
		int    ThermalID;
		double centerX;
		double centerY;
		double outerDiameter;
		double innerDiameter;
		double gapThickness;
		double rotAngle;
	}AmThermal;

	typedef struct TableList
	{
		CString BareBoard;
		CString FovPath;
		CString General;
		CString Mark;
		CString Pad;
		CString Shape;
		//Shape
		CString ShapeItem;
		CString Shape_Arc;
		CString Shape_Polygon;
		CString Shape_Circle;
		CString Shape_Rectangle;
		//AD_Shape
		CString AdRectangle;
		CString AdCircle;
		CString AdObround;
		CString AdRegularPolygon;
		CString AdAm;
		//AM_Shape
		CString AmCircle;
		CString AmVectorLine;
		CString AmCenterLine;
		CString AmOutLine;
		CString AmPolygon; 
		CString AmMoire;
		CString AmThermal;

	}TableList;

	enum TableStyle
	{
		enumBareBoard,
		enumFovPath,
		enumGeneral,
		enumMark,
		enumPad,
		enumShape,
		//Shape
		enumShapeItem,
		enumShape_Arc,
		enumShape_Polygon,
		enumShape_Circle,
		enumShape_Rectangle,
		//AD_Shape
		enumAdRectangle,
		enumAdCircle,
		enumAdObround,
		enumAdRegularPolygon,
		enumAdAm,
		//AM_Shape
		enumAmCircle,
		enumAmVectorLine,
		enumAmCenterLine,
		enumAmOutLine,
		enumAmPolygon,
		enumAmMoire,
		enumAmThermal
	};

	static  PadElement s_padElement;
	static  BareBoardElement s_bareBoardElement;
	static  FovPathElement s_fovPathElement;
	static  GeneralElement s_generalElement;
	static  MarkElement s_markElement;
	static  ShapeElement s_shapeElement;
	//Shape
	static Shape_ArcElement s_arcElement;
	static Shape_RectangleElement s_rectangleElement;
	static Shape_CircleElement s_circleElement;
	static Shape_PolygonElement s_polygonElement;
	static ShapeItemElement s_shapeItemElement;

	//AD_Shape
	static AdRectangleElement s_adRectangleElement;
	static AdCircleElement s_adCircleElement;
	static AdObroundElement s_adObroundElement;
	static AdRegularPolygonElement s_adRegularPolygonElement;
	static AdAmElement s_adAmElement;
	//AM_Shape
 	static AmCircleElement s_amCircleElement;
 	static AmVectorLineElement s_amVectorLineElement;
 	static AmCenterLineElement s_amCenterLineElement;
 	static AmOutLineElement s_amOutLineElement;
 	static AmPolygonElement s_amPolygonElement;
 	static AmMoireElement s_amMoireElement;
 	static AmThermalElement s_amThermalElement;

	static  vector<TableStyle> s_vecTableStyle;
	static  TableList tableList;

private:
	static  bool bIni;
	static  bool InitialVecTable()
	{
		s_vecTableStyle.push_back(enumBareBoard);
		s_vecTableStyle.push_back(enumFovPath);
		s_vecTableStyle.push_back(enumGeneral);
		s_vecTableStyle.push_back(enumMark);
		s_vecTableStyle.push_back(enumPad);
		s_vecTableStyle.push_back(enumShape);
		s_vecTableStyle.push_back(enumShapeItem);
		s_vecTableStyle.push_back(enumShape_Arc);
		s_vecTableStyle.push_back(enumShape_Polygon);
		s_vecTableStyle.push_back(enumShape_Circle);
		s_vecTableStyle.push_back(enumShape_Rectangle);

		s_vecTableStyle.push_back(enumAdRectangle);
		s_vecTableStyle.push_back(enumAdCircle);
		s_vecTableStyle.push_back(enumAdObround);
		s_vecTableStyle.push_back(enumAdRegularPolygon);
		s_vecTableStyle.push_back(enumAdAm);

		s_vecTableStyle.push_back(enumAmCircle);
		s_vecTableStyle.push_back(enumAmCenterLine);
		s_vecTableStyle.push_back(enumAmVectorLine);
		s_vecTableStyle.push_back(enumAmOutLine);
		s_vecTableStyle.push_back(enumAmPolygon);
		s_vecTableStyle.push_back(enumAmMoire);
		s_vecTableStyle.push_back(enumAmThermal);
		
		return true;
	}

public:
	CGxmcDatabase(void);
	~CGxmcDatabase(void);
private:
	//ADO创建数据库的表，ADO没有创建数据库的功能
	_ConnectionPtr   m_pConnection; // 数据库 
	_RecordsetPtr    m_pRecordset; //  记录
	_CommandPtr      m_pCommand; // 命令
	//ADOX创建数据库，可以创建、删除、修改表和存储过程
	ADOX::_CatalogPtr  m_pCatalog;//数据库目录
	ADOX::_TablePtr    m_pTable;//数据表

	bool m_IsRecordsetOpen;
	
public:
	//打开数据库文件
	void ConnectAdo(CString DataBaseFileName);
	//关闭数据库
	void ExitConnect();
	//打开数据表
	void OpenTable(CString tableName);
	void CloseTable();
	//判断数据表是否已经存在
	bool IsTableExist(CString DataBaseFileName,CString tableName);
	bool IsDatabaeOpen()
	{
		return (m_pConnection->GetState()>0);
	}

    /*************************************************************************
	//            对字段的获取、设置  
	/*************************************************************************/
	//根据键值查找该纪录中指定字段的值
	void GetItemByKey(CString tableName,CString KeyName,CString KeyValue,CString fieldName,CString& fieldValue);
	//根据键值修改该纪录中指定字段的值
	void SetItemByKey(CString tableName,CString KeyName,CString KeyValue,CString fieldName,CString fieldValue);

	/*************************************************************************
	//            获取所有纪录中指定字段值的集合
	/*************************************************************************/
	void ReadItemsFromTable(CString fieldName,CString tableName,vector<CString>& vecItems);
	void ReadRecordFromTable(CString tableName,PadElement padElement,Pad pad);
	/*************************************************************************
	//            存储过程的调用
	/*************************************************************************/
	void Procedure(string procName);
	void ReadProcedure();

	/*************************************************************************
	//          对纪录的操作 增、删、查、改
	/*************************************************************************/
	//向表中添加一条新纪录                 
	void AddNewRecordToTable(CString tableName,PadElement padElement,Pad pad);
	//删除表中指定的一条纪录               
	void DeleteRecordFromTable(CString tableName,CString KeyName,CString KeyValue);
	//                                     
	int FindRecordByKey(CString tableName,CString KeyName,CString KeyValue);
	//修改表中指定字段的值                 
	void ModifyItemOfTable(CString fieldName,CString tableName,vector<CString> vecItems);

	/*************************************************************************
	//              创建表、添加纪录
	/*************************************************************************/
	void CreateTable(CString DataBaseFileName,vector<TableStyle> vecTableStyle);
	void InitialTableHeader(TableStyle tableStyle);
	void InitialTableHeaderBySQL(TableStyle tableStyle);
	//添加Mark纪录
	void AddNewRecordToTable(CString tableName,MarkElement markElement,Mark mark);
	//添加FovPath纪录
	void AddNewRecordToTable(CString tableName,FovPathElement fovPathElement,FovPath fovPath);
	//添加Arc纪录
	void AddNewRecordToTable(CString tableName,Shape_ArcElement shapeArcElement,Shape_Arc shapeArc);
	//添加ArcPolygon纪录
	void AddNewRecordToTable(CString tableName,ShapeItemElement shapeItemElement,ShapeItem shapeItem);
	//添加Polygon纪录
	void AddNewRecordToTable(CString tableName,Shape_PolygonElement shapePolygonElement,Shape_Polygon shapePolygon);

	//添加AdCircle纪录
	void AddNewRecordToTable(CString tableName,AdCircleElement adCircleElement,AdCircle adCircle);
	//添加AdRectangle纪录
	void AddNewRecordToTable(CString tableName,AdRectangleElement adRectangleElement,AdRectangle adRectangle);
	//添加AdObround纪录
	void AddNewRecordToTable(CString tableName,AdObroundElement adObroundElement,AdObround adObround);
	//添加AdRegularPolygon纪录
	void AddNewRecordToTable(CString tableName,AdRegularPolygonElement adRegularPolygonElement,AdRegularPolygon adRegularPolygon);
	//添加AdAm纪录
	void AddNewRecordToTable(CString tableName,AdAmElement adAmElement,AdAm adAm);

	//添加AmCircle纪录
	void AddNewRecordToTable(CString tableName,AmCircleElement amCircleElement,AmCircle amCircle);
	//添加AmCenterLine纪录
	void AddNewRecordToTable(CString tableName,AmCenterLineElement amCenterLineElement,AmCenterLine amCenterLine);
	//添加AmVectorLine纪录
	void AddNewRecordToTable(CString tableName,AmVectorLineElement amVectorLineElement,AmVectorLine amVectorLine);
	//添加AmOutLine纪录
	void AddNewRecordToTable(CString tableName,AmOutLineElement amOutLineElement,AmOutLine amOutLine);
	//添加AmPolygon纪录
	void AddNewRecordToTable(CString tableName,AmPolygonElement amPolygonElement,AmPolygon amPolygon);
	//添加AmMoire纪录
	void AddNewRecordToTable(CString tableName,AmMoireElement amMoireElement,AmMoire amMoire);
	//添加AmThermal纪录
	void AddNewRecordToTable(CString tableName,AmThermalElement amThermalElement,AmThermal amThermal);

	/*************************************************************************
	//              读取纪录
	/*************************************************************************/
	//读取Mark纪录
	void ReadRecordFromTable(CString tableName,MarkElement markElement,vector<Mark>& vecMark);
	//读取FovPath纪录
	void ReadRecordFromTable(CString tableName,FovPathElement fovPathElement,vector<FovPath>& vecFovPath);
	//读取Arc纪录
	void ReadRecordFromTable(CString tableName,Shape_ArcElement shapeArcElement,map<int,vector<Shape_Arc>>& mapShapeArc);
	//读取ArcPolygon纪录
	void ReadRecordFromTable(CString tableName,ShapeItemElement shapeItemElement,map<int,ShapeItem>& mapShapeItem);
	//读取Polygon纪录
	void ReadRecordFromTable(CString tableName,Shape_PolygonElement shapePolygonElement,map<int,vector<Gxmc_Draw::PointD>>& mapShapePolygon);
	//读取焊盘表
	void ReadRecordFromTable(CString tableName,PadElement padElement,map<int,Pad>& mapPad);
	//读取AdCircle纪录
	void ReadRecordFromTable(CString tableName,AdCircleElement adCircleElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap);
	//读取AdRectangle纪录
	void ReadRecordFromTable(CString tableName,AdRectangleElement adRectangleElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap);
	//读取AdObround纪录
	void ReadRecordFromTable(CString tableName,AdObroundElement adObroundElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap);
	//读取AdRegularPolygon纪录
	void ReadRecordFromTable(CString tableName,AdRegularPolygonElement adRegularPolygonElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap);
	//读取AdAm纪录
	void ReadRecordFromTable(CString tableName,AdAmElement adAmElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap);

	//读取AmCircle纪录
	void ReadRecordFromTable(CString tableName,AmCircleElement amCircleElement,std::map<int,vector<Gxmc_Gerber::AM_Circle>>& mapAmCircle);
	//读取AmCenterLine纪录
	void ReadRecordFromTable(CString tableName,AmCenterLineElement amCenterLineElement,std::map<int,vector<Gxmc_Gerber::AM_CenterLine>>& mapCenterLine);
	//读取AmVectorLine纪录
	void ReadRecordFromTable(CString tableName,AmVectorLineElement amVectorLineElement,std::map<int,vector<Gxmc_Gerber::AM_VectorLine>>& mapVectorLine);
	//读取AmOutLine纪录
	void ReadRecordFromTable(CString tableName,AmOutLineElement amOutLineElement,std::map<int,vector<Gxmc_Gerber::AM_OutLine>>& mapOutLine);
	//读取AmPolygon纪录
	void ReadRecordFromTable(CString tableName,AmPolygonElement amPolygonElement,std::map<int,vector<Gxmc_Gerber::AM_Polygon>>& mapPolygon);
	//读取AmMoire纪录
	void ReadRecordFromTable(CString tableName,AmMoireElement amMoireElement,std::map<int,vector<Gxmc_Gerber::AM_Moire>>& mapMoire);
	//读取AmThermal纪录
	void ReadRecordFromTable(CString tableName,AmThermalElement amThermalElement,std::map<int,vector<Gxmc_Gerber::AM_Thermal>>& mapThermal);
};

