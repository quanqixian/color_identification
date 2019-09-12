#include<opencv2/opencv.hpp>
using namespace cv;
int main()
{
	//1.从摄像头读取视频
	VideoCapture capture (0);
	//2.循环显示每一帧
	while(1)
	{
		Mat frame;//定义一个Mat变量，用于存储每一帧的图像
		capture>>frame;//读取当前帧
		imshow("读取视频",frame);//显示当前帧
		char c=waitKey(30);
		if(c==27)
			break;
	}
	return 0;
}


