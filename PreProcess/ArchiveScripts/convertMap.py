#Read SCCx
#create Graph form file

map_path="/home/users/apandey/SCC-new/Examples/baidu_1_Map.txt"
scc_path="/home/users/apandey/SCC-new/Examples/scc_baidu.txt"

# filename="wiki-en"

map_dict = dict()
with open(map_path) as file:
    #meta_info = file.readline()  # Read the first line (meta information)
    mymap = file.readlines()     # Read the remaining lines (edges)

for each in mymap:
    vals = each.split(" ")
    k,v = int(vals[0]), int(vals[1])
    map_dict[k] = v


with open(scc_path) as sccfile:
    SCCx = sccfile.readlines()
    SCCx =[int(a) for a in SCCx]
#read MAP

print("length of map is" ,len(map_dict))
print("length of SCCx is" ,len(SCCx))

print(2141300 in SCCx)
print(map_dict[2141300])
new_sccx = [map_dict[a] for a in SCCx]

with open ("/home/users/apandey/SCC-new/Examples/xbaidu","w")as fp:
   for each in new_sccx:
       fp.write(str(each)+"\n")