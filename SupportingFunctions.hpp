#ifndef SupportingFunctions_HPP
#define SupportingFunctions_HPP


#include<iostream>
#include<vector>
#include <unordered_map>
#include <map>
#include <stack>
#include <unordered_set>
#include "DataStructure.hpp"
#include <cstdio>

typedef pair<int,int> int_int;
using namespace std;

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
void color(const char *selected){
    if (strcmp(selected,"red")==0){printf("\033[1;31m");}
    else if (strcmp(selected,"green")==0){printf("\033[1;32m");}
    else if (strcmp(selected,"yellow")==0){printf("\033[1;33m");}
    else if (strcmp(selected,"purple")==0){printf("\033[1;35m");}
    else if (strcmp(selected,"reset")==0){printf("\033[0m");}
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

    // parallel in here did not work???
    for (int i = 0; i<m; i++){
        graph->f_row_ptr[src[i]+1]++;
        graph->b_row_ptr[dest[i]+1]++;
    }
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
void find_hubs(Graph* g, Graph* mg, MetaNode*& MN_list,vector<int>* SCCx, bool*& trimmed,int*& Hubs, int n, int N, int hubsize, int p){
    int count=0;
    vector<int_int> hub_candidates(N); //TODO change to array
    int non_zero = 0;
    bool* trim = new bool[N]; //boolean to indicate if node is source or sink 
    int* temp_hub = new int[hubsize]{0}; 


//Identify Candidates for Hubs
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int id =0; id<N; ++id)
    {
        int pr=0; //product of in and out degree
        int v = 0; //count for vertices that are not src or sink
        if ((mg->num_parents(id) >0) && (mg->num_childrens(id)>0)) //TODO can we get value during read and then store in array
        {
            pr = mg->num_parents(id) * mg->num_childrens(id);
            trim[id] = false;
    #pragma omp atomic capture
            v=non_zero++; 
            hub_candidates.at(v).first = pr;
            hub_candidates.at(v).second = id;
        }
        else{
            trim[id] = true;
        }
    }//end of for
    

    //Erase non-zeros to keep only viable candidates, and then sort by product of in*out
    hub_candidates.erase(hub_candidates.begin() + non_zero, hub_candidates.end());
    sort(hub_candidates.begin(), hub_candidates.end(), [](const int_int &a, const int_int &b)
         { return a.first > b.first; });

    #pragma omp parallel for num_threads(p) schedule(guided) //we may not need this parallel for
    for (int h=0; h<hubsize; ++h)
    {
        temp_hub[h] = hub_candidates.at(h).second; //TODO check if we can replace temp_hub with Hubs
        MN_list[hub_candidates.at(h).second].is_hub = true;
        MN_list[hub_candidates.at(h).second].h_idx = h;
    }

    trimmed = trim;
    Hubs = temp_hub;
    hub_candidates.clear();

}
// ******************************************************************************


// ******************************************************************************
void write_levels(Graph* g_meta,MetaNode*& MN_list, int*& Hubs,bool*& trimmed, bool*& propagate_changed_up, bool*& propagate_changed_down, int hubsize, int p){
    #pragma omp parallel for num_threads(p) schedule(dynamic)
    for (int h=0; h<hubsize; h++){
        int current_hub = Hubs[h];
        MN_list[current_hub].Hub_info[h] = 5; //mark itself as 5 to show it is a hub

//Get start and end indices of children and parent
        int_int range_children = g_meta->get_children(current_hub);
        int_int range_parents = g_meta->get_parents(current_hub);

        // look for the children of the hub and mark them +1 if untrimmed
        for (int out = range_children.first; out<range_children.second; out++){
            int current_neigh = g_meta->f_col_idx[out];      
            if (!trimmed[current_neigh]){
                MN_list[current_neigh].Hub_info[h] = 1;
                propagate_changed_down[current_neigh] = true; //mark that 1 to be propagated to children

            }
        }
         // look for tht parents of the hub and mark them -1 if untrimmed
        for (int in = range_parents.first; in<range_parents.second; in++){
            int current_neigh = g_meta->b_col_idx[in];      
            if (!trimmed[current_neigh]){
                MN_list[current_neigh].Hub_info[h] = -1;
                propagate_changed_up[current_neigh] = true; //mark that -1 to be propagated to parent
            }
        }
    }
}
// ******************************************************************************

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
void convert_changes(Graph* g, Graph* g_meta, vector<int_int>* inserts,vector<int_int>* deletes, vector<int>* SCCx, unordered_map<int,int>* sccMAP,MetaNode*& MN_list, bool*& trimmed, bool*& insert_status,bool*& delete_status, int p){
    int count_i=0;
    int count_d = 0;

    #pragma omp parallel for num_threads(p) schedule(dynamic)
    for (int i = 0; i<inserts->size(); i++)
    {
        //Obtain src and dest of insert
    int src = inserts->at(i).first;
    int dest = inserts->at(i).second;

      //Convert to corresponding MetaNode or SCC
    int m_src = sccMAP->at(SCCx->at(src));
    int m_dest = sccMAP->at(SCCx->at(dest));

    // RENAME THEM
    inserts->at(i).first = m_src;
    inserts->at(i).second = m_dest;

    if (m_src == m_dest){
        insert_status[i] = true;} //inserted within same SCC
    else{
        #pragma omp atomic
            count_i++;

        //different scc insertion
        // mark status false indicating to be procesased further
        //mark corresponding MetaNodes as src and dest of inserts
        //TODO check if this is required
        insert_status[i] = false;
        MN_list[m_src].inserted_from = true;
        MN_list[m_dest].inserted_to = true;

        //Mark MetaNode trim=false if particiapting in insert
        if(trimmed[m_src] && (g_meta->num_childrens(m_src) == 0)){
            trimmed[m_src] = false;
        }
        if(trimmed[m_dest] && (g_meta->num_parents(m_dest) == 0)){
            trimmed[m_dest] = false;
        }

        // if still trimmed then mark completed for the inserts
        if(trimmed[m_src] || trimmed[m_dest]){
            insert_status[i] = true; // mark true for the inserted
        }
        }
    }//end of for


    // #pragma omp parallel for num_threads(p) schedule(dynamic)
    // for(int i = 0; i<deletes->size(); i++)
    // {
    //     int src = deletes->at(i).first;
    //     int dest =  deletes->at(i).second;

    //     int m_src = sccMAP->at(SCCx->at(src));
    //     int m_dest = sccMAP->at(SCCx->at(dest));

    //     //         // RENAME THEM
    //     // deletes->at(i).first = m_src;
    //     // deletes->at(i).second = m_dest;

    //     if (m_src == m_dest){ //within scc deletion
    //         //check key edge, if not key edge, delete and mark done for the delete
    //         int_int children_range = g->get_children(src);
    //         for (int c = children_range.first; c<children_range.second; c++){
    //             if (dest == g->f_col_idx[c]){
    //                 if (g->edge_wt[c] == -1) {
    //                     delete_status[i] = false;
    //                   #pragma omp atomic
    //                     count_d++;} // key edge deletion mark false
                    
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
    //     // SINCHRONIZation Required 
    //     if (m_src != m_dest)
    //     {
    //         delete_status[i] = true;
    //         int_int c_range = g_meta->get_children(m_src);
    //        for (int c = c_range.first; c<c_range.second; c++)
    //        {
    //         if (m_dest == g_meta->f_col_idx[c]){
    //             #pragma omp atomic
    //             g_meta->edge_wt--; // can this be done????
    //         }
    //        }

    //     }
    // }

    printf("Inserts Unhandled = %d , Deletes Unhandled=d", count_i);

}
// ******************************************************************************

// ******************************************************************************
int_int action_from_hub_values(MetaNode*& MN_list,int src,int dest,int*& Hubs,int hubsize){
    int_int return_status;
    return_status.first = 3; //differ status
    return_status.second = -1; // reference hub
    // color("red");
    // printf("\n\n Finding Action %d -> %d\n", src, dest);
    // color("reset");

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
                // printf("Returned as CREATED SCC with reference hub %d \n",Hubs[i]);
                return {1,i};
            }
            else
            {
                //SCC will not be createe as path already exixts 
                // src -> hub -> dest
                // printf("Returned as NOT CREATED SCC with reference hub %d \n",Hubs[i]);
                return {0,i};
            }
        }

        //CASE B
        else if(val == 1)
        {
            // this means both src and dest are in same side of hub
            // for this if we search for dest to hub and find src ; then SCC will be created
            // if we do not find src in paths then scc will not be created
            return_status = {-1,i};
            // return {-1,i};
        }

        //CASE C
        else if(val == -5)
        {
            if (src==Hubs[i]){
                // printf("Returned as CREATED SCC (hub->upstream) with reference hub %d \n",Hubs[i]);
                return {-5,i};//SCC can be created hub to upstream
            }
            else{
                // printf("Returned as NOT CREATED SCC (upstream ->HUB) with reference hub %d \n",Hubs[i]);
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }

        //CASE D
        else if(val == 5)
        {
            if (dest==Hubs[i]){
                // printf("Returned as CREATED SCC (downstream->hub) with reference hub %d \n",Hubs[i]);
                return {5,i};//SCC can be created down -> hub
            }
            else{
                // printf("Returned as NOT CREATED SCC (hub ->down) with reference hub %d \n",Hubs[i]);
                return {0,i};  // SCC will not be created upstream -> hub
            }
        }    

        else if(val == 25)
        {
            // printf("Returned as SAME SCC");
            return {0,i}; // returned will not be created, already in same scc
        }

    }
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



// ******************************************************************************

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
//copied from previous vector implementation of merge scc; not using currently; 
void merge_SCC(Graph* g_meta, MetaNode*& MN_list, int h,int*& Hubs,int hubsize, vector<int>* upstream, vector<int>* downstream)
{
    vector<int> nodes_scc;
    nodes_scc.clear();
    int* new_hub_info = new int[hubsize]{0}; // new hubinfo for all the nodes after converging
    //BFS Datastructure
    int num_metanodes = g_meta->node_count;
    bool* visited = new bool[num_metanodes]{false};
    int current_child, current_parent, s;
    bool up = true;
    stack<int> upStack(deque<int>(upstream->begin(), upstream->end()));
    stack<int> downStack(deque<int>(downstream->begin(), downstream->end()));
    while(!upStack.empty()){
        // printf("Collecting upstack size %d\n",upStack.size());
        s = upStack.top();
        upStack.pop();
        nodes_scc.push_back(s); //collect the nodes that is a part of new SCC
        visited[s] = true; //mark visited
        update_new_hub_info(MN_list, s, new_hub_info, up, hubsize);
        //*** update_new_hub_info()  // change the hubinfo for the merged node s
        //look for all neighbors and check the hub value
        int_int children_range = g_meta->get_children(s);
        for (int i = children_range.first; i<children_range.second; i++)
        {
            current_child = g_meta->f_col_idx[i];
            // if children is not visited
            // children reaches the hub meaning Hubinfo is -1 or 5 children is The  hub
            if ((!visited[current_child]) && ((MN_list[current_child].Hub_info[h] == -1) || (MN_list[current_child].Hub_info[h] == 5)))
            {
                upStack.push(current_child);
            } //end if
        }//end for
    }// end while

    up = false;
    while(!downStack.empty()){
       s = downStack.top();
       downStack.pop(); 
       nodes_scc.push_back(s);
       visited[s] = true; 
        // printf("Collecting Downstack size %d\n",downStack.size());
    update_new_hub_info(MN_list, s, new_hub_info, up, hubsize);
    int_int parents_range = g_meta->get_parents(s);
    for (int i = parents_range.first; i<parents_range.second; i++)
    {
        current_parent = g_meta->b_col_idx[i];
        // if parent is not visited
        // parent can be reached from the hub meaning Hubinfo is 1 or 5 parent is The  hub
         if ((!visited[current_parent]) && ((MN_list[current_parent].Hub_info[h] == 1) || (MN_list[current_parent].Hub_info[h] == 5)))
            {
                downStack.push(current_parent); // add it to the list of
            }//end if condition

    }//end for loop

    }//end while loop

// Now since all the vectors have same SCC ID we can update their
for (int c = 0; c < nodes_scc.size(); c++)
    {
        int changed_node = nodes_scc.at(c);
        MN_list[changed_node].currentID = h;
        color("red");
        printf(" Node %d Changed to %d \n",changed_node, h);
        color("reset");
    }
return;   

}
// ******************************************************************************


// ******************************************************************************
void copy_hubInfo(int*& from, int*& to, int hubsize){
    for (int i = 0; i< hubsize; i++){
        if (from[i] == 5){to[i] = 5;}
        else if(to[i] == 0){
            to[i] = from[i];
        }
    }
}
// ******************************************************************************


// ******************************************************************************
void merge_scc(Graph* g_meta, MetaNode*& MN_list, bool*& trimmed, int h, int*& Hubs, int hubsize, int src, int dest, bool*& visited, int*& num_common_hubs, bool& unique_hub_value){
    int ref_hub = Hubs[h];
    stack<int> dfsStack;
    int s;
    dfsStack.push(src);
    while(!dfsStack.empty()){
        s = dfsStack.top();
        dfsStack.pop();
        visited[s] = true;
        MN_list[s].Hub_info[h] = 5; //mark as hub
        // MN_list[s].is_hub = true;
        MN_list[s].currentID = ref_hub;
        //(!MN_list[s].is_hub) &&
        if( (!MN_list[s].is_hub) && (unique_hub_value)){
            unique_hub_value = false;
            num_common_hubs[s] = num_common_hubs[s] + 1;
        }

        // Now we know s is part of new Hub; update the info for hub and s itself
        for (int h_id = 0; h_id<hubsize; h_id++){
            int current_hub_info = MN_list[ref_hub].Hub_info[h_id];
            //if the ref_hub do not know about any hub (h_id) it copies the info form s
            if(current_hub_info == 0){
                 MN_list[ref_hub].Hub_info[h_id]  = MN_list[s].Hub_info[h_id]; 
            }//end if
            //if the ref_hub know the hub but the hub is now part of single SCC
            else if(MN_list[s].Hub_info[h_id] == 5){
                MN_list[ref_hub].Hub_info[h_id] = 5;
            }
        }//end for

        int_int parent_range = g_meta->get_parents(s);
        if(s != 69){
        for (int p = parent_range.first; p<parent_range.second; p++){
            int current_p = g_meta->b_col_idx[p];
            //if current parent not visited, and goes to hub, or is hub  || (MN_list[current_p].Hub_info[h] == 5)
            // if ((!trimmed[current_p]) && (!visited[current_p]) && (MN_list[current_p].Hub_info[h] == 1))
            if ((!trimmed[current_p]) && (!visited[current_p]) && ((MN_list[current_p].Hub_info[h] == 1) || (MN_list[current_p].Hub_info[h] == 5)))
            {
                dfsStack.push(current_p);
            } //end if
            //if hub found do not push but process itself
        }
        }
        else{
            printf("Big Parent size for %d Number of parents %d reference Hub %d \n   ", s, g_meta->num_parents(s), ref_hub);
        }


    }

    dfsStack.push(dest);
    while(!dfsStack.empty()){
        s = dfsStack.top();
        dfsStack.pop();
        visited[s] = true;
        MN_list[s].Hub_info[h] = 5; //mark as hub
        MN_list[s].currentID = ref_hub;
        // (!MN_list[s].is_hub) && 
        if((!MN_list[s].is_hub) &&  (unique_hub_value)){
        unique_hub_value = false;
        num_common_hubs[s] = num_common_hubs[s] + 1;
        }
        // Now we know s is part of new Hub; update the info for hub and s itself
        for (int h_id = 0; h_id<hubsize; h_id++){
            int current_hub_info = MN_list[ref_hub].Hub_info[h_id];
            //if the ref_hub do not know about any hub (h_id) it copies the info form s
            if(current_hub_info == 0){
                 MN_list[ref_hub].Hub_info[h_id]  = MN_list[s].Hub_info[h_id]; 
            }//end if
            else if (MN_list[s].Hub_info[h_id] == 5){
                MN_list[ref_hub].Hub_info[h_id] = 5;
            }

        }//end for

        int_int children_range = g_meta->get_children(s);
        if(s!= 69){
        for (int c = children_range.first; c<children_range.second; c++){
            int current_c = g_meta->b_col_idx[c];
            //if current parent not visited, and goes to hub, or is hub || (MN_list[current_c].Hub_info[h] == 5)
            //if ((!trimmed[current_c]) && (!visited[current_c]) && (MN_list[current_c].Hub_info[h] == -1))
            if ((!trimmed[current_c]) && (!visited[current_c]) && ((MN_list[current_c].Hub_info[h] == -1)|| (MN_list[current_c].Hub_info[h] == 5)))
            {
                dfsStack.push(current_c);
            } //end if
        } // end for
        }
        else{
            printf("Big Children size for %d Number of Children %d\n ", s, g_meta->num_childrens(s));
        }

    }

    
    return;


}

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
void merge_me(Graph* g_meta, MetaNode*& MN_list, bool*& trimmed, int h, int*& Hubs, int hubsize, int src, int dest, bool*& visited, int*& num_common_hubs){
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
                }// end if 
               }// end if found smaller hub 
            else{
                finalID_to = get_RootSCCID(MN_list,ref_hub);
                finalID_from = get_RootSCCID(MN_list,s);
                MN_list[s].currentID = finalID_to;
                MN_list[s].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                    copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize);                
                }//end if copy same hub_idx
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
               }
             }// end if found smaller hub 
            else{
                finalID_to = get_RootSCCID(MN_list,ref_hub);
                finalID_from = get_RootSCCID(MN_list,s);
                MN_list[s].currentID = finalID_to;
                MN_list[s].h_idx = MN_list[finalID_to].h_idx;
                if(finalID_to != finalID_from){
                    copy_hubInfo(MN_list[finalID_from].Hub_info , MN_list[finalID_to].Hub_info, hubsize);   
                    // printf("Copied from %d -> %d \n",finalID_from,finalID_to);
                    // printf("Condition U2\n");
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
            if((from[i] == direction) && (to[i] == 0)){
                to[i] = direction;
                changed = true;
            } //end if
        }
        return changed;
}//end function
// ****************************************************************************** 


// ****************************************************************************** 
void propagate_all(Graph* g_meta, MetaNode*& MN_list,bool*& propagate_changed_up, bool*& propagate_changed_down, bool*&trimmed, int hubsize,int p ){
    int num_nodes = g_meta->node_count;
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int node_id = 0; node_id<num_nodes; node_id++){
        propagate_changed_up[node_id] = false; //mark false as already propagated
        propagate_changed_down[node_id] = false; 
        int scc_id; // look at the scc_id of the node instead of own id
        bool updated; // boolean value for continuing the propagation
        //if the node_id is changed and node_id is not trimmed
        if(!trimmed[node_id] && propagate_changed_up[node_id]){
            int_int p_range = g_meta->get_parents(node_id);
            for (int p = p_range.first; p<p_range.second; p++){
                int parent_id = g_meta->b_col_idx[p];
                //if parent is not trimmed, copy the changes to the parent(SCC_ID of parent)
                if(!trimmed[parent_id]){
                    scc_id = MN_list[parent_id].currentID; // route the changed ID to the current ID of scc
                    updated = apply_change(MN_list[node_id].Hub_info, MN_list[scc_id].Hub_info, hubsize, -1); //-1 means parents
                    if (updated){propagate_changed_up[scc_id] = true;} //mark propagate_changed_up 
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
                    if (updated){propagate_changed_down[scc_id] = true;} //mark propagate_changed_down for further propagation
                }
            }
        }//end if propagate changed down

    }//end for all nodes

}//end function
// ****************************************************************************** 


// ******************************************************************************
//TODO Can make this better / removed if hub idx for all hub is marked during find_hub function check if the hub id is renamed. 
int check_renamed( int*& hub_info , int hub_idx){
    for (int i = 0; i<hub_idx; i++){
        if (hub_info[i] == 5){
            return i;
        }
    }
    return hub_idx;
}
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
    // int* status = new int[inserts->size()]{0};
    double st = omp_get_wtime();
    bool* involved_in_merge = new bool[g_meta->node_count]{false}; //yet to be populated
    int* num_common_hubs = new int[g_meta->node_count]{0}; //reduce 
    int* cases = new int [inserts->size()]{-1};
    int* common_hub = new int[inserts->size()]{-1};
    #pragma omp parallel for num_threads(p) schedule(guided)
    for (int i=0; i<inserts->size(); i++){
        //handle only if the insert status is false
        if(!insert_status[i]){
            int src = inserts->at(i).first;
            int dest = inserts->at(i).second;
            int_int action;
            action = action_from_hub_values(MN_list,src,dest,Hubs,hubsize);
            handle_action_initial(MN_list, Hubs, hubsize, i, action, insert_status, cases, common_hub);
        }
    }
    printf("\n Time determining the actions : %f \n", (float)(omp_get_wtime()-st));

    // Collect the upstream and downstream for each hubs
    // bool* active_hub = new bool[hubsize]{true};// active hub
    for (int h = 0; h < hubsize; h++)
    {
        bool* visited = new bool[g_meta->node_count]{false};
            #pragma omp parallel for num_threads(p) schedule(guided)
            for (int ins = 0; ins<inserts->size(); ins++){
                if (common_hub[ins] == h && cases[ins] == 1){
                   merge_me(g_meta, MN_list,trimmed, h, Hubs, hubsize, inserts->at(ins).first, inserts->at(ins).second, visited,num_common_hubs);
                    cases[ins] = 3; //not used currently
                }// end common hub processing
            }//end parallel for loop
        delete[] visited;
    }
     printf("\n Time for merging : %f \n", (float)(omp_get_wtime()-st));
       printf("\n");

// UNCOMMENT
    // int itr = 0;
    // while (itr < 3)
    // {
    //     #pragma omp parallel for num_threads(p) schedule (guided)
    //     for (int i = 0; i<inserts->size(); i++){
    //         int src = inserts->at(i).first;
    //         int dest = inserts->at(i).second;
    //         propagate_all(g_meta,MN_list,propagate_changed_up, propagate_changed_down, trimmed, hubsize, p);
    //         int_int action;
    //         action = action_from_hub_values(MN_list,src,dest,Hubs,hubsize);
    //         handle_action_initial(MN_list, Hubs, hubsize, i, action, insert_status, cases, common_hub);
    //     }

    // }
}
// ******************************************************************************


// ******************************************************************************
void print_meta_network(Graph* g,MetaNode*& MN_List, int graph_size,int*& Hubs, int hubsize, bool*& trimmed)
{
    for (int i = 0; i<graph_size; i++)
    {
        color("yellow");
        printf("\nNode: %d ",i);
               if(trimmed[i]){
            color("red");
            printf(" TRIMMED!!!\n");
            color("reset");
        }
        if(!trimmed[i]){
            color("green");
            printf("  NOT TRIMMED!!!\n");
            color("reset");
        }
        MN_List[i].is_hub? printf(" Hub %d\n",MN_List[i].h_idx):printf("Not Hub\n");
        printf("Current SCC ID: %d \n", MN_List[i].currentID);
        // if(!trimmed[i]){
        // printf("Changed SCC ID: %d \n", Hubs[MN_List[i].h_idx]);
        // }
        // else{
        //      printf("Changed SCC ID: %d \n", i);
        // }

        printf("Children: ");
        int_int c_range = g->get_children(i);
        for (int c = c_range.first; c < c_range.second; c++)
        {
            printf("%d, ", g->f_col_idx[c]);
        }
        printf("\n Parents: ");
        int_int p_range = g->get_parents(i);
        for (int p = p_range.first; p < p_range.second; p++)
        {
            printf("%d, ", g->b_col_idx[p]);
        }
        printf("\n Hub Info : ");
        for (int h = 0; h<hubsize; h++)
        {
            printf(" (%d->%d) \n", Hubs[h],MN_List[i].Hub_info[h]);
        }
        

    }
}
// ******************************************************************************
#endif