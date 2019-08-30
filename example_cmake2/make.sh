#!/bin/bash 
make
if [ $? -eq 0 ]
then      
	echo "[__   make   ok    __]"
else
	echo "[**   make   fail! **]"
	exit
fi 		
cp ./opencv_example /home/quan/nfs/ 
echo "[__ cp to nfs ok __]"
