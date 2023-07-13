#include <stdint.h>
#include <stdlib.h>

#include "mp10.h"

/*
*	Modifier:	Yu Siying
*	File name:	mp10alloc.c
*	History:	12	 Dec 2022
*				First written, implement all 4 functions
*               16   Dec 2022
*               Add NULL pointer checking for "free" functions
*	Function:	Include 4 functions: new_vertex_set, new_path, free_vertex_set, free_path
*               Allocate or free memory and initialize the struct
*/

/*
* new_vertex_set -- allocate a new vertex set
* INPUTS: none
* OUTPUS: none
* RETURN VALUE: NULL if failure, new vertex set if success
* SIDE EFFECT: none
*/
vertex_set_t*
new_vertex_set ()
{
    vertex_set_t* vs = malloc(sizeof(vertex_set_t));
    if ( NULL == vs ){ return NULL; }
    vs->id = malloc(sizeof(int32_t));
    if ( NULL == vs->id ){
        free(vs);
        return NULL;
    }
    vs->count = 0;
    vs->id_array_size = 1;
    return vs;
}

/*
* free_vertex_set -- free a vertex set and its id array.
* INPUTS: vertex set needs to be free
* OUTPUS: none
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void
free_vertex_set (vertex_set_t* vs)
{
    if ( NULL != vs ){
        if ( NULL!= vs->id ){
            free(vs->id);
        }
        free(vs);
    }
}

/*
* new_path -- allocate a new path
* INPUTS: none
* OUTPUS: none
* RETURN VALUE: NULL if failure, new vertex set if success
* SIDE EFFECT: none
*/
path_t*
new_path ()
{
    path_t* path = malloc(sizeof(path_t));
    if ( NULL == path ){ return NULL; }
    path->id = malloc(sizeof(int32_t));
    if ( NULL == path->id ){
        free(path);
        return NULL;
    }
    path->n_vertices = 0;
    return path;
}

/*
* free_path -- free a path and its id array
* INPUTS: path needs to be free
* OUTPUS: none
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void
free_path (path_t* path)
{
    if ( NULL != path ){
        if ( NULL != path->id ){
            free(path->id);
        }
        free(path);
    }
}

