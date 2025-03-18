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

    int* out_deg; //out degree of the node
    int* in_deg; //in degree of the node 

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
            printf("Degrees: %d(IN) %d(OUT)\n",in_deg[node],out_deg[node]);
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

// ******************************************************************************



struct MetaNode{
    bool trimmed=true;
    int currentID;
    bool is_hub;
    int h_idx;
    // vector <uint8_t> *Hub_info = new vector<uint8_t>(5,0); 
    vector <uint8_t> *Hub_info = new vector<uint8_t>(100,0); 
    // vector<int> H_info = std::vector<int>(5, 0);
    // vector<int> * update_addr;
    // vector<int>* Hub_info=nullptr;
};


// struct MetaNode {
//     bool trimmed = true;
//     int currentID = 0;
//     bool is_hub = false;
//     int h_idx = -1;
//     std::vector<uint8_t> *Hub_info;

//     // Constructor to initialize Hub_info with size k
//     MetaNode(int k) : Hub_info(new std::vector<uint8_t>(k, 0)) {}
//         // Default constructor with Hub_info as nullptr
//     MetaNode() : Hub_info(nullptr) {}
//     // Destructor to clean up allocated memory
//     ~MetaNode() {
//         delete Hub_info;
//     }
// };



#endif
