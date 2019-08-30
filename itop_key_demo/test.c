/*======================================================================
    A test program to access /dev/second
    This example is to help understand async IO 
    
    The initial developer of the original code is Baohua Song
    <author@linuxdriver.cn>. All Rights Reserved.
======================================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "myioctl.h"

int fd;
char buf[3]={0};

/*接收到异步读信号后的动作*/
void input_handler(int signum)
{
  int ret=0;
  printf("receive a signal, signalnum:%d\n",signum);

  ret=read(fd,buf,3);
  if(ret<0)
  {
	  perror("read");
  }
  printf("[*]mode is -->%d\r\n",buf[0]);
  if(buf[1]==1)
  {
  	printf("[*]ctrl is --> +\r\n");
	ret=ioctl(fd,CLEAR_ADD);
	if(ret!=0)
	{
		perror("ioctl");
	}
  }
  if(buf[2]==1)
  {
  	printf("[*]ctrl is --> -\r\n");
	ret=ioctl(fd,CLEAR_SUB);
	if(ret!=0)
	{
		perror("ioctl");
	}
  }


}

int main()
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
  while(1)
  {
	  sleep(100);
  }

  return 0;
}
