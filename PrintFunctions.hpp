
#ifndef PrintFunctions_HPP
#define PrintFunctions_HPP

#include<iostream>
#include<vector>
#include <unordered_map>
#include <map>
#include <stack>
#include <unordered_set>
#include "DataStructure.hpp"
#include <cstdio>
#include<string.h>

using namespace std;
typedef pair<int,int> int_int;


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
void print_status(bool*& status, vector<int_int>* inserts){
    for (int i = 0 ; i<inserts->size(); i++){
        if (status[i]){
            //color("green");
            printf("Insert (%d->%d) Completed\n",inserts->at(i).first,inserts->at(i).second);
            //color("reset");
        }
        else{
            //color("red");
            printf("Insert (%d->%d) Not - Completed\n",inserts->at(i).first,inserts->at(i).second);
            //color("reset");
        }
    }
}
// ******************************************************************************



// ******************************************************************************
void print_Hub_info(MetaNode*& MN_List,int node, int hubsize)
{
    printf("\n");
    for (int i = 0; i<hubsize; i++)
    {
        printf("(%d : %d) , ",i,MN_List[node].Hub_info[i]);
    }
    printf("\n");
}
// ******************************************************************************



// ******************************************************************************
/*void print_meta_network(Graph* g,MetaNode*& MN_List, int graph_size,int*& Hubs, int hubsize)
{
    for (int i = 0; i<graph_size; i++)
    {
        //color("yellow");
        printf("\nNode: %d ",i);
               if(MN_List[i].trimmed){
            //color("red");
            printf(" TRIMMED!!!\n");
            //color("reset");
        }
        if(!MN_List[i].trimmed){
            //color("green");
            printf("  NOT TRIMMED!!!\n");
            //color("reset");
        }
        MN_List[i].is_hub? printf(" Hub %d\n",MN_List[i].h_idx):printf("Not Hub\n");
        printf("Current SCC ID: %d \n", MN_List[i].currentID);

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
*/
// ******************************************************************************
#endif
