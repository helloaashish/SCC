#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <unordered_map>
#include "DataStructure.hpp"
#include "ReadData.hpp"
#include "SupportingFunctions.hpp"
#include "PrintFunctions.hpp"
#include "hinfo.h"
#include <sys/time.h>
#include <cstdint>

#include<chrono>


using namespace std;

typedef pair<int,int> int_int;

#define VERBOSE 1 
#define DEBUG 0

const int IN = 1;
const int AT = 3;
const int OUT = 2;
const int NOT = 0;



int main(int argc, char *argv[]){

    // Incoming Files only MetaGraph, Changed Edges, Hubs_Count, Thread Count 
    check_file_open(argv[1],argv[2],argc);
    int n,N,m,M; //number of nodes and metanodes
    int hubsize = atoi(argv[3]);
    int p = atoi(argv[4]); //number of threads
    double start_timer;
    int *i_src, *i_dest, *i_wt, *m_src, *m_dest, *m_wt;
    int len_inserts;
    vector<int> Hubs;
    Graph g_inserts,g_meta;
    vector<int_int> inserts,deletes;
    inserts.clear();
    
// ******************* READING FILES ****************************
    start_timer = omp_get_wtime();
    // omp_set_nested(1); 
  
    read_graph(argv[1], N,M, m_src, m_dest, m_wt); //reading the metagraph data
    read_inserts(argv[2],i_src,i_dest,i_wt,len_inserts);
    // read_changes(&inserts, &deletes,argv[2]); // read_changes has deletes argument so keeping it for now
    printf("Number of Nodes %d \n", N);
    #if VERBOSE
        color("purple");
        printf("\n Time for Reading: %f \n", (float)(omp_get_wtime()-start_timer));
        color("reset");
    #endif
// ******************* READING COMPLETED ****************************

// // ******************* CREATING GRAPHS ****************************
    printf("Requested number of hubs and threads %d and %d\n", hubsize,p);
    // double st =omp_get_wtime();
    // // create_graph(e_src,e_dest,e_wt,n,m,&g); //GRAPH
    start_timer = omp_get_wtime();
    create_graph(m_src,m_dest,m_wt,N,M,&g_meta); //METAGRAPH
    create_graph(i_src,i_dest,i_wt,N,len_inserts,&g_inserts);
    #if VERBOSE
        color("purple");
        printf("\n Time for Creating graph: %f \n", (float)(omp_get_wtime()-start_timer));
        color("reset");
        printf("Graph: Nodes %d Edges %d \n",N,M);
        printf("Inserts: Nodes %d Edges %d \n",N,len_inserts);
    #endif
// // ******************* CREATING GRPAHS COMPLETED **************************** 

    // #if DEBUG
    //     printf("Printing Insert Graph! ");
    //     printArray(g_inserts.f_row_ptr, N+1);
    //     printArray(g_inserts.f_col_idx,len_inserts);
    //#endif
    // int* MN_list = new int[N];
    //      printf("Printing Meta Graph! ");
    //     printArray(g_meta.f_row_ptr, N+1);
    //     printArray(g_meta.f_col_idx,M);

    //     printf("Printing Inserted Edges! ");
    //     printArray(i_src, len_inserts);
    //     printArray(i_dest,len_inserts);
    // #endif
//Creating MetaNode Array for Hub Infornmation

// vector<MetaNode>* MN_lists;
// MN_lists = new vector<MetaNode> {N,MetaNode(N)};
// bool* is_hub = new bool[N]{false};
MetaNode* MN_list = new MetaNode[N];

// // // ******************* FINDING HUBS ****************************
start_timer = omp_get_wtime();
get_hubs(&g_meta,&g_inserts,MN_list,N,&Hubs,hubsize,p);
// find_hubs(&g, &g_meta,MN_list, &SCCx, propagate_changed_up, propagate_changed_down,Hubs, n,N, hubsize,p);
initialize_labels(&g_meta,MN_list,N,&Hubs, hubsize, p);
// collect_labels(&g_meta, &g_inserts,MN_list, N,&Hubs,hubsize,p);
float c = (float)(omp_get_wtime()-start_timer);
color("purple");
printf("\n Time for Finding Hubs: %f \n", c);
color("reset");

printf("Hub info for node %d is %d \n",16,get_value(*MN_list[16].Hub_info,0));
printf("Hub info for node %d is %d\n",17,get_value(*MN_list[17].Hub_info,0));
printf("Hub info for node %d is %d\n",18,get_value(*MN_list[7].Hub_info,0));

// printf("Hub info for node %d is %d \n",16,MN_list[16].H_info[0]);
// printf("Hub info for node %d is %d\n",17,MN_list[17].H_info[0]);
// printf("Hub info for node %d is %d\n",18,MN_list[7].H_info[0]);
// delete MN_list;
// delete MN_lists;
// // // ******************* FINDING HUBS COMPLETED ****************************

// ******************* Initialize Labels and propagate ****************************


/*


// ******************* ADDING INSERTS & DELETES ****************************

int insert_size = int(inserts.size());
int delete_size = int(deletes.size());
insert_status = new bool[insert_size]{false}; //true if completed false if needs to be processed
delete_status = new bool[delete_size]{false};
printf("Count of  completed inserts at first: %d\n",count_true(insert_status,insert_size,p));
printf("Count of  completed deletes at first: %d\n",count_true(delete_status,delete_size,p));
st = omp_get_wtime();
// convert_changes(&g, &g_meta, &inserts, &deletes, &SCCx, &sccMAP, MN_list, trimmed, insert_status, delete_status, p);
// clean_inserts(&inserts, insert_status, trimmed, p);
// clean_deletes(&deletes, delete_size, trimmed, p);
color("purple");
printf("\n Time for Insert/Delets Conversion: %f \n", (float)(omp_get_wtime()-st));
color("reset");
// ******************* ADDING INSERTS & DELETES COMPLETED ****************************

// print_status(insert_status, &inserts);

// // ******************* Writing Levels ****************************

printf("Count of  completed inserts after convert changes: %d\n",count_true(insert_status,insert_size,p));
printf("Count of completed inserts %d\n",int(inserts.size()));
printf("Count of  completed deletes after convert changes: %d\n",count_true(delete_status,delete_size,p));
st = omp_get_wtime();
//write level is taking short time because of the exact changes being handled
write_levels(&g_meta, MN_list,Hubs,trimmed,propagate_changed_up, propagate_changed_down,hubsize, p);

//Graph* g_meta, MetaNode*& MN_list,bool*& propagate_changed_up, bool*& propagate_changed_down,bool*& p_up, bool*& p_down, bool*&trimmed, int hubsize,int p 
propagate_all(&g_meta, MN_list, propagate_changed_up, propagate_changed_down, p_up, p_down, trimmed, hubsize, p );
// propagate_inserts(&g_meta,&inserts, MN_list, propagate_changed_up, propagate_changed_down, p_up, p_down, trimmed, hubsize, p );

color("purple");
printf("\n Time for Writing Levels: %f \n", (float)(omp_get_wtime()-st));
color("reset");
// // ******************* HUBS FINDING COMPLETED ****************************





// // ******************* UPDATING PROPERTY ****************************
// st = omp_get_wtime();
// //update_property(&g, &g_meta, Hubs, &inserts, &deletes, &SCCx, &sccMAP, MN_list, trimmed,propagate_changed_up, propagate_changed_down, insert_status, delete_status,hubsize, p);
//update_inserts(&g, &g_meta, Hubs, &inserts, &deletes, &SCCx, &sccMAP, MN_list, trimmed,propagate_changed_up, propagate_changed_down, p_up, p_down, insert_status, delete_status,hubsize, p);
// //update_deletes(&g, &g_meta, Hubs, &inserts, &deletes, &SCCx, &sccMAP, MN_list, trimmed,propagate_changed_up, propagate_changed_down, insert_status, delete_status,hubsize, p);

color("purple");
printf("\n Time for Update: %f\n", (float)(omp_get_wtime()-st));
color("reset");
printf("Count of  completed inserts after convert changes: %d\n",count_true(insert_status,insert_size,p));
#ifdef DEBUG
  print_meta_network(&g_meta, MN_list, N, Hubs, hubsize, trimmed);
#endif

// printf("Count of  completed inserts after update propeerty: %d\n",count_true(insert_status,insert_size,p));
// printf("Count of  completed deletes after update property: %d\n",count_true(delete_status,delete_size,p));
// // ******************* UPDATING PROPERTY COMPLETED ****************************

*/

    return 0;
}



