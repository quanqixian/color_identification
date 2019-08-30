#!/bin/bash 
make&&cp *.ko /home/quan/nfs  
arm-linux-gcc test.c&&cp a.out /home/quan/nfs/ 
