#ifndef ReadData_HPP
#define ReadData_HPP

#include<iostream>
#include<fstream>
#include<vector>
#include<string.h>
#include<map>

using namespace std;
typedef pair<int,int> int_int;


// ******************************************************************************
//Reading original (possibly non-contiguous) SCC Ids of nodes
void read_scc(char *file, int n, vector<int> *SCCx)
{
    // File reading parameters
    SCCx->resize(n);
    FILE *graph_file;
    char line[128];
    graph_file = fopen(file, "r");
    int ID;
    int l = 0;
    while (fgets(line, 128, graph_file) != NULL)
    {
        // Read line
        sscanf(line, "%d ", &ID);
        SCCx->at(l) = ID;
        l++;
    } // end of while
    return;
}
// ******************************************************************************

// ******************************************************************************
//Reading contigous SCC Ids
void read_sccmap(char *file, unordered_map<int,int> *sccMAP){
    FILE *graph_file;
    char line[128];
    graph_file = fopen(file, "r");
    while (fgets(line, 128, graph_file) != NULL)
    {
        int initial_id, final_id;
       sscanf(line, "%d  %d ", &initial_id, &final_id);
       sccMAP->insert({initial_id,final_id});    
    }
}
// ******************************************************************************

// ******************************************************************************
// Reading list of edges that got iserted or deleted
void read_changes(char *myfile,vector<int_int> *inserts, vector<int_int> *deletes,vector<int_int> *inserts_meta, vector<int_int> *deletes_meta, vector<int> *SCCx, unordered_map<int,int>* sccMAP)
{
    // File reading parameters
    FILE *graph_file;
    char line[128];
    int type;
    int_int ID_edge;
    int_int meta_ID_edge;
    graph_file = fopen(myfile, "r");
    while (fgets(line, 128, graph_file) != NULL)
    { 
        sscanf(line, "%d  %d %d", &ID_edge.first, &ID_edge.second, &type);

	//Push to delete and delete_meta_edge after conversion
        if (type == 0) // Delete from X
        {       
	    meta_ID_edge.first=sccMAP->at(SCCx->at(ID_edge.first));
	    meta_ID_edge.second=sccMAP->at(SCCx->at(ID_edge.second));
	    
	    deletes_meta->push_back(meta_ID_edge);
            deletes->push_back(ID_edge);
        }

        if (type == 1) // Insert to X
        {
	    meta_ID_edge.first=sccMAP->at(SCCx->at(ID_edge.first));
	    meta_ID_edge.second=sccMAP->at(SCCx->at(ID_edge.second));

 
             //Only insert if across meta nodes	 
       	    if(meta_ID_edge.first!=meta_ID_edge.second)
	    {
            
            inserts_meta->push_back(meta_ID_edge);
            inserts->push_back(ID_edge);
	    }
        
	} 
    } // end of while

    // printf("\n Deleted Edges which are Key Edges: %d ",key_deleted);
    // printf("\n Deleted Edges which are Not Key Edges: %d ",not_key_deleted);
    // printf("\n Inserted Edges on Same SCC: %d ",same_inserted);
    // printf("\n Inserted Edges on different SCC: %d ",different_inserted);

    fclose(graph_file);
}
// ******************************************************************************



// ******************************************************************************
//Reading the edgelist file and storing in src dest and weight arrays
void read_graph(char* file,int& N, int&M, int*& srcA, int*& destA, int*& weightsA){
    ifstream File(file);
    if(!File.is_open()){printf("ERROR OPENING FILE");}
    else{
        int wt;
        File >> N >>M;
        int* src = new int[M];
        int* dest = new int[M];
        int* wt_list = new int[M];
        for (int i =0; i<M; i++){
            File>>src[i] >> dest[i] >> wt_list[i];
        }
    
    srcA = src;
    destA = dest;
    weightsA = wt_list;
    }
}
// ******************************************************************************


#endif

