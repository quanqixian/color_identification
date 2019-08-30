#ifndef __MYIOCTL_H_
#define __MYIOCTL_H_
#include <asm-generic/ioctl.h>

#define TYPE 'A'
#define CLEAR_ADD _IO(TYPE,1)
#define CLEAR_SUB _IO(TYPE,2)

#endif
