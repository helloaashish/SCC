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
#endif
