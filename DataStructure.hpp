#ifndef DatasStructure_HPP
#define DatasStructure_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include <fstream>

using namespace std;
typedef pair<int,int> int_int;

// ******************************************************************************
struct Graph{
    int* f_row_ptr; //forward row pointer for forward relations
    int* f_col_idx; //colume vector for edges forward
    int* f_node_idx; //node vector will point to the node. <node col> together give an edge
    
    int* b_row_ptr; // backward row pointer for backward edges
    int* b_col_idx; // backward row pointer for backward edges
    int* b_node_idx; //node vector will point to the node. <node col> together give an edge
    
    int* edge_wt; //edge weight will be 1 for active edges; 0 for deleted edges ; -1 for key edges
                  // in case of metagraph, the edge_wt shows the number of links between each metagraph.

    int *out_deg; //out degree of the node
    int  *in_deg; //in degree of the node 
    
    bool *is_hub;
    int node_count;
    int edge_count;


    //int num_parents(int n){return b_row_ptr[n+1] - b_row_ptr[n];}

    //int num_childrens(int n){return f_row_ptr[n+1] - f_row_ptr[n];}


    /*void print_graph(){
        // int node_count = num_nodes();
        printf("Size of Graph: %d Nodes and %d Edges \n", node_count, edge_count );
        for (int node = 0; node<node_count; node++)
        {
            int_int range_childern = get_children(node);
            int_int range_parents = get_parents(node);
            printf("***** NODE: %d *****\n", node);
            printf("Degrees: %d(IN) %d(OUT)\n",in_deg[node],out_deg[node]);
            for (int i = range_childern.first; i<range_childern.second; i++){
                printf(" ---> %d %d (%d) \n",f_node_idx[i],f_col_idx[i],edge_wt[i]);
            }
            for (int i = range_parents.first; i<range_parents.second; i++){
                printf(" <--- %d %d\n",f_node_idx[i],b_col_idx[i]);
            }

        }
    }//end of print*/
};


//Sorting vector using first as primary key and second as secondary key
bool sort_first_second(const pair<int, int>& a,
                const pair<int, int>& b)
{
    if (a.first != b.first)
        return (a.first < b.first);
    else
       return (a.second < b.second);
}
// ******************************************************************************





struct MetaNode{
    bool trimmed=false;
    int currentID=-1;
    bool is_hub=false;
    int h_idx=-1;
    int Hub_info[100]={0};//which Hub it is connected to
    int up=0; //whether to propagate value up or down
    int down=0;
    bool affected=false; // whether the meta node is potentially in a SCC.
};

#endif
