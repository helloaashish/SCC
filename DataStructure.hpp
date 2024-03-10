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
    int* b_row_ptr; // backward row pointer for backward edges
    int* b_col_idx; // backward row pointer for backward edges
    int* edge_wt; //edge weight will be 1 for active edges; 0 for deleted edges ; -1 for key edges
                  // in case of metagraph, the edge_wt shows the number of links between each metagraph.
    int node_count;
    int edge_count;

    int_int get_children(int n){
        int start,end;
        start = f_row_ptr[n];
        end = f_row_ptr[n+1];
        return {start,end};
    }
    int_int get_parents(int n){
        int start,end;
        start = b_row_ptr[n];
        end = b_row_ptr[n+1];
        return {start,end};
    }

    int num_parents(int n){return b_row_ptr[n+1] - b_row_ptr[n];}

    int num_childrens(int n){return f_row_ptr[n+1] - f_row_ptr[n];}


    void print_graph(){
        // int node_count = num_nodes();
        printf("Size of Graph: %d Nodes and %d Edges \n", node_count, edge_count );
        for (int node = 0; node<node_count; node++)
        {
            int_int range_childern = get_children(node);
            int_int range_parents = get_parents(node);
            printf("***** NODE: %d *****\n", node);
            for (int i = range_childern.first; i<range_childern.second; i++){
                printf(" ---> %d (%d) \n",f_col_idx[i],edge_wt[i]);
            }
            for (int i = range_parents.first; i<range_parents.second; i++){
                printf(" <--- %d\n",b_col_idx[i]);
            }

        }
    }
};
// ******************************************************************************


// ******************************************************************************
struct X_Graph{
    vector<int> f_row_ptr; //forward row pointer for forward relations
    vector<int> f_col_idx; //colume vector for edges forward
    vector<int> b_row_ptr; // backward row pointer for backward edges
    vector<int> b_col_idx; // backward row pointer for backward edges
    vector<int> edge_wt; //edge weight will be 1 for active edges; 
                        // in case of metagraph, the edge_wt shows the number of links between each metagraph.

    int_int get_children(int n){
        int start,end;
        start = f_row_ptr.at(n);
        end = f_row_ptr.at(n+1);
        return {start,end};
    }
    int_int get_parents(int n){
        int start,end;
        start = b_row_ptr.at(n);
        end = b_row_ptr.at(n+1);
        return {start,end};
    }

    int num_parents(int n){return b_row_ptr.at(n+1) - b_row_ptr.at(n);}

    int num_childrens(int n){return f_row_ptr.at(n+1) - f_row_ptr.at(n);}

    int num_nodes(){return f_row_ptr.size()-1;}
    int num_edges(){return f_col_idx.size();}

    void print_graph(){
        for (int node = 0; node<num_nodes(); node++)
        {
            int_int range_childern = get_children(node);
            int_int range_parents = get_parents(node);
            printf("***** NODE: %d *****\n", node);
            for (int i = range_childern.first; i<range_childern.second; i++){
                printf(" ---> %d (%d) \n",f_col_idx.at(i),edge_wt.at(i));
            }
            for (int i = range_parents.first; i<range_parents.second; i++){
                printf(" <--- %d\n",b_col_idx.at(i));
            }

        }
    }
};
// ******************************************************************************



struct MetaNode{
    bool inserted_from;
    bool inserted_to;
    int currentID;
    bool is_hub;
    int h_idx;
    int* Hub_info;
};
#endif
