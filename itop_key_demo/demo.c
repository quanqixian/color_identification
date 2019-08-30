#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/poll.h>

#include "myioctl.h"

#define NAME "demo_key"
int count = 1;
int major = 0 , minor = 0;
struct class *cls = NULL;
struct device *dev = NULL;
struct device_node *np = NULL;
int gpx2_0,gpx2_1,gpx3_3;
int irq_0 ,irq_1 ,irq_2;
//  val- val+ Sleep
struct  timer_list tm;
struct fasync_struct *fasync = NULL;
char  ker_buf[3]={0};
/*按键电平的再次确认*/
void key_function(unsigned long data)
{
	if(!gpio_get_value(gpx2_0)){
		while(!gpio_get_value(gpx2_0));
		printk("key0 enter...\n");
		ker_buf[0]++;
		if(ker_buf[0]>=6)
		{
			ker_buf[0]=0;
		}
		kill_fasync(&fasync,SIGIO,POLL_IN);
	}

	if(!gpio_get_value(gpx2_1)){
		while(!gpio_get_value(gpx2_1));
		printk("key1 enter...\n");
		ker_buf[1]=1;
		kill_fasync(&fasync,SIGIO,POLL_IN);
	}

	if(!gpio_get_value(gpx3_3)){
		while(!gpio_get_value(gpx3_3));
		printk("key2 enter...\n");
		ker_buf[2]=1;
		kill_fasync(&fasync,SIGIO,POLL_IN);
	}
}

/*中断处理函数*/
irqreturn_t key0_irq_handler(int irq, void *data)
{
	mod_timer(&tm,jiffies + HZ/100);
	return IRQ_HANDLED;
}
/*中断处理函数*/
irqreturn_t key1_irq_handler(int irq, void *data)
{
	mod_timer(&tm,jiffies + HZ/100);
	return IRQ_HANDLED;
}
/*中断处理函数*/
irqreturn_t key2_irq_handler(int irq, void *data)
{
	mod_timer(&tm,jiffies + HZ/100);
	return IRQ_HANDLED;
}


int demo_open(struct inode *inode, struct file *file)
{
	printk("%s,%d\n",__func__,__LINE__);
	return 0;
}
int demo_close(struct inode *inode, struct file *file)
{
	printk("%s,%d\n",__func__,__LINE__);
	return 0;
}
ssize_t demo_read(struct file *file, char __user *user, size_t size, loff_t *loff)
{

	if(copy_to_user(user,ker_buf,size)){
		printk("copy_to_user fail...\n");
		return -EINVAL;
	}
	printk("%s,%d\n",__func__,__LINE__);
	return size;
}
long demo_ioctl (struct file *file, unsigned int cmd , unsigned long arg)
{
	switch(cmd)
	{
	case CLEAR_ADD:
		ker_buf[1]=0;
		printk("CLEAR_ADD\n");
		break;
	case CLEAR_SUB:
		ker_buf[2]=0;
		printk("CLEAR_SUB\n");
		break;
	default:
		break;
	}
	return 0;
}

int demo_fasync (int fd , struct file *file, int on)
{
	return fasync_helper(fd,file,on,&fasync);
}
struct file_operations f_ops = {
	.owner = THIS_MODULE,
	.open = demo_open,
	.release = demo_close,
	.fasync  = demo_fasync,
	.read=demo_read,
	.unlocked_ioctl=demo_ioctl,
};

int __init demo_init(void)
{
	int i ,ret = 0;
	major = register_chrdev(major,NAME,&f_ops);
	if(major < 0){
		printk("register_chrdev fail ...%s,%d\n",__func__,__LINE__);
		return -EINVAL;
	}
	cls = class_create(THIS_MODULE,"demo_key_class");
	if(IS_ERR(cls)){
		printk("class_create ...\n");
		ret = PTR_ERR(cls);
		goto ERR_STEP1;
	}
	for(i = minor ; i < minor + count ; i++){
		dev = device_create(cls,NULL,MKDEV(major,i),NULL,"%s%d","demo_key",i);
		if(IS_ERR(dev)){
			printk("class_create ...\n");
			ret = PTR_ERR(dev);
			goto ERR_STEP2;
		}
	}
	/*-------------------------device_tree----------------------------*/
	/*1 获取设备key节点*/
	np = of_find_node_by_path("/itop-key");
	if(np == NULL){
		printk("of_find_node_by_path...%s,%d\n",__func__,__LINE__);
		goto ERR_STEP2;
	}
	gpx2_0 = of_get_named_gpio(np,"vol_down",0);
	if(gpx2_0 < 0){
		printk("of_get_named_gpio...%s,%d\n",__func__,__LINE__);
		goto ERR_STEP2;
	}
	gpx2_1 = of_get_named_gpio(np,"vol_up",0);
	if(gpx2_1 < 0){
		printk("of_get_named_gpio...%s,%d\n",__func__,__LINE__);
		goto ERR_STEP2;
	}
	gpx3_3 = of_get_named_gpio(np,"home",0);
	if(gpx3_3 < 0){
		printk("of_get_named_gpio...%s,%d\n",__func__,__LINE__);
		goto ERR_STEP2;
	}

	/*2.将gpio管脚号---->irq*/
	irq_0 = gpio_to_irq(gpx2_0);
	irq_1 = gpio_to_irq(gpx2_1);
	irq_2 = gpio_to_irq(gpx3_3);
	//printk("irq:%d\n",irq);
	if((irq_0 < 0)||(irq_1<0)||(irq_2<0)){
		printk("gpio_to_irq...%s,%d\n",__func__,__LINE__);
		goto ERR_STEP2;
	}

	ret = request_irq(irq_0,key0_irq_handler,IRQF_TRIGGER_FALLING,"KEY0",NULL);
	if(ret < 0){
		printk("request_irq...%s,%d\n",__func__,__LINE__);
		goto ERR_STEP2;
	}
	ret = request_irq(irq_1,key1_irq_handler,IRQF_TRIGGER_FALLING,"KEY1",NULL);
	if(ret < 0){
		printk("request_irq...%s,%d\n",__func__,__LINE__);
		goto ERR_STEP2;
	}
	ret = request_irq(irq_2,key2_irq_handler,IRQF_TRIGGER_FALLING,"KEY2",NULL);
	if(ret < 0){
		printk("request_irq...%s,%d\n",__func__,__LINE__);
		goto ERR_STEP2;
	}


	/*3.按键消抖*/
//	tm.expries = jiffies + HZ/100;
	tm.function = key_function;
	init_timer(&tm);
	printk("%s,%d\n",__func__,__LINE__);
	return 0;
	
ERR_STEP2:
	for(i-- ; i >= minor ; i--)
		device_destroy(cls,MKDEV(major,i));
	class_destroy(cls);
ERR_STEP1:
	unregister_chrdev(major,NAME);
	return ret;
}

void __exit demo_exit(void)
{
	int i ;
	del_timer(&tm);
	free_irq(irq_0,NULL);
	free_irq(irq_1,NULL);
	free_irq(irq_2,NULL);
	for(i = minor ; i < minor + count ; i++)
		device_destroy(cls,MKDEV(major,i));
	class_destroy(cls);
	unregister_chrdev(major,NAME);
	printk("%s,%d\n",__func__,__LINE__);
}

module_init(demo_init);
module_exit(demo_exit);
MODULE_LICENSE("GPL");










