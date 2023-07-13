/*									tab:8
 *
 * main.c - skeleton source file for ECE220 picture drawing program
 *
 * "Copyright (c) 2018 by Charles H. Zega, and Saransh Sinha."
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO 
 * ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
 * EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE 
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
 * THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE, 
 * SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Author:	    Charles Zega, Saransh Sinha
 * Version:	    1
 * Creation Date:   12 February 2018
 * Filename:	    mp5.h
 * History:
 *	CZ	1	12 February 2018
 *		First written.
 */
#include "mp5.h"

/*
	You must write all your code only in this file, for all the functions!
*/


//
// File info
// Modifier:	       Yu Siying
// Last modified date: 1 November 2022
// Function: 	       Include function used to draw shapes, such as lines.
//		       triangles, rectangles and circles.
//


/* 
 *  near_horizontal
 *	
 * INPUTS: x_start,y_start -- the coordinates of the pixel at one end of the line
 * 	   x_end, y_end    -- the coordinates of the pixel at the other end
 * OUTPUTS: draws a pixel to all points in between the two given pixels including
 *          the end points
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
near_horizontal(int32_t x_start, int32_t y_start, int32_t x_end, int32_t y_end){
	int32_t y, flag = 1, sgn_y = 0, sgn_x = 0;
	int32_t dx = x_end - x_start;
	int32_t dy = y_end - y_start;
	(dy > 0) ? (sgn_y = 1) : (sgn_y = -1);
	(dx > 0) ? (sgn_x = 1) : (sgn_x = -1);
	for ( int32_t x = x_start ; x != x_end ; x += sgn_x ){
		y = (2*dy*(x-x_start)+sgn_y*dx)/(2*dx) + y_start;
		flag &= draw_dot(x,y);
	}
	return flag;
}


/* 
 *  near_vertical
 *	 
 * INPUTS: x_start,y_start -- the coordinates of the pixel at one end of the line
 * 	   x_end, y_end    -- the coordinates of the pixel at the other end
 * OUTPUTS: draws a pixel to all points in between the two given pixels including
 *          the end points
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
near_vertical(int32_t x_start, int32_t y_start, int32_t x_end, int32_t y_end){
	int32_t flag = 1;
	if ( x_start == x_end && y_start == y_end ){ flag = 0; }	// If two points are identical, draw nothing
	else{
		int32_t x, sgn_y = 0, sgn_x = 0;
		int32_t dx = x_end - x_start;
		int32_t dy = y_end - y_start;
		(dx > 0) ? (sgn_x = 1) : (sgn_x = -1);
		(dy > 0) ? (sgn_y = 1) : (sgn_y = -1);
		for ( int32_t y = y_start ; y != y_end; y += sgn_y ){
			x = ( 2*dx*(y-y_start) + sgn_x*dy ) / (2*dy) + x_start;
			flag &= draw_dot(x,y);
		}
	}
	return flag;
}

/* 
 *  draw_line
 *	 
 * INPUTS: x_start,y_start -- the coordinates of the pixel at one end of the line
 * 	   x_end, y_end    -- the coordinates of the pixel at the other end
 * OUTPUTS: draws a pixel to all points in between the two given pixels including
 *          the end points
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_line(int32_t x_start, int32_t y_start, int32_t x_end, int32_t y_end){
	int32_t flag;
	if ( x_start == x_end ){ flag = near_vertical(x_start,y_start,x_end,y_end); }
	else{
		int32_t slope = (y_end-y_start) / (x_end-x_start);
		if( -1<=slope && slope<=1 ){ flag = near_horizontal(x_start,y_start,x_end,y_end); }
		else { flag = near_vertical(x_start,y_start,x_end,y_end); }
	}
	return flag;
}


/* 
 *  draw_rect
 *	 
 * INPUTS: x,y -- the coordinates of the of the top-left pixel of the rectangle
 *         w,h -- the width and height, respectively, of the rectangle
 * OUTPUTS: draws a pixel to every point of the edges of the rectangle
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_rect(int32_t x, int32_t y, int32_t w, int32_t h){
	int32_t flag = 1;
	if( w>=0 && h>=0  ){
		if( w==0 && h!=0 ){ 
			draw_line(x,y,x,y+h); 
			flag = 0;
		}
		else if ( w!=0 && h==0 ){
			draw_line(x,y,x+w,y);
			flag = 0;
		}
		else if ( w==0 && h==0 ){ flag = 0; }
		else{
			flag &= draw_line(x,y,x,y+h);
			flag &= draw_line(x,y,x+w,y);
			flag &= draw_line(x+w,y,x+w,y+h);
			flag &= draw_line(x,y+h,x+w,y+h);
		}
	}
	else{ flag = 0; }
	return flag;
}


/* 
 *  draw_triangle
 *	
 * INPUTS: x_A,y_A -- the coordinates of one of the vertices of the triangle
 *         x_B,y_B -- the coordinates of another of the vertices of the triangle
 *         x_C,y_C -- the coordinates of the final of the vertices of the triangle
 * OUTPUTS: draws a pixel to every point of the edges of the triangle
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_triangle(int32_t x_A, int32_t y_A, int32_t x_B, int32_t y_B, int32_t x_C, int32_t y_C){
	int32_t flag = 1;
	flag &= draw_line(x_A,y_A,x_B,y_B);
	flag &= draw_line(x_B,y_B,x_C,y_C);
	flag &= draw_line(x_C,y_C,x_A,y_A);
	return flag;
}

/* 
 *  draw_parallelogram
 *	 
 * INPUTS: x_A,y_A -- the coordinates of one of the vertices of the parallelogram
 *         x_B,y_B -- the coordinates of another of the vertices of the parallelogram
 *         x_C,y_C -- the coordinates of another of the vertices of the parallelogram
 * OUTPUTS: draws a pixel to every point of the edges of the parallelogram
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_parallelogram(int32_t x_A, int32_t y_A, int32_t x_B, int32_t y_B, int32_t x_C, int32_t y_C){
	int32_t flag = 1;
	int32_t x_D = x_C - (x_B - x_A) ;	// Decide the fourth point
	int32_t y_D = y_A - (y_B - y_C) ;
	flag &= draw_line(x_A,y_A,x_B,y_B);
	flag &= draw_line(x_B,y_B,x_C,y_C);	
	flag &= draw_line(x_C,y_C,x_D,y_D);
	flag &= draw_line(x_D,y_D,x_A,y_A);
	return flag;
}


/* 
 *  draw_circle
 *	 
 * INPUTS: x,y -- the center of the circle
 *         inner_r,outer_r -- the inner and outer radius of the circle
 * OUTPUTS: draws a pixel to every point whose distance from the center is
 * 	    greater than or equal to inner_r and less than or equal to outer_r
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
draw_circle(int32_t x, int32_t y, int32_t inner_r, int32_t outer_r){
	int32_t flag = 1;
	if ( inner_r>=0 && outer_r>=inner_r ){
		for ( int32_t x1 = x-outer_r; x1 <= x+outer_r; x1++){
			for ( int32_t y1 = y-outer_r; y1 <= y+outer_r; y1++){
				int32_t dis = (x1-x)*(x1-x) + (y1-y)*(y1-y);
				if ( (dis <= outer_r*outer_r) && (dis >= inner_r*inner_r) ){ flag &= draw_dot(x1,y1);}
			}
		}		
	}
	else{ flag = 0; }
	return flag;
}


/* 
 *  rect_gradient
 *	 
 * INPUTS: x,y -- the coordinates of the of the top-left pixel of the rectangle
 *         w,h -- the width and height, respectively, of the rectangle
 *         start_color -- the color of the far left side of the rectangle
 *         end_color -- the color of the far right side of the rectangle
 * OUTPUTS: fills every pixel within the bounds of the rectangle with a color
 *	    based on its position within the rectangle and the difference in
 *          color between start_color and end_color
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */

int32_t
rect_gradient(int32_t x, int32_t y, int32_t w, int32_t h, int32_t start_color, int32_t end_color){
	int32_t flag = 1;
	if ( h>=0 && w>=1 ){
		int32_t color, start_level, end_level, d_level;
		int32_t level[3];							// Store computed level
		int32_t mask[3] = {0x00FF0000, 0x0000FF00, 0x000000FF};			// mask for RGB
		int32_t bias[3] = {16,8,0};						// Bias from bit[0] for RGB
		for ( int32_t x1 = x; x1 <= x+w ; x1++ ){
			for ( int32_t i = 0; i < 3; i++ ){
				start_level = (start_color & mask[i]) >> bias[i];	// Recover each level's value
				end_level = (end_color & mask[i]) >> bias[i];
				d_level = end_level - start_level;
				int32_t sgn = 0;
				( d_level>0 ) ? ( sgn=1 ) : ( sgn=-1 );
				// Compute and move back to the level's orignal position
				level[i] = ((2*(x1-x)*d_level - w*sgn) / (2*w) + start_level) << bias[i];	
			}
			color = level[0] | level[1] | level[2];				// Put three colors together
			set_color(color);
			for ( int32_t y1 = y; y1 <= y+h ; y1++ ){ flag &= draw_dot(x1,y1); }
		}
	}
	else{ flag = 0; }
	return flag;
}


/* 
 *  draw_picture
 *	 
 * INPUTS: none
 * OUTPUTS: alters the image by calling any of the other functions in the file
 * RETURN VALUE: 0 if any of the pixels drawn are out of bounds, otherwise 1
 * SIDE EFFECTS: none
 */


int32_t
draw_picture(){

	// Draw a combination of strong "Seeking truth eagle" and UIUC's logo "I"
	// The output figure is transposed

	int32_t ZJU_BLUE    = 0x00003F88;
	int32_t UIUC_ORANGE = 0x00E84A27;
	int32_t middle = HEIGHT/2;
	int32_t bias = 170;

	set_color(ZJU_BLUE);
	rect_gradient(70+bias,middle-70,30,140,ZJU_BLUE,ZJU_BLUE);					// Shoulder
	draw_circle(45+bias,middle-65,0,10);								// Left hand
	draw_circle(48+bias,middle-71,0,9);
	rect_gradient(50+bias,middle-80,40,15,ZJU_BLUE,ZJU_BLUE);			
	draw_circle(88+bias,middle-68,0,12);								// Left joint
	draw_circle(75+bias,middle-55,0,15);								// Left muscle
	draw_circle(76+bias,middle-37,0,12);
	draw_circle(45+bias,middle+65,0,10);								// Right hand
	draw_circle(48+bias,middle+71,0,9);
	rect_gradient(50+bias,middle+65,40,15,ZJU_BLUE,ZJU_BLUE);
	draw_circle(88+bias,middle+68,0,12);								// Right joint
	draw_circle(75+bias,middle+55,0,15);								// Right muscle
	draw_circle(76+bias,middle+37,0,12);
	draw_circle(73+bias,middle,0,20);								// Neck
	draw_circle(60+bias,middle,0,16);		
	draw_circle(48+bias,middle,0,14);	

	int32_t height = 17;										// Eagle mouth
	int32_t y0 = 35+bias;
	for (int32_t i = middle; i >= middle-30; i-- ){
		int32_t h = height - height*(middle-i+1)/30;
		int32_t y = y0 + height*(middle-i+1)/60;
		rect_gradient(y,i,h,1,ZJU_BLUE,ZJU_BLUE);}
		
	rect_gradient(100+bias,middle-20,84,40,ZJU_BLUE,UIUC_ORANGE);		// Middle line of "I"

	set_color(UIUC_ORANGE);
	rect_gradient(184+bias,middle-67,36,134,UIUC_ORANGE,UIUC_ORANGE);	// The bottom line of "I"
	draw_circle(202+bias,middle-67,0,18);
	draw_circle(202+bias,middle+67,0,18);
	set_color(0x00FFFFFF);							// clear area between muscle and neck
	draw_circle(68+bias,middle-24,0,3);
	draw_circle(68+bias,middle+24,0,3);
	return 0;
}
