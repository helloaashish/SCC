#ifndef SupportingFunctions_HPP
#define SupportingFunctions_HPP


#include<iostream>
#include<vector>
#include <unordered_map>
#include <map>
#include <stack>
#include <unordered_set>
#include "DataStructure.hpp"
#include "PrintFunctions.hpp"
#include <cstdio>

typedef pair<int,int> int_int;
using namespace std;

// #define DEBUG



// ******************************************************************************
void check_file_open(char *graph_file, char *meta_graph, char *scc_ID, char *map, char* changes, int total_args)
{

    if (total_args < 8)
    {
        cout << "usage ./a.out graph_file metagrpah_file sccx_file map_file change_file hubsize num_threads  \n";
    }
    // Check to see if file opening succeeded
    ifstream the_file1(graph_file);
    if (!the_file1.is_open())
    {
        cout << "INPUT ERROR:: Could not open  graph file\n";
    }

    ifstream the_file2(meta_graph);
    if (!the_file2.is_open())
    {
        cout << "INPUT ERROR:: Could not open metagraph file\n";
    }

    ifstream the_file3(scc_ID);
    if (!the_file3.is_open())
    {
        cout << "INPUT ERROR:: Could not open SCC file\n";
    }

    ifstream the_file4(map);
    if (!the_file4.is_open())
    {
        cout << "INPUT ERROR:: Could not open MAP file\n";
    }

    ifstream the_file5(changes);
    if (!the_file5.is_open())
    {
        cout << "INPUT ERROR:: Could not open Change file\n";
    }


}
// *********************** END OF FUNCTION *******************************************************

/** Store Information into MetaNode **/
void meta_node_info(MetaNode *MN_list, int N, Graph *g_meta, Graph *gI_meta)
{
for(int i=0;i<N;i++)
{
//On Original Edges
MN_list[i].in=g_meta->in_deg[i];
MN_list[i].out=g_meta->out_deg[i];

if(MN_list[i].out>0)
{
int fstart=g_meta->f_row_ptr[i];
MN_list[i].fstart=fstart;
MN_list[i].fend=g_meta->f_row_ptr[i+1];
MN_list[i].fn1=g_meta->f_col_idx[fstart];//first fwd neighbor
}

if(MN_list[i].in>0)
{
int bstart=g_meta->b_row_ptr[i];
MN_list[i].bstart=bstart;
MN_list[i].bend=g_meta->b_row_ptr[i+1];
MN_list[i].bn1I=g_meta->b_col_idx[bstart];//first bwd neighbor
}


//On Inserted Edges
MN_list[i].inI=gI_meta->in_deg[i];
MN_list[i].outI=gI_meta->out_deg[i];

if(MN_list[i].outI>0)
{
int fstartI=gI_meta->f_row_ptr[i];
MN_list[i].fstartI=fstartI;
MN_list[i].fendI=gI_meta->f_row_ptr[i+1];
MN_list[i].fn1I=gI_meta->f_col_idx[fstartI];//first fwd neighbor
}


if(MN_list[i].inI>0)
{
int bstartI=gI_meta->b_row_ptr[i];
MN_list[i].bstartI=bstartI;
MN_list[i].bendI=gI_meta->b_row_ptr[i+1];
MN_list[i].bn1I=gI_meta->b_col_idx[bstartI];//first bwd neighbor
}

}//end of for	

}
// *********************** END OF FUNCTION *******************************************************

// ******************************************************************************
/** Create Graph from  Data **/
void create_graph(int* src, int* dest, int* wt_list, int n, int m, Graph* graph){
    graph->f_col_idx = new int[m]();
    graph->b_col_idx = new int[m]();
    graph->edge_wt = new int[m]();
    graph->f_node_idx = new int[m]();
    graph->b_node_idx = new int[m]();
    graph->f_row_ptr = new int[n+1]();
    graph->b_row_ptr = new int[n+1]();
    graph->node_count = n;
    graph->edge_count = m;

    graph->in_deg = new int[n]{0};
    graph->out_deg = new int[n]{0};
    

   for (int i = 0; i<m; i++){
   if(src[i]==-1||dest[i]==-1){continue;} 
    //printf("FXX===%d %d \n", src[i], dest[i]);
	graph->in_deg[dest[i]]++;
        graph->out_deg[src[i]]++;
        graph->f_row_ptr[src[i]+1]++;
        graph->b_row_ptr[dest[i]+1]++;
    }

    //finding the cumulative sum for starting vertex
    for (int i = 1; i<= n; i++){
        graph->f_row_ptr[i] += graph->f_row_ptr[i-1];
        graph->b_row_ptr[i] += graph->b_row_ptr[i-1];
    }
    
    int* temp_next_outgoing_idx = new int[n]{0};
    int* temp_next_incoming_idx = new int[n]{0};

    for (int i = 0; i<m; i++){
   if(src[i]==-1||dest[i]==-1){continue;} 
        int oIdx = graph->f_row_ptr[src[i]] + temp_next_outgoing_idx[src[i]];
        int iIdx = graph->b_row_ptr[dest[i]] + temp_next_incoming_idx[dest[i]];

        graph->f_col_idx[oIdx] = dest[i];
        graph->f_node_idx[oIdx] = src[i];
        graph->edge_wt[oIdx] = wt_list[i];
        graph->b_col_idx[iIdx] = src[i];
        graph->b_node_idx[iIdx] = dest[i];

        temp_next_outgoing_idx[src[i]]++;
        temp_next_incoming_idx[dest[i]]++;
    }
    
}
// ******  END OF FUNCTION ************************************************************************



// ******************************************************************************
void process_inserts(vector<int_int> * inserts_meta, int *active_nodes, int *src, int *dest){
    
}
// ******  END OF FUNCTION ************************************************************************
#endif
