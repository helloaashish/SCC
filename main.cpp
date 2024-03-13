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

using namespace std;

typedef pair<int,int> int_int;

// ./a.out /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/baidu_1 /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/baidu_1_ME.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/scc_baidu.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/baidu_1_Map.txt /home/users/apandey/Datasets_SCC/ChangeEdges/baidu_ce/baidu_1M_25 2141301 1503003 4

int main(int argc, char *argv[]) {
    check_file_open(argv[1],argv[2],argv[3], argv[4], argv[5], argc);
    int n,N,m,M; //number of nodes and metanodes
    int hubsize = atoi(argv[6]);
    int p = atoi(argv[7]); //number of threads

    double start_timer;

    int *e_src, *e_dest, *e_wt, *m_src, *m_dest, *m_wt;
    bool *trimmed;
    bool* insert_status;
    bool* delete_status;

    Graph g,g_meta;
    vector<int> SCCx; //SCC IDs for graph nodes
    unordered_map<int,int> sccMAP; //Mapping of SCCID from old to new 0 to N Continuous
    vector<int_int> inserts, deletes;
    vector<int> new_SCCx;
    inserts.clear();
    deletes.clear();
    int* Hubs;
    
// ******************* READING FILES ****************************
    start_timer = omp_get_wtime();
    // read_graph(argv[1], n,m, e_src, e_dest, e_wt, dummy_wt); //reading the graph data
    read_graph(argv[1], n,m, e_src, e_dest, e_wt); //reading the metagraph data
    read_graph(argv[2], N,M, m_src, m_dest, m_wt); //reading the metagraph data
    read_scc(&SCCx,n,argv[3]);
    read_sccmap(&sccMAP, argv[4]);
    read_changes(&inserts, &deletes,argv[5]);
    color("purple");
    printf("\n Time for Reading: %f \n", (float)(omp_get_wtime()-start_timer));
    color("reset");
// ******************* READING COMPLETED ****************************

  


// // ******************* CREATING GRAPHS ****************************

    double st =omp_get_wtime();
    create_graph(e_src,e_dest,e_wt,n,m,&g); //GRAPH
    create_graph(m_src,m_dest,m_wt,N,M,&g_meta); //METAGRAPH
    color("purple");
    printf("\n Time for Creating graph: %f \n", (float)(omp_get_wtime()-st));
    color("reset");
// // ******************* CREATING GRPAHS COMPLETED **************************** 

//Creating MetaNode Array for Hub Infornmation
MetaNode* MN_list = new MetaNode[N];
#pragma omp parallel for num_threads(p) schedule(dynamic)
for (int mn = 0; mn<N; mn++){
    MN_list[mn].Hub_info=new int[hubsize]{0}; //initialize all hubs as zero
    MN_list[mn].currentID = mn; //current value of SCC
    MN_list[mn].is_hub = false; //boolean to denote if Metanode is a hub
    MN_list[mn].h_idx = -1; // initialize -1 for h_idx
}


// ******************* FINDING HUBS ****************************
st = omp_get_wtime();
find_hubs(&g, &g_meta,MN_list, &SCCx,trimmed,Hubs,n,N, hubsize,p);
color("purple");
printf("\n Time for Finding Hubs: %f \n", (float)(omp_get_wtime()-st));
color("reset");
// // ******************* FINDING HUBS COMPLETED ****************************

// ******************* ADDING INSERTS & DELETES ****************************
st = omp_get_wtime();
int insert_size = int(inserts.size());
int delete_size = int(deletes.size());
insert_status = new bool[insert_size]{false}; //true if completed false if needs to be processed
delete_status = new bool[delete_size]{false};
printf("Count of  completed inserts at first: %d\n",count_true(insert_status,insert_size,p));
convert_changes(&g, &g_meta, &inserts, &deletes, &SCCx, &sccMAP, MN_list, trimmed, insert_status, delete_status, p);
color("purple");
printf("\n Time for Insert/Delets Conversion: %f \n", (float)(omp_get_wtime()-st));
color("reset");
// ******************* ADDING INSERTS & DELETES COMPLETED ****************************


// ******************* Writing Levels ****************************
bool* propagate_changed_up = new bool[N]{false};
bool* propagate_changed_down = new bool[N]{false};
printf("Count of  completed inserts: %d\n",count_true(insert_status,insert_size,p));
st = omp_get_wtime();
write_levels(&g_meta, MN_list,Hubs,trimmed,propagate_changed_up, propagate_changed_down,hubsize, p);
color("purple");
printf("\n Time for Writing Levels: %f \n", (float)(omp_get_wtime()-st));
color("reset");
// ******************* HUBS FINDING COMPLETED ****************************

// print_meta_network(&g_meta,MN_list, n,Hubs,hubsize,trimmed);
printf("Count of  completed inserts: %d\n",count_true(insert_status,insert_size,p));
// ******************* UPDATING PROPERTY ****************************
st = omp_get_wtime();
update_property(&g, &g_meta, Hubs, &inserts, &deletes, &SCCx, &sccMAP, MN_list, trimmed,propagate_changed_up, propagate_changed_down, insert_status, delete_status,hubsize, p);
color("purple");
printf("\n Time for Update: %f\n", (float)(omp_get_wtime()-st));
color("reset");

//print_meta_network(&g_meta, MN_list, N, Hubs, hubsize, trimmed);
printf("Count of  completed inserts: %d\n",count_true(insert_status,insert_size,p));
// ******************* UPDATING PROPERTY COMPLETED ****************************



    return 0;
}



