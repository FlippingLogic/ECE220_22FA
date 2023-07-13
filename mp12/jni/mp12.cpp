//									tab:8
//
// mp12.cpp - source file for Spring 2018 MP12 student code
//
// "Copyright (c) 2018 by Steven S. Lumetta."
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose, without fee, and without written agreement is
// hereby granted, provided that the above copyright notice and the following
// two paragraphs appear in all copies of this software.
//
// IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO
// ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
// EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//
// THE AUTHOR AND THE UNIVERSITY OF ILLINOIS SPECIFICALLY DISCLAIM ANY
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
// PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND NEITHER THE AUTHOR NOR
// THE UNIVERSITY OF ILLINOIS HAS ANY OBLIGATION TO PROVIDE MAINTENANCE,
// SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
//
// Author:	    Steve Lumetta
// Version:	    1.00
// Creation Date:   20 April 2018
// Filename:	    mp12.cpp
// History:
//	SL	1.00	20 April 2018
//		First written.
//

/*
* Modifier: Yu Siying
* Filename: mp12.cpp
* History:
*   SY  1   Wed Jan 4 21:38:00 2022
*           Implement all 4 functions and the turtle behaves nice
* Function: creat stream, close stream, read new command line, execute command
*/

#include <iostream>
#include <list>

#include <math.h>
#include <stdio.h>

#include "WALYmain.h"

#include "TurtleScreen.h"

#include "mp5.h"
#include "mp12.h"

using namespace std;
using namespace WALY;
#define pi 3.1415926535

// THESE ARE GLOBAL VARIABLES IN THE INTEREST OF TESTING.  THEY SHOULD
// BE FILE-SCOPE OR LOCAL TO frameUpdate.
FILE *input = NULL;           // input stream
list<segment_t> lines;        // list of lines drawn
command_t cmd = {CMD_NONE};   // current command
int32_t moveColor = 0xFFFFFF; // current movement drawing color

/*
* openInputStream -- Open the input stream for reading
* INPUTS: fname -- source file
* OUTPUTS: input -- input stream
* RETURN VALUE: 1 on success, 0 on failure.
* SIDE EFFECT: none
*/
int32_t
openInputStream(const char *fname)
{
    input = fopen(fname,"r");
    if ( NULL == input ){ return 0; }
    return 1;
}

/*
* closeInputStream -- Close the input stream
* INPUTS: none
* OUTPUTS: none
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void closeInputStream(void)
{
    fclose(input);
}

void showLines(void)
{
    // you can use 'call showLines ()' in GDB to look at the
    // list of segments.  Don't forget to call makeVisibleChange
    // when you change this list, since the background won't be
    // redrawn unless you call that function (so changes will not
    // show up.
    for (list<segment_t>::iterator it = lines.begin();
         lines.end() != it; it++)
    {
        Screen::debug() << it->color << " " << it->x1 << "," << it->y1 << "  " << it->x2 << "," << it->y2 << endl;
    }
}

/*
* drawEverything -- Draw all lines in the list of lines using
*                   set_color and draw_line from MP5.
* INPUTS: none
* OUTPUTS: all lines drawn
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void drawEverything(void)
{
    if ( lines.empty() ){ return; }
    for (list<segment_t>::iterator it = lines.begin(); lines.end() != it; it++){
        set_color(it->color);
        draw_line(it->x1,it->y1,it->x2,it->y2);
    }
}

/*
* readNewCommand -- Read commands and execute them until a command
*                   that needs animation is read.
* INPUTS: ts -- TurtleScreen class instance
* OUTPUTS: cmd -- returned with commands read and executed
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void readNewCommand(TurtleScreen *ts)
{
    while( cmd.type == CMD_NONE ){
        // read one command line from source file
        char read_line[200];
        if ( NULL == fgets(read_line,200,input) ){ break; }
        // figure out the command
        int32_t len_cmd = 0, len_name = 0;
        while( read_line[len_cmd] != '\n' ){ len_cmd++; }
        while( read_line[len_name] != ' ' ){ len_name++; }
        if ( len_name > len_cmd ){ len_name = len_cmd; }
        char* cmd_line = (char*)malloc(sizeof(char)*len_cmd);
        char* cmd_name = (char*)malloc(sizeof(char)*len_name);
        for ( int32_t i = 0; i < len_cmd; i++ ){ cmd_line[i] = read_line[i]; } cmd_line[len_cmd]='\0';
        for ( int32_t i = 0; i < len_name; i++ ){ cmd_name[i] = read_line[i]; } cmd_name[len_name]='\0';
        
        // execute and read next line
        if ( !strcasecmp(cmd_name,"color") ){
            free(cmd_name);
            char buf[5],trail;
            if( 2 == sscanf(cmd_line,"%5s%x%1s",buf,&moveColor,&trail) ){
                free(cmd_line);
            } else { fputs("INVALID ARGUMENT",stderr); }
            continue;
        }
        if ( !strcasecmp(cmd_name,"restart") ){
            free(cmd_name);
            free(cmd_line);
            rewind(input);
            continue;
        }
        // require animation
        if ( !strcasecmp(cmd_name,"move") ){
            free(cmd_name);
            int32_t dist,frames; char buf[4],trail;
            if( 3 == sscanf(cmd_line,"%4s%d%d%1s",buf,&dist,&frames,&trail) ){
                free(cmd_line);
                if ( dist <= 0 || frames <= 0 ){ continue; } // ignore non-positive arguments
                cmd.type = CMD_MOVE;
                cmd.cmdFrames = cmd.framesLeft = frames;
                cmd.startX = ts->getX();
                cmd.startY = ts->getY();
                cmd.endX = ts->getX()-round(sin(ts->getDirection()*pi/18)*dist);
                cmd.endY = ts->getY()-round(cos(ts->getDirection()*pi/18)*dist);
            } else { fputs("INVALID ARGUMENT",stderr); }
            continue;
        }
        if ( !strcasecmp(cmd_name,"turn") ){
            free(cmd_name);
            int32_t amt; char buf[4],trail;
            if ( 2 == sscanf(cmd_line,"%4s%d%1s",buf,&amt,&trail) ){
                free(cmd_line);
                cmd.type = CMD_TURN;
                cmd.cmdFrames = cmd.framesLeft = abs(amt);
                ( amt > 0 ) ? ( cmd.dir = 1 ) : ( cmd.dir = -1 );
            } else { fputs("INVALID ARGUMENT",stderr); }
            continue;
        }
        if ( !strcasecmp(cmd_name,"wait") ){
            free(cmd_name);
            int32_t frames; char buf[4],trail;
            if ( 2 == sscanf(cmd_line,"%4s%d%1s",buf,&frames,&trail) ){
                free(cmd_line);
                if ( frames <= 0 ){ continue; } // ignore non-poisitive frames
                cmd.type = CMD_WAIT;
                cmd.cmdFrames = cmd.framesLeft = frames;
            } else { fputs("INVALID ARGUMENT",stderr); }
            continue;
        }
    }
}

/*
* frameUpdate -- Called 25 times per second to redraw the scene.
                 Call readNewCommand if new command is needed (cmd.type is CMD_NONE).
* INPUTS: ts -- TurtleScreen class instance
* OUTPUTS: ts -- with frame updated
* RETURN VALUE: none
* SIDE EFFECT: none
*/
void frameUpdate(TurtleScreen *ts)
{
    if ( cmd.type == CMD_NONE ){ readNewCommand(ts); }
    switch (cmd.type){
        case CMD_MOVE:
            segment_t new_line;
            new_line.color = moveColor;
            new_line.x1 = ts->getX();
            new_line.y1 = ts->getY();
            new_line.x2 = new_line.x1 + (cmd.endX-ts->getX()) / cmd.framesLeft;
            new_line.y2 = new_line.y1 + (cmd.endY-ts->getY()) / cmd.framesLeft;
            lines.push_back(new_line);
            ts->makeVisibleChange();
            ts->setX(new_line.x2);
            ts->setY(new_line.y2);
            cmd.framesLeft--;
            if ( cmd.framesLeft == 0 ){ cmd.type = CMD_NONE; }
            break;
        case CMD_TURN:
            cmd.framesLeft--;
            if ( cmd.dir == 1 ){ ts->turnLeft(); } else { ts->turnRight(); }
            if ( cmd.framesLeft == 0 ){ cmd.type = CMD_NONE; }
            break;
        case CMD_WAIT:
            cmd.framesLeft--;
            if ( cmd.framesLeft == 0 ){ cmd.type = CMD_NONE; }
            break;
        default: break;
    }
}
