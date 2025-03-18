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
    fclose(graph_file);
}
// ******************************************************************************

// ******************************************************************************
void read_graph(char* file, int& n, int&m, int*& srcA, int*& destA, int*& weightsA, int active){
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
void read_inserts(const char* file, int*& srcA, int*& destA, int*& weightsA, int& m) {
    ifstream File(file);
    if (!File.is_open()) {
        printf("ERROR OPENING FILE\n");
        return;
    }

    vector<int> src, dest, weights;

    int u, v, w;
    while (File >> u >> v >> w) {  // Read file in a single pass
        src.push_back(u);
        dest.push_back(v);
        weights.push_back(w);
    }
    File.close();

    // Get total count
    m = src.size();

    // Allocate memory and copy data
    srcA = new int[m];
    destA = new int[m];
    weightsA = new int[m];

    copy(src.begin(), src.end(), srcA);
    copy(dest.begin(), dest.end(), destA);
    copy(weights.begin(), weights.end(), weightsA);
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
#endif

