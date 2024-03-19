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
   ./a.out /work/08434/apandey/ls6/SCC-new/SCC/Examples/dag.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/dagME.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/dagCC.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/dagMAP.txt /work/08434/apandey/ls6/SCC-new/SCC/Examples/dagCE.txt $2 $3 > out.txt
fi

if [ $1 = "baidu1" ]; then
    ./a.out /work/08434/apandey/ls6/SCC-new/Datasets/KeyEdged/baidu /work/08434/apandey/ls6/SCC-new/Datasets/Metagraphs/baidu /work/08434/apandey/ls6/SCC-new/Datasets/SCCx/baidu /work/08434/apandey/ls6/SCC-new/Datasets/Map/baidu /work/08434/apandey/ls6/SCC-new/Datasets/ChangedEdges/baidu_ce/baidu_1M_25 $2 $3
fi

if [ $1 = "flickr" ]; then
    ./a.out /work/08434/apandey/ls6/SCC-new/Datasets/KeyEdged/flickr /work/08434/apandey/ls6/SCC-new/Datasets/Metagraphs/flickr /work/08434/apandey/ls6/SCC-new/Datasets/SCCx/flickr /work/08434/apandey/ls6/SCC-new/Datasets/Map/flickr /work/08434/apandey/ls6/SCC-new/Datasets/ChangedEdges/flickr_ce/flickr_1M_25 $2 $3
fi

if [ $1 = "livejournal" ]; then
    ./a.out /work/08434/apandey/ls6/SCC-new/Datasets/KeyEdged/livejournal /work/08434/apandey/ls6/SCC-new/Datasets/Metagraphs/livejournal /work/08434/apandey/ls6/SCC-new/Datasets/SCCx/livejournal /work/08434/apandey/ls6/SCC-new/Datasets/Map/livejournal /work/08434/apandey/ls6/SCC-new/Datasets/ChangedEdges/livejournal_ce/livejournal_1M_25 $2 $3
fi

if [ $1 = "pokec" ]; then
    ./a.out /work/08434/apandey/ls6/SCC-new/Datasets/KeyEdged/pokec /work/08434/apandey/ls6/SCC-new/Datasets/Metagraphs/pokec /work/08434/apandey/ls6/SCC-new/Datasets/SCCx/pokec /work/08434/apandey/ls6/SCC-new/Datasets/Map/pokec /work/08434/apandey/ls6/SCC-new/Datasets/ChangedEdges/pokec_ce/pokec_1M_25 $2 $3
fi

if [ $1 = "wiki-com" ]; then
    ./a.out /work/08434/apandey/ls6/SCC-new/Datasets/KeyEdged/wiki-com /work/08434/apandey/ls6/SCC-new/Datasets/Metagraphs/wiki-com /work/08434/apandey/ls6/SCC-new/Datasets/SCCx/wiki-com /work/08434/apandey/ls6/SCC-new/Datasets/Map/wiki-com /work/08434/apandey/ls6/SCC-new/Datasets/ChangedEdges/wiki-com_ce/wiki-com_1M_25 $2 $3
fi

if [ $1 = "wiki-en" ]; then
    ./a.out /work/08434/apandey/ls6/SCC-new/Datasets/KeyEdged/wiki-en /work/08434/apandey/ls6/SCC-new/Datasets/Metagraphs/wiki-en /work/08434/apandey/ls6/SCC-new/Datasets/SCCx/wiki-en /work/08434/apandey/ls6/SCC-new/Datasets/Map/wiki-en /work/08434/apandey/ls6/SCC-new/Datasets/ChangedEdges/wiki-en_ce/wiki-en_1M_25 $2 $3
fi




# baidu
