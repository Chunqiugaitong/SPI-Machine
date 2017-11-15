#include "stdafx.h"
#include "GxmcGerber.h"
#include <iostream>
#include <sstream>
#include <string> 

#include <fstream>
#include <regex>

namespace Gxmc_Gerber
{
	CGxmcGerber::CGxmcGerber(void)
	{
 		m_RegexPattern.prefix_ADD = "^[%AD]{3}([D0-9]{2,})([CROP\\,]{2})([-0-9.]{1,}){1,}X?([-0-9.]{1,}){0,}";
 		m_RegexPattern.prefix_AM = "^%AM";
 		m_RegexPattern.prefix_AMDescription = "([123,]{2,})([0-9,]{2})([-0-9.,]{1,})([-0-9.,]{1,})([-0-9.]{1,})";
 		//L前导省零，常用；T末尾省零，即将废弃，不推荐使用
 		//A绝对坐标值，I增量坐标值（相对值）
 		m_RegexPattern.prefix_FS = "%FS(LA|TA|LI|TI)X(\\d)(\\d)Y(\\d)(\\d)";
 		//英寸、毫米
 		m_RegexPattern.prefix_MO = "(%MO)(MM|IN)";
 		m_RegexPattern.prefix_LP = "(%LP)(D|C)";

		m_penWidth   = 1;
		m_penStyle   = PS_SOLID;
		m_penColor   = RGB(0,255,0);
		m_brushColor = RGB(0,255,0);
		m_nLayer     = 0;//图层索引
		m_bIsFilled  = true;

	}
	CGxmcGerber::~CGxmcGerber(void)
	{

	}

	/*************************************************Gerber文件读取 开头*************************************************************************************/
	void CGxmcGerber::UpdateMargin(PointD point,RectD& realMarginRect)
	{
		realMarginRect.left = point.x < realMarginRect.left ? point.x : realMarginRect.left;
		realMarginRect.right = point.x > realMarginRect.right ? point.x : realMarginRect.right;
		realMarginRect.top = point.y > realMarginRect.top ? point.y : realMarginRect.top;
		realMarginRect.bottom = point.y < realMarginRect.bottom ? point.y : realMarginRect.bottom;
	}
	void  CGxmcGerber::UpdateMargin(RectD rect,RectD& realMarginRect)
	{
		realMarginRect.left = rect.left < realMarginRect.left ? rect.left : realMarginRect.left;
		realMarginRect.right = rect.right > realMarginRect.right ? rect.right : realMarginRect.right;
		realMarginRect.top = rect.top > realMarginRect.top ? rect.top : realMarginRect.top;
		realMarginRect.bottom = rect.bottom < realMarginRect.bottom ? rect.bottom : realMarginRect.bottom;
	}
	void CGxmcGerber::AddAperture(std::map<std::string,AdStyle> ADStyleMap,std::string DCode,PointD point,DataBlock dataBlock,std::list<DataBlock>& dataBlockList)
	{
		//当map中键值不存在时，会自动插入一个
		switch (ADStyleMap[DCode].adName)
		{
		case Circle:
			{
				ACircle*  pShape = new ACircle(m_penWidth, m_penStyle, m_penColor,m_brushColor,
					              m_nLayer,m_bIsFilled,point,ADStyleMap[DCode].adInfo.adCircle,DCode);
				dataBlock.push_back(pShape);
				dataBlockList.push_back(dataBlock);
				pShape = NULL;
			}
			break;
		case Rectangle:
			{
				ARectangle*  pShape = new ARectangle(m_penWidth, m_penStyle, m_penColor,m_brushColor, 
					m_nLayer,m_bIsFilled,point,ADStyleMap[DCode].adInfo.adRectangle,DCode);
				dataBlock.push_back(pShape);
				dataBlockList.push_back(dataBlock);
				pShape = NULL;
			}							
			break;
		case RegularPolygon:
			{
				ARegularPolygon*  pShape = new ARegularPolygon(m_penWidth, m_penStyle, m_penColor,m_brushColor, 
					m_nLayer,m_bIsFilled,point,ADStyleMap[DCode].adInfo.adRegularPolygon,DCode);
				dataBlock.push_back(pShape);
				dataBlockList.push_back(dataBlock);
				pShape = NULL;
			}
			break;
		case Obround:
			{
				AObround*  pShape = new AObround(m_penWidth, m_penStyle, m_penColor,m_brushColor, 
					m_nLayer,m_bIsFilled,point,ADStyleMap[DCode].adInfo.adObround,DCode);		
				dataBlock.push_back(pShape);
				dataBlockList.push_back(dataBlock);
				pShape = NULL;
			}
			break;
		case Am:
			{
				AMShape* pShape = new AMShape(m_penWidth, m_penStyle, m_penColor,m_brushColor, 
					m_nLayer,m_bIsFilled,point,*(ADStyleMap[DCode].adInfo.adAmInfo),DCode);
				dataBlock.push_back(pShape);
				dataBlockList.push_back(dataBlock);
				pShape = NULL;
			}
			break;
		}
	}
	void CGxmcGerber::ReadGerberFile(CString fileFullName,std::list<DataBlock>& dataBlockList,RectD& realMarginRect)
	{
		char strBuf[BUFFERLENGTH];
		std::ifstream infile(fileFullName);
		//读取gerber提示信息
		ReadTipInfo(infile,strBuf,m_TipInfo,m_adStyleMap);
		std::vector<std::string> vecMatch;
		std::string srcExpression(strBuf);	
		//定义图元对象存储数据块中的对象	
		DataBlock dataBlock;	
		PointD point,CurPoint;
		//判断是否要读取下一行
		bool isReadLine = true;
		//当前使用的光圈代码DXXX
		std::string DCode;
		//判断圆弧多边形是否填充
	    bool isFilled = true;
		//当前插值模式，G01，G02，G03
		//std::string GInterpolateCode;
		//读取数据块

		double dPreviousX = 0;
		double dPreviousY = 0;
		
		while(true)
		{
			ReadLine(infile,isReadLine,srcExpression);
			std::list<GxmcShape*> listTemp;
			dataBlock.swap(listTemp);
			if (srcExpression == "M02*")//gerber文件结束标志
			{
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			///                            封闭多边形区域
			//////////////////////////////////////////////////////////////////////////
			int nG36 = srcExpression.find("G36");
			if(nG36>=0)
			{
				GxmcPolygon polygon;
				while(true)
				{		
					ReadLine(infile,isReadLine,srcExpression);
					//读取识别功能码 GXX
                    #pragma region 读取线性插值模式标志G01
					//********读取G01、D02等信息
					if (srcExpression.length() > 4)//G01带XY坐标
					{
						ReadMoveOpreationD02(srcExpression,dPreviousX,dPreviousY,point,CurPoint,polygon,realMarginRect);
					}				
					//读取D01
					while(true)
					{				
						ReadLine(infile,isReadLine,srcExpression);
						bool isD01Read = false;
						bool isFilled = true;
						bool isIncludeLine = false;
						ReadLinearIterpolateOperationD01(srcExpression,dPreviousX,dPreviousY,point,CurPoint,polygon,realMarginRect,isD01Read,isFilled,isIncludeLine);
						if (!isD01Read)
						{
							break;
						}	
					}	
					
					int nG37 = srcExpression.find("G37");//读取封闭区域结束标志
					if(nG37>=0)
					{	
						GxmcPolygon* pShape= new GxmcPolygon(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,m_bIsFilled);
						for (unsigned int i = 0; i < polygon.m_vecArc.size(); i++) //扇形区域
						{
							pShape->m_vecArc.push_back(polygon.m_vecArc[i]);
						}	
						for (unsigned int i = 0; i < polygon.m_vecPoint.size(); i++)//多边形区域
						{
							pShape->m_vecPoint.push_back(polygon.m_vecPoint[i]);
						}
						dataBlock.push_back(pShape);
						dataBlockList.push_back(dataBlock);
						pShape = NULL;
						break;
					}
					
                    #pragma endregion

                    #pragma region 四分之三圆弧插补功能码G75
					int nG75 = srcExpression.find("G75");
					if(nG75>=0)
					{	
						m_QuadrantMode = MultiplyQuadrant;
						while(true)
						{				
							ReadLine(infile,isReadLine,srcExpression);
							bool isD01Read = false;
							isFilled = true;
							ReadCircularIterpolateOperationD01(srcExpression,dPreviousX,dPreviousY,point,CurPoint,polygon,realMarginRect,isD01Read,isFilled);		
							if (!isD01Read)
							{
								break;
							}
						}
					}
                    #pragma endregion

                    #pragma region 四分之一圆弧插补功能码G74
					int nG74 = srcExpression.find("G74");
					if(nG74>=0)
					{	
					 	m_QuadrantMode = SingleQuadrant; 
					 	while(infile.getline(strBuf,BUFFERLENGTH))
					 	{				
					 		srcExpression = strBuf;
							int nG01 = srcExpression.find("G01");
							if(nG01>=0)
					 		{
					            //GInterpolateCode = "G01";
					 			//continue;
					 		}
					 		break;
					 	}
						isReadLine = false;
						continue;
					}
                    #pragma endregion

				}
			} 
             #pragma region 自定义光圈孔径
 			//////////////////////////////////////////////////////////////////////////
 			///                            非封闭多边形线
 			//////////////////////////////////////////////////////////////////////////
 			int nG54 = srcExpression.find("G54");
 			if(nG54>=0)
 			{
 				int nStar = srcExpression.find("*");
 				DCode = srcExpression.substr(3,nStar-3);
 				if (DCode == "D72")
 				{
 					int vbn = 9;
 				}
 				std::vector<PointD> vecPoint;
 				GxmcPolygon polygon;
 				while (true)
                 {   
 					ReadLine(infile,isReadLine,srcExpression);
 					int nFlag = 0;
 					int nD03 = srcExpression.find("D03");
 					int nD02 = srcExpression.find("D02");
 					int nD01 = srcExpression.find("D01");
 					if (nD03 > 0)
 					{
 						nFlag = 3;
 					}
 					if (nD02 > 0)
 					{
 						nFlag = 2;
 					}
 					if (nD01 > 0)
 					{
 						nFlag = 1;
 					}
 
 					#pragma region D01、D02、D03
                     switch (nFlag)
 					{
 					case 0:
 						break;
 					case 1:	
 						if (nFlag == 1)
 						{
 							nFlag = 0;
 							isReadLine = false;
 							while (true)
 							{
 								ReadLine(infile,isReadLine,srcExpression);
 								bool isD01Read = false;
 								bool isFilled = false;
 								bool isIncludeLine = false;
 								ReadLinearIterpolateOperationD01(srcExpression,dPreviousX,dPreviousY,point,CurPoint,polygon,realMarginRect,isD01Read,isFilled,isIncludeLine);
 								if (!isD01Read)
 								{
 									break;
 								}	
  									
  								point.isMoveto = false;
  								vecPoint.push_back(point);
 								continue;
 							}
 							if (vecPoint.size() > 0)
 							{
 								//m_penColor = RGB(0,255,0);
 								bool IsFilled = false;
 								GxmcPLine* pShape= new GxmcPLine(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,IsFilled,vecPoint);
 								//m_penColor = RGB(255,255,0);
 								dataBlock.push_back(pShape);
 								dataBlockList.push_back(dataBlock);
 								//vecPoint清空
 								std::vector<PointD> vecPointTmp;
 								vecPoint.swap(vecPointTmp);
 								//DataBlock清空
 								DataBlock tmpDataBlock;
 								dataBlock.swap(tmpDataBlock);
 							}
 							nD02 = srcExpression.find("D02");
 							if ( nD02 > 0)
 							{
 								nFlag = 2;
 								isReadLine = false;
 							}						
 						}	  
 						break;
 					case 2:							
 						if (nFlag == 2)
 						{
 							nFlag = 0;
 	                        int nX,nY;
 							nX = srcExpression.find("X");
 							nY = srcExpression.find("Y");
 							std::string strX = srcExpression.substr(nX+1,nY-1);
 							std::string strY = srcExpression.substr(nY+1,nD02-nY-1);
 							GetValueFromStr(strX,strY,dPreviousX,dPreviousY,point.x,point.y);
 							UpdateMargin(point,realMarginRect);
 							//将自定义的宏光圈图元添加到数据块列表
 							point.isMoveto = true;
 							vecPoint.push_back(point);	
 							point.isMoveto = false;
 						}						
 						break;
 					case 3:
 						while(true)
 						{
 							int nX,nY;
 							if (nFlag == 3)
 							{
 								nFlag = 0;
 							}else
 							{
 								infile.getline(strBuf,BUFFERLENGTH);
 								srcExpression = strBuf;
 							}
 							nD03 = srcExpression.find("D03");
 							if (nD03 > 0)
 							{
 								nX = srcExpression.find("X");
 								nY = srcExpression.find("Y");
 								std::string strX = srcExpression.substr(nX+1,nY-1);
 								std::string strY = srcExpression.substr(nY+1,nD03-nY-1);
 								GetValueFromStr(strX,strY,dPreviousX,dPreviousY,point.x,point.y);
 								UpdateMargin(point,realMarginRect);
 								//将自定义的宏光圈图元添加到数据块列表
 								AddAperture(m_adStyleMap,DCode,point,dataBlock,dataBlockList);
 								//DataBlock清空
 								DataBlock tmpDataBlock;
 								dataBlock.swap(tmpDataBlock);
 								continue;
 							}
 							else
 							{
 								break;
 							}		
 						}
 						break;
 					}
                     #pragma endregion D01、D02、D03
 					
 	                #pragma region 四分之三圆弧插补功能码G75
 					int nG75 = srcExpression.find("G75");
 					if(nG75>=0)
 					{	
 						m_QuadrantMode = MultiplyQuadrant;
 						while(true)
 						{				
 							ReadLine(infile,isReadLine,srcExpression);
 							bool isD01Read = false;
 							isFilled = false;
 							bool isIncludeLine = false;
 							ReadLinearIterpolateOperationD01(srcExpression,dPreviousX,dPreviousY,point,CurPoint,polygon,realMarginRect,isD01Read,isFilled,isIncludeLine);
 							if (!isD01Read)
 							{
 								break;
 							}
 						}
 					}
 	                #pragma endregion
 	
 	                #pragma region 四分之一圆弧插补功能码G74
 					int nG74 = srcExpression.find("G74");
 					if(nG74>=0)
 					{	
 						m_QuadrantMode = SingleQuadrant; 
 						while(infile.getline(strBuf,BUFFERLENGTH))
 						{				
 							srcExpression = strBuf;
 							int nG01 = srcExpression.find("G01");
 							if(nG01>=0)
 							{
 							//////////////////////////////////////////////////////////////////////////
 							//////////////////////////////////////////////////////////////////////////
 								int nG75 = srcExpression.find("G75");
 								if(nG75>=0)
 								{
 									break;
 								}
 								ReadLine(infile,isReadLine,srcExpression);
 								bool isD01Read = false;
 								bool isFilled = false;
 								bool isIncludeLine = true;
 								ReadLinearIterpolateOperationD01(srcExpression,dPreviousX,dPreviousY,point,CurPoint,polygon,realMarginRect,isD01Read,isFilled,isIncludeLine);
 								if (!isD01Read)
 								{
 									break;
 								}	
 
 // 								point.isMoveto = false;
 // 								polygon.m_vecPoint.push_back(point);
 								continue;
 							//////////////////////////////////////////////////////////////////////////
 							}
 							break;
 						}
 						//isReadLine = false;
 						continue;
 					}
 	                #pragma endregion
 	
 					//判断G54非封闭区域结束标志
 					int nG36 = srcExpression.find("G36");
 					nG54 = srcExpression.find("G54");
 					if(nG36>=0 || nG54>=0)
 					{
 						bool isFilled = false;
 						GxmcPolygon* pShape= new GxmcPolygon(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,isFilled);
 						for (unsigned int i = 0; i < polygon.m_vecArc.size(); i++) //扇形区域
 						{
 							pShape->m_vecArc.push_back(polygon.m_vecArc[i]);
 						}	
 						for (unsigned int i = 0; i < polygon.m_vecPoint.size(); i++)//多边形区域
 						{
 							pShape->m_vecPoint.push_back(polygon.m_vecPoint[i]);
 						}
 						for (unsigned int i = 0; i < polygon.m_vecLine.size(); i++)//多边形区域
 						{
 							pShape->m_vecLine.push_back(polygon.m_vecLine[i]);
 						}
 						dataBlock.push_back(pShape);
 						dataBlockList.push_back(dataBlock);
 						pShape = NULL;
 
 						isReadLine = false;
 						break;
 					}
 
 					int nM02 = srcExpression.find("M02*");
 					if ( nM02 >= 0)
 					{
 						isReadLine = false;
 						break;
 					}
 				}
             }
 			#pragma endregion	
		}
	}
	void CGxmcGerber::ReadLine(std::ifstream& infile,bool& isReadLine,std::string& srcExpression)
	{
		char strBuf[BUFFERLENGTH];
		if (isReadLine)
		{
			infile.getline(strBuf,BUFFERLENGTH);
			srcExpression = strBuf;
		}
		else
		{
			isReadLine = true;
		}
		return;
	}
	void CGxmcGerber::ReadAmPara(std::string source,std::string chToken,std::vector<std::string>& vecMatch)
	{
		int index = 1;
		std::string str;
		std::vector<std::string> vecMatchTmp;
		vecMatch.swap(vecMatchTmp);
		while (index > 0)
		{
			index  = source.find(chToken);
			if (index <0)
			{
				vecMatch.push_back(source);
				break;
			}
			vecMatch.push_back(source.substr(0,index));
			source.erase(0,index+1);
		}

	}
	void CGxmcGerber::ReadTipInfo(std::ifstream& infile,char* strBuf,_TipInfo &tipInfo,std::map<std::string,AdStyle>& ADStyleMap)
	{	
		AdStyle adStyle;
		adStyle.Initialize();

		while(infile.getline(strBuf,BUFFERLENGTH))
		{
			std::vector<std::string> vecMatch;
			std::string srcExpression(strBuf);
			if (RegexSearch(srcExpression,"^G04",vecMatch))
			{
				continue;
			}

			#pragma region gerber文件格式
            if (RegexSearch(srcExpression,m_RegexPattern.prefix_FS,vecMatch))
			{
				if ("LA" == vecMatch[0])
				{
					m_TipInfo.ZeroOmit = LeadingOmit;
					m_TipInfo.YNumber.IntegerBits = atoi(vecMatch[1].c_str());
					m_TipInfo.XNumber.DecimalBits = atoi(vecMatch[2].c_str());
					m_TipInfo.XNumber.IntegerBits = atoi(vecMatch[3].c_str());
					m_TipInfo.YNumber.DecimalBits = atoi(vecMatch[4].c_str());

				}
				else
				{
					AfxMessageBox(L"gerbers数据格式未采用\n前导省零绝对坐标格式");
				}

				//m_RegexPattern.prefix_FS.clear();
				continue;
			} 
			if (RegexSearch(srcExpression,m_RegexPattern.prefix_MO,vecMatch))
			{
				m_TipInfo.Unit = vecMatch[1] == "MM" ? eUnit::Millimetre : eUnit::Inch;
				continue;
			} 
            #pragma endregion

			#pragma region AD信息读取
            if (RegexSearch(srcExpression,m_RegexPattern.prefix_ADD,vecMatch))
			{
				adStyle.Initialize();
				if (vecMatch[1] == "C,")
				{
					//此处圆直径本为零，为方便查看相关信息设置为0.8
					adStyle.adInfo.adCircle.Diameter = atof(vecMatch[2].c_str())==0.000 ? 0.6 : atof(vecMatch[2].c_str());
					adStyle.adInfo.adCircle.HoleDiameter = 0;
					adStyle.adName = Circle;
					ADStyleMap[vecMatch[0]] = adStyle;
				}
				if (vecMatch[1] == "O,")
				{
					adStyle.adInfo.adObround.Width = atof(vecMatch[2].c_str());
					adStyle.adInfo.adObround.Height = atof(vecMatch[3].c_str());
					adStyle.adInfo.adObround.HoleDiameter = 0;
					adStyle.adName = Obround;
					ADStyleMap[vecMatch[0]] = adStyle;
				}
				if (vecMatch[1] == "P,")
				{
					adStyle.adInfo.adRegularPolygon.OuterDiameter = atof(vecMatch[2].c_str());
					adStyle.adInfo.adRegularPolygon.numVertice = atof(vecMatch[3].c_str());
					adStyle.adInfo.adRegularPolygon.RotateAngle = atof(vecMatch[4].c_str());
					adStyle.adInfo.adRegularPolygon.HoleDiameter = 0;
					adStyle.adName = RegularPolygon;
					ADStyleMap[vecMatch[0]] = adStyle;
				}
				if (vecMatch[1] == "R,")
				{
					adStyle.adInfo.adRectangle.Width = atof(vecMatch[2].c_str());
					adStyle.adInfo.adRectangle.Height = atof(vecMatch[3].c_str());
					adStyle.adInfo.adRectangle.HoleDiameter = 0;
					adStyle.adName = Rectangle;
					ADStyleMap[vecMatch[0]] = adStyle;
				}
				continue;
			} 
            #pragma endregion

			#pragma region AM信息读取
            if (RegexSearch(srcExpression,m_RegexPattern.prefix_AM,vecMatch))
			{	
				AD_AmInfo adAmInfo;			
				int ncircle = 0,nVectorline = 0,nCenterline = 0;

				int count  = srcExpression.find("*");
				std::string str1 = srcExpression.substr(0,count);
				RegexSearch(str1,"([D0-9]{3,4})",vecMatch);
				std::string strDCode = vecMatch[0];

				std::string str2 = srcExpression.erase(0,count+1);

				std::string strToken;
				int codePrimitive = 0;
				if (str2 != "")//判断一行是否有两条语句
				{
					//去掉后缀*
					str2 = str2.erase(str2.length()-1,1);
					RegexSearch(str2,"([0-9]+),",vecMatch);
					strToken = vecMatch[0];
					codePrimitive = atoi(strToken.c_str());

					int num  = str2.find(",");
					str2.erase(0,num+1);
				}
				
				while (true)
				{
					switch (codePrimitive)
					{
					case 0:             //注释行
						break;
					case 1:             //circle
						{
							AM_Circle amCircle;
							ReadAmPara(str2,",",vecMatch);
							amCircle.exposure = atoi(vecMatch[0].c_str());
							amCircle.diameter = atof(vecMatch[1].c_str());
							double cpX = atof(vecMatch[2].c_str());
							double cpY = atof(vecMatch[3].c_str());
							PointD* cp = new PointD(cpX,cpY);
							amCircle.centerPoint = cp;
							amCircle.rotationAngle = 0.0;
							if (vecMatch.size() > 4)
							{
								amCircle.rotationAngle = atof(vecMatch[4].c_str());
							}		
							adAmInfo.vecAmCircle.push_back(amCircle);
						}
						break;
					case 4:             //outline
						{
							AM_OutLine amOutLine;
							ReadAmPara(str2,",",vecMatch);
							amOutLine.exposure = atoi(vecMatch[0].c_str());
							amOutLine.numVertex = atoi(vecMatch[1].c_str());
						}
						break;
					case 5:             //Polygon
						{
							AM_Polygon amPolygon;
							ReadAmPara(str2,",",vecMatch);
							amPolygon.exposure = atoi(vecMatch[0].c_str());
							amPolygon.numVertex = atoi(vecMatch[1].c_str());
							double cpX = atof(vecMatch[2].c_str());
							double cpY = atof(vecMatch[3].c_str());
							PointD* cp = new PointD(cpX,cpY);
							amPolygon.centerPoint = cp;
							amPolygon.diameter = atof(vecMatch[4].c_str());
							amPolygon.rotationAngle = 0.0;
							if (vecMatch.size() > 5)
							{
								amPolygon.rotationAngle = atof(vecMatch[5].c_str());
							}

							adAmInfo.vecAmPolygon.push_back(amPolygon);
						}
						break;
					case 6:             //moire
						{
							AM_Moire amMoire;
							ReadAmPara(str2,",",vecMatch);
							double cpX = atof(vecMatch[0].c_str());
							double cpY = atof(vecMatch[1].c_str());
							PointD* cp = new PointD(cpX,cpY);
							amMoire.centerPoint = cp;
							amMoire.outerDiameter = atof(vecMatch[2].c_str());
							amMoire.ringThickness = atof(vecMatch[3].c_str());
							amMoire.ringGap = atof(vecMatch[4].c_str());
							amMoire.ringMaxNum = atoi(vecMatch[5].c_str());
							amMoire.crosshairThickness = atof(vecMatch[6].c_str());
							amMoire.crosshairLength = atof(vecMatch[7].c_str());
							amMoire.rotationAngle = 0.0;
							if (vecMatch.size() > 8)
							{
								amMoire.rotationAngle = atof(vecMatch[8].c_str());
							}

							adAmInfo.vecAmMoire.push_back(amMoire);
						}
						break;
					case 7:             //thermal
						{
							AM_Thermal amThermal;
							ReadAmPara(str2,",",vecMatch);
							double cpX = atof(vecMatch[0].c_str());
							double cpY = atof(vecMatch[1].c_str());
							PointD* cp = new PointD(cpX,cpY);
							amThermal.centerPoint = cp;
							amThermal.outerDiameter = atof(vecMatch[2].c_str());
							amThermal.innerDiameter = atof(vecMatch[3].c_str());
							amThermal.gapThickness = atof(vecMatch[4].c_str());
							amThermal.rotationAngle = 0.0;
							if (vecMatch.size() > 5)
							{
							  amThermal.rotationAngle = atof(vecMatch[5].c_str());
							}

							adAmInfo.vecAmThermal.push_back(amThermal);
						}
						break;
					case 20:            //vectorline
						{
							AM_VectorLine	amVectorLine;
							ReadAmPara(str2,",",vecMatch);
							amVectorLine.exposure = atoi(vecMatch[0].c_str());
							amVectorLine.lineWidth = atof(vecMatch[1].c_str());
							double startPointX = atof(vecMatch[2].c_str());
							double startPointY = atof(vecMatch[3].c_str());
							PointD* startPoint = new PointD(startPointX,startPointY);
							amVectorLine.startPoint = startPoint;						
							double stopPointX = atof(vecMatch[4].c_str());
							double stopPointY = atof(vecMatch[5].c_str());
							PointD* stopPoint = new PointD(stopPointX,stopPointY);
							amVectorLine.stopPoint = stopPoint;
							amVectorLine.rotationAngle = 0.0;
							if (vecMatch.size() > 6)
							{
								amVectorLine.rotationAngle = atof(vecMatch[6].c_str());
							}

							adAmInfo.vecAmVectorLine.push_back(amVectorLine);
						}
						break;
					case 21:            //centerline
						{
							AM_CenterLine amCenterLine;
							ReadAmPara(str2,",",vecMatch);
							amCenterLine.exposure = atoi(vecMatch[0].c_str());
							amCenterLine.rectangleWidth = atof(vecMatch[1].c_str());
							amCenterLine.rectangleHeight = atof(vecMatch[2].c_str());
							double cpX = atof(vecMatch[3].c_str());
							double cpY = atof(vecMatch[4].c_str());
							PointD* cp = new PointD(cpX,cpY);
							amCenterLine.centerPoint = cp;
							amCenterLine.rotationAngle = 0.0;
							if (vecMatch.size() > 5)
							{
							   amCenterLine.rotationAngle = atof(vecMatch[5].c_str());
							}

							adAmInfo.vecAmCenterLine.push_back(amCenterLine);
						}
						break;
					}

					infile.getline(strBuf,BUFFERLENGTH);
					std::string str(strBuf);
					str2 = str;
					if (str2.find("%ADD") > 0)
					{
						int num  = str2.find(",");
						strToken = str2.substr(0,num);
						codePrimitive = atoi(strToken.c_str());
						str2.erase(0,num+1);
						str2.erase(str2.length()-1,1);
					}
					else
					{
						AD_AmInfo* _adAmInfo = new AD_AmInfo();
						_adAmInfo->vecAmCenterLine = adAmInfo.vecAmCenterLine;
						_adAmInfo->vecAmCircle = adAmInfo.vecAmCircle;
						_adAmInfo->vecAmMoire = adAmInfo.vecAmMoire;
						_adAmInfo->vecAmOutLine = adAmInfo.vecAmOutLine;
						_adAmInfo->vecAmPolygon = adAmInfo.vecAmPolygon;
						_adAmInfo->vecAmThermal = adAmInfo.vecAmThermal;
						_adAmInfo->vecAmVectorLine = adAmInfo.vecAmVectorLine;
						adStyle.Initialize();
						adStyle.adInfo.adAmInfo = _adAmInfo;
						adStyle.adName = Am;
						ADStyleMap[strDCode] = adStyle;
						break;
					}
				}	
				continue;	
			}
            #pragma endregion

			if (RegexSearch(srcExpression,"%LPD",vecMatch))
			{
				break;
			}	
		}

	}
	void CGxmcGerber::ReadLinearIterpolateOperationD01(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD& point,PointD& CurPoint,GxmcPolygon& polygon,
		                                           RectD& realMarginRect,bool& isD01Read,bool isFilled,bool isIncludeLine)
	{
		int nX = srcExpression.find("X");
		int nY = srcExpression.find("Y");
		int nD = srcExpression.find("D01");
		if(nX >= 0 && nY > 0 && nD > 0)
		{
			//G36绘图模式下，D02结束当前轮廓的绘制，光圈移动到当前指定的位置点，绘制线型轮廓段								
			std::string strX = srcExpression.substr(nX+1,nY-1);
			std::string strY = srcExpression.substr(nY+1,nD-nY-1);
			GetValueFromStr(strX,strY,dPreviousX,dPreviousY,point.x,point.y);	
			if (isFilled)
			{
				polygon.m_vecPoint.push_back(point);
			}
			if(isIncludeLine)
			{
				GxmcLine line(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,isFilled,CurPoint,point);
				polygon.m_vecLine.push_back(line);
			}
		
			UpdateMargin(point,realMarginRect);
			//更新当前点
			CurPoint = point;		
			isD01Read = true;
		}
		else
		{
			isD01Read = false;
		}
	}
	void CGxmcGerber::ReadCircularIterpolateOperationD01(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD point,PointD& CurPoint,GxmcPolygon& polygon,
		                                             RectD& realMarginRect,bool& isD01Read,bool isFilled)
	{
		int nG = srcExpression.find("G02")*srcExpression.find("G03");
		int nX = srcExpression.find("X");
		int nY = srcExpression.find("Y");
		int nI = srcExpression.find("I");
		int nJ = srcExpression.find("J");
		int nD01 = srcExpression.find("D01");
		int nArcDirection = AD_COUNTERCLOCKWISE;

		if(nX*nY*nI*nJ*nD01 >= 0 && nG <= 0)
		{
			std::string strX = srcExpression.substr(nX+1,nY-nX-1);
			std::string strY = srcExpression.substr(nY+1,nI-nY-1);
			std::string strI = srcExpression.substr(nI+1,nJ-nI-1);
			std::string strJ = srcExpression.substr(nJ+1,nD01-nJ-1);
			int nG02 = srcExpression.find("G02");
			if ( nG02>= 0)
			{
				nArcDirection = AD_CLOCKWISE;
			}
			else
			{
				nArcDirection = AD_COUNTERCLOCKWISE;
			}

			PointD startPoint,stopPoint,centerPoint;
			//当前点（圆弧始点）为点集的最后一个点，圆弧终点为圆弧始点加XY偏移量，
			GetValueFromStr(strX,strY,dPreviousX,dPreviousY,stopPoint.x,stopPoint.y);
			//圆弧终点更新边界
			UpdateMargin(stopPoint,realMarginRect);
			//读取圆弧中心对圆弧始点偏移量IJ
			GetValueFromStr(strI,strJ,dPreviousX,dPreviousY,centerPoint.x,centerPoint.y);
			//////////////////////////////////////////////////////////////////////////
			///起始点，没有考虑两次以上点集存在的情况	
			startPoint = CurPoint;
			centerPoint.x += startPoint.x;
			centerPoint.y += startPoint.y;
			double radius = sqrt(pow(centerPoint.x - startPoint.x,2) + pow(centerPoint.y - startPoint.y,2));
			GxmcArc arc = GxmcArc(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,isFilled,centerPoint,radius,startPoint,stopPoint,nArcDirection); 
			polygon.m_vecArc.push_back(arc);	
			polygon.m_vecPoint.push_back(startPoint);
			polygon.m_vecPoint.push_back(stopPoint);
			//更新当前点
			CurPoint = arc.m_stopPoint;

			isD01Read = true;
			return;
		}
		
		isD01Read = false;
	}
	void CGxmcGerber::ReadMoveOpreationD02(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect)
	{
		int nX = srcExpression.find("X");
		int nY = srcExpression.find("Y");
		int nD = srcExpression.find("D02");
		if(nX*nY*nD >= 0)
		{
			std::string strX = srcExpression.substr(nX+1,nY-1);
			std::string strY = srcExpression.substr(nY+1,nD-nY-1);
			GetValueFromStr(strX,strY,dPreviousX,dPreviousY,point.x,point.y);
			polygon.m_vecPoint.push_back(point);

			UpdateMargin(point,realMarginRect);
			//图形区域的起始点，G36后第一次遇到G01
			CurPoint = point;
		}
	}
	void CGxmcGerber::ReadMultiQuadrantModeG75(std::string srcExpression,double& dPreviousX,double& dPreviousY,PointD point,PointD& CurPoint,GxmcPolygon& polygon,RectD& realMarginRect)
	{

	}
	void CGxmcGerber::GetValueFromStr(std::string strX,std::string strY,double& dPreviousX,double& dPreviousY,double& dX,double& dY)
	{
		int nXDecimalBits = m_TipInfo.XNumber.DecimalBits;
		int nYDecimalBits = m_TipInfo.YNumber.DecimalBits;
		int XBits = 0;
		int YBits = 0;
		switch (m_TipInfo.ZeroOmit)
		{
		case NotOmit:
		case LeadingOmit:
			break;
		case TailOmit:
			//整数部分位数与规定一致不变，小数部分位数可能会忽略末尾的0
			nXDecimalBits = XBits - m_TipInfo.XNumber.IntegerBits;
			nYDecimalBits = YBits - m_TipInfo.YNumber.IntegerBits;
			break;
		}

		if (strX != "")
		{
			dX = std::stoi(strX)/pow(10.0,nXDecimalBits);
			dPreviousX = dX;
		} 
		else
		{
			dX = dPreviousX;
		}

		if (strY != "")
		{
			dY = std::stoi(strY)/pow(10.0,nYDecimalBits);
			dPreviousY = dY;
		} 
		else
		{
			dY = dPreviousY;
		}

		if (m_TipInfo.Unit == Inch)
		{
			dX = dX *(25.4);
			dY = dY *(25.4);
		}

		//	return;
	}
	/*************************************************Gerber文件读取 结尾*************************************************************************************/

	/*************************************************图元显示 开头*************************************************************************************/
	void CGxmcGerber::DisplayGerber(CDC& dc,ScreenStruct screen,const std::list<DataBlock>& dataBlockList)
	{
		//const_iterator迭代器可修改，迭代器指向的数据不能修改
		std::list<DataBlock>::const_iterator  dataBlockItor = dataBlockList.begin();
		for (;dataBlockItor != dataBlockList.end();dataBlockItor++)
		{
			DataBlock::const_iterator  shapeItor = dataBlockItor->begin();
			for (;shapeItor != dataBlockItor->end();shapeItor++)
			{
				(*shapeItor)->Draw(dc,screen);		
			}
		}		
	}
	void CGxmcGerber::SetDrawingPara(const DrawingPara& drawpara)
	{
		m_drawingPara.startX   = drawpara.startX;
		m_drawingPara.startY   = drawpara.startY;
		m_drawingPara.heightScreen = drawpara.heightScreen;
		m_drawingPara.widthScreen  = drawpara.widthScreen;
		m_drawingPara.factor  = drawpara.factor;
	}
	void CGxmcGerber::ReleaseDataBlock(std::list<DataBlock>& dataBlockList)
	{
		std::list<DataBlock>::iterator dataBlockItor = dataBlockList.begin();
		for (;dataBlockItor != dataBlockList.end();)
		{
			DataBlock::iterator shapeItor = dataBlockItor->begin();
			for (;shapeItor != dataBlockItor->end();)
			{
				delete 	*shapeItor;
				*shapeItor = NULL;
				dataBlockItor->erase(shapeItor++);
			}
			dataBlockList.erase(dataBlockItor++);
		}		
	}
	void CGxmcGerber::ReleaseClusterMap(std::map<int,Cluster>& mapCluster)
	{
		std::map<int,Cluster>::iterator iteratorCluster = mapCluster.begin();
		for ( ; iteratorCluster != mapCluster.end();)
		{
			if (iteratorCluster->second.clusterMargin != NULL)
			{
				delete iteratorCluster->second.clusterMargin;
				iteratorCluster->second.clusterMargin = NULL;
			}	

			mapCluster.erase(iteratorCluster++);
		}
	}
	void CGxmcGerber::ReleaseADStyleMap()
	{
		if (m_adStyleMap.size() <= 0)
		{
			return;
		}
		std::map<std::string,AdStyle>::iterator iter = m_adStyleMap.begin();
		for ( ; iter != m_adStyleMap.end(); )
		{
			if (iter->second.adName != eAdName::Am)
			{
				m_adStyleMap.erase(iter++);
				continue;
			}
			if (iter->second.adInfo.adAmInfo->vecAmCircle.size() > 0 )
			{
				std::vector<AM_Circle>::iterator  itorAmCircle = iter->second.adInfo.adAmInfo->vecAmCircle.begin();
				for ( ; itorAmCircle != iter->second.adInfo.adAmInfo->vecAmCircle.end(); itorAmCircle++)
				{
					delete itorAmCircle->centerPoint;
					itorAmCircle->centerPoint = NULL;
					itorAmCircle = iter->second.adInfo.adAmInfo->vecAmCircle.erase(itorAmCircle);
				}
			}
			if (iter->second.adInfo.adAmInfo->vecAmVectorLine.size() > 0 )
			{
			}
			if (iter->second.adInfo.adAmInfo->vecAmCenterLine.size() > 0 )
			{
				std::vector<AM_CenterLine>::iterator itorAmCenterLine = iter->second.adInfo.adAmInfo->vecAmCenterLine.begin();
				for ( ; itorAmCenterLine != iter->second.adInfo.adAmInfo->vecAmCenterLine.end(); itorAmCenterLine++)
				{
					delete itorAmCenterLine->centerPoint;
					itorAmCenterLine->centerPoint = NULL; 
					itorAmCenterLine = iter->second.adInfo.adAmInfo->vecAmCenterLine.erase(itorAmCenterLine);
				}
			}
			if (iter->second.adInfo.adAmInfo->vecAmOutLine.size() > 0 )
			{
				std::vector<AM_OutLine>::iterator itorAmOutLine = iter->second.adInfo.adAmInfo->vecAmOutLine.begin();
				for ( ; itorAmOutLine != iter->second.adInfo.adAmInfo->vecAmOutLine.end(); itorAmOutLine++)
				{
					delete[] itorAmOutLine->pointList;
					itorAmOutLine->pointList = NULL; 
					itorAmOutLine = iter->second.adInfo.adAmInfo->vecAmOutLine.erase(itorAmOutLine);
				}
			}
			if (iter->second.adInfo.adAmInfo->vecAmPolygon.size() > 0 )
			{
				std::vector<AM_Polygon>::iterator itorAmPolygon = iter->second.adInfo.adAmInfo->vecAmPolygon.begin();
				for ( ; itorAmPolygon != iter->second.adInfo.adAmInfo->vecAmPolygon.end(); itorAmPolygon++)
				{
					delete itorAmPolygon->centerPoint;
					itorAmPolygon->centerPoint = NULL; 
					itorAmPolygon = iter->second.adInfo.adAmInfo->vecAmPolygon.erase(itorAmPolygon);
				}
			}
			if (iter->second.adInfo.adAmInfo->vecAmMoire.size() > 0 )
			{
				std::vector<AM_Moire>::iterator itorAmMoire = iter->second.adInfo.adAmInfo->vecAmMoire.begin();
				for ( ; itorAmMoire != iter->second.adInfo.adAmInfo->vecAmMoire.end(); itorAmMoire++)
				{
					delete itorAmMoire->centerPoint;
					itorAmMoire->centerPoint = NULL; 
					itorAmMoire = iter->second.adInfo.adAmInfo->vecAmMoire.erase(itorAmMoire);
				}
			}
			if (iter->second.adInfo.adAmInfo->vecAmThermal.size() > 0 )
			{
				std::vector<AM_Thermal>::iterator itorAmThermal = iter->second.adInfo.adAmInfo->vecAmThermal.begin();
				for ( ; itorAmThermal != iter->second.adInfo.adAmInfo->vecAmThermal.end(); itorAmThermal++)
				{
					delete itorAmThermal->centerPoint;
					itorAmThermal->centerPoint = NULL; 
					itorAmThermal = iter->second.adInfo.adAmInfo->vecAmThermal.erase(itorAmThermal);
				}
			}
			delete iter->second.adInfo.adAmInfo;
			iter->second.adInfo.adAmInfo = NULL;
			m_adStyleMap.erase(iter++);

		}
	}
	/*************************************************图元显示 结尾*************************************************************************************/

    #pragma region 光圈形状定义
	void ACircle::GetBoundaryRect(RectD& rect)
	{
		rect = RectD(m_CenterPoint.x - m_adCircle.Diameter/2,m_CenterPoint.y + m_adCircle.Diameter/2,
			        m_CenterPoint.x + m_adCircle.Diameter/2,m_CenterPoint.y - m_adCircle.Diameter/2);
	}
	void ACircle::Draw(CDC& dc,const ScreenStruct& screen)
	{
		if (m_bIsHidden)
		{
			return;
		}
		GxmcCircle circle(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,m_CenterPoint,m_adCircle.Diameter/2);
		circle.Draw(dc,screen);
	}
	void ACircle::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}
	void ARectangle::GetBoundaryRect(RectD& rect)
	{
		double width = m_adRectangle.Width/2;
		double height = m_adRectangle.Height/2;
		rect = RectD(m_CenterPoint.x - width,m_CenterPoint.y + height,m_CenterPoint.x + width,m_CenterPoint.y - height);
	}
	void ARectangle::Draw(CDC& dc,const ScreenStruct& screen)
	{
		if (m_bIsHidden)
		{
			return;
		}
		PointD Point1,Point2,Point3,Point4;
		double width = m_adRectangle.Width/2;
		double height = m_adRectangle.Height/2;

		//Point1 = PointD(m_CenterPoint.x - width,m_CenterPoint.y - height);
		Point2 = PointD(m_CenterPoint.x + width,m_CenterPoint.y - height);
		//Point3 = PointD(m_CenterPoint.x + width,m_CenterPoint.y + height);
		Point4 = PointD(m_CenterPoint.x - width,m_CenterPoint.y + height);

		GxmcRect rect(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,Point4,Point2);
		rect.Draw(dc,screen);
	}
	void ARectangle::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}
	void AObround::GetBoundaryRect(RectD& rect)
	{
		double halfWidth = m_adObround.Width/2;
		double halfHeight = m_adObround.Height/2;
		double radius = halfWidth < halfHeight ? halfWidth : halfHeight;
		if (halfWidth > halfHeight)//Horizonal
		{
			double dW = halfWidth - radius;
			rect = RectD(m_CenterPoint.x - dW-radius,m_CenterPoint.y + halfHeight,
				        m_CenterPoint.x + dW+radius,m_CenterPoint.y - halfHeight);
		}
		else//Vertical
		{
			double dH = halfHeight - radius;
			rect = RectD(m_CenterPoint.x - halfWidth,m_CenterPoint.y + dH+radius,
				        m_CenterPoint.x + halfWidth,m_CenterPoint.y - dH-radius);
		}
	}
	void AObround::Draw(CDC& dc,const ScreenStruct& screen)
	{
		if (m_bIsHidden)
		{
			return;
		}
		double halfWidth = m_adObround.Width/2;
		double halfHeight = m_adObround.Height/2;
		double radius = halfWidth < halfHeight ? halfWidth : halfHeight;
		//m_penColor = RGB(255,0,255);
		PointD centerPoint = m_CenterPoint;

		if (halfWidth > halfHeight)//Horizonal
		{
			double dW = halfWidth - radius;
			PointD LeftCenter(m_CenterPoint.x - dW,m_CenterPoint.y);
			PointD RightCenter(m_CenterPoint.x + dW,m_CenterPoint.y);

			PointD ltPoint(m_CenterPoint.x - dW,m_CenterPoint.y + halfHeight);
			PointD rtPoint(m_CenterPoint.x + dW,m_CenterPoint.y + halfHeight);

			PointD lbPoint(m_CenterPoint.x - dW,m_CenterPoint.y - halfHeight);
			PointD rbPoint(m_CenterPoint.x + dW,m_CenterPoint.y - halfHeight);

			GxmcCircle circleL(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,LeftCenter,radius);
			circleL.Draw(dc,screen);
			GxmcCircle circleR(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,RightCenter,radius);
			circleR.Draw(dc,screen);

			GxmcRect rect(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,ltPoint,rbPoint);
			rect.Draw(dc,screen);

		}
		else//Vertical
		{
			double dH = halfHeight - radius;
			PointD TopCenter(m_CenterPoint.x,m_CenterPoint.y - dH);
			PointD BottomCenter(m_CenterPoint.x,m_CenterPoint.y + dH);

			PointD ltPoint(m_CenterPoint.x - halfWidth,m_CenterPoint.y + dH);
			PointD rtPoint(m_CenterPoint.x + halfWidth,m_CenterPoint.y + dH);

			PointD lbPoint(m_CenterPoint.x - halfWidth,m_CenterPoint.y - dH);
			PointD rbPoint(m_CenterPoint.x + halfWidth,m_CenterPoint.y - dH);

			GxmcCircle circleT(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,TopCenter,radius);
			circleT.Draw(dc,screen);
			GxmcCircle circleB(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,BottomCenter,radius);
			circleB.Draw(dc,screen);

			GxmcRect rect(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,ltPoint,rbPoint);
			rect.Draw(dc,screen);
		}

	}
	void AObround::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}
	void ARegularPolygon::GetBoundaryRect(RectD& rect)
	{

	}
	void ARegularPolygon::Draw(CDC& dc,const ScreenStruct& screen)
	{
		if (m_bIsHidden)
		{
			return;
		}
		// 		int numVertex;
		// 		PointD* PointList;
		// 		CGxmcPolygon polygon(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,numVertex,PointList);
		// 		polygon.Draw(dc,screen);
	}
	void ARegularPolygon::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}
	void AMShape::GetBoundaryRect(RectD& rect)
	{
		if (!m_adAmInfo.vecAmCircle.empty())
		{
			for (unsigned int i = 0; i < m_adAmInfo.vecAmCircle.size(); i++)
			{
				PointD centerPoint(m_CenterPoint.x + m_adAmInfo.vecAmCircle[i].centerPoint->x,
					               m_CenterPoint.y + m_adAmInfo.vecAmCircle[i].centerPoint->y);
				UpdateMargin(centerPoint,rect);
			}
			double radius = m_adAmInfo.vecAmCircle[0].diameter/2;
			rect = RectD(rect.left - radius,rect.top + radius,rect.right + radius,rect.bottom - radius);
		}
		if (!m_adAmInfo.vecAmVectorLine.empty())
		{
			for (unsigned int i = 0; i < m_adAmInfo.vecAmVectorLine.size(); i++)
			{
				double left,top,right,bottom;
				left = m_CenterPoint.x + m_adAmInfo.vecAmVectorLine[i].startPoint->x;
				top = m_CenterPoint.y + m_adAmInfo.vecAmVectorLine[i].startPoint->y + m_adAmInfo.vecAmVectorLine[i].lineWidth/2;
				right  = m_CenterPoint.x + m_adAmInfo.vecAmVectorLine[i].stopPoint->x;
				bottom  = m_CenterPoint.y + m_adAmInfo.vecAmVectorLine[i].stopPoint->y - m_adAmInfo.vecAmVectorLine[i].lineWidth/2;
				RectD rectTmp(left,top,right,bottom);
				UpdateMargin(rectTmp,rect);
			}

		}
		if (!m_adAmInfo.vecAmCenterLine.empty())
		{
			for (unsigned int i = 0; i < m_adAmInfo.vecAmCenterLine.size(); i++)
			{
				double left,top,right,bottom;
				left = m_CenterPoint.x + m_adAmInfo.vecAmCenterLine[i].centerPoint->x - m_adAmInfo.vecAmCenterLine[i].rectangleWidth/2;
				top = m_CenterPoint.y + m_adAmInfo.vecAmCenterLine[i].centerPoint->y + m_adAmInfo.vecAmCenterLine[i].rectangleHeight/2;
				right = m_CenterPoint.x + m_adAmInfo.vecAmCenterLine[i].centerPoint->x + m_adAmInfo.vecAmCenterLine[i].rectangleWidth/2;
				bottom = m_CenterPoint.y + m_adAmInfo.vecAmCenterLine[i].centerPoint->y - m_adAmInfo.vecAmCenterLine[i].rectangleHeight/2;
				RectD rectTmp(left,top,right,bottom);
				UpdateMargin(rectTmp,rect);
			}
			
		}
		if (!m_adAmInfo.vecAmPolygon.empty())
		{

		}
		if (!m_adAmInfo.vecAmOutLine.empty())
		{

		}
		if (!m_adAmInfo.vecAmMoire.empty())
		{

		}
		if (!m_adAmInfo.vecAmThermal.empty())
		{

		}

	}
	void AMShape::Draw(CDC& dc,const ScreenStruct& screen)
	{
		if (m_bIsHidden)
		{
			return;
		}
		m_adAmInfo.vecAmCenterLine;
		//m_penColor = RGB(0,0,255);
		//Circle
		for (unsigned int i = 0; i < m_adAmInfo.vecAmCircle.size();i++)
		{
			PointD centerPoint;
			centerPoint.x = m_CenterPoint.x + m_adAmInfo.vecAmCircle[i].centerPoint->x;
			centerPoint.y = m_CenterPoint.y + m_adAmInfo.vecAmCircle[i].centerPoint->y;

			GxmcCircle gxmcCircle(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,centerPoint,m_adAmInfo.vecAmCircle[i].diameter/2);
			gxmcCircle.Draw(dc,screen);
		}
		//VectorLine
		for (unsigned int i = 0; i < m_adAmInfo.vecAmVectorLine.size();i++)
		{
			PointD startPoint,stopPoint;
			startPoint.x = m_CenterPoint.x + m_adAmInfo.vecAmVectorLine[i].startPoint->x;
			startPoint.y = m_CenterPoint.y + m_adAmInfo.vecAmVectorLine[i].startPoint->y + m_adAmInfo.vecAmVectorLine[i].lineWidth/2;
			stopPoint.x  = m_CenterPoint.x + m_adAmInfo.vecAmVectorLine[i].stopPoint->x;
			stopPoint.y  = m_CenterPoint.y + m_adAmInfo.vecAmVectorLine[i].stopPoint->y - m_adAmInfo.vecAmVectorLine[i].lineWidth/2;

			GxmcRect gxmcRect(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,startPoint,stopPoint);
			gxmcRect.Draw(dc,screen);
		}
		//CenterLine
		for (unsigned int i = 0; i < m_adAmInfo.vecAmCenterLine.size();i++)
		{
			PointD startPoint,stopPoint;
			startPoint.x = m_CenterPoint.x + m_adAmInfo.vecAmCenterLine[i].centerPoint->x - m_adAmInfo.vecAmCenterLine[i].rectangleWidth/2;
			startPoint.y = m_CenterPoint.y + m_adAmInfo.vecAmCenterLine[i].centerPoint->y + m_adAmInfo.vecAmCenterLine[i].rectangleHeight/2;
			stopPoint.x = m_CenterPoint.x + m_adAmInfo.vecAmCenterLine[i].centerPoint->x + m_adAmInfo.vecAmCenterLine[i].rectangleWidth/2;
			stopPoint.y = m_CenterPoint.y + m_adAmInfo.vecAmCenterLine[i].centerPoint->y - m_adAmInfo.vecAmCenterLine[i].rectangleHeight/2;

			GxmcRect gxmcRect(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,startPoint,stopPoint);
			gxmcRect.Draw(dc,screen);
		}
		//Polygon
		for (unsigned int i = 0; i < m_adAmInfo.vecAmPolygon.size();i++)
		{
			
		}
		//Outline
		for (unsigned int i = 0; i < m_adAmInfo.vecAmOutLine.size();i++)
		{

		}
		//Moire
		for (unsigned int i = 0; i < m_adAmInfo.vecAmMoire.size();i++)
		{

		}
		//Thermal
		for (unsigned int i = 0; i < m_adAmInfo.vecAmThermal.size();i++)
		{

		}

	}
	void AMShape::SetColour(COLORREF PenColor,COLORREF BrushColor)
	{
		m_penColor = PenColor; m_brushColor = BrushColor;
	}
    #pragma endregion

	#pragma region FOV路径计算

    void CGxmcGerber::DisplayFovPath(CDC& dc,ScreenStruct screen,const std::map<int,Cluster>& mapCluster)
	{
		std::map<int,Cluster>::const_iterator iteratorCluster = mapCluster.begin();
		for ( ; iteratorCluster != mapCluster.end(); iteratorCluster++)
		{
			iteratorCluster->second.clusterMargin->Draw(dc,screen);
		}
	}
	//计算数据块所围封闭图形的边界框
	void CGxmcGerber::GetFovPath(const std::list<DataBlock>& dataBlockList,double ViewWidth,double ViewHeight,std::map<int,Cluster>& mapCluster)
	{
		if (dataBlockList.empty())
		{
			return;
		}
		int clusterIndex = 0;
		//把Polygon存到Map里
		//std::map<int,_Polygon> mapPolygon;
		if (m_mapPolygon.size() < 1)
		{
			int polygonIndex = 0;
			std::list<DataBlock>:: const_iterator itorPolygon = dataBlockList.begin();
			for ( ; itorPolygon != dataBlockList.end();itorPolygon++)
			{
				_Polygon polygon;
				polygon.dataBlock = *itorPolygon;
				polygon.margin = INITI_RECT;
				RectD rect= INITI_RECT;
				//计算数据块所围封闭图形的边界框
				std::list<GxmcShape*>:: iterator itorShape = polygon.dataBlock.begin();
				for ( ; itorShape != polygon.dataBlock.end(); itorShape++)
				{
					if ((*itorShape)->m_bIsHidden)
					{
						continue;
					}
					(*itorShape)->GetBoundaryRect(rect);
					(*itorShape)->UpdateMargin(rect,polygon.margin);
				}
				if (polygon.margin.left == MAXNUM || polygon.margin.top == MINNUM || polygon.margin.right == MINNUM || polygon.margin.bottom == MAXNUM)
				{
					continue;
				}
				m_mapPolygon.insert(std::make_pair<int,_Polygon>(polygonIndex++,polygon));
			}
		}
		
		CalculateFovPath(mapCluster,clusterIndex,m_mapPolygon,ViewWidth,ViewHeight);
	}
	/***************************************************************************************************************/                   
	/**                 SplitGerberRegion                                                                              
	/**   @函数描述：   镜头视野分配及其路径优化                                                                             
	/**   @函数参数：                                                                                 
	/**   @作者：       陈杰                                                                                  
	/**   @日期：       2016/11/25_09                                                                          
	/**   @修订说明：                                                                                     
	/**                 深圳慧大成智能科技有限公司 All Rights Reserved.                                           
	/***************************************************************************************************************/
	void CGxmcGerber::CalculateFovPath(std::map<int,Cluster>& mapCluster,int& clusterIndex,std::map<int,_Polygon>& mapPolygon,double ViewWidth,double ViewHeight)
	{
		//寻找Polygon集合里Bottom值最小的polygon
		std::map<int,_Polygon>::iterator itorMapPolygon = mapPolygon.begin();
		itorMapPolygon = min_element( mapPolygon.begin(), mapPolygon.end(),map_PointLessByY());
		_Polygon polygonMinY = itorMapPolygon->second;

		//筛选在[Bottom，Bottom + 2*ViewHeight]之间的Polygon
		std::map<int,_Polygon> mapPolygonsByY;
		int mapIndex = 0;
		itorMapPolygon = mapPolygon.begin();
		for ( ; itorMapPolygon != mapPolygon.end();itorMapPolygon++)
		{
			if (itorMapPolygon->second.margin.bottom >= polygonMinY.margin.bottom        &&
				itorMapPolygon->second.margin.top <= polygonMinY.margin.bottom + ViewHeight )
			{
				mapPolygonsByY.insert(std::make_pair<int,_Polygon>(mapIndex++,itorMapPolygon->second));
			}
		}
		
		bool IsClusterFirstWorkInMap = true;
		//划分top区间内的polygon集合里的元素，直到集合为空
		while (!mapPolygonsByY.empty())
		{
			int nClusterHeight = 1;
			double minLeft,minBottom;
			//筛选[Bottom，Bottom +  n*ViewHeight]区间Polygon集合里left最小的polygon
			std::map<int,_Polygon>::iterator itormapPolygonsByY = mapPolygonsByY.begin();
			itormapPolygonsByY = min_element( mapPolygonsByY.begin(), mapPolygonsByY.end(),map_PointLessByY());
			polygonMinY = itormapPolygonsByY->second;
			itormapPolygonsByY = min_element( mapPolygonsByY.begin(), mapPolygonsByY.end(),map_PointLessByX());
			minLeft = itormapPolygonsByY->second.margin.left;
			
			//**************确定Cluster边界***************
			//确定cluster边框的bottom边
			minBottom = MAXNUM;
			std::vector<_Polygon> vecPolygons;
			itormapPolygonsByY = mapPolygonsByY.begin();
			for ( ; itormapPolygonsByY != mapPolygonsByY.end();itormapPolygonsByY++)
			{
				if (itormapPolygonsByY->second.margin.left   >= minLeft    &&
					itormapPolygonsByY->second.margin.right  <= minLeft + ViewWidth )
				{	
					if (itormapPolygonsByY->second.margin.bottom < minBottom)
					{
						minBottom = itormapPolygonsByY->second.margin.bottom;
					}
					vecPolygons.push_back(itormapPolygonsByY->second);
				}
			}

			int count = 0;
			while (true)
			{
				count++;
				if (count > nClusterHeight ||vecPolygons.empty())
				{ 
					break; 
				}
				if (count > nClusterHeight )
				{
					std::vector<_Polygon>::iterator iteratorVecY = min_element(vecPolygons.begin(),vecPolygons.end(),vector_PointLessByY());
					minBottom = iteratorVecY->margin.bottom;

					std::vector<_Polygon>::iterator iteratorVecX = min_element(vecPolygons.begin(),vecPolygons.end(),vector_PointLessByX());
					minLeft = iteratorVecX->margin.left;
				}
				Cluster cluster;
				PointD startPoint,stopPoint;
				//确定cluster边界框及中心点
				startPoint.x = minLeft;
				startPoint.y = minBottom;
				stopPoint.x  = startPoint.x + ViewWidth;
				stopPoint.y  = startPoint.y + ViewHeight;
				cluster.centerPoint.x  = (startPoint.x + stopPoint.x) /2;
				cluster.centerPoint.y  = (startPoint.y + stopPoint.y) /2;

				bool isFilled = false;
				cluster.clusterMargin = new GxmcRect(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,isFilled,startPoint,stopPoint);
				itormapPolygonsByY = mapPolygonsByY.begin();
				for ( ; itormapPolygonsByY != mapPolygonsByY.end();)
				{
					if (itormapPolygonsByY->second.margin.left   >= startPoint.x  &&
						itormapPolygonsByY->second.margin.bottom >= startPoint.y  &&
						itormapPolygonsByY->second.margin.right  <= stopPoint.x   &&
						itormapPolygonsByY->second.margin.top    <= stopPoint.y   )
					{ 
						cluster.vecPolygon.push_back(itormapPolygonsByY->second);
						_Polygon polygon = itormapPolygonsByY->second;
						//从总的Polygon集合里删除选中的Polygon
						itorMapPolygon = find_if(mapPolygon.begin(),mapPolygon.end(),map_finder(polygon));
						if (itorMapPolygon != mapPolygon.end())
						{
							mapPolygon.erase(itorMapPolygon);
						}					
						//从top区间内的polygon集合里删除选中的Polygon
						if (itormapPolygonsByY != mapPolygonsByY.end())
						{
							mapPolygonsByY.erase(itormapPolygonsByY++);
						}						
						std::vector<_Polygon>::iterator iteratorVec = find_if(vecPolygons.begin(),vecPolygons.end(),vector_finder(polygon));
						if (iteratorVec != vecPolygons.end())
						{
							vecPolygons.erase(iteratorVec);
						}						
					}
					else
					{
						itormapPolygonsByY++;
					}
				}

				if (cluster.vecPolygon.size() > 0)
				{
					mapCluster.insert(std::make_pair<int,Cluster>(clusterIndex++,cluster));	
				} 

// 				if (mapCluster.size() >6)
// 				{
// 					//return;
// 				}
			}
		}

		//如果所有图形都被分配完了，结束递归
		if (mapPolygon.empty())
		{
			return;
		}

		CalculateFovPath(mapCluster,clusterIndex,mapPolygon,ViewWidth,ViewHeight);
	}
    #pragma endregion FOV路径计算
	void CGxmcGerber::SelectPads(std::list<DataBlock>& dataBlockList,std::vector<GxmcRect> vecSelectRect,ScreenStruct screen,bool isInvertSelection)
	{
		if (dataBlockList.empty())
		{
			return;
		}

		std::vector<GxmcRect>::iterator itorRect = vecSelectRect.begin();
		for (; itorRect != vecSelectRect.end(); itorRect++)
		{
			RectD rect(itorRect->m_startPoint.x,itorRect->m_stopPoint.y,itorRect->m_stopPoint.x,itorRect->m_startPoint.y);
			if (rect.width() == 0|| rect.height() == 0)
			{
				continue;
			}

			std::list<DataBlock>::iterator itor = dataBlockList.begin();
			for ( ; itor != dataBlockList.end();itor++)
			{
				DataBlock::iterator itorDatablock = (*itor).begin();
				for ( ; itorDatablock != (*itor).end();itorDatablock++)
				{
					(*itorDatablock)->GetBoundaryRect((*itorDatablock)->m_marginRect);
					if (isInvertSelection)
					{
						if (!(*itorDatablock)->m_marginRect.IsRectInRect(rect))
						{
							(*itorDatablock)->SetColour(RGB(255,255,255),RGB(255,255,255));							
						}
					}
					else
					{
						if ((*itorDatablock)->m_marginRect.IsRectInRect(rect))
						{
							(*itorDatablock)->SetColour(RGB(255,255,255),RGB(255,255,255));
						}
					}	
				}
			}
		}

	}
	void CGxmcGerber::HidePads(std::list<DataBlock>& dataBlockList,std::vector<GxmcRect> vecSelectRect,ScreenStruct screen,bool isInvertSelection)
	{
		if (dataBlockList.empty())
		{
			return;
		}
		
		std::vector<GxmcRect>::iterator itorRect = vecSelectRect.begin();
		for (; itorRect != vecSelectRect.end(); itorRect++)
		{
			RectD rect(itorRect->m_startPoint.x,itorRect->m_stopPoint.y,itorRect->m_stopPoint.x,itorRect->m_startPoint.y);
			if (rect.width() == 0|| rect.height() == 0)
			{
				continue;
			}

			std::list<DataBlock>::iterator itor = dataBlockList.begin();
			for ( ; itor != dataBlockList.end();itor++)
			{
				DataBlock::iterator itorDatablock = (*itor).begin();
				for ( ; itorDatablock != (*itor).end();itorDatablock++)
				{
					(*itorDatablock)->GetBoundaryRect((*itorDatablock)->m_marginRect);
					if (isInvertSelection)
					{
						if (!(*itorDatablock)->m_marginRect.IsRectInRect(rect))
						{
							(*itorDatablock)->m_bIsHidden = true; 
						}
					}
					else
					{
						if ((*itorDatablock)->m_marginRect.IsRectInRect(rect))
						{
							(*itorDatablock)->m_bIsHidden = true; 
						}
					}	
				}
			}
		}
	}
	void CGxmcGerber::SelectMarkPoints(std::list<DataBlock>& dataBlockList,std::vector<GxmcRect> vecSelectRect,ScreenStruct screen,std::map<int,_Polygon>& mapMarkPoint)
	{
		if (dataBlockList.empty())
		{
			return;
		}

		int clusterIndex = 0;
		std::vector<GxmcRect>::iterator itorRect = vecSelectRect.begin();
		for (; itorRect != vecSelectRect.end(); itorRect++)
		{
			RectD rect(itorRect->m_startPoint.x,itorRect->m_stopPoint.y,itorRect->m_stopPoint.x,itorRect->m_startPoint.y);
			if (rect.width() == 0|| rect.height() == 0)
			{
				continue;
			}
			
			std::list<DataBlock>::iterator itor = dataBlockList.begin();
			for ( ; itor != dataBlockList.end();itor++)
			{
				_Polygon polygon;
				DataBlock::iterator itorDatablock = (*itor).begin();
				for ( ; itorDatablock != (*itor).end();itorDatablock++)
				{
					(*itorDatablock)->GetBoundaryRect((*itorDatablock)->m_marginRect);					
					if ((*itorDatablock)->m_marginRect.IsRectInRect(rect))
					{
						polygon.margin = (*itorDatablock)->m_marginRect;
						(*itorDatablock)->SetColour(RGB(255,0,0),RGB(255,0,0));
						polygon.dataBlock.push_back(*itorDatablock);
			            (*itorDatablock)->m_bIsHidden = true;
						break;
					}
				}
				if (!polygon.dataBlock.empty())
				{
					mapMarkPoint.insert(std::make_pair<int,_Polygon>(clusterIndex++,polygon));
					break;	
				}	
			}
		}
	}
	void CGxmcGerber::DisplaySelectRect(CDC& dc,ScreenStruct screen,std::vector<GxmcRect> vecSelectRect)
	{
		if (vecSelectRect.empty())
		{
			return;
		}
		std::vector<GxmcRect>::iterator itor = vecSelectRect.begin();
		for (; itor != vecSelectRect.end(); itor++)
		{
			itor->Draw(dc,screen);
		}

	}
	void CGxmcGerber::ExportFovData(const std::list<DataBlock>& dataBlockList,const std::map<int,Cluster>& mapCluster,const std::map<int,_Polygon>& mapMarkPoint)
	{
		if (mapCluster.empty())
		{
			AfxMessageBox(L"先计算Fov路径！");
			return;
		}
		if (mapMarkPoint.empty())
		{
			AfxMessageBox(L"选择Mark点！");
			return;
		}

		TCHAR szFilter[] = L"Access文件(*.mdb)|*.mdb|所有文件(*.*)|*.*||";
		CFileDialog fileDlg(true,L"mdb",L"Gxmc",0,szFilter);
		CString strFilePath;
		if (fileDlg.DoModal() == IDOK)
		{
			strFilePath = fileDlg.GetFolderPath() + "\\" + fileDlg.GetFileName();
		}
		else
		{
			return;
		}

		m_dataBase.CreateTable(strFilePath,CGxmcDatabase::s_vecTableStyle);

		int nFovNum = -1;
		int nPadID = -1;
		int nArcPolygonID = -1;
		int nPolygonID = -1;
		int nArcID = -1;
		std::map<int,Cluster>::const_iterator itorMapCluster = mapCluster.begin();
		for ( ;itorMapCluster != mapCluster.end();itorMapCluster++)
		{
			//导出FovPath
			nFovNum++;
			CGxmcDatabase::FovPath fovPath;
			fovPath.FovNo = nFovNum;
			fovPath.BlockID = nFovNum;
			fovPath.PosX = itorMapCluster->second.centerPoint.x;
			fovPath.PosY = itorMapCluster->second.centerPoint.y;
			fovPath.SizeX = itorMapCluster->second.clusterMargin->m_stopPoint.x - itorMapCluster->second.clusterMargin->m_startPoint.x;
			fovPath.SizeY = itorMapCluster->second.clusterMargin->m_stopPoint.y - itorMapCluster->second.clusterMargin->m_startPoint.y;
			m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.FovPath,CGxmcDatabase::s_fovPathElement,fovPath);

			//导出Pad
			std::vector<_Polygon>::const_iterator itorPolygon = itorMapCluster->second.vecPolygon.begin();
			for (;itorPolygon != itorMapCluster->second.vecPolygon.end();itorPolygon++)
			{
				CGxmcDatabase::Pad pad;
				CGxmcDatabase::PadElement padElement;
				nPadID++;
				pad.FovNo = nFovNum;
				pad.ID = nPadID;
				pad.PadID = nPadID;
				pad.PosXmm = (itorPolygon->margin.left + itorPolygon->margin.right)/2;
				pad.PosYmm = (itorPolygon->margin.bottom + itorPolygon->margin.top)/2;
				pad.ShapeID = nPadID;
				m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_padElement,pad);

				int shapeItemIndex =-1;
				std::list<GxmcShape*>::const_iterator itorpShape = itorPolygon->dataBlock.begin();
				for (;itorpShape != itorPolygon->dataBlock.end();itorpShape++)
				{
					CGxmcDatabase::ShapeItem shapeItem;
					CGxmcDatabase::ShapeItemElement shapeItemElement;
					shapeItem.shapeID = ++nArcPolygonID;
					shapeItem.arcID = -1;
					shapeItem.polygonID = -1;
					shapeItem.DCode = "  -1";

					string strShapeStyle = (*itorpShape)->GetShapeStyle();
					if (strShapeStyle == "ACircle")
					{
						shapeItem.DCode = dynamic_cast<ACircle*>(*itorpShape)->m_strDCode;
					}
					if (strShapeStyle == "AObround")
					{
						shapeItem.DCode = dynamic_cast<AObround*>(*itorpShape)->m_strDCode;
					}
					if (strShapeStyle == "ARectangle")
					{
						shapeItem.DCode = dynamic_cast<ARectangle*>(*itorpShape)->m_strDCode;
					}
					if (strShapeStyle == "ARegularPolygon")
					{
						shapeItem.DCode = dynamic_cast<ARegularPolygon*>(*itorpShape)->m_strDCode;
					}
					if (strShapeStyle == "AMShape")
					{
						shapeItem.nIndex = ++shapeItemIndex;
						shapeItem.DCode = dynamic_cast<AMShape*>(*itorpShape)->m_strDCode;
					}

					if (strShapeStyle == "CGxmcArcPolygon")
					{
						GxmcPolygon* pGxmcPolygon = dynamic_cast<GxmcPolygon*>(*itorpShape);
						if (!pGxmcPolygon->m_vecArc.empty())
						{
							shapeItem.arcID = ++nArcID;
							int arcIndex = -1;
							int size = pGxmcPolygon->m_vecArc.size();
							for ( int i = 0; i < size;i++)
							{
								shapeItem.nIndex = ++shapeItemIndex;
								CGxmcDatabase::Shape_Arc arc;
								CGxmcDatabase::Shape_ArcElement arcElement;
								arc.nIndex = ++arcIndex;
								arc.arcID = nArcID;
								arc.arcDirection = pGxmcPolygon->m_vecArc[i].m_nArcDirection;
								arc.startPointX = pGxmcPolygon->m_vecArc[i].m_startPoint.x;
								arc.startPointY = pGxmcPolygon->m_vecArc[i].m_startPoint.y;
								arc.stopPointX = pGxmcPolygon->m_vecArc[i].m_stopPoint.x;
								arc.stopPointY = pGxmcPolygon->m_vecArc[i].m_stopPoint.y;
								arc.centerPointX = pGxmcPolygon->m_vecArc[i].m_CenterPoint.x;
								arc.centerPointY = pGxmcPolygon->m_vecArc[i].m_CenterPoint.y;
								arc.radius = pGxmcPolygon->m_vecArc[i].m_Radius;

								m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.Shape_Arc,CGxmcDatabase::s_arcElement,arc);
							}
						}
						if (!pGxmcPolygon->m_vecLine.empty())
						{

						}
						if (!pGxmcPolygon->m_vecPoint.empty())
						{
							shapeItem.polygonID = ++nPolygonID;
							int polygonIndex = -1;
							CGxmcDatabase::Shape_Polygon polygon;
							CGxmcDatabase::Shape_PolygonElement polygonElement;
							int size = pGxmcPolygon->m_vecPoint.size();
							for ( int i = 0; i < size;i++)
							{
								shapeItem.nIndex = ++shapeItemIndex;
								polygon.nIndex = ++polygonIndex;
								polygon.polygonID = nPolygonID;
								polygon.pointX = pGxmcPolygon->m_vecPoint[i].x;
								polygon.pointY = pGxmcPolygon->m_vecPoint[i].y;

								m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.Shape_Polygon,CGxmcDatabase::s_polygonElement,polygon);
							}
						}			
					}
					m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.ShapeItem,CGxmcDatabase::s_shapeItemElement,shapeItem);
				}
			}		
		}
		//return;

		//导出Mark点
		std::map<int,_Polygon>::const_iterator itorMapMarkPoint = mapMarkPoint.begin();
		for ( ;itorMapMarkPoint != mapMarkPoint.end();itorMapMarkPoint++)
		{
			CGxmcDatabase::Mark mark;
			mark.BlockID = itorMapMarkPoint->first;
			mark.ID = itorMapMarkPoint->first;
			mark.PosXmm = (itorMapMarkPoint->second.margin.left + itorMapMarkPoint->second.margin.right)/2;
			mark.PosYmm = (itorMapMarkPoint->second.margin.bottom + itorMapMarkPoint->second.margin.top)/2;
			mark.SizeXmm = itorMapMarkPoint->second.margin.right - itorMapMarkPoint->second.margin.left;
			mark.SizeYmm = itorMapMarkPoint->second.margin.top - itorMapMarkPoint->second.margin.bottom;
			m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.Mark,CGxmcDatabase::s_markElement,mark);
		}

		//导出焊盘Shape-----------AD
		int nAdIndex = 0;
		int nCircleID = -1;
		int nVectorLineID = -1;
		int nCenterLineID = -1;
		int nOutLineID = -1;
		int nPolygonAmID = -1;
		int nMoireID = -1;
		int nThermalID = -1;
		std::map<std::string,AdStyle>::iterator itorMapAdstyle =  m_adStyleMap.begin();
		for ( ;itorMapAdstyle != m_adStyleMap.end();itorMapAdstyle++)
		{
// 			CGxmcDatabase::Shape shape;
// 			shape.ShapeID = nAdIndex++;
// 			shape.SizeXmm = itorMapAdstyle->second.adName;
			switch (itorMapAdstyle->second.adName)
			{
			case Circle:
				{
					CGxmcDatabase::AdCircle adCircle;
					CGxmcDatabase::AdCircleElement adCircleElement;
					adCircle.DCode = itorMapAdstyle->first.c_str();
					adCircle.Diameter = itorMapAdstyle->second.adInfo.adCircle.Diameter;
					adCircle.HoleDiameter = itorMapAdstyle->second.adInfo.adCircle.HoleDiameter;
					m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdCircle,CGxmcDatabase::s_adCircleElement,adCircle);
				}
				break;
			case Rectangle:
				{
					CGxmcDatabase::AdRectangle adRectangle;
					CGxmcDatabase::AdRectangleElement adRectangleElement;
					adRectangle.DCode = itorMapAdstyle->first;
					adRectangle.Width = itorMapAdstyle->second.adInfo.adRectangle.Width;
					adRectangle.Height = itorMapAdstyle->second.adInfo.adRectangle.Height;
					adRectangle.HoleDiameter = itorMapAdstyle->second.adInfo.adRectangle.HoleDiameter;
					m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdRectangle,CGxmcDatabase::s_adRectangleElement,adRectangle);
				}
				break;
			case Obround:
				{
					CGxmcDatabase::AdObround adObround;
					CGxmcDatabase::AdObroundElement adObroundElement;
					adObround.DCode = itorMapAdstyle->first;
					adObround.Width = itorMapAdstyle->second.adInfo.adObround.Width;
					adObround.Height = itorMapAdstyle->second.adInfo.adObround.Height;
					adObround.HoleDiameter = itorMapAdstyle->second.adInfo.adObround.HoleDiameter;
					m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdObround,CGxmcDatabase::s_adObroundElement,adObround);
				}
				break;
			case RegularPolygon:
				{
 					//CGxmcDatabase::AdRegularPolygon adRegularPolygon;
 					//adRegularPolygon.DCode = itorMapAdstyle->first;
 					//adRegularPolygon.numVertice = itorMapAdstyle->second.adInfo.adRegularPolygon.numVertice;
 					//adRegularPolygon.OuterDiameter = itorMapAdstyle->second.adInfo.adRegularPolygon.OuterDiameter;
 					//adRegularPolygon.HoleDiameter = itorMapAdstyle->second.adInfo.adRegularPolygon.HoleDiameter;
 					//adRegularPolygon.RotateAngle = itorMapAdstyle->second.adInfo.adRegularPolygon.RotateAngle;
					//m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdRegularPolygon,CGxmcDatabase::s_adRegularPolygonElement,adRegularPolygon);
				}
				break;
			case Am:
				{
					CGxmcDatabase::AdAm adAm;
					CGxmcDatabase::AdAmElement adAmElement;
					adAm.DCode = itorMapAdstyle->first;
					adAm.CircleID = -1;
					adAm.CenterLineID = -1;
					adAm.VectorLineID = -1;
					adAm.OutLineID = -1;
					adAm.PolygonID = -1;
					adAm.MoireID = -1;
					adAm.ThermalID = -1;
					//m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,adAm);
					
					/*adam = itormapadstyle->second.adinfo.adaminfo.width;
					adam.height = itormapadstyle->second.adinfo.adaminfo.height;
					adam.holediameter = itormapadstyle->second.adinfo.adaminfo->holediameter;*/
					
					if (!itorMapAdstyle->second.adInfo.adAmInfo->vecAmCircle.empty())
					{
						adAm.CircleID = ++nCircleID;
						for (unsigned int i = 0; i<itorMapAdstyle->second.adInfo.adAmInfo->vecAmCircle.size();i++)
						{
							CGxmcDatabase::AmCircle amCircle;
							amCircle.CircleID = nCircleID;
							amCircle.centerX = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCircle[i].centerPoint->x;
							amCircle.centerY = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCircle[i].centerPoint->y;
							amCircle.diameter = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCircle[i].diameter;
							amCircle.exposure = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCircle[i].exposure;
							amCircle.rotAngle = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCircle[i].rotationAngle;

							m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AmCircle,CGxmcDatabase::s_amCircleElement,amCircle);
							//m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,adAm);
						}
						
					}
					if (!itorMapAdstyle->second.adInfo.adAmInfo->vecAmCenterLine.empty())
					{
						adAm.CenterLineID = ++nCenterLineID;
						for (unsigned int i = 0; i<itorMapAdstyle->second.adInfo.adAmInfo->vecAmCenterLine.size();i++)
						{
							CGxmcDatabase::AmCenterLine amCenterLine;
							amCenterLine.CenterLineID = nCenterLineID;
							amCenterLine.centerX = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCenterLine[i].centerPoint->x;
							amCenterLine.centerY = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCenterLine[i].centerPoint->y;
							amCenterLine.rectHeight = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCenterLine[i].rectangleHeight;
							amCenterLine.rectWidth = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCenterLine[i].rectangleWidth;
							amCenterLine.exposure = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCenterLine[i].exposure;
							amCenterLine.rotAngle = itorMapAdstyle->second.adInfo.adAmInfo->vecAmCenterLine[i].rotationAngle;
					
							m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AmCenterLine,CGxmcDatabase::s_amCenterLineElement,amCenterLine);
							//m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,adAm);
						}
					}
					if (!itorMapAdstyle->second.adInfo.adAmInfo->vecAmVectorLine.empty())
					{
						adAm.VectorLineID = ++nVectorLineID;
						for (unsigned int i = 0; i<itorMapAdstyle->second.adInfo.adAmInfo->vecAmVectorLine.size();i++)
						{
							CGxmcDatabase::AmVectorLine amVectorLine;
							amVectorLine.VectorLineID = nVectorLineID;
							amVectorLine.startX = itorMapAdstyle->second.adInfo.adAmInfo->vecAmVectorLine[i].startPoint->x;
							amVectorLine.startY = itorMapAdstyle->second.adInfo.adAmInfo->vecAmVectorLine[i].startPoint->y;
							amVectorLine.stopX = itorMapAdstyle->second.adInfo.adAmInfo->vecAmVectorLine[i].stopPoint->x;
							amVectorLine.stopY = itorMapAdstyle->second.adInfo.adAmInfo->vecAmVectorLine[i].stopPoint->y;
							amVectorLine.lineWidth = itorMapAdstyle->second.adInfo.adAmInfo->vecAmVectorLine[i].lineWidth;
							amVectorLine.exposure = itorMapAdstyle->second.adInfo.adAmInfo->vecAmVectorLine[i].exposure;
							amVectorLine.rotAngle = itorMapAdstyle->second.adInfo.adAmInfo->vecAmVectorLine[i].rotationAngle;

							m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AmVectorLine,CGxmcDatabase::s_amVectorLineElement,amVectorLine);
							//m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,adAm);
						}
					}
					if (!itorMapAdstyle->second.adInfo.adAmInfo->vecAmOutLine.empty())
					{
						adAm.OutLineID = ++nOutLineID;
						for (unsigned int i = 0; i<itorMapAdstyle->second.adInfo.adAmInfo->vecAmOutLine.size();i++)
						{
							CGxmcDatabase::AmOutLine amOutLine;
							amOutLine.OutLineID = nOutLineID;
							amOutLine.pointX = itorMapAdstyle->second.adInfo.adAmInfo->vecAmOutLine[i].pointList[0].x;
							amOutLine.pointY = itorMapAdstyle->second.adInfo.adAmInfo->vecAmOutLine[i].pointList[0].x;
							amOutLine.numVertex = itorMapAdstyle->second.adInfo.adAmInfo->vecAmOutLine[i].numVertex;
							amOutLine.exposure = itorMapAdstyle->second.adInfo.adAmInfo->vecAmOutLine[i].exposure;
							amOutLine.rotAngle = itorMapAdstyle->second.adInfo.adAmInfo->vecAmOutLine[i].rotationAngle;

							m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AmOutLine,CGxmcDatabase::s_amOutLineElement,amOutLine);
							//m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,adAm);
						}
					}
					if (!itorMapAdstyle->second.adInfo.adAmInfo->vecAmPolygon.empty())
					{
						adAm.PolygonID = ++nPolygonAmID;
						for (unsigned int i = 0; i<itorMapAdstyle->second.adInfo.adAmInfo->vecAmPolygon.size();i++)
						{
							CGxmcDatabase::AmPolygon amPolygon;
							amPolygon.PolygonID = nPolygonAmID;
							amPolygon.centerX = itorMapAdstyle->second.adInfo.adAmInfo->vecAmPolygon[i].centerPoint->x;
							amPolygon.centerY = itorMapAdstyle->second.adInfo.adAmInfo->vecAmPolygon[i].centerPoint->y;
							amPolygon.diameter = itorMapAdstyle->second.adInfo.adAmInfo->vecAmPolygon[i].diameter;
							amPolygon.numVertex = itorMapAdstyle->second.adInfo.adAmInfo->vecAmPolygon[i].numVertex;
							amPolygon.exposure = itorMapAdstyle->second.adInfo.adAmInfo->vecAmPolygon[i].exposure;
							amPolygon.rotAngle = itorMapAdstyle->second.adInfo.adAmInfo->vecAmPolygon[i].rotationAngle;

							m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AmPolygon,CGxmcDatabase::s_amPolygonElement,amPolygon);
							//m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,adAm);
						}
					}
					if (!itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire.empty())
					{
						adAm.MoireID = ++nMoireID;
						for (unsigned int i = 0; i<itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire.size();i++)
						{
							CGxmcDatabase::AmMoire amMoire;
							amMoire.MoireID = nMoireID;
							amMoire.centerX = itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire[i].centerPoint->x;
							amMoire.centerY = itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire[i].centerPoint->y;
							amMoire.crosshairLength = itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire[i].crosshairLength;
							amMoire.crosshairThickness = itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire[i].crosshairThickness;
							amMoire.outerDiameter = itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire[i].outerDiameter;
							amMoire.ringGap = itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire[i].ringGap;
							amMoire.ringMaxNum = itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire[i].ringMaxNum;
							amMoire.ringThickness = itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire[i].ringThickness;
							amMoire.rotAngle = itorMapAdstyle->second.adInfo.adAmInfo->vecAmMoire[i].rotationAngle;

							m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AmMoire,CGxmcDatabase::s_amMoireElement,amMoire);
							//m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,adAm);
						}
					}
					if (!itorMapAdstyle->second.adInfo.adAmInfo->vecAmThermal.empty())
					{
						adAm.ThermalID = ++nThermalID;
						for (unsigned int i = 0; i<itorMapAdstyle->second.adInfo.adAmInfo->vecAmThermal.size();i++)
						{
							CGxmcDatabase::AmThermal amThermal;
							amThermal.ThermalID = nThermalID;
							amThermal.centerX = itorMapAdstyle->second.adInfo.adAmInfo->vecAmThermal[i].centerPoint->x;
							amThermal.centerY = itorMapAdstyle->second.adInfo.adAmInfo->vecAmThermal[i].centerPoint->y;
							amThermal.gapThickness = itorMapAdstyle->second.adInfo.adAmInfo->vecAmThermal[i].gapThickness;
							amThermal.innerDiameter = itorMapAdstyle->second.adInfo.adAmInfo->vecAmThermal[i].innerDiameter;
							amThermal.outerDiameter = itorMapAdstyle->second.adInfo.adAmInfo->vecAmThermal[i].outerDiameter;							
							amThermal.rotAngle = itorMapAdstyle->second.adInfo.adAmInfo->vecAmThermal[i].rotationAngle;

							m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AmThermal,CGxmcDatabase::s_amThermalElement,amThermal);
							//m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,adAm);
						}
					}
					m_dataBase.AddNewRecordToTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,adAm);
				}
				break;
			default:
				break;
			}
		}
	}
	//加载数据库
	int CGxmcGerber::ReadFromDatabase()
	{
		TCHAR szFilter[] = L"Access文件(*.mdb)|*.mdb|所有文件(*.*)|*.*||";
		CFileDialog fileDlg(true,L"mdb",L"Gxmc",0,szFilter);
		CString strFilePath;
		if (fileDlg.DoModal() == IDOK)
		{
			strFilePath = fileDlg.GetFolderPath() + "\\" + fileDlg.GetFileName();
		}
		else
		{
			return -1;
		}
		m_dataBase.ConnectAdo(strFilePath);

		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.Mark,CGxmcDatabase::s_markElement,m_vecMark);
		
		//读取FOV路径表
		vector<CGxmcDatabase::FovPath> vecFovPath;
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.FovPath,CGxmcDatabase::s_fovPathElement,vecFovPath);
		//读取光圈参数AD 表
		std::map<string,Gxmc_Gerber::AdStyle> ADStyleMap;
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.AdCircle,CGxmcDatabase::s_adCircleElement,ADStyleMap);
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.AdObround,CGxmcDatabase::s_adObroundElement,ADStyleMap);
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.AdRectangle,CGxmcDatabase::s_adRectangleElement,ADStyleMap);
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.AdRegularPolygon,CGxmcDatabase::s_adRegularPolygonElement,ADStyleMap);
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.AdAm,CGxmcDatabase::s_adAmElement,ADStyleMap);
		//读取光圈宏AM

		//读取形状信息表
		map<int,vector<CGxmcDatabase::Shape_Arc>> mapShapeArc;
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.Shape_Arc,CGxmcDatabase::s_arcElement,mapShapeArc);
		map<int,vector<Gxmc_Draw::PointD>> mapShapePolygon;
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.Shape_Polygon,CGxmcDatabase::s_polygonElement,mapShapePolygon);
		//读取ShapeItem表
		map<int,CGxmcDatabase::ShapeItem> mapShapeItem;
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.ShapeItem,CGxmcDatabase::s_shapeItemElement,mapShapeItem);
		//读取焊盘表
		map<int,CGxmcDatabase::Pad> mapPad;
		m_dataBase.ReadRecordFromTable(CGxmcDatabase::tableList.Pad,CGxmcDatabase::s_padElement,mapPad);

		//pad不填充
		bool isFilled = false;
		m_clustersMargin = INITI_RECT;//初始化clusters边框
		vector<CGxmcDatabase::FovPath>::iterator itorFovPath = vecFovPath.begin();
		for ( ;itorFovPath != vecFovPath.end();itorFovPath++)
		{
			PadCluster cluster;
			int nFov = itorFovPath->FovNo;
			cluster.centerPoint.x = itorFovPath->PosX;
			cluster.centerPoint.y = itorFovPath->PosY;
			PointD startPoint,stopPoint;
			startPoint.x = itorFovPath->PosX - (itorFovPath->SizeX)/2;
			startPoint.y = itorFovPath->PosY - (itorFovPath->SizeY)/2;
			stopPoint.x = itorFovPath->PosX + (itorFovPath->SizeX)/2;
			stopPoint.y = itorFovPath->PosY + (itorFovPath->SizeY)/2;
			cluster.clusterMargin = new GxmcRect(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,isFilled,startPoint,stopPoint);
			UpdateMargin(RectD(startPoint.x,stopPoint.y,stopPoint.x,startPoint.y),m_clustersMargin);//更新clusters边框
			
			map<int,CGxmcDatabase::Pad>::iterator itorPad = mapPad.begin();
			for (;itorPad != mapPad.end();itorPad++)
			{
				//读取FOV下的所有焊盘
				if (itorPad->second.FovNo == nFov)
				{
					_Polygon pad;
					if (itorPad->first > 2755)
					{
						int nb =9;
					}

					int nShapeID = itorPad->second.ShapeID;
					int nArcID = mapShapeItem[nShapeID].arcID;
					if (nArcID != -1)
					{
						GxmcPolygon* pShape= new GxmcPolygon(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,isFilled);
						for (int i =0;i<mapShapeArc[nArcID].size();i++)
						{
							PointD centerPoint,startPoint,stopPoint;
							startPoint.x = mapShapeArc[nArcID][i].startPointX;
							startPoint.y = mapShapeArc[nArcID][i].startPointY;
							stopPoint.x = mapShapeArc[nArcID][i].stopPointX;
							stopPoint.y = mapShapeArc[nArcID][i].stopPointY;
							centerPoint.x = mapShapeArc[nArcID][i].centerPointX;
							centerPoint.y = mapShapeArc[nArcID][i].centerPointY;
							double radius = mapShapeArc[nArcID][i].radius;
							int nArcDirection = mapShapeArc[nArcID][i].arcDirection;
							GxmcArc arc = GxmcArc(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,isFilled,
								                centerPoint,radius,startPoint,stopPoint,nArcDirection); 
							pShape->m_vecArc.push_back(arc);
						}
						pad.dataBlock.push_back(pShape);					
					}
					int nPolygonID = mapShapeItem[nShapeID].polygonID;
					if (nPolygonID != -1)
					{
						GxmcPolygon* pShape= new GxmcPolygon(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,isFilled);
						PointD startPoint,stopPoint;
						for (int i = 0;i<mapShapePolygon[nPolygonID].size();i++)
						{
							PointD point;
							point.x = mapShapePolygon[nPolygonID][i].x;
							point.y = mapShapePolygon[nPolygonID][i].y;
							if (i==0)
							{
								point.isMoveto = true;
								startPoint = point;
							}
							else
							{
								startPoint = stopPoint;
							}		
							stopPoint  = point;
							GxmcLine line = GxmcLine(m_penWidth, m_penStyle, m_penColor,m_brushColor, m_nLayer,isFilled,startPoint,stopPoint);
							pShape->m_vecLine.push_back(line);
						}
						pad.dataBlock.push_back(pShape);
					}
					string DCode = mapShapeItem[nShapeID].DCode;
					if (DCode != "  -1")
					{
						PointD centerPoint;
						centerPoint.x = itorPad->second.PosXmm;
						centerPoint.y = itorPad->second.PosYmm;
						switch (ADStyleMap[DCode].adName)
						{
						case Circle:
							{
								ACircle* pShape = new ACircle(m_penWidth, m_penStyle, m_penColor,m_brushColor,
									                 m_nLayer,isFilled,centerPoint,ADStyleMap[DCode].adInfo.adCircle,DCode);
								pad.dataBlock.push_back(pShape);
							}
							break;
						case Obround:
							{
								AObround*  pShape = new AObround(m_penWidth, m_penStyle, m_penColor,m_brushColor, 
									                   m_nLayer,isFilled,centerPoint,ADStyleMap[DCode].adInfo.adObround,DCode);
								pad.dataBlock.push_back(pShape);
							}
							break;
						case Rectangle:
							{
								ARectangle*  pShape = new ARectangle(m_penWidth, m_penStyle, m_penColor,m_brushColor, 
									                     m_nLayer,isFilled,centerPoint,ADStyleMap[DCode].adInfo.adRectangle,DCode);
								pad.dataBlock.push_back(pShape);
							}
							break;
						case RegularPolygon:
							{
								ARegularPolygon*  pShape = new ARegularPolygon(m_penWidth, m_penStyle, m_penColor,m_brushColor, 
									                         m_nLayer,isFilled,centerPoint,ADStyleMap[DCode].adInfo.adRegularPolygon,DCode);
								pad.dataBlock.push_back(pShape);
							}
							break;
						case Am:
							{
 								Gxmc_Gerber::AMShape* pShape = new AMShape(m_penWidth, m_penStyle, m_penColor,m_brushColor, 
 									                             m_nLayer,isFilled,centerPoint,*(ADStyleMap[DCode].adInfo.adAmInfo),DCode);
 								pad.dataBlock.push_back(pShape);
							}
							break;
						default:
							break;
						}
					}
					cluster.mapPolygon.insert(make_pair(nShapeID,pad));	
				}	
			}
            m_mapPadCluster.insert(make_pair(nFov,cluster));	
		}

		/////////////////////////////////
		//释放 AD_AmInfo new的内存
		std::map<string,Gxmc_Gerber::AdStyle>::iterator itorAdStyle = ADStyleMap.begin();
		for (;itorAdStyle != ADStyleMap.end();itorAdStyle++)
		{
			if (itorAdStyle->second.adName == Am)
			{
				delete itorAdStyle->second.adInfo.adAmInfo;
				itorAdStyle->second.adInfo.adAmInfo = NULL;
			}		
		}

		return 1;
	}

	void CGxmcGerber::DisplayMarkPoint(CDC& dc,ScreenStruct screen,const vector<CGxmcDatabase::Mark>& vecMark)
	{	
		vector<CGxmcDatabase::Mark>::const_iterator itorMark = vecMark.begin();
		for (;itorMark != vecMark.end();itorMark++)
		{
			PointD centerPoint(itorMark->PosXmm,itorMark->PosYmm);
			double radius = 2;
			GxmcCircle circle(m_penWidth,m_penStyle, m_penColor,m_brushColor,m_nLayer,m_bIsFilled,centerPoint,radius);
			circle.SetColour(RGB(255,0,0),RGB(255,0,0));
			circle.Draw(dc,screen);
			circle.SetColour(m_penColor,m_brushColor);
		}
	}

	vector<CGxmcDatabase::Mark> CGxmcGerber::GetMarkPoints()
	{
		return m_vecMark;
	}

	std::map<int,PadCluster> CGxmcGerber::GetPadCluster()
	{
		return m_mapPadCluster;
	}
	RectD CGxmcGerber::GetClustersMargin()
	{
		return m_clustersMargin;
	}
	void CGxmcGerber::DisplayPadCluster(CDC& dc,ScreenStruct screen,const map<int,PadCluster>& mapPadCluster)
	{
		//const_iterator迭代器可修改，迭代器指向的数据不能修改
		map<int,PadCluster>::const_iterator itorPadCluster = mapPadCluster.begin();
		for (;itorPadCluster != mapPadCluster.end();itorPadCluster++)
		{
			// 			if (itorPadCluster->first != mapPadCluster.size()-1)
			// 			{
			// 				continue;
			// 			}
			//绘制Fov边框
			itorPadCluster->second.clusterMargin->Draw(dc,screen);
			//绘制Fov里的焊盘
			map<int,_Polygon>::const_iterator itorPad = itorPadCluster->second.mapPolygon.begin();
			for (;itorPad != itorPadCluster->second.mapPolygon.end();itorPad++)
			{		
				// 				if (itorPad->first != 2756 && itorPad->first != 2757)
				// 				{
				// 					continue;
				// 				}
				DataBlock::const_iterator itorShape = itorPad->second.dataBlock.begin();
				for (;itorShape != itorPad->second.dataBlock.end();itorShape++)
				{
					(*itorShape)->Draw(dc,screen);
				}	
			}
		}
	}
	void CGxmcGerber::DisplayPadCluster(CDC& dc,ScreenStruct screen,const PadCluster& PadCluster)
	{
		CPen pen(PS_SOLID, 1, RGB(255,0,0));
		CPen* pOldPen = dc.SelectObject(&pen);
		PadCluster.clusterMargin->Draw(dc,screen);
		dc.SelectObject(pOldPen);

		//绘制Fov里的焊盘
		map<int,_Polygon>::const_iterator itorPad = PadCluster.mapPolygon.begin();
		for (;itorPad != PadCluster.mapPolygon.end();itorPad++)
		{		
			DataBlock::const_iterator itorShape = itorPad->second.dataBlock.begin();
			for (;itorShape != itorPad->second.dataBlock.end();itorShape++)
			{
				(*itorShape)->Draw(dc,screen);
			}	
		}
	}

	void CGxmcGerber::ReleasePadCluster()
	{
		map<int,PadCluster>::iterator itorPadCluster = m_mapPadCluster.begin();
		for (;itorPadCluster != m_mapPadCluster.end();)
		{
			//删除cluster边界框指针
			delete itorPadCluster->second.clusterMargin;
			itorPadCluster->second.clusterMargin = NULL;
			//绘制Fov里的焊盘
			map<int,_Polygon>::iterator itorPad = itorPadCluster->second.mapPolygon.begin();
			for (;itorPad != itorPadCluster->second.mapPolygon.end();)
			{		
				DataBlock::iterator itorShape = itorPad->second.dataBlock.begin();	
				for (;itorShape != itorPad->second.dataBlock.end();)
				{
					delete *itorShape;
					*itorShape = NULL;
					itorPad->second.dataBlock.erase(itorShape++);
				}

				itorPadCluster->second.mapPolygon.erase(itorPad++);
			}
			m_mapPadCluster.erase(itorPadCluster++);
		}		
	}
	
    #pragma region 相关正则表达式函数
	bool CGxmcGerber::RegexSearch(std::string source,std::string pattern,std::vector<std::string>& vecMatch)
	{
		bool rst = false;
		try
		{
			std::regex regexpattern(pattern);
			std::smatch match;
			rst = std::regex_search(source,match,regexpattern);
			//存储前保证vector是空的
			std::vector<std::string> vecMatch0;
			vecMatch.swap(vecMatch0);
			if (rst)
			{
				for (unsigned int i = 1; i < match.size();i++)
				{
					vecMatch.push_back(match[i].str());
				}
			}
			return rst;
		}
		catch(const std::regex_error& e)
		{
			CString str(e.what());
			std::regex_constants::error_type err =e.code();
			AfxMessageBox(str);
			return rst;
		}	
	}

	bool CGxmcGerber::RegexReplace(std::string source,std::string pattern,std::string fmt,std::string& dest)
	{
		bool rst = false;
		try
		{
			std::regex regexpattern(pattern);
			dest = std::regex_replace(source,regexpattern,fmt);
			return rst;
		}
		catch(const std::regex_error& e)
		{
			CString str(e.what());
			std::regex_constants::error_type err =e.code();
			AfxMessageBox(str);
			return rst;
		}	
	}
    #pragma endregion 
}
