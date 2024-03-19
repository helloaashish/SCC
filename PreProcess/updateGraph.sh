
#!/bin/bash

original_path="/work/08434/apandey/ls6/SCC-new/Datasets/OriginalReady"
changed_path="/work/08434/apandey/ls6/SCC-new/Datasets/ChangedEdges"
updated_path="/work/08434/apandey/ls6/SCC-new/Datasets/UpdatedGraphs"
filename=${1}

# declare -a percent_insertion=(0 25 50 75 100)
# declare -a size_names=("1M" "2M" "5M" "10M")

# declare -a no_changed_edges=(1000000 2000000 5000000 10000000)
declare -a size_names=("2M" "5M" "10M")
declare -a percent_insertion=(0 25 50 75 100)


# get length of an array
len_a1=${#size_names[@]} 
len_a2=${#percent_insertion[@]}

# use for loop to read all values and indexes
for (( i=0; i<${len_a1}; i++ ));
do
	for (( j=0; j<${len_a2}; j++));
	do
        cp ${original_path}/${filename}  ${updated_path}/${filename}/${filename}
        awk 'NR > 1 {print $1" "$2}' ${updated_path}/${filename}/${filename} > ${updated_path}/${filename}/${filename}_x
        awk '$3==1 {print $1" "$2}'  ${changed_path}/${filename}_ce/${filename}_${size_names[$i]}_${percent_insertion[$j]} >> ${updated_path}/${filename}/${filename}_x
        sort -n -k1 -k2 ${updated_path}/${filename}/${filename}_x | uniq > ${updated_path}/${filename}/${filename}_xy
        rm ${updated_path}/${filename}/${filename}_x
        rm ${updated_path}/${filename}/${filename}
        awk '$3==0 {print $1" "$2}'  ${changed_path}/${filename}_ce/${filename}_${size_names[$i]}_${percent_insertion[$j]} >  ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]}_del
        sort -n -k1 -k2  ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]}_del | uniq > ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]}_del_xy
        rm  ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]}_del
        diff ${updated_path}/${filename}/${filename}_xy  ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]}_del_xy | grep "<" | sed 's/^<//g' > ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]}
        awk '{print $1" "$2}' ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]} > ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]}.txt
        rm ${updated_path}/${filename}/${filename}_xy
        rm ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]}_del_xy
        rm ${updated_path}/${filename}/${filename}_${size_names[$i]}_${percent_insertion[$j]}
    done
done


 
# cp ${original_path}/${filename}  ${updated_path}/${filename}/${filename}
# awk 'NR > 1 {print $1" "$2}' ${updated_path}/${filename}/${filename} > ${updated_path}/${filename}/${filename}_x
# awk '$3==1 {print $1" "$2}'  ${changed_path}/${filename}_ce/${filename}_1M_0 >> ${updated_path}/${filename}/${filename}_x
# sort -n -k1 -k2 ${updated_path}/${filename}/${filename}_x | uniq > ${updated_path}/${filename}/${filename}_xy
# rm ${updated_path}/${filename}/${filename}_x
# rm ${updated_path}/${filename}/${filename}
# awk '$3==0 {print $1" "$2}'  ${changed_path}/${filename}_ce/${filename}_1M_0 >  ${updated_path}/${filename}/${filename}_1M_0_del
# sort -n -k1 -k2  ${updated_path}/${filename}/${filename}_1M_0_del | uniq > ${updated_path}/${filename}/${filename}_1M_0_del_xy
# rm  ${updated_path}/${filename}/${filename}_1M_0_del
# diff ${updated_path}/${filename}/${filename}_xy  ${updated_path}/${filename}/${filename}_1M_0_del_xy | grep "<" | sed 's/^<//g' > ${updated_path}/${filename}/${filename}_1M_0
# awk '{print $1" "$2}' ${updated_path}/${filename}/${filename}_1M_0 > ${updated_path}/${filename}/${filename}_1M_0.edges
# rm ${updated_path}/${filename}/${filename}_xy
# rm ${updated_path}/${filename}/${filename}_1M_0_del_xy
# rm ${updated_path}/${filename}/${filename}_1M_0



# cp /home/users/apandey/Datasets_SCC/Datasets/baidu_1 baidu_updated/baidu_x 
# awk 'NR > 1' your_file.txt > new_file.txt
# awk '$3==1 {print $1" "$2}'  /home/users/apandey/Datasets_SCC/ChangeEdges/baidu_ce/baidu_1M_0 >> baidu_updated/baidu_x
# sort -n -k1 -k2 baidu_updated/baidu_x | uniq > baidu_updated/baidu_xy
# rm baidu_updated/baidu_x
# awk '$3==0 {print $1" "$2}'  /home/users/apandey/Datasets_SCC/ChangeEdges/baidu_ce/baidu_1M_0 >  baidu_updated/baidu_1M_0_del
# sort -n -k1 -k2  baidu_updated/baidu_1M_0_del | uniq >  baidu_updated/baidu_1M_0_del_xy
# rm  baidu_updated/baidu_1M_0_del
# diff baidu_updated/baidu_xy  baidu_updated/baidu_1M_0_del_xy | grep "<" | sed 's/^<//g' > baidu_updated/baidu_1M_0
# awk '{print $1+1" "$2+1}' baidu_updated/baidu_1M_0 > baidu_updated/baidu_1M_0.edges
# rm baidu_updated/baidu_xy
# rm baidu_updated/baidu_1M_0_del_xy
# rm baidu_updated/baidu_1M_0