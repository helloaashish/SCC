#!/bin/sh
make all

if [ $1 = "baidu" ]; then
    ./a.out /home/users/apandey/SCC-new/Examples/baiduKeyEdged.txt /home/users/apandey/SCC-new/Examples/baidu_ME.txt /home/users/apandey/SCC-new/Examples/scc_baidu.txt /home/users/apandey/SCC-new/Examples/baidu_1_Map.txt /home/users/apandey/SCC-new/Examples/baidu_1M_25 $2 $3
fi

if [ $1 = "test" ]; then
   ./a.out /home/users/apandey/SCC-new/Examples/t.txt /home/users/apandey/SCC-new/Examples/test_ME.txt /home/users/apandey/SCC-new/Examples/test_CC1.txt /home/users/apandey/SCC-new/Examples/test_map.txt /home/users/apandey/SCC-new/Examples/test_change1.txt $2 $3
fi

if [ $1 = "dag" ]; then
   ./a.out /home/users/apandey/SCC-new/Examples/dag.txt /home/users/apandey/SCC-new/Examples/dagME.txt /home/users/apandey/SCC-new/Examples/dagCC.txt /home/users/apandey/SCC-new/Examples/dagMAP.txt /home/users/apandey/SCC-new/Examples/dagCE.txt $2 $3
fi


# baidu
