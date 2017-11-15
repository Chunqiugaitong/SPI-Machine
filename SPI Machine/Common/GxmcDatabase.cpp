#pragma once
#include "StdAfx.h"
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
	AfxMessageBox(L"对应键值的记录不存在！");
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
	AfxMessageBox(L"对应键值的记录不存在！");
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
		AfxMessageBox(L"表内数据为空");
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
#pragma endregion 

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
		m_pRecordset->PutCollect((LPCTSTR)padElement.PosXmm,_variant_t(pad.PosXmm));
		m_pRecordset->PutCollect((LPCTSTR)padElement.PosYmm,_variant_t(pad.PosYmm));

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

void CGxmcDatabase::CreateTable(CString DataBaseFileName,CString tableName,vector<TableStyle> vecTableStyle)
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
	for (int i = 0; i < vecTableStyle.size(); i++)
	{
		m_pTable.CreateInstance(__uuidof(ADOX::Table));
		//if (!IsTableExist(path,tableList.Pad))
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
		m_pRecordset->PutCollect((LPCTSTR)markElement.PosXmm,_variant_t(mark.PosXmm));
		m_pRecordset->PutCollect((LPCTSTR)markElement.PosYmm,_variant_t(mark.PosYmm));

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


////
CGxmcDatabase::TableList CGxmcDatabase::tableList =
{
	L"BareBoard",
	L"FovPath",
	L"GeneralInfo",
	L"Mark",
	L"Pad",
	L"Shape"
};

#pragma endregion