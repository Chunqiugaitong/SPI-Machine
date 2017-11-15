#pragma once

#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include <sstream>
#include "..\\ImageProcess\\ExtendHoughCircle.h"

using namespace cv;
using namespace std;

class CPcbInspect
{
public:
	CPcbInspect(void);
	~CPcbInspect(void);

	IplImage* ReadImage(string imgPath);
	void ShowImage(HDC hPicDC,Mat imgMat,CRect picRect,float xf,float yf);
	void ShowImage(HDC hPicDC,IplImage* pSrcImg,CRect picRect,float xf,float yf);
	Mat IplImage2Mat(IplImage iplImage);
	IplImage Mat2IplImage(Mat matImage);   
	IplImage* Bmp2Ipl(HBITMAP hBmp);

	void ReleaseImage(IplImage* pImg);

	Mat SetRoiRect(IplImage* srcImg,CRect roiRect,float xf,float yf);
    //梯度hough圆优化
  	void Gxmc_FindCircle(InputArray _image, vector<circle_found>& _circles,float dp, int min_dist,
  						int low_threshold, int high_threshold,int acc_threshold,int minRadius, int maxRadius,float minScore, InputArray _contour_image=Mat() );
 	//最小二乘法拟合圆
	Vec3f circleLeastFit(const vector<CvPoint*> &points);  
	Vec3f CircleFit(Mat& inputImg);
	double getThreshVal_Otsu_8u( const Mat& _src );

	//像素坐标与机械手坐标之间的映射
	void PcbMap2PcbRobot(Point2f PicturePoint1,Point2f PicturePoint2,Point2f RobotPoint1,Point2f RobotPoint2,float& fX,float& fY);
};


	