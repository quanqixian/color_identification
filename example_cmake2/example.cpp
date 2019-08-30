#include<opencv2/opencv.hpp>
#include <stdio.h>
#include<math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "myioctl.h"
 #include <sys/ioctl.h>
using namespace std;
using namespace cv;

Rect roi;
Point pxx;
Point sec,third;

typedef struct  _tag_rgb_threshold_t
{
	unsigned char  b_l;
	unsigned char  g_l;
	unsigned char  r_l;

	unsigned char  b_h;
	unsigned char  g_h;
	unsigned char  r_h;
}rgb_threshold_t;
typedef struct _tag_kernel_read_t
{
	unsigned char mode;
	unsigned char add_flag;
	unsigned char sub_flag;
}kernel_read_t;

int fd;
char buf_txt[100]={0};
rgb_threshold_t rgb_threshold={0,130,0,120,255,110};
kernel_read_t   kernel_read={0,0,0};

/*接收到异步读信号后的动作*/
void input_handler(int signum)
{
  int ret=0;
  printf("receive a signal, signalnum:%d\n",signum);

  ret=read(fd,&kernel_read,3);
  if(ret<0)
  {
	  perror("read");
  }
  printf("[*]mode is -->%d\r\n",kernel_read.mode);
  if(kernel_read.add_flag==1)
  {
	  if(  ((unsigned char *)&rgb_threshold)[kernel_read.mode] <=250)
	  {
		  ((unsigned char *)&rgb_threshold)[kernel_read.mode]+=5;//printf("[*]ctrl is --> +\r\n");
	  }
	ret=ioctl(fd,CLEAR_ADD);
	if(ret!=0)
	{
		perror("ioctl");
	}
  }
  if(kernel_read.sub_flag==1)
  {
	  if(  ((unsigned char *)&rgb_threshold)[kernel_read.mode] >=5)
	  {
		  ((unsigned char *)&rgb_threshold)[kernel_read.mode]-=5;//printf("[*]ctrl is --> -\r\n");
	  }
	ret=ioctl(fd,CLEAR_SUB);
	if(ret!=0)
	{
		perror("ioctl");
	}
  }


}
int main(int argc,const char*argv[])
{
  int oflags;
  fd = open("/dev/demo_key0", O_RDWR);
  if (fd < 0)
  {
	  perror("fai to open");
	  return -1;
  }
  //启动信号驱动机制
  signal(SIGIO, input_handler); //让input_handler()处理SIGIO信号
  fcntl(fd, F_SETOWN, getpid());
  oflags = fcntl(fd, F_GETFL);
  fcntl(fd, F_SETFL, oflags | FASYNC);
	//load video 
	VideoCapture capture(0);
	Mat frame ,mask;
	Mat kernel1=getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
	Mat kernel2=getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
//	namedWindow("input_video",CV_WINDOW_AUTOSIZE);
//	namedWindow("output_video",CV_WINDOW_AUTOSIZE);
	while(1)
	{ 
		capture>>frame; 
		//1. inRange过滤
		inRange(frame,Scalar(rgb_threshold.b_l,rgb_threshold.g_l,rgb_threshold.r_l),
		              Scalar(rgb_threshold.b_h,rgb_threshold.g_h,rgb_threshold.r_h),
					  mask);
		//2.形态学操作提取
		morphologyEx(mask,mask,MORPH_OPEN,kernel1,Point(-1,-1),1);
		//膨胀
		dilate(mask,mask,kernel2,Point(-1,-1),4);
	//	imshow("output_video",mask);
		//3.轮廓查找与位置标定
		vector<vector<Point> > contours;
		vector<Vec4i>hireachy;
		findContours(mask,contours,hireachy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point(0,0));
		//processFrame(mask,roi);
		//rectangle(frame,roi,Scalar(0,0,255),3,8,0);
	    //rectangle(mask,roi,Scalar(255,255,255),3,8,0);
		
		for (int i = 0; i<contours.size(); i++)  
		{  
			//绘制轮廓    
			//drawContours(mask, contours, i, Scalar(255),1,8,hireachy);  
	  
			//绘制轮廓的最小外结矩形    
			RotatedRect rect = minAreaRect(contours[i]);  
			//rectangle(mask,rect.boundingRect(),Scalar(55));  
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
		    circle(frame, pxx, 3,Scalar(255,0,255),-1);//第五个参数我设为-1，表明这是个实点。
			sprintf(buf_txt,"A%d",i);
			putText(frame,buf_txt,pxx,FONT_HERSHEY_PLAIN,3,Scalar(0,255,0),1,8);
			if(0==i)
			{    
				third.x=pxx.x;
				third.y=pxx.y;
			}
		}  
		sprintf(buf_txt,"num:%d",contours.size());
		putText(frame,buf_txt,Point(5,20),FONT_HERSHEY_PLAIN,2,Scalar(0,255,0),1,8);

		sprintf(buf_txt,"%03d %03d %03d %03d %03d %03d",
				rgb_threshold.b_l,
				rgb_threshold.g_l,
				rgb_threshold.r_l,
				rgb_threshold.b_h,
				rgb_threshold.g_h,
				rgb_threshold.r_h
			   );
		putText(frame,buf_txt,Point(5,70),FONT_HERSHEY_PLAIN,1.5,Scalar(0,0,255),1,8);

		sprintf(buf_txt,"%3s %3s %3s %3s %3s %3s",
				kernel_read.mode==0?"___":"   ",
				kernel_read.mode==1?"___":"   ",
				kernel_read.mode==2?"___":"   ",
				kernel_read.mode==3?"___":"   ",
				kernel_read.mode==4?"___":"   ",
				kernel_read.mode==5?"___":"   "
			   );
		putText(frame,buf_txt,Point(5,80),FONT_HERSHEY_PLAIN,1.5,Scalar(0,0,255),1,8);

		//+++++++++++++++++++++++++++
		sec.y=sin(    atan( third.y/(double)third.x )-acos( sqrt( third.x*third.x+third.y*third.y)/800.0 ) )*400;
		sec.x=sqrt(160000-sec.y*sec.y);

		line(frame, Point(0,0), sec, Scalar(0,255,255), 4);  
		line(frame, sec, third, Scalar(255,0,255), 3);  
		imwrite("/pic/1.jpg",frame);
	}
	capture.release();
} 

