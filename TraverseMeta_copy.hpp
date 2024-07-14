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

void identify_SCC_all(Graph* g_meta, Graph* gI_meta, MetaNode*& MN_list, int N, int hubs, int iter, int p)
{
#pragma omp parallel for num_threads(p)schedule(guided)
	for(int i=0;i<N;i++)
	{

    int start,end;
 
    if(MN_list[i].trimmed){continue;}
    if(!MN_list[i].is_hub){continue;}
    if(MN_list[i].down!=iter && MN_list[i].up!=iter){continue;}    


    if(MN_list[i].down==iter)
	  {
	   start = g_meta->f_row_ptr[i];
	   end = g_meta->f_row_ptr[i+1];
           for (int out = start; out<end; out++)
	    {
          int child = g_meta->f_col_idx[out];
	  if(MN_list[child].trimmed){continue;}
	    
	    for(int x=0;x<hubs;x++)
	    {
	    //Option A: value=0;  Mark as 1; and pass down stream
         if(MN_list[child].Hub_info[i]==0)
           {
          MN_list[child].Hub_info[i]=1;
          MN_list[child].down=iter+1; //mark as true;
           }

         //Option B: value =-1; that means cycle closes and SCC identified
          // No need to forward
          //Update value to 5 to indicate in SCC of hub
            if(MN_list[child].Hub_info[i]==-1)
            {MN_list[child].Hub_info[i]=5;}

          //Option C: value =1; that means there is already a path
          //Option D: value =5; that means is part of hub
          // No need to forward: nothing to do
	    
	    }//end of for hubs
	    
	    
	    }
	   
	   start = gI_meta->f_row_ptr[i];
	   end = gI_meta->f_row_ptr[i+1];
           for (int out = start; out<end; out++)
	    {
	    
	    int child = gI_meta->f_col_idx[out];
	    if(MN_list[child].trimmed){continue;}
	     
	    
	    for(int x=0;x<hubs;x++)
	    {
	    
          //Option A: value=0;  Mark as 1; and pass down stream
         if(MN_list[child].Hub_info[i]==0)
           {
          MN_list[child].Hub_info[i]=1;
          MN_list[child].down=iter+1; //mark as true;
           }

         //Option B: value =-1; that means cycle closes and SCC identified
          // No need to forward
          //Update value to 5 to indicate in SCC 
            if(MN_list[child].Hub_info[i]==-1)
            {MN_list[child].Hub_info[i]=5;}

          //Option C: value =1; that means there is already a path [CHECK AASHISH]
          // But it was found at an earlier iterations, so this is cycle/fork in a hub free zone
          // Update value to 2 to indicate SCC of hub
            if(MN_list[child].Hub_info[i]==1)
            {MN_list[child].Hub_info[i]=2;}

          //Option D: value =5; that means is part of hub
          // No need to forward: nothing to do	    
	    
	    }//end of for hub
	    
	    }//end of for start to end
	  }//end of if

          if(MN_list[i].up==iter)
	  {
	   start = g_meta->b_row_ptr[i];
	   end = g_meta->b_row_ptr[i+1];
           for (int out = start; out<end; out++)
	    {
	    
          int parent  = g_meta->b_col_idx[out];
	  if(MN_list[parent].trimmed){continue;}

	    for(int x=0;x<hubs;x++)
	    {
	    
	    //Option A: value=0;  Mark as -1; and pass down stream
         if(MN_list[parent].Hub_info[i]==0)
           {
          MN_list[parent].Hub_info[i]=-1;
          MN_list[parent].down=iter+1; //mark as true;
           }

         //Option B: value =1; that means cycle closes and SCC identified
          // No need to forward
          //Update value to 5 to indicate in SCC of hub
            if(MN_list[parent].Hub_info[i]==1)
            {MN_list[parent].Hub_info[i]=-5;}

          //Option C: value =-1; that means there is already a path
          // No need to forward: nothing to do

          //Option D: value =5; that means is part of hub
          // No need to forward: nothing to do
	    }//end of for hub
	    
	    }//end of for start to end
	   
	   start = gI_meta->b_row_ptr[i];
	   end = gI_meta->b_row_ptr[i+1];
           for (int out = start; out<end; out++)
	    {
	    
          int parent = gI_meta->f_col_idx[out];
	  if(MN_list[parent].trimmed){continue;}

	    for(int x=0;x<hubs;x++)
	    {
	     //Option A: value=0;  Mark as -1; and pass down stream
         if(MN_list[parent].Hub_info[i]==0)
           {
          MN_list[parent].Hub_info[i]=-1;
          MN_list[parent].down=iter+1; //mark as true;
           }

         //Option B: value =1; that means cycle closes/fork and SCC identified
          // No need to forward
          //Update value to 5 to indicate in SCC of hub
            if(MN_list[parent].Hub_info[i]==1)
            {MN_list[parent].Hub_info[i]=-5;}


          //Option C: value =-1; that means there is already a path [CHECK AASHISH]
          // But it was found at an earlier iterations, so this is cycle in a hub free zone
          // Update value to -2 to indicate SCC of hub
            if(MN_list[parent].Hub_info[i]==-1)
            {MN_list[parent].Hub_info[i]=-2;}

            //Option D: value =5; that means is part of hub
          // No need to forward: nothing to do
	    }
	    }//end of hub 
	  }//end of if
	}

}
/*******/
void identify_SCC_hubs(Graph* g_meta, Graph* gI_meta, MetaNode*& MN_list, int* Hub_Id, int hubs, int iter, int p)
{
    int count_i=0;
    int count_d = 0;
    //printf("%d --\n", inserts_meta->size());
    int non_zero=0;
    int node;
    
   for (int i = 0; i<hubs; i++)
    {
        int start,end;
	int mn=Hub_Id[i];
	int down=MN_list[mn].down;
	int up=MN_list[mn].up;
//	printf("H:%d iter%d Down%d Up%d--\n",mn,iter,down,up);
      //Go over meta graph to update hub info
      //
      //Update Downstream only if iteration value matches
          if(down==iter)
	  {
	//Over original meta graph	  
           start = g_meta->f_row_ptr[mn];
	   end = g_meta->f_row_ptr[mn+1];
         
//	printf("DOWN %d \n",mn);
          #pragma omp parallel for num_threads(p)schedule(guided)
        for (int out = start; out<end; out++)
	{
          int child = g_meta->f_col_idx[out];
	 if(MN_list[child].trimmed){continue;}


	 //Get the active hubs
	
       	 //Option A: value=0;  Mark as 1; and pass down stream
	 if(MN_list[child].Hub_info[i]==0)
           {
	  MN_list[child].Hub_info[i]=1;
	  MN_list[child].down=iter+1; //mark as true;
	   } 
	  
	 //Option B: value =-1; that means cycle closes and SCC identified
	  // No need to forward
	  //Update value to 5 to indicate in SCC of hub
	    if(MN_list[child].Hub_info[i]==-1)
	    {MN_list[child].Hub_info[i]=5;}    
	 
	  //Option C: value =1; that means there is already a path
	  //Option D: value =5; that means is part of hub
	  // No need to forward: nothing to do
	    
	}//end of for
	
	//Over insert meta graph	  
           start = gI_meta->f_row_ptr[mn];
	   end = gI_meta->f_row_ptr[mn+1];
         
//	printf("GI DOWN %d \n",mn);
           #pragma omp parallel for num_threads(p)schedule(guided)
        for (int out = start; out<end; out++)
	{
          int child = gI_meta->f_col_idx[out];
	 if(MN_list[child].trimmed){continue;}
	 
	  //Option A: value=0;  Mark as 1; and pass down stream
	 if(MN_list[child].Hub_info[i]==0)
           {
	  MN_list[child].Hub_info[i]=1;
	  MN_list[child].down=iter+1; //mark as true;
	   } 
	  
	 //Option B: value =-1; that means cycle closes and SCC identified
	  // No need to forward
	  //Update value to 5 to indicate in SCC 
	    if(MN_list[child].Hub_info[i]==-1)
	    {MN_list[child].Hub_info[i]=5;}    
	 
	  //Option C: value =1; that means there is already a path [CHECK AASHISH]
	  // But it was found at an earlier iterations, so this is cycle/fork in a hub free zone
	  // Update value to 2 to indicate SCC of hub
	    //if(MN_list[child].Hub_info[i]==1)
	    //{MN_list[child].Hub_info[i]=2;}    

	  //Option C: value =1; that means there is already a path
	  //Option D: value =5; that means is part of hub
	  // No need to forward: nothing to do
	}//end of for	    
       }//end of if down==iter;

	  //Upstream
      //Go over meta graph to update hub info
          if(up!=0)
	  {

          //Over Original meta graph 
           start = g_meta->b_row_ptr[mn];
	   end = g_meta->b_row_ptr[mn+1];
         
//	printf("UP %d \n",mn);
           #pragma omp parallel for num_threads(p)schedule(guided)
        for (int out = start; out<end; out++)
	{
          int parent = g_meta->b_col_idx[out];
	 if(MN_list[parent].trimmed){continue;}
	 
	 //Option A: value=0;  Mark as -1; and pass down stream
	 if(MN_list[parent].Hub_info[i]==0)
           {
	  MN_list[parent].Hub_info[i]=-1;
	  MN_list[parent].up=iter+1; //mark as true;
	   } 
	  
	 //Option B: value =1; that means cycle closes and SCC identified
	  // No need to forward
	  //Update value to 5 to indicate in SCC of hub
	    if(MN_list[parent].Hub_info[i]==1)
	    {MN_list[parent].Hub_info[i]=-5;}    
	 
	  //Option C: value =-1; that means there is already a path
	  // No need to forward: nothing to do
	  
	  //Option D: value =5; that means is part of hub
	  // No need to forward: nothing to do
	    
	}//end of for
          
	//Over iteration meta graph 
           start = gI_meta->b_row_ptr[mn];
	   end = gI_meta->b_row_ptr[mn+1];
         
//	printf("GI UP %d \n",mn);
           #pragma omp parallel for num_threads(p)schedule(guided)
        for (int out = start; out<end; out++)
	{
          int parent = gI_meta->b_col_idx[out];
	 if(MN_list[parent].trimmed){continue;}

	  //Option A: value=0;  Mark as -1; and pass down stream
	 if(MN_list[parent].Hub_info[i]==0)
           {
	  MN_list[parent].Hub_info[i]=-1;
	  MN_list[parent].up=iter+1; //mark as true;
	   } 
	  
	 //Option B: value =1; that means cycle closes/fork and SCC identified
	  // No need to forward
	  //Update value to 5 to indicate in SCC of hub
	    if(MN_list[parent].Hub_info[i]==1)
	    {MN_list[parent].Hub_info[i]=-5;}    
	 
	  
	  //Option C: value =-1; that means there is already a path [CHECK AASHISH]
	  // But it was found at an earlier iterations, so this is cycle in a hub free zone
	  // Update value to -2 to indicate SCC of hub
	    if(MN_list[parent].Hub_info[i]==-1)
	    {MN_list[parent].Hub_info[i]=-2;}    
	  
	    //Option D: value =5; that means is part of hub
	  // No need to forward: nothing to do
	}   

       }//end of if down==iter;

/*********/
//	printf("END %d \n",mn);
    }//end of for hubs

  // printf("All done \n");
}
//End of function process inserts
	

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
// ******************************************************************************

// ******************************************************************************
void create_graphXX(int* src, int* dest, int* wt_list, int n, int m, Graph* graph){

    // if (wt_list->size() == 0){wt_list->resize(int(edge_list->size()),1);}
    //resize both col_idx
    //
printf("I came here TTT\n");
    graph->f_col_idx = new int[m]();
    graph->b_col_idx = new int[m]();
    graph->edge_wt = new int[m]();
    graph->f_row_ptr = new int[n+1]();
    graph->b_row_ptr = new int[n+1]();
    graph->node_count = n;
    graph->edge_count = m;

    graph->in_deg = new int[n]{0};
    graph->out_deg = new int[n]{0};
    graph->is_hub = new bool[n]{0};
    

printf("First I came here \n");
    // parallel in here did not work???
    for (int i = 0; i<m; i++){
        graph->in_deg[dest[i]]++;
        graph->out_deg[src[i]]++;
        graph->f_row_ptr[src[i]+1]++;
        graph->b_row_ptr[dest[i]+1]++;
    }
printf("I came here \n");
    //if(graph->in_deg>30 &&graph->out_deg> 30) {graph->is_hub=true;}
    // for (const auto& edge : *edge_list){
    //     graph->f_row_ptr[edge.first + 1]++;
    //     graph->b_row_ptr[edge.second + 1]++;
    // }
    //finding the cumulative sum for starting vertex
    for (int i = 1; i<= n; i++){
        graph->f_row_ptr[i] += graph->f_row_ptr[i-1];
        graph->b_row_ptr[i] += graph->b_row_ptr[i-1];
    }
    
    // vector<int> temp_next_outgoing_idx(n,0);
    // vector<int> temp_next_incoming_idx(n,0);
    int* temp_next_outgoing_idx = new int[n]();
    int* temp_next_incoming_idx = new int[n]();

    for (int i = 0; i<m; i++){
        int oIdx = graph->f_row_ptr[src[i]] + temp_next_outgoing_idx[src[i]];
        int iIdx = graph->b_row_ptr[dest[i]] + temp_next_incoming_idx[dest[i]];

        graph->f_col_idx[oIdx] = dest[i];
        graph->edge_wt[oIdx] = wt_list[i];
        graph->b_col_idx[iIdx] = src[i];

        temp_next_outgoing_idx[src[i]]++;
        temp_next_incoming_idx[dest[i]]++;
    }
    
}
// ******************************************************************************

// ******************************************************************************
void find_hubs(Graph* g, Graph* mg, MetaNode*& MN_list,vector<int>* SCCx, bool*& propagate_changed_up, bool*& propagate_changed_down, int*& Hubs, int n, int N, int hubsize, int p){
    int count=0;
    vector<int_int> hub_candidates(N); //TODO change to array
    int non_zero = 0;
    // bool* trim = new bool[N]; //boolean to indicate if node is source or sink 
    int* temp_hub = new int[hubsize]{0}; 


//Identify Candidates for Hubs
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int id =0; id<N; ++id)
    {
	int th_in_deg=10;
        int th_out_deg=10;	
        int pr=0; //product of in and out degree
        int v = 0; //count for vertices that are not src or sink
        int deg_out = mg->out_deg[id];
        int deg_in = mg->in_deg[id];
        // if ((mg->num_parents(id) >0) && (mg->num_childrens(id)>0)) //TODO can we get value during read and then store in array
        if ((deg_in >th_in_deg) &&  (deg_out>th_out_deg)) //TODO can we get value during read and then store in array
        {
            // pr = mg->num_parents(id) * mg->num_childrens(id);
            //pr = deg_out * deg_in;
            //MN_list[id].trimmed = false;
            // trim[id] = false;
    #pragma omp atomic capture
            v=non_zero++; 
            //hub_candidates.at(v).first = pr;
            //hub_candidates.at(v).second = id;
        }
        // else{
        //     trim[id] = true;
        // }
    }//end of for
    
printf("XX %d : \n", non_zero);
    //Erase non-zeros to keep only viable candidates, and then sort by product of in*out
   //hub_candidates.erase(hub_candidates.begin() + non_zero, hub_candidates.end());
   //sort(hub_candidates.begin(), hub_candidates.end());
   //  sort(hub_candidates.begin(), hub_candidates.end(), [](const int_int &a, const int_int &b)
   //      { return a.first > b.first; });

    /*#pragma omp parallel for num_threads(p) schedule(guided) //we may not need this parallel for
    for (int h=0; h<hubsize; ++h)
    {
        temp_hub[h] = hub_candidates.at(h).second; //TODO check if we can replace temp_hub with Hubs
        MN_list[hub_candidates.at(h).second].is_hub = true;
        MN_list[hub_candidates.at(h).second].h_idx = h;
        MN_list[hub_candidates.at(h).second].Hub_info[h] = 5;
        propagate_changed_up[hub_candidates.at(h).second] = true;
        propagate_changed_down[hub_candidates.at(h).second] = true;
    }

    // trimmed = trim;
    Hubs = temp_hub;
    hub_candidates.clear();
*/
}
// ******************************************************************************


 // ******************************************************************************
void clean_inserts(vector<int_int>* inserts, bool*& insert_status, bool*& trimmed, int p){
    //loop through inserts and mark true for insert status
    #pragma omp parallel for num_threads(p)
    for(int ins = 0; ins<inserts->size(); ins++){
        int src = inserts->at(ins).first;
        int dest = inserts->at(ins).second;

        // check within scc
        if(src == dest){
            insert_status[ins] = true;
        }
         // check if any trimmed
        if(trimmed[src] || trimmed[dest]){
            insert_status[ins] = true;
        }

    }
}
 // ******************************************************************************


// // ******************************************************************************
// void write_levels_inserts(Graph* g_meta, vector<int_int>* inserts, bool*& insert_status, MetaNode*& MN_list, int*& Hubs,bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, int hubsize, int p){
//     #pragma omp parallel for num_threads(p) schedule(dynamic)
//     for (int ins=0; ins<inserts->size(); ins++){
//         if(!insert_status[ins]){

//         }
//     }

//     for (int h=0; h<hubsize; h++){
//         // int current_hub = Hubs[h];
//         // MN_list[current_hub].Hub_info[h] = 5; //mark itself as 5 to show it is a hub

// //Get start and end indices of children and parent
//         int_int range_children = g_meta->get_children(current_hub);
//         int_int range_parents = g_meta->get_parents(current_hub);

//         // look for the children of the hub and mark them +1 if untrimmed
//         // #pragma omp parallel for num_threads(p) schedule(dynamic)
//         for (int out = range_children.first; out<range_children.second; out++){
//             int current_neigh = g_meta->f_col_idx[out];      
//             if (!trimmed[current_neigh]){
//                 MN_list[current_neigh].Hub_info[h] = 1;
//                 propagate_changed_down[current_neigh] = true; //mark that 1 to be propagated to children
//             }
//         }
//          // look for tht parents of the hub and mark them -1 if untrimmed
//         //  #pragma omp parallel for num_threads(p) schedule(dynamic)
//         for (int in = range_parents.first; in<range_parents.second; in++){
//             int current_neigh = g_meta->b_col_idx[in];      
//             if (!trimmed[current_neigh]){
//                 MN_list[current_neigh].Hub_info[h] = -1;
//                 propagate_changed_up[current_neigh] = true; //mark that -1 to be propagated to parent
//             }
//         }
//     }
// }
// // ******************************************************************************

// ******************************************************************************

// ******************************************************************************


// // ******************************************************************************
void write_levels_new(Graph* g_meta,MetaNode*& MN_list, int N, int p){

	int start, end;
	int child, parent;
	int index;
    //    printf("%d :: \n", sizeof(MN_list));    
    for (int mn=0; mn<N; mn++){

	if(!MN_list[mn].is_hub){continue;}

	//int mn=69;
	 index=MN_list[mn].h_idx;
       	//printf("******%d --\n",mn);    
	//Check if children are hubs
	    //Mark with 1 for upstream
	  start = g_meta->f_row_ptr[mn];
	  end = g_meta->f_row_ptr[mn+1];

        //  printf("%d...\n",end-start);	
#pragma omp parallel for num_threads(p) schedule(guided)
	  for (int out = start; out<end; out++){
            int child = g_meta->f_col_idx[out];
          
		if(MN_list[child].trimmed){continue;}

               MN_list[child].Hub_info[index]=1;
              
	   }//end of for children


       	    //Check if parents are hubs
	    //Mark with -1 for upstream
	  start = g_meta->b_row_ptr[mn];
	  end = g_meta->b_row_ptr[mn+1];
        //  printf("%d...\n",end-start);	
	#pragma omp parallel for num_threads(p) schedule(guided)
	  for (int out = start; out<end; out++){
            int parent = g_meta->f_col_idx[out];
          
		if(MN_list[parent].trimmed){continue;}
               MN_list[parent].Hub_info[index]=-1;
              
	   }//end of for parent

//	   printf("%d: Child:%d Parent:%d \n", mn,children,parents);
        }//end of for meta nodes
    }
// // ******************************************************************************

// ******************************************************************************
// *** ISSUE Sync ****

// // ******************************************************************************
void write_levels(Graph* g_meta,MetaNode*& MN_list, int*& Hubs,bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, int hubsize, int p){
    #pragma omp parallel for num_threads(p) schedule(dynamic)
    for (int h=0; h<hubsize; h++){
        int current_hub = Hubs[h];
        MN_list[current_hub].Hub_info[h] = 5; //mark itself as 5 to show it is a hub

//Get start and end indices of children and parent
        int_int range_children = g_meta->get_children(current_hub);
        int_int range_parents = g_meta->get_parents(current_hub);

        // look for the children of the hub and mark them +1 if untrimmed
        // #pragma omp parallel for num_threads(p) schedule(dynamic)
        for (int out = range_children.first; out<range_children.second; out++){
            int current_neigh = g_meta->f_col_idx[out];      
            if (!trimmed[current_neigh]){
                MN_list[current_neigh].Hub_info[h] = 1;
                propagate_changed_down[current_neigh] = true; //mark that 1 to be propagated to children
            }
        }
         // look for tht parents of the hub and mark them -1 if untrimmed
        //  #pragma omp parallel for num_threads(p) schedule(dynamic)
        for (int in = range_parents.first; in<range_parents.second; in++){
            int current_neigh = g_meta->b_col_idx[in];      
            if (!trimmed[current_neigh]){
                MN_list[current_neigh].Hub_info[h] = -1;
                propagate_changed_up[current_neigh] = true; //mark that -1 to be propagated to parent
            }
        }
    }
}

void XXXprocess_inserts(Graph* g_meta, vector<int_int>* inserts_meta, MetaNode*& MN_list, bool*& insert_status,int p)
{
    int count_i=0;
    int count_d = 0;

    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int i = 0; i<inserts_meta->size(); i++)
    {
        //Obtain src and dest of insert
    int m_src = inserts_meta->at(i).first;
    int m_dest = inserts_meta->at(i).second;

        if (m_src == m_dest){
        insert_status[i] = true;} //inserted within same SCC
    }//end of for
}
//End of function process inserts
	
	
void convert_changes(Graph* g, Graph* g_meta, vector<int_int>* inserts,vector<int_int>* deletes, vector<int>* SCCx, MetaNode*& MN_list, bool*& insert_status,bool*& delete_status, int p){
    int count_i=0;
    int count_d = 0;

    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int i = 0; i<inserts->size(); i++)
    {
        //Obtain src and dest of insert
    int src = inserts->at(i).first;
    int dest = inserts->at(i).second;

    //   //Convert to corresponding MetaNode or SCC
    //  int m_src = sccMAP->at(SCCx->at(src));
    //  int m_dest = sccMAP->at(SCCx->at(dest));

          //Convert to corresponding MetaNode or SCC
    int m_src = SCCx->at(src);
    int m_dest = SCCx->at(dest);

        if (m_src == m_dest){
        insert_status[i] = true;} //inserted within same SCC

 /* else{
       if(trimmed[m_src]){
    //        // if(g_meta->num_childrens(m_src) == 0)
    //         {
             trimmed[m_src] = false;
    //         }
         }

     if(trimmed[m_dest]){
    //        // if(g_meta->num_parents(m_dest) == 0) 
    //         {
             trimmed[m_dest] = false;
    //         }
         }
        }*/
    // RENAME THEM
    //inserts->at(i).first = m_src;
    //inserts->at(i).second = m_dest;


    // if(trimmed[m_dest] && (g_meta->num_parents(m_dest) == 0)){
    //         trimmed[m_dest] = false;
    //     }

    //Mark MetaNode trim=false if particiapting in insert
    // if(trimmed[m_src] && (g_meta->num_childrens(m_src) == 0)){
    //         trimmed[m_src] = false;
    //     }

    // if(trimmed[m_dest] && (g_meta->num_parents(m_dest) == 0)){
    //         trimmed[m_dest] = false;
    //     }

    // if (m_src == m_dest){
    //     insert_status[i] = true;} //inserted within same SCC
    // else{
    //     // #pragma omp atomic
    //     //     count_i++;

    //     //different scc insertion
    //     // mark status false indicating to be procesased further
    //     //mark corresponding MetaNodes as src and dest of inserts
    //     //TODO check if this is required
    //     insert_status[i] = false;
    //     MN_list[m_src].inserted_from = true;
    //     MN_list[m_dest].inserted_to = true;

    //     //Mark MetaNode trim=false if particiapting in insert
    //     if(trimmed[m_src] && (g_meta->num_childrens(m_src) == 0)){
    //         trimmed[m_src] = false;
    //     }
    //     if(trimmed[m_dest] && (g_meta->num_parents(m_dest) == 0)){
    //         trimmed[m_dest] = false;
    //     }

    //     // if still trimmed then mark completed for the inserts
    //     // if(trimmed[m_src] || trimmed[m_dest]){
    //     //     // insert_status[i] = true; // mark true for the inserted
    //     // }
    //     }
    }//end of for


    // #pragma omp parallel for num_threads(p) schedule(dynamic)
    // for(int i = 0; i<deletes->size(); i++)
    // {
    //     int src = deletes->at(i).first;
    //     int dest =  deletes->at(i).second;

    //     int m_src = sccMAP->at(SCCx->at(src));
    //     int m_dest = sccMAP->at(SCCx->at(dest));

    //     //         // RENAME THEM
    //     deletes->at(i).first = m_src;
    //     deletes->at(i).second = m_dest;

    //     if (m_src == m_dest){ //within scc deletion
    //         //check key edge, if not key edge, delete and mark done for the delete
    //         int_int children_range = g->get_children(src);
    //         for (int c = children_range.first; c<children_range.second; c++){
    //             if (dest == g->f_col_idx[c]){
    //                 if (g->edge_wt[c] == -1) {
    //                     delete_status[i] = false;
    //                 //   #pragma omp atomic
    //                 //     count_d++;
    //                     } // key edge deletion mark false
                    
    //                 if (g->edge_wt[c] == 1) 
    //                 {
    //                     delete_status[i] = true; // delete completed because of non-key edge deletion
    //                     g->edge_wt[c] == 0;
    //                 }

    //                 if (g->edge_wt[c] == 0)  //duplicate deletion
    //                 {
    //                     delete_status[i] = true; // delete completed because of non-key edge deletion
    //                 }
    //             }
    //         }
    //     }
    //     //Inter SCC deletion
    //     // Weight processing
    //     if (m_src != m_dest)
    //     {
    //         delete_status[i] = true;
    //     //     int_int c_range = g_meta->get_children(m_src);
    //     //    for (int c = c_range.first; c<c_range.second; c++)
    //     //    {
    //     //        printf("Processing Child %d for src %d\n", g_meta->f_col_idx[c],src);
    //     //     if (m_dest == g_meta->f_col_idx[c]){

    //     //         g_meta->edge_wt[c]--; // can this be done????
    //     //     }
    //     //    }

    //    }
    // }
}
// ******************************************************************************

	

// ******************************************************************************
void copy_hubInfo(int*& from, int*& to, int hubsize){
    for (int i = 0; i< hubsize; i++){
        //
        if (from[i] == 5){to[i] = 5;}
        else if(to[i] == 0){
            to[i] = from[i];
        }
        else if(from[i]*to[i]<0){
            to[i] = 5;
            // printf("Changed SCC ID from CopyHubinfo\n");
        }
    }
}
// ******************************************************************************

// ******************************************************************************
int_int action_from_hub(MetaNode*& MN_list,int from,int to, int*& Hubs,int hubsize){
    int_int return_status;
    return_status.first = 3; //differ status
    return_status.second = -1; // reference hub

    int src = MN_list[from].currentID;
    int dest = MN_list[to].currentID;

    //already inside parallel region
    for (int i = 0; i<hubsize; i++){
        int id = MN_list[Hubs[i]].h_idx;
        int val = MN_list[src].Hub_info[id] * MN_list[dest].Hub_info[id];

        // printf("found prod value %d for hub %d\n",val, Hubs[i]);
        // // If src and dest are opposite side of a hub
        // // if src is 1 then SCC will be created
        // // SCC will not be created otherwise "Addition of direct path"
        // // CASE A
        if (val == -1){
            // color("red");
            // printf("found -1\n");
            // color("reset");
            if (MN_list[src].Hub_info[i] > 0)
            {
                // printf("(%d->%d) Returned as CREATED SCC with reference hub %d \n",from,to,Hubs[id]);
                return {1,i};
            }
            else
            {
                //SCC will not be createe as path already exixts 
                // src -> hub -> dest
                // printf("(%d->%d) Returned as NOT CREATED SCC with reference hub %d \n",from,to,Hubs[id]);
                return {0,i};
            }
        }

        //CASE B
        else if(val == 1)
        {
            // this means both src and dest are in same side of hub
            // for this if we search for dest to hub and find src ; then SCC will be created
            // if we do not find src in paths then scc will not be created
                    #ifdef DEBUG
                            printf("(%d,%d) FOUND COMMOPN HUB SAME SIDE\n", from, to, Hubs[id]);
                    #endif

            return_status = {-1,i};
            
            // return {-1,i};
        }

        //CASE C
        else if(val == -5)
        {
            if (src == Hubs[i]){
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC (hub->upstream) with reference hub %d \n",from,to,Hubs[id]);
                #endif
                return {-5,i};//SCC can be created hub to upstream
            }

            else{
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC (upstream ->HUB) with reference hub %d \n",from,to,Hubs[id]);
                #endif
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }

        //CASE D
        else if(val == 5)
        {
            if (dest==Hubs[i]){
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC (downstream->hub) with reference hub %d \n",from, to,Hubs[id]);
                #endif

                return {5,i};//SCC can be created down -> hub
            }
            else{
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC (hub ->down) with reference hub %d \n",from, to, Hubs[id]);
                #endif
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }    

        else if(val == 25)
        {
            #ifdef DEBUG
            printf("(%d->%d) Returned as SAME SCC",from,to);
            #endif

            return {0,i}; // returned will not be created, already in same scc
        }

    }

    #ifdef DEBUG
    printf(" (%d->%d) not created return code %d \n",from, to,return_status.first);
    #endif

    return return_status;

}
// ******************************************************************************

// ******************************************************************************
int_int action_from_hub_values(MetaNode*& MN_list,int from,int to, int*& Hubs,int hubsize){
    int_int return_status;
    return_status.first = 3; //differ status
    return_status.second = -1; // reference hub
    // color("red");
    // printf("\n\n Finding Action %d -> %d\n", from, to);
    // color("reset");
    int src = MN_list[from].currentID;
    int dest = MN_list[to].currentID;

    #ifdef DEBUG
    printf("I AM PROCESSING %d -> %d \n", src, dest);
    #endif 
    // NOTE: Further checking required because multiple hubs may be found with different return values. "Make precedence"
    // #pragma omp parallel for num_threads(p) schedule(guided)
    //already inside parallel region
    for (int i = 0; i<hubsize; i++){
        int val = MN_list[src].Hub_info[i] * MN_list[dest].Hub_info[i];
        // printf("found prod value %d for hub %d\n",val, Hubs[i]);
        // // If src and dest are opposite side of a hub
        // // if src is 1 then SCC will be created
        // // SCC will not be created otherwise "Addition of direct path"
        // // CASE A
        if (val == -1){
            // color("red");
            // printf("found -1\n");
            // color("reset");
            if (MN_list[src].Hub_info[i] > 0)
            {
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC with reference hub %d \n",from,to,Hubs[i]);
                #endif
                return {1,i};
            }
            else
            {
                //SCC will not be createe as path already exixts 
                // src -> hub -> dest
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC with reference hub %d \n",from,to,Hubs[i]);
                #endif
                return {0,i};
            }
        }

        //CASE B
        else if(val == 1)
        {
            // this means both src and dest are in same side of hub
            // for this if we search for dest to hub and find src ; then SCC will be created
            // if we do not find src in paths then scc will not be created
            #ifdef DEBUG
            printf("(%d,%d) FOUND COMMOPN HUB SAME SIDE\n", from, to, Hubs[i]);
            #endif
            return_status = {-1,i};
            
            // return {-1,i};
        }

        //CASE C
        else if(val == -5)
        {
            if (src == Hubs[i]){
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC (hub->upstream) with reference hub %d \n",from,to,Hubs[i]);
                #endif
                return {-5,i};//SCC can be created hub to upstream
            }
            else{
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC (upstream ->HUB) with reference hub %d \n",from,to,Hubs[i]);
                #endif
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }

        //CASE D
        else if(val == 5)
        {
            if (dest==Hubs[i]){
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC (downstream->hub) with reference hub %d \n",from, to,Hubs[i]);
                #endif
                return {5,i};//SCC can be created down -> hub
            }
            else{
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC (hub ->down) with reference hub %d \n",from, to, Hubs[i]);
                #endif
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }    

        else if(val == 25)
        {
            #ifdef DEBUG
            printf("(%d->%d) Returned as SAME SCC",from,to);
            #endif
            return {0,i}; // returned will not be created, already in same scc
        }

    }

    #ifdef DEBUG
    printf(" (%d->%d) not created return code %d \n",from, to,return_status.first);
    #endif
    return return_status;

}
// ******************************************************************************

// ******************************************************************************
void handle_action_initial(MetaNode*& MN_list, int*& Hubs, int hubsize, int idx, int_int action,bool*& status,int*& cases, int*& common_hub){
   
   int todo = action.first;
   bool created = false;

   if (todo == 1) //SCC will be created wrt hub
   {
    int hub_idx = action.second;
    status[idx] = true; // mark completed
    cases[idx] = 1; // created SCC 
    common_hub[idx] = hub_idx;
    created = true;
   }

   else if (todo == 0) //SCC will not be created, path already exists
   {
    status[idx] = true; //completed
    cases[idx] = 0;  // Not created SCC, path already existed
   }

   else if (todo == -1)
   {
    status[idx] = false; // not resolved
    cases[idx] = 2; //found common hub
    common_hub[idx] = action.second;
   }

   // change -5 & 5 as 1
    else if (todo == -5 || todo == 5) // SCC will be created Hub to Ancestor or Successor to Hub
    {
        int hub_idx = action.second;
        status[idx] = true; // Completed insertion
        cases[idx] = 1;  // Created SCC
        common_hub[idx]= hub_idx;
        created = true;
    }

    else if (todo == 25) // Inside same scc after merged
    {
        status[idx] = true;
        cases[idx] = 1; // Created SCC
        common_hub[idx]= action.second;
        created = true;
    }

    else // Zero for all values
    {
        status[idx] = false; // deffered
        cases[idx] = 4;  // path product = 0 for all hhubs
    }

// IMPORTANT ISSUE
// ADD THE NEW EDGES TO THE GRAPH ????


//         // //add the parent and src values
//     if (!created){
//                 int src = inserts->at(change_idx).first;
//         int dest = inserts->at(change_idx).second;
// //TODO need to get rid of critical 
//         #pragma omp critical
//         {
//         XM->at(src).children.push_back(dest);
//         XM->at(dest).parents.push_back(src);
//         }


}
// ******************************************************************************
// for any inserts, if the case is not handled, they share the +ve, -ve info to each others, 

// ******************************************************************************
void share_hubInfo(MetaNode*& MN_list, int src, int dest, bool*& propagate_changed_up, bool*& propagate_changed_down, int hubsize){
    int src_root = MN_list[src].currentID;
    int dest_root = MN_list[dest].currentID;
    #ifdef DEBUG
    printf("Sharing Hub info between %d and %d\n", src_root, dest_root);
    print_Hub_info(MN_list, src_root, hubsize);
    print_Hub_info(MN_list, dest_root, hubsize);
    #endif
    //for all hubs
    for (int h =0; h<hubsize; h++){
        int s_h = MN_list[src_root].Hub_info[h];
        int d_h = MN_list[dest_root].Hub_info[h] ;
            #ifdef DEBUG
            printf("attempting sharing!!! %d \n", h);
            #endif
        //if sharing creates SCC 
        if((s_h == 1 || s_h == 5) && (d_h == 5 || d_h == -1)){
            //scc created 
            printf("Sharing is creating SCC\n");
            // if (MN_list[src_root].is_hub && MN_list[src_root].is_hub){
            //     if (MN_list[src_root].h_idx < MN_list[dest_root].h_idx){
            //         copy_hubInfo(MN_list[dest_root].Hub_info, MN_list[src_root].Hub_info, hubsize);
            //         printf("Edges (%d->%d) Roots:(%d -> %d) Copied to %d\n",src,dest, src_root, dest_root, src_root);
            //         MN_list[dest_root].currentID = src_root;
            //         MN_list[dest_root].h_idx = MN_list[src_root].h_idx;
            //         propagate_changed_up[src_root] = true;
            //         propagate_changed_down[src_root] = true;
            //         return;
            //     } // end if

            //     else if(MN_list[src_root].h_idx > MN_list[dest_root].h_idx){
            //         copy_hubInfo(MN_list[src_root].Hub_info, MN_list[dest_root].Hub_info, hubsize);
            //         printf("Edges (%d->%d) Roots:(%d -> %d Copied to %d\n",src,dest,src_root, dest_root,dest_root);
            //         MN_list[src_root].currentID = dest_root;
            //         MN_list[src_root].h_idx = MN_list[dest_root].h_idx;
            //         propagate_changed_up[dest_root] = true;
            //         propagate_changed_down[dest_root] = true;
            //         return;
            //     }//end else if
            // }//end if both hub
            //if only one is hub
        }//end if created scc during share

        //TODO is update on all the neighbors necessary? 
        if(d_h  == 0 && s_h > 0){
            #ifdef DEBUG
            printf("SHARED SUCCESSFULLY -> %d\n", h);
            #endif
            MN_list[dest_root].Hub_info[h] = 1;
            propagate_changed_down[dest_root] = true;
             }//end if copy 1 to destination

        if (s_h == 0 && d_h < 0){
            #ifdef DEBUG
            printf("SHARED SUCCESSFULLY <-\n", h);
            #endif
            MN_list[src_root].Hub_info[h] = -1;
            propagate_changed_up[src_root] = true;
            } // end if copy -1 to src

    }// end for loop
    #ifdef DEBUG
    print_Hub_info(MN_list, src_root, hubsize);
    print_Hub_info(MN_list, dest_root, hubsize);
    #endif
}
// ******************************************************************************


// ******************************************************************************
void handle_action(MetaNode*& MN_list, int*& Hubs, int hubsize, int idx, int src, int dest, int_int action, bool*& status, int*& cases, int*& common_hub, bool*& propagate_changed_up, bool*& propagate_changed_down, int itr){
   
   int todo = action.first;
   bool created = false;
   int src_scc = MN_list[src].currentID;
   int dest_scc = MN_list[dest].currentID;

   if (todo == 1) //SCC will be created wrt hub
   {
    int hub_idx = action.second;
    status[idx] = true; // mark completed
    cases[idx] = 1; // created SCC 
    common_hub[idx] = hub_idx;
   }

   else if (todo == 0) //SCC will not be created, path already exists
   {
    status[idx] = true; //completed
    cases[idx] = 0;  // Not created SCC, path already existed
    //Copy the hubinfo anyway?
   }

   else if (todo == -1) //if differed
   {
    status[idx] = false; // not resolved
    cases[idx] = 2; //found common hub
    common_hub[idx] = action.second;
    // if (propagate_changed_down[src_scc] || propagate_changed_up[dest_scc]){
    // share_hubInfo(MN_list, src, dest,propagate_changed_up, propagate_changed_down,hubsize);
    // }//end if itr
   } // end if todo -1

   // change -5 & 5 as 1
    else if (todo == -5 || todo == 5) // SCC will be created Hub to Ancestor or Successor to Hub
    {
        int hub_idx = action.second;
        status[idx] = true; // Completed insertion
        cases[idx] = 1;  // Created SCC
        common_hub[idx]= hub_idx;
    }

    else if (todo == 25) // Inside same scc after merged
    {
        status[idx] = true;
        cases[idx] = 1; // Created SCC
        common_hub[idx]= action.second;

    }

    else // Zero for all values
    {
        status[idx] = false; // deffered
        cases[idx] = 4;  // path product = 0 for all hhubs
        // if (propagate_changed_down[src_scc] || propagate_changed_up[dest_scc]){
        // share_hubInfo(MN_list, src, dest, propagate_changed_up, propagate_changed_down,hubsize);
        // }
        
    }

// IMPORTANT ISSUE
// ADD THE NEW EDGES TO THE GRAPH ????

}
// ******************************************************************************





// ******************************************************************************
void update_new_hub_info(MetaNode*& MN_list, int node, int*& new_hub_info, bool up, int hubsize){

    for(int h=0; h<hubsize; h++){
        if(up) //collect only -ve values
        {
            if(MN_list[node].Hub_info[h] < 0){
                new_hub_info[h] = MN_list[node].Hub_info[h]; //can we add just -1?
            } //end if 
        }// end if

        else //collect the +ve values
        {
            if(MN_list[node].Hub_info[h] > 0){
            new_hub_info[h] = MN_list[node].Hub_info[h]; //can we add just 1?
            } //end if 
        } //end else
    }//end for
}
// ******************************************************************************


// ******************************************************************************
int get_RootSCCID(MetaNode*& MN_list, int id){
    int final = id;
    while( final != MN_list[final].currentID)
    {
        final = MN_list[final].currentID;
    }
    return final;
}
// ******************************************************************************


// ******************************************************************************
/*void merge_me(Graph* g_meta, MetaNode*& MN_list, bool*& trimmed, int h, int*& Hubs, int hubsize, int src, int dest, bool*& visited){
    int ref_hub = Hubs[h]; // Metanode ID for the hub

    stack<int> dfsStack;
    int s;
    int finalID_from; 
    int finalID_to;
    dfsStack.push(src);
    while(!dfsStack.empty()){
        // printf("Procesing From Src: \n");
        s = dfsStack.top();
        dfsStack.pop();
        // printf("Node %d\n",s);
        //pop the node form stack, mark visited, 
        visited[s] = true;
        if (MN_list[s].is_hub){
            if (MN_list[s].h_idx < h){
                // copy from h to MN_list[s]
                finalID_to = get_RootSCCID(MN_list,s);
                finalID_from = get_RootSCCID(MN_list,ref_hub);
                MN_list[ref_hub].currentID = finalID_to;
                MN_list[ref_hub].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                    copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize);
                     #ifdef DEBUG
                     printf("(%d -> %d) 1 Copied to %d\n",src, dest, finalID_to);
                     #endif
                }// end if 
               }// end if found smaller hub 
            else{
                finalID_to = get_RootSCCID(MN_list,ref_hub);
                finalID_from = get_RootSCCID(MN_list,s);
                MN_list[s].currentID = finalID_to;
                MN_list[s].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                    copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize); 
                    #ifdef DEBUG
                    printf("(%d -> %d) 2 Copied to %d\n",src, dest, finalID_to);  
                    #endif           
                }//end if copy same hub_idx
            } // end else bigger hu
        }
        //if not hub
        else{
            // if not hub, mark hub as true, update the root
            finalID_to = get_RootSCCID(MN_list,ref_hub);
            MN_list[s].is_hub = true;
            MN_list[s].currentID = finalID_to;
            MN_list[s].h_idx = MN_list[finalID_to].h_idx;
            copy_hubInfo(MN_list[s].Hub_info, MN_list[finalID_to].Hub_info, hubsize);
            #ifdef DEBUG
            printf("(%d -> %d) 3 Copied to %d\n",src, dest, finalID_to);
            #endif
        } // if not hub

        int_int parent_range = g_meta->get_parents(s);
        if(MN_list[s].currentID == Hubs[0]){parent_range={0,0};}// do not look for parent/children of the hub itself
        
        for (int p = parent_range.first; p<parent_range.second; p++){
            int current_p = g_meta->b_col_idx[p];
            //if current parent not visited, and goes to hub, or is hub  || (MN_list[current_p].Hub_info[h] == 5)
            if ((!trimmed[current_p]) && (!visited[current_p]) && ((MN_list[current_p].Hub_info[h] == 1) || (MN_list[current_p].Hub_info[h] == 5)))
            {
                dfsStack.push(current_p);
            } //end if
            //if hub found do not push but process itself
        }
    }

    dfsStack.push(dest);
    while(!dfsStack.empty()){
        s = dfsStack.top();
        dfsStack.pop();
        visited[s] = true;
        if (MN_list[s].is_hub){
            if (MN_list[s].h_idx < h){
                // copy from h to MN_list[s]
                finalID_to = get_RootSCCID(MN_list,s);
                finalID_from = get_RootSCCID(MN_list,ref_hub);
                MN_list[ref_hub].currentID = finalID_to;
                MN_list[ref_hub].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                  copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize);  
                    #ifdef DEBUG
                     printf("(%d -> %d) 4 Copied to %d\n",src, dest, finalID_to);
                     #endif
               }
             }// end if found smaller hub 
            else{
                finalID_to = get_RootSCCID(MN_list,ref_hub);
                finalID_from = get_RootSCCID(MN_list,s);
                MN_list[s].currentID = finalID_to;
                MN_list[s].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                    copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize);   
                    #ifdef DEBUG
                    printf("(%d -> %d) 5 Copied to %d\n",src, dest, finalID_to);
                    #endif
                }
                
            } // end else bigger hub
        }
        //if not hub
        else{
            // if not hub, mark hub as true, update the root
            finalID_to = get_RootSCCID(MN_list,ref_hub);
            MN_list[s].is_hub = true;
            MN_list[s].currentID = finalID_to;
            MN_list[s].h_idx = MN_list[finalID_to].h_idx;
            copy_hubInfo(MN_list[s].Hub_info, MN_list[finalID_to].Hub_info, hubsize);
            #ifdef DEBUG
             printf("(%d -> %d) 6 Copied to %d\n",src, dest, finalID_to);
             #endif
        } // if not hub

        int_int children_range = g_meta->get_children(s);
        if(MN_list[s].currentID == Hubs[0]){children_range={0,0};}// do not look for parent/children of the hub itself
        
        for (int c = children_range.first; c<children_range.second; c++){
            int current_c = g_meta->f_col_idx[c];
            //if current parent not visited, and goes to hub, or is hub  || (MN_list[current_p].Hub_info[h] == 5)
            if ((!trimmed[current_c]) && (!visited[current_c]) && ((MN_list[current_c].Hub_info[h] == -1) || (MN_list[current_c].Hub_info[h] == 5)))
            {
                dfsStack.push(current_c);
            } //end if
            //if hub found do not push but process itself
        }

    }//end second while loop 

    } //end merge me function

// ******************************************************************************

// ******************************************************************************
void update_scc_id(MetaNode*& MN_list, int id){
    int final = id;
    while( final != MN_list[final].currentID)
    {
        final = MN_list[final].currentID;
    }
    MN_list[id].currentID = final;
}
// ****************************************************************************** 

// ****************************************************************************** 
bool apply_change(int*& from, int*& to, int hubsize, int direction){
    bool changed = false; // initialize return value as false, if any change is applied it will mark true, so the propagation continues
    // copy only the direction values
        for (int i = 0; i<hubsize; i++){
            
            //if dir = +1
            if (direction == 1){
                //propagate +1 values
                //if from[i] is hub/downstream of hub
                if (from[i] > 1 || from[i] == 5){
                    // downstream propagation reaches upstream/hub
                    if (to[i] == -1 || to[i] == 5){
                        to[i] = 5;
                        from[i] = 5;
                        changed = true;
                        // printf("Found SCC during propagation.\n");
                    }//end if
                    else if (to[i] != 1){
                        to[i] = 1; 
                        changed = true;
                    } //end else
                }//end if from is +ve
            }

            if (direction == -1){
                //propagate -1 values
                //if from[i] is hub/upstream of hub
                if (from[i] == -1 || from[i] == 5){
                    if(to[i] == 1 || to[i] == 5){
                        to[i] = 5;
                        from[i] = 5;
                        changed = true;
                        // printf("Found SCC during propagation.\n");
                    }//end if
                    else if(to[i] != -1){
                        to[i] = -1;
                        changed = true;
                    }//end else
  
                } // end if from is -ve
            }

            // //todo
            // //if from[i] = 1 and to[i] = -1/5 (mark them as merged???)
            // //if from  = hub or downstream of hub && to = hub or upstream of hub
            // if( ((from[i] == 1 || from[i] == 5) && (to[i] == 5 || to[i] == -1))) {
            //     printf("MUST MERGE THE SCC\n");   
                
            // }

            // else if((from[i] == direction) && (to[i] == 0)){
            //     to[i] = direction;
            //     changed = true;
            // } //end if
        }
        return changed;
}//end function
// ****************************************************************************** 


// ****************************************************************************** 
void propagate_inserts(Graph* g_meta,vector<int_int>* inserts, bool*& insert_status, MetaNode*& MN_list, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& p_up, bool*& p_down, bool*& trimmed, int hubsize, int p ){
        #pragma omp parallel for num_threads(p) schedule(guided)
        for (int ins = 0; ins<inserts->size(); ins++){
            if(!insert_status[ins]){
                int src = inserts->at(ins).first;
                int dest = inserts->at(ins).second;
                
            }
        }
    
    
    int num_nodes = g_meta->node_count;
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int node_id = 0; node_id<num_nodes; node_id++){
        int scc_id; // look at the scc_id of the node instead of own id
        bool updated; // boolean value for continuing the propagation
        //if the node_id is changed and node_id is not trimmed
        if(!trimmed[node_id] && propagate_changed_up[node_id]){
            // asynchronous way????
            int_int p_range = g_meta->get_parents(node_id);
            for (int p = p_range.first; p<p_range.second; p++){
                int parent_id = g_meta->b_col_idx[p];
                //if parent is not trimmed, copy the changes to the parent(SCC_ID of parent)
                if(!trimmed[parent_id]){
                    scc_id = MN_list[parent_id].currentID; // route the changed ID to the current ID of scc
                    updated = apply_change(MN_list[node_id].Hub_info, MN_list[scc_id].Hub_info, hubsize, -1); //-1 means parents
                    // if (updated){propagate_changed_up[scc_id] = true;} //mark propagate_changed_up 
                    if (updated){p_up[scc_id] = true;} //mark propagate_changed_up 
                }
            }//end for all parents
        }//end if proppagate up

        //if the node_id is changed and node_id is not trimmed    
        if(!trimmed[node_id] && propagate_changed_down[node_id]){
            int_int c_range = g_meta->get_children(node_id);
            for(int c = c_range.first; c<c_range.second; c++){
                int child_id = g_meta->f_col_idx[c];
                //if child is not trimmed, copy the changes to the child(SCC_ID of child)
                if(!trimmed[child_id]){
                    scc_id = MN_list[child_id].currentID; //route the changed ID to the current ID of scc
                    updated = apply_change(MN_list[node_id].Hub_info, MN_list[scc_id].Hub_info, hubsize, 1); //1 means the children update
                    // if (updated){propagate_changed_down[scc_id] = true;} //mark propagate_changed_down for further propagation
                     if (updated){p_down[scc_id] = true;} //mark propagate_changed_down for further propagation
       
                }
            }
        }//end if propagate changed down
        // propagate_changed_up[node_id] = false; //mark false as already propagated
        // propagate_changed_down[node_id] = false; 
    }//end for all nodes
    //TODO pointer optimization
    // delete[] propagate_changed_up;
    // delete[] propagate_changed_down;

    // propagate_changed_up = p_up;
    // propagate_changed_down = p_down;
    // #pragma omp parallel for num_threads(p) schedule(guided)
    // for (int node_id = 0; node_id<num_nodes; node_id++){
    //     propagate_changed_up[node_id] = p_up[node_id];
    //     propagate_changed_down[node_id] = p_down[node_id];
    // }//end parallel for

    // delete[] p_down;
    // delete[] p_up;

}//end function

// ****************************************************************************** 
void propagate_all(Graph* g_meta, MetaNode*& MN_list, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& p_up, bool*& p_down, bool*& trimmed, int hubsize, int p ){
    int num_nodes = g_meta->node_count;
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int node_id = 0; node_id<num_nodes; node_id++){
        int scc_id; // look at the scc_id of the node instead of own id
        bool updated; // boolean value for continuing the propagation
        //if the node_id is changed and node_id is not trimmed
        if(!trimmed[node_id] && propagate_changed_up[node_id]){
            // asynchronous way????
            int_int p_range = g_meta->get_parents(node_id);
            for (int p = p_range.first; p<p_range.second; p++){
                int parent_id = g_meta->b_col_idx[p];
                //if parent is not trimmed, copy the changes to the parent(SCC_ID of parent)
                if(!trimmed[parent_id]){
                    scc_id = MN_list[parent_id].currentID; // route the changed ID to the current ID of scc
                    updated = apply_change(MN_list[node_id].Hub_info, MN_list[scc_id].Hub_info, hubsize, -1); //-1 means parents
                    // if (updated){propagate_changed_up[scc_id] = true;} //mark propagate_changed_up 
                    if (updated){p_up[scc_id] = true;} //mark propagate_changed_up 
                }
            }//end for all parents
        }//end if proppagate up

        //if the node_id is changed and node_id is not trimmed    
        if(!trimmed[node_id] && propagate_changed_down[node_id]){
            int_int c_range = g_meta->get_children(node_id);
            for(int c = c_range.first; c<c_range.second; c++){
                int child_id = g_meta->f_col_idx[c];
                //if child is not trimmed, copy the changes to the child(SCC_ID of child)
                if(!trimmed[child_id]){
                    scc_id = MN_list[child_id].currentID; //route the changed ID to the current ID of scc
                    updated = apply_change(MN_list[node_id].Hub_info, MN_list[scc_id].Hub_info, hubsize, 1); //1 means the children update
                    // if (updated){propagate_changed_down[scc_id] = true;} //mark propagate_changed_down for further propagation
                     if (updated){p_down[scc_id] = true;} //mark propagate_changed_down for further propagation
       
                }
            }
        }//end if propagate changed down
        // propagate_changed_up[node_id] = false; //mark false as already propagated
        // propagate_changed_down[node_id] = false; 
    }//end for all nodes
    //TODO pointer optimization
    // delete[] propagate_changed_up;
    // delete[] propagate_changed_down;

    propagate_changed_up = p_up;
    propagate_changed_down = p_down;
    // #pragma omp parallel for num_threads(p) schedule(guided)
    // for (int node_id = 0; node_id<num_nodes; node_id++){
    //     propagate_changed_up[node_id] = p_up[node_id];
    //     propagate_changed_down[node_id] = p_down[node_id];
    // }//end parallel for

    // delete[] p_down;
    // delete[] p_up;

}//end function
// ****************************************************************************** 


// ******************************************************************************

// ******************************************************************************

int count_true(bool*& status, int size, int p){
    int count=0;
    #pragma omp parallel for num_threads(p) schedule(guided) reduction(+:count)
    for (int i = 0; i<size; i++){
        if(status[i]){
            count++;
        }
    }
    return count;
}
// ******************************************************************************
// Case = 0 for Do Not Create SCC(Same Src & dest or already existed path)
    // Case = 1 for Created SCC(Determined Path)
    // Case = 2 for Not Created Yet found a common hub (Traversal Incomplete Group)
    // Case = 4 Not created yet due to all zeros for multiplication

    // action list
    //  1 SCC can be created with respect to hub i
    //  0 SCC will not be created due to a hub same direction hub i
    //  -1 same side of a hub, if not changed until last, then do BFS to the hub
    //  -5 SCC will be created Hub to Ancestor i or Src is the hub
    //  5 SCC will be created Successor to Hub i or Dest is the hub
    //  25 The node was already in SCC
    //  3 default not found the product on any hub values

void update_property(Graph* g, Graph* g_meta,int*& Hubs, vector<int_int>* inserts,vector<int_int>* deletes, vector<int>* SCCx, unordered_map<int,int>* sccMAP,MetaNode*& MN_list, bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& insert_status,bool*& delete_status, int hubsize, int p)
{
    double st = omp_get_wtime();
    int* cases = new int [inserts->size()]{-1};
    int* common_hub = new int[inserts->size()]{-1};
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int i=0; i<inserts->size(); i++){
        //handle only if the insert status is false
        if(!insert_status[i]){
            int src = inserts->at(i).first;
            int dest = inserts->at(i).second;
            int_int action = action_from_hub_values(MN_list,src,dest,Hubs,hubsize);
            handle_action_initial(MN_list, Hubs, hubsize, i, action, insert_status, cases, common_hub);
        }
    }

    printf("\n Time determining the actions : %f \n", (float)(omp_get_wtime()-st));

    // // Collect the upstream and downstream for each hubs
    for (int h = 0; h < hubsize; h++)
    {
        bool* visited = new bool[g_meta->node_count]{false};
            #pragma omp parallel for num_threads(p) schedule(guided)
            for (int ins = 0; ins<inserts->size(); ins++){
                if (common_hub[ins] == h && cases[ins] == 1){
                   merge_me(g_meta, MN_list,trimmed, h, Hubs, hubsize, inserts->at(ins).first, inserts->at(ins).second, visited);
                    cases[ins] = 3; //not used currently
                }// end common hub processing
            }//end parallel for loop
        delete[] visited;
    }
     printf("\n Time for merging : %f \n", (float)(omp_get_wtime()-st));
       printf("\n");

}
// ******************************************************************************
void update_inserts(Graph* g, Graph* g_meta,int*& Hubs, vector<int_int>* inserts,vector<int_int>* deletes, vector<int>* SCCx, unordered_map<int,int>* sccMAP,MetaNode*& MN_list, bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& p_up, bool*& p_down, bool*& insert_status,bool*& delete_status, int hubsize, int p)
{
    // Step 1: Perform Initial Find action/ handle action and merge
    double st = omp_get_wtime();
    int* cases = new int [inserts->size()]{-1};
    int* common_hub = new int[inserts->size()]{-1};
    bool* new_completed =  new bool[inserts->size()]{false};

    int itr = 0;
    while(itr <2){
        itr++;
        #ifdef DEBUG
         printf("Iteration: %d \n", itr);
        print_status(insert_status, inserts);
        print_meta_network(g_meta, MN_list, g_meta->node_count, Hubs, hubsize, trimmed);
         #endif
        //step 1 propagate
        propagate_all(g_meta, MN_list, propagate_changed_up, propagate_changed_down, p_up, p_down, trimmed, hubsize, p );
        //step 2 find and handle action
        #pragma omp parallel for num_threads(p) schedule(guided)
        for (int i=0; i<inserts->size(); i++){
            if(!insert_status[i]){ // check if the insert status is false
                int src = inserts->at(i).first;
                int dest = inserts->at(i).second;
                    #ifdef DEBUG
                         printf("Iteration: %d, Unhandled insert (%d->%d)\n", itr, src, dest);
                    #endif
                
                int_int action = action_from_hub(MN_list,src,dest,Hubs,hubsize);
                    #ifdef DEBUG
                printf("Iteration: %d, Insert(%d->%d),  Determined action (%d : %d)\n", itr, src, dest, action.first, action.second);
                    #endif


                handle_action(MN_list, Hubs, hubsize, i,src,dest, action, insert_status, cases, common_hub, propagate_changed_up, propagate_changed_down, itr);
                
            }
        }
        //step 3 merge
         // Merge the inserts created
        for (int h = 0; h < hubsize; h++)
        {
            bool* visited = new bool[g_meta->node_count]{false};
                #pragma omp parallel for num_threads(p) schedule(guided)
                for (int ins = 0; ins<inserts->size(); ins++){
                    
                    if (insert_status[ins] && common_hub[ins] == h && cases[ins] == 1){
                    // if (common_hub[ins] == h && cases[ins] == 1){
                    merge_me(g_meta, MN_list,trimmed, h, Hubs, hubsize, inserts->at(ins).first, inserts->at(ins).second, visited);
                    cases[ins] = 3; //not used currently
                    }// end common hub processing
                }//end parallel for loop
            delete[] visited;
        }//end for all hubs during insertion



    }
    // Step


    // print_status(insert_status, inserts);
    // print_meta_network(g_meta, MN_list, g_meta->node_count, Hubs, hubsize, trimmed);
    /*
    int itr = 0;
    // printf("Iteration %d before while loop\n", itr);
    while (itr<2){

        // printf("\n\n\n\n\n\n ITERATION %d STARTED \n\n\n\n\n\n", itr);
        // // if(itr!=0){
        // //         // printf("Iteration %d for propagating\n", itr);
        // //     // do not propagate in first iteration
        // //     // propagate_all(g_meta, MN_list, propagate_changed_up, propagate_changed_down, trimmed, hubsize, p);
        // // }
        propagate_all(g_meta, MN_list, propagate_changed_up, propagate_changed_down, p_up, p_down, trimmed, hubsize, p );
        //find and handle action
        #pragma omp parallel for num_threads(p) schedule(guided)
        for (int i=0; i<inserts->size(); i++){
            int src = inserts->at(i).first;
            int dest = inserts->at(i).second;
        // if(trimmed[src] || trimmed[dest]){
        //     insert_status[i] = true; // mark true for the inserted
        // }
        //handle only if the insert status is false
            if(!insert_status[i]){
                int_int action = action_from_hub(MN_list,src,dest,Hubs,hubsize);
                handle_action(MN_list, Hubs, hubsize, i,src,dest, action, insert_status, cases, common_hub, propagate_changed_up, propagate_changed_down, itr);
            }//end if not handled inserts
        }//end for all inserts
        // printf("\n\n All actions Determined\n\n");
            // printf("Iteration %d before merge \n", itr);
        // Merge the inserts created
        for (int h = 0; h < hubsize; h++)
        {
            bool* visited = new bool[g_meta->node_count]{false};
                #pragma omp parallel for num_threads(p) schedule(guided)
                for (int ins = 0; ins<inserts->size(); ins++){
                    
                    if (insert_status[ins] && common_hub[ins] == h && cases[ins] == 1){
                    // if (common_hub[ins] == h && cases[ins] == 1){
                    merge_me(g_meta, MN_list,trimmed, h, Hubs, hubsize, inserts->at(ins).first, inserts->at(ins).second, visited);
                    cases[ins] = 3; //not used currently
                    }// end common hub processing
                }//end parallel for loop
            delete[] visited;
        }//end for all hubs during insertion

    // printf("Iteration %d: Count of  completed inserts after update propeerty: %d\n",itr, count_true(insert_status,inserts->size(),p));
    // printf("Iteration %d: Count of  completed deletes after update property: %d\n",itr,count_true(delete_status,deletes->size(),p));
    print_status(insert_status, inserts);
    print_meta_network(g_meta, MN_list, g_meta->node_count, Hubs, hubsize, trimmed);
    printf("\n\n\n\n\n\n ITERATION %d COMPLETED \n\n\n\n\n\n", itr);
    
        itr++;

    }//end while iteration 
   */
    // for (int ins = 0; ins<inserts->size(); ins++){
    //     printf("Insert (%d -> %d) Status %d, Case %d, Common Hub %d\n", inserts->at(ins).first, inserts->at(ins).second,insert_status[ins], cases[ins],common_hub[ins]);
    // }
  
    // Step 2: Perform Iteration k times

    // Step 3: Perform BFS for the unhandled cases.

//}

// ******************************************************************************




// ******************************************************************************
void update_deletes(Graph* g, Graph* g_meta,int*& Hubs, vector<int_int>* inserts,vector<int_int>* deletes, vector<int>* SCCx, unordered_map<int,int>* sccMAP,MetaNode*& MN_list, bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& insert_status,bool*& delete_status, int hubsize, int p)
{
    // Step 1: Perform Initial Find action/ handle action and merge
    double st = omp_get_wtime();
    // int* cases = new int [inserts->size()]{-1};
    // int* common_hub = new int[inserts->size()]{-1};

// for all metanodes
// if metanode is not trimmed 
// for all deletes:
#pragma omp parallel for num_threads(p) schedule(dynamic)
    for (int del = 0; del<deletes->size(); del++){
        for (int metanode = 0; metanode < g_meta->node_count; metanode++){
            if (!trimmed[metanode]){
            // if (true){ //TODO change true to if delete in metanode
            //   // Check 2 hop path
            //   // fwd set = (forward of deletes.first)
            //   // bwd set = (backward of deletes.sec)
            //   // fwd_set.intersection(bwd_set) > 0
            //   // status = completed
            //   // otherwise = differ
            // }
            }
    }
}


    

}

// ******************************************************************************






// ******************************************************************************
// void print_meta_network(Graph* g,MetaNode*& MN_List, int graph_size,int*& Hubs, int hubsize, bool*& trimmed)
// {
//     for (int i = 0; i<graph_size; i++)
//     {
//         color("yellow");
//         printf("\nNode: %d ",i);
//                if(trimmed[i]){
//             color("red");
//             printf(" TRIMMED!!!\n");
//             color("reset");
//         }
//         if(!trimmed[i]){
//             color("green");
//             printf("  NOT TRIMMED!!!\n");
//             color("reset");
//         }
//         MN_List[i].is_hub? printf(" Hub %d\n",MN_List[i].h_idx):printf("Not Hub\n");
//         printf("Current SCC ID: %d \n", MN_List[i].currentID);

//         printf("Children: ");
//         int_int c_range = g->get_children(i);
//         for (int c = c_range.first; c < c_range.second; c++)
//         {
//             printf("%d, ", g->f_col_idx[c]);
//         }
//         printf("\n Parents: ");
//         int_int p_range = g->get_parents(i);
//         for (int p = p_range.first; p < p_range.second; p++)
//         {
//             printf("%d, ", g->b_col_idx[p]);
//         }
//         printf("\n Hub Info : ");
//         for (int h = 0; h<hubsize; h++)
//         {
//             printf(" (%d->%d) \n", Hubs[h],MN_List[i].Hub_info[h]);
//         }
        

//     }
// }
// ******************************************************************************
#endif
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
// ******************************************************************************

// ******************************************************************************
void create_graph(int* src, int* dest, int* wt_list, int n, int m, Graph* graph){
    // if (wt_list->size() == 0){wt_list->resize(int(edge_list->size()),1);}
    //resize both col_idx
    graph->f_col_idx = new int[m]();
    graph->b_col_idx = new int[m]();
    graph->edge_wt = new int[m]();
    graph->f_row_ptr = new int[n+1]();
    graph->b_row_ptr = new int[n+1]();
    graph->node_count = n;
    graph->edge_count = m;

    graph->in_deg = new int[n]{0};
    graph->out_deg = new int[n]{0};
    graph->is_hub = new bool[n]{0};
    

  // printf("I am here in create graph first\n");
    // parallel in here did not work???
    for (int i = 0; i<m; i++){
   if(src[i]==-1||dest[i]==-1){continue;} 
    //printf("%d %d \n", src[i], dest[i]);
	graph->in_deg[dest[i]]++;
        graph->out_deg[src[i]]++;
        graph->f_row_ptr[src[i]+1]++;
        graph->b_row_ptr[dest[i]+1]++;
    }

    //printf("I am here in create graph \n");
    //if(graph->in_deg>30 &&graph->out_deg> 30) {graph->is_hub=true;}
    // for (const auto& edge : *edge_list){
    //     graph->f_row_ptr[edge.first + 1]++;
    //     graph->b_row_ptr[edge.second + 1]++;
    // }
    //finding the cumulative sum for starting vertex
    for (int i = 1; i<= n; i++){
        graph->f_row_ptr[i] += graph->f_row_ptr[i-1];
        graph->b_row_ptr[i] += graph->b_row_ptr[i-1];
	if(graph->in_deg[i]>30 && graph->out_deg[i]>30)
		graph->is_hub[i]=true;
    }
    
    // vector<int> temp_next_outgoing_idx(n,0);
    // vector<int> temp_next_incoming_idx(n,0);
    int* temp_next_outgoing_idx = new int[n]();
    int* temp_next_incoming_idx = new int[n]();

    for (int i = 0; i<m; i++){
   if(src[i]==-1||dest[i]==-1){continue;} 
        int oIdx = graph->f_row_ptr[src[i]] + temp_next_outgoing_idx[src[i]];
        int iIdx = graph->b_row_ptr[dest[i]] + temp_next_incoming_idx[dest[i]];

        graph->f_col_idx[oIdx] = dest[i];
        graph->edge_wt[oIdx] = wt_list[i];
        graph->b_col_idx[iIdx] = src[i];

        temp_next_outgoing_idx[src[i]]++;
        temp_next_incoming_idx[dest[i]]++;
    }
    
}
// ******************************************************************************

// ******************************************************************************
void find_hubs(Graph* g, Graph* mg, MetaNode*& MN_list,vector<int>* SCCx, bool*& propagate_changed_up, bool*& propagate_changed_down, int*& Hubs, int n, int N, int hubsize, int p){
    int count=0;
    vector<int_int> hub_candidates(N); //TODO change to array
    int non_zero = 0;
    // bool* trim = new bool[N]; //boolean to indicate if node is source or sink 
    int* temp_hub = new int[hubsize]{0}; 


//Identify Candidates for Hubs
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int id =0; id<N; ++id)
    {
	int th_in_deg=10;
        int th_out_deg=10;	
        int pr=0; //product of in and out degree
        int v = 0; //count for vertices that are not src or sink
        int deg_out = mg->out_deg[id];
        int deg_in = mg->in_deg[id];
        // if ((mg->num_parents(id) >0) && (mg->num_childrens(id)>0)) //TODO can we get value during read and then store in array
        if ((deg_in >th_in_deg) &&  (deg_out>th_out_deg)) //TODO can we get value during read and then store in array
        {
            // pr = mg->num_parents(id) * mg->num_childrens(id);
            //pr = deg_out * deg_in;
            //MN_list[id].trimmed = false;
            // trim[id] = false;
    #pragma omp atomic capture
            v=non_zero++; 
            //hub_candidates.at(v).first = pr;
            //hub_candidates.at(v).second = id;
        }
        // else{
        //     trim[id] = true;
        // }
    }//end of for
    
printf("XX %d : \n", non_zero);
    //Erase non-zeros to keep only viable candidates, and then sort by product of in*out
   //hub_candidates.erase(hub_candidates.begin() + non_zero, hub_candidates.end());
   //sort(hub_candidates.begin(), hub_candidates.end());
   //  sort(hub_candidates.begin(), hub_candidates.end(), [](const int_int &a, const int_int &b)
   //      { return a.first > b.first; });

    /*#pragma omp parallel for num_threads(p) schedule(guided) //we may not need this parallel for
    for (int h=0; h<hubsize; ++h)
    {
        temp_hub[h] = hub_candidates.at(h).second; //TODO check if we can replace temp_hub with Hubs
        MN_list[hub_candidates.at(h).second].is_hub = true;
        MN_list[hub_candidates.at(h).second].h_idx = h;
        MN_list[hub_candidates.at(h).second].Hub_info[h] = 5;
        propagate_changed_up[hub_candidates.at(h).second] = true;
        propagate_changed_down[hub_candidates.at(h).second] = true;
    }

    // trimmed = trim;
    Hubs = temp_hub;
    hub_candidates.clear();
*/
}
// ******************************************************************************


 // ******************************************************************************
void clean_inserts(vector<int_int>* inserts, bool*& insert_status, bool*& trimmed, int p){
    //loop through inserts and mark true for insert status
    #pragma omp parallel for num_threads(p)
    for(int ins = 0; ins<inserts->size(); ins++){
        int src = inserts->at(ins).first;
        int dest = inserts->at(ins).second;

        // check within scc
        if(src == dest){
            insert_status[ins] = true;
        }
         // check if any trimmed
        if(trimmed[src] || trimmed[dest]){
            insert_status[ins] = true;
        }

    }
}
 // ******************************************************************************


// // ******************************************************************************
// void write_levels_inserts(Graph* g_meta, vector<int_int>* inserts, bool*& insert_status, MetaNode*& MN_list, int*& Hubs,bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, int hubsize, int p){
//     #pragma omp parallel for num_threads(p) schedule(dynamic)
//     for (int ins=0; ins<inserts->size(); ins++){
//         if(!insert_status[ins]){

//         }
//     }

//     for (int h=0; h<hubsize; h++){
//         // int current_hub = Hubs[h];
//         // MN_list[current_hub].Hub_info[h] = 5; //mark itself as 5 to show it is a hub

// //Get start and end indices of children and parent
//         int_int range_children = g_meta->get_children(current_hub);
//         int_int range_parents = g_meta->get_parents(current_hub);

//         // look for the children of the hub and mark them +1 if untrimmed
//         // #pragma omp parallel for num_threads(p) schedule(dynamic)
//         for (int out = range_children.first; out<range_children.second; out++){
//             int current_neigh = g_meta->f_col_idx[out];      
//             if (!trimmed[current_neigh]){
//                 MN_list[current_neigh].Hub_info[h] = 1;
//                 propagate_changed_down[current_neigh] = true; //mark that 1 to be propagated to children
//             }
//         }
//          // look for tht parents of the hub and mark them -1 if untrimmed
//         //  #pragma omp parallel for num_threads(p) schedule(dynamic)
//         for (int in = range_parents.first; in<range_parents.second; in++){
//             int current_neigh = g_meta->b_col_idx[in];      
//             if (!trimmed[current_neigh]){
//                 MN_list[current_neigh].Hub_info[h] = -1;
//                 propagate_changed_up[current_neigh] = true; //mark that -1 to be propagated to parent
//             }
//         }
//     }
// }
// // ******************************************************************************

// ******************************************************************************

// ******************************************************************************


// // ******************************************************************************
void write_levels_new(Graph* g_meta,MetaNode*& MN_list, int N, int p){

	int start, end;
	int child, parent;
	int index;
    //    printf("%d :: \n", sizeof(MN_list));    
    for (int mn=0; mn<N; mn++){

	if(!MN_list[mn].is_hub){continue;}

	//int mn=69;
	 index=MN_list[mn].h_idx;
       	//printf("******%d --\n",mn);    
	//Check if children are hubs
	    //Mark with 1 for upstream
	  start = g_meta->f_row_ptr[mn];
	  end = g_meta->f_row_ptr[mn+1];

        //  printf("%d...\n",end-start);	
#pragma omp parallel for num_threads(p) schedule(guided)
	  for (int out = start; out<end; out++){
            int child = g_meta->f_col_idx[out];
          
		if(MN_list[child].trimmed){continue;}

               MN_list[child].Hub_info[index]=1;
              
	   }//end of for children


       	    //Check if parents are hubs
	    //Mark with -1 for upstream
	  start = g_meta->b_row_ptr[mn];
	  end = g_meta->b_row_ptr[mn+1];
        //  printf("%d...\n",end-start);	
	#pragma omp parallel for num_threads(p) schedule(guided)
	  for (int out = start; out<end; out++){
            int parent = g_meta->f_col_idx[out];
          
		if(MN_list[parent].trimmed){continue;}
               MN_list[parent].Hub_info[index]=-1;
              
	   }//end of for parent

//	   printf("%d: Child:%d Parent:%d \n", mn,children,parents);
        }//end of for meta nodes
    }
// // ******************************************************************************

// ******************************************************************************
// *** ISSUE Sync ****

// // ******************************************************************************
void write_levels(Graph* g_meta,MetaNode*& MN_list, int*& Hubs,bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, int hubsize, int p){
    #pragma omp parallel for num_threads(p) schedule(dynamic)
    for (int h=0; h<hubsize; h++){
        int current_hub = Hubs[h];
        MN_list[current_hub].Hub_info[h] = 5; //mark itself as 5 to show it is a hub

//Get start and end indices of children and parent
        int_int range_children = g_meta->get_children(current_hub);
        int_int range_parents = g_meta->get_parents(current_hub);

        // look for the children of the hub and mark them +1 if untrimmed
        // #pragma omp parallel for num_threads(p) schedule(dynamic)
        for (int out = range_children.first; out<range_children.second; out++){
            int current_neigh = g_meta->f_col_idx[out];      
            if (!trimmed[current_neigh]){
                MN_list[current_neigh].Hub_info[h] = 1;
                propagate_changed_down[current_neigh] = true; //mark that 1 to be propagated to children
            }
        }
         // look for tht parents of the hub and mark them -1 if untrimmed
        //  #pragma omp parallel for num_threads(p) schedule(dynamic)
        for (int in = range_parents.first; in<range_parents.second; in++){
            int current_neigh = g_meta->b_col_idx[in];      
            if (!trimmed[current_neigh]){
                MN_list[current_neigh].Hub_info[h] = -1;
                propagate_changed_up[current_neigh] = true; //mark that -1 to be propagated to parent
            }
        }
    }
}
// // ******************************************************************************

// ******************************************************************************
// *** ISSUE Sync ****
/* 
inserts:
    if same SCC; mark done
    if Different SCC:
        mark:  not done
        mark inserted_from on meta node = true and false for both src and dest
        untrim if trimmed 

deletes:
    if same SCC; 



*/

void convert_changes(vector<int_int>* inserts,vector<int_int>* deletes, vector<int>* SCCx, int p){
    int count_i=0;
    int count_d = 0;

    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int i = 0; i<inserts->size(); i++)
    {
        //Obtain src and dest of insert
    //int src = inserts->at(i).first;
    //int dest = inserts->at(i).second;


          //Convert to corresponding MetaNode or SCC
    int m_src = SCCx->at(inserts->at(i).first);
    int m_dest = SCCx->at(inserts->at(i).second);

     //  if (m_src == m_dest){
        //insert_status[i] = true;} //inserted within same SCC

 /* else{
       if(trimmed[m_src]){
    //        // if(g_meta->num_childrens(m_src) == 0)
    //         {
             trimmed[m_src] = false;
    //         }
         }

     if(trimmed[m_dest]){
    //        // if(g_meta->num_parents(m_dest) == 0) 
    //         {
             trimmed[m_dest] = false;
    //         }
         }
        }*/
    // RENAME THEM
    //inserts->at(i).first = m_src;
    //inserts->at(i).second = m_dest;


    // if(trimmed[m_dest] && (g_meta->num_parents(m_dest) == 0)){
    //         trimmed[m_dest] = false;
    //     }

    //Mark MetaNode trim=false if particiapting in insert
    // if(trimmed[m_src] && (g_meta->num_childrens(m_src) == 0)){
    //         trimmed[m_src] = false;
    //     }

    // if(trimmed[m_dest] && (g_meta->num_parents(m_dest) == 0)){
    //         trimmed[m_dest] = false;
    //     }

    // if (m_src == m_dest){
    //     insert_status[i] = true;} //inserted within same SCC
    // else{
    //     // #pragma omp atomic
    //     //     count_i++;

    //     //different scc insertion
    //     // mark status false indicating to be procesased further
    //     //mark corresponding MetaNodes as src and dest of inserts
    //     //TODO check if this is required
    //     insert_status[i] = false;
    //     MN_list[m_src].inserted_from = true;
    //     MN_list[m_dest].inserted_to = true;

    //     //Mark MetaNode trim=false if particiapting in insert
    //     if(trimmed[m_src] && (g_meta->num_childrens(m_src) == 0)){
    //         trimmed[m_src] = false;
    //     }
    //     if(trimmed[m_dest] && (g_meta->num_parents(m_dest) == 0)){
    //         trimmed[m_dest] = false;
    //     }

    //     // if still trimmed then mark completed for the inserts
    //     // if(trimmed[m_src] || trimmed[m_dest]){
    //     //     // insert_status[i] = true; // mark true for the inserted
    //     // }
    //     }
    }//end of for


    // #pragma omp parallel for num_threads(p) schedule(dynamic)
    // for(int i = 0; i<deletes->size(); i++)
    // {
    //     int src = deletes->at(i).first;
    //     int dest =  deletes->at(i).second;

    //     int m_src = sccMAP->at(SCCx->at(src));
    //     int m_dest = sccMAP->at(SCCx->at(dest));

    //     //         // RENAME THEM
    //     deletes->at(i).first = m_src;
    //     deletes->at(i).second = m_dest;

    //     if (m_src == m_dest){ //within scc deletion
    //         //check key edge, if not key edge, delete and mark done for the delete
    //         int_int children_range = g->get_children(src);
    //         for (int c = children_range.first; c<children_range.second; c++){
    //             if (dest == g->f_col_idx[c]){
    //                 if (g->edge_wt[c] == -1) {
    //                     delete_status[i] = false;
    //                 //   #pragma omp atomic
    //                 //     count_d++;
    //                     } // key edge deletion mark false
                    
    //                 if (g->edge_wt[c] == 1) 
    //                 {
    //                     delete_status[i] = true; // delete completed because of non-key edge deletion
    //                     g->edge_wt[c] == 0;
    //                 }

    //                 if (g->edge_wt[c] == 0)  //duplicate deletion
    //                 {
    //                     delete_status[i] = true; // delete completed because of non-key edge deletion
    //                 }
    //             }
    //         }
    //     }
    //     //Inter SCC deletion
    //     // Weight processing
    //     if (m_src != m_dest)
    //     {
    //         delete_status[i] = true;
    //     //     int_int c_range = g_meta->get_children(m_src);
    //     //    for (int c = c_range.first; c<c_range.second; c++)
    //     //    {
    //     //        printf("Processing Child %d for src %d\n", g_meta->f_col_idx[c],src);
    //     //     if (m_dest == g_meta->f_col_idx[c]){

    //     //         g_meta->edge_wt[c]--; // can this be done????
    //     //     }
    //     //    }

    //    }
    // }
}
// ******************************************************************************


// ******************************************************************************
void copy_hubInfo(int*& from, int*& to, int hubsize){
    for (int i = 0; i< hubsize; i++){
        //
        if (from[i] == 5){to[i] = 5;}
        else if(to[i] == 0){
            to[i] = from[i];
        }
        else if(from[i]*to[i]<0){
            to[i] = 5;
            // printf("Changed SCC ID from CopyHubinfo\n");
        }
    }
}
// ******************************************************************************

// ******************************************************************************
int_int action_from_hub(MetaNode*& MN_list,int from,int to, int*& Hubs,int hubsize){
    int_int return_status;
    return_status.first = 3; //differ status
    return_status.second = -1; // reference hub

    int src = MN_list[from].currentID;
    int dest = MN_list[to].currentID;

    //already inside parallel region
    for (int i = 0; i<hubsize; i++){
        int id = MN_list[Hubs[i]].h_idx;
        int val = MN_list[src].Hub_info[id] * MN_list[dest].Hub_info[id];

        // printf("found prod value %d for hub %d\n",val, Hubs[i]);
        // // If src and dest are opposite side of a hub
        // // if src is 1 then SCC will be created
        // // SCC will not be created otherwise "Addition of direct path"
        // // CASE A
        if (val == -1){
            // color("red");
            // printf("found -1\n");
            // color("reset");
            if (MN_list[src].Hub_info[i] > 0)
            {
                // printf("(%d->%d) Returned as CREATED SCC with reference hub %d \n",from,to,Hubs[id]);
                return {1,i};
            }
            else
            {
                //SCC will not be createe as path already exixts 
                // src -> hub -> dest
                // printf("(%d->%d) Returned as NOT CREATED SCC with reference hub %d \n",from,to,Hubs[id]);
                return {0,i};
            }
        }

        //CASE B
        else if(val == 1)
        {
            // this means both src and dest are in same side of hub
            // for this if we search for dest to hub and find src ; then SCC will be created
            // if we do not find src in paths then scc will not be created
                    #ifdef DEBUG
                            printf("(%d,%d) FOUND COMMOPN HUB SAME SIDE\n", from, to, Hubs[id]);
                    #endif

            return_status = {-1,i};
            
            // return {-1,i};
        }

        //CASE C
        else if(val == -5)
        {
            if (src == Hubs[i]){
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC (hub->upstream) with reference hub %d \n",from,to,Hubs[id]);
                #endif
                return {-5,i};//SCC can be created hub to upstream
            }

            else{
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC (upstream ->HUB) with reference hub %d \n",from,to,Hubs[id]);
                #endif
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }

        //CASE D
        else if(val == 5)
        {
            if (dest==Hubs[i]){
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC (downstream->hub) with reference hub %d \n",from, to,Hubs[id]);
                #endif

                return {5,i};//SCC can be created down -> hub
            }
            else{
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC (hub ->down) with reference hub %d \n",from, to, Hubs[id]);
                #endif
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }    

        else if(val == 25)
        {
            #ifdef DEBUG
            printf("(%d->%d) Returned as SAME SCC",from,to);
            #endif

            return {0,i}; // returned will not be created, already in same scc
        }

    }

    #ifdef DEBUG
    printf(" (%d->%d) not created return code %d \n",from, to,return_status.first);
    #endif

    return return_status;

}
// ******************************************************************************

// ******************************************************************************
int_int action_from_hub_values(MetaNode*& MN_list,int from,int to, int*& Hubs,int hubsize){
    int_int return_status;
    return_status.first = 3; //differ status
    return_status.second = -1; // reference hub
    // color("red");
    // printf("\n\n Finding Action %d -> %d\n", from, to);
    // color("reset");
    int src = MN_list[from].currentID;
    int dest = MN_list[to].currentID;

    #ifdef DEBUG
    printf("I AM PROCESSING %d -> %d \n", src, dest);
    #endif 
    // NOTE: Further checking required because multiple hubs may be found with different return values. "Make precedence"
    // #pragma omp parallel for num_threads(p) schedule(guided)
    //already inside parallel region
    for (int i = 0; i<hubsize; i++){
        int val = MN_list[src].Hub_info[i] * MN_list[dest].Hub_info[i];
        // printf("found prod value %d for hub %d\n",val, Hubs[i]);
        // // If src and dest are opposite side of a hub
        // // if src is 1 then SCC will be created
        // // SCC will not be created otherwise "Addition of direct path"
        // // CASE A
        if (val == -1){
            // color("red");
            // printf("found -1\n");
            // color("reset");
            if (MN_list[src].Hub_info[i] > 0)
            {
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC with reference hub %d \n",from,to,Hubs[i]);
                #endif
                return {1,i};
            }
            else
            {
                //SCC will not be createe as path already exixts 
                // src -> hub -> dest
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC with reference hub %d \n",from,to,Hubs[i]);
                #endif
                return {0,i};
            }
        }

        //CASE B
        else if(val == 1)
        {
            // this means both src and dest are in same side of hub
            // for this if we search for dest to hub and find src ; then SCC will be created
            // if we do not find src in paths then scc will not be created
            #ifdef DEBUG
            printf("(%d,%d) FOUND COMMOPN HUB SAME SIDE\n", from, to, Hubs[i]);
            #endif
            return_status = {-1,i};
            
            // return {-1,i};
        }

        //CASE C
        else if(val == -5)
        {
            if (src == Hubs[i]){
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC (hub->upstream) with reference hub %d \n",from,to,Hubs[i]);
                #endif
                return {-5,i};//SCC can be created hub to upstream
            }
            else{
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC (upstream ->HUB) with reference hub %d \n",from,to,Hubs[i]);
                #endif
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }

        //CASE D
        else if(val == 5)
        {
            if (dest==Hubs[i]){
                #ifdef DEBUG
                printf("(%d->%d) Returned as CREATED SCC (downstream->hub) with reference hub %d \n",from, to,Hubs[i]);
                #endif
                return {5,i};//SCC can be created down -> hub
            }
            else{
                #ifdef DEBUG
                printf("(%d->%d) Returned as NOT CREATED SCC (hub ->down) with reference hub %d \n",from, to, Hubs[i]);
                #endif
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }    

        else if(val == 25)
        {
            #ifdef DEBUG
            printf("(%d->%d) Returned as SAME SCC",from,to);
            #endif
            return {0,i}; // returned will not be created, already in same scc
        }

    }

    #ifdef DEBUG
    printf(" (%d->%d) not created return code %d \n",from, to,return_status.first);
    #endif
    return return_status;

}
// ******************************************************************************

// ******************************************************************************
void handle_action_initial(MetaNode*& MN_list, int*& Hubs, int hubsize, int idx, int_int action,bool*& status,int*& cases, int*& common_hub){
   
   int todo = action.first;
   bool created = false;

   if (todo == 1) //SCC will be created wrt hub
   {
    int hub_idx = action.second;
    status[idx] = true; // mark completed
    cases[idx] = 1; // created SCC 
    common_hub[idx] = hub_idx;
    created = true;
   }

   else if (todo == 0) //SCC will not be created, path already exists
   {
    status[idx] = true; //completed
    cases[idx] = 0;  // Not created SCC, path already existed
   }

   else if (todo == -1)
   {
    status[idx] = false; // not resolved
    cases[idx] = 2; //found common hub
    common_hub[idx] = action.second;
   }

   // change -5 & 5 as 1
    else if (todo == -5 || todo == 5) // SCC will be created Hub to Ancestor or Successor to Hub
    {
        int hub_idx = action.second;
        status[idx] = true; // Completed insertion
        cases[idx] = 1;  // Created SCC
        common_hub[idx]= hub_idx;
        created = true;
    }

    else if (todo == 25) // Inside same scc after merged
    {
        status[idx] = true;
        cases[idx] = 1; // Created SCC
        common_hub[idx]= action.second;
        created = true;
    }

    else // Zero for all values
    {
        status[idx] = false; // deffered
        cases[idx] = 4;  // path product = 0 for all hhubs
    }

// IMPORTANT ISSUE
// ADD THE NEW EDGES TO THE GRAPH ????


//         // //add the parent and src values
//     if (!created){
//                 int src = inserts->at(change_idx).first;
//         int dest = inserts->at(change_idx).second;
// //TODO need to get rid of critical 
//         #pragma omp critical
//         {
//         XM->at(src).children.push_back(dest);
//         XM->at(dest).parents.push_back(src);
//         }


}
// ******************************************************************************
// for any inserts, if the case is not handled, they share the +ve, -ve info to each others, 

// ******************************************************************************
void share_hubInfo(MetaNode*& MN_list, int src, int dest, bool*& propagate_changed_up, bool*& propagate_changed_down, int hubsize){
    int src_root = MN_list[src].currentID;
    int dest_root = MN_list[dest].currentID;
    #ifdef DEBUG
    printf("Sharing Hub info between %d and %d\n", src_root, dest_root);
    print_Hub_info(MN_list, src_root, hubsize);
    print_Hub_info(MN_list, dest_root, hubsize);
    #endif
    //for all hubs
    for (int h =0; h<hubsize; h++){
        int s_h = MN_list[src_root].Hub_info[h];
        int d_h = MN_list[dest_root].Hub_info[h] ;
            #ifdef DEBUG
            printf("attempting sharing!!! %d \n", h);
            #endif
        //if sharing creates SCC 
        if((s_h == 1 || s_h == 5) && (d_h == 5 || d_h == -1)){
            //scc created 
            printf("Sharing is creating SCC\n");
            // if (MN_list[src_root].is_hub && MN_list[src_root].is_hub){
            //     if (MN_list[src_root].h_idx < MN_list[dest_root].h_idx){
            //         copy_hubInfo(MN_list[dest_root].Hub_info, MN_list[src_root].Hub_info, hubsize);
            //         printf("Edges (%d->%d) Roots:(%d -> %d) Copied to %d\n",src,dest, src_root, dest_root, src_root);
            //         MN_list[dest_root].currentID = src_root;
            //         MN_list[dest_root].h_idx = MN_list[src_root].h_idx;
            //         propagate_changed_up[src_root] = true;
            //         propagate_changed_down[src_root] = true;
            //         return;
            //     } // end if

            //     else if(MN_list[src_root].h_idx > MN_list[dest_root].h_idx){
            //         copy_hubInfo(MN_list[src_root].Hub_info, MN_list[dest_root].Hub_info, hubsize);
            //         printf("Edges (%d->%d) Roots:(%d -> %d Copied to %d\n",src,dest,src_root, dest_root,dest_root);
            //         MN_list[src_root].currentID = dest_root;
            //         MN_list[src_root].h_idx = MN_list[dest_root].h_idx;
            //         propagate_changed_up[dest_root] = true;
            //         propagate_changed_down[dest_root] = true;
            //         return;
            //     }//end else if
            // }//end if both hub
            //if only one is hub
        }//end if created scc during share

        //TODO is update on all the neighbors necessary? 
        if(d_h  == 0 && s_h > 0){
            #ifdef DEBUG
            printf("SHARED SUCCESSFULLY -> %d\n", h);
            #endif
            MN_list[dest_root].Hub_info[h] = 1;
            propagate_changed_down[dest_root] = true;
             }//end if copy 1 to destination

        if (s_h == 0 && d_h < 0){
            #ifdef DEBUG
            printf("SHARED SUCCESSFULLY <-\n", h);
            #endif
            MN_list[src_root].Hub_info[h] = -1;
            propagate_changed_up[src_root] = true;
            } // end if copy -1 to src

    }// end for loop
    #ifdef DEBUG
    print_Hub_info(MN_list, src_root, hubsize);
    print_Hub_info(MN_list, dest_root, hubsize);
    #endif
}
// ******************************************************************************


// ******************************************************************************
void handle_action(MetaNode*& MN_list, int*& Hubs, int hubsize, int idx, int src, int dest, int_int action, bool*& status, int*& cases, int*& common_hub, bool*& propagate_changed_up, bool*& propagate_changed_down, int itr){
   
   int todo = action.first;
   bool created = false;
   int src_scc = MN_list[src].currentID;
   int dest_scc = MN_list[dest].currentID;

   if (todo == 1) //SCC will be created wrt hub
   {
    int hub_idx = action.second;
    status[idx] = true; // mark completed
    cases[idx] = 1; // created SCC 
    common_hub[idx] = hub_idx;
   }

   else if (todo == 0) //SCC will not be created, path already exists
   {
    status[idx] = true; //completed
    cases[idx] = 0;  // Not created SCC, path already existed
    //Copy the hubinfo anyway?
   }

   else if (todo == -1) //if differed
   {
    status[idx] = false; // not resolved
    cases[idx] = 2; //found common hub
    common_hub[idx] = action.second;
    // if (propagate_changed_down[src_scc] || propagate_changed_up[dest_scc]){
    // share_hubInfo(MN_list, src, dest,propagate_changed_up, propagate_changed_down,hubsize);
    // }//end if itr
   } // end if todo -1

   // change -5 & 5 as 1
    else if (todo == -5 || todo == 5) // SCC will be created Hub to Ancestor or Successor to Hub
    {
        int hub_idx = action.second;
        status[idx] = true; // Completed insertion
        cases[idx] = 1;  // Created SCC
        common_hub[idx]= hub_idx;
    }

    else if (todo == 25) // Inside same scc after merged
    {
        status[idx] = true;
        cases[idx] = 1; // Created SCC
        common_hub[idx]= action.second;

    }

    else // Zero for all values
    {
        status[idx] = false; // deffered
        cases[idx] = 4;  // path product = 0 for all hhubs
        // if (propagate_changed_down[src_scc] || propagate_changed_up[dest_scc]){
        // share_hubInfo(MN_list, src, dest, propagate_changed_up, propagate_changed_down,hubsize);
        // }
        
    }

// IMPORTANT ISSUE
// ADD THE NEW EDGES TO THE GRAPH ????

}
// ******************************************************************************





// ******************************************************************************
void update_new_hub_info(MetaNode*& MN_list, int node, int*& new_hub_info, bool up, int hubsize){

    for(int h=0; h<hubsize; h++){
        if(up) //collect only -ve values
        {
            if(MN_list[node].Hub_info[h] < 0){
                new_hub_info[h] = MN_list[node].Hub_info[h]; //can we add just -1?
            } //end if 
        }// end if

        else //collect the +ve values
        {
            if(MN_list[node].Hub_info[h] > 0){
            new_hub_info[h] = MN_list[node].Hub_info[h]; //can we add just 1?
            } //end if 
        } //end else
    }//end for
}
// ******************************************************************************


// ******************************************************************************
int get_RootSCCID(MetaNode*& MN_list, int id){
    int final = id;
    while( final != MN_list[final].currentID)
    {
        final = MN_list[final].currentID;
    }
    return final;
}
// ******************************************************************************


// ******************************************************************************
/*void merge_me(Graph* g_meta, MetaNode*& MN_list, bool*& trimmed, int h, int*& Hubs, int hubsize, int src, int dest, bool*& visited){
    int ref_hub = Hubs[h]; // Metanode ID for the hub

    stack<int> dfsStack;
    int s;
    int finalID_from; 
    int finalID_to;
    dfsStack.push(src);
    while(!dfsStack.empty()){
        // printf("Procesing From Src: \n");
        s = dfsStack.top();
        dfsStack.pop();
        // printf("Node %d\n",s);
        //pop the node form stack, mark visited, 
        visited[s] = true;
        if (MN_list[s].is_hub){
            if (MN_list[s].h_idx < h){
                // copy from h to MN_list[s]
                finalID_to = get_RootSCCID(MN_list,s);
                finalID_from = get_RootSCCID(MN_list,ref_hub);
                MN_list[ref_hub].currentID = finalID_to;
                MN_list[ref_hub].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                    copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize);
                     #ifdef DEBUG
                     printf("(%d -> %d) 1 Copied to %d\n",src, dest, finalID_to);
                     #endif
                }// end if 
               }// end if found smaller hub 
            else{
                finalID_to = get_RootSCCID(MN_list,ref_hub);
                finalID_from = get_RootSCCID(MN_list,s);
                MN_list[s].currentID = finalID_to;
                MN_list[s].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                    copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize); 
                    #ifdef DEBUG
                    printf("(%d -> %d) 2 Copied to %d\n",src, dest, finalID_to);  
                    #endif           
                }//end if copy same hub_idx
            } // end else bigger hu
        }
        //if not hub
        else{
            // if not hub, mark hub as true, update the root
            finalID_to = get_RootSCCID(MN_list,ref_hub);
            MN_list[s].is_hub = true;
            MN_list[s].currentID = finalID_to;
            MN_list[s].h_idx = MN_list[finalID_to].h_idx;
            copy_hubInfo(MN_list[s].Hub_info, MN_list[finalID_to].Hub_info, hubsize);
            #ifdef DEBUG
            printf("(%d -> %d) 3 Copied to %d\n",src, dest, finalID_to);
            #endif
        } // if not hub

        int_int parent_range = g_meta->get_parents(s);
        if(MN_list[s].currentID == Hubs[0]){parent_range={0,0};}// do not look for parent/children of the hub itself
        
        for (int p = parent_range.first; p<parent_range.second; p++){
            int current_p = g_meta->b_col_idx[p];
            //if current parent not visited, and goes to hub, or is hub  || (MN_list[current_p].Hub_info[h] == 5)
            if ((!trimmed[current_p]) && (!visited[current_p]) && ((MN_list[current_p].Hub_info[h] == 1) || (MN_list[current_p].Hub_info[h] == 5)))
            {
                dfsStack.push(current_p);
            } //end if
            //if hub found do not push but process itself
        }
    }

    dfsStack.push(dest);
    while(!dfsStack.empty()){
        s = dfsStack.top();
        dfsStack.pop();
        visited[s] = true;
        if (MN_list[s].is_hub){
            if (MN_list[s].h_idx < h){
                // copy from h to MN_list[s]
                finalID_to = get_RootSCCID(MN_list,s);
                finalID_from = get_RootSCCID(MN_list,ref_hub);
                MN_list[ref_hub].currentID = finalID_to;
                MN_list[ref_hub].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                  copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize);  
                    #ifdef DEBUG
                     printf("(%d -> %d) 4 Copied to %d\n",src, dest, finalID_to);
                     #endif
               }
             }// end if found smaller hub 
            else{
                finalID_to = get_RootSCCID(MN_list,ref_hub);
                finalID_from = get_RootSCCID(MN_list,s);
                MN_list[s].currentID = finalID_to;
                MN_list[s].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                    copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize);   
                    #ifdef DEBUG
                    printf("(%d -> %d) 5 Copied to %d\n",src, dest, finalID_to);
                    #endif
                }
                
            } // end else bigger hub
        }
        //if not hub
        else{
            // if not hub, mark hub as true, update the root
            finalID_to = get_RootSCCID(MN_list,ref_hub);
            MN_list[s].is_hub = true;
            MN_list[s].currentID = finalID_to;
            MN_list[s].h_idx = MN_list[finalID_to].h_idx;
            copy_hubInfo(MN_list[s].Hub_info, MN_list[finalID_to].Hub_info, hubsize);
            #ifdef DEBUG
             printf("(%d -> %d) 6 Copied to %d\n",src, dest, finalID_to);
             #endif
        } // if not hub

        int_int children_range = g_meta->get_children(s);
        if(MN_list[s].currentID == Hubs[0]){children_range={0,0};}// do not look for parent/children of the hub itself
        
        for (int c = children_range.first; c<children_range.second; c++){
            int current_c = g_meta->f_col_idx[c];
            //if current parent not visited, and goes to hub, or is hub  || (MN_list[current_p].Hub_info[h] == 5)
            if ((!trimmed[current_c]) && (!visited[current_c]) && ((MN_list[current_c].Hub_info[h] == -1) || (MN_list[current_c].Hub_info[h] == 5)))
            {
                dfsStack.push(current_c);
            } //end if
            //if hub found do not push but process itself
        }

    }//end second while loop 

    } //end merge me function

// ******************************************************************************

// ******************************************************************************
void update_scc_id(MetaNode*& MN_list, int id){
    int final = id;
    while( final != MN_list[final].currentID)
    {
        final = MN_list[final].currentID;
    }
    MN_list[id].currentID = final;
}
// ****************************************************************************** 

// ****************************************************************************** 
bool apply_change(int*& from, int*& to, int hubsize, int direction){
    bool changed = false; // initialize return value as false, if any change is applied it will mark true, so the propagation continues
    // copy only the direction values
        for (int i = 0; i<hubsize; i++){
            
            //if dir = +1
            if (direction == 1){
                //propagate +1 values
                //if from[i] is hub/downstream of hub
                if (from[i] > 1 || from[i] == 5){
                    // downstream propagation reaches upstream/hub
                    if (to[i] == -1 || to[i] == 5){
                        to[i] = 5;
                        from[i] = 5;
                        changed = true;
                        // printf("Found SCC during propagation.\n");
                    }//end if
                    else if (to[i] != 1){
                        to[i] = 1; 
                        changed = true;
                    } //end else
                }//end if from is +ve
            }

            if (direction == -1){
                //propagate -1 values
                //if from[i] is hub/upstream of hub
                if (from[i] == -1 || from[i] == 5){
                    if(to[i] == 1 || to[i] == 5){
                        to[i] = 5;
                        from[i] = 5;
                        changed = true;
                        // printf("Found SCC during propagation.\n");
                    }//end if
                    else if(to[i] != -1){
                        to[i] = -1;
                        changed = true;
                    }//end else
  
                } // end if from is -ve
            }

            // //todo
            // //if from[i] = 1 and to[i] = -1/5 (mark them as merged???)
            // //if from  = hub or downstream of hub && to = hub or upstream of hub
            // if( ((from[i] == 1 || from[i] == 5) && (to[i] == 5 || to[i] == -1))) {
            //     printf("MUST MERGE THE SCC\n");   
                
            // }

            // else if((from[i] == direction) && (to[i] == 0)){
            //     to[i] = direction;
            //     changed = true;
            // } //end if
        }
        return changed;
}//end function
// ****************************************************************************** 


// ****************************************************************************** 
void propagate_inserts(Graph* g_meta,vector<int_int>* inserts, bool*& insert_status, MetaNode*& MN_list, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& p_up, bool*& p_down, bool*& trimmed, int hubsize, int p ){
        #pragma omp parallel for num_threads(p) schedule(guided)
        for (int ins = 0; ins<inserts->size(); ins++){
            if(!insert_status[ins]){
                int src = inserts->at(ins).first;
                int dest = inserts->at(ins).second;
                
            }
        }
    
    
    int num_nodes = g_meta->node_count;
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int node_id = 0; node_id<num_nodes; node_id++){
        int scc_id; // look at the scc_id of the node instead of own id
        bool updated; // boolean value for continuing the propagation
        //if the node_id is changed and node_id is not trimmed
        if(!trimmed[node_id] && propagate_changed_up[node_id]){
            // asynchronous way????
            int_int p_range = g_meta->get_parents(node_id);
            for (int p = p_range.first; p<p_range.second; p++){
                int parent_id = g_meta->b_col_idx[p];
                //if parent is not trimmed, copy the changes to the parent(SCC_ID of parent)
                if(!trimmed[parent_id]){
                    scc_id = MN_list[parent_id].currentID; // route the changed ID to the current ID of scc
                    updated = apply_change(MN_list[node_id].Hub_info, MN_list[scc_id].Hub_info, hubsize, -1); //-1 means parents
                    // if (updated){propagate_changed_up[scc_id] = true;} //mark propagate_changed_up 
                    if (updated){p_up[scc_id] = true;} //mark propagate_changed_up 
                }
            }//end for all parents
        }//end if proppagate up

        //if the node_id is changed and node_id is not trimmed    
        if(!trimmed[node_id] && propagate_changed_down[node_id]){
            int_int c_range = g_meta->get_children(node_id);
            for(int c = c_range.first; c<c_range.second; c++){
                int child_id = g_meta->f_col_idx[c];
                //if child is not trimmed, copy the changes to the child(SCC_ID of child)
                if(!trimmed[child_id]){
                    scc_id = MN_list[child_id].currentID; //route the changed ID to the current ID of scc
                    updated = apply_change(MN_list[node_id].Hub_info, MN_list[scc_id].Hub_info, hubsize, 1); //1 means the children update
                    // if (updated){propagate_changed_down[scc_id] = true;} //mark propagate_changed_down for further propagation
                     if (updated){p_down[scc_id] = true;} //mark propagate_changed_down for further propagation
       
                }
            }
        }//end if propagate changed down
        // propagate_changed_up[node_id] = false; //mark false as already propagated
        // propagate_changed_down[node_id] = false; 
    }//end for all nodes
    //TODO pointer optimization
    // delete[] propagate_changed_up;
    // delete[] propagate_changed_down;

    // propagate_changed_up = p_up;
    // propagate_changed_down = p_down;
    // #pragma omp parallel for num_threads(p) schedule(guided)
    // for (int node_id = 0; node_id<num_nodes; node_id++){
    //     propagate_changed_up[node_id] = p_up[node_id];
    //     propagate_changed_down[node_id] = p_down[node_id];
    // }//end parallel for

    // delete[] p_down;
    // delete[] p_up;

}//end function

// ****************************************************************************** 
void propagate_all(Graph* g_meta, MetaNode*& MN_list, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& p_up, bool*& p_down, bool*& trimmed, int hubsize, int p ){
    int num_nodes = g_meta->node_count;
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int node_id = 0; node_id<num_nodes; node_id++){
        int scc_id; // look at the scc_id of the node instead of own id
        bool updated; // boolean value for continuing the propagation
        //if the node_id is changed and node_id is not trimmed
        if(!trimmed[node_id] && propagate_changed_up[node_id]){
            // asynchronous way????
            int_int p_range = g_meta->get_parents(node_id);
            for (int p = p_range.first; p<p_range.second; p++){
                int parent_id = g_meta->b_col_idx[p];
                //if parent is not trimmed, copy the changes to the parent(SCC_ID of parent)
                if(!trimmed[parent_id]){
                    scc_id = MN_list[parent_id].currentID; // route the changed ID to the current ID of scc
                    updated = apply_change(MN_list[node_id].Hub_info, MN_list[scc_id].Hub_info, hubsize, -1); //-1 means parents
                    // if (updated){propagate_changed_up[scc_id] = true;} //mark propagate_changed_up 
                    if (updated){p_up[scc_id] = true;} //mark propagate_changed_up 
                }
            }//end for all parents
        }//end if proppagate up

        //if the node_id is changed and node_id is not trimmed    
        if(!trimmed[node_id] && propagate_changed_down[node_id]){
            int_int c_range = g_meta->get_children(node_id);
            for(int c = c_range.first; c<c_range.second; c++){
                int child_id = g_meta->f_col_idx[c];
                //if child is not trimmed, copy the changes to the child(SCC_ID of child)
                if(!trimmed[child_id]){
                    scc_id = MN_list[child_id].currentID; //route the changed ID to the current ID of scc
                    updated = apply_change(MN_list[node_id].Hub_info, MN_list[scc_id].Hub_info, hubsize, 1); //1 means the children update
                    // if (updated){propagate_changed_down[scc_id] = true;} //mark propagate_changed_down for further propagation
                     if (updated){p_down[scc_id] = true;} //mark propagate_changed_down for further propagation
       
                }
            }
        }//end if propagate changed down
        // propagate_changed_up[node_id] = false; //mark false as already propagated
        // propagate_changed_down[node_id] = false; 
    }//end for all nodes
    //TODO pointer optimization
    // delete[] propagate_changed_up;
    // delete[] propagate_changed_down;

    propagate_changed_up = p_up;
    propagate_changed_down = p_down;
    // #pragma omp parallel for num_threads(p) schedule(guided)
    // for (int node_id = 0; node_id<num_nodes; node_id++){
    //     propagate_changed_up[node_id] = p_up[node_id];
    //     propagate_changed_down[node_id] = p_down[node_id];
    // }//end parallel for

    // delete[] p_down;
    // delete[] p_up;

}//end function
// ****************************************************************************** 


// ******************************************************************************

// ******************************************************************************

int count_true(bool*& status, int size, int p){
    int count=0;
    #pragma omp parallel for num_threads(p) schedule(guided) reduction(+:count)
    for (int i = 0; i<size; i++){
        if(status[i]){
            count++;
        }
    }
    return count;
}
// ******************************************************************************
// Case = 0 for Do Not Create SCC(Same Src & dest or already existed path)
    // Case = 1 for Created SCC(Determined Path)
    // Case = 2 for Not Created Yet found a common hub (Traversal Incomplete Group)
    // Case = 4 Not created yet due to all zeros for multiplication

    // action list
    //  1 SCC can be created with respect to hub i
    //  0 SCC will not be created due to a hub same direction hub i
    //  -1 same side of a hub, if not changed until last, then do BFS to the hub
    //  -5 SCC will be created Hub to Ancestor i or Src is the hub
    //  5 SCC will be created Successor to Hub i or Dest is the hub
    //  25 The node was already in SCC
    //  3 default not found the product on any hub values

void update_property(Graph* g, Graph* g_meta,int*& Hubs, vector<int_int>* inserts,vector<int_int>* deletes, vector<int>* SCCx, unordered_map<int,int>* sccMAP,MetaNode*& MN_list, bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& insert_status,bool*& delete_status, int hubsize, int p)
{
    double st = omp_get_wtime();
    int* cases = new int [inserts->size()]{-1};
    int* common_hub = new int[inserts->size()]{-1};
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int i=0; i<inserts->size(); i++){
        //handle only if the insert status is false
        if(!insert_status[i]){
            int src = inserts->at(i).first;
            int dest = inserts->at(i).second;
            int_int action = action_from_hub_values(MN_list,src,dest,Hubs,hubsize);
            handle_action_initial(MN_list, Hubs, hubsize, i, action, insert_status, cases, common_hub);
        }
    }

    printf("\n Time determining the actions : %f \n", (float)(omp_get_wtime()-st));

    // // Collect the upstream and downstream for each hubs
    for (int h = 0; h < hubsize; h++)
    {
        bool* visited = new bool[g_meta->node_count]{false};
            #pragma omp parallel for num_threads(p) schedule(guided)
            for (int ins = 0; ins<inserts->size(); ins++){
                if (common_hub[ins] == h && cases[ins] == 1){
                   merge_me(g_meta, MN_list,trimmed, h, Hubs, hubsize, inserts->at(ins).first, inserts->at(ins).second, visited);
                    cases[ins] = 3; //not used currently
                }// end common hub processing
            }//end parallel for loop
        delete[] visited;
    }
     printf("\n Time for merging : %f \n", (float)(omp_get_wtime()-st));
       printf("\n");

}
// ******************************************************************************
void update_inserts(Graph* g, Graph* g_meta,int*& Hubs, vector<int_int>* inserts,vector<int_int>* deletes, vector<int>* SCCx, unordered_map<int,int>* sccMAP,MetaNode*& MN_list, bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& p_up, bool*& p_down, bool*& insert_status,bool*& delete_status, int hubsize, int p)
{
    // Step 1: Perform Initial Find action/ handle action and merge
    double st = omp_get_wtime();
    int* cases = new int [inserts->size()]{-1};
    int* common_hub = new int[inserts->size()]{-1};
    bool* new_completed =  new bool[inserts->size()]{false};

    int itr = 0;
    while(itr <2){
        itr++;
        #ifdef DEBUG
         printf("Iteration: %d \n", itr);
        print_status(insert_status, inserts);
        print_meta_network(g_meta, MN_list, g_meta->node_count, Hubs, hubsize, trimmed);
         #endif
        //step 1 propagate
        propagate_all(g_meta, MN_list, propagate_changed_up, propagate_changed_down, p_up, p_down, trimmed, hubsize, p );
        //step 2 find and handle action
        #pragma omp parallel for num_threads(p) schedule(guided)
        for (int i=0; i<inserts->size(); i++){
            if(!insert_status[i]){ // check if the insert status is false
                int src = inserts->at(i).first;
                int dest = inserts->at(i).second;
                    #ifdef DEBUG
                         printf("Iteration: %d, Unhandled insert (%d->%d)\n", itr, src, dest);
                    #endif
                
                int_int action = action_from_hub(MN_list,src,dest,Hubs,hubsize);
                    #ifdef DEBUG
                printf("Iteration: %d, Insert(%d->%d),  Determined action (%d : %d)\n", itr, src, dest, action.first, action.second);
                    #endif


                handle_action(MN_list, Hubs, hubsize, i,src,dest, action, insert_status, cases, common_hub, propagate_changed_up, propagate_changed_down, itr);
                
            }
        }
        //step 3 merge
         // Merge the inserts created
        for (int h = 0; h < hubsize; h++)
        {
            bool* visited = new bool[g_meta->node_count]{false};
                #pragma omp parallel for num_threads(p) schedule(guided)
                for (int ins = 0; ins<inserts->size(); ins++){
                    
                    if (insert_status[ins] && common_hub[ins] == h && cases[ins] == 1){
                    // if (common_hub[ins] == h && cases[ins] == 1){
                    merge_me(g_meta, MN_list,trimmed, h, Hubs, hubsize, inserts->at(ins).first, inserts->at(ins).second, visited);
                    cases[ins] = 3; //not used currently
                    }// end common hub processing
                }//end parallel for loop
            delete[] visited;
        }//end for all hubs during insertion



    }
    // Step


    // print_status(insert_status, inserts);
    // print_meta_network(g_meta, MN_list, g_meta->node_count, Hubs, hubsize, trimmed);
    /*
    int itr = 0;
    // printf("Iteration %d before while loop\n", itr);
    while (itr<2){

        // printf("\n\n\n\n\n\n ITERATION %d STARTED \n\n\n\n\n\n", itr);
        // // if(itr!=0){
        // //         // printf("Iteration %d for propagating\n", itr);
        // //     // do not propagate in first iteration
        // //     // propagate_all(g_meta, MN_list, propagate_changed_up, propagate_changed_down, trimmed, hubsize, p);
        // // }
        propagate_all(g_meta, MN_list, propagate_changed_up, propagate_changed_down, p_up, p_down, trimmed, hubsize, p );
        //find and handle action
        #pragma omp parallel for num_threads(p) schedule(guided)
        for (int i=0; i<inserts->size(); i++){
            int src = inserts->at(i).first;
            int dest = inserts->at(i).second;
        // if(trimmed[src] || trimmed[dest]){
        //     insert_status[i] = true; // mark true for the inserted
        // }
        //handle only if the insert status is false
            if(!insert_status[i]){
                int_int action = action_from_hub(MN_list,src,dest,Hubs,hubsize);
                handle_action(MN_list, Hubs, hubsize, i,src,dest, action, insert_status, cases, common_hub, propagate_changed_up, propagate_changed_down, itr);
            }//end if not handled inserts
        }//end for all inserts
        // printf("\n\n All actions Determined\n\n");
            // printf("Iteration %d before merge \n", itr);
        // Merge the inserts created
        for (int h = 0; h < hubsize; h++)
        {
            bool* visited = new bool[g_meta->node_count]{false};
                #pragma omp parallel for num_threads(p) schedule(guided)
                for (int ins = 0; ins<inserts->size(); ins++){
                    
                    if (insert_status[ins] && common_hub[ins] == h && cases[ins] == 1){
                    // if (common_hub[ins] == h && cases[ins] == 1){
                    merge_me(g_meta, MN_list,trimmed, h, Hubs, hubsize, inserts->at(ins).first, inserts->at(ins).second, visited);
                    cases[ins] = 3; //not used currently
                    }// end common hub processing
                }//end parallel for loop
            delete[] visited;
        }//end for all hubs during insertion

    // printf("Iteration %d: Count of  completed inserts after update propeerty: %d\n",itr, count_true(insert_status,inserts->size(),p));
    // printf("Iteration %d: Count of  completed deletes after update property: %d\n",itr,count_true(delete_status,deletes->size(),p));
    print_status(insert_status, inserts);
    print_meta_network(g_meta, MN_list, g_meta->node_count, Hubs, hubsize, trimmed);
    printf("\n\n\n\n\n\n ITERATION %d COMPLETED \n\n\n\n\n\n", itr);
    
        itr++;

    }//end while iteration 
   */
    // for (int ins = 0; ins<inserts->size(); ins++){
    //     printf("Insert (%d -> %d) Status %d, Case %d, Common Hub %d\n", inserts->at(ins).first, inserts->at(ins).second,insert_status[ins], cases[ins],common_hub[ins]);
    // }
  
    // Step 2: Perform Iteration k times

    // Step 3: Perform BFS for the unhandled cases.

//}

// ******************************************************************************




// ******************************************************************************
void update_deletes(Graph* g, Graph* g_meta,int*& Hubs, vector<int_int>* inserts,vector<int_int>* deletes, vector<int>* SCCx, unordered_map<int,int>* sccMAP,MetaNode*& MN_list, bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, bool*& insert_status,bool*& delete_status, int hubsize, int p)
{
    // Step 1: Perform Initial Find action/ handle action and merge
    double st = omp_get_wtime();
    // int* cases = new int [inserts->size()]{-1};
    // int* common_hub = new int[inserts->size()]{-1};

// for all metanodes
// if metanode is not trimmed 
// for all deletes:
#pragma omp parallel for num_threads(p) schedule(dynamic)
    for (int del = 0; del<deletes->size(); del++){
        for (int metanode = 0; metanode < g_meta->node_count; metanode++){
            if (!trimmed[metanode]){
            // if (true){ //TODO change true to if delete in metanode
            //   // Check 2 hop path
            //   // fwd set = (forward of deletes.first)
            //   // bwd set = (backward of deletes.sec)
            //   // fwd_set.intersection(bwd_set) > 0
            //   // status = completed
            //   // otherwise = differ
            // }
            }
    }
}


    

}

// ******************************************************************************






// ******************************************************************************
// void print_meta_network(Graph* g,MetaNode*& MN_List, int graph_size,int*& Hubs, int hubsize, bool*& trimmed)
// {
//     for (int i = 0; i<graph_size; i++)
//     {
//         color("yellow");
//         printf("\nNode: %d ",i);
//                if(trimmed[i]){
//             color("red");
//             printf(" TRIMMED!!!\n");
//             color("reset");
//         }
//         if(!trimmed[i]){
//             color("green");
//             printf("  NOT TRIMMED!!!\n");
//             color("reset");
//         }
//         MN_List[i].is_hub? printf(" Hub %d\n",MN_List[i].h_idx):printf("Not Hub\n");
//         printf("Current SCC ID: %d \n", MN_List[i].currentID);

//         printf("Children: ");
//         int_int c_range = g->get_children(i);
//         for (int c = c_range.first; c < c_range.second; c++)
//         {
//             printf("%d, ", g->f_col_idx[c]);
//         }
//         printf("\n Parents: ");
//         int_int p_range = g->get_parents(i);
//         for (int p = p_range.first; p < p_range.second; p++)
//         {
//             printf("%d, ", g->b_col_idx[p]);
//         }
//         printf("\n Hub Info : ");
//         for (int h = 0; h<hubsize; h++)
//         {
//             printf(" (%d->%d) \n", Hubs[h],MN_List[i].Hub_info[h]);
//         }
        

//     }
// }
// ******************************************************************************
#endif
