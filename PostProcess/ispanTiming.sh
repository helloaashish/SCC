#!/bin/bash
updated_path="/work/08434/apandey/ls6/SCC-new/Datasets/UpdatedGraphs"
results_path="/work/08434/apandey/ls6/SCC-new/Datasets/ResultIspan"
ispan_path="/work/08434/apandey/ls6/SCC-new/SCC/Baseline/iSpan"

# declare -a size_names=("1M" "2M" "5M" "10M")
# declare -a percent_insertion=(0 25 50 75 100)
# declare -a graphs=("baidu" "flickr" "livejournal" "pokec" "wiki-com")
# declare -a threads=(1 2 4 8 16 32 64 72)

declare -a size_names=("1M" "2M" "5M" "10M")
declare -a percent_insertion=(0 25 50 75 100)
declare -a graphs=("wiki-com" "wiki-en")
declare -a threads=(1 2 4 8 16 32 64 72)


# get length of an array
len_a1=${#graphs[@]}
len_a2=${#size_names[@]}
len_a3=${#percent_insertion[@]}
len_threads=${#threads[@]}


# loop inside the files
for (( i=0; i<${len_a1}; i++ ));
do
    for (( j=0; j<${len_a2}; j++ ));
    do
        for (( k=0; k<${len_a3}; k++ ));
        do
            rm ${ispan_path}/graph_converter/*.bin
            # ${ispan_path}/graph_converter/txt_to_bin_fw /work/08434/apandey/ls6/SCC-new/Datasets/OriginalReady/baidu > /dev/null
            # ${ispan_path}/graph_converter/txt_to_bin_bw /work/08434/apandey/ls6/SCC-new/Datasets/OriginalReady/baidu > /dev/null
            ${ispan_path}/graph_converter/txt_to_bin_fw ${updated_path}/${graphs[$i]}/${graphs[$i]}_${size_names[$j]}_${percent_insertion[$k]}.txt >/dev/null 2>&1
            ${ispan_path}/graph_converter/txt_to_bin_bw ${updated_path}/${graphs[$i]}/${graphs[$i]}_${size_names[$j]}_${percent_insertion[$k]}.txt >/dev/null 2>&1
            mv *.bin ${ispan_path}/graph_converter/
            for (( l=0; l<${len_threads}; l++ ));
            do
                ${ispan_path}/src/ispan ${ispan_path}/graph_converter/fw_begin.bin ${ispan_path}/graph_converter/fw_adjacent.bin ${ispan_path}/graph_converter/bw_begin.bin ${ispan_path}/graph_converter/bw_adjacent.bin ${threads[$l]} 30 200 10 0.1 10 > ${results_path}/${graphs[$i]}/${size_names[$j]}_${percent_insertion[$k]}_${threads[$l]}.txt
            done
        done
    done
done

#