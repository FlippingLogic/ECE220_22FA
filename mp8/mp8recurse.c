#include <stdlib.h>
#include <string.h>

#include "mp8.h"

/*
*	Modifier:	Yu Siying
*	File name:	mp8recurse.c
*	History:	1	20 Nov 2022
*				First written, implement all 4 functions
*	Function:	Include 3 recursive functions called by functions in mp8.c
*				All 3 function mark points satisfing specific condition in
*					the marking array by recursively calling itself.
*/

/*
 * basicRecurse -- flood fill recursively from a point until reaching
 *                 white or the image border
 * INPUTS: width -- width of the input image
 *         height -- height of the input image
 *         inRed -- pointer to the input red channel (1-D array)
 *         inGreen -- pointer to the input green channel (1-D array)
 *         inBlue -- pointer to the input blue channel (1-D array)
 *         x -- current x position of the flood
 *         y -- current y position of the flood
 *         marking -- array used to mark seen positions (indexed
 *                    in the same way as the RGB input arrays)
 * OUTPUTS: marking -- marked with flooded pixels
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
basicRecurse (int32_t width, int32_t height,
	      const uint8_t* inRed, const uint8_t* inGreen, 
	      const uint8_t* inBlue, 
	      int32_t x, int32_t y, 
	      uint8_t* marking)
{
	int32_t move[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
	for ( int32_t i = 0; i<4 ; i++ ){	// Loop its upper, right, down, left pixels
		int32_t x1 = x+move[i][0];
		int32_t y1 = y+move[i][1];
		int32_t pos = x1 + y1*width;
		// Judging condition: image boundary & not visited & the color is not white
		if ( 0<=x1 && x1<width && 0<=y1 && y1<height && marking[pos]==0 &&
			 (inRed[pos]!=0xFF || inGreen[pos]!=0xFF || inBlue[pos]!=0xFF) ){
			marking[pos]=1;
			basicRecurse(width,height,inRed,inGreen,inBlue,x1,y1,marking);
		}
	}
}


/*
 * greyRecurse -- flood fill recursively from a point until reaching
 *                near-white pixels or the image border
 * INPUTS: width -- width of the input image
 *         height -- height of the input image
 *         inRed -- pointer to the input red channel (1-D array)
 *         inGreen -- pointer to the input green channel (1-D array)
 *         inBlue -- pointer to the input blue channel (1-D array)
 *         x -- current x position of the flood
 *         y -- current y position of the flood
 *         distSq -- maximum distance squared between white and boundary
 *                   pixel color
 *         marking -- array used to mark seen positions (indexed
 *                    in the same way as the RGB input arrays)
 * OUTPUTS: marking -- marked with flooded pixels
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
greyRecurse (int32_t width, int32_t height,
	     const uint8_t* inRed, const uint8_t* inGreen, 
	     const uint8_t* inBlue, 
	     int32_t x, int32_t y, uint32_t distSq, 
	     uint8_t* marking)
{
	int32_t move[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
	for ( int32_t i = 0; i<4 ; i++ ){	// Loop its upper, right, down, left pixels
		int32_t x1 = x+move[i][0];
		int32_t y1 = y+move[i][1];
		int32_t pos = x1 + y1*width;
		// Judging condition: image boundary & not visited & color not within the distance with white
		if ( 0<=x1 && x1<width && 0<=y1 && y1<height && marking[pos]==0 && 
			!colorsWithinDistSq(0xFF,0xFF,0xFF,inRed[pos],inGreen[pos],inBlue[pos],distSq)){							// If it's not checked
				marking[pos]=1;
				greyRecurse(width,height,inRed,inGreen,inBlue,x1,y1,distSq,marking);
		}
	}
}


/*
 * limitedRecurse -- flood fill recursively from a point until reaching
 *                   pixels too different (in RGB color) from the color at
 *                   the flood start point, too far away (> 35 pixels), or
 *                   beyond the image border
 * INPUTS: width -- width of the input image
 *         height -- height of the input image
 *         inRed -- pointer to the input red channel (1-D array)
 *         inGreen -- pointer to the input green channel (1-D array)
 *         inBlue -- pointer to the input blue channel (1-D array)
 *         origX -- starting x position of the flood
 *         origY -- starting y position of the flood
 *         x -- current x position of the flood
 *         y -- current y position of the flood
 *         distSq -- maximum distance squared between pixel at origin 
 *                   and boundary pixel color
 *         marking -- array used to mark seen positions (indexed
 *                    in the same way as the RGB input arrays)
 * OUTPUTS: marking -- marked with flooded pixels
 * RETURN VALUE: none
 * SIDE EFFECTS: none
 */
void 
limitedRecurse (int32_t width, int32_t height,
	        const uint8_t* inRed, const uint8_t* inGreen, 
	        const uint8_t* inBlue, 
	        int32_t origX, int32_t origY, int32_t x, int32_t y, 
			uint32_t distSq, uint8_t* marking)
{
	int32_t move[4][2] = {{0,-1},{1,0},{0,1},{-1,0}};
	int32_t  posOrig = origX + origY*width;
	for ( int32_t i = 0; i<4 ; i++ ){	// Loop its upper, right, down, left pixels
		int32_t x1 = x+move[i][0];
		int32_t y1 = y+move[i][1];
		int32_t posNow = x1 + y1*width;
		int32_t distPixel = (x-origX)*(x-origX) + (y-origY)*(y-origY);
		// Judging condition: no further than 35 pixels & image boundary & color close enough to orignal color
		if ( distPixel<35*35 && 0<=x1 && x1<width && 0<=y1 && y1<height && marking[posNow]==0 && 
		colorsWithinDistSq(inRed[posOrig],inGreen[posOrig],inBlue[posOrig],inRed[posNow],inGreen[posNow],inBlue[posNow],distSq)){							// If it's not checked
			marking[posNow]=1;
			limitedRecurse(width,height,inRed,inGreen,inBlue,origX,origY,x1,y1,distSq,marking);
		}
	}
}

