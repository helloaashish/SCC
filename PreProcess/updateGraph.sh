cp /home/users/apandey/Datasets_SCC/Datasets/baidu_1 baidu_updated/baidu_x 
awk '$3==1 {print $1" "$2}'  /home/users/apandey/Datasets_SCC/ChangeEdges/baidu_ce/baidu_1M_0 >> baidu_updated/baidu_x
sort -n -k1 -k2 baidu_updated/baidu_x | uniq > baidu_updated/baidu_xy
rm baidu_updated/baidu_x
awk '$3==0 {print $1" "$2}'  /home/users/apandey/Datasets_SCC/ChangeEdges/baidu_ce/baidu_1M_0 >  baidu_updated/baidu_1M_0_del
sort -n -k1 -k2  baidu_updated/baidu_1M_0_del | uniq >  baidu_updated/baidu_1M_0_del_xy
rm  baidu_updated/baidu_1M_0_del
diff baidu_updated/baidu_xy  baidu_updated/baidu_1M_0_del_xy | grep "<" | sed 's/^<//g' > baidu_updated/baidu_1M_0
awk '{print $1+1" "$2+1}' baidu_updated/baidu_1M_0 > baidu_updated/baidu_1M_0.edges
rm baidu_updated/baidu_xy
rm baidu_updated/baidu_1M_0_del_xy
rm baidu_updated/baidu_1M_0

