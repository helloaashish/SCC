#ifndef ReadData_HPP
#define ReadData_HPP

#include<iostream>
#include<fstream>
#include<vector>
#include<string.h>


using namespace std;
typedef pair<int,int> int_int;




// ******************************************************************************
void read_scc(vector<int> *SCCx, int n,char *file)
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
void read_sccmap(unordered_map<int,int> * sccMAP, char* file){
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
void read_changes(vector<int_int> *inserts, vector<int_int> *deletes, char* myfile)
{
    // File reading parameters
    FILE *graph_file;
    char line[128];
    int type;
    int_int ID_edge;
    graph_file = fopen(myfile, "r");
    while (fgets(line, 128, graph_file) != NULL)
    {
        sscanf(line, "%d  %d %d", &ID_edge.first, &ID_edge.second, &type);
        if (type == 0) // Delete from X
        {       
            //only insert if key edge
            deletes->push_back(ID_edge);
        }

        if (type == 1) // Insert to X
        {
            inserts->push_back(ID_edge);
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
void read_graph(char* file,int& n, int&m, int*& srcA, int*& destA, int*& weightsA, int active){
    ifstream File(file);
    if(!File.is_open()){printf("ERROR OPENING FILE");}
    else{
        // int m;
        File >> n >>m;
        int* src = new int[m];
        int* dest = new int[m];
        int* weights = new int[m];

        for (int i =0; i<m; i++){
            File>>src[i] >> dest[i];
            weights[i] = 1;
            // printf("read %d %d %d\n",src[i], dest[i], weights[i]);
        }
        srcA = src;
        destA = dest;
        weightsA = weights;
    }
}
// ******************************************************************************

// ******************************************************************************
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


// ******************************************************************************
void X_read_graph(char* file,int& n, int&m, vector<int_int>* edgelist, vector<int>* weights, int active){
    ifstream File(file);
    if(!File.is_open()){printf("ERROR OPENING FILE");}
    else{
        // int m;
        File >> n >>m;
        edgelist->resize(m);
        weights->resize(m);
        for (int i =0; i<m; i++){
            File>>edgelist->at(i).first >> edgelist->at(i).second;
            weights->at(i) = 1;
        }
    }
}
// ******************************************************************************

// ******************************************************************************
void X_read_graph(char* file,int& N,int& M, vector<int_int>* edgelist, vector<int>* wt_list){
    ifstream File(file);
    if(!File.is_open()){printf("ERROR OPENING FILE");}
    else{
        int wt;
        File >> N >>M;
        edgelist->resize(M);
        wt_list->resize(M);
        for (int i =0; i<M; i++){
            File>>edgelist->at(i).first >> edgelist->at(i).second >> wt_list->at(i);

        }
    }
}
// ******************************************************************************

// ******************************************************************************
void X_read_AllGraphs(char* file, int& N, vector<int_int>* edgelist, vector<int>* wt_list) {
    ifstream File(file);

    if (!File.is_open()) {
        printf("ERROR OPENING FILE");
    } else {
        int m;
        File >> N >> m;
        edgelist->resize(m);
        wt_list->resize(m);

        for (int i = 0; i < m; i++) {
            File >> edgelist->at(i).first >> edgelist->at(i).second;

            // Check if there is a third column (weight)
            if (File.peek() == '\n' ||File.peek() == ' ' || File.peek() == EOF) {
                // If the next character is a newline or end of file, default the weight to 1
                  wt_list->at(i) = 1;
            } else {
                // Default the weight to 1 if not present
              
                File>>wt_list->at(i);
                // Clear the failbit to avoid issues in subsequent iterations
                File.clear();
            }
        }
        }
}
// ******************************************************************************
#endif

