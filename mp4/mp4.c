// Author: Yu Siying
// Identity: ECE220 MP4
// Last Modified: 2022/10/22

#include "mp4.h"
#include <stdio.h>

//
//  print_row -- determine which pixels are known to be black and print to the monitor
//
//  INPUT: r1,r2,r3,r4 -- the sizes of contiguous black regions in each row, 
//                        all but the first region may be size 0
//         width -- the width of the row in pixels
//  OUTPUT: print the row with 'X' (capital x) representing known black pixels, 
//          and '.' (period) representing other pixels. Ended by linefeed "\n".
//  RETRUN VALUE: 0 -- the specified regions can't fit into the given width
//                1 -- the fitting combination exists
//  SIDE EFFECT: none
//  COMMENT: the program decide the known black pixels by repersenting the 
//           result region as right_extrem & left_extrem region wise.
//

int32_t print_row (int32_t r1, int32_t r2, int32_t r3, int32_t r4, int32_t width){
    
    int32_t     bias = width;
    int64_t     common_array = 0;
    int64_t     mask = 1;
    int64_t     check[4];
    int64_t     right_extrem[4] = {0,0,0,0};
    int32_t     field[4] = {r1,r2,r3,r4};
    
    for(uint8_t i = 0; i < 4 ; i++){
        if( field[i] != 0 ) // If region size == 0, ignore it
        {
            for( int32_t num = field[i] ; num > 0 ; num-- ) // Repersent the right_extrem by bit, 1 for black and 0 for white
            {
                right_extrem[i] = (right_extrem[i]+1) << 1;
                for( uint8_t j=0 ; j<i ;j++ ){ right_extrem[j] <<= 1; }
            }
            bias -= field[i]+1;
            for( uint8_t j=0 ; j<i ; j++ ){ right_extrem[j] <<= 1; }
        }
    }
    bias += 1; // Compute the distance between left_extrem and right_extrem
    if( bias < 0 ){ return 0;} // Minimum valid length > width -> raise error

    for( uint8_t i=0 ; i<4 ; i++ )
    {
        right_extrem[i] >>= 1;
        check[i] = right_extrem[i] << bias; // Generate left_extrem
        right_extrem[i] &= check[i];
        common_array |= right_extrem[i]; // The result row
    }

    for ( uint8_t i = 0; i < width-1 ; i++ ){ mask <<= 1; } // Initialize mask to the highest bit
    for ( uint8_t i = 0; i < width ; i++ ) // Print the row as formatted
    {
        if(common_array & mask){ printf("X"); }else{ printf("."); }
        mask >>= 1;
    }
    printf("\n");

    return 1;
}