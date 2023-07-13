#include <stdint.h>
#include <stdio.h>

#include "mp5.h"
#include "mp9.h"

/*
* match_requests -- find graph vertices within range of the starting and ending locales
*					for two requests and find a shortest path between any vertex in the
*					starting set and any vertex in the ending set. 
* INPUTS: g -- graph
*		  p -- pyramid tree
*		  h -- blank heap for Dijkstra's algorithm
*		  r1 -- request1
*		  r2 -- request2
*		  src_vs -- vertices within range of starting point for both requests
*		  dst_vs -- vertices within range of ending point for both requests
* OUTPUTS: path -- shortest forward path from initial to final nodes
* RETURN VALUE: 0 -- Failure
*				1 -- Success
* SIDE EFFECTS: none
*/
int32_t
match_requests (graph_t* g, pyr_tree_t* p, heap_t* h,
		request_t* r1, request_t* r2,
		vertex_set_t* src_vs, vertex_set_t* dst_vs, path_t* path)
{
	src_vs->count = 0;
	dst_vs->count = 0;
	find_nodes(&(r1->from),src_vs,p,0);
	find_nodes(&(r1->to),dst_vs,p,0);
	trim_nodes(g,src_vs,&(r2->from));
	trim_nodes(g,dst_vs,&(r2->to));
	if ( 1 != dijkstra(g,h,src_vs,dst_vs,path) ){ return 0; }
	return 1;
}