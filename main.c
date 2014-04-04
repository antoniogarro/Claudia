/***************************************************************************************
* Copyright (c) 2014, Antonio Garro.                                                   *
* All rights reserved                                                                  *
*                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are *
* permitted provided that the following conditions are met:                            *
*                                                                                      *
* 1. Redistributions of source code must retain the above copyright notice, this list  *
* of conditions and the following disclaimer.                                          *
*                                                                                      *
* 2. Redistributions in binary form must reproduce the above copyright notice, this    *
* list of conditions and the following disclaimer in the documentation and/or other    *
* materials provided with the distribution.                                            *
*                                                                                      *
* 3. Neither the name of the copyright holder nor the names of its contributors may be *
* used to endorse or promote products derived from this software without specific      *
* prior written permission.                                                            *
*                                                                                      * 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"          *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE            *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE           *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE            *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL    *
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR           *
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER           *
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR     *
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF        *
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    *
***************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "board.h"

#define BUFFER 2048

/*Globals.*/
HASHTABLE hash_table;
PAWNTABLE pawn_table;
ZOBKEYS zobkeys;

int main(int argc, char *argv[])
{
    BOARD main_board;
    InitBoard(&main_board);
    ReadFEN(STARTPOS, &main_board);
    
    CONTROL engine_control;
    engine_control.stop = 1;
    
    ENGINE_STATE engine = {&main_board, &engine_control};
    
    if(AllocTable(&hash_table, TABLESIZE) == 0){
        printf("Not enough memory\n");
        return 1;
    }
    ClearHashTable(&hash_table);
    
    if(AllocPawnTable(&pawn_table, 100) == 0){
        printf("Not enough memory\n");
        return 1;
    }
    ClearPawnTable(&pawn_table);
    
    setvbuf(stdin, 0, _IONBF, 0);
    setvbuf(stdout, 0, _IONBF, 0);
    fflush(NULL);
    char input[BUFFER];

    while(fgets(input, BUFFER, stdin)){
        if(!Command(input, &engine)){
            break;
        }    
        memset(input, 0, BUFFER);
    }
    
    DeleteTable(&hash_table);
    DeletePawnTable(&pawn_table);
    return 0;
}
