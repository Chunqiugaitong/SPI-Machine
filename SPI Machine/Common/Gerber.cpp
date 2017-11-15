#include "StdAfx.h"
#include "Gerber.h"

#include <string>
#include <math.h>
#include <algorithm>

Gerber::Gerber(void)
{
	TipInfo.XNumber.DecimalBits = 3;
	TipInfo.XNumber.IntegerBits = 3;
	TipInfo.YNumber.DecimalBits = 3;
	TipInfo.YNumber.IntegerBits = 3;
	TipInfo.Unit = Millimetre;

	m_MarginReal.bottom = 100;
	m_MarginReal.top = 0;
	m_MarginReal.left =0;
	m_MarginReal.right = 100;

	m_leftMargin = 5;
	m_rightMargin = 5;
	m_topMargin = 5;
	m_bottomMargin = 5;
}
Gerber::~Gerber(void)
{
}

/***********************************************************************/   
/*                 GetLine
/*   @函数描述：   从文本中读取一行数据
/*   @函数参数：   
/*   @作者：       
/*   @日期：       
/*   @修订说明：   
/*                 深圳慧大成智能科技有限公司 All rights reserved.
/***********************************************************************/
int Gerber::GetLine(FILE *pf,char*t)
{
	int i =0;
	int c;

	while((c=getc(pf))!='\n'&&c!=EOF)
	{
		t[i] = c;
		i++;
	}
	if(c == EOF)
	{
		return 0;
	}
	return 1;
}

/***********************************************************************/   
/*                 
/*   @函数描述：   
/*   @函数参数：   
/*   @作者：       
/*   @日期：       
/*   @修订说明：   
/*                 深圳慧大成智能科技有限公司 All Rights Reserved.
/***********************************************************************/
int Gerber::HandleStrXY(float& nCurX,float& nCurY,char* t,_TipInfo tipInfo,CRect& marginReal)   //处理提取出来的一行字符
{
	int len = 0;           //字符的长度
	int i = 0;         //i,j,k整型临时变量，用于操作
	int j = 0;
	int k = 0;
	int flag = 0;       //标志位，为0表示此行无坐标，为1表示此行有坐标


	char xlabel[20];     //横坐标
	char ylabel[20];     //纵坐标

	//功能代码D01下的圆弧圆心
	char ilabel[20];     //横坐标
	char jlabel[20];     //纵坐标

	float nPreX = 0.0;
	float nPreY = 0.0;

	int XBits = 0;
	int YBits = 0;

	len = strlen(t);
	if(len<10)           //字符长度小于10,则不可能有坐标数据
	{
		return 0;
	}

	while(t[i] != '*')
	{
		if (t[i] != 'X')
		{
			i++;
			continue;
		}
		if(t[i] == 'X' || t[i] == 'x')    //若这行数据中出现了‘X’，
		{
			flag = 1;
			if(t[i+1] != '-')        //正数
			{
				j =i+1;
				k = 0;
				while (t[j] >= '0' && t[j] <= '9' || t[j] == '+')
				{
					xlabel[k] = t[j];
					j++;
					k++;
				}
			}
			else                    //负数
			{
				j=i+1;
				k = 0;
				while (t[j] >= '0' && t[j] <= '9' || t[j] == '-')
				{
					xlabel[k] = t[j];
					j++;
					k++;
				}
			}

		}
		if (t[j] == 'Y' || t[j] == 'y')
		{
			flag = 1;
			if(t[j+1] != '-')
			{
				j =j+1;
				k = 0;
				while (t[j] >= '0' && t[j] <= '9' || t[j] == '+')
				{
					ylabel[k] = t[j];
					j++;
					k++;
				}
			}
			else
			{
				j=j+1;
				k = 0;
				while (t[j] >= '0' && t[j] <= '9' || t[j] == '-')
				{
					ylabel[k] = t[j];
					j++;
					k++;
				}
			}
		}

		break;      //找到坐标数据退出循环
		i++;          //若没有发现‘X’，检测下一个字符
	}

	if(1 == flag)                 //若有坐标数据变为整型
	{
		int nXDecimalBits = tipInfo.XNumber.DecimalBits;
		int nYDecimalBits = tipInfo.YNumber.DecimalBits;

		switch (tipInfo.ZeroOmit)
		{
		case NotOmit:
		case LeadingOmit:
			break;
		case TailOmit:
			//整数部分位数与规定一致不变，小数部分位数可能会忽略末尾的0
			nXDecimalBits = XBits - tipInfo.XNumber.IntegerBits;
			nYDecimalBits = YBits - tipInfo.YNumber.IntegerBits;
			break;
		}

		if(atoi(xlabel) != 0)
		{
			nPreX = atoi(xlabel)/pow(10.0,nXDecimalBits);
			nCurX = nPreX;
		}   
		else
		{
			nCurX = nPreX;
		}

		if(atoi(ylabel) != 0)
		{
			nPreY = atoi(ylabel)/pow(10.0,nYDecimalBits);
			nCurY = nPreY;
		}   
		else
		{
			nCurY = nPreY;
		}

		if (tipInfo.Unit == Inch)
		{
			nCurX = nCurX *(25.4);
			nCurY = nCurY *(25.4);
		}

		//计算数据集合X、Y的边界
		UpdateMargin(marginReal,nCurX,nCurY);

	}
	return 0;
}

/***********************************************************************/   
/*                 
/*   @函数描述：   
/*   @函数参数：   
/*   @作者：       
/*   @日期：       
/*   @修订说明：   
/*                 深圳慧大成智能科技有限公司 All rights reserved.
/***********************************************************************/
int Gerber::HandleStrXYIJ(float& nCurX,float& nCurY,float& nCurI,float& nCurJ,char* t,_TipInfo tipInfo,CRect& marginReal)   //处理提取出来的一行字符
{
	int len = 0;           //字符的长度
	int i = 0;         //i,j,k整型临时变量，用于操作
	int j = 0;
	int k = 0;
	int flag = 0;       //标志位，为0表示此行无坐标，为1表示此行有坐标

	char xlabel[20];     //横坐标
	char ylabel[20];     //纵坐标

	//功能代码D01下的圆弧圆心
	char ilabel[20];     //横坐标
	char jlabel[20];     //纵坐标

	int XBits = 0;
	int YBits = 0;

	len = strlen(t);
	if(len<10)           //字符长度小于10,则不可能有坐标数据
	{
		return 0;
	}

	while(t[i])
	{
		if (t[i] != 'X')
		{
			i++;
			continue;
		}
		if(t[i] == 'X' || t[i] == 'x')    //若这行数据中出现了‘X’，
		{
			flag = 1;
			if(t[i+1] != '-')        //正数
			{
				j =i+1;
				k = 0;
				while (t[j] >= '0' && t[j] <= '9' || t[j] == '+')
				{
					xlabel[k] = t[j];
					j++;
					k++;
				}
			}
			else                    //负数
			{
				j=i+1;
				k = 0;
				while (t[j] >= '0' && t[j] <= '9' || t[j] == '-')
				{
					xlabel[k] = t[j];
					j++;
					k++;
				}
			}
			XBits = k;

		}
		if (t[j] == 'Y' || t[j] == 'y')
		{
			flag = 1;
			if(t[j+1] != '-')
			{
				j =j+1;
				k = 0;
				while (t[j] >= '0' && t[j] <= '9' || t[j] == '+')
				{
					ylabel[k] = t[j];
					j++;
					k++;
				}
			}
			else
			{
				j=j+1;
				k = 0;
				while (t[j] >= '0' && t[j] <= '9' || t[j] == '-')
				{
					ylabel[k] = t[j];
					j++;
					k++;
				}
			}
			YBits = k;

		}
		if ('I'== t[j] || 'i' == t[j])
		{
			flag = 1;
			if('-'==t[j+1])
			{
				j=j+2;
			}
			else
			{
				j=j+1;
			}
			k = 0;
			while (t[j] >= '0' && t[j] <= '9' || t[j] == '-')
			{
				ilabel[k] = t[j];
				j++;
				k++;
			}
		}

		if ('J'== t[j] || 'j' == t[j])
		{
			flag = 1;
			if('-'==t[j+1])
			{
				j=j+2;
			}
			else
			{
				j=j+1;
			}
			k = 0;
			while (t[j] >= '0' && t[j] <= '9' || t[j] == '-')
			{
				jlabel[k] = t[j];
				j++;
				k++;
			}
		}

		break;      //找到坐标数据退出循环
		i++;          //若没有发现‘X’，检测下一个字符
	}

	if(1 == flag)                 //若有坐标数据变为整型
	{
		int nXDecimalBits = tipInfo.XNumber.DecimalBits;
		int nYDecimalBits = tipInfo.YNumber.DecimalBits;
		switch (tipInfo.ZeroOmit)
		{
		case NotOmit:
		case LeadingOmit:
			break;
		case TailOmit:
			//整数部分位数与规定一致不变，小数部分位数可能会忽略末尾的0
			nXDecimalBits = XBits - tipInfo.XNumber.IntegerBits;
			nYDecimalBits = YBits - tipInfo.YNumber.IntegerBits;
			break;
		}
		nCurX = atoi(xlabel)/pow(10.0,nXDecimalBits);
		nCurY = atoi(ylabel)/pow(10.0,nYDecimalBits);
		nCurI = nCurX + atoi(ilabel)/pow(10.0,nXDecimalBits);
		nCurJ = nCurY + atoi(jlabel)/pow(10.0,nYDecimalBits);
		if (tipInfo.Unit == Inch)
		{
			nCurX = nCurX *(25.4);
			nCurY = nCurY *(25.4);
			nCurI = nCurI *(25.4);
			nCurJ = nCurJ *(25.4);
		}

		//计算数据集合X、Y的边界
		UpdateMargin(marginReal,nCurX,nCurY);
	}
	return 0;
}

void Gerber::UpdateMargin(CRect& margin,int nCurX,int nCurY)
{
	margin.left = margin.left  > nCurX ? nCurX : margin.left;
	margin.right = margin.right  < nCurX ? nCurX : margin.right;
	margin.top = margin.top  < nCurY ? nCurY : margin.top;
	margin.bottom = margin.bottom  > nCurY ? nCurY : margin.bottom;
}
void Gerber::UpdateMargin(CRect& margin,int nCurleft,int nCurTop,int nCurRight,int nCurBottom)
{
	margin.left = margin.left  > nCurleft ? nCurleft : margin.left;
	margin.right = margin.right  < nCurRight ? nCurRight : margin.right;
	margin.top = margin.top  < nCurTop ? nCurTop : margin.top;
	margin.bottom = margin.bottom  > nCurBottom ? nCurBottom : margin.bottom;
}

 void Gerber::ReadNumberFromStr(char* t,int i,float& nX,float& nY)
{
	char Xlabel[20];
	char Ylabel[20];
	int k = 0;
	int j = 0;
	while (t[i])
	{
		if (t[i] >= '0' && t[i] <= '9' || t[i] == '.')
		{
			Xlabel[k] = t[i];
			i++;
			k++;
			continue;
		}

		if (t[i] == 'X')
		{
			nX = atof(Xlabel);
			i++;
			break;
		}
		if (t[i] == '*')
		{
			nX = atof(Xlabel);
			return;
		}

	}
	while (t[i])
	{
		if (t[i] >= '0' && t[i] <= '9' || t[i] == '.')
		{
			Ylabel[j] = t[i];
			i++;
			j++;
			continue;
		}
		if (t[i] == '*')
		{
			nY = atof(Ylabel);
			return;
		}	
	}
}
 void Gerber::GetRegionRealMargin(vector<_Polygon>& vecPolygon)
 {
	CString str;
	int count = 0;
	vector<_Polygon>::iterator itorPolygon = vecPolygon.begin();
	for ( ; itorPolygon != vecPolygon.end();itorPolygon++)
	{
		if (itorPolygon->vecPoint.size() > 0)
		{
			itorPolygon->RealPolygonMargin.left = Max;
			itorPolygon->RealPolygonMargin.right = Min;
			itorPolygon->RealPolygonMargin.top = Min;
			itorPolygon->RealPolygonMargin.bottom = Max;
			vector<float> vecX,vecY;
			vector<_Point>::iterator itorPoint  = itorPolygon->vecPoint.begin();
			for ( ; itorPoint  != itorPolygon->vecPoint.end(); itorPoint++)
			{
// 				itorPolygon->RealPolygonMargin.left    = itorPolygon->RealPolygonMargin.left   >= itorPoint->XReal ? itorPoint->XReal : itorPolygon->RealPolygonMargin.left;
// 				itorPolygon->RealPolygonMargin.right   = itorPolygon->RealPolygonMargin.right  <= itorPoint->XReal ? itorPoint->XReal : itorPolygon->RealPolygonMargin.right;
// 				itorPolygon->RealPolygonMargin.top     = itorPolygon->RealPolygonMargin.top    <= itorPoint->YReal ? itorPoint->YReal : itorPolygon->RealPolygonMargin.top;
// 				itorPolygon->RealPolygonMargin.bottom  = itorPolygon->RealPolygonMargin.bottom >= itorPoint->YReal ? itorPoint->YReal : itorPolygon->RealPolygonMargin.bottom;

				vecX.push_back(itorPoint->XReal);
				vecY.push_back(itorPoint->YReal);

			}
			
			vector<float>::iterator itorX = min_element(begin(vecX),end(vecX));
			itorPolygon->RealPolygonMargin.left = *itorX;
			itorX = max_element(begin(vecX),end(vecX));
			itorPolygon->RealPolygonMargin.right = *itorX;

			vector<float>::iterator itorY = min_element(begin(vecY),end(vecY));
			itorPolygon->RealPolygonMargin.bottom = *itorY;
			itorY = max_element(begin(vecY),end(vecY));
			itorPolygon->RealPolygonMargin.top = *itorY;

			itorPolygon->AveragePoint.XReal = (itorPolygon->RealPolygonMargin.right + itorPolygon->RealPolygonMargin.left)/2;
			itorPolygon->AveragePoint.YReal = (itorPolygon->RealPolygonMargin.bottom + itorPolygon->RealPolygonMargin.top)/2;

		}	
	}
 }

/************************************************************************   
**                
**                 GetRealRegions
**   @函数描述：   获取各种图形元素的参数信息，包括矩形、椭圆、圆等基本图形以及组合多边形
**   @函数参数：   文件名
**   @作者：       陈杰
**   @日期：       2016.11.18
**   @修订说明：   
**                 深圳慧大成智能科技有限公司 All Rights Reserved.
************************************************************************/
vector<_Region> Gerber::GetRealRegions(char *filename,CRect& marginReal)
{
	vector<_Region> vecRegion;
	_Region region;

#pragma region Polygon
	vector<_Polygon> vecPolygon;

	_Polygon polygon;
	vector<_Point> vecPoint;
	vector<_Arc> vecArc; 
	_Point startDrawPoint;
	_Point averagePoint;

	_Arc arc;
	_Point startPoint;//存放,画圆弧前上一个线段端点坐标,也就是圆弧的起点坐标。
	_Point endPoint;
	_Point centerPoint;
	eDirection dir;
	eArcStyle arcStyle;

	_Point linePoint;
#pragma endregion

	eApertureShape ApertureShape = RectangleShape;

	vector<_Ellipse> vecEllipse;
	_Ellipse ellipse;
	vector<_Circle> vecCircle;
	_Circle circle;
	vector<_RectangleEx> vectRect;
	_RectangleEx rect;
	vector<_Custom> vecCustom;
	_Custom custom;

	bool isLine = false;
	bool isClockwiseCircle = false;
	bool isCounterClockwiseCircle = false;

	FILE *ifp;
	ifp = fopen(filename,"r");
	if (ifp == NULL)
	{
		AfxMessageBox("请检查Gerber文件路径是否正确！");
		return vecRegion;
	}

	char tmp[100];               //Buffer
	memset(tmp,0,100);

	GetDataFormatParam(ifp,tmp);

	while(GetLine(ifp,tmp))
	{
		if (tmp == " ") continue;
		GetG54Info(ifp,tmp,TipInfo,vecRegion,marginReal,ApertureShape);
		GetG36Info(ifp,tmp,TipInfo,vecRegion,marginReal);           
		memset(tmp,0,100);
	}

	fclose(ifp);
	ifp = NULL;
	return vecRegion;
}

void Gerber::GetDispRegions(vector<_Region>& vecRegions,CRect& RegionMargin,float xoffset,float yoffset,int rate)
{
	vector<_Polygon>::iterator itorPolygon = m_vecPolygon.begin();
	for ( ; itorPolygon != m_vecPolygon.end(); itorPolygon++)
	{
#pragma region G36/G37 Polygon区域
		itorPolygon->StartDrawPoint.XDisp = (itorPolygon->StartDrawPoint.XReal - xoffset)*rate;
		itorPolygon->StartDrawPoint.YDisp = (yoffset - itorPolygon->StartDrawPoint.YReal)*rate;

		itorPolygon->AveragePoint.XDisp = (itorPolygon->AveragePoint.XReal - xoffset)*rate;
		itorPolygon->AveragePoint.YDisp = (yoffset - itorPolygon->AveragePoint.YReal)*rate;

		//region中心点坐标
		//iterRegion->centerPoint = iterRegion->Polygon.AveragePoint;

		itorPolygon->DispPolygonMargin.left   = (itorPolygon->RealPolygonMargin.left - xoffset)*rate;
		itorPolygon->DispPolygonMargin.right  = (itorPolygon->RealPolygonMargin.right- xoffset)*rate;
		itorPolygon->DispPolygonMargin.top    = (yoffset - itorPolygon->RealPolygonMargin.top)*rate;
		itorPolygon->DispPolygonMargin.bottom = (yoffset - itorPolygon->RealPolygonMargin.bottom)*rate;

		vector<_Point>::iterator iterPoint = itorPolygon->vecPoint.begin();
		for ( ; iterPoint != itorPolygon->vecPoint.end();iterPoint++)
		{
			iterPoint->XDisp = (iterPoint->XReal - xoffset)*rate;
			iterPoint->YDisp = (yoffset - iterPoint->YReal)*rate;
		}

		vector<_Arc>::iterator iterArc = itorPolygon->vecArc.begin();
		for ( ; iterArc != itorPolygon->vecArc.end(); iterArc++ )
		{
			iterArc->StartPoint.XDisp = (iterArc->StartPoint.XReal - xoffset)*rate;
			iterArc->StartPoint.YDisp = (yoffset - iterArc->StartPoint.YReal)*rate;

			iterArc->EndPoint.XDisp = (iterArc->EndPoint.XReal - xoffset)*rate;
			iterArc->EndPoint.YDisp = (yoffset - iterArc->EndPoint.YReal)*rate;

			iterArc->CenterPoint.XDisp = (iterArc->CenterPoint.XReal - xoffset)*rate;
			iterArc->CenterPoint.YDisp = (yoffset - iterArc->CenterPoint.YReal)*rate;
		}
#pragma endregion
	}

	vector<_Region>::iterator iterRegion = vecRegions.begin();
	for ( ; iterRegion != vecRegions.end(); iterRegion++)
	{

		//         #pragma region G36/G37 Polygon区域
		// 		iterRegion->Polygon.StartDrawPoint.XDisp = (iterRegion->Polygon.StartDrawPoint.XReal - xoffset)*rate;
		// 		iterRegion->Polygon.StartDrawPoint.YDisp = (yoffset - iterRegion->Polygon.StartDrawPoint.YReal)*rate;
		// 
		// 		iterRegion->Polygon.AveragePoint.XDisp = (iterRegion->Polygon.AveragePoint.XReal - xoffset)*rate;
		// 		iterRegion->Polygon.AveragePoint.YDisp = (yoffset - iterRegion->Polygon.AveragePoint.YReal)*rate;
		// 
		// 		//region中心点坐标
		// 		//iterRegion->centerPoint = iterRegion->Polygon.AveragePoint;
		// 
		// 		iterRegion->Polygon.DispPolygonMargin.left   = (iterRegion->Polygon.RealPolygonMargin.left - xoffset)*rate;
		// 		iterRegion->Polygon.DispPolygonMargin.right  = (iterRegion->Polygon.RealPolygonMargin.right- xoffset)*rate;
		// 		iterRegion->Polygon.DispPolygonMargin.top    = (yoffset - iterRegion->Polygon.RealPolygonMargin.top)*rate;
		// 		iterRegion->Polygon.DispPolygonMargin.bottom = (yoffset - iterRegion->Polygon.RealPolygonMargin.bottom)*rate;
		// 
		// 		vector<_Point>::iterator iterPoint = iterRegion->Polygon.vecPoint.begin();
		// 		for ( ; iterPoint != iterRegion->Polygon.vecPoint.end();iterPoint++)
		// 		{
		// 			iterPoint->XDisp = (iterPoint->XReal - xoffset)*rate;
		// 			iterPoint->YDisp = (yoffset - iterPoint->YReal)*rate;
		// 		}
		// 
		// 		vector<_Arc>::iterator iterArc = iterRegion->Polygon.vecArc.begin();
		// 		for ( ; iterArc != iterRegion->Polygon.vecArc.end(); iterArc++ )
		// 		{
		// 			iterArc->StartPoint.XDisp = (iterArc->StartPoint.XReal - xoffset)*rate;
		// 			iterArc->StartPoint.YDisp = (yoffset - iterArc->StartPoint.YReal)*rate;
		// 
		// 			iterArc->EndPoint.XDisp = (iterArc->EndPoint.XReal - xoffset)*rate;
		// 			iterArc->EndPoint.YDisp = (yoffset - iterArc->EndPoint.YReal)*rate;
		// 
		// 			iterArc->CenterPoint.XDisp = (iterArc->CenterPoint.XReal - xoffset)*rate;
		// 			iterArc->CenterPoint.YDisp = (yoffset - iterArc->CenterPoint.YReal)*rate;
		// 		}
		//         #pragma endregion

		vector<_Circle>::iterator iterCircle = iterRegion->vecCircle.begin();
		for ( ; iterCircle != iterRegion->vecCircle.end(); iterCircle++)
		{
			iterCircle->CenterPoint.XDisp = (iterCircle->CenterPoint.XReal - xoffset)*rate;
			iterCircle->CenterPoint.YDisp = (yoffset - iterCircle->CenterPoint.YReal)*rate;
			iterCircle->Radius *= rate;
		}

		vector<_Ellipse>::iterator iterEllipse = iterRegion->vecEllipse.begin();
		for ( ; iterEllipse != iterRegion->vecEllipse.end(); iterEllipse++)
		{
			iterEllipse->CenterPoint.XDisp = (iterEllipse->CenterPoint.XReal - xoffset)*rate;
			iterEllipse->CenterPoint.YDisp = (yoffset - iterEllipse->CenterPoint.YReal)*rate;
			iterEllipse->LongAxis *= rate;
			iterEllipse->ShortAxis *= rate;
		}

		vector<_RectangleEx>::iterator iterRect = iterRegion->vecRect.begin();
		for ( ; iterRect != iterRegion->vecRect.end(); iterRect++)
		{
			iterRect->CenterPoint.XDisp = (iterRect->CenterPoint.XReal - xoffset)*rate;
			iterRect->CenterPoint.YDisp = (yoffset - iterRect->CenterPoint.YReal)*rate;
			iterRect->LongAxis *= rate;
			iterRect->ShortAxis *= rate;
		}

		vector<_Custom>::iterator iterCustom = iterRegion->vecCustom.begin();
		for ( ; iterCustom != iterRegion->vecCustom.end(); iterCustom++)
		{
			iterCustom->CenterPoint.XDisp = (iterCustom->CenterPoint.XReal - xoffset)*rate;
			iterCustom->CenterPoint.YDisp = (yoffset - iterCustom->CenterPoint.YReal)*rate;
			iterCustom->LongAxis *= rate;
			iterCustom->ShortAxis *= rate;
		}
	}

	RegionMargin.left = (RegionMargin.left - xoffset)*rate;
	RegionMargin.right = (RegionMargin.right - xoffset)*rate;

	RegionMargin.bottom = (yoffset - RegionMargin.bottom)*rate;
	RegionMargin.top = (yoffset - RegionMargin.top)*rate;

	//由于RECT 及CRect要求 r>l,b>t,故把b、t作交换
// 	LONG temp;
// 	temp = RegionMargin.bottom;
// 	RegionMargin.bottom = RegionMargin.top;
// 	RegionMargin.top = temp;

}
void Gerber::PreDrawRegions(char *filename,vector<_Region>& vecRegion,CRect& RegionMargin,map<int,_Cluster>& mapCluster,int rate)
{
	vecRegion = GetRealRegions(filename,m_MarginReal);
	GetRegionRealMargin(m_vecPolygon);
	RegionMargin = m_MarginReal;
	GetDispRegions(vecRegion,RegionMargin,m_MarginReal.left - m_leftMargin,m_MarginReal.top + m_topMargin,rate);
	int ViewWidth  = 20*rate;
	int ViewHeight = 20*rate;
	int clusterIndex = 0;

	//把Polygon存到Map里
	map<int,_Polygon> mapPolygon;
	int polygonIndex = 0;
	vector<_Polygon>:: iterator itorPolygon = m_vecPolygon.begin();
	for ( ; itorPolygon != m_vecPolygon.end();itorPolygon++)
	{
		mapPolygon.insert(make_pair<int,_Polygon>(polygonIndex++,*itorPolygon));
	}
	SplitGerberRegion(mapCluster,clusterIndex,mapPolygon,ViewWidth,ViewHeight);
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
void Gerber::SplitGerberRegion(map<int,_Cluster>& mapCluster,int& clusterIndex,map<int,_Polygon>& mapPolygon,int ViewWidth,int ViewHeight)
{
	CString str;
	//寻找Polygon集合的最小top值
	map<int,_Polygon>::iterator itorMapPolygon = mapPolygon.begin();
	itorMapPolygon = min_element( mapPolygon.begin(), mapPolygon.end(),map_PointLessByY());
	_Polygon polygonMinY = itorMapPolygon->second;

	//筛选在[top，Top + ViewHeight]之间的Polygon
	map<int,_Polygon> mapPolygonsByY;
	int mapIndex = 0;
	itorMapPolygon = mapPolygon.begin();
	for ( ; itorMapPolygon != mapPolygon.end();itorMapPolygon++)
	{
		if (itorMapPolygon->second.DispPolygonMargin.top >= polygonMinY.DispPolygonMargin.top                    &&
			itorMapPolygon->second.DispPolygonMargin.bottom <= polygonMinY.DispPolygonMargin.top + 2*ViewHeight  )
		{
			mapPolygonsByY.insert(make_pair<int,_Polygon>(mapIndex++,itorMapPolygon->second));
		}
	}

	bool IsClusterFirstWorkInMap = true;
	//划分top区间内的polygon集合里的元素，直到集合为空
	while (!mapPolygonsByY.empty())
	{
		map<int,_Polygon>::iterator itormapPolygonsByY = mapPolygonsByY.begin();
		//寻找top区间内集合Polygon的最小top值
		if (IsClusterFirstWorkInMap)
		{
			IsClusterFirstWorkInMap = false;
			itorMapPolygon = min_element( mapPolygon.begin(), mapPolygon.end(),map_PointLessByY());
			polygonMinY = itorMapPolygon->second;
		}
		else
		{
			itormapPolygonsByY = min_element( mapPolygonsByY.begin(), mapPolygonsByY.end(),map_PointLessByY());
			polygonMinY = itormapPolygonsByY->second;
		}
		
		itormapPolygonsByY = min_element( mapPolygonsByY.begin(), mapPolygonsByY.end(),map_PointLessByX());
		_Polygon polygonMinX = itormapPolygonsByY->second;

// 		str.Format("Left--Top: %f  %f\n",polygonMinX.DispPolygonMargin.left,polygonMinY.DispPolygonMargin.top);
// 		OutputDebugString(str);
// 		str.Empty();

		//**************Cluster边界调整***************//
		_Cluster cluster;
		float TopDistMin = Max;
		cluster.clusterMargin.left   = polygonMinX.DispPolygonMargin.left;
		cluster.clusterMargin.top    = polygonMinY.DispPolygonMargin.top;
		itormapPolygonsByY = mapPolygonsByY.begin();
		for ( ; itormapPolygonsByY != mapPolygonsByY.end();itormapPolygonsByY++)
		{
			if (itormapPolygonsByY->second.DispPolygonMargin.left   >= polygonMinX.DispPolygonMargin.left    &&
				itormapPolygonsByY->second.DispPolygonMargin.right  <= polygonMinX.DispPolygonMargin.left + ViewWidth &&
				itormapPolygonsByY->second.DispPolygonMargin.top    >= polygonMinY.DispPolygonMargin.top                  )
			{
				if(itormapPolygonsByY->second.DispPolygonMargin.top - polygonMinY.DispPolygonMargin.top < TopDistMin)
				{
					TopDistMin = itormapPolygonsByY->second.DispPolygonMargin.top - polygonMinY.DispPolygonMargin.top;
					cluster.clusterMargin.top  = itormapPolygonsByY->second.DispPolygonMargin.top;

				}	
			}
		}

		cluster.clusterMargin.right  = cluster.clusterMargin.left + ViewWidth;
		cluster.clusterMargin.bottom = cluster.clusterMargin.top + ViewHeight;
		cluster.centerPoint.XDisp   = (cluster.clusterMargin.left + cluster.clusterMargin.right) /2;
		cluster.centerPoint.YDisp    = (cluster.clusterMargin.top + cluster.clusterMargin.bottom) /2;

// 		str.Format("Cluster矩形框：%f  %f  %f  %f\n",cluster.clusterMargin.left,cluster.clusterMargin.top,cluster.clusterMargin.right,cluster.clusterMargin.bottom);
// 		OutputDebugString(str);
// 		str.Empty();

		itormapPolygonsByY = mapPolygonsByY.begin();
		for ( ; itormapPolygonsByY != mapPolygonsByY.end();)
		{
			if (itormapPolygonsByY->second.DispPolygonMargin.left   >= cluster.clusterMargin.left    &&
				itormapPolygonsByY->second.DispPolygonMargin.top    >= cluster.clusterMargin.top     &&
				itormapPolygonsByY->second.DispPolygonMargin.right  <= cluster.clusterMargin.right   &&
				itormapPolygonsByY->second.DispPolygonMargin.bottom <= cluster.clusterMargin.bottom   )
			{
				cluster.vecPolygon.push_back(itormapPolygonsByY->second);
				
				_Polygon polygon = itormapPolygonsByY->second;
				//从top区间内的polygon集合里删除选中的Polygon
				mapPolygonsByY.erase(itormapPolygonsByY++);

				//从总的Polygon集合里删除选中的Polygon
				itorMapPolygon = find_if(mapPolygon.begin(),mapPolygon.end(),map_finder(polygon));
				mapPolygon.erase(itorMapPolygon);

			}
			else
			{
				itormapPolygonsByY++;

			}
		}

 		if (mapCluster.size() > 0)
 		{
 			//break;
 		}

		if (cluster.vecPolygon.size() > 0)
		{
			mapCluster.insert(make_pair<int,_Cluster>(clusterIndex++,cluster));	
		} 
	}
	
	//如果所有图形都被分配完了，结束递归
	if (mapPolygon.empty())
	{
		return;
	}

	SplitGerberRegion(mapCluster,clusterIndex,mapPolygon,ViewWidth,ViewHeight);
}
void Gerber::DrawRegions(CDC &memDC,vector<_Region> vecRegions,map<int,_Cluster> mapCluster)
{
	CPen pen(PS_SOLID,2,RGB(255,0,0));
	CPen greenPen(PS_SOLID,0.5,RGB(102,205,0));
	CPen bluePen(PS_SOLID,1,RGB(0,180,255));
	memDC.SelectObject(&pen);

	CBrush brush,brush2;
	brush.CreateSolidBrush(RGB(1,4,1)); 
	brush2.CreateSolidBrush(RGB(255,0,0));

	CString str;

    vector<_Polygon>::iterator itorPolygon = m_vecPolygon.begin();
	for ( ; itorPolygon != m_vecPolygon.end(); itorPolygon++)
	{
//     	memDC.SelectStockObject(NULL_BRUSH);   
//     	memDC.SelectObject(&greenPen);
//      	memDC.Rectangle(itorPolygon->DispPolygonMargin.left,itorPolygon->DispPolygonMargin.top,itorPolygon->DispPolygonMargin.right,itorPolygon->DispPolygonMargin.bottom);

		memDC.MoveTo(itorPolygon->StartDrawPoint.XDisp,itorPolygon->StartDrawPoint.YDisp);
		vector<_Point>::iterator iterPoint = itorPolygon->vecPoint.begin();
		for ( ; iterPoint != itorPolygon->vecPoint.end();iterPoint++)
		{
			memDC.SelectObject(&pen);//重要
			memDC.LineTo(iterPoint->XDisp,iterPoint->YDisp);
		}

		vector<_Arc>::iterator iterArc = itorPolygon->vecArc.begin();
		for ( ; iterArc != itorPolygon->vecArc.end(); iterArc++ )
		{
			// 					int radius = 0;
			// 					float sAngle,eAngle;
			// 					pDC->MoveTo(iterArc->StartPoint.XDisp,iterArc->StartPoint.YDisp);
			// 					m_gerber.CalculateArc(iterArc->CenterPoint,iterArc->StartPoint,iterArc->EndPoint,radius,sAngle,eAngle);  					
			//  					CRect rect(iterArc->CenterPoint.XDisp - radius,iterArc->CenterPoint.YDisp - radius,iterArc->CenterPoint.XDisp + radius,iterArc->CenterPoint.YDisp + radius);
			//  					CPoint sp(iterArc->StartPoint.XDisp,iterArc->StartPoint.YDisp);
			//  					CPoint ep(iterArc->EndPoint.XDisp,iterArc->EndPoint.YDisp);
			//  					pDC->SetArcDirection(iterArc->Dir);
			// 
			//  					pDC->Arc(&rect,sp,ep);
		}

		if (itorPolygon->isPolygon)
		{
			//选择透明画刷
// 			memDC.SelectStockObject(NULL_BRUSH);   
// 			memDC.SelectObject(&bluePen);
// 
//  			memDC.MoveTo(itorPolygon->AveragePoint.XDisp,itorPolygon->AveragePoint.YDisp);
//  			memDC.LineTo(itorPolygon->AveragePoint.XDisp - 5,itorPolygon->AveragePoint.YDisp);
//  			memDC.MoveTo(itorPolygon->AveragePoint.XDisp,itorPolygon->AveragePoint.YDisp);
//  			memDC.LineTo(itorPolygon->AveragePoint.XDisp + 5,itorPolygon->AveragePoint.YDisp);
//  			memDC.MoveTo(itorPolygon->AveragePoint.XDisp,itorPolygon->AveragePoint.YDisp);
//  			memDC.LineTo(itorPolygon->AveragePoint.XDisp,itorPolygon->AveragePoint.YDisp - 5);
//  			memDC.MoveTo(itorPolygon->AveragePoint.XDisp,itorPolygon->AveragePoint.YDisp);
//  			memDC.LineTo(itorPolygon->AveragePoint.XDisp,itorPolygon->AveragePoint.YDisp + 5);
// 
// 			memDC.SelectObject(&pen);
		}
	}

	memDC.SelectStockObject(NULL_BRUSH);   
	memDC.SelectObject(&greenPen);
	map<int,_Cluster>::iterator itorMapCluster = mapCluster.begin();

	for ( ; itorMapCluster != mapCluster.end();itorMapCluster++)
	{
 		memDC.SelectObject(&greenPen);
 		memDC.Rectangle(itorMapCluster->second.clusterMargin.left,itorMapCluster->second.clusterMargin.top,itorMapCluster->second.clusterMargin.right,itorMapCluster->second.clusterMargin.bottom);
		str.Format("%d #%d ",itorMapCluster->first,itorMapCluster->second.vecPolygon.size());
		memDC.SelectObject(&bluePen);
		memDC.TextOutA(itorMapCluster->second.centerPoint.XDisp,itorMapCluster->second.centerPoint.YDisp,str);

		vector<_Polygon>::iterator itorPolygon = itorMapCluster->second.vecPolygon.begin();
		for ( ; itorPolygon != itorMapCluster->second.vecPolygon.end();itorPolygon++)
		{
			memDC.MoveTo(itorPolygon->StartDrawPoint.XDisp,itorPolygon->StartDrawPoint.YDisp);
			vector<_Point>::iterator itorPoint = itorPolygon->vecPoint.begin();
			for ( ; itorPoint != itorPolygon->vecPoint.end();itorPoint++)
			{
				memDC.LineTo(itorPoint->XDisp,itorPoint->YDisp);
			}
		}
	}
	return;

	/************************************************************************/
	/*                                                                       
	/************************************************************************/

#pragma region 组合多边形绘制
	CBrush* pOldBrush = memDC.SelectObject(&brush);//重要
	vector<_Region>::iterator iterRegion = vecRegions.begin();
	for ( ; iterRegion !=  vecRegions.end(); iterRegion++)
	{
		memDC.MoveTo(iterRegion->Polygon.StartDrawPoint.XDisp,iterRegion->Polygon.StartDrawPoint.YDisp);
		vector<_Point>::iterator iterPoint = iterRegion->Polygon.vecPoint.begin();
		for ( ; iterPoint != iterRegion->Polygon.vecPoint.end();iterPoint++)
		{
			memDC.SelectObject(&pen);//重要
			memDC.LineTo(iterPoint->XDisp,iterPoint->YDisp);
		}

		vector<_Arc>::iterator iterArc = iterRegion->Polygon.vecArc.begin();
		for ( ; iterArc != iterRegion->Polygon.vecArc.end(); iterArc++ )
		{
			// 					int radius = 0;
			// 					float sAngle,eAngle;
			// 					pDC->MoveTo(iterArc->StartPoint.XDisp,iterArc->StartPoint.YDisp);
			// 					m_gerber.CalculateArc(iterArc->CenterPoint,iterArc->StartPoint,iterArc->EndPoint,radius,sAngle,eAngle);  					
			//  					CRect rect(iterArc->CenterPoint.XDisp - radius,iterArc->CenterPoint.YDisp - radius,iterArc->CenterPoint.XDisp + radius,iterArc->CenterPoint.YDisp + radius);
			//  					CPoint sp(iterArc->StartPoint.XDisp,iterArc->StartPoint.YDisp);
			//  					CPoint ep(iterArc->EndPoint.XDisp,iterArc->EndPoint.YDisp);
			//  					pDC->SetArcDirection(iterArc->Dir);
			// 
			//  					pDC->Arc(&rect,sp,ep);
		}

		if (iterRegion->Polygon.isPolygon)
		{
			//选择透明画刷
			memDC.SelectStockObject(NULL_BRUSH);   
			memDC.SelectObject(&bluePen);

			memDC.MoveTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp);
			memDC.LineTo(iterRegion->Polygon.AveragePoint.XDisp - 5,iterRegion->Polygon.AveragePoint.YDisp);
			memDC.MoveTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp);
			memDC.LineTo(iterRegion->Polygon.AveragePoint.XDisp + 5,iterRegion->Polygon.AveragePoint.YDisp);
			memDC.MoveTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp);
			memDC.LineTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp - 5);
			memDC.MoveTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp);
			memDC.LineTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp + 5);

			memDC.SelectObject(&bluePen);
			memDC.Rectangle(iterRegion->Polygon.AveragePoint.XDisp - 30*2/*iterRegion->Polygon.DispPolygonMargin.Width()*/,
				iterRegion->Polygon.AveragePoint.YDisp - 40*2/*iterRegion->Polygon.DispPolygonMargin.Height()*/,
				iterRegion->Polygon.AveragePoint.XDisp + 30*2/*iterRegion->Polygon.DispPolygonMargin.Width()*/,
				iterRegion->Polygon.AveragePoint.YDisp + 40*2/*iterRegion->Polygon.DispPolygonMargin.Height()*/);

			// 			CString str;
			// 			str.Format("**  %f  %f\n", iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp);
			// 			OutputDebugString(str);

			memDC.SelectObject(&pen);
		}
#pragma endregion

#pragma  region 基本图形绘制
		vector<_RectangleEx>::iterator iterRectAngle = iterRegion->vecRect.begin();
		for ( ; iterRectAngle != iterRegion->vecRect.end();  iterRectAngle++)
		{
			memDC.SelectStockObject(NULL_BRUSH);
			memDC.Rectangle(iterRectAngle->CenterPoint.XDisp - iterRectAngle->LongAxis/2,
				iterRectAngle->CenterPoint.YDisp + iterRectAngle->ShortAxis/2,
				iterRectAngle->CenterPoint.XDisp + iterRectAngle->LongAxis/2,
				iterRectAngle->CenterPoint.YDisp - iterRectAngle->ShortAxis/2 );
		}

		vector<_Ellipse>::iterator iterEllipse = iterRegion->vecEllipse.begin();
		for (;iterEllipse != iterRegion->vecEllipse.end();iterEllipse++)
		{
			memDC.SelectStockObject(NULL_BRUSH);
			memDC.Ellipse(iterEllipse->CenterPoint.XDisp - iterEllipse->LongAxis/2,
				iterEllipse->CenterPoint.YDisp + iterEllipse->ShortAxis/2,
				iterEllipse->CenterPoint.XDisp + iterEllipse->LongAxis/2,
				iterEllipse->CenterPoint.YDisp - iterEllipse->ShortAxis/2   );
			//CBrush* pOldBrush = memDC.SelectObject(&brush2);//重要
			//memDC.SelectObject(pOldBrush);
		}

		vector<_Circle>::iterator iterCircle = iterRegion->vecCircle.begin();
		for (;iterCircle != iterRegion->vecCircle.end();iterCircle++)
		{
			memDC.SelectStockObject(NULL_BRUSH);
			memDC.Ellipse(iterCircle->CenterPoint.XDisp - iterCircle->Radius,
				iterCircle->CenterPoint.YDisp + iterCircle->Radius,
				iterCircle->CenterPoint.XDisp + iterCircle->Radius,
				iterCircle->CenterPoint.YDisp - iterCircle->Radius   );
		}

		vector<_Custom>::iterator iterCustom = iterRegion->vecCustom.begin();
		for (;iterCustom != iterRegion->vecCustom.end();iterCustom++)
		{
			memDC.SelectStockObject(NULL_BRUSH);
			memDC.Rectangle(iterCustom->CenterPoint.XDisp - iterCustom->LongAxis/2,
				iterCustom->CenterPoint.YDisp + iterCustom->ShortAxis/2,
				iterCustom->CenterPoint.XDisp + iterCustom->LongAxis/2,
				iterCustom->CenterPoint.YDisp - iterCustom->ShortAxis/2 );
		}
#pragma  endregion

	}
	
}

//读取Gerber文件里提示性参数
void Gerber::GetDataFormatParam(FILE* pfile,char* charTemp)
{
	_ApertureInfo ApertureInfo;

	while (GetLine(pfile,charTemp))
	{
#pragma region %FSLAX33Y33*%
		if (strstr(charTemp,"%FS"))
		{
			if (charTemp[3] == 'L')
			{
				TipInfo.ZeroOmit = LeadingOmit;
			}
			if (charTemp[3] == 'T')
			{
				TipInfo.ZeroOmit = TailOmit;
			}
			if (charTemp[3] == 'N')
			{
				TipInfo.ZeroOmit = NotOmit;
			}

			if (charTemp[4] == 'A')
			{

			}
			if (charTemp[4] == 'I')
			{

			}

			if (charTemp[5] == 'X')
			{
				CString str6(charTemp[6]);
				TipInfo.XNumber.IntegerBits = atoi(str6);
				CString str7(charTemp[7]);
				TipInfo.XNumber.DecimalBits = atoi(str7);
			}

			if (charTemp[8] == 'Y')
			{
				CString str9(charTemp[9]);
				TipInfo.YNumber.IntegerBits = atoi(str9);
				CString str10(charTemp[10]);
				TipInfo.YNumber.DecimalBits = atoi(str10);
			}

			memset(charTemp,0,100);
			continue;
		}
#pragma endregion

#pragma region Unit MO
		if (strstr(charTemp,"%MOMM*%"))
		{
			TipInfo.Unit = Millimetre;
			memset(charTemp,0,100);
			continue;
		}
		if (strstr(charTemp,"%MOIN*%"))
		{
			TipInfo.Unit = Inch;
			memset(charTemp,0,100);
			continue;
		}
#pragma endregion

#pragma region  Aperture %ADD11C,0.100000*%  

		//ADD295R,1.320800X1.117600*% 
		if (strstr(charTemp,"%ADD")/* && strstr(charTemp,"C,")*/)
		{		
			//charTemp[6]为数字0~9,    D代码为3位数100~999

			CString str(charTemp);
			//OutputDebugString(str + "\n");

			if (charTemp[6] >= '0' && charTemp[6] <= '9')
			{
				CString str4(charTemp[4]);
				CString str5(charTemp[5]);
				CString str6(charTemp[6]);

				ApertureInfo.DCode = atoi(str4+str5+str6);
				if (charTemp[7] == 'O' && charTemp[8] == ',')
				{
					ApertureInfo.ApertureShape = ObroundShape;
					ReadNumberFromStr(charTemp,9,ApertureInfo.Parameter.RectAngle.LongAxis,
						ApertureInfo.Parameter.RectAngle.ShortAxis);
				}
				if (charTemp[7] == 'R' && charTemp[8] == ',')
				{
					ApertureInfo.ApertureShape = RectangleShape;
					ReadNumberFromStr(charTemp,9,ApertureInfo.Parameter.RectAngle.LongAxis,
						ApertureInfo.Parameter.RectAngle.ShortAxis);
				}
				if (charTemp[7] == 'C' && charTemp[8] == ',')
				{
					float temp = 0.0;
					ApertureInfo.ApertureShape = CircleShape;
					ReadNumberFromStr(charTemp,9,ApertureInfo.Parameter.Diameter,temp);

					if (ApertureInfo.Parameter.Diameter == 0.000000)
					{
						ApertureInfo.Parameter.Diameter = 0.100000;
					}
				}
				if (charTemp[7] == 'P' && charTemp[8] == ',')
				{
					ApertureInfo.ApertureShape = PolygonShape;
				}
				//AM宏光圈判断
				if (charTemp[8] != ',')
				{
					ApertureInfo.ApertureShape = CustomShape;
					ApertureInfo.Parameter.RectAngle.LongAxis = 0.4;
					ApertureInfo.Parameter.RectAngle.ShortAxis = 0.4;
				}
			}
			else  //D代码为两位数10~99
			{
				CString str4(charTemp[4]);
				CString str5(charTemp[5]);

				ApertureInfo.DCode = atoi(str4+str5);

				if (charTemp[6] == 'O' && charTemp[7] == ',')
				{
					ApertureInfo.ApertureShape = ObroundShape;
					ReadNumberFromStr(charTemp,8,ApertureInfo.Parameter.RectAngle.LongAxis,
						ApertureInfo.Parameter.RectAngle.ShortAxis);
				}
				if (charTemp[6] == 'R' && charTemp[7] == ',')
				{
					ApertureInfo.ApertureShape = RectangleShape;
					ReadNumberFromStr(charTemp,8,ApertureInfo.Parameter.RectAngle.LongAxis,
						ApertureInfo.Parameter.RectAngle.ShortAxis);
				}
				if (charTemp[6] == 'C' && charTemp[7] == ',')
				{
					float temp = 0.0;
					ApertureInfo.ApertureShape = CircleShape;
					ReadNumberFromStr(charTemp,8,ApertureInfo.Parameter.Diameter,temp);
					if (ApertureInfo.Parameter.Diameter == 0.000000)
					{
						ApertureInfo.Parameter.Diameter = 0.100000;
					}
				}
				if (charTemp[6] == 'P' && charTemp[7] == ',')
				{
					ApertureInfo.ApertureShape = PolygonShape;
				}

				//AM宏光圈判断
				if (charTemp[7] != ',')
				{
					ApertureInfo.ApertureShape = CustomShape;
					ApertureInfo.Parameter.RectAngle.LongAxis = 0.4;
					ApertureInfo.Parameter.RectAngle.ShortAxis = 0.4;
				}
			}
			vecApertureInfo.push_back(ApertureInfo);
			memset(charTemp,0,100);
			continue;
		}
#pragma endregion

		if (strstr(charTemp,"%LP"))
		{
			memset(charTemp,0,100);
			return;
		}

	}
}
void Gerber::GetG54Info(FILE *ifp,char* tmp,_TipInfo tipInfo,vector<_Region>& vecRegion,CRect& marginReal,eApertureShape ApertureShape)
{
	_Region region;
	_Point linePoint;

	vector<_Ellipse> vecEllipse;
	_Ellipse ellipse;
	vector<_Circle> vecCircle;
	_Circle circle;
	vector<_RectangleEx> vectRect;
	_RectangleEx rect;
	vector<_Custom> vecCustom;
	_Custom custom;


	if (tmp[0]=='G'&&tmp[1]=='5'&&tmp[2]=='4')
	{
		CString str;
		CString str4(tmp[4]);
		CString str5(tmp[5]);
		if (tmp[6] >= '0' && tmp[6] <= '9')
		{
			CString str6(tmp[6]);
			str = str4 + str5 + str6;
		}
		else
		{
			str = str4 + str5;
		}

		vector<_ApertureInfo>::iterator iterApertureInfo = vecApertureInfo.begin();
		for ( ; iterApertureInfo != vecApertureInfo.end(); iterApertureInfo++)
		{
			if (iterApertureInfo->DCode == atoi(str))
			{
				ApertureShape = iterApertureInfo->ApertureShape;
				switch (iterApertureInfo->ApertureShape)
				{
				case CircleShape:
					circle.Radius = (iterApertureInfo->Parameter.Diameter)/2;
					break;
				case ObroundShape:
					ellipse.LongAxis = iterApertureInfo->Parameter.RectAngle.LongAxis;
					ellipse.ShortAxis = iterApertureInfo->Parameter.RectAngle.ShortAxis;
					break;
				case RectangleShape:
					rect.LongAxis = iterApertureInfo->Parameter.RectAngle.LongAxis;
					rect.ShortAxis = iterApertureInfo->Parameter.RectAngle.ShortAxis;
					break;
				case PolygonShape:
					break;
				case CustomShape:
					custom.LongAxis = iterApertureInfo->Parameter.RectAngle.LongAxis;
					custom.ShortAxis = iterApertureInfo->Parameter.RectAngle.ShortAxis;
					break;
				}
				break;
			}
		}
	}
	else
	{
		return;
	}

	memset(tmp,0,100);	

	while (GetLine(ifp,tmp))
	{
		CString str(tmp);
		//OutputDebugString(str +"\n");

		if (strstr(tmp,"D03*") || strstr(tmp,"D01*"))
		{
			HandleStrXY(linePoint.XReal,linePoint.YReal,tmp,tipInfo,marginReal);	

			switch (ApertureShape)
			{
			case CircleShape:
				circle.CenterPoint = linePoint;
				vecCircle.push_back(circle);

				break;
			case ObroundShape:
				ellipse.CenterPoint = linePoint;
				vecEllipse.push_back(ellipse);
				break;
			case RectangleShape:
				rect.CenterPoint = linePoint;
				vectRect.push_back(rect);
				break;
			case PolygonShape:
				break;
			case CustomShape:
				custom.CenterPoint = linePoint;
				vecCustom.push_back(custom);
				break;
			}
			memset(tmp,0,100);	
			continue;
		} 	

		if (strstr(tmp,"D02*"))
		{
			memset(tmp,0,100);	
			continue;
		}

		if (tmp[0]=='G'&&tmp[1]=='5'&&tmp[2]=='4')
		{
			GetG54Info(ifp,tmp,tipInfo,vecRegion,marginReal,ApertureShape);
		}
		else
		{
			CString str;
			str.Format("%d  %d   %d  %d\n",vecCircle.size(),vecEllipse.size(),vectRect.size(),vecCustom.size());
			//OutputDebugString(str);
			if (vecCircle.size() || vecEllipse.size() || vectRect.size() || vecCustom.size())
			{
				region.vecCircle = vecCircle;
				region.vecEllipse = vecEllipse;
				region.vecRect = vectRect;
				region.vecCustom = vecCustom;

				vecRegion.push_back(region);

				vecCircle.clear();
				vecEllipse.clear();
				vectRect.clear();
				vecCustom.clear();
			}

			memset(tmp,0,100);	
			break;
		}

	}	
	return;
}
void Gerber::GetG36Info(FILE *ifp,char* tmp,_TipInfo tipInfo,vector<_Region>& vecRegion,CRect& marginReal)
{
	_Region region;
	vector<_Polygon> vecPolygon;

	_Polygon polygon;
	vector<_Point> vecPoint;
	vector<_Arc> vecArc;
	_Point startDrawPoint;
	_Point averagePoint;

	_Arc arc;
	_Point startPoint;//存放,画圆弧前上一个线段端点坐标,也就是圆弧的起点坐标。
	_Point endPoint;
	_Point centerPoint;
	eDirection dir;
	eArcStyle arcStyle;

	_Point linePoint;

	polygon.RealPolygonMargin.left = Max;
	polygon.RealPolygonMargin.right = Min;
	polygon.RealPolygonMargin.top = Min;
	polygon.RealPolygonMargin.bottom = Max;
	polygon.isPolygon = false;
	polygon.isSelected = false;

	if(tmp[0]=='G'&&tmp[1]=='3'&&tmp[2]=='6')
	{
		memset(tmp,0,100);
		while(GetLine(ifp,tmp))
		{
			if (tmp == " ")
			{
				continue;
			}

			if(tmp[0]=='G'&&tmp[1]=='3'&&tmp[2]=='7')
			{
				polygon.vecPoint = vecPoint;
				polygon.vecArc = vecArc;
				polygon.StartDrawPoint = startDrawPoint;
				polygon.isPolygon = true;

				//用中程数代替平均值，x = (Xmax + Xmin)/2
				polygon.AveragePoint.XReal = (polygon.RealPolygonMargin.right + polygon.RealPolygonMargin.left)/2;
				polygon.AveragePoint.YReal = (polygon.RealPolygonMargin.bottom + polygon.RealPolygonMargin.top)/2;
				m_vecPolygon.push_back(polygon);

				region.Polygon = polygon;
				vector<_Circle>().swap(region.vecCircle);
				vector<_Custom>().swap(region.vecCustom);
				vector<_Ellipse>().swap(region.vecEllipse);
				vector<_RectangleEx>().swap(region.vecRect);

				vecRegion.push_back(region);

				vector<_Polygon>().swap(vecPolygon);
				vector<_Point>().swap(vecPoint);
				vector<_Arc>().swap(vecArc);
				break;
			}

			//圆弧插补，优弧
			if (tmp[0]=='G'&&tmp[1]=='7'&&tmp[2]=='5')
			{
				arcStyle = MajorArc;
				memset(tmp,0,100);
				continue;
			}
			//圆弧插补，劣弧
			if (tmp[0]=='G'&&tmp[1]=='7'&&tmp[2]=='4')
			{
				arcStyle = MinorArc;
				memset(tmp,0,100);
				continue;
			}

			//读取Region绘制起始点，读取直线插补标志G01
			if (strstr(tmp,"D02*") ||tmp[0]=='G'&&tmp[1]=='0'&&tmp[2]=='1')
			{
				HandleStrXY(startDrawPoint.XReal,startDrawPoint.YReal,tmp,tipInfo,marginReal);
				//	  			isLine = true;
				memset(tmp,0,100);
				continue;
			}

			//读取直线插补坐标
			if (strstr(tmp,"D01*") && (strstr(tmp,"X") || strstr(tmp,"Y"))&& !strstr(tmp,"I") && !strstr(tmp,"J"))
			{
				HandleStrXY(linePoint.XReal,linePoint.YReal,tmp,tipInfo,marginReal);	
				vecPoint.push_back(linePoint);
				//更新组合多边形polygon的边界值
//				UpdateMargin(polygon.RealPolygonMargin,linePoint.XReal,linePoint.YReal);
				//每次保存读到的线段端点坐标
				startPoint.XReal = linePoint.XReal;
				startPoint.YReal = linePoint.YReal;

				memset(tmp,0,100);
				continue;
			}

			//读取圆弧插补坐标
			if (strstr(tmp,"D01*") && strstr(tmp,"X") && strstr(tmp,"Y") && strstr(tmp,"I") && strstr(tmp,"J"))
			{
				HandleStrXYIJ(endPoint.XReal,endPoint.YReal,centerPoint.XReal,centerPoint.YReal,tmp,tipInfo,marginReal);
				arc.StartPoint = startPoint;//把上一条线段的端点作为圆弧起始点
				arc.EndPoint = endPoint;
				arc.CenterPoint = centerPoint;
				arc.arcStyle = arcStyle;
				if (strstr(tmp,"G02"))
				{
					arc.Dir = Clockwise;
				}
				if (strstr(tmp,"G03"))
				{
					arc.Dir = CounterClockwise;
				}
				vecArc.push_back(arc);

				//更新组合多边形polygon的边界值
//				UpdateMargin(polygon.RealPolygonMargin,linePoint.XReal,linePoint.YReal);

				memset(tmp,0,100);
				continue;
			} 
			memset(tmp,0,100);
		}			

	}
	return;

}

void Gerber::CalculateArc(_Point cp,_Point sp,_Point ep,int& radius,float& sAngle,float& eAngle)
{
	radius = sqrt(pow(cp.XDisp - sp.XDisp,2) + pow(cp.YReal - sp.YReal,2));
	sAngle = atan(cp.YDisp - sp.YDisp/(cp.XDisp - sp.XDisp))*180/Pie;
	eAngle = atan(cp.YDisp - ep.YDisp/(cp.XDisp - ep.XDisp))*180/Pie;

	CString str;
	str.Format("%d  %f  %f  %f  %f\n",radius,cp.XDisp,cp.YDisp,sp.XDisp,sp.YDisp);
	//OutputDebugString(str);
	return;
}

/************************************************************************/
/* 容器内存的释放        测试用                                                            
/************************************************************************/
//     vector<int> v;    
// 	v.push_back(21);
// 	v.push_back(34);
// 	v.clear();//size为0，capacity没变
// 	//用一个临时对象替代v的内存空间，函数执行完后原临时对象的内存会释放
// 	vector<int>().swap(v);//size为0，capacity也为0，内存得到释放。
// 	v.swap(vector<int>());
	/////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/*   选择透明画刷                                                                  
/************************************************************************/
// 	memDC.SelectStockObject(NULL_BRUSH);   
// 	vector<_RoiRegion>::iterator itorRoiRegion = m_vecRoiRegion.begin();
// 	memDC.SelectObject(&greenPen);
// 	for ( ; itorRoiRegion != m_vecRoiRegion.end(); itorRoiRegion++)
// 	{
// 		memDC.Rectangle(itorRoiRegion->RoiMargin.left,itorRoiRegion->RoiMargin.top,itorRoiRegion->RoiMargin.right,itorRoiRegion->RoiMargin.bottom);
// 	}
// 	////////////////////////////////////////////////////////////////////////////

//  void SplitGerberRegion_11(map<int,_Cluster>& mapCluster,int& clusterIndex,map<int,_Polygon>& mapPolygon,int ViewWidth,int ViewHeight)
// {
// 	CString str;
// 
// 	// Y方向排列_Polygon
// 	map<int,_Polygon>::iterator itorMapPolygon = mapPolygon.begin();
// 	map<int,_Polygon>::iterator itorBack = mapPolygon.begin();
// 
// 	// X方向排列_Polygon
// 	itorMapPolygon = min_element( mapPolygon.begin(), mapPolygon.end(),map_PointLessByX());
// 	_Polygon polygonMinX = itorMapPolygon->second;
// 
// 	_Cluster cluster;
// 	bool isFirstClustered = false;
// 	if (polygonMinX.DispPolygonMargin.right - polygonMinX.DispPolygonMargin.left < ViewWidth  && 
// 		polygonMinX.DispPolygonMargin.bottom - polygonMinX.DispPolygonMargin.top < ViewHeight )
// 	{
// 		map<int,_Polygon>::iterator itor = find_if(mapPolygon.begin(),mapPolygon.end(),map_finder(polygonMinX));
// 		if (itor != mapPolygon.end())
// 		{
// 			mapPolygon.erase(itor);	
// 		} 
// 
// 		str.Format("left最小的Polygon：%d  %d  %d  %d\n",polygonMinX.DispPolygonMargin.left,polygonMinX.DispPolygonMargin.top,polygonMinX.DispPolygonMargin.right,polygonMinX.DispPolygonMargin.bottom);
// 		OutputDebugString(str);
// 		str.Empty();
// 
// 		itorMapPolygon = mapPolygon.begin();
// 		for ( ; itorMapPolygon != mapPolygon.end();)
// 		{
// 			if (!isFirstClustered)
// 			{
// 				if (itorMapPolygon->second.DispPolygonMargin.left > polygonMinX.DispPolygonMargin.left                 &&
// 					itorMapPolygon->second.DispPolygonMargin.right < polygonMinX.DispPolygonMargin.left +  ViewWidth   )
// 				{
// 					isFirstClustered = true;
// 
// 					cluster.clusterMargin.left = polygonMinX.DispPolygonMargin.left;
// 					cluster.clusterMargin.top = itorMapPolygon->second.DispPolygonMargin.top;
// 					cluster.clusterMargin.right = polygonMinX.DispPolygonMargin.left + ViewWidth;
// 					cluster.clusterMargin.bottom = itorMapPolygon->second.DispPolygonMargin.top + ViewHeight;
// 
// 					str.Format("Cluster矩形框：%d  %d  %d  %d\n",cluster.clusterMargin.left,cluster.clusterMargin.top,cluster.clusterMargin.right,cluster.clusterMargin.bottom);
// 					OutputDebugString(str);
// 					str.Empty();
// 
// 					cluster.centerPoint.XDisp = (cluster.clusterMargin.left + cluster.clusterMargin.right)/2;
// 					cluster.centerPoint.YDisp = (cluster.clusterMargin.top + cluster.clusterMargin.bottom)/2;
// 
// 					cluster.vecPolygon.push_back(polygonMinX);
// 					cluster.vecPolygon.push_back(itorMapPolygon->second);
// 
// 					itorBack = itorMapPolygon;
// 					itorMapPolygon++;		
// 					mapPolygon.erase(itorBack);	
// 				}	
// 				else
// 				{
// 					itorMapPolygon++;	
// 				}
// 
// 			}
// 			else
// 			{
// 				if (itorMapPolygon->first > cluster.clusterMargin.bottom)
// 				{
// 					//break;
// 				}
// 
// 				if (itorMapPolygon->second.DispPolygonMargin.top >= cluster.clusterMargin.top        &&
// 					itorMapPolygon->second.DispPolygonMargin.bottom <= cluster.clusterMargin.bottom  &&
// 					itorMapPolygon->second.DispPolygonMargin.left >= cluster.clusterMargin.left      &&
// 					itorMapPolygon->second.DispPolygonMargin.right <= cluster.clusterMargin.right    )
// 				{
// 					cluster.vecPolygon.push_back(itorMapPolygon->second);
// 
// 					str.Format("视野内__的Polygon：%d  %d  %d  %d\n",itorMapPolygon->second.DispPolygonMargin.left,itorMapPolygon->second.DispPolygonMargin.top,itorMapPolygon->second.DispPolygonMargin.right,itorMapPolygon->second.DispPolygonMargin.bottom);
// 					OutputDebugString(str);
// 					str.Empty();
// 
// 					itorBack = itorMapPolygon;
// 					itorMapPolygon++;
// 					mapPolygon.erase(itorBack);		
// 				}
// 				else
// 				{
// 					// 					str.Format("视野外__的Polygon：%d  %d  %d  %d\n",itorMapPolygon->second.DispPolygonMargin.left,itorMapPolygon->second.DispPolygonMargin.top,itorMapPolygon->second.DispPolygonMargin.right,itorMapPolygon->second.DispPolygonMargin.bottom);
// 					// 					OutputDebugString(str);
// 					// 					str.Empty();
// 
// 					itorMapPolygon++;	
// 				}
// 			}
// 
// 		}
// 
// 		if (cluster.vecPolygon.size() > 0)
// 		{
// 			mapCluster.insert(make_pair<int,_Cluster>(clusterIndex++,cluster));	
// 		}
// 
// 	}
// 	else
// 	{
// 		//用一个视野单独覆盖图形vecPolygonX[0]
// 		cluster.clusterMargin.left = polygonMinX.DispPolygonMargin.left;
// 		cluster.clusterMargin.top = polygonMinX.DispPolygonMargin.top;
// 		cluster.clusterMargin.right = polygonMinX.DispPolygonMargin.left + ViewWidth;
// 		cluster.clusterMargin.bottom = polygonMinX.DispPolygonMargin.top + ViewHeight;
// 
// 		cluster.centerPoint.XDisp = (cluster.clusterMargin.left + cluster.clusterMargin.right)/2;
// 		cluster.centerPoint.YDisp = (cluster.clusterMargin.top + cluster.clusterMargin.bottom)/2;
// 
// 		str.Format("%d  %d  %d  %d\n",cluster.clusterMargin.left,cluster.clusterMargin.top,cluster.clusterMargin.right,cluster.clusterMargin.bottom);
// 		OutputDebugString(str);
// 
// 		cluster.vecPolygon.push_back(polygonMinX);
// 		map<int,_Polygon>::iterator itor = mapPolygon.end();
// 		itor = find_if(mapPolygon.begin(),mapPolygon.end(),map_finder(polygonMinX));
// 		if (itor != mapPolygon.end())
// 		{
// 			mapPolygon.erase(itor);	
// 		}
// 
// 		mapCluster.insert(make_pair<int,_Cluster>(clusterIndex++,cluster));	
// 	}
// 
// 	//如果所有图形都被分配完了，结束递归
// 	if (mapPolygon.empty())
// 	{
// 		return;
// 	}
// 
// 	if (mapCluster.size() > 2)
// 	{
// 		return;
// 		int t  = 20;
// 	}
// 
// 	SplitGerberRegion_11(mapCluster,clusterIndex,mapPolygon,ViewWidth,ViewHeight);
// }
//  void SplitGerberRegion_12(map<int,_Cluster>& mapCluster,int& clusterIndex,map<int,_Polygon>& mapPolygon,int ViewWidth,int ViewHeight)
// {
// 	CString str;
// 
// 	// Y方向排列_Polygon
// 	map<int,_Polygon>::iterator itorMapPolygon = mapPolygon.begin();
// 	map<int,_Polygon>::iterator itorBack = mapPolygon.begin();
// 
// 	itorMapPolygon = min_element( mapPolygon.begin(), mapPolygon.end(),map_PointLesByY());
// 	_Polygon polygonMinY = itorMapPolygon->second;
// 
// 	// X方向排列_Polygon
// 	itorMapPolygon = min_element( mapPolygon.begin(), mapPolygon.end(),map_PointLessByX());
// 	_Polygon polygonMinX = itorMapPolygon->second;
// 
// 	str.Format("X排序最小的Polygon：%d  %d  %d  %d\n",polygonMinX.DispPolygonMargin.left,polygonMinX.DispPolygonMargin.top,polygonMinX.DispPolygonMargin.right,polygonMinX.DispPolygonMargin.bottom);
// 	OutputDebugString(str);
// 	str.Empty();
// 	str.Format("Y排序最小的Polygon：%d  %d  %d  %d\n",polygonMinY.DispPolygonMargin.left,polygonMinY.DispPolygonMargin.top,polygonMinY.DispPolygonMargin.right,polygonMinY.DispPolygonMargin.bottom);
// 	OutputDebugString(str);
// 	str.Empty();
// 
// 	_Cluster cluster;
// 	bool isFirstClustered = false;
// 	if (abs(polygonMinY.DispPolygonMargin.right - polygonMinX.DispPolygonMargin.left) < ViewWidth  && 
// 		abs(polygonMinX.DispPolygonMargin.bottom - polygonMinY.DispPolygonMargin.top) < ViewHeight )
// 	{
// 		cluster.clusterMargin.left = polygonMinX.DispPolygonMargin.left;
// 		cluster.clusterMargin.top = polygonMinY.DispPolygonMargin.top;
// 		cluster.clusterMargin.right = polygonMinX.DispPolygonMargin.left + ViewWidth;
// 		cluster.clusterMargin.bottom = polygonMinY.DispPolygonMargin.top + ViewHeight;
// 
// 		str.Format("Cluster矩形框：%d  %d  %d  %d\n",cluster.clusterMargin.left,cluster.clusterMargin.top,cluster.clusterMargin.right,cluster.clusterMargin.bottom);
// 		OutputDebugString(str);
// 		str.Empty();
// 
// 		cluster.centerPoint.XDisp = (cluster.clusterMargin.left + cluster.clusterMargin.right)/2;
// 		cluster.centerPoint.YDisp = (cluster.clusterMargin.top + cluster.clusterMargin.bottom)/2;
// 
// 		cluster.vecPolygon.push_back(polygonMinX);
// 		cluster.vecPolygon.push_back(polygonMinY);
// 
// 		str.Format("视野内__的Polygon：%d  %d  %d  %d\n",polygonMinX.DispPolygonMargin.left,polygonMinX.DispPolygonMargin.top,polygonMinX.DispPolygonMargin.right,polygonMinX.DispPolygonMargin.bottom);
// 		OutputDebugString(str);
// 		str.Empty();
// 		str.Format("视野内__的Polygon：%d  %d  %d  %d\n",polygonMinY.DispPolygonMargin.left,polygonMinY.DispPolygonMargin.top,polygonMinY.DispPolygonMargin.right,polygonMinY.DispPolygonMargin.bottom);
// 		OutputDebugString(str);
// 		str.Empty();
// 
// 		map<int,_Polygon>::iterator itor = find_if(mapPolygon.begin(),mapPolygon.end(),map_finder(polygonMinX));
// 		if (itor != mapPolygon.end())
// 		{
// 			mapPolygon.erase(itor);	
// 		} 
// 
// 		itor = find_if(mapPolygon.begin(),mapPolygon.end(),map_finder(polygonMinY));
// 		if (itor != mapPolygon.end())
// 		{
// 			mapPolygon.erase(itor);	
// 		} 
// 
// 	}
// 	else
// 	{
// 		//根据Left最小的Polygon重新确定Cluster边界	
// 		cluster.clusterMargin.left = polygonMinX.DispPolygonMargin.left;
// 		cluster.clusterMargin.top = polygonMinX.DispPolygonMargin.top;
// 
// 		vector<int> vecTop;
// 		map<int,_Polygon>::iterator itorMapPolygonTemp = mapPolygon.begin();
// 		for ( ; itorMapPolygonTemp != mapPolygon.end();itorMapPolygonTemp++)
// 		{
// 			if (itorMapPolygonTemp->second.DispPolygonMargin.top < polygonMinY.DispPolygonMargin.top    &&
// 				itorMapPolygonTemp->second.DispPolygonMargin.top > polygonMinX.DispPolygonMargin.top       &&
// 				itorMapPolygonTemp->second.DispPolygonMargin.left > polygonMinX.DispPolygonMargin.left     &&
// 				itorMapPolygonTemp->second.DispPolygonMargin.left < polygonMinY.DispPolygonMargin.left     )
// 			{
// 				vecTop.push_back(itorMapPolygonTemp->second.DispPolygonMargin.top);
// 
// 				//    				cluster.clusterMargin.top = cluster.clusterMargin.top > itorMapPolygonTemp->second.DispPolygonMargin.top ? 
// 				//   					                         itorMapPolygonTemp->second.DispPolygonMargin.top : cluster.clusterMargin.top;
// 				// 				str.Format("Cluster矩形框上边界top：%d  %d  %d  %d\n",cluster.clusterMargin.left,cluster.clusterMargin.top,cluster.clusterMargin.right,cluster.clusterMargin.bottom);
// 				// 				OutputDebugString(str);
// 				// 				str.Empty();
// 			}
// 		}
// 		cluster.clusterMargin.right = cluster.clusterMargin.left + ViewWidth;
// 		cluster.clusterMargin.bottom = cluster.clusterMargin.top + ViewHeight;
// 
// 		str.Format("Cluster矩形框：%d  %d  %d  %d\n",cluster.clusterMargin.left,cluster.clusterMargin.top,cluster.clusterMargin.right,cluster.clusterMargin.bottom);
// 		OutputDebugString(str);
// 		str.Empty();
// 
// 		cluster.centerPoint.XDisp = (cluster.clusterMargin.left + cluster.clusterMargin.right)/2;
// 		cluster.centerPoint.YDisp = (cluster.clusterMargin.top + cluster.clusterMargin.bottom)/2;
// 
// 		cluster.vecPolygon.push_back(polygonMinX);
// 
// 		str.Format("视野内__的Polygon：%d  %d  %d  %d\n",polygonMinX.DispPolygonMargin.left,polygonMinX.DispPolygonMargin.top,polygonMinX.DispPolygonMargin.right,polygonMinX.DispPolygonMargin.bottom);
// 		OutputDebugString(str);
// 		str.Empty();
// 
// 		map<int,_Polygon>::iterator itor = find_if(mapPolygon.begin(),mapPolygon.end(),map_finder(polygonMinX));
// 		if (itor != mapPolygon.end())
// 		{
// 			mapPolygon.erase(itor);	
// 		} 
// 	}
// 
// 	itorMapPolygon = mapPolygon.begin();
// 	for ( ; itorMapPolygon != mapPolygon.end();)
// 	{
// 		if (itorMapPolygon->first > cluster.clusterMargin.bottom)
// 		{
// 			//break;
// 		}
// 		if (itorMapPolygon->second.DispPolygonMargin.top >= cluster.clusterMargin.top        &&
// 			itorMapPolygon->second.DispPolygonMargin.bottom <= cluster.clusterMargin.bottom  &&
// 			itorMapPolygon->second.DispPolygonMargin.left >= cluster.clusterMargin.left      &&
// 			itorMapPolygon->second.DispPolygonMargin.right <= cluster.clusterMargin.right    )
// 		{
// 			cluster.vecPolygon.push_back(itorMapPolygon->second);
// 
// 			str.Format("视野内__的Polygon：%d  %d  %d  %d\n",itorMapPolygon->second.DispPolygonMargin.left,itorMapPolygon->second.DispPolygonMargin.top,itorMapPolygon->second.DispPolygonMargin.right,itorMapPolygon->second.DispPolygonMargin.bottom);
// 			OutputDebugString(str);
// 			str.Empty();
// 
// 			itorBack = itorMapPolygon;
// 			itorMapPolygon++;
// 			mapPolygon.erase(itorBack);		
// 		}
// 		else
// 		{
// 			// 			str.Format("视野外__的Polygon：%d  %d  %d  %d\n",itorMapPolygon->second.DispPolygonMargin.left,itorMapPolygon->second.DispPolygonMargin.top,itorMapPolygon->second.DispPolygonMargin.right,itorMapPolygon->second.DispPolygonMargin.bottom);
// 			// 			OutputDebugString(str);
// 			// 			str.Empty();
// 			itorMapPolygon++;	
// 		}
// 
// 	}
// 
// 	if (cluster.vecPolygon.size() > 0)
// 	{
// 		mapCluster.insert(make_pair<int,_Cluster>(clusterIndex++,cluster));	
// 	}
// 
// 	//如果所有图形都被分配完了，结束递归
// 	if (mapPolygon.empty())
// 	{
// 		return;
// 	}
// 
// 	if (mapCluster.size() > 5)
// 	{
// 		//return;
// 		int t  = 20;
// 	}
// 
// 	SplitGerberRegion_12(mapCluster,clusterIndex,mapPolygon,ViewWidth,ViewHeight);
// }
 
void SplitGerberRegion_NormalVersion(map<int,_Cluster>& mapCluster,int& clusterIndex,map<int,_Polygon>& mapPolygon,int ViewWidth,int ViewHeight)
{
	CString str;
	//寻找Polygon集合的最小top值
	map<int,_Polygon>::iterator itorMapPolygon = mapPolygon.begin();
	itorMapPolygon = min_element( mapPolygon.begin(), mapPolygon.end(),map_PointLessByY());
	_Polygon polygonMinY = itorMapPolygon->second;

	//筛选在[top，Top + ViewHeight]之间的Polygon
	map<int,_Polygon> mapPolygonsByY;
	int mapIndex = 0;
	itorMapPolygon = mapPolygon.begin();
	for ( ; itorMapPolygon != mapPolygon.end();itorMapPolygon++)
	{
		if (itorMapPolygon->second.DispPolygonMargin.top >= polygonMinY.DispPolygonMargin.top                    &&
			itorMapPolygon->second.DispPolygonMargin.bottom <= polygonMinY.DispPolygonMargin.top + ViewHeight  )
		{
			mapPolygonsByY.insert(make_pair<int,_Polygon>(mapIndex++,itorMapPolygon->second));
		}
	}

	bool IsClusterFirstWorkInMap = true;
	//划分top区间内的polygon集合里的元素，直到集合为空
	while (!mapPolygonsByY.empty())
	{
		map<int,_Polygon>::iterator itormapPolygonsByY = mapPolygonsByY.begin();
		//寻找top区间内集合Polygon的最小top值
		if (IsClusterFirstWorkInMap)
		{
			IsClusterFirstWorkInMap = false;
			itorMapPolygon = min_element( mapPolygon.begin(), mapPolygon.end(),map_PointLessByY());
			polygonMinY = itorMapPolygon->second;
		}
		else
		{
			itormapPolygonsByY = min_element( mapPolygonsByY.begin(), mapPolygonsByY.end(),map_PointLessByY());
			polygonMinY = itormapPolygonsByY->second;
		}

		itormapPolygonsByY = min_element( mapPolygonsByY.begin(), mapPolygonsByY.end(),map_PointLessByX());
		_Polygon polygonMinX = itormapPolygonsByY->second;

		_Cluster cluster;
		cluster.clusterMargin.left   = polygonMinX.DispPolygonMargin.left;
		cluster.clusterMargin.top    = polygonMinY.DispPolygonMargin.top;
		cluster.clusterMargin.right  = cluster.clusterMargin.left + ViewWidth;
		cluster.clusterMargin.bottom = cluster.clusterMargin.top + ViewHeight;
		cluster.centerPoint.XDisp   = (cluster.clusterMargin.left + cluster.clusterMargin.right) /2;
		cluster.centerPoint.YDisp    = (cluster.clusterMargin.top + cluster.clusterMargin.bottom) /2;

		//  		str.Format("%d\nCluster矩形框：%f  %f  %f  %f\n",clusterIndex,cluster.clusterMargin.left,cluster.clusterMargin.top,cluster.clusterMargin.right,cluster.clusterMargin.bottom);
		//  		OutputDebugString(str);
		//  		str.Empty();
		itormapPolygonsByY = mapPolygonsByY.begin();
		for ( ; itormapPolygonsByY != mapPolygonsByY.end();)
		{
			if (itormapPolygonsByY->second.DispPolygonMargin.left   >= cluster.clusterMargin.left    &&
				itormapPolygonsByY->second.DispPolygonMargin.top    >= cluster.clusterMargin.top     &&
				itormapPolygonsByY->second.DispPolygonMargin.right  <= cluster.clusterMargin.right   &&
				itormapPolygonsByY->second.DispPolygonMargin.bottom <= cluster.clusterMargin.bottom   )
			{
				cluster.vecPolygon.push_back(itormapPolygonsByY->second);

				//  				str.Format("视野内__的Polygon：%f  %f  %f  %f\n",itormapPolygonsByY->second.DispPolygonMargin.left,itormapPolygonsByY->second.DispPolygonMargin.top,itormapPolygonsByY->second.DispPolygonMargin.right,itormapPolygonsByY->second.DispPolygonMargin.bottom);
				//  				OutputDebugString(str);
				//  				str.Empty();

				_Polygon polygon = itormapPolygonsByY->second;
				//从top区间内的polygon集合里删除选中的Polygon
				mapPolygonsByY.erase(itormapPolygonsByY++);

				//从总的Polygon集合里删除选中的Polygon
				itorMapPolygon = find_if(mapPolygon.begin(),mapPolygon.end(),map_finder(polygon));
				mapPolygon.erase(itorMapPolygon);

			}
			else
			{
				itormapPolygonsByY++;

			}
		}

		if (cluster.vecPolygon.size() > 1)
		{
			mapCluster.insert(make_pair<int,_Cluster>(clusterIndex++,cluster));	
		}

	}

	//如果所有图形都被分配完了，结束递归
	if (mapPolygon.empty())
	{
		return;
	}

	if (mapCluster.size() > 1)
	{
		//return;
		int t  = 20;
	}

	SplitGerberRegion_NormalVersion(mapCluster,clusterIndex,mapPolygon,ViewWidth,ViewHeight);
}