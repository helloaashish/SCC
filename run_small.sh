#!/bin/sh
module load g++
make all


if [ $1 = "baidu" ]; then
    ./a.out /work/07060/sb0837/ls6/Examples/baiduKeyEdged.txt /work/07060/sb0837/ls6/Examples/baidu_ME.txt /work/07060/sb0837/ls6/Examples/scc_baidu.txt /work/07060/sb0837/ls6/Examples/baidu_1_Map.txt /work/07060/sb0837/ls6/Examples/baidu_1M_100 $2 $3
fi

if [ $1 = "test" ]; then
   ./a.out /work/07060/sb0837/ls6/Examples/t.txt /work/07060/sb0837/ls6/Examples/test_ME.txt /work/07060/sb0837/ls6/Examples/test_CC1.txt /work/07060/sb0837/ls6/Examples/test_map.txt /work/07060/sb0837/ls6/Examples/test_change1.txt $2 $3
fi

if [ $1 = "dag" ]; then
   ./a.out /work/07060/sb0837/ls6/Examples/dag.txt /work/07060/sb0837/ls6/Examples/dagME.txt /work/07060/sb0837/ls6/Examples/dagCC.txt /work/07060/sb0837/ls6/Examples/dagMAP.txt /work/07060/sb0837/ls6/Examples/dagCE.txt $2 $3 > out.txt
fi


# baidu
