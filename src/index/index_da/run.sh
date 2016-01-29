#! /bin/bash
############################################
#
# Author: lichao
# Create time: 2014 12ÔÂ 11 14Ê±40·Ö35Ãë
# E-Mail: lichaotx020@sogou-inc.com
# version 1.0
#
############################################
g++ *.cpp *.h -I/search/yonghuahu/vr/dependence_/ssplatform-encoding/_include -L/search/yonghuahu/vr/dependence_/ssplatform-encoding/_lib -lssplatform -lencoding -g -o make_index


#./make_index /search/yonghuahu/vr/data/base/analysis/queryclass/whitelist.common /search/yonghuahu/vr/data/base/analysis/queryclass/whitelist.common.index
#./make_index /search/yonghuahu/vr/data/base/analysis/queryclass/blacklist.common /search/yonghuahu/vr/data/base/analysis/queryclass/blacklist.common.index
#./make_index /search/yonghuahu/vr/data/base/analysis/queryclass/blacklist.vrqo /search/yonghuahu/vr/data/base/analysis/queryclass/blacklist.vrqo.index
#./make_index /search/yonghuahu/vr/data/base/analysis/queryclass/whitelist.vrqo /search/yonghuahu/vr/data/base/analysis/queryclass/whitelist.vrqo.index
