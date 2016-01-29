#! /usr/bin/python 
#encoding:gb18030
############################################
#
# Author: 
# Create time: 2015 9月 24 10时41分40秒
# E-Mail: @sogou-inc.com
# version 1.0
#
############################################


import sys,os
from optparse import OptionParser

if __name__ == "__main__":
	parser = OptionParser()
	parser.add_option("-1","--old",\
			dest = "old_file",
			help = "old")
	parser.add_option("-2","--new", \
			dest = "new_file",
			help = "new")
	parser.add_option("-3","--add", \
			dest = "add_file",
			help = "add")

	(opt,args) = parser.parse_args()

	if(not opt.old_file or not opt.new_file \
			or not opt.add_file):
		parser.print_help()
		sys.exit(-1)

	fp_old = open(opt.old_file,"r")
	fp_new = open(opt.new_file,"r")
	fp_add = open(opt.add_file,"w")


	dic_old = {}
	for line in fp_old:
		line = line.strip()
		parts = line.split("\t")
		dic_old[parts[0]] = 1

	for line in fp_new:
		line = line.strip()
		parts = line.split("\t")
		if not dic_old.has_key(parts[0]):
			fp_add.write("%s\n" % line)
