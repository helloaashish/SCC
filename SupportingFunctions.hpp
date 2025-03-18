#ifndef SupportingFunctions_HPP
#define SupportingFunctions_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <stack>
#include <unordered_set>
#include "DataStructure.hpp"
#include "PrintFunctions.hpp"
#include <cstdio>
#include "hinfo.h"
typedef pair<int, int> int_int;
using namespace std;

// #define DEBUG

// ******************************************************************************
void check_file_open(char *metagraph_file, char *changes, int total_args)
{

    if (total_args < 5)
    {
        cout << "usage ./a.out metagraph change_metaedges hubsize num_threads  \n";
    }
    // Check to see if file opening succeeded
    ifstream the_file1(metagraph_file);
    if (!the_file1.is_open())
    {
        cout << "INPUT ERROR:: Could not open  Metagraph file\n";
    }

    ifstream the_file2(changes);
    if (!the_file2.is_open())
    {
        cout << "INPUT ERROR:: Could not open changes file\n";
    }
}
// ******************************************************************************

// ******************************************************************************
void create_graph(int *src, int *dest, int *wt_list, int n, int m, Graph *graph)
{
    // if (wt_list->size() == 0){wt_list->resize(int(edge_list->size()),1);}
    // resize both col_idx
    graph->f_col_idx = new int[m]();
    graph->b_col_idx = new int[m]();
    graph->edge_wt = new int[m]();
    graph->f_row_ptr = new int[n + 1]();
    graph->b_row_ptr = new int[n + 1]();
    graph->node_count = n;
    graph->edge_count = m;

    graph->in_deg = new int[n]{0};
    graph->out_deg = new int[n]{0};

    // parallel in here did not work???
    for (int i = 0; i < m; i++)
    {
        graph->in_deg[dest[i]]++;
        graph->out_deg[src[i]]++;
        graph->f_row_ptr[src[i] + 1]++;
        graph->b_row_ptr[dest[i] + 1]++;
    }
    // for (const auto& edge : *edge_list){
    //     graph->f_row_ptr[edge.first + 1]++;
    //     graph->b_row_ptr[edge.second + 1]++;
    // }
    // finding the cumulative sum for starting vertex
    for (int i = 1; i <= n; i++)
    {
        graph->f_row_ptr[i] += graph->f_row_ptr[i - 1];
        graph->b_row_ptr[i] += graph->b_row_ptr[i - 1];
    }

    // vector<int> temp_next_outgoing_idx(n,0);
    // vector<int> temp_next_incoming_idx(n,0);
    int *temp_next_outgoing_idx = new int[n]();
    int *temp_next_incoming_idx = new int[n]();

    for (int i = 0; i < m; i++)
    {
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
// Set get a value (0 to 3) at a given index for  hub information
void set_value(std::vector<uint8_t> &arr, size_t index, uint8_t value)
{
    size_t byte_index = index / 4;                   // Each byte holds 4 values
    size_t bit_offset = (index % 4) * 2;             // 2 bits per value
    arr[byte_index] &= ~(0b11 << bit_offset);        // Clear the previous value
    arr[byte_index] |= (value & 0b11) << bit_offset; // Set new value
}

// Get a value at a given index
uint8_t get_value(const std::vector<uint8_t> &arr, size_t index)
{
    size_t byte_index = index / 4;
    size_t bit_offset = (index % 4) * 2;
    return (arr[byte_index] >> bit_offset) & 0b11; // Extract 2 bits
}
// ******************************************************************************

void get_hubs(Graph *g_meta, Graph *g_inserts, MetaNode *&MN_list, int N, vector<int> *Hubs, int hubsize, int p)
//void get_hubs(Graph *g_meta, Graph *g_inserts, vector<MetaNode> *MN_list, bool*& is_hub, int N, vector<int> *Hubs, int hubsize, int p)
{
    // int h_count = 0;
    int ntcount = 0;

#pragma omp parallel for num_threads(p) schedule(guided)
    for (int mn = 0; mn < N; mn++)
    {

        if ((g_meta->in_deg[mn] > 0 || g_inserts->in_deg[mn] > 0) && (g_meta->out_deg[mn] > 0 || g_inserts->out_deg[mn] > 0))
        {
            MN_list[mn].trimmed = false;
            #pragma omp atomic
                ntcount++;
            int in_d, out_d;
            // in_d = g_meta->in_deg[mn] + g_inserts->in_deg[mn];
            // out_d = g_meta->out_deg[mn] + g_inserts->out_deg[mn];
            in_d = g_meta->in_deg[mn]; 
            out_d = g_meta->out_deg[mn];
            // if (in_d >= 2 || out_d >= 2)
            // {
            //     if (in_d >= 2 and out_d >= 2)
            if (in_d >= 100 || out_d >= 100)
            {
                if (in_d >= 5 and out_d >= 5)
                {
                    MN_list[mn].is_hub = true;
                    // is_hub[mn] = true; 
#pragma omp critical
                    Hubs->push_back(mn);
                }
            }
        }
        // else{
        //     delete MN_list[mn].Hub_info;
        // }
    }
    printf("Trimmed: %d  Not-Trimmed: %d  Hub_Count: %d \n", N-ntcount, ntcount, Hubs->size());
}

// ******************************************************************************
void clean_inserts(vector<int_int> *inserts, bool *&insert_status, bool *&trimmed, int p)
{
// loop through inserts and mark true for insert status
#pragma omp parallel for num_threads(p)
    for (int ins = 0; ins < inserts->size(); ins++)
    {
        int src = inserts->at(ins).first;
        int dest = inserts->at(ins).second;

        // check within scc
        if (src == dest)
        {
            insert_status[ins] = true;
        }
        // check if any trimmed
        if (trimmed[src] || trimmed[dest])
        {
            insert_status[ins] = true;
        }
    }
}
// ******************************************************************************

void initialize_labels(Graph *g_meta, MetaNode *&MN_list, int N, vector<int> *Hubs, int hubsize, int p)
{
/*
if not trimmed allocate the memory,
if hub then mark-itself AT propagate_up & dowon to true,
# for all members

*/
// #pragma omp parallel for num_threads(p) schedule(guided)
    for (int h = 0; h < Hubs->size(); h++)
    {

        // int in=0;
        // int out = 0;
        int current_hub = Hubs->at(h);
        // vector<uint8_t> packed_array((Hubs->size() + 3) / 4, NOT);
        // vector<int> packed_array(Hubs->size(),NOT);
        // MN_list[current_hub].Hub_info = new vector<uint8_t>((Hubs->size() + 3) / 4, NOT);
        set_value(*MN_list[current_hub].Hub_info, h, AT);
        // MN_list[current_hub].H_info[h] = AT;

        int_int range_c = g_meta->get_children(current_hub);
        int_int range_p = g_meta->get_parents(current_hub);

#pragma omp parallel for num_threads(p) schedule(guided)
        for (int out_negh = range_c.first; out_negh < range_c.second; out_negh++)
        {
            // #pragma omp atomic
            // out++;
            int current_neigh = g_meta->f_col_idx[out_negh];
            if (!MN_list[current_neigh].trimmed)
            {
                // MN_list[current_neigh].currentID = 5;
// #pragma omp critical
//                 {
//                     if (MN_list[current_neigh].Hub_info == nullptr)
//                     {
//                        MN_list[current_neigh].Hub_info = new vector<uint8_t>((Hubs->size() + 3) / 4, NOT);
//                     }
//                 }
                set_value(*MN_list[current_neigh].Hub_info, h, OUT);
                // MN_list[current_neigh].H_info[h] = OUT;
                // printf("C %d\n",current_neigh);
            }
        }

#pragma omp parallel for num_threads(p) schedule(guided)
        for (int in_negh = range_p.first; in_negh < range_p.second; in_negh++)
        {
            // #pragma omp atomic
            // in++;
            int current_neigh = g_meta->b_col_idx[in_negh];
            if (!MN_list[current_neigh].trimmed)
            {
// #pragma omp critical
//                 {
//                     if (MN_list[current_neigh].Hub_info == nullptr)
//                     {
//                         MN_list[current_neigh].Hub_info = new vector<uint8_t>((Hubs->size() + 3) / 4, NOT);
//                     }
//                 }
                set_value(*MN_list[current_neigh].Hub_info, h, IN);
                // MN_list[current_neigh].H_info[h] = IN;
                // MN_list[current_neigh].H_info[h] = IN;
            }
        }

// printf("for Hub %d total indeg = %d and outdeg = %d \n",Hubs->at(h),in,out);
   
}
}



/*
collect_labels
*/

void collect_labels(Graph *g_meta, Graph *g_inserts, MetaNode *&MN_list, int N, vector<int> *Hubs, int hubsize, int p){
    /*
    For all nodes:
        if not trimmed:
            if hub-> initialize vectors for  for info send (up and dowm)
            if indeg || outdeg > 0 in inserted graph
                 -> initialize hubinfo vector for those nodes
                 -> for all parents(request info (ID, Direction))
                 -> for all children(request info (ID, Direction))

    */
   vector<bool> affected_up(N,false); // affected so all parents should be looked up
   vector<bool> affected_down(N,false); // affected so all children should be looked 
   #pragma omp parallel for num_threads(p) schedule (guided)
   for (int mn = 0; mn<N; mn++){
    if (!MN_list[mn].trimmed){
        MN_list[mn].Hub_info = new vector<uint8_t>((Hubs->size() + 3) / 4, NOT);
        // int od = g_inserts->out_deg[mn];
        // int id = g_inserts->in_deg[mn];
        // if (od > 0) {affected_down[mn] = true;}
        // if (id > 0) {affected_up[mn] = true;}
        // if ((g_inserts->out_deg[mn] > 0) || (g_inserts->in_deg[mn]>0)){
        //     if (MN_list[mn].Hub_info == nullptr){ // allocate memory to collect the hub information
        //         MN_list[mn].Hub_info = new vector<uint8_t>((Hubs->size() + 3) / 4, NOT);
        //     }
        // if (MN_list[mn].is_hub){
        //     #pragma omp critical
        //     MN_list[mn].update_addr->push_back(1);
        // }


        // }
    }

   }



}
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
void convert_changes(Graph *g, Graph *g_meta, vector<int_int> *inserts, vector<int_int> *deletes, vector<int> *SCCx, unordered_map<int, int> *sccMAP, MetaNode *&MN_list, bool *&trimmed, bool *&insert_status, bool *&delete_status, int p)
{
    int count_i = 0;
    int count_d = 0;

#pragma omp parallel for num_threads(p) schedule(guided)
    for (int i = 0; i < inserts->size(); i++)
    {
        // Obtain src and dest of insert
        int src = inserts->at(i).first;
        int dest = inserts->at(i).second;

        //   //Convert to corresponding MetaNode or SCC
        //  int m_src = sccMAP->at(SCCx->at(src));
        //  int m_dest = sccMAP->at(SCCx->at(dest));

        // Convert to corresponding MetaNode or SCC
        int m_src = SCCx->at(src);
        int m_dest = SCCx->at(dest);

        if (m_src == m_dest)
        {
            insert_status[i] = true;
        } // inserted within same SCC

        else
        {
            if (trimmed[m_src])
            {
                //        // if(g_meta->num_childrens(m_src) == 0)
                //         {
                trimmed[m_src] = false;
                //         }
            }

            if (trimmed[m_dest])
            {
                //        // if(g_meta->num_parents(m_dest) == 0)
                //         {
                trimmed[m_dest] = false;
                //         }
            }
        }
        // RENAME THEM
        // inserts->at(i).first = m_src;
        // inserts->at(i).second = m_dest;

        // if(trimmed[m_dest] && (g_meta->num_parents(m_dest) == 0)){
        //         trimmed[m_dest] = false;
        //     }

        // Mark MetaNode trim=false if particiapting in insert
        //  if(trimmed[m_src] && (g_meta->num_childrens(m_src) == 0)){
        //          trimmed[m_src] = false;
        //      }

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
    } // end of for

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
void copy_hubInfo(int *&from, int *&to, int hubsize)
{
    for (int i = 0; i < hubsize; i++)
    {
        //
        if (from[i] == 5)
        {
            to[i] = 5;
        }
        else if (to[i] == 0)
        {
            to[i] = from[i];
        }
        else if (from[i] * to[i] < 0)
        {
            to[i] = 5;
            // printf("Changed SCC ID from CopyHubinfo\n");
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