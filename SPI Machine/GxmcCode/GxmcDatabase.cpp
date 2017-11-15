#pragma once
#include "StdAfx.h"

#include "GxmcGerber.h"
#include "GxmcDatabase.h"

CGxmcDatabase::CGxmcDatabase(void)
{
	//判断Ole组件是否已被初始化
	if (AfxOleGetMessageFilter() == NULL)
	{
		//Ole是建立在COM组件技术之上的技术
		//同一个进程，只能调用一次AfxOleInit，不能调用第二次，多次调用必然失败。
		//一般来说，推荐在程序入口函数中调用AfxOleInit，调用之后，你封装的类无需再调用AfxOleInit。
		//	可以尝试用AfxOleGetMessageFilter()   !=   NULL判断OLE是否初始化
		AfxOleInit();
	}
    m_pCatalog = NULL;
	m_pTable = NULL;
	m_IsRecordsetOpen = false;
}
CGxmcDatabase::~CGxmcDatabase(void)
{

}
//静态变量初始化
vector<CGxmcDatabase::TableStyle> CGxmcDatabase::s_vecTableStyle;
bool CGxmcDatabase::bIni = CGxmcDatabase::InitialVecTable();

#pragma region 数据库、数据表的打开及关闭
//打开数据库
void CGxmcDatabase::ConnectAdo(CString DataBaseFileName)
{
	//初始化ADO组件
	//MFC 环境下用AfxOleInit()可自动实现初始化COM和结束时关闭COM的操作
	CoInitialize(NULL);
	try
	{
		//创建连接对象实例；
		// m_pConnection.CreateInstance(__uuidof(Connection));
		m_pConnection.CreateInstance("ADODB.Connection");
		//设置连接字符串
		//_bstr_t strConnect  = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=ARM-MDL-V1.3(WD658)-A.mdb";
		//CString strConnect_ = L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=shujuku.mdb;Persist Security Info=False";

		CString strConnect;
		strConnect.Format(L"DRIVER={Microsoft Access Driver (*.mdb)};uid=;pwd=;DBQ=%s;",DataBaseFileName);
		//使用Open连接数据库
		m_pConnection->Open((_bstr_t)strConnect,"","",adModeUnknown);
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//关闭数据库
void CGxmcDatabase::ExitConnect()
{
	if (m_pConnection != NULL)
	{
		m_pRecordset->Close();
		m_pConnection->Close();

		m_pRecordset  = NULL;
		m_pConnection = NULL;
	}
	//关闭初始化的ADO组件
	CoUninitialize();
	return;
}
//打开数据库中的表
void CGxmcDatabase::OpenTable(CString tableName)
{
	try
	{
		//创建数据记录集合对象指针
		if (m_pRecordset == NULL)
		{
			//m_pRecordset.CreateInstance(__uuidof(Recordset));
			m_pRecordset.CreateInstance("ADODB.Recordset");
		}
// 		else
// 		{
// 			m_pRecordset->Close();
// 			m_pRecordset = NULL;
// 			m_pRecordset.CreateInstance("ADODB.Recordset");
// 		} 
		//设置查询字符串
		CString bstrSQL = L"select * from "; 
		bstrSQL += tableName /*+ L" where " + fieldName*/;
		if (!m_IsRecordsetOpen)
		{
			m_pRecordset->Open((_bstr_t)bstrSQL,this->m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
			m_IsRecordsetOpen = true;
		}
		
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
void CGxmcDatabase::CloseTable()
{
	if (m_pRecordset != NULL)
	{
		m_pRecordset->Close();
		m_pRecordset = NULL;
		m_IsRecordsetOpen = false;
	}
}

//判断数据库中数据表是否已经存在
bool CGxmcDatabase::IsTableExist(CString DataBaseFileName,CString tableName)
{
	try
	{
		m_pCatalog.CreateInstance(__uuidof(ADOX::Catalog)); 
		m_pCatalog->PutActiveConnection(_bstr_t(DataBaseFileName)); 
		int tableCount=m_pCatalog->Tables->Count;
		int i=0;
		while(i<tableCount)
		{
			m_pTable=(ADOX::_TablePtr)m_pCatalog->Tables->GetItem((long)i); 
			CString _tablename=(BSTR)m_pTable->Name;
			if(_tablename==tableName)
			{
				AfxMessageBox(L"该表已经存在!");
				return true;
			}
			i++;
		}
		return false;
	}
	
	catch(_com_error &e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return true;
	}
 	
}
#pragma endregion 

#pragma region  对字段的获取、设置

void CGxmcDatabase::GetItemByKey(CString tableName,CString KeyName,CString KeyValue,CString fieldName,CString& fieldValue)
{
	OpenTable(tableName);
	while (!m_pRecordset->adoEOF)
	{
		_variant_t val;
		val = m_pRecordset->GetCollect((_variant_t)KeyName);
		
		if ( val.vt != VT_NULL && KeyValue == (LPCTSTR)(_bstr_t)val )
		{
			fieldValue = m_pRecordset->GetCollect((_variant_t)fieldName);
			return;
		}
		
		m_pRecordset->MoveNext();	
	}
	//AfxMessageBox(L"对应键值的记录不存在！");
	CloseTable();
	return;
}

void CGxmcDatabase::SetItemByKey(CString tableName,CString KeyName,CString KeyValue,CString fieldName,CString fieldValue)
{
	OpenTable(tableName);
	while (!m_pRecordset->adoEOF)
	{
		_variant_t val;
		val = m_pRecordset->GetCollect((_variant_t)KeyName);

		if ( val.vt != VT_NULL && KeyValue == (LPCTSTR)(_bstr_t)val )
		{
			m_pRecordset->PutCollect((_variant_t)fieldName,(_variant_t)fieldValue);
			m_pRecordset->Update();
			return;
		}

		m_pRecordset->MoveNext();	
	}
	//AfxMessageBox(L"对应键值的记录不存在！");
	CloseTable();
	return;
}

#pragma endregion 

#pragma region 获取所有纪录中指定字段值的集合

void CGxmcDatabase::ReadItemsFromTable(CString fieldName,CString tableName,vector<CString>& vecItems)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	//读取数据表中各字段
	_variant_t val;
	CString strPosX,strPosY;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)fieldName);
		if (val.vt != VT_NULL)
		{
			strPosX = (LPCTSTR)_bstr_t(val);
			vecItems.push_back(strPosX);
		}
		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}

//读取Mark纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,MarkElement markElement,vector<Mark>& vecMark)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	Mark mark;
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)markElement.PosXmm);
		mark.PosXmm = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)markElement.PosYmm);
		mark.PosYmm = atof(_bstr_t(val));
		vecMark.push_back(mark);
		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取FovPath纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,FovPathElement fovPathElement,vector<FovPath>& vecFovPath)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	FovPath fovPath;
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)fovPathElement.FovNo);
		fovPath.FovNo = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)fovPathElement.PosX);
		fovPath.PosX = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)fovPathElement.PosY);
		fovPath.PosY = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)fovPathElement.SizeX);
		fovPath.SizeX = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)fovPathElement.SizeY);
		fovPath.SizeY = atof(_bstr_t(val));

		vecFovPath.push_back(fovPath);
		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取Arc纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,Shape_ArcElement shapeArcElement,map<int,vector<Shape_Arc>>& mapShapeArc)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	int nArcID = 0;
	Shape_Arc shapeArc;
	vector<Shape_Arc> vecShapeArc;
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)shapeArcElement.arcDirection);
		shapeArc.arcDirection = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)shapeArcElement.centerPointX);
		shapeArc.centerPointX = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)shapeArcElement.centerPointY);
		shapeArc.centerPointY = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)shapeArcElement.radius);
		shapeArc.radius = atof(_bstr_t(val));

		val = m_pRecordset->GetCollect((LPCTSTR)shapeArcElement.startPointX);
		shapeArc.startPointX = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)shapeArcElement.startPointY);
		shapeArc.startPointY = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)shapeArcElement.stopPointX);
		shapeArc.stopPointX = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)shapeArcElement.stopPointY);
		shapeArc.stopPointY = atof(_bstr_t(val));

		val = m_pRecordset->GetCollect((LPCTSTR)shapeArcElement.arcID);
		shapeArc.arcID = atoi(_bstr_t(val));
// 		if (shapeArc.arcID > 154)
// 		{
// 			int cbv =98;
// 		}
		if (shapeArc.arcID == nArcID)
		{
			vecShapeArc.push_back(shapeArc);
			if (shapeArc.arcID < nArcID)
			{
				mapShapeArc.insert(make_pair(nArcID,vecShapeArc));
			}
		}
		else
		{
			mapShapeArc.insert(make_pair(nArcID,vecShapeArc));
			nArcID++;
			vector<Shape_Arc> vecShapeArcTmp;
			vecShapeArc.swap(vecShapeArcTmp);
			//将vector插入map后，把当前shapearc压入清空后的vector
			vecShapeArc.push_back(shapeArc);
		}

		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取ArcPolygon纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,ShapeItemElement shapeItemElement,map<int,ShapeItem>& mapShapeItem)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		ShapeItem shapeItem;
		val = m_pRecordset->GetCollect((LPCTSTR)shapeItemElement.arcID);
		shapeItem.arcID = atoi(_bstr_t(val));
		
		val = m_pRecordset->GetCollect((LPCTSTR)shapeItemElement.DCode);		
		shapeItem.DCode = _bstr_t(val);
		
		val = m_pRecordset->GetCollect((LPCTSTR)shapeItemElement.polygonID);	
		shapeItem.polygonID = atoi(_bstr_t(val));
		
		val = m_pRecordset->GetCollect((LPCTSTR)shapeItemElement.shapeID);
		
		shapeItem.shapeID = atoi(_bstr_t(val));
 		mapShapeItem.insert(make_pair(shapeItem.shapeID,shapeItem));
 		
		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取Polygon纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,Shape_PolygonElement shapePolygonElement,map<int,vector<Gxmc_Draw::PointD>>& mapShapePolygon)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	int nPolygonID = 0;
	Shape_Polygon shapePolygon;
	Gxmc_Draw::PointD point;
	vector<Gxmc_Draw::PointD> vectorPoint;
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)shapePolygonElement.pointX);
		shapePolygon.pointX = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)shapePolygonElement.pointY);
		shapePolygon.pointY = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)shapePolygonElement.polygonID);
		shapePolygon.polygonID = atoi(_bstr_t(val));
		point.x = shapePolygon.pointX;
		point.y = shapePolygon.pointY;
		if (shapePolygon.polygonID == nPolygonID)
		{
			vectorPoint.push_back(point);
		}
		else
		{
			mapShapePolygon.insert(make_pair(nPolygonID,vectorPoint));
			nPolygonID++;
			vector<Gxmc_Draw::PointD> vectorPointTmp;
			vectorPoint.swap(vectorPointTmp);
			//将vector插入map后，把当前shapearc压入清空后的vector
			vectorPoint.push_back(point);
		}

		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取焊盘表
void CGxmcDatabase::ReadRecordFromTable(CString tableName,PadElement padElement,map<int,Pad>& mapPad)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	Pad pad;
	PointD point;
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)padElement.FovNo);
		pad.FovNo = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)padElement.PadID);
		pad.PadID = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)padElement.ShapeID);
		pad.ShapeID = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)padElement.PosXmm);
		pad.PosXmm = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)padElement.PosYmm);
		pad.PosYmm = atof(_bstr_t(val));
		point.x = pad.PosXmm;
		point.y = pad.PosYmm;
		
		mapPad.insert(make_pair(pad.ShapeID,pad));

		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;


}
//读取AdCircle纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AdCircleElement adCircleElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	Gxmc_Gerber::AdStyle adStyle;
	adStyle.Initialize();
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)adCircleElement.DCode);
		string DCode = _bstr_t(val);
		val = m_pRecordset->GetCollect((LPCTSTR)adCircleElement.Diameter);
		adStyle.adInfo.adCircle.Diameter = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adCircleElement.HoleDiameter);
		adStyle.adInfo.adCircle.HoleDiameter = atof(_bstr_t(val));
		adStyle.adName = Gxmc_Gerber::Circle; 
		ADStyleMap.insert(make_pair(DCode,adStyle));
		
		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取AdRectangle纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AdRectangleElement adRectangleElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	Gxmc_Gerber::AdStyle adStyle;
	adStyle.Initialize();
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)adRectangleElement.DCode);
		string DCode = _bstr_t(val);
		val = m_pRecordset->GetCollect((LPCTSTR)adRectangleElement.Height);
		adStyle.adInfo.adRectangle.Height = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adRectangleElement.Width);
		adStyle.adInfo.adRectangle.Width = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adRectangleElement.HoleDiameter);
		adStyle.adInfo.adRectangle.HoleDiameter = atof(_bstr_t(val));
		adStyle.adName = Gxmc_Gerber::Rectangle;
		ADStyleMap.insert(make_pair(DCode,adStyle));
		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取AdObround纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AdObroundElement adObroundElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	Gxmc_Gerber::AdStyle adStyle;
	adStyle.Initialize();
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)adObroundElement.DCode);
		string DCode = _bstr_t(val);
		val = m_pRecordset->GetCollect((LPCTSTR)adObroundElement.Height);
		adStyle.adInfo.adObround.Height = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adObroundElement.Width);
		adStyle.adInfo.adObround.Width = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adObroundElement.HoleDiameter);
		adStyle.adInfo.adObround.HoleDiameter = atof(_bstr_t(val));
		adStyle.adName = Gxmc_Gerber::Obround;
		ADStyleMap.insert(make_pair(DCode,adStyle));
		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取AdRegularPolygon纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AdRegularPolygonElement adRegularPolygonElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	Gxmc_Gerber::AdStyle adStyle;
	adStyle.Initialize();
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)adRegularPolygonElement.DCode);
		string DCode = _bstr_t(val);
		adStyle.adName = Gxmc_Gerber::RegularPolygon;
		ADStyleMap.insert(make_pair(DCode,adStyle));
		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取AdAm纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AdAmElement adAmElement,std::map<string,Gxmc_Gerber::AdStyle>& ADStyleMap)
{
	std::map<int,vector<Gxmc_Gerber::AM_Circle>> mapAmCircle;
	ReadRecordFromTable(CGxmcDatabase::tableList.AmCircle,s_amCircleElement,mapAmCircle);
	std::map<int,vector<Gxmc_Gerber::AM_CenterLine>> mapAmCenterLine;
	ReadRecordFromTable(CGxmcDatabase::tableList.AmCenterLine,s_amCenterLineElement,mapAmCenterLine);
 	std::map<int,Pad> mapPad;
 	ReadRecordFromTable(CGxmcDatabase::tableList.Pad,s_padElement,mapPad);

	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	Gxmc_Gerber::AdStyle adStyle;
	adStyle.Initialize();
	AdAm adAm;
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		val = m_pRecordset->GetCollect((LPCTSTR)adAmElement.DCode);
		adAm.DCode = _bstr_t(val);
		val = m_pRecordset->GetCollect((LPCTSTR)adAmElement.CenterLineID);
		adAm.CenterLineID = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adAmElement.CircleID);
		adAm.CircleID = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adAmElement.MoireID);
		adAm.MoireID = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adAmElement.OutLineID);
		adAm.OutLineID = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adAmElement.PolygonID);
		adAm.PolygonID = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adAmElement.ThermalID);
		adAm.ThermalID = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)adAmElement.VectorLineID);
		adAm.VectorLineID = atoi(_bstr_t(val));

		Gxmc_Gerber::AD_AmInfo* adAmInfo = new Gxmc_Gerber::AD_AmInfo();
		if (adAm.CircleID != -1)
		{
			vector<Gxmc_Gerber::AM_Circle> vecAmCircle = mapAmCircle[adAm.CircleID];
			adAmInfo->vecAmCircle = vecAmCircle;
		}
		if (adAm.CenterLineID != -1)
		{
			vector<Gxmc_Gerber::AM_CenterLine> vecAmCenterLine = mapAmCenterLine[adAm.CenterLineID];
			adAmInfo->vecAmCenterLine = vecAmCenterLine;
		}
		if (adAm.MoireID != -1)
		{

		}
		if (adAm.OutLineID != -1)
		{

		}
		if (adAm.PolygonID != -1)
		{

		}
		if (adAm.ThermalID != -1)
		{

		}
		if (adAm.VectorLineID != -1)
		{

		}

		adStyle.adName = Gxmc_Gerber::Am;
		adStyle.adInfo.adAmInfo=adAmInfo;
		ADStyleMap.insert(make_pair(adAm.DCode,adStyle));
		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();

	////////////////////////////////////////////////////////////
	//释放mapAmCircle、mapAmCenterLine中PointD开的内存
	std::map<int,vector<Gxmc_Gerber::AM_Circle>>::iterator itorMapAmCircle = mapAmCircle.begin();
	for (;itorMapAmCircle != mapAmCircle.end();itorMapAmCircle++)
	{
		vector<Gxmc_Gerber::AM_Circle>::iterator itorAmCircle = itorMapAmCircle->second.begin();
		for (;itorAmCircle != itorMapAmCircle->second.end();itorAmCircle++)
		{
			delete itorAmCircle->centerPoint;
			itorAmCircle->centerPoint = NULL;
		}
	}
	std::map<int,vector<Gxmc_Gerber::AM_CenterLine>>::iterator itorMapAmCenterLine = mapAmCenterLine.begin();
	for (;itorMapAmCenterLine != mapAmCenterLine.end();itorMapAmCenterLine++)
	{
		vector<Gxmc_Gerber::AM_CenterLine>::iterator itorCenterLine = itorMapAmCenterLine->second.begin();
		for (;itorCenterLine != itorMapAmCenterLine->second.end();itorCenterLine++)
		{
			delete itorCenterLine->centerPoint;
			itorCenterLine->centerPoint = NULL;
		}
	}

	return;
}

//读取AmCircle纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AmCircleElement amCircleElement,std::map<int,vector<Gxmc_Gerber::AM_Circle>>& mapAmCircle)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	int nCircleID = 0;
	Gxmc_Gerber::AM_Circle amCircle;
	vector<Gxmc_Gerber::AM_Circle> vecAmCircle;
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		double centerX,centerY;
		val = m_pRecordset->GetCollect((LPCTSTR)amCircleElement.centerX);
		centerX = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCircleElement.centerY);
		centerY = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCircleElement.diameter);
		amCircle.diameter = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCircleElement.exposure);
		amCircle.exposure = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCircleElement.rotAngle);
		amCircle.rotationAngle = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCircleElement.CircleID);
		int CircleID = atoi(_bstr_t(val));
		amCircle.centerPoint = new PointD(centerX,centerY);
		
		if (CircleID == nCircleID)
		{
			vecAmCircle.push_back(amCircle);
		}
		else
		{
			mapAmCircle.insert(make_pair(nCircleID,vecAmCircle));
			nCircleID++;
			vector<Gxmc_Gerber::AM_Circle> vectorAmCircleTmp;
			vecAmCircle.swap(vectorAmCircleTmp);
			//将vector插入map后，把当前shapearc压入清空后的vector
			vecAmCircle.push_back(amCircle);
		}

		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取AmCenterLine纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AmCenterLineElement amCenterLineElement,std::map<int,vector<Gxmc_Gerber::AM_CenterLine>>& mapCenterLine)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	int nCenterLineID = 0;

	Gxmc_Gerber::AM_CenterLine amCenterLine;
	vector<Gxmc_Gerber::AM_CenterLine> vecAmCenterLine;
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		double centerX,centerY;
		val = m_pRecordset->GetCollect((LPCTSTR)amCenterLineElement.centerX);
		centerX = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCenterLineElement.centerY);
		centerY = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCenterLineElement.rectHeight);
		amCenterLine.rectangleHeight = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCenterLineElement.rectWidth);
		amCenterLine.rectangleWidth = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCenterLineElement.exposure);
		amCenterLine.exposure = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCenterLineElement.rotAngle);
		amCenterLine.rotationAngle = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amCenterLineElement.CenterLineID);
		int CenterLineID = atoi(_bstr_t(val));
		amCenterLine.centerPoint = new PointD(centerX,centerY);

		if (CenterLineID == nCenterLineID)
		{
			vecAmCenterLine.push_back(amCenterLine);
		}
		else
		{
			mapCenterLine.insert(make_pair(nCenterLineID,vecAmCenterLine));
			nCenterLineID++;
			vector<Gxmc_Gerber::AM_CenterLine> vectorAmCenterLineTmp;
			vecAmCenterLine.swap(vectorAmCenterLineTmp);
			//将vector插入map后，把当前shapearc压入清空后的vector
			vecAmCenterLine.push_back(amCenterLine);
		}

		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取AmVectorLine纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AmVectorLineElement amVectorLineElement,std::map<int,vector<Gxmc_Gerber::AM_VectorLine>>& mapVectorLine)
{
	OpenTable(tableName);
	//指向表中的第一行
	if (!m_pRecordset->adoBOF)
	{
		m_pRecordset->MoveFirst();
	} 
	else
	{
		//AfxMessageBox(L"表内数据为空");
		CloseTable();
		return;
	}

	int nVectorLineID = 0;

	Gxmc_Gerber::AM_VectorLine amVectorLine;
	vector<Gxmc_Gerber::AM_VectorLine> vecAmVectorLine;
	//读取数据表中各字段
	_variant_t val;
	while (!m_pRecordset->adoEOF)
	{
		double startX,startY,stopX,stopY;
		val = m_pRecordset->GetCollect((LPCTSTR)amVectorLineElement.startX);
		startX = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amVectorLineElement.startY);
		startY = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amVectorLineElement.stopX);
		stopX = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amVectorLineElement.stopY);
		stopY = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amVectorLineElement.lineWidth);
		amVectorLine.rotationAngle = atof(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amVectorLineElement.exposure);
		amVectorLine.exposure = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amVectorLineElement.rotAngle);
		amVectorLine.rotationAngle = atoi(_bstr_t(val));
		val = m_pRecordset->GetCollect((LPCTSTR)amVectorLineElement.VectorLineID);
		int VectorLineID = atoi(_bstr_t(val));

		amVectorLine.startPoint = new PointD(startX,startY);
		amVectorLine.stopPoint = new PointD(stopX,stopY);

		if (VectorLineID == nVectorLineID)
		{
			vecAmVectorLine.push_back(amVectorLine);
		}
		else
		{
			mapVectorLine.insert(make_pair(nVectorLineID,vecAmVectorLine));
			nVectorLineID++;
			vector<Gxmc_Gerber::AM_VectorLine> vecAmVectorLineTmp;
			vecAmVectorLine.swap(vecAmVectorLineTmp);
			//将vector插入map后，把当前shapearc压入清空后的vector
			vecAmVectorLine.push_back(amVectorLine);
		}

		//读取该行字段后，指针移到下一行
		m_pRecordset->MoveNext();
	}
	//指针回到默认行
	m_pRecordset->MoveFirst();
	CloseTable();
	return;
}
//读取AmOutLine纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AmOutLineElement amOutLineElement,std::map<int,vector<Gxmc_Gerber::AM_OutLine>>& mapOutLine)
{
	
}
//读取AmPolygon纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AmPolygonElement amPolygonElement,std::map<int,vector<Gxmc_Gerber::AM_Polygon>>& mapPolygon)
{
	
}
//读取AmMoire纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AmMoireElement amMoireElement,std::map<int,vector<Gxmc_Gerber::AM_Moire>>& mapMoire)
{
	
}
//读取AmThermal纪录
void CGxmcDatabase::ReadRecordFromTable(CString tableName,AmThermalElement amThermalElement,std::map<int,vector<Gxmc_Gerber::AM_Thermal>>& mapThermal)
{
	
}
#pragma endregion 
void CGxmcDatabase::Procedure(string procName)
{
	_ParameterPtr retParam =NULL;
	_ParameterPtr inputParam1 =NULL;
	_ParameterPtr inputParam2 =NULL;
	_ParameterPtr outputParam1 =NULL;
	_ParameterPtr outputParam2 =NULL;

	string str1;

	m_pCommand.CreateInstance(__uuidof(Command));
	m_pCommand->ActiveConnection = m_pConnection;//设置连接
	m_pCommand->CommandText = _bstr_t(procName.c_str());//存储过程名称

	m_pCommand->CommandType = adCmdStoredProc;//存储过程命令

	inputParam1 = m_pCommand->CreateParameter(_bstr_t(str1.c_str()),DataTypeEnum::adInteger,adParamInput,sizeof(int));
	m_pCommand->Parameters->Append(inputParam1);
	retParam = m_pCommand->CreateParameter(_bstr_t("return"),DataTypeEnum::adInteger,adParamReturnValue,sizeof(int));

	m_pRecordset.CreateInstance(__uuidof(Recordset));
	retParam.CreateInstance(__uuidof(Parameter));
}
void CGxmcDatabase::ReadProcedure()
{
	string procName = "";
	m_pCommand.CreateInstance(__uuidof(Command));
	m_pCommand->ActiveConnection = m_pConnection;//设置连接
	m_pCommand->CommandText = _bstr_t(procName.c_str());//存储过程名称

	m_pCommand->CommandType = adCmdStoredProc;//存储过程命令
}
#pragma region 对纪录的查询、添加、删除等操作

//根据主键查找某条纪录是否存在
int CGxmcDatabase::FindRecordByKey(CString tableName,CString KeyName,CString KeyValue)
{
	OpenTable(tableName);
	int indicator = 0;
	m_pRecordset->Move(0);
	while (!m_pRecordset->adoEOF)
	{
		_variant_t val;
		val = m_pRecordset->GetCollect((_variant_t)KeyName);
		if ( val.vt != VT_NULL && KeyValue == (LPCTSTR)(_bstr_t)val )
		{
			return indicator;
		}
		 
		m_pRecordset->MoveNext();
		indicator++;
		if (indicator > 67)
		{
			int b = 0;
		}
	}
	AfxMessageBox(L"对应键值的记录不存在！");
	return -1;
}

void CGxmcDatabase::AddNewRecordToTable(CString tableName,PadElement padElement,Pad pad)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();

		m_pRecordset->PutCollect((LPCTSTR)padElement.ID,_variant_t(pad.ID));
		m_pRecordset->PutCollect((LPCTSTR)padElement.PadID,_variant_t(pad.PadID));
		m_pRecordset->PutCollect((LPCTSTR)padElement.ShapeID,_variant_t(pad.ShapeID));
		m_pRecordset->PutCollect((LPCTSTR)padElement.PosXmm,_variant_t(pad.PosXmm));
		m_pRecordset->PutCollect((LPCTSTR)padElement.PosYmm,_variant_t(pad.PosYmm));
		m_pRecordset->PutCollect((LPCTSTR)padElement.FovNo,_variant_t(pad.FovNo));

		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}

void CGxmcDatabase::DeleteRecordFromTable(CString tableName,CString KeyName,CString KeyValue)
{
	try
	{
		OpenTable(tableName);
		int indicator = FindRecordByKey(tableName,KeyName,KeyValue);
		if (indicator < 0)
		{
			AfxMessageBox(L"对应键值的记录不存在！");
			return;
		}
		//指针必须移到纪录集的第一条
		m_pRecordset->MoveFirst();
		m_pRecordset->Move(indicator);
		//是指针移动到要删除的纪录所在行
		m_pRecordset->Delete(adAffectCurrent);//删除当前纪录
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
	
}
#pragma endregion

#pragma region 创建数据库文件、创建数据表

void CGxmcDatabase::CreateTable(CString DataBaseFileName,vector<TableStyle> vecTableStyle)
{
	HRESULT hresult;
	//数据库路径
// 	CString DataBasePath = L"../SPI_DataBase";
// 	//检查数据库保存路径是否存在
// 	CFileFind fileFind;
// 	if (!fileFind.FindFile(DataBasePath))
// 	{
// 		CreateDirectory((LPCTSTR)DataBasePath,NULL);
// 	}
//	//数据库文件全路径名
//	CString databaseFilename;
//	databaseFilename.Format(L"%s/%s.mdb",DataBasePath,DataBaseFileName);
	//创建数据库文件
	CString path;
	path.Format(L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s",DataBaseFileName);
	DWORD dwAttribute = GetFileAttributes((LPCTSTR)DataBaseFileName);
	if( !(INVALID_FILE_ATTRIBUTES != dwAttribute && 0 == (dwAttribute & FILE_ATTRIBUTE_DIRECTORY)) )
	{
		hresult = m_pCatalog.CreateInstance(__uuidof(ADOX::Catalog));
		if (SUCCEEDED(hresult))
		{
			m_pCatalog->Create(_bstr_t(path));
		}
		else
		{
			AfxMessageBox(L"数据库文件创建失败！");
		}
	}
	ConnectAdo(DataBaseFileName);
	//创建数据表
	for (unsigned int i = 0; i < vecTableStyle.size(); i++)
	{
		m_pTable.CreateInstance(__uuidof(ADOX::Table));
		//if (!IsTableExist(path,vecTableStyle[i].))
		{
			InitialTableHeaderBySQL(vecTableStyle[i]);
			m_pTable.Detach();
		}	
	}
	return;
}

void CGxmcDatabase::InitialTableHeaderBySQL(TableStyle tableStyle)
{
	try
	{
		CString  strCmd;
		_variant_t RecordsAffected;  
		switch (tableStyle)
		{
		case enumBareBoard:
			{
				strCmd.Format(L"create table %s(ID int Primary Key,\
							   PadID int,\
							   VenderID int,\
							   VenderName Text,\
							   Height int)",
							   tableList.BareBoard);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumFovPath:
			{
				strCmd.Format(L"create table %s(FovNo int,\
							   PosX double,\
							   PosY double,\
							   SizeX int,\
							   SizeY int,\
							   CaptureType int,\
							   BlockID int,\
							   LumiPara0 int,\
							   LumiPara1 int,\
							   LumiPara2 int,\
							   LumiPara3 int,\
							   LumiPara4 int,\
							   LumiPara5 int,\
							   LumiPara6 int,\
							   LumiPara7 int)",
							   tableList.FovPath);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumGeneral:
			{
				strCmd.Format(L"create table %s(\
							   CreatDateTime datetime,\
							   LastSaveTime datetime,\
							   BoardWidth single,\
							   BoardLength single,\
							   CreatBy Text,\
							   JobFileVersion Text,\
							   Rotation single,\
							   BoardOffsetX single,\
							   BoardOffsetY single,\
							   ID int,\
							   PCBType int,\
							   MeasureRange single,\
							   HeightType int)",
							   tableList.General);

				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumMark:
			{
				strCmd.Format(L"create table %s(ID int,\
							   Type int,\
							   Shape int,\
							   GlbOffsetXmm single,\
							   GlbOffsetYmm single,\
							   PosXmm single,\
							   PosYmm single,\
							   SearchSizeXmm single,\
							   SearchSizeYmm single,\
							   SizeXmm single,\
							   SizeYmm single,\
							   IsBright yesno,\
							   LumiType int,\
							   LumiPara0 int,\
							   LumiPara1 int,\
							   LumiPara2 int,\
							   LumiPara3 int,\
							   LumiPara4 int,\
							   LumiPara5 int,\
							   TemplateImage Text,\
							   ImgScale single,\
							   ImgWidth single,\
							   ImgHeight single,\
							   ImgPara0 int,\
							   ImgPara1 int,\
							   ImgPara2 int,\
							   ImgPara3 int,\
							   ScoreMin single,\
							   BlockID int)",
							   tableList.Mark);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);

			}
			break;
		case enumPad:
			{
				strCmd.Format(L"create table %s(ID int,\
							   PadID int,\
							   BadMarkID int,\
							   BoardID int,\
							   BlockID int,\
							   ComponentID int,\
							   PackageType int,\
							   SpecIndex int,\
							   PinNumber int,\
							   ShapeID int,\
							   ExtXum single,\
							   ExtYum single,\
							   FovNo int,\
							   PosXmm single,\
							   PosYmm single,\
							   Rotation int,\
							   BaseType int,\
							   Check yesno,\
							   CheckVol yesno,\
							   CheckArea yesno,\
							   CheckHeight yesno,\
							   CheckShape yesno,\
							   CheckBridge yesno,\
							   CheckOffset yesno,\
							   CheckCoplanarity yesno,\
							   HeightU single,\
							   HeightL single,\
							   AreaU single,\
							   AreaL single,\
							   VolU single,\
							   VolL single,\
							   ShiftXU single,\
							   ShiftYU single,\
							   ShapeDeltaU single,\
							   OffsetMark int,\
							   StencilHeight single,\
							   BridgeWidth single,\
							   BridgeLength single,\
							   BridgeHeight single,\
							   PadGroup int,\
							   UserAdjustMeasuredHeight int)",
							   tableList.Pad);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);

			}
			break;
		case enumShape:
			{
				strCmd.Format(L"create table %s(ShapeID int,\
							   ShapeStyle int,\
							   SizeXmm single,\
							   SizeYmm single,\
							   StencilAreaPer single,\
							   CShiftXmm single,\
							   CShiftYmm single,\
							   UseMask single,\
							   MaskPattern single,\
							   MaskScale int,\
							   PixLength int,\
							   PixWidth single)",
							   tableList.Shape);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumShape_Circle:
			{
				strCmd.Format(L"create table %s(CircleID int,\
							   CenterX double,\
							   CenterY double,\
							   Radius double)",					   
							   tableList.Shape_Circle);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumShape_Rectangle:
			{
				strCmd.Format(L"create table %s(RectID int,\
							   Left double,\
							   Top double,\
							   Right double,\
							   Bottom double)",					   
							   tableList.Shape_Rectangle);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumShape_Arc:
			{
				strCmd.Format(L"create table %s(nIndex int,\
							   ArcID int,\
							   ArcDirection int,\
							   StartPointX double,\
							   StartPointY double,\
							   StopPointX double,\
							   StopPointY double,\
							   CenterPointX double,\
							   CenterPointY double,\
							   Radius double)",					   
							   tableList.Shape_Arc);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumShape_Polygon:
			{
				strCmd.Format(L"create table %s(nIndex int,\
					            PolygonID int,\
							   PointX double,\
							   PointY double)",					   
							   tableList.Shape_Polygon);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumShapeItem:
			{
				strCmd.Format(L"create table %s(nIndex int,\
					            ShapeID int,\
							   PolygonID int,\
							   ArcID int,\
							   DCode varchar)",					   
							   tableList.ShapeItem);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAdRectangle:
			{
				strCmd.Format(L"create table %s(DCode varchar,\
							   Width double,\
							   Height double,\
							   HoleDiameter double)",					   
							   tableList.AdRectangle);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAdCircle:
			{
				strCmd.Format(L"create table %s(DCode varchar,\
							   Diameter double,\
							   HoleDiameter double)",					   
							   tableList.AdCircle);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAdObround:
			{
				strCmd.Format(L"create table %s(DCode varchar,\
							   Width double,\
							   Height double,\
							   HoleDiameter double)",						   
							   tableList.AdObround);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAdRegularPolygon:
			{
				strCmd.Format(L"create table %s(DCode varchar,\
							   Width double,\
							   Height double,\
							   HoleDiameter double)",					   
							   tableList.AdRegularPolygon);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAdAm:
			{
				strCmd.Format(L"create table %s(DCode varchar,\
							   CircleID int,\
							   VectorLineID int,\
							   CenterLineID int,\
							   OutLineID int,\
							   PolygonID int,\
							   MoireID int,\
							   ThermalID int)",					   
							   tableList.AdAm);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAmCircle:
			{
				strCmd.Format(L"create table %s(CircleID int,\
					            exposure int,\
							   diameter double,\
							   centerX double,\
							   centerY double,\
							   rotAngle double)",					   
							   tableList.AmCircle);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAmCenterLine:
			{
				strCmd.Format(L"create table %s(CenterLineID int,\
					            exposure int,\
							   rectWidth double,\
							   rectHeight double,\
							   centerX double,\
							   centerY double,\
							   rotAngle double)",					   
							   tableList.AmCenterLine);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAmVectorLine:
			{
				strCmd.Format(L"create table %s(VectorLineID int,\
					            exposure int,\
							   lineWidth double,\
							   startX double,\
							   startY double,\
							   stopX double,\
							   stopY double,\
							   rotAngle double)",					   
							   tableList.AmVectorLine);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAmOutLine:
			{
				strCmd.Format(L"create table %s(OutLineID int,\
					            exposure int,\
							   numVertex int,\
							   pointX double,\
							   pointY double,\
							   rotAngle double)",					   
							   tableList.AmOutLine);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAmPolygon:
			{
				strCmd.Format(L"create table %s( PolygonID int,\
					            exposure int,\
							   numVertex int,\
							   centerX double,\
							   centerY double,\
							   diameter double,\
							   rotAngle double)",					   
							   tableList.AmPolygon);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAmMoire:
			{
				strCmd.Format(L"create table %s(MoireID int,\
							   centerX double,\
							   centerY double,\
							   outerDiameter double,\
							   ringThickness double,\
							   ringGap double,\
							   ringMaxNum int,\
							   crosshairThickness double,\
							   crosshairLength double,\
							   rotAngle double)",					   
							   tableList.AmMoire);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		case enumAmThermal:
			{
				strCmd.Format(L"create table %s(ThermalID int,\
							   centerX double,\
							   centerY double,\
							   outerDiameter double,\
							   innerDiameter double,\
							   gapThickness double,\
							   rotAngle double)",					   
							   tableList.AmThermal);
				m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);
			}
			break;
		}
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}

}

void CGxmcDatabase::InitialTableHeader(TableStyle tableStyle)
{
	try
	{
		CString  strCmd;
		_variant_t RecordsAffected;  
		switch (tableStyle)
			{
			case enumBareBoard:
				{
					m_pTable->PutName((_bstr_t)tableList.BareBoard);
	
					m_pTable->Columns->Append((LPCTSTR)s_bareBoardElement.ID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_bareBoardElement.PadID,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_bareBoardElement.VenderID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_bareBoardElement.VenderName,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_bareBoardElement.Height,ADOX::adInteger,0);

					strCmd.Format(L"create table %s(ID INTEGER,PadID INTEGER,VenderID INTEGER,VenderName Text,Height Height)",tableList.BareBoard);
					m_pConnection->Execute((_bstr_t)strCmd,&RecordsAffected,adCmdText);


					m_pCatalog-> Tables-> Append(_variant_t((IDispatch*)m_pTable)); 

				}
				break;
			case enumFovPath:
				{
					m_pTable->PutName((_bstr_t)tableList.FovPath);
	
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.FovNo,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.PosX,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.PosY,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.SizeX,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.SizeY,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.CaptureType,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.BlockID,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.LumiPara0,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.LumiPara1,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.LumiPara2,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.LumiPara3,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.LumiPara4,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.LumiPara5,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.LumiPara6,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_fovPathElement.LumiPara7,ADOX::adInteger,0);
					m_pCatalog-> Tables-> Append(_variant_t((IDispatch*)m_pTable)); 
	
				}
				break;
			case enumGeneral:
				{
					m_pTable->PutName((_bstr_t)tableList.General);
	
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.CreatDateTime,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.LastSaveTime,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.BoardWidth,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.BoardLength,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.CreatBy,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.JobFileVersion,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.Rotation,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.BoardOffsetX,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.BoardOffsetY,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.ID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.PCBType,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.MeasureRange,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_generalElement.HeightType,ADOX::adInteger,0);
	
					m_pCatalog-> Tables-> Append(_variant_t((IDispatch*)m_pTable)); 
				}
	
				break;
			case enumMark:
				{
					m_pTable->PutName((_bstr_t)tableList.Mark);
	
					m_pTable->Columns->Append((LPCTSTR)s_markElement.ID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.Type,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.Shape,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.GlbOffsetXmm,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.GlbOffsetYmm,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.PosXmm,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.PosYmm,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.SearchSizeXmm,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.SearchSizeYmm,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.SizeXmm,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.SizeYmm,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.IsBright,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.GrayThresh,ADOX::adInteger,0);
	
					m_pTable->Columns->Append((LPCTSTR)s_markElement.LumiType,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.LumiPara0,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.LumiPara1,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.LumiPara2,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.LumiPara3,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.LumiPara4,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.LumiPara5,ADOX::adVarWChar,50);
//					m_pTable->Columns->Append((LPCTSTR)s_markElement.TemplateImage,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.ImgScale,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.ImgWidth,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.ImgHeight,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.ImgPara0,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.ImgPara1,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.ImgPara2,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.ImgPara3,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.ScoreMin,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_markElement.BlockID,ADOX::adInteger,0);
	
					m_pCatalog-> Tables-> Append(_variant_t((IDispatch*)m_pTable)); 
				}
	
				break;
			case enumPad:
				{
					m_pTable->PutName((_bstr_t)tableList.Pad);
	
					m_pTable->Columns->Append((LPCTSTR)s_padElement.ID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.PadID,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.BadMarkID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.BoardID,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.BlockID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.ComponentID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.PackageType,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.SpecIndex,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.PinNumber,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.ShapeID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.ExtXum,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.ExtYum,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.FovNo,ADOX::adInteger,0);
	
					m_pTable->Columns->Append((LPCTSTR)s_padElement.PosXmm,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.PosYmm,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.Rotation,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.BaseType,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.Check,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.CheckVol,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.CheckArea,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.CheckHeight,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.CheckShape,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.CheckBridge,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.CheckOffset,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.CheckCoplanarity,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.HeightU,ADOX::adInteger,0);
	
					m_pTable->Columns->Append((LPCTSTR)s_padElement.HeightL,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.AreaU,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.AreaL,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.VolU,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.VolL,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.ShiftXU,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.ShiftYU,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.ShapeDeltaU,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.OffsetMark,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.StencilHeight,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.BridgeWidth,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.BridgeLength,ADOX::adInteger,50);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.BridgeHeight,ADOX::adInteger,0);
	
					m_pTable->Columns->Append((LPCTSTR)s_padElement.PadGroup,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_padElement.UserAdjustMeasuredHeight,ADOX::adVarWChar,50);
	

					m_pCatalog-> Tables-> Append(_variant_t((IDispatch*)m_pTable)); 
				}
				break;
			case enumShape:
				{
					m_pTable->PutName((_bstr_t)tableList.Shape);
	
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.ShapeID,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.ShapeStyle,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.SizeXmm,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.SizeYmm,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.StencilAreaPer,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.CShiftXmm,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.CShiftYmm,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.UseMask,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.MaskPattern,ADOX::adVarWChar,50);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.MaskScale,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.PixLength,ADOX::adInteger,0);
					m_pTable->Columns->Append((LPCTSTR)s_shapeElement.PixWidth,ADOX::adVarWChar,50);
	
					m_pCatalog-> Tables-> Append(_variant_t((IDispatch*)m_pTable)); 
				}
	
				break;
			}
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
	
}
//添加Mark纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,MarkElement markElement,Mark mark)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();

		m_pRecordset->PutCollect((LPCTSTR)markElement.ID,_variant_t(mark.ID));
		m_pRecordset->PutCollect((LPCTSTR)markElement.BlockID,_variant_t(mark.BlockID));
		m_pRecordset->PutCollect((LPCTSTR)markElement.PosXmm,_variant_t(mark.PosXmm));
		m_pRecordset->PutCollect((LPCTSTR)markElement.PosYmm,_variant_t(mark.PosYmm));
		m_pRecordset->PutCollect((LPCTSTR)markElement.SizeXmm,_variant_t(mark.SizeXmm));
		m_pRecordset->PutCollect((LPCTSTR)markElement.SizeYmm,_variant_t(mark.SizeYmm));

		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加FovPath纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,FovPathElement fovPathElement,FovPath fovPath)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();

		m_pRecordset->PutCollect((LPCTSTR)fovPathElement.FovNo,_variant_t(fovPath.FovNo));
		//m_pRecordset->PutCollect((LPCTSTR)fovPathElement.BlockID,_variant_t(fovPath.BlockID));
		m_pRecordset->PutCollect((LPCTSTR)fovPathElement.PosX,_variant_t(fovPath.PosX));
		m_pRecordset->PutCollect((LPCTSTR)fovPathElement.PosY,_variant_t(fovPath.PosY));
		m_pRecordset->PutCollect((LPCTSTR)fovPathElement.SizeX,_variant_t(fovPath.SizeX));
		m_pRecordset->PutCollect((LPCTSTR)fovPathElement.SizeY,_variant_t(fovPath.SizeY));
		
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加FovPath纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,Shape_ArcElement shapeArcElement,Shape_Arc shapeArc)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.nIndex,_variant_t(shapeArc.nIndex));
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.arcID,_variant_t(shapeArc.arcID));
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.arcDirection,_variant_t(shapeArc.arcDirection));
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.startPointX,_variant_t(shapeArc.startPointX));
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.startPointY,_variant_t(shapeArc.startPointY));
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.stopPointX,_variant_t(shapeArc.stopPointX));
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.stopPointY,_variant_t(shapeArc.stopPointY));
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.centerPointX,_variant_t(shapeArc.centerPointX));
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.centerPointY,_variant_t(shapeArc.centerPointY));
		m_pRecordset->PutCollect((LPCTSTR)shapeArcElement.radius,_variant_t(shapeArc.radius));


		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加ArcPolygon纪录
void  CGxmcDatabase::AddNewRecordToTable(CString tableName,ShapeItemElement shapeItemElement,ShapeItem shapeItem)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)shapeItemElement.nIndex,_variant_t(shapeItem.nIndex));
		m_pRecordset->PutCollect((LPCTSTR)shapeItemElement.shapeID,_variant_t(shapeItem.shapeID));
		m_pRecordset->PutCollect((LPCTSTR)shapeItemElement.polygonID,_variant_t(shapeItem.polygonID));
		m_pRecordset->PutCollect((LPCTSTR)shapeItemElement.arcID,_variant_t(shapeItem.arcID));
		m_pRecordset->PutCollect((LPCTSTR)shapeItemElement.DCode,_variant_t(shapeItem.DCode.c_str()));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}

//添加Polygon纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,Shape_PolygonElement shapePolygonElement,Shape_Polygon shapePolygon)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)shapePolygonElement.nIndex,_variant_t(shapePolygon.nIndex));
		m_pRecordset->PutCollect((LPCTSTR)shapePolygonElement.polygonID,_variant_t(shapePolygon.polygonID));
		m_pRecordset->PutCollect((LPCTSTR)shapePolygonElement.pointX,_variant_t(shapePolygon.pointX));
		m_pRecordset->PutCollect((LPCTSTR)shapePolygonElement.pointY,_variant_t(shapePolygon.pointY));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}

//添加AdCircle纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AdCircleElement adCircleElement,AdCircle adCircle)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
	
		m_pRecordset->PutCollect((LPCTSTR)adCircleElement.DCode,_variant_t(adCircle.DCode.c_str()));
		m_pRecordset->PutCollect((LPCTSTR)adCircleElement.Diameter,_variant_t(adCircle.Diameter));
		m_pRecordset->PutCollect((LPCTSTR)adCircleElement.HoleDiameter,_variant_t(adCircle.HoleDiameter));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加AdRectangle纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AdRectangleElement adRectangleElement,AdRectangle adRectangle)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)adRectangleElement.DCode,_variant_t(adRectangle.DCode.c_str()));
		m_pRecordset->PutCollect((LPCTSTR)adRectangleElement.Width,_variant_t(adRectangle.Width));
		m_pRecordset->PutCollect((LPCTSTR)adRectangleElement.Height,_variant_t(adRectangle.Height));
		m_pRecordset->PutCollect((LPCTSTR)adRectangleElement.HoleDiameter,_variant_t(adRectangle.HoleDiameter));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加AdObround纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AdObroundElement adObroundElement,AdObround adObround)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)adObroundElement.DCode,_variant_t(adObround.DCode.c_str()));
		m_pRecordset->PutCollect((LPCTSTR)adObroundElement.Width,_variant_t(adObround.Width));
		m_pRecordset->PutCollect((LPCTSTR)adObroundElement.Height,_variant_t(adObround.Height));
		m_pRecordset->PutCollect((LPCTSTR)adObroundElement.HoleDiameter,_variant_t(adObround.HoleDiameter));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加AdRegularPolygon纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AdRegularPolygonElement adRegularPolygonElement,AdRegularPolygon adRegularPolygon)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)adRegularPolygonElement.DCode,_variant_t(adRegularPolygon.DCode.c_str()));
		/*m_pRecordset->PutCollect((LPCTSTR)adObroundElement.Width,_variant_t(adObround.Width));
		m_pRecordset->PutCollect((LPCTSTR)adObroundElement.Height,_variant_t(adObround.Height));
		m_pRecordset->PutCollect((LPCTSTR)adObroundElement.HoleDiameter,_variant_t(adObround.HoleDiameter));*/
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加AdAm纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AdAmElement adAmElement,AdAm adAm)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)adAmElement.DCode,_variant_t(adAm.DCode.c_str()));
 		m_pRecordset->PutCollect((LPCTSTR)adAmElement.CircleID,_variant_t(adAm.CircleID));
 		m_pRecordset->PutCollect((LPCTSTR)adAmElement.CenterLineID,_variant_t(adAm.CenterLineID));
 		m_pRecordset->PutCollect((LPCTSTR)adAmElement.VectorLineID,_variant_t(adAm.VectorLineID));
		m_pRecordset->PutCollect((LPCTSTR)adAmElement.OutLineID,_variant_t(adAm.OutLineID));
		m_pRecordset->PutCollect((LPCTSTR)adAmElement.PolygonID,_variant_t(adAm.PolygonID));
		m_pRecordset->PutCollect((LPCTSTR)adAmElement.MoireID,_variant_t(adAm.MoireID));
		m_pRecordset->PutCollect((LPCTSTR)adAmElement.ThermalID,_variant_t(adAm.ThermalID));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}

//添加AmCircle纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AmCircleElement amCircleElement,AmCircle amCircle)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)amCircleElement.CircleID,_variant_t(amCircle.CircleID));
		m_pRecordset->PutCollect((LPCTSTR)amCircleElement.exposure,_variant_t(amCircle.exposure));
		m_pRecordset->PutCollect((LPCTSTR)amCircleElement.centerX,_variant_t(amCircle.centerX));
		m_pRecordset->PutCollect((LPCTSTR)amCircleElement.centerY,_variant_t(amCircle.centerY));
		m_pRecordset->PutCollect((LPCTSTR)amCircleElement.diameter,_variant_t(amCircle.diameter));
		m_pRecordset->PutCollect((LPCTSTR)amCircleElement.rotAngle,_variant_t(amCircle.rotAngle));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加AmCenterLine纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AmCenterLineElement amCenterLineElement,AmCenterLine amCenterLine)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)amCenterLineElement.CenterLineID,_variant_t(amCenterLine.CenterLineID));
		m_pRecordset->PutCollect((LPCTSTR)amCenterLineElement.exposure,_variant_t(amCenterLine.exposure));
		m_pRecordset->PutCollect((LPCTSTR)amCenterLineElement.centerX,_variant_t(amCenterLine.centerX));
		m_pRecordset->PutCollect((LPCTSTR)amCenterLineElement.centerY,_variant_t(amCenterLine.centerY));
		m_pRecordset->PutCollect((LPCTSTR)amCenterLineElement.rectHeight,_variant_t(amCenterLine.rectHeight));
		m_pRecordset->PutCollect((LPCTSTR)amCenterLineElement.rectWidth,_variant_t(amCenterLine.rectWidth));
		m_pRecordset->PutCollect((LPCTSTR)amCenterLineElement.rotAngle,_variant_t(amCenterLine.rotAngle));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加AmVectorLine纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AmVectorLineElement amVectorLineElement,AmVectorLine amVectorLine)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)amVectorLineElement.VectorLineID,_variant_t(amVectorLine.VectorLineID));
		m_pRecordset->PutCollect((LPCTSTR)amVectorLineElement.exposure,_variant_t(amVectorLine.exposure));
		m_pRecordset->PutCollect((LPCTSTR)amVectorLineElement.lineWidth,_variant_t(amVectorLine.lineWidth));
		m_pRecordset->PutCollect((LPCTSTR)amVectorLineElement.startX,_variant_t(amVectorLine.startX));
		m_pRecordset->PutCollect((LPCTSTR)amVectorLineElement.startY,_variant_t(amVectorLine.startY));
		m_pRecordset->PutCollect((LPCTSTR)amVectorLineElement.stopX,_variant_t(amVectorLine.stopX));
		m_pRecordset->PutCollect((LPCTSTR)amVectorLineElement.stopY,_variant_t(amVectorLine.stopY));
		m_pRecordset->PutCollect((LPCTSTR)amVectorLineElement.rotAngle,_variant_t(amVectorLine.rotAngle));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加AmOutLine纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AmOutLineElement amOutLineElement,AmOutLine amOutLine)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)amOutLineElement.OutLineID,_variant_t(amOutLine.OutLineID));
		m_pRecordset->PutCollect((LPCTSTR)amOutLineElement.exposure,_variant_t(amOutLine.exposure));
		m_pRecordset->PutCollect((LPCTSTR)amOutLineElement.numVertex,_variant_t(amOutLine.numVertex));
		m_pRecordset->PutCollect((LPCTSTR)amOutLineElement.pointX,_variant_t(amOutLine.pointX));
		m_pRecordset->PutCollect((LPCTSTR)amOutLineElement.pointY,_variant_t(amOutLine.pointY));
		m_pRecordset->PutCollect((LPCTSTR)amOutLineElement.rotAngle,_variant_t(amOutLine.rotAngle));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加AmPolygon纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AmPolygonElement amPolygonElement,AmPolygon amPolygon)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)amPolygonElement.PolygonID,_variant_t(amPolygon.PolygonID));
		m_pRecordset->PutCollect((LPCTSTR)amPolygonElement.exposure,_variant_t(amPolygon.exposure));
		m_pRecordset->PutCollect((LPCTSTR)amPolygonElement.numVertex,_variant_t(amPolygon.numVertex));
		m_pRecordset->PutCollect((LPCTSTR)amPolygonElement.centerX,_variant_t(amPolygon.centerX));
		m_pRecordset->PutCollect((LPCTSTR)amPolygonElement.centerY,_variant_t(amPolygon.centerY));
		m_pRecordset->PutCollect((LPCTSTR)amPolygonElement.diameter,_variant_t(amPolygon.diameter));
		m_pRecordset->PutCollect((LPCTSTR)amPolygonElement.rotAngle,_variant_t(amPolygon.rotAngle));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
//添加AmMoire纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AmMoireElement amMoireElement,AmMoire amMoire)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.MoireID,_variant_t(amMoire.MoireID));
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.crosshairLength,_variant_t(amMoire.crosshairLength));
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.crosshairThickness,_variant_t(amMoire.crosshairThickness));
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.centerX,_variant_t(amMoire.centerX));
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.centerY,_variant_t(amMoire.centerY));
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.outerDiameter,_variant_t(amMoire.outerDiameter));
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.ringGap,_variant_t(amMoire.ringGap));
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.ringMaxNum,_variant_t(amMoire.ringMaxNum));
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.ringThickness,_variant_t(amMoire.ringThickness));
		m_pRecordset->PutCollect((LPCTSTR)amMoireElement.rotAngle,_variant_t(amMoire.rotAngle));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
	
}
//添加AmThermal纪录
void CGxmcDatabase::AddNewRecordToTable(CString tableName,AmThermalElement amThermalElement,AmThermal amThermal)
{
	try
	{
		OpenTable(tableName);
		if (!m_pRecordset->adoBOF)
		{
			m_pRecordset->MoveFirst();//添加一条纪录会在第一条纪录处添加
		} 
		m_pRecordset->AddNew();
		m_pRecordset->PutCollect((LPCTSTR)amThermalElement.ThermalID,_variant_t(amThermal.ThermalID));
		m_pRecordset->PutCollect((LPCTSTR)amThermalElement.centerX,_variant_t(amThermal.centerX));
		m_pRecordset->PutCollect((LPCTSTR)amThermalElement.centerY,_variant_t(amThermal.centerY));
		m_pRecordset->PutCollect((LPCTSTR)amThermalElement.gapThickness,_variant_t(amThermal.gapThickness));
		m_pRecordset->PutCollect((LPCTSTR)amThermalElement.innerDiameter,_variant_t(amThermal.innerDiameter));
		m_pRecordset->PutCollect((LPCTSTR)amThermalElement.outerDiameter,_variant_t(amThermal.outerDiameter));
		m_pRecordset->PutCollect((LPCTSTR)amThermalElement.rotAngle,_variant_t(amThermal.rotAngle));
		m_pRecordset->MoveNext();
		m_pRecordset->Update();

		CloseTable();
	}
	catch (_com_error e)
	{
		LPCTSTR str = (LPCTSTR)e.Description();
		AfxMessageBox(str);
		return;
	}
}
#pragma endregion 

#pragma region 初始化每个表头各字段
CGxmcDatabase::PadElement CGxmcDatabase::s_padElement = 
{
	L"ID",
	L"PadID",
	L"BadMarkID",
	L"BoardID",
	L"BlockID",
	L"ComponentID",
	L"PackageType",
	L"SpecIndex",
	L"PinNumber",
	L"ShapeID",
	L"ExtXum",
	L"ExtYum",
	L"FovNo",
	L"PosXmm",
	L"PosYmm",
	L"Rotation",
	L"BaseType",
	L"Check",
	L"CheckVol",
	L"CheckArea",
	L"CheckHeight",
	L"CheckShape",
	L"CheckBridge",
	L"CheckOffset",
	L"CheckCoplanarity",
	L"HeightU",
	L"HeightL",
	L"AreaU",
	L"AreaL",
	L"VolU",
	L"VolL",
	L"ShiftXU",
	L"ShiftYU",
	L"ShapeDeltaU",
	L"OffsetMark",
	L"StencilHeight",
	L"BridgeWidth",
	L"BridgeLength",
	L"BridgeHeight",
	L"PadGroup",
	L"UserAdjustMeasuredHeight"
};

CGxmcDatabase::BareBoardElement CGxmcDatabase::s_bareBoardElement = 
{
	L"ID",
	L"PadID",
	L"VenderName",
	L"VenderID",
	L"Height"
};

CGxmcDatabase::FovPathElement CGxmcDatabase::s_fovPathElement = 
{
	L"FovNo",
	L"PosX",
	L"PosY",
	L"SizeX",
	L"SizeY",
	L"CaptureType",
	L"BlockID",
	L"LumiPara0",
	L"LumiPara1",
	L"LumiPara2",
	L"LumiPara3",
	L"LumiPara4",
	L"LumiPara5",
	L"LumiPara6",
	L"LumiPara7"
};

CGxmcDatabase::GeneralElement CGxmcDatabase::s_generalElement = 
{
	L"CreatDateTime",
	L"LastSaveTime",
	L"BoardWidth",
	L"BoardLength",
	L"CreatBy",
	L"JobFileVersion",
	L"Rotation",
	L"BoardOffsetX",
	L"BoardOffsetY",
	L"ID",
	L"PCBType",
	L"MeasureRange",
	L"HeightType"
};

CGxmcDatabase::MarkElement CGxmcDatabase::s_markElement = 
{
	L"ID",
	L"Type",
	L"Shape",
	L"GlbOffsetXmm",
	L"GlbOffsetYmm",
	L"PosXmm",
	L"PosYmm",
	L"SearchSizeXmm",
	L"SearchSizeYmm",
	L"SizeXmm",
	L"SizeYmm",
	L"MeasureRange",
	L"IsBright",

	L"GrayThresh",
	L"LumiType",
	L"LumiPara0",
	L"LumiPara1",
	L"LumiPara2",
	L"LumiPara3",
	L"LumiPara4",
	L"LumiPara5",
//	L"TemplateImage",
	L"ImgScale",
	L"ImgWidth",
	L"ImgHeight",
	L"ImgPara0",
	L"ImgPara1",
	L"ImgPara2",
	L"ImgPara3",
	L"ImgPara4",
	L"ScoreMin",
	L"BlockID"
};

CGxmcDatabase::ShapeElement CGxmcDatabase::s_shapeElement = 
{
	L"ShapeID",
	L"ShapeStyle",
	L"SizeXmm",
	L"SizeYmm",
	L"StencilAreaPer",
	L"CShiftXmm",
	L"CShiftYmm",
	L"UseMask",
	L"MaskPattern",
	L"MaskScale",
	L"PixLength",
	L"PixWidth"
};

CGxmcDatabase::Shape_ArcElement CGxmcDatabase::s_arcElement = 
{
	L"nIndex",
	L"ArcID",
	L"ArcDirection",
	L"StartPointX",
	L"StartPointY",
	L"StopPointX",
	L"StopPointY",
	L"CenterPointX",
	L"CenterPointY",
	L"Radius"
};
CGxmcDatabase::Shape_PolygonElement CGxmcDatabase::s_polygonElement = 
{
	L"nIndex",
	L"PolygonID",
	L"PointX",
	L"PointY"
};

CGxmcDatabase::ShapeItemElement CGxmcDatabase::s_shapeItemElement = 
{
	L"nIndex",
	L"ShapeID",
	L"PolygonID",
	L"ArcID",
	L"DCode"
};

CGxmcDatabase::AdCircleElement CGxmcDatabase::s_adCircleElement = 
{
	L"DCode",
	L"Diameter",
	L"HoleDiameter"
};
CGxmcDatabase::AdRectangleElement CGxmcDatabase::s_adRectangleElement = 
{
	L"DCode",
	L"Width",
	L"Height",
	L"HoleDiameter"
};
CGxmcDatabase::AdObroundElement CGxmcDatabase::s_adObroundElement = 
{
	L"DCode",
	L"Width",
	L"Height",
	L"HoleDiameter"
};
CGxmcDatabase::AdRegularPolygonElement CGxmcDatabase::s_adRegularPolygonElement = 
{
	L"DCode"
};
CGxmcDatabase::AdAmElement CGxmcDatabase::s_adAmElement = 
{
	L"DCode",
	L"CircleID",
	L"VectorLineID",
	L"CenterLineID",
	L"OutLineID",
	L"PolygonID",
	L"MoireID",
	L"ThermalID"
};

CGxmcDatabase::AmCircleElement CGxmcDatabase::s_amCircleElement = 
{
	L"CircleID",
	L"exposure",
	L"diameter",
	L"centerX",
	L"centerY",
	L"rotAngle"
};
CGxmcDatabase::AmCenterLineElement CGxmcDatabase::s_amCenterLineElement = 
{
	L"CenterLineID",
	L"exposure",
	L"rectWidth",
	L"rectHeight",
	L"centerX",
	L"centerY",
	L"rotAngle"

};
CGxmcDatabase::AmVectorLineElement CGxmcDatabase::s_amVectorLineElement = 
{
	L"VectorLineID",
	L"exposure",
	L"lineWidth",
	L"startX",
	L"startY",
	L"stopX",
	L"stopY",
	L"rotAngle"
};
CGxmcDatabase::AmOutLineElement CGxmcDatabase::s_amOutLineElement = 
{
	L"OutLineID",
	L"exposure",
	L"numVertex",
	L"pointX",
	L"pointY",
	L"rotAngle"
};
CGxmcDatabase::AmPolygonElement CGxmcDatabase::s_amPolygonElement = 
{
	L"PolygonID",
	L"exposure",
	L"numVertex",
	L"centerX",
	L"centerY",
	L"diameter",
	L"rotAngle"
};
CGxmcDatabase::AmMoireElement CGxmcDatabase::s_amMoireElement = 
{
	L"MoireID",
	L"centerX",
	L"centerY",
	L"outerDiameter",
	L"ringThickness",
	L"ringGap",
	L"ringMaxNum",
	L"crosshairThickness",
	L"crosshairLength",
	L"rotAngle"
};
CGxmcDatabase::AmThermalElement CGxmcDatabase::s_amThermalElement = 
{
	L"ThermalID",
	L"centerX",
	L"centerY",
	L"outerDiameter",
	L"innerDiameter",
	L"gapThickness",
	L"rotAngle"
};

////初始化表名列表
CGxmcDatabase::TableList CGxmcDatabase::tableList =
{
	L"BareBoard",
	L"FovPath",
	L"GeneralInfo",
	L"Mark",
	L"Pad",
	L"Shape",
	L"ShapeItem",
	L"Shape_Arc",
	L"Shape_Polygon",
	L"Shape_Circle",
	L"Shape_Rectangle",
	L"AD_Rectangle",
	L"AD_Circle",
	L"AD_Obround",
	L"AD_RegularPolygon",
	L"AD_Am",
	L"AmCircle",
	L"AmVectorLine",
	L"AmCenterLine",
	L"AmOutLine",
	L"AmPolygon",
	L"AmMoire",
	L"AmThermal"

};

#pragma endregion