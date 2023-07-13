#include <stdint.h>
#include <stdio.h>

#include "mp5.h"
#include "mp10.h"
#include <stdlib.h>

#define MY_INFINITY   1000000000
#define NOT_IN_HEAP   0

/*
*	Modifier:	Yu Siying
*	File name:	mp10alloc.c
*	History:	04	 Dec 2022
*				First written, implement find_node, trim_node and dijsktra functionality
*               16   Dec 2022
*               Update find_node and dijsktra with dynamic allocation, delete trim_node
*	Function:	Called by mp10match.c, used to find vertexs within certain range and 
*               implement dijsktra algorithm to find the shortest path
*               Include 2 main functions: find_node, dijsktra
*                   and 6 auxiliary function: heap_init, heap_sort, heap_update,
*                                             heap_swap, heap_backtrack, heap_check
*/

// clear the heap and push all starting points into heap
void heap_init(graph_t* g, heap_t* h, vertex_set_t* src);
// generate minimum tree
void heap_sort (graph_t* g, heap_t* h);
// pop the closest vertex, update distance info, push new neighbors
void heap_update (graph_t* g, heap_t* h);
// exchange heap id and heap index of given two vertexs
void heap_swap (graph_t* g, heap_t* h, int32_t heap_id_1, int32_t heap_id_2);
// backtrack swapped nodes until reaches leaf node,
// make sure all parent nodes smaller than child nodes
void heap_backtrack (graph_t* g, heap_t* h, int32_t heap_id);
// check if parent node is smaller than both child nodes,
// return -1 if no need to swap, return 0 to swap n with 2*n,
// return 1 to swap n with 2*n+1
int32_t heap_check (graph_t* g, heap_t* h, int32_t n);

/*
* find_nodes -- starting at array index nnum of a pyramid tree, find vertices 
*               within range of a locale and write their ids into a vertex set
* INPUTS: p -- pyramid tree
*         loc -- locale within distance from (x,y)
*         nnum -- starting index of pyramid tree
* OUTPUTS: vs -- vertex set of array indices
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void
find_nodes (locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, int32_t nnum)
{
    // Do not modify the following line nor add anything above in the function.
    record_fn_call ();

    if ( nnum < p->n_nodes ){
        int32_t x_loc = loc->x;
        int32_t y_loc = loc->y;
        int32_t x = p->node[nnum].x;
        int32_t y_left = p->node[nnum].y_left;
        int32_t y_right = p->node[nnum].y_right;
        if ( 4*nnum+1>=p->n_nodes ){   
            // leaf node
            if ( in_range(loc,x,y_left) ){
                if ( vs->count >= vs->id_array_size ){
                    int32_t* buffer = realloc(vs->id,2*vs->id_array_size*sizeof(int32_t));
                    if ( NULL == buffer ){ return;
                    } else {
                        vs->id = buffer;
                        vs->id_array_size *= 2;
                    }
                }
                int32_t i = ++vs->count-1;
                while( (vs->id[i-1])>(p->node[nnum].id) && i>0 ){
                    vs->id[i] = vs->id[i-1];
                    i--;
                } // insert vertex id in increasing order
                vs->id[i] = p->node[nnum].id;
            }
        } else {
            // internal node
            int32_t check[5] = { 0, 1, 1, 1, 1 }; // check[0] is a placeholder
            // entire locale on the left of x: no 4N+1 & 4N+3; on the right: no 4N+2 & 4N+4
            if ( !in_range(loc,x,y_loc) ){
                (x-x_loc<0) ? (check[1]=check[3]=0) : (check[2]=check[4]=0); }
            // entire locale below y_left: no 4N+1; above y_left: no 4N+3
            if ( !in_range(loc,x_loc,y_left) ){
                (y_left-y_loc<0) ? (check[1]=0) : (check[3]=0); }
            // entire locale below y_right: no 4N+2; above y_right: no 4N+4
            if ( !in_range(loc,x_loc,y_right) ){
                (y_right-y_loc<0) ? (check[2]=0) : (check[4]=0); }
            for ( int32_t i = 1; i < 5; i++ ){
                if ( check[i] ){ find_nodes(loc,vs,p,4*nnum+i); }
            }
        }
    }

}

/*
* dijkstra -- find the shortest path between any node in the src vertex set and any
*             node in the destination vertex set, and write that path into path
* INPUS: g -- graph
*        h -- blank heap
*        src -- starting vertex set
*        dest -- destination vertex set
* OUTPUTS: path -- array of the shortest path, consisting of vertex id, between any node 
*                  in the src vertex set and any node in the destination vertex set
* RETURN VALUE: 0 -- Failure
*               1 -- Success
* SIDE EFFECT: none
*/
int32_t
dijkstra (graph_t* g, heap_t* h, vertex_set_t* src, vertex_set_t* dest,
          path_t* path)
{
    if ( (src->count==0) || (dest->count==0) ){ return 0; }
    // Dijkstra Algorithm with heap optimization
    for ( int32_t i = 0; i < g->n_vertices; i++ ){
        // initialize vertex info
        g->vertex[i].from_src = MY_INFINITY;
        g->vertex[i].heap_id = NOT_IN_HEAP;
        // actual heap id starts from 1, 0 for unvisited, -1 for visited
    }
    for ( int32_t i = 0; i < src->count; i++ ){
        // initialize starting vertex info
        g->vertex[ src->id[i] ].from_src = 0;
        g->vertex[ src->id[i] ].pred = -1;
        // -1: A imaginary mutual pred for all starting points
    }
    heap_init(g,h,src);
    while ( h->n_elts > 0 ){ // run dijkstra until heap is empty
        heap_sort(g,h);
        heap_update(g,h);
    }
    // find closest destination vertex
    int32_t path_node = dest->id[0];
    for ( int32_t i = 1; i < dest->count; i++ ){
        if ( g->vertex[path_node].from_src > g->vertex[dest->id[i]].from_src ){
            path_node = dest->id[i];
        } else if ( g->vertex[path_node].from_src == g->vertex[dest->id[i]].from_src &&
                    g->vertex[path_node].from_src != MY_INFINITY && path_node < dest->id[i] ){
            // if two vertexs have the same from_id, choose the one with larger id 
            path_node = dest->id[i];
        }
    }
    if ( g->vertex[ path_node ].from_src == MY_INFINITY ){ return 0; } // no such path exists
    // generate path
    int32_t reverse_array_size = 1;
    int32_t* reverse_path = malloc(reverse_array_size*sizeof(int32_t));
    path->n_vertices = 0;
    while( path_node != -1 ){ // loop until hits the starting points' mutual pred
        if ( path->n_vertices == reverse_array_size ){
            int32_t* buffer = realloc(reverse_path,2*reverse_array_size*sizeof(int32_t));
            if ( NULL == buffer ){ return 0; }
            reverse_path = buffer;
            reverse_array_size *= 2;
        }
        reverse_path[ path->n_vertices ] = path_node;
        path_node = g->vertex[ path_node ].pred;
        path->n_vertices++;
    }
    path->id = malloc(path->n_vertices*sizeof(int32_t));
    if ( NULL == path->id ){ return 0; }
    for ( int32_t i = 0; i < path->n_vertices; i++ ){
        path->id[i] = reverse_path[ path->n_vertices-1-i ];
    }
    if ( NULL != reverse_path ){ free(reverse_path); }
    return 1;
}

/*
* heap_init -- clear the heap and push all starting points into heap
* INPUTS: g -- graph
*         h -- blank heap
*         src -- starting vertex set
* OUTPUTS: h -- initialized heap
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void
heap_init(graph_t* g, heap_t* h, vertex_set_t* src){
    h->n_elts = 0;
    for ( int32_t i = 0; i < src->count; i++ ){
        h->n_elts++;
        h->elt[h->n_elts] = src->id[i];
        g->vertex[src->id[i]].heap_id = h->n_elts;
    }
}

/*
* heap_sort -- generate minimum tree
* INPUTS: g -- graph
*         h -- heap (of vertex id)
* OUTPUTS: h -- min-heap
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void
heap_sort (graph_t* g, heap_t* h){
    int32_t last_parent;
    ( h->n_elts - 2*(h->n_elts/2) == 0) ? (last_parent = h->n_elts/2) : (last_parent = (h->n_elts-1)/2);
    for ( int32_t n = last_parent; n > 0; n-- ){
        int32_t check = heap_check(g,h,n);
        if ( check != -1 ){
            heap_swap(g,h,n,2*n+check);
            heap_backtrack(g,h,2*n+check);
        }
    }
}

/*
* heap_update -- pop the closest vertex, update distance info, push new neighbors
* INPUTS: g -- graph
*         h -- min-heap
* OUTPUTS: g -- graph with neighbor vertex info updated
*          h -- heap with cloest vertex poped and new neighbor vertex pushed
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void
heap_update (graph_t* g, heap_t* h){
    // Update neighbor distance
    vertex_t vertex_pop = g->vertex[ h->elt[1] ];
    for ( int32_t i = 0; i < vertex_pop.n_neighbors; i++ ){
        // update neighbor distance
        int32_t dist = vertex_pop.distance[i] + vertex_pop.from_src;
        if ( dist < g->vertex[ vertex_pop.neighbor[i] ].from_src ){
            if ( g->vertex[ vertex_pop.neighbor[i] ].heap_id == NOT_IN_HEAP ){
                // Not in heap yet? push     
                h->n_elts++;
                h->elt[h->n_elts] = vertex_pop.neighbor[i];
                g->vertex[ vertex_pop.neighbor[i] ].heap_id = h->n_elts;
            }
            g->vertex[ vertex_pop.neighbor[i] ].from_src = dist;
            g->vertex[ vertex_pop.neighbor[i] ].pred = h->elt[1];
        }
    }
    // Pop the cloeset vertex
    heap_swap(g,h,1,h->n_elts); // Swap with the last vertex
    h->n_elts--;
    g->vertex[ h->elt[h->n_elts+1] ].heap_id = NOT_IN_HEAP; 
}

/*
* heap_swap -- exchange heap id and heap index of given two vertexs
* INPUTS: g -- graph
*         h -- heap
*         heap_id_1 -- heap array index for the first vertex to be swapped
*         heap_id_2 -- heap array index for the second vertex to be swapped
* OUTPUTS: g -- graph with target vertexs' heap_id swapped
*          h -- heap with target vertexs' index swapped
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void
heap_swap (graph_t* g, heap_t* h, int32_t heap_id_1, int32_t heap_id_2){
    int32_t temp;
    // swap vertex id in heap
    temp = h->elt[heap_id_1];
    h->elt[heap_id_1] = h->elt[heap_id_2];
    h->elt[heap_id_2] = temp;
    // swap heap_id of vertex
    g->vertex[ h->elt[heap_id_1] ].heap_id = heap_id_1;
    g->vertex[ h->elt[heap_id_2] ].heap_id = heap_id_2;
}

/*
* heap_backtrack -- backtrack swapped nodes until reaches leaf node,
*                   make sure all parent nodes smaller than child nodes
* INPUTS: g -- graph
*         h -- heap
*         n -- heap array index of the node (serve as parent node)
* OUTPUTS: h -- backtracked heap
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void
heap_backtrack (graph_t* g, heap_t* h, int32_t n){
    if ( 2*n <= h->n_elts ){
        int32_t check = heap_check(g,h,n);
        if ( check != -1 ){
            heap_swap(g,h,n,2*n+check);
            heap_backtrack(g,h,2*n+check);
        }
    }
}

/*
* heap_check -- check if parent node is smaller than both child nodes,
*               if no, check which child node should parent node swap with
* INPUTS: g -- graph
*         h -- heap
*         n -- heap array index of the node (serve as parent node)
* OUTPUTS: none
* RETURN VALUE: -1 -- No need to swap
*               0 -- swap n with 2*n
*               1 -- swap n with 2*n+1
* SIDE EFFECT: none
*/
int32_t
heap_check(graph_t* g, heap_t* h, int32_t n){
    if ( 2*n+1 <= h->n_elts ){
        if ( (g->vertex[ h->elt[n] ].from_src > g->vertex[ h->elt[2*n] ].from_src) ||
            (g->vertex[ h->elt[n] ].from_src > g->vertex[ h->elt[2*n+1] ].from_src) ){
            if ( g->vertex[ h->elt[2*n] ].from_src < g->vertex[ h->elt[2*n+1] ].from_src ){
                return 0;
            } else { return 1; }
        } else { return -1; }
    } else {
        if ( (g->vertex[ h->elt[n] ].from_src > g->vertex[ h->elt[2*n] ].from_src) ){
            return 0;
        } else { return -1; }
    }
}