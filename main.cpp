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

using namespace std;

typedef pair<int,int> int_int;

// #define DEBUG 

int main(int argc, char *argv[]){
    check_file_open(argv[1],argv[2],argv[3], argv[4], argv[5], argc);
    int n,N,m,M; //number of nodes and metanodes
    int hubsize = atoi(argv[6]);
    int p = atoi(argv[7]); //number of threads

    double start_timer;

    int *e_src, *e_dest, *e_wt, *m_src, *m_dest, *m_wt;
    // bool *trimmed;
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

//   double s = omp_get_wtime();
//   int ret = system("awk 'NR==FNR { lines[NR-1] = $0; next } { if (FNR <= length(lines)) { $1 = lines[$1]; $2 = lines[$2] } }' /home/users/apandey/SCC-new/Examples/xbaidu /home/users/apandey/SCC-new/Examples/baidu_1M_25T");
//       color("red");
//     printf("\n Time for Creating shell: %f \n", (float)(omp_get_wtime()-s));
//     color("reset");


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
    MN_list[mn].trimmed = true;
}


// ******************* FINDING HUBS ****************************
bool* propagate_changed_up = new bool[N]{false};
bool* propagate_changed_down = new bool[N]{false};
bool* p_up = new bool[N]{false};
bool* p_down = new bool[N]{false};
st = omp_get_wtime();
find_hubs(&g, &g_meta,MN_list, &SCCx, propagate_changed_up, propagate_changed_down,Hubs, n,N, hubsize,p);
color("purple");
printf("\n Time for Finding Hubs: %f \n", (float)(omp_get_wtime()-st));
color("reset");
// print_meta_network(&g_meta, MN_list, N, Hubs, hubsize);
// // ******************* FINDING HUBS COMPLETED ****************************
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
//clean_inserts(&inserts, insert_status, trimmed, p);
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
//write_levels(&g_meta, MN_list,Hubs,trimmed,propagate_changed_up, propagate_changed_down,hubsize, p);

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



