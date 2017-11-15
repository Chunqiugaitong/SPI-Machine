#pragma once
#include "DataStruct.h"

#define ARRAYLEN 512
class Gerber
{
public:
	Gerber(void);
	~Gerber(void);


	vector<_Region> GetRealRegions(FILE* pFile,CRect& marginReal);
	void DrawRegions(CDC &memDC,vector<_Region> vecRegions,map<int,_Cluster> mapCluster);
	void GetDispRegions(vector<_Region>& vecRegions,CRect& RegionMargin,float xoffset,float yoffset,int rate);
	void PreDrawRegions(FILE* pFile, vector<_Region>& vecRegions,CRect& RegionMargin,map<int,_Cluster>& mapCluster,int rate, float ViewWidth, float ViewHeight);

    void CalculateArc(_Point cp, _Point sp, _Point ep, int& radius, float& sAngle, float& eAngle);
	void SplitGerberRegion(map<int,_Cluster>& mapCluster,int& clusterIndex,map<int,_Polygon>& mapPolygon,int ViewWidth,int ViewHeight);
	
private:
	_TipInfo TipInfo;
	vector<_ApertureInfo> vecApertureInfo;
	vector<_Region> m_vecRegions;
	CRect m_MarginReal;

	vector<_Polygon> m_vecPolygon;

	int m_leftMargin;
	int m_rightMargin;
	int m_topMargin;
	int m_bottomMargin;

public:
	CRect m_MarginDisp;
	map<int,_Cluster> m_mapCluster;

private:
	int GetLine(FILE*, char*);
	int HandleStrXY(float&, float&, char*, _TipInfo,CRect& marginReal);   //处理提取出来的一行字符
	int HandleStrXYIJ(float&, float&, float&, float&, char*, _TipInfo,CRect& marginReal);   //处理提取出来的一行字符
	void ReadNumberFromStr(char*, int, float& nX, float& nY);
	void GetRegionRealMargin(vector<_Polygon>& vecPolygon);
	void UpdateMargin(CRect& margin,int nCurX,int nCurY);
	void UpdateMargin(CRect& margin,int nCurleft,int nCurTop,int nCurRight,int nCurBottom);
	void GetDataFormatParam(FILE* pFile, char* charTemp);
	void GetG54Info(FILE *ifp,char* tmp,_TipInfo tipInfo,vector<_Region>& vecRegion,CRect& marginReal,eApertureShape ApertureShape);
	void GetG36Info(FILE *ifp,char* tmp,_TipInfo tipInfo,vector<_Region>& vecRegion,CRect& marginReal);
};

