//

#include <vector>
#include <map>
#include <deque>

using namespace std;

#define Pie 3.1415926
#define Max  100000
#define Min -100000

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

struct  _ApertureInfo
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

struct _Point 
{
	float XReal;
	float YReal;

	float XDisp;
	float YDisp;

	_Point()
	{
		XReal = 0.000;
		YReal = 0.000;
		XDisp = 0.000;
		YDisp = 0.000;
	}

	void operator= (const _Point& point)
	{
 		XReal = point.XReal;
 		YReal = point.YReal;
 		XDisp = point.XDisp;
 		YDisp = point.YDisp;	
	}

};

struct _Arc
{
	_Point StartPoint;
	_Point EndPoint;
	_Point CenterPoint;
	eDirection Dir;
	eArcStyle arcStyle;
};

struct _Rectangle 
{
	float left;
	float right;
	float top;
	float bottom;

	void operator= (const _Rectangle& rectangle)
	{
		left = rectangle.left;
		right = rectangle.right;
		top = rectangle.top;
		bottom = rectangle.bottom;
	}
	bool operator == (const _Rectangle& rectangle)
	{
		return left == rectangle.left  &&
			   right ==rectangle.right &&
			   top == rectangle.top    &&
			   bottom == rectangle.bottom;
	}

};

struct _Polygon 
{
	_Point StartDrawPoint;
	vector<_Point> vecPoint;
	vector<_Arc> vecArc;
	_Point AveragePoint;
// 	CRect RealPolygonMargin;
// 	CRect DispPolygonMargin;
	_Rectangle RealPolygonMargin;
	_Rectangle DispPolygonMargin;
	bool isPolygon;
	bool isSelected;
	bool operator == (const _Polygon& polygon)
	{
		return RealPolygonMargin == polygon.RealPolygonMargin;
	}
};

struct _Circle
{
	float Radius;
	_Point CenterPoint;
};
struct _Ellipse 
{
	float LongAxis;
	float ShortAxis;
	_Point CenterPoint;
};

//椭圆外切矩形
struct _RectangleEx 
{
	float LongAxis;
	float ShortAxis;
	_Point CenterPoint;
};

struct _Custom 
{
	float LongAxis;
	float ShortAxis;
	_Point CenterPoint;
};



struct _RoiRegion 
{
	CRect RoiMargin;
	int RoiIndex;
};

struct _Region
{
	_Polygon Polygon;
	vector<_Circle> vecCircle;
	vector<_Ellipse> vecEllipse;
	vector<_RectangleEx> vecRect;
	vector<_Custom> vecCustom;
	bool isClustered;
	_Point centerPoint;
	 
};

struct IntervalY 
{
	int Ymin;
	int Ymax;
};

inline bool PointLessByX(const _Polygon& point1,const _Polygon& point2)
{
	return point1.DispPolygonMargin.left < point2.DispPolygonMargin.left;
}

inline bool PointLessByY(const _Polygon& point1,const _Polygon& point2)
{
	return point1.DispPolygonMargin.top < point2.DispPolygonMargin.top;
}

inline bool PointGreaterByX(const _Polygon& point1,const _Polygon& point2)
{
	return point1.DispPolygonMargin.left > point2.DispPolygonMargin.left;
}

inline bool PointGreaterByY(const _Polygon& point1,const _Polygon& point2)
{
	return point1.DispPolygonMargin.top > point2.DispPolygonMargin.top;
}

inline bool nLess(const int& n1,const int& n2)
{
	return n1 < n2;
}
inline bool nGreater(const int& n1,const int& n2)
{
	return n1 > n2;
}


struct  _Node
{
	_Point nodePoint;
	bool isCluster;
};
struct _Cluster 
{
	vector<_Polygon> vecPolygon;
	_Point centerPoint;
	_Rectangle clusterMargin;
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

struct map_PointLessByX 
{
public:
	map_PointLessByX(){}
	bool operator() (const map<int,_Polygon>::value_type& pair1,const map<int,_Polygon>::value_type& pair2)
	{
        return pair1.second.DispPolygonMargin.left < pair2.second.DispPolygonMargin.left;
	}
};

struct map_PointLessByY 
{
public:
	map_PointLessByY(){}
	bool operator() (const map<int,_Polygon>::value_type& pair1,const map<int,_Polygon>::value_type& pair2)
	{
		return pair1.second.DispPolygonMargin.top < pair2.second.DispPolygonMargin.top;
	}
};

//到矩形边界最近的多边形的边界

struct map_LessDistanceTop 
{
public:
	map_LessDistanceTop(const _Cluster& cluster):map_cluster(cluster){}
	bool operator() (const _Polygon& polygon1,const _Polygon& polygon2)
	{
		double dist1 = polygon1.DispPolygonMargin.top - map_cluster.clusterMargin.bottom;
		double dist2 = polygon1.DispPolygonMargin.top - map_cluster.clusterMargin.bottom;
		return dist1 < dist2;
	}

private:
	const _Cluster& map_cluster;
};

struct map_LessDistanceLeft 
{
public:
	map_LessDistanceLeft(const _Cluster& cluster):map_cluster(cluster){}
	bool operator() (const _Polygon& polygon1,const _Polygon& polygon2)
	{
		double dist1 = polygon1.DispPolygonMargin.left - map_cluster.clusterMargin.left;
		double dist2 = polygon1.DispPolygonMargin.left - map_cluster.clusterMargin.left;
		return dist1 < dist2;
	}

private:
	const _Cluster& map_cluster;
};

// strceCriteria : std::binary_function<DPoint3d,DPoint3d,bool>
// {
// 	distanceCriteria(DPoint3d* targetPt):m_targetPt(*targetPt){}
// 	bool operator()(const DPoint3d& pt1,const DPoint3d& pt2)
// 	{
// 		double dis1 = mdlVec_distanceXY((DVec3d*)&pt1,(DVec3d*)&m_targetPt);
// 		double dis2 = mdlVec_distanceXY((DVec3d*)&pt2,(DVec3d*)&m_targetPt);
// 		return ( dis1 < dis2 );
// 	}
// private:
// 	DPoint3d m_targetPt;
// };
// 
// DPoint3d* getShortest(DPoint3d* pArray,size_t ptNums,DPoint3d* targetPt)
// {
// 	return std::min_element(pArray, pArray + ptNums, distanceCriteria(targetPt));
// }