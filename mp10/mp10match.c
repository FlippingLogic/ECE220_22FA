#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp5.h"
#include "mp10.h"

/*
*	Modifier:	Yu Siying
*	File name:	mp10match.c
*	History:	12	 Dec 2022
*				First written, implement handle_request function
*	Function:	called by mp10, handle the passed single request,
*               match it with ones in available, give the pair
*               result and provide function for viewing result
*/

//
// These variables hold the heads of two singly-linked lists of 
// requests.  
//
// The avaialble list consists of unpaired requests: partner 
// should be NULL, and next is used to form the list.   
// 
// The shared list consists of groups with non-empty start and end 
// vertex intersections.  Only one of the requests in the group is in 
// the list.  The others are linked through the first's partner field 
// and then through the next field of the others in the group.  The 
// next field of the first request in a group is used to form the
// shared list.
//
// Note: for MP2, you should only build groups of two in the shared
// list.
//

static request_t* available = NULL;
static request_t* shared = NULL;

/*
* handle request -- handle the passed single request, match it with ones in available,
*                   find the shortest path, store the paried requests into shared
* INPUTS: g -- graph
*         p -- pyramid tree
*         h -- heap for dijsktra
*         r -- current request
* OUTPUS: available -- linked list with available request info updated
*         shared -- linked list with paired requests info updated
* RETURN VALUE: 1 -- handled successfully
*               0 -- allocation failed
* SIDE EFFECT: none
*/
int32_t
handle_request (graph_t* g, pyr_tree_t* p, heap_t* h, request_t* r)
{
    r->src_vs = new_vertex_set();
    if ( NULL == r->src_vs ){ return 0; }
    r->dst_vs = new_vertex_set();
    if ( NULL == r->dst_vs ){
        free_vertex_set(r->src_vs);
        return 0;
    }
    find_nodes(&(r->from),r->src_vs,p,0);
    find_nodes(&(r->to),r->dst_vs,p,0);
    if ( r->src_vs->count == 0 || r->dst_vs->count == 0 ){
        free_vertex_set(r->src_vs);
        free_vertex_set(r->dst_vs);
        return 0;
    }
    r->next = NULL;
    r->partner = NULL;
    r->path = NULL;
    build_vertex_set_minimap(g,r->src_vs);
    build_vertex_set_minimap(g,r->dst_vs);
    // allocate memory for vint & path used in next while loop
    vertex_set_t* vint_src = new_vertex_set();
    if ( NULL == vint_src ){
        free_vertex_set(r->src_vs);
        free_vertex_set(r->dst_vs);
        return 0;
    }
    vint_src->id = realloc(vint_src->id,sizeof(int32_t)*r->src_vs->count);
    if ( NULL == vint_src->id ){
        free_vertex_set(r->src_vs);
        free_vertex_set(r->dst_vs);
        free_vertex_set(vint_src);
        return 0;
    }
    vertex_set_t* vint_dst = new_vertex_set();
    if ( NULL == vint_dst ){
        free_vertex_set(r->src_vs);
        free_vertex_set(r->dst_vs);
        free_vertex_set(vint_src);
        return 0;
    }
    vint_dst->id = realloc(vint_dst->id,sizeof(int32_t)*r->dst_vs->count);
    if ( NULL == vint_dst->id ){
        free_vertex_set(r->src_vs);
        free_vertex_set(r->dst_vs);
        free_vertex_set(vint_src);
        free_vertex_set(vint_dst);
        return 0;
    }
    path_t* path = new_path(sizeof(path_t));
    if ( NULL == path ){
        free_vertex_set(r->src_vs);
        free_vertex_set(r->dst_vs);
        free_vertex_set(vint_src);
        free_vertex_set(vint_dst);
        return 0;
    }
    if ( available == NULL ){ // first request
        available = r;
        return 1;
    }
    request_t* match = available;
    request_t* prev = NULL;
    while ( NULL != match ){
        if ( ( (r->src_vs->minimap & match->src_vs->minimap) == 0) ||
             ( (r->dst_vs->minimap & match->dst_vs->minimap) == 0) ||
             merge_vertex_sets(r->src_vs,match->src_vs,vint_src) == 0 || 
             merge_vertex_sets(r->dst_vs,match->dst_vs,vint_dst) == 0 ){
            prev = match;
            match = match->next;
            continue; // if there's no common point
        }
        if ( dijkstra(g,h,vint_src,vint_dst,path) ){
            build_path_minimap(g,path);
            // delete from available
            if ( NULL == prev ){
                available = available->next;
            } else {
                prev->next = match->next;
            }
            // add to shared list
            if ( NULL == shared ){
                shared = r;
            } else {
                r->next = shared;
                shared = r;
            }
            free_vertex_set(r->src_vs);
            free_vertex_set(r->dst_vs);
            free_vertex_set(match->src_vs);
            free_vertex_set(match->dst_vs);
            build_vertex_set_minimap(g,vint_src);
            build_vertex_set_minimap(g,vint_dst);
            r->partner = match;
            r->partner->src_vs = r->src_vs = vint_src;
            r->partner->dst_vs = r->dst_vs = vint_dst;
            r->partner->path = r->path = path;
            r->partner->next = NULL;
            return 1;
        } else {
            prev = match;
            match = match->next;
            continue;
        }
    }
    free_vertex_set(vint_src);
    free_vertex_set(vint_dst);
    free_path(path);
    // add request to the front of available
    r->next = available;
    available = r;
    return 1;
}


void
print_results ()
{
    request_t* r;
    request_t* prt;

    printf ("Matched requests:\n");
    for (r = shared; NULL != r; r = r->next) {
        printf ("%5d", r->uid);
	for (prt = r->partner; NULL != prt; prt = prt->next) {
	    printf (" %5d", prt->uid);
	}
	printf (" src=%016lX dst=%016lX path=%016lX\n", r->src_vs->minimap,
		r->dst_vs->minimap, r->path->minimap);
    }

    printf ("\nUnmatched requests:\n");
    for (r = available; NULL != r; r = r->next) {
        printf ("%5d src=%016lX dst=%016lX\n", r->uid, r->src_vs->minimap,
		r->dst_vs->minimap);
    }
}


int32_t
show_results_for (graph_t* g, int32_t which)
{
    request_t* r;
    request_t* prt;

    // Can only illustrate one partner.
    for (r = shared; NULL != r; r = r->next) {
	if (which == r->uid) {
	    return show_find_results (g, r, r->partner);
	}
	for (prt = r->partner; NULL != prt; prt = prt->next) {
	    if (which == prt->uid) {
		return show_find_results (g, prt, r);
	    }
	}
    }

    for (r = available; NULL != r; r = r->next) {
        if (which == r->uid) {
	    return show_find_results (g, r, r);
	}
    }
    return 0;
}


static void
free_request (request_t* r)
{
    free_vertex_set (r->src_vs);
    free_vertex_set (r->dst_vs);
    if (NULL != r->path) {
	free_path (r->path);
    }
    free (r);
}

void
free_all_data ()
{
    request_t* r;
    request_t* prt;
    request_t* next;

    // All requests in a group share source and destination vertex sets
    // as well as a path, so we need free those elements only once.
    for (r = shared; NULL != r; r = next) {
	for (prt = r->partner; NULL != prt; prt = next) {
	    next = prt->next;
	    free (prt);
	}
	next = r->next;
	free_request (r);
    }

    for (r = available; NULL != r; r = next) {
	next = r->next;
	free_request (r);
    }
}


