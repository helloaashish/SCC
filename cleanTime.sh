#!/bin/sh

start_time=`date +%s`
awk 'NR==FNR { lines[NR-1] = $0; next } { if (FNR <= length(lines)) { $1 = lines[$1]; $2 = lines[$2] } print }' /home/users/apandey/SCC-new/Examples/xbaidu /home/users/apandey/SCC-new/Examples/baidu_1M_25T > tempCE.txt
end_time=`date +%s`
echo execution time was `expr $end_time - $start_time` s.

