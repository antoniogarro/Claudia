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

#include "board.h"
#include "engine.h"

char IsLegal(BOARD *board, MOVE *curr_move)
{
  MOVE poss_moves[MAXMOVES];
  int nposs_movs = MoveGen(board, poss_moves, 1);
  /*We only compare info about squares to decide legality, not captured piece or previous EP:*/
  MOVE sqs = SQSMASK(*curr_move); 
  /*TODO: promoted.*/
  for (int i = 0; i < nposs_movs; i++) {
    if (sqs == SQSMASK(poss_moves[i])) {  /*curr_move is possible.*/
      MakeMove(board, curr_move);
      if (!LeftInCheck(board)) {
        Takeback(board, *curr_move);
        return 1;    /*curr_move is legal.*/
      }
      Takeback(board, *curr_move);
    }
  }
  return 0;
}

int Perft(BOARD *board, int depth)
{
  int val = 0;
  int nposs_movs = 0;
  MOVE poss_moves[MAXMOVES];
  
  nposs_movs = MoveGen(board, poss_moves, 1);
  
  for (int i = 0; i < nposs_movs; i++) {
    MakeMove(board, &poss_moves[i]);
    if (depth > 1) {
      if (board->white_to_move) {
        if (!BlackInCheck(board)) val += Perft(board, depth - 1);
      } else {
        if (!WhiteInCheck(board)) val += Perft(board, depth - 1);
      }
    } else {
      if (board->white_to_move) {
        if (!BlackInCheck(board)) val++;
      } else {
        if (!WhiteInCheck(board)) val++;
      }
    }      
    Takeback(board, poss_moves[i]);
  }
  return val;
}
