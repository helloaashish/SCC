#python script to rename edgelist and write in file
# usage python3 rename.py original_graph_name
#output
'''
n m
1 2
3 4
'''
import networkx as nx
import sys

input_filepath = "/work/08434/apandey/ls6/SCC-new/Datasets/OriginalDataset/"
output_filepath = "/work/08434/apandey/ls6/SCC-new/Datasets/OriginalReady/"
# Renames the graph file to make it 0-vertexed and continuing name of nodes
filename = sys.argv[1]
data = open(input_filepath+filename,"r")

graph = nx.parse_edgelist(data,delimiter = ' ', create_using=nx.DiGraph(), nodetype = int)
reindexed_graph = nx.relabel.convert_node_labels_to_integers(graph, first_label=0, ordering='default')
n = reindexed_graph.number_of_nodes()
m = reindexed_graph.number_of_edges()
# print("Original Graph: ",graph.number_of_nodes(), " Reindexed Graph: ",n)
# print("Original Graph: ",graph.number_of_edges(), " Reindexed Graph: ",m)

with open(output_filepath+filename, 'w') as f:
    f.write(str(n)+" "+str(m)+"\n")
    for a,b in reindexed_graph.edges:
        f.write(str(a)+" "+str(b)+" "+"1\n")
#nx.write_edgelist(reindexed_graph, filename+"_renamed",data = False)


#nodes:  2141300  edges  17643697