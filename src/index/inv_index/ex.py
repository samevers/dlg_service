#! /usr/bin/python 
#encoding:gb18030
############################################
#
# Author: 
# Create time: 2015 9月 21 15时17分07秒
# E-Mail: @sogou-inc.com
# version 1.0
#
############################################


import sys,os

for line in sys.stdin:
	line = line.strip()
	parts = line.split("\t")
	if(len(parts) < 2):
		continue
	if(len(parts[0]) >= 8):
		print line
