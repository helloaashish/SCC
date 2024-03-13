#!/bin/bash
## declare an array variable
declare -a no_changed_edges=(1000000 2000000 5000000 10000000)
# declare -a no_changed_edges=(10 20 50 100)
declare -a size_names=("1M" "2M" "5M" "10M")
declare -a percent_insertion=(0 25 50 75 100)


# get length of an array
len_a1=${#no_changed_edges[@]}
len_a2=${#percent_insertion[@]}

#get number of nodes
n=$(awk 'NR==1 {print $1}' /work/08434/apandey/ls6/SCC-new/Datasets/OriginalReady/${1})

# use for loop to read all values and indexes
for (( i=0; i<${len_a1}; i++ ));
do
	for (( j=0; j<${len_a2}; j++));
	do
		./cE-undirected /work/08434/apandey/ls6/SCC-new/Datasets/OriginalReady/${1} ${n} ${no_changed_edges[$i]} ${percent_insertion[$j]} > /work/08434/apandey/ls6/SCC-new/Datasets/ChangedEdges/${1}_ce/${1}_${size_names[$i]}_${percent_insertion[$j]}
	done
  #echo "index: $i, value: ${array[$i]}"
done