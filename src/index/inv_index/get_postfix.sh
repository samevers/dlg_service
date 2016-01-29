#! /bin/bash
############################################
#
# Author: 
# Create time: 2015 9ÔÂ 22 11Ê±22·Ö28Ãë
# E-Mail: @sogou-inc.com
# version 1.0
#
############################################

#cat base_data.txt | ./get_postfix 1>postfix.out
sort postfix.out | uniq -c | awk '{print $2"\t"$1}' > postfix.out.uniq
sort -t"	" -k2 -rn postfix.out.uniq > postfix.out.uniq.out
