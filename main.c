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
#include "claudia.h"
#include "engine.h"
#include "board.h"

const int TABLESIZE = 32;

/*Globals.*/
struct BOARD board;
struct ZOBKEYS zobkeys;
struct CONTROL control;
struct HASHTABLE hash_table;

int main(int argc, char* argv[])
{
    InitBoard();
    if(AllocTable(TABLESIZE) == 0){
        printf("Not enough memory\n");
        return 1;
    }
    ClearHashTable();

    ReadFEN(STARTPOS);        /*or ReadFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");*/
    setvbuf(stdin, 0, _IONBF, 0);
    setvbuf(stdout, 0, _IONBF, 0);
    fflush(NULL);
    char input[2048];

    /*
    char sFirst[] = "position startpos moves b1c3 d7d5 a1b1 d5d4 c3b5 e7e5 b1a1 a7a6 b5a3 b8c6 a1b1 g8f6 b1a1 e5e4 a1b1 f8c5 b1a1 e8g8 a1b1 c8e6 b1a1 c6b4 b2b3 f6g4 c1b2 e4e3 g1h3 e3f2 h3f2 g4f2 e1f2 d8f6 f2e1 f6h4 g2g3 h4e4 h1g1 d4d3 e2e3 d3c2 d1e2 a8d8 d2d4 e6g4 e2f2 d8d4 f1e2 b4d3 e2d3 e4d3 f2f3 g4f3";
    char sFirst[] = "position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 moves e1g1\n";
    */
    control.stop = 1;
    while(fgets(input, sizeof(input), stdin)){
        if(!Command(input)){
            break;
        }    
        memset(input, 0, 2048);
    }
    
    DeleteTable();
    return 0;
}
