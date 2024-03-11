#!/bin/sh
module load g++
make all

if [ $1 = "baidu" ]; then
    ./a.out /work/08434/apandey/ls6/SCC-new/SCC/Examples/baiduKeyEdged.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/baidu_ME.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/scc_baidu.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/baidu_1_Map.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/baidu_1M_25 $2 $3
fi

if [ $1 = "test" ]; then
   ./a.out /work/08434/apandey/ls6/SCC-new/SCC/Examples/t.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/test_ME.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/test_CC1.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/test_map.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/test_change1.txt $2 $3
fi

if [ $1 = "dag" ]; then
   ./a.out /work/08434/apandey/ls6/SCC-new/SCC/Examples/dag.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/dagME.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/dagCC.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/dagMAP.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/dagCE.txt $2 $3
fi


# baidu
