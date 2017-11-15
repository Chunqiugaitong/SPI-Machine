#include "stdafx.h"
#include "Gerber.h"
#include "../AutoDevice.h"

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

//����ļ��е�һ�У��洢���ַ���t��
int Gerber::GetLine(FILE *pf, char*t)
{
	int i =0;
	int c;
	char szTemp[ARRAYLEN];
	memset(szTemp, 0, sizeof(szTemp));
	fgets(szTemp, sizeof(szTemp), pf);
	if (strlen(szTemp))
	{
		strcpy(t, szTemp);
		return 1;
	}
	else
	{
		return 0;
	}

	//while((c = getc(pf))!='\n'&&c != EOF)
	//{
	//	t[i] = c;
	//	i++;
	//}
	//if(c == EOF)
	//{
	//	return 0;
	//}
	//return 1;
}

int Gerber::HandleStrXY(float& nCurX,float& nCurY,char* t,_TipInfo tipInfo,CRect& marginReal)   //������ȡ������һ���ַ�
{
	int len = 0;           //�ַ��ĳ���
	int i = 0;         //i,j,k������ʱ���������ڲ���
	int j = 0;
	int k = 0;
	int flag = 0;       //��־λ��Ϊ0��ʾ���������꣬Ϊ1��ʾ����������


	char xlabel[20];     //������
	char ylabel[20];     //������

	//���ܴ���D01�µ�Բ��Բ��
	char ilabel[20];     //������
	char jlabel[20];     //������

	float nPreX = 0.0;
	float nPreY = 0.0;

	int XBits = 0;
	int YBits = 0;

	len = strlen(t);
	if(len<10)           //�ַ�����С��10,�򲻿�������������
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
		if(t[i] == 'X' || t[i] == 'x')    //�����������г����ˡ�X����
		{
			flag = 1;
			if(t[i+1] != '-')        //����
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
			else                    //����
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

		break;      //�ҵ����������˳�ѭ��
		i++;          //��û�з��֡�X���������һ���ַ�
	}

	if(1 == flag)                 //�����������ݱ�Ϊ����
	{
		int nXDecimalBits = tipInfo.XNumber.DecimalBits;
		int nYDecimalBits = tipInfo.YNumber.DecimalBits;

		switch (tipInfo.ZeroOmit)
		{
		case NotOmit:
		case LeadingOmit:
			break;
		case TailOmit:
			//��������λ����涨һ�²��䣬С������λ�����ܻ����ĩβ��0
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

		//�������ݼ���X��Y�ı߽�
		UpdateMargin(marginReal,nCurX,nCurY);

	}
	return 0;
}

int Gerber::HandleStrXYIJ(float& nCurX,float& nCurY,float& nCurI,float& nCurJ,char* t,_TipInfo tipInfo,CRect& marginReal)   //������ȡ������һ���ַ�
{
	int len = 0;           //�ַ��ĳ���
	int i = 0;         //i,j,k������ʱ���������ڲ���
	int j = 0;
	int k = 0;
	int flag = 0;       //��־λ��Ϊ0��ʾ���������꣬Ϊ1��ʾ����������

	char xlabel[20];     //������
	char ylabel[20];     //������

	//���ܴ���D01�µ�Բ��Բ��
	char ilabel[20];     //������
	char jlabel[20];     //������

	int XBits = 0;
	int YBits = 0;

	len = strlen(t);
	if(len<10)           //�ַ�����С��10,�򲻿�������������
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
		if(t[i] == 'X' || t[i] == 'x')    //�����������г����ˡ�X����
		{
			flag = 1;
			if(t[i+1] != '-')        //����
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
			else                    //����
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

		break;      //�ҵ����������˳�ѭ��
		i++;          //��û�з��֡�X���������һ���ַ�
	}

	if(1 == flag)                 //�����������ݱ�Ϊ����
	{
		int nXDecimalBits = tipInfo.XNumber.DecimalBits;
		int nYDecimalBits = tipInfo.YNumber.DecimalBits;
		switch (tipInfo.ZeroOmit)
		{
		case NotOmit:
		case LeadingOmit:
			break;
		case TailOmit:
			//��������λ����涨һ�²��䣬С������λ�����ܻ����ĩβ��0
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

		//�������ݼ���X��Y�ı߽�
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
	memset(Xlabel, 0, sizeof(Xlabel));
	memset(Ylabel, 0, sizeof(Ylabel));
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

//@����������   ��ȡ����ͼ��Ԫ�صĲ�����Ϣ���������Ρ���Բ��Բ�Ȼ���ͼ���Լ���϶����
//@����������   �ļ���
vector<_Region> Gerber::GetRealRegions(FILE* pFile, CRect& marginReal)
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
	_Point startPoint;//���,��Բ��ǰ��һ���߶ζ˵�����,Ҳ����Բ����������ꡣ
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

	char tmp[ARRAYLEN];               
	memset(tmp, 0, sizeof(tmp));
	GetDataFormatParam(pFile, tmp);
	//fseek(pFile, 0L, SEEK_SET);

	while(GetLine(pFile, tmp))
	{
		if (tmp == " ") continue;
		GetG54Info(pFile, tmp, TipInfo, vecRegion, marginReal, ApertureShape);
		GetG36Info(pFile, tmp, TipInfo, vecRegion, marginReal);           
		memset(tmp, 0, sizeof(tmp));
  	}
 	return vecRegion;
}

void Gerber::DrawRegions(CDC &memDC,vector<_Region> vecRegions,map<int,_Cluster> mapCluster)
{
	CPen pen(PS_SOLID,2,RGB(255,0,0));
	CPen greenPen(PS_SOLID,0.2,RGB(10,255,120));
	CPen bluePen(PS_SOLID,1,RGB(0,180,255));
	memDC.SelectObject(&pen);

	CBrush brush,brush2;
	brush.CreateSolidBrush(RGB(1,4,1)); 
	brush2.CreateSolidBrush(RGB(255,0,0));

	CString str;

    vector<_Polygon>::iterator itorPolygon = m_vecPolygon.begin();
	for ( ; itorPolygon != m_vecPolygon.end(); itorPolygon++)
	{
		memDC.MoveTo(itorPolygon->StartDrawPoint.XDisp,itorPolygon->StartDrawPoint.YDisp);
		vector<_Point>::iterator iterPoint = itorPolygon->vecPoint.begin();
		for ( ; iterPoint != itorPolygon->vecPoint.end();iterPoint++)
		{
			memDC.SelectObject(&pen);//��Ҫ
			memDC.LineTo(iterPoint->XDisp,iterPoint->YDisp);

// 			CPoint pt;
// 			pt.x = iterPoint->XReal;
// 			pt.y = iterPoint->YReal;
// 			memDC.LPtoDP(&pt);
// 			memDC.LineTo(pt.x,pt.y);
		}

// 		str.Format("%f  %f  %f  %f\n",itorPolygon->RealPolygonMargin.left,itorPolygon->RealPolygonMargin.top,itorPolygon->RealPolygonMargin.right,itorPolygon->RealPolygonMargin.bottom);
// 		OutputDebugString(str);

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
	}

	memDC.SelectStockObject(NULL_BRUSH);   
	memDC.SelectObject(&greenPen);
	map<int,_Cluster>::iterator itorMapCluster = mapCluster.begin();

	for ( ; itorMapCluster != mapCluster.end();itorMapCluster++)
	{
 		memDC.SelectObject(&greenPen);
 		memDC.Rectangle(itorMapCluster->second.clusterMargin.left,itorMapCluster->second.clusterMargin.top,itorMapCluster->second.clusterMargin.right,itorMapCluster->second.clusterMargin.bottom);
		//str.Format("%d #%d ",itorMapCluster->first,itorMapCluster->second.vecPolygon.size());
		//str.Format("%d ",itorMapCluster->first);	
		//	memDC.TextOutA(itorMapCluster->second.centerPoint.XDisp,itorMapCluster->second.centerPoint.YDisp,str);

//	memDC.SelectObject(&bluePen);
// 		vector<_Polygon>::iterator itorPolygon = itorMapCluster->second.vecPolygon.begin();
// 		for ( ; itorPolygon != itorMapCluster->second.vecPolygon.end();itorPolygon++)
// 		{
// 			memDC.MoveTo(itorPolygon->StartDrawPoint.XDisp,itorPolygon->StartDrawPoint.YDisp);
// 			vector<_Point>::iterator itorPoint = itorPolygon->vecPoint.begin();
// 			for ( ; itorPoint != itorPolygon->vecPoint.end();itorPoint++)
// 			{
// 				memDC.LineTo(itorPoint->XDisp,itorPoint->YDisp);
// 			}
// 		}
	}
	/*return;*/

	/************************************************************************/
	/*                                                                       
	/************************************************************************/

#pragma region ��϶���λ���
	CBrush* pOldBrush = memDC.SelectObject(&brush);//��Ҫ
	vector<_Region>::iterator iterRegion = vecRegions.begin();
	for ( ; iterRegion !=  vecRegions.end(); iterRegion++)
	{
		memDC.MoveTo(iterRegion->Polygon.StartDrawPoint.XDisp,iterRegion->Polygon.StartDrawPoint.YDisp);
		vector<_Point>::iterator iterPoint = iterRegion->Polygon.vecPoint.begin();
		for ( ; iterPoint != iterRegion->Polygon.vecPoint.end();iterPoint++)
		{
			memDC.SelectObject(&pen);//��Ҫ
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
			//ѡ��͸����ˢ
// 			memDC.SelectStockObject(NULL_BRUSH);   
// 			memDC.SelectObject(&bluePen);
// 
// 			memDC.MoveTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp);
// 			memDC.LineTo(iterRegion->Polygon.AveragePoint.XDisp - 5,iterRegion->Polygon.AveragePoint.YDisp);
// 			memDC.MoveTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp);
// 			memDC.LineTo(iterRegion->Polygon.AveragePoint.XDisp + 5,iterRegion->Polygon.AveragePoint.YDisp);
// 			memDC.MoveTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp);
// 			memDC.LineTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp - 5);
// 			memDC.MoveTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp);
// 			memDC.LineTo(iterRegion->Polygon.AveragePoint.XDisp,iterRegion->Polygon.AveragePoint.YDisp + 5);

		}
#pragma endregion

#pragma  region ����ͼ�λ���
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
			//CBrush* pOldBrush = memDC.SelectObject(&brush2);//��Ҫ
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


void Gerber::GetDispRegions(vector<_Region>& vecRegions,CRect& RegionMargin,float xoffset,float yoffset,int rate)
{
	vector<_Polygon>::iterator itorPolygon = m_vecPolygon.begin();
	for ( ; itorPolygon != m_vecPolygon.end(); itorPolygon++)
	{
#pragma region G36/G37 Polygon����
		itorPolygon->StartDrawPoint.XDisp = (itorPolygon->StartDrawPoint.XReal - xoffset)*rate;
		itorPolygon->StartDrawPoint.YDisp = (yoffset - itorPolygon->StartDrawPoint.YReal)*rate;

		itorPolygon->AveragePoint.XDisp = (itorPolygon->AveragePoint.XReal - xoffset)*rate;
		itorPolygon->AveragePoint.YDisp = (yoffset - itorPolygon->AveragePoint.YReal)*rate;

		//region���ĵ�����
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

		//         #pragma region G36/G37 Polygon����
		// 		iterRegion->Polygon.StartDrawPoint.XDisp = (iterRegion->Polygon.StartDrawPoint.XReal - xoffset)*rate;
		// 		iterRegion->Polygon.StartDrawPoint.YDisp = (yoffset - iterRegion->Polygon.StartDrawPoint.YReal)*rate;
		// 
		// 		iterRegion->Polygon.AveragePoint.XDisp = (iterRegion->Polygon.AveragePoint.XReal - xoffset)*rate;
		// 		iterRegion->Polygon.AveragePoint.YDisp = (yoffset - iterRegion->Polygon.AveragePoint.YReal)*rate;
		// 
		// 		//region���ĵ�����
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

	//����RECT ��CRectҪ�� r>l,b>t,�ʰ�b��t������
	// 	LONG temp;
	// 	temp = RegionMargin.bottom;
	// 	RegionMargin.bottom = RegionMargin.top;
	// 	RegionMargin.top = temp;

}

void Gerber::PreDrawRegions(FILE* pFile,vector<_Region>& vecRegion,CRect& RegionMargin,map<int,_Cluster>& mapCluster,int rate, float ViewWidth, float ViewHeight)
{
	vecRegion = GetRealRegions(pFile, m_MarginReal);
	GetRegionRealMargin(m_vecPolygon);
	RegionMargin = m_MarginReal;
	GetDispRegions(vecRegion,RegionMargin,m_MarginReal.left - m_leftMargin,m_MarginReal.top + m_topMargin,rate);
	//ViewWidth  = 20*rate;
	//ViewHeight = 20*rate;
	int clusterIndex = 0;

	//��Polygon�浽Map��
	map<int,_Polygon> mapPolygon;
	int polygonIndex = 0;
	vector<_Polygon>:: iterator itorPolygon = m_vecPolygon.begin();
	for ( ; itorPolygon != m_vecPolygon.end();itorPolygon++)
	{
		mapPolygon.insert(make_pair<int,_Polygon>(polygonIndex++,*itorPolygon));
	}
	SplitGerberRegion(mapCluster, clusterIndex, mapPolygon, ViewWidth, ViewHeight);
}

 
//��ȡGerber�ļ�����ʾ�Բ���
void Gerber::GetDataFormatParam(FILE* pFile, char* charTemp)
{
	_ApertureInfo ApertureInfo;

	while (GetLine(pFile,charTemp))
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
				TipInfo.XNumber.IntegerBits = _wtoi(str6);
				CString str7(charTemp[7]);
				TipInfo.XNumber.DecimalBits = _wtoi(str7);
			}

			if (charTemp[8] == 'Y')
			{
				CString str9(charTemp[9]);
				TipInfo.YNumber.IntegerBits = _wtoi(str9);
				CString str10(charTemp[10]);
				TipInfo.YNumber.DecimalBits = _wtoi(str10);
			}

			memset(charTemp, 0, sizeof(charTemp));
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
			memset(charTemp, 0, sizeof(charTemp));
			continue;
		}
#pragma endregion

#pragma region  Aperture %ADD11C,0.100000*%  

		//ADD295R,1.320800X1.117600*% 
		if (strstr(charTemp,"%ADD")/* && strstr(charTemp,"C,")*/)
		{		
			//charTemp[6]Ϊ����0~9,    D����Ϊ3λ��100~999

			CString str(charTemp);
			//OutputDebugString(str + "\n");

			if (charTemp[6] >= '0' && charTemp[6] <= '9')
			{
				CString str4(charTemp[4]);
				CString str5(charTemp[5]);
				CString str6(charTemp[6]);

				ApertureInfo.DCode = _wtoi(str4+str5+str6);
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
				//AM���Ȧ�ж�
				if (charTemp[8] != ',')
				{
					ApertureInfo.ApertureShape = CustomShape;
					ApertureInfo.Parameter.RectAngle.LongAxis = 0.4;
					ApertureInfo.Parameter.RectAngle.ShortAxis = 0.4;
				}
			}
			else  //D����Ϊ��λ��10~99
			{
				CString str4(charTemp[4]);
				CString str5(charTemp[5]);

				ApertureInfo.DCode = _wtoi(str4+str5);

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

				//AM���Ȧ�ж�
				if (charTemp[7] != ',')
				{
					ApertureInfo.ApertureShape = CustomShape;
					ApertureInfo.Parameter.RectAngle.LongAxis = 0.4;
					ApertureInfo.Parameter.RectAngle.ShortAxis = 0.4;
				}
			}
			vecApertureInfo.push_back(ApertureInfo);
			memset(charTemp, 0, sizeof(charTemp));
			continue;
		}
#pragma endregion

		if (strstr(charTemp,"%LP"))
		{
			memset(charTemp, 0, sizeof(charTemp));
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
			if (iterApertureInfo->DCode == _wtoi(str))
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
	memset(tmp, 0, sizeof(tmp));	
	while (GetLine(ifp,tmp))
	{
		CString str(tmp);
		//OutputDebugString(str +"\n");

		if (strstr(tmp,"D03*") || strstr(tmp,"D01*"))
		{
			HandleStrXY(linePoint.XReal, linePoint.YReal, tmp, tipInfo,marginReal);	

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
			memset(tmp, 0, sizeof(tmp));	
			continue;
		} 	
		if (strstr(tmp,"D02*"))
		{
			memset(tmp, 0, sizeof(tmp));	
			continue;
		}
		if (tmp[0]=='G'&&tmp[1]=='5'&&tmp[2]=='4')
		{
			GetG54Info(ifp,tmp,tipInfo,vecRegion,marginReal,ApertureShape);
		}
		else
		{
			CString str;
			str.Format(_T("%d  %d   %d  %d\n"), vecCircle.size(), 
				vecEllipse.size(), vectRect.size(), vecCustom.size());
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
			memset(tmp, 0, sizeof(tmp));	
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
	_Point startPoint;//���,��Բ��ǰ��һ���߶ζ˵�����,Ҳ����Բ����������ꡣ
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
		memset(tmp, 0, sizeof(tmp));	
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
				//���г�������ƽ��ֵ��x = (Xmax + Xmin)/2
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
			//Բ���岹���Ż�
			if (tmp[0]=='G'&&tmp[1]=='7'&&tmp[2]=='5')
			{
				arcStyle = MajorArc;
				memset(tmp, 0, sizeof(tmp));	
				continue;
			}
			//Բ���岹���ӻ�
			if (tmp[0]=='G'&&tmp[1]=='7'&&tmp[2]=='4')
			{
				arcStyle = MinorArc;
				memset(tmp, 0, sizeof(tmp));	
				continue;
			}

			//��ȡRegion������ʼ�㣬��ȡֱ�߲岹��־G01
			if (strstr(tmp,"D02*") ||tmp[0]=='G'&&tmp[1]=='0'&&tmp[2]=='1')
			{
				HandleStrXY(startDrawPoint.XReal, startDrawPoint.YReal, tmp, tipInfo,marginReal);
				//	  			isLine = true;
				memset(tmp, 0, sizeof(tmp));	
				continue;
			}
			//��ȡֱ�߲岹����
			if (strstr(tmp,"D01*") && (strstr(tmp,"X") || strstr(tmp,"Y"))&& !strstr(tmp,"I") && !strstr(tmp,"J"))
			{
				HandleStrXY(linePoint.XReal, linePoint.YReal, tmp, tipInfo, marginReal);	
				vecPoint.push_back(linePoint);
				//������϶����polygon�ı߽�ֵ
				//				UpdateMargin(polygon.RealPolygonMargin,linePoint.XReal,linePoint.YReal);
				//ÿ�α���������߶ζ˵�����
				startPoint.XReal = linePoint.XReal;
				startPoint.YReal = linePoint.YReal;

				memset(tmp, 0, sizeof(tmp));	
				continue;
			}
			//��ȡԲ���岹����
			if (strstr(tmp,"D01*") && strstr(tmp,"X") && strstr(tmp,"Y") && strstr(tmp,"I") && strstr(tmp,"J"))
			{
				HandleStrXYIJ(endPoint.XReal, endPoint.YReal, centerPoint.XReal, 
					centerPoint.YReal, tmp, tipInfo,marginReal);
				arc.StartPoint = startPoint;//����һ���߶εĶ˵���ΪԲ����ʼ��
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

				//������϶����polygon�ı߽�ֵ
				//				UpdateMargin(polygon.RealPolygonMargin,linePoint.XReal,linePoint.YReal);

				memset(tmp, 0, sizeof(tmp));	
				continue;
			} 
			memset(tmp, 0, sizeof(tmp));	
		}			
	}
	return;
}


void Gerber::CalculateArc(_Point cp,_Point sp,_Point ep,int& radius,float& sAngle,float& eAngle)
{
	radius = sqrt(pow(cp.XDisp - sp.XDisp,2) + pow(cp.YReal - sp.YReal,2));
	sAngle = atan(cp.YDisp - sp.YDisp/(cp.XDisp - sp.XDisp))*180/Pie;
	eAngle = atan(cp.YDisp - ep.YDisp/(cp.XDisp - ep.XDisp))*180/Pie;
	
	CString str = _T("");
	str.Format(_T("%d  %f  %f  %f  %f\n"), radius, cp.XDisp, cp.YDisp, sp.XDisp, sp.YDisp);
    OutputDebugString(str);
	return;
}


//**                 SplitGerberRegion                                                                              
//**   @����������   ��ͷ��Ұ���估��·���Ż�                                                                             
void Gerber::SplitGerberRegion(map<int,_Cluster>& mapCluster,int& clusterIndex,map<int,_Polygon>& mapPolygon,int ViewWidth,int ViewHeight)
{
	CString str;
	//Ѱ��Polygon���ϵ���Сtopֵ
	map<int,_Polygon>::iterator itorMapPolygon = mapPolygon.begin();
	itorMapPolygon = min_element( mapPolygon.begin(), mapPolygon.end(),map_PointLessByY());
	_Polygon polygonMinY = itorMapPolygon->second;

	//ɸѡ��[top��Top + ViewHeight]֮���Polygon
	map<int,_Polygon> mapPolygonsByY;
	int mapIndex = 0;
	itorMapPolygon = mapPolygon.begin();
	for ( ; itorMapPolygon != mapPolygon.end(); itorMapPolygon++)
	{
		if (itorMapPolygon->second.DispPolygonMargin.top >= polygonMinY.DispPolygonMargin.top &&
			itorMapPolygon->second.DispPolygonMargin.bottom <= polygonMinY.DispPolygonMargin.top + 2*ViewHeight)
		{
			mapPolygonsByY.insert(make_pair<int,_Polygon>(mapIndex++,itorMapPolygon->second));
		}
	}

	bool IsClusterFirstWorkInMap = true;
	//����top�����ڵ�polygon�������Ԫ�أ�ֱ������Ϊ��
	while (!mapPolygonsByY.empty())
	{
		map<int,_Polygon>::iterator itormapPolygonsByY = mapPolygonsByY.begin();
		//Ѱ��top�����ڼ���Polygon����Сtopֵ
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

		//**************Cluster�߽����***************//
		_Cluster cluster;
		float TopDistMin = Max;
		cluster.clusterMargin.left   = polygonMinX.DispPolygonMargin.left;
		cluster.clusterMargin.top    = polygonMinY.DispPolygonMargin.top;
		itormapPolygonsByY = mapPolygonsByY.begin();
		for ( ; itormapPolygonsByY != mapPolygonsByY.end();itormapPolygonsByY++)
		{
			if (itormapPolygonsByY->second.DispPolygonMargin.left   >= polygonMinX.DispPolygonMargin.left    &&
				itormapPolygonsByY->second.DispPolygonMargin.right  <= polygonMinX.DispPolygonMargin.left + ViewWidth &&
				itormapPolygonsByY->second.DispPolygonMargin.top    >= polygonMinY.DispPolygonMargin.top)
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
				//��top�����ڵ�polygon������ɾ��ѡ�е�Polygon
				mapPolygonsByY.erase(itormapPolygonsByY++);

				//���ܵ�Polygon������ɾ��ѡ�е�Polygon
				itorMapPolygon = find_if(mapPolygon.begin(),mapPolygon.end(),map_finder(polygon));
				mapPolygon.erase(itorMapPolygon);
			}
			else
			{
				itormapPolygonsByY++;
			}
		}
		if (cluster.vecPolygon.size() > 0)
		{
			mapCluster.insert(make_pair<int,_Cluster>(clusterIndex++,cluster));	
		} 
	}
	//�������ͼ�ζ����������ˣ������ݹ�
	if (mapPolygon.empty())
	{
		return;
	}
	SplitGerberRegion(mapCluster,clusterIndex,mapPolygon,ViewWidth,ViewHeight);
}