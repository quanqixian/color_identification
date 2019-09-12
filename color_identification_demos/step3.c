#include<opencv2/opencv.hpp>
#include <stdio.h>
using namespace std;
using namespace cv;
int main(int argc,const char*argv[])
{
	Mat src,dst,dst2;//定义Mat变量，用于存储每一帧的图像
	Mat kernel1=getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
	int b_h = 80,b_l = 0,g_h = 255,g_l = 110,r_h = 80,r_l = 0;
	namedWindow("output",CV_WINDOW_AUTOSIZE);
	namedWindow("control",CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar("bh", "control", &b_h, 255); 
	cvCreateTrackbar("bl", "control", &b_l, 255);
	cvCreateTrackbar("gh", "control", &g_h, 255); 
	cvCreateTrackbar("gl", "control", &g_l, 255);
	cvCreateTrackbar("rh", "control", &r_h, 255);  
	cvCreateTrackbar("rl", "control", &r_l, 255);
	VideoCapture capture (0);//从摄像头读取视频
	//2.循环显示每一帧
	while(1)
	{
		capture>>src;//读取当前帧
		//1. inRange过滤
		inRange(src,Scalar(b_l,g_l,r_l),Scalar(b_h,g_h,r_h),dst);
		imshow("output",dst);
		//2.形态学操作
		morphologyEx(dst,dst2,MORPH_OPEN,kernel1,Point(-1,-1),5);
		imshow("dst2",dst2);
		char c=waitKey(30);
		if(c==27)
			break;
	}
	return 0;
}
