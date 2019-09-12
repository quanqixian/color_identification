#include<opencv2/opencv.hpp>
#include <stdio.h>
using namespace std;
using namespace cv;
int main(int argc,const char*argv[])
{
	VideoCapture capture(0);
	Mat frame ,mask;
	Mat kernel1=getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
	Mat kernel2=getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
	namedWindow("input_video",CV_WINDOW_AUTOSIZE);
	namedWindow("output_video",CV_WINDOW_AUTOSIZE);
	namedWindow("control",CV_WINDOW_AUTOSIZE);
	int b_h = 80,b_l = 0,g_h = 255,g_l = 110,r_h = 80,r_l = 0;
	cvCreateTrackbar("bh", "control", &b_h, 255); 
	cvCreateTrackbar("bl", "control", &b_l, 255);
	cvCreateTrackbar("gh", "control", &g_h, 255);  
	cvCreateTrackbar("gl", "control", &g_l, 255);
	cvCreateTrackbar("rh", "control", &r_h, 255);  
	cvCreateTrackbar("rl", "control", &r_l, 255);
	while(1)
	{ 
		capture>>frame; 
		imshow("input_video",frame);
		//1. inRange过滤
		inRange(frame,Scalar(b_l,g_l,r_l),Scalar(b_h,g_h,r_h),mask);
		//2.形态学操作提取
		morphologyEx(mask,mask,MORPH_OPEN,kernel1,Point(-1,-1),1);
		//膨胀
		dilate(mask,mask,kernel2,Point(-1,-1),4);
		//3.轮廓查找与位置标定
		vector<vector<Point> > contours;
		vector<Vec4i>hireachy;
		findContours(mask,contours,hireachy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point(0,0));
		for (int i = 0; i<contours.size(); i++)  
		{  
			//绘制轮廓    
			drawContours(mask, contours, i, Scalar(255),1,8,hireachy);  
			//绘制轮廓的最小外结矩形    
			RotatedRect rect = minAreaRect(contours[i]);  
			Point2f P[4];
			rect.points(P);  
		}  
		imshow("output_video",mask);
		char c=waitKey(10);
		if(c==27)
			break;
	}
	capture.release();
}
