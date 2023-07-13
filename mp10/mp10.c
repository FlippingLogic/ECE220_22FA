#include <stdint.h>
#include <stdio.h>

#include "mp10.h"

/*
*	Modifier:	Yu Siying
*	File name:	mp10.c
*	History:	12	 Dec 2022
*				First written, implement 4 functions, add 1 auxiliary function
*	Function:   called by mp10match.c,
*               Used to mark mm_bit, build minimap and merging vertex sets
*               Include 4 main functions: 
                        mark_vertex_minimap, build_vertex_set_minimap,
*                       build_path_minimap, mergex_vertex_sets
*                   and 1 auxiliary function: mark_minimap
*/

/*
* mark_mmbit -- mark leaf nodes' mm_bit by the (nnum-21) of internodes 
*               on the fourth layer of the tree
* INUPUTS: g -- graph
*          p -- pyramid tree
*          nnum -- index of nodes in pyramid tree
*          layer -- mark the layer current node is in (index begin with 1)
*          ancestor -- record the nnum of internode in 4th
* OUTPUTS: g -- graph with vertexes marked with mm_bit
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void
mark_mmbit(graph_t* g, pyr_tree_t* p, int32_t nnum, int32_t layer, int32_t ancestor){
    if ( nnum < p->n_nodes ){
        if ( 4*nnum+1 >= p->n_nodes ){
            g->vertex[ p->node[nnum].id ].mm_bit = ancestor-21;
        } else {
            if ( layer == 4 ){ ancestor = nnum; }
            mark_mmbit(g,p,4*nnum+1,layer+1,ancestor);
            mark_mmbit(g,p,4*nnum+2,layer+1,ancestor);
            mark_mmbit(g,p,4*nnum+3,layer+1,ancestor);
            mark_mmbit(g,p,4*nnum+4,layer+1,ancestor);
        }
    }
}

/* 
* mark_vertex_minimap -- marks each graph vertex in g with a minimap 
*                        bit number based on the pyramid tree p.
* INPUTS: g -- graph
*         p -- pyramid tree
* OUTPUTS: g -- graph with vertexes marked with mm_bit
* RETURN VALUE: none
* SIDE EFFECT: none
*/
int32_t 
mark_vertex_minimap (graph_t* g, pyr_tree_t* p)
{
    if ( g->n_vertices <=64 ){
        for ( int32_t i = 0; i < g->n_vertices; i++ ){
            g->vertex[i].mm_bit = i;
        }
    } else {
        int32_t init_nnum = 0;
        int32_t init_layer = 1;
        int32_t init_ances = 0;
        mark_mmbit(g,p,init_nnum,init_layer,init_ances);
    }
    return 1;
}

/* 
* build_vertex_set_minnimap -- build minimap for vertex set by ORing the
*                              mm_bit of vertexs in its id field together
* INPUTS: g -- graph
*         vs -- vertex set whose minimap needs to be built
* OUTPUTS: vs -- vertex set with vs->minimap filled
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void 
build_vertex_set_minimap (graph_t* g, vertex_set_t* vs)
{
    vs->minimap = 0;
    for ( int32_t i = 0; i < vs->count; i++ ){
        vs->minimap |= ( 1ULL << g->vertex[ vs->id[i] ].mm_bit );
    }
}

/* 
* build_path_minnimap -- build minimap for path by ORing the mm_bit
*                        of vertexs in its id field together
* INPUTS: g -- graph
*         path -- path whose minimap needs to be built
* OUTPUTS: path -- vpath with path->minimap filled
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void 
build_path_minimap (graph_t* g, path_t* p)
{
    p->minimap = 0;
    for ( int32_t i = 0; i < p->n_vertices; i++ ){
        p->minimap |= ( 1ULL << g->vertex[ p->id[i] ].mm_bit );
    }
}

/* 
* merge_vertex_sets -- find common id of v1->id and v2->id, and fill
*                      them into vint->id
* INPUTS: v1 -- first vertex set
*         v2 -- second vertex set
*         vint -- vertex set to contain merged id info
* OUTPUTS: vint -- vertex set with vint->id filled with common id in v1 and v2
* RETURN VALUE: 0 -- no common point found
*               1 -- merged successfully
* SIDE EFFECT: none
*/
int32_t
merge_vertex_sets (const vertex_set_t* v1, const vertex_set_t* v2,
		   vertex_set_t* vint)
{
    int32_t start = 0;
    vint->count = 0;
    for ( int32_t i = 0; i < v1->count; i++ ){
        if ( v1->id[i] > v2->id[v2->count-1] ){ break; }
        for ( int32_t j = start; j < v2->count; j++ ){
            if ( v1->id[i] < v2->id[j] ){ break; }
            if ( v1->id[i] == v2->id[j] ){
                start = j;
                vint->id[ vint->count ] = v1->id[i];
                vint->count++;
                break;
            }
        }
    }
    if ( vint->count != 0 ){ return 1; } else { return 0; }
}