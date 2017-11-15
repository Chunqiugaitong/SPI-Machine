#include "StdAfx.h"
#include "PcbInspect.h"

using namespace cv;
using namespace std;  
int MAX_KERNEL_LENGTH = 31;

CPcbInspect::CPcbInspect(void)
{
	
}
CPcbInspect::~CPcbInspect(void)
{
}

/************************************************************************/
/*                         图像显示到MFC界面                                                                    
/************************************************************************/

IplImage* CPcbInspect::ReadImage(string imgPath)
{
	IplImage* pImg = cvLoadImage(imgPath.c_str(),CV_LOAD_IMAGE_UNCHANGED);
	if (NULL == pImg)
	{
		AfxMessageBox(_T("图像加载失败！"), MB_OK);
		return pImg;
	}
	return pImg;
}
void CPcbInspect::ShowImage(HDC hPicDC,IplImage* pSrcImg,CRect picRect,float xf,float yf)
{
	picRect.right = picRect.left + pSrcImg->width /xf;
	picRect.bottom = picRect.top + pSrcImg->height /yf;
	CvvImage cimg;
	cimg.CopyOf(pSrcImg);               // 复制图片
	cimg.DrawToHDC(hPicDC, &picRect);  // 将图片绘制到显示控件的指定区域内

}
void CPcbInspect::ShowImage(HDC hPicDC,Mat imgMat,CRect picRect,float xf,float yf)
{
	picRect.right = picRect.left + imgMat.cols/xf;
	picRect.bottom = picRect.top + imgMat.rows/yf;

	IplImage img = imgMat;
	CvvImage cimg;
	cimg.CopyOf(&img);                          // 复制图片
	cimg.DrawToHDC(hPicDC, &picRect);                // 将图片绘制到显示控件的指定区域内

}
Mat CPcbInspect::IplImage2Mat(IplImage iplImage)
{
	//! converts old-style IplImage to the new matrix; the data is not copied by default
	Mat mat(&iplImage);
	return mat;
}
IplImage CPcbInspect::Mat2IplImage(Mat matImage)
{
	//IplImage iplImage = matImage; //方法一
	IplImage iplImage(matImage) ; //方法二
	return iplImage;
}
//bmp转Iplimage*
IplImage* CPcbInspect::Bmp2Ipl(HBITMAP hBmp) 
{
	BITMAP bmp;       
	GetObject(hBmp, sizeof(BITMAP), &bmp);   
	int depth     = (bmp.bmBitsPixel == 1) ? IPL_DEPTH_1U : IPL_DEPTH_8U;   
	int nChannels = (bmp.bmBitsPixel == 1) ? 1 : bmp.bmBitsPixel/8;     
	//创建图像结构头并分配数据 cvCreateImageHeader
	IplImage* img = cvCreateImage(cvSize(bmp.bmWidth,bmp.bmHeight), depth, nChannels); 

	BYTE *pBuffer = new BYTE[bmp.bmHeight*bmp.bmWidth*nChannels];       
	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, pBuffer);   
	memcpy(img->imageData, pBuffer, bmp.bmHeight*bmp.bmWidth*nChannels);      
	delete[] pBuffer;  
	pBuffer = NULL;
// 	IplImage *dst = cvCreateImage(cvGetSize(img), img->depth,nChannels);       
// 	cvCvtColor(img, dst, CV_BGRA2BGR);      
 //	cvReleaseImage(&img);      

	return img;        
}

void CPcbInspect::ReleaseImage(IplImage* pImg)
{
	if (pImg!=NULL)
	{
		cvReleaseImage(&pImg);
	}
}



Mat CPcbInspect::SetRoiRect(IplImage* srcImg,CRect roiRect,float xf,float yf)
{
	IplImage* pRoiImg = cvCreateImage((cvSize(xf*roiRect.Width(),yf*roiRect.Height())),srcImg->depth,srcImg->nChannels);
	cvSetImageROI(srcImg,cvRect(xf*roiRect.left,yf*roiRect.top,xf*roiRect.Width(),yf*roiRect.Height()));
	cvCopy(srcImg,pRoiImg,NULL);
	//重置，如果不重置，则img只会显示感兴趣的矩形一部分
	cvResetImageROI(srcImg);
	return Mat(pRoiImg);
}
/************************************************************************/
/*                            找圆算法                                                                    
/************************************************************************/

//梯度霍夫圆提取
   void CPcbInspect::Gxmc_FindCircle(InputArray _image, vector<circle_found>& _circles,float dp, int min_dist,
   					 int low_threshold, int high_threshold,int acc_threshold,int minRadius, int maxRadius,
   					 float minScore, InputArray _contour_image)
   {
   	//ExtendCV::FindCircles(_image,_circles,dp,min_dist,low_threshold,high_threshold,acc_threshold,minRadius,maxRadius,minScore,_contour_image);
   }

//最小二乘法拟合圆
Vec3f CPcbInspect::circleLeastFit(const vector<CvPoint*> &points)  
{  	
	Vec3f circle;  
	circle[0] = 0.0f;  
	circle[1] = 0.0f;  
	circle[2] = 0.0f;  

	if (points.size() < 3)  
	{  
		return circle;  
	}  

	int i=0;  

	double X1=0;  
	double Y1=0;  
	double X2=0;  
	double Y2=0;  
	double X3=0;  
	double Y3=0;  
	double X1Y1=0;  
	double X1Y2=0;  
	double X2Y1=0;  

	int Sum = points.size();  
	for (i=0;i<Sum;i++)  
	{  
		X1 = X1 + points[i]->x;  
		Y1 = Y1 + points[i]->y;  
		X2 = X2 + points[i]->x*points[i]->x;  
		Y2 = Y2 + points[i]->y*points[i]->y;  
		X3 = X3 + points[i]->x*points[i]->x*points[i]->x;  
		Y3 = Y3 + points[i]->y*points[i]->y*points[i]->y;  
		X1Y1 = X1Y1 + points[i]->x*points[i]->y;  
		X1Y2 = X1Y2 + points[i]->x*points[i]->y*points[i]->y;  
		X2Y1 = X2Y1 + points[i]->x*points[i]->x*points[i]->y;  
	}  

	double C,D,E,G,H,N;  
	double a,b,c;  
	N = points.size();  
	C = N*X2 - X1*X1;  
	D = N*X1Y1 - X1*Y1;  
	E = N*X3 + N*X1Y2 - (X2+Y2)*X1;  
	G = N*Y2 - Y1*Y1;  
	H = N*X2Y1 + N*Y3 - (X2+Y2)*Y1;  
	a = (H*D-E*G)/(C*G-D*D);  
	b = (H*C-E*D)/(D*D-G*C);  
	c = -(a*X1 + b*Y1 + X2 + Y2)/N;  

	double A,B,R;  
	A = a/(-2);  
	B = b/(-2);  
	R = sqrt(a*a+b*b-4*c)/2;  

	circle[0]= A;  
	circle[1] = B;  
	circle[2] = R;  

	return circle;  
}

Vec3f CPcbInspect::CircleFit(Mat& inputImg)
{
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* first_contours = NULL;
	IplImage pImg(inputImg);

	cvSmooth(&pImg,&pImg,CV_GAUSSIAN,3,3,0,0);
	double nThreshold = getThreshVal_Otsu_8u(inputImg);
	cvThreshold (&pImg, &pImg, nThreshold, 255, CV_THRESH_BINARY); 
	int nContours = cvFindContours(&pImg,storage,&first_contours/*,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE*/);

	cvDrawContours(&pImg,first_contours,cvScalar(255,0,0),cvScalar(255,255,0),1);
	
	CvPoint* point;
	vector<CvPoint*> vecPoints;
	CvSeq* tempContour = first_contours;
 	
 	for (;tempContour != NULL;tempContour = tempContour->h_next)
 	{
 		//cvDrawContours(&pImg,tempContour,cvScalar(255,0,0),cvScalar(255,255,0),1);
 	}

	for (int i =0; i< first_contours->total;i++)
	{
		point = (CvPoint*)cvGetSeqElem(first_contours,i);
		vecPoints.push_back(point);
	}

	Vec3f circle = circleLeastFit(vecPoints);
	cvCircle(&pImg,cvPoint(circle[0],circle[1]),circle[2],cvScalar(255,0,0),3);

	cvReleaseMemStorage(&storage);
	return circle;
}
//大津展之阈值分割--Ostu
double CPcbInspect::getThreshVal_Otsu_8u( const Mat& _src )
{
	Size size = _src.size();
	if( _src.isContinuous() )
	{
		size.width *= size.height;
		size.height = 1;
	}
	const int N = 256;
	int i, j, h[N] = {0};
	for( i = 0; i < size.height; i++ )
	{
		const uchar* src = _src.data + _src.step*i;
		j = 0;
        #if CV_ENABLE_UNROLLED
		for( ; j <= size.width - 4; j += 4 )
		{
			int v0 = src[j], v1 = src[j+1];
			h[v0]++; h[v1]++;
			v0 = src[j+2]; v1 = src[j+3];
			h[v0]++; h[v1]++;
		}
        #endif
		for( ; j < size.width; j++ )
			h[src[j]]++;
	}

	double mu = 0, scale = 1./(size.width*size.height);
	for( i = 0; i < N; i++ )
		mu += i*(double)h[i];

	mu *= scale;
	double mu1 = 0, q1 = 0;
	double max_sigma = 0, max_val = 0;

	for( i = 0; i < N; i++ )
	{
		double p_i, q2, mu2, sigma;

		p_i = h[i]*scale;
		mu1 *= q1;
		q1 += p_i;
		q2 = 1. - q1;

		if( std::min(q1,q2) < FLT_EPSILON || std::max(q1,q2) > 1. - FLT_EPSILON )
			continue;

		mu1 = (mu1 + i*p_i)/q1;
		mu2 = (mu - q1*mu1)/q2;
		sigma = q1*q2*(mu1 - mu2)*(mu1 - mu2);
		if( sigma > max_sigma )
		{
			max_sigma = sigma;
			max_val = i;
		}
	}
	return max_val;
}

// 计算
void CPcbInspect::PcbMap2PcbRobot(Point2f PicturePoint1,Point2f PicturePoint2,Point2f RobotPoint1,Point2f RobotPoint2,float& fX,float& fY)
{
	RobotPoint1 = Point2f(25.175,30.085);
	RobotPoint2 = Point2f(225.370,230.181);
	//比例因子f PCB机械手坐标（mm）/MAP像素坐标(pixel)
	fX = (RobotPoint1.x - RobotPoint2.x)/(PicturePoint1.x - PicturePoint2.x);
	fY = (RobotPoint1.y - RobotPoint2.y)/(PicturePoint1.y - PicturePoint2.y);
	return;
}
 
    