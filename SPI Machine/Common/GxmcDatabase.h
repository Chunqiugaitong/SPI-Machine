#pragma once
//#include "stdafx.h"
#include <vector>

#import "C:/Program Files/Common Files/System/ado/msadox.dll"   
#import "C:/Program Files/Common Files/System/ado/msado15.dll"       \
no_namespace                                                         \
rename("BOF","adoBOF")                                               \
rename("EOF","adoEOF")

using namespace std;
using namespace ADOX;

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

	typedef struct TableList
	{
		CString BareBoard;
		CString FovPath;
		CString General;
		CString Mark;
		CString Pad;
		CString Shape;

	}TableList;

	enum TableStyle
	{
		enumBareBoard,
		enumFovPath,
		enumGeneral,
		enumMark,
		enumPad,
		enumShape
	};

	static  PadElement s_padElement;
	static  BareBoardElement s_bareBoardElement;
	static  FovPathElement s_fovPathElement;
	static  GeneralElement s_generalElement;
	static  MarkElement s_markElement;
	static  ShapeElement s_shapeElement;
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

		return true;
	}

public:
	CGxmcDatabase(void);
	~CGxmcDatabase(void);

private:
	_ConnectionPtr   m_pConnection; // 数据库 
	_RecordsetPtr    m_pRecordset; //  记录
	_CommandPtr      m_pCommand; // 命令

	_CatalogPtr  m_pCatalog;//数据库目录
	_TablePtr    m_pTable;//数据表

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
		return m_pConnection->GetState();
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
	//              数据表的操作
	/*************************************************************************/
	void CreateTable(CString DataBaseFileName,CString tableName,vector<TableStyle> vecTableStyle);
	void InitialTableHeader(TableStyle tableStyle);
	void InitialTableHeaderBySQL(TableStyle tableStyle);
	//添加Mark纪录
	void AddNewRecordToTable(CString tableName,MarkElement markElement,Mark mark);
};

