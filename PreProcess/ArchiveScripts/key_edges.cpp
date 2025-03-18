//./a.out /home/users/apandey/Bhowmick_SCC/SCC-shared/Examples/dag.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/Examples/TestDAGs/dag_MG.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/Examples/dag_CC.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/Examples/TestDAGs/dag_MAP.txt /home/users/apandey/Datasets_SCC/ChangeEdges/dag_ce.txt 19 19
//./a.out /home/users/apandey/Bhowmick_SCC/SCC-shared/Examples/dag.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/Examples/TestDAGs/test_dag14.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/Examples/TestDAGs/test_dag14CC.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/Examples/TestDAGs/test_dag14CC.txt 19 14
//./a.out /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/baidu_1 /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/baidu_1_ME.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/scc_baidu.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/baidu_1_Map.txt /home/users/apandey/Datasets_SCC/ChangeEdges/baidu_ce/baidu_1M_25 2141301 1503003 
//./a.out /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/livejournal_1 /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/livejournal_1_ME.txt /home/users/apandey/Bhowmick_SCC/SCC-shared/DatasetsV2/scc_baidu.txt /home/users/apandey/Datasets_SCC/SCCx/scc_livejournal.txt 4846609 970270
// INPUT HEADERS

#include <omp.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <map>
#include <stack>
#include <iostream>
#include <string>

using namespace std;


typedef pair<int, int> int_int;


struct D_ADJ
{
    vector<int> In_Neigh;  // indegree neighbors
    vector<int> Out_Neigh; // outdegree neighbors
    vector<int> Weight;  //weight is 1 for all outgoing nodes
                    // if wt is -1 than the 
    // Constructor
    D_ADJ()
    {
        In_Neigh.clear();
        Out_Neigh.clear();
    }

    // Destructor
    void clear()
    {
        while (!In_Neigh.empty())
        {
            In_Neigh.pop_back();
        }
        while (!Out_Neigh.empty())
        {
            Out_Neigh.pop_back();
        }
    }
};
typedef vector<D_ADJ> D_Network;

void read_network(D_Network *X,int& a, int& b, char *file)
{
    // File reading parameters
    FILE *graph_file;
    char line[128];

    graph_file = fopen(file, "r");
    fgets(line,128,graph_file);
    int n,m;
    sscanf(line,"%d %d", &n,&m);
    D_ADJ row;
    row.In_Neigh.clear();
    row.Out_Neigh.clear();
    row.Weight.clear();
    X->resize(n,row);
    a = n;
    b = m;
    while (fgets(line, 128, graph_file) != NULL)
    {
        int n1, n2;
        sscanf(line, "%d %d ", &n1, &n2);
        X->at(n1).Out_Neigh.push_back(n2);
        X->at(n2).In_Neigh.push_back(n1);
        X->at(n1).Weight.push_back(1); //add weight for graphs

    } // end of while
    fclose(graph_file);

    return;

}
void read_SCC(vector<int> *SCCx, char *file)
{
    // File reading parameters
    FILE *graph_file;
    char line[128];

    graph_file = fopen(file, "r");
    int ID;
    SCCx->clear();
    while (fgets(line, 128, graph_file) != NULL)
    {
        // Read line
        sscanf(line, "%d ", &ID);
        SCCx->push_back(ID);

    } // end of while

    return;
}
void find_key_edges(D_Network* X, vector<int>* SCCx){
    map<int, int> elementCounts;
    vector<int_int> counts;
    vector<bool> visited(X->size(),false);
    counts.resize(0);
    // Counting occurrences of each element


    for (int element : *SCCx) {
        // If element is already in the map, increment its count
        if (elementCounts.find(element) != elementCounts.end()) {
            elementCounts[element]++;
        } else {
            // If element is not in the map, add it with count 1
            elementCounts[element] = 1;
        }
    }//end for
//count the number of nodes
    for (const auto& pair : elementCounts) {
        if (pair.second > 1) {counts.push_back({pair.first,pair.second});}
    }
int total = 0;


#pragma omp parallel for num_threads(p) reduction(+:total)
for (int i = 0; i<counts.size(); i++)
{
    // cout<<counts.at(i).first<<"and"<<counts.at(i).second<<"\n";
    int first_time = true;
    int scc_id = counts.at(i).first;
    // int remaining_visits = counts.at(i).second;
    stack<int_int> dfsStack;
    dfsStack.push({scc_id,-1}); // start with the scc_id node, it is guranteed in SCC
    while(!dfsStack.empty()){
        int current = dfsStack.top().first;
        int parent = dfsStack.top().second;
        dfsStack.pop();
        if(!visited[current]){
            visited[current] = true; //mark visited
            if (first_time){
                visited[current] = false;
                first_time = false;
            }
            if (parent != -1){
                // temp_key_edges.push_back({parent, current});
                auto it = find(X->at(parent).Out_Neigh.begin(), X->at(parent).Out_Neigh.end(), current);
                if (it == X->at(parent).Out_Neigh.end()) {cout<<" ERROR";}
        // Calculate the index of the found element using std::distance()
                else{
                int index = distance(X->at(parent).Out_Neigh.begin(), it);
                X->at(parent).Weight.at(index) = -1; //mark as key edge for the edge.
                // printf("Key Edge: %d -> %d \n",parent,current);
                total += 1;
        }     
                } // add to the temp path
            for (int j = 0; j<X->at(current).Out_Neigh.size(); j++){
                int neigh = X->at(current).Out_Neigh.at(j);
                if (SCCx->at(neigh) == scc_id){ //only look for the neighbors with same sccID
                    if (!visited[neigh]){
                        dfsStack.push({neigh,current});
                    }
                }

            }
        }
 }

}
cout<<"Total number of Key edges: "<<total<<endl;
}



int main(int argc, char *argv[])
{
    D_Network X;
    vector<int> SCCx;
    int n;
    int m;
    string filename = argv[1];
    read_network(&X,n,m, argv[1]);
    read_SCC(&SCCx, argv[2]);
    find_key_edges(&X, &SCCx); 


    ofstream Myfile("keyEdged.txt");
    Myfile << n <<" "<<m<<" "<<"\n";
    for (int i=0; i<X.size(); i++){
        int src = i;
        for (int j = 0; j<X.at(i).Out_Neigh.size(); j++){
            int dest = X.at(i).Out_Neigh.at(j);
            int wt = X.at(i).Weight.at(j);
            Myfile << src <<" "<<dest<<" "<<wt<<"\n";
        }
    }
    return 0; 

}




