#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <map>
#include <cmath>
#include <omp.h>
#include <unordered_map>
#include "DataStructure.hpp"
#include "ReadData.hpp"
#include "CreateGraph.hpp"
#include "TraverseMeta.hpp"
#include "PrintFunctions.hpp"

using namespace std;

typedef pair<int,int> int_int;

// #define DEBUG 

int main(int argc, char *argv[]){
    check_file_open(argv[1],argv[2],argv[3], argv[4], argv[5], argc);
    int n,N,m,M; //number of nodes and metanodes
    int p = atoi(argv[7]); //number of threads

    double start_timer,st;

    int *e_src, *e_dest, *e_wt, *m_src, *m_dest, *m_wt;
    bool* insert_status;
    bool* delete_status;

    //Many SCC detection algorithms do not map the SCCs contiguously. For example, SCCs can be 2 8 10
    // For better memory allocation we need to number SCCs contiguously (2->0; 8->1; 10->3). This is done by sccMAP
    vector<int> SCCx; //SCC IDs for graph nodes
    unordered_map<int,int> sccMAP; //Mapping of SCCID from old to new 0 to N Continuous
    vector<int_int> edge_list,inserts, deletes;
    vector<int_int> inserts_meta, deletes_meta;
    inserts.clear();
    deletes.clear();
    inserts_meta.clear();
    deletes_meta.clear();
    

printf("Threads: %d \n",p);    
// ******************* READING FILES ****************************
    start_timer = omp_get_wtime();
    read_graph(argv[1], n,m, e_src, e_dest, e_wt); //reading the graph data
    read_graph(argv[2], N,M, m_src, m_dest, m_wt); //reading the metagraph data
    read_scc(argv[3],n,&SCCx);
    read_sccmap(argv[4],&sccMAP);
    read_changes(argv[5],&inserts, &deletes, &inserts_meta, &deletes_meta, &SCCx, &sccMAP);
    color("purple");
    printf("\n Time for Reading: %f \n", (float)(omp_get_wtime()-start_timer));
    color("reset");
// ******************* READING COMPLETED ****************************




/*** Processing Inserts ****/
    st =omp_get_wtime();
    sort(inserts_meta.begin(), inserts_meta.end(),sort_first_second);
    inserts_meta.erase( unique( inserts_meta.begin(), inserts_meta.end() ), inserts_meta.end());
    int insert_percent=floor((double)inserts_meta.size()/(double)M*100.0);

    int *ins_src =new int[inserts_meta.size()];
    int *ins_dest =new int[inserts_meta.size()];
    for(int i=0;i<inserts_meta.size();i++)
    {
      ins_src[i]=inserts_meta[i].first;
      ins_dest[i]=inserts_meta[i].second;
    }

    printf("%d  %d %d", M,inserts_meta.size(),insert_percent); 
    color("purple");
    printf("\n Time for Processing Inserts: %f \n", (float)(omp_get_wtime()-st));
    color("reset");
/**** Processing Inserts *****/

/*for(int i=0;i<inserts_meta.size();i++)
{
	printf("%d  %d \n",inserts_meta[i].first, inserts_meta[i].second);
}*/

// // ******************* CREATING GRAPHS ****************************
    
    int MI=inserts_meta.size();
    Graph g,g_meta,gI_meta;
    MetaNode* MN_list = new MetaNode[N];
    st =omp_get_wtime();
    //process_inserts(inserts_meta,active_nodes,src,dest); //Remove Duplicates in insert_meta and identify nodes that are active
    create_graph(e_src,e_dest,e_wt,n,m,&g); //GRAPH
    create_graph(m_src,m_dest,m_wt,N,M,&g_meta); //METAGRAPH
    create_graph(ins_src,ins_dest,e_wt,N,MI,&gI_meta);//INSERTION GRAPH
    meta_node_info(MN_list, N,&g,&gI_meta); // Store Information in  Meta_node Array
    color("purple");
    printf("\n Time for Creating graph: %f \n", (float)(omp_get_wtime()-st));
    color("reset");
// // ******************* CREATING GRPAHS COMPLETED **************************** 

 

    
/*Creating MetaNode Array for Hub Infornmation*/


int* Hub_Id = new int[1000];

int hub_th_high=30; //nodes with degree >= threshold are hubs
int hub_th_low=3; //nodes with degree >= threshold are hubs
int non_zero=0;


st = omp_get_wtime();
#pragma omp parallel for num_threads(p) schedule(guided)
for (int mn = 0; mn<N; mn++){

	//Sources and sinks are marked as trim
	if(MN_list[mn].out+ MN_list[mn].outI==0)
	{MN_list[mn].trimmed=true;
	MN_list[mn].is_hub=false;

	if(MN_list[mn].in>0)
          {
	    int first_neigh=MN_list[mn].bn1;
	    if(MN_list[first_neigh].out+MN_list[first_neigh].outI==1)
	     {MN_list[first_neigh].trimmed=true;}  
	  }
	
	if(MN_list[mn].inI>0)
          {
	    int first_neigh=MN_list[mn].bn1I;
	    if(MN_list[first_neigh].out+MN_list[first_neigh].outI==1)
	     {MN_list[first_neigh].trimmed=true;}  
	  }
	   continue;}	
	
	if(MN_list[mn].in+MN_list[mn].inI==0)
	{MN_list[mn].trimmed=true;
	MN_list[mn].is_hub=false;
	
	if(MN_list[mn].out>0)
          {
	    int first_neigh=MN_list[mn].fn1;
	    if(MN_list[first_neigh].in+MN_list[first_neigh].inI==1)
	     {MN_list[first_neigh].trimmed=true;}  
	  }
	
	if(MN_list[mn].outI>0)
          {
	    int first_neigh=MN_list[mn].fn1I;
	    if(MN_list[first_neigh].in+MN_list[first_neigh].inI==1)
	     {MN_list[first_neigh].trimmed=true;}  
	  }

	   continue;}	

	//Criteria for Hub
	if(MN_list[mn].out>hub_th_high  || MN_list[mn].in>hub_th_high)	
	if(MN_list[mn].out>hub_th_low &&  MN_list[mn].in>hub_th_low)	
	{
	MN_list[mn].is_hub=true;
	MN_list[mn].affected=true;
	}//end if for hub


}//end of for
/*****************************/
	

color("purple");
printf("\n Time for Finding Hubs: %f \n", (float)(omp_get_wtime()-st));
color("reset");
non_zero++;
printf("\n N: %d Hubs: %d \n",N,non_zero);

/******************* FINDING HUBS COMPLETED ****************************/


/*** FIND TRIMS ******/


int t=0;
int threshold=2; //threshold of how far to check for trimmed nodes
st = omp_get_wtime();
SCC_trim(&g_meta, &gI_meta, MN_list,N,threshold,p); 
printf("\n Time for Trim: %f \n", (float)(omp_get_wtime()-st));
/*** FIND TRIMS ******/
/*st = omp_get_wtime();
#pragma omp parallel for num_threads(p) schedule(guided)
for(int i=0;i<M;i++)
{
 int child = g_meta.f_col_idx[i];
}

printf("\n Time for Hub : %f \n", (float)(omp_get_wtime()-st));*/
//Creating MetaNode Array for Hub Infornmation
// ******************* ADDING INSERTS & DELETES ****************************
/*int insert_size = int(inserts.size());
int delete_size = int(deletes.size());
insert_status = new bool[insert_size]{false}; //true if completed false if needs to be processed
delete_status = new bool[delete_size]{false};
*/
    //printf("Count of  completed inserts at first: %d\n",count_true(insert_status,insert_size,p));
//printf("Count of  completed deletes at first: %d\n",count_true(delete_status,delete_size,p));
//printf("Count of  completed inserts after convert changes: %d\n",count_true(insert_status,insert_size,p));
//printf("Count of completed inserts %d\n",int(inserts.size()));
//printf("Count of  completed deletes after convert changes: %d\n",count_true(delete_status,delete_size,p));
int iter=1;
t=0;
printf("I am here \n");
st = omp_get_wtime();
/*while(t<3)
{
st = omp_get_wtime();
SCC_hubs(&g_meta, &gI_meta, MN_list,Hub_Id,M,MI,iter,p); 
printf("\n Time for Hub : %f \n", (float)(omp_get_wtime()-st));
st = omp_get_wtime();
printf("\n Time for Others : %f \n", (float)(omp_get_wtime()-st));
t++;
}
 double ed = omp_get_wtime();
color("purple");
printf("\n Time for Insert/Delets Conversion: %f \n", (float)(ed-st));
color("reset");
*/
    // ******************* ADDING INSERTS & DELETES COMPLETED ****************************


//printf("Count of  completed inserts after convert changes: %d\n",count_true(insert_status,insert_size,p));
#ifdef DEBUG
  //print_meta_network(&g_meta, MN_list, N, Hubs, hubsize, trimmed);
#endif

}



