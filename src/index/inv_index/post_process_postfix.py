#! /usr/bin/python 
#encoding:gb18030
############################################
#
# Author: 
# Create time: 2015 9月 22 11时28分04秒
# E-Mail: @sogou-inc.com
# version 1.0
#
############################################


import sys,os
sys.path.append("sk_utils")
import gl

if __name__ == "__main__":
	for line in sys.stdin:
		line = line.strip()
		parts = line.split("\t")

		if(len(parts) < 2):
			continue
		
		parts[1] = gl.strQ2B(parts[1].decode("gb18030")).encode("gb18030")
		sub_parts = parts[0].split("#_#")

		tmp_str = "".join(sub_parts)
		result = gl.GetEngWordNum(tmp_str)
		if(len(result) == 0):
			sys.stderr.write("result empty\n")
			continue
		
		if(result[0] != 0 or result[2] != 0 or result[3] != 0):
			sys.stderr.write("contain non chinese %s %d %d %d\n" %(tmp_str,result[0],result[2],result[3]))
			continue

		if(len(sub_parts) < 2):
			#只有一个部分
			if(len(sub_parts[0]) <= 2):
				sys.stderr.write("too short\n")
				continue
			if(int(parts[1]) > 8000):
				print parts[0] + "\t" + parts[1]
		
		else:
			#两部分
			if(len(sub_parts[0]) == 1 and len(sub_parts[1]) == 1):
				sys.stderr.write("too short1\n")
				continue

			if(int(parts[1]) > 8000):
				print "".join(sub_parts) + "\t" + parts[1]
			
			

		
