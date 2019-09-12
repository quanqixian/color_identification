#include<opencv2/opencv.hpp>
#include <stdio.h>
#include<math.h>
#define ARM_LENGTH 400
using namespace std;
using namespace cv;
Point sec,pxx;
char buf[8]={0};
int main(int argc,const char*argv[])
{
	VideoCapture capture(0);
	Mat frame ,mask;
	Mat kernel1=getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
	Mat kernel2=getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
	namedWindow("input_video",CV_WINDOW_AUTOSIZE);
	namedWindow("output_video",CV_WINDOW_AUTOSIZE);
	int b_h = 80,b_l = 0,g_h = 255,g_l = 110,r_h = 80,r_l = 0;
	cvCreateTrackbar("bh", "output_video", &b_h, 255); 
	cvCreateTrackbar("bl", "output_video", &b_l, 255);
	cvCreateTrackbar("gh", "output_video", &g_h, 255); 
	cvCreateTrackbar("gl", "output_video", &g_l, 255);
	cvCreateTrackbar("rh", "output_video", &r_h, 255); 
	cvCreateTrackbar("rl", "output_video", &r_l, 255);
	while(1)
	{ 
		capture>>frame; 
		//1. inRange过滤
		inRange(frame,Scalar(b_l,g_l,r_l),Scalar(b_h,g_h,r_h),mask);
		imshow("output_video",mask);
		//2.形态学操作提取
		morphologyEx(mask,mask,MORPH_OPEN,kernel1,Point(-1,-1),1);
		//3膨胀
		dilate(mask,mask,kernel2,Point(-1,-1),4);
		//4.轮廓查找与位置标定
		vector<vector<Point> > contours;
		vector<Vec4i>hireachy;
		findContours(mask,contours,hireachy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point(0,0));
		for (int i = 0; i<contours.size(); i++)  
		{  
			//绘制轮廓    
			//drawContours(mask, contours, i, Scalar(255),1,8,hireachy);  
			//5.绘制轮廓的最小外结矩形    
			RotatedRect rect = minAreaRect(contours[i]);  
			Point2f P[4];  
			rect.points(P);  
			for (int j = 0; j <= 3; j++)  
			{  
				line(frame, P[j], P[(j + 1) % 4], Scalar(0,0,255), 3);  
				line(mask, P[j], P[(j + 1) % 4], Scalar(111), 3);  
			} 
			pxx.x=(P[0].x+P[2].x)/2;
			pxx.y=(P[0].y+P[2].y)/2;
			//画实心点
		    circle(frame, pxx, 3,Scalar(255,0,255),-1);//第五个参数设为-1，表明这是个实点。
			sprintf(buf,"A%d",i);
			putText(frame,buf,pxx,FONT_HERSHEY_PLAIN,3,Scalar(0,255,0),1,8);
		}  
		sprintf(buf,"num:%d",contours.size());
		putText(frame,buf,Point(510,20),FONT_HERSHEY_PLAIN,2,Scalar(0,255,255),2,8);
		sec.y=sin(    atan( pxx.y/(double)pxx.x )-acos( sqrt( pxx.x*pxx.x+pxx.y*pxx.y)/(2*ARM_LENGTH) ) )*ARM_LENGTH;
		sec.x=sqrt(ARM_LENGTH*ARM_LENGTH-sec.y*sec.y);
		line(frame, Point(0,0), sec, Scalar(0,0,255), 4);  
		line(frame, sec, pxx, Scalar(255,0,255), 4);  
		imshow("input_video",frame);
		char c=waitKey(10);
		if(c==27)
			break;
	}
	capture.release();
}
