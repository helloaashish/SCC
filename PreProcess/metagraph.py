# input Graph, and SCCx
# output Metagraph => 0 vertexed and map of original ID

'''
Input: Directed Graph data: n m
       SCC data
Processing:
        1. Read the graph data and SCC data
        2. Change edge into metaedge
        3. Create meta-graph digraph and remove the self loops
        4. 
Output:
    1. Original Data: Do not change
    2. SCCx Data: Do not change
    3. Create New file: MetaEdge List
    4. Create New file: Mapping: old to new

'''

import networkx as nx
import sys

filename = sys.argv[1]

graph_path = "/work/08434/apandey/ls6/SCC-new/Datasets/OriginalReady/"
scc_path = "/work/08434/apandey/ls6/SCC-new/Datasets/SCCx/"

map_path = "/work/08434/apandey/ls6/SCC-new/Datasets/Map/"
metagraph_path = "/work/08434/apandey/ls6/SCC-new/Datasets/Metagraphs/"


#create Graph form file
with open(graph_path+filename) as file:
    #meta_info = file.readline()  # Read the first line (meta information)
    edges = file.readlines()     # Read the remaining lines (edges)

with open(scc_path+filename) as sccfile:
    SCCx = sccfile.readlines()
    SCCx =[int(a) for a in SCCx]


metaEdgelist = []
for edge in edges[1:]:
    edge = edge.split(" ")
    a=int(edge[0])
    b=int(edge[1])
    metaEdgelist.append([SCCx[a],SCCx[b]])

#create metanetwork form metaedgelist to remove duplicate values and self loops
meta_network = nx.DiGraph()
meta_network.add_edges_from(metaEdgelist)

meta_network.remove_edges_from(nx.selfloop_edges(meta_network))


#Write the meta-edgelist to the file by giving new label id and also mapping

nodelist = list(meta_network.nodes())
nodelist.sort()
mapping = {old_label:new_label for new_label, old_label in enumerate(nodelist)}
relabeled_meta_network = nx.relabel_nodes(meta_network, mapping)

with open(metagraph_path+filename, 'w') as graphfile:
    graphfile.write("{} {}\n".format(relabeled_meta_network.number_of_nodes(), relabeled_meta_network.number_of_edges())) 
    for edge in relabeled_meta_network.edges():
        graphfile.write("{} {} {}\n".format(edge[0],edge[1],1))
    


with open(map_path+filename,'w') as file:
    for key, value in mapping.items():
        file.write("{} {}\n".format(key, value))