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

#include <string.h>
#include <stdio.h>
#include "engine.h"
#include "hashtable.h"

#define PVLEN 100
#define SPVLEN 600
#define MVLEN 7

/*This works because the replacement scheme ensures shallow PV is not overwritten,
and may fail if the hash table is full.*/
static int RetrievePV(BOARD *board, MOVE *PV, int depth)
{
  int PVlen = 0;
  MOVE mov = GetHashMove(&hash_table, board->zobrist_key);
  while (mov && PVlen <= depth && IsLegal(board, &mov)) {
    PV[PVlen++] = mov;
    MakeMove(board, &mov);
    mov = GetHashMove(&hash_table, board->zobrist_key);
  }
  for (int i = PVlen; i > 0; i--) {
    Takeback(board, PV[i-1]);
  }
  return PVlen;
}

static int AssessDraw(const BOARD *board, int contempt)
{
  int draw = board->white_to_move ?
              DRAW_VALUE + contempt*(1.0-GameStage(board)):
              DRAW_VALUE - contempt*(1.0-GameStage(board));
  if (board->rev_plies[board->ply] >= 50) {
    return draw;
  } else {
    for (int p = board->ply-board->rev_plies[board->ply]; p < board->ply; p++) {
      if (board->zobrist_history[p] == board->zobrist_key) {
        return draw;
      }
    }
  }
  return ERRORVALUE;
}

static int DrawMove(BOARD *board, MOVE move)
{
  MakeMove(board, &move);
  int draw = AssessDraw(board, 0);
  Takeback(board, move);
  return draw != ERRORVALUE;
}

static int AlphaBeta (BOARD *board, int depth, int alpha, int beta,
               int root, CONTROL *control, char skip_null, MOVE killers[][2])
{
  int nmoves, good = 0, nlegal = 0;
  MOVE moves[MAXMOVES];
  MOVE best_move = 0;
  char str_mov[MVLEN];
  int val = ERRORVALUE;
  char hash_flag = HASH_ALPHA;
  
  int in_check = InCheck(board, 0);

  if (root > control->seldepth) control->seldepth = root;

  if (depth > 0 || in_check) {
    if (root > 0) {
      val = GetHashEval(&hash_table, board->zobrist_key, depth, alpha, beta);
      if (val != ERRORVALUE) return val;
    }
    if (depth > 2 && !in_check) {    
      if (!skip_null && board->piece_material[board->white_to_move] != 0) {
        MOVE null_mv = NULL_MOVE;
        MakeMove(board, &null_mv);
        val = -AlphaBeta(board, depth-3, -beta, -beta+1, root+1, control, 1, killers);
        Takeback(board, null_mv);
        if (val >= beta) return beta;
      }
    }
    nmoves = MoveGen(board, moves, 1);
    good = SortMoves(board, moves, nmoves, killers[root]);
  } else {
    if (!control->ponder && clock() - control->init_time >= (control->max_time - 10)*CPMS) {
      control->stop = 1;
    }
    val = LazyEval(board);
    if (val-LAZYBETA >= beta) return beta;
    if (val+LAZYALPHA < alpha) return alpha;

    val = StaticEval(board);
    UpdateTable(&hash_table, board->zobrist_key, val, 0, 0, HASH_EXACT);
    if (val >= beta) return beta;
    if (val > alpha) alpha = val;
    
    nmoves = CaptureGen(board, moves);
    nmoves = FilterWinning(board, moves, nmoves);
  }
        
  for (int i = 0; i < nmoves; i++) {
    MakeMove(board, &moves[i]);
    control->node_count++;
    if (LeftInCheck(board)) {
      Takeback(board, moves[i]);
      continue;
    }
    if (root == 0) {
      if (!control->best_move) control->best_move = moves[i];    /* Better than nothing. */
      if (depth > 6 && !control->ponder) {
        MoveToAlgeb(moves[i], str_mov);
        printf("info depth %i seldepth %i hashfull %i currmove %s currmovenumber %i\n",
               depth, control->seldepth, hash_table.full/(hash_table.size/1000), str_mov, i+1);
      }
    }
    nlegal++;
    val = AssessDraw(board, control->contempt);
    if (val == ERRORVALUE) {
      int ext = 0; //InCheck(board, 0) ? 1 : 0;
      if (best_move) {
        int LMR = (depth > 2 && !in_check && i > good &&
                   !CAPTMASK(moves[i]) && !InCheck(board, 0)) ? 1 : 0;
        val = -AlphaBeta(board, depth+ext-LMR-1, -alpha-1, -alpha, root+1, control, 0, killers);
        if (val > alpha) {
          val = -AlphaBeta(board, depth+ext-1, -alpha-1, -alpha, root+1, control, 0, killers);
          if (val > alpha && val < beta) {
            val = -AlphaBeta(board, depth+ext-1, -beta, -alpha, root+1, control, 0, killers);
          }
        }
      } else {
        val = -AlphaBeta(board, depth+ext-1, -beta, -alpha, root+1, control, 0, killers);
      }
    }
    Takeback(board, moves[i]);
    if (!control->ponder && control->stop) return alpha;
    if (val >= beta) {
      UpdateTable(&hash_table, board->zobrist_key, val, moves[i], depth, HASH_BETA);
      if (CAPTMASK(moves[i]) == 0 && killers[root][0] != moves[i] && killers[root][1] != moves[i]) {
        killers[root][1] = killers[root][0];
        killers[root][0] = moves[i];
      }
      return beta;
    }
    if (val > alpha) {
      alpha = val;
      hash_flag = HASH_EXACT;
      best_move = moves[i];
      if (root == 0) control->best_move = best_move;
    }
    if (root == 0 && ((clock() - control->init_time) > control->wish_time*CPMS)) {
    /* if short of time, don't search anymore after current move */
      control->stop = 1;
      return alpha;
    }
  }
  if (nlegal == 0) {
    if (in_check) {
      /*UpdateTable(&hash_table, board->zobrist_key, MATE_VALUE+root, 0, depth, HASH_EXACT, hash_table.entries);*/
      return MATE_VALUE;
    } else if (depth > 0) {
      /*UpdateTable(&hash_table, board->zobrist_key, DRAW_VALUE, 0, depth, HASH_EXACT, hash_table.entries);*/
      return DRAW_VALUE;    /*Stalemate*/
    }
  } else {
    UpdateTable(&hash_table, board->zobrist_key, alpha, best_move, depth, hash_flag);
  }
  return alpha;
}

void IterativeDeep(BOARD *board, CONTROL *control)
{
  MOVE iPV[PVLEN];
  char sPV[SPVLEN];
  char str_mov[MVLEN];
  int eval = 0, pvlen = 0;
  int alpha = -INFINITE;
  int beta = INFINITE;
  int widening = ASP_WINDOW;
  unsigned long long nps = 0;
  MOVE killers[MAXDEPTH][2];
  control->best_move = 0;

/*printf("time %llu %llu\n", control->max_time, control->wish_time);*/
    
  for (int depth = 1; depth <= control->max_depth;) {
    clock_t curr_time = clock();
    memset(sPV, 0, SPVLEN);
    control->node_count = 0;
    control->seldepth = 0;
        
    eval = AlphaBeta(board, depth, alpha, beta, 0, control, 1, killers);
    if (control->stop && !control->ponder) {
      break;
    }
    pvlen = RetrievePV(board, iPV, depth+10);
    for (int i = 0; i < pvlen; i++) {
      MoveToAlgeb(iPV[i], str_mov);
      strcat(sPV, str_mov);
    }

    curr_time = (clock() - curr_time)/CPMS;
    if (curr_time) {
      nps = 1000*(control->node_count/curr_time);
    }
    if (eval > MATE_VALUE/2 && -eval > MATE_VALUE/2) {
      printf("info depth %u time %llu nodes %llu nps %llu score cp %i pv %s\n",
              depth, (unsigned long long)curr_time, control->node_count, nps, eval, sPV);
    } else if (-eval < MATE_VALUE/2) {
      printf("info depth %u time %llu nodes %llu nps %llu score mate %i pv %s\n",
              depth, (unsigned long long)curr_time, control->node_count, nps, (pvlen+1)/2, sPV);
    } else if (eval < MATE_VALUE/2) {
      printf("info depth %u time %llu nodes %llu nps %llu score mate %i pv %s\n",
              depth, (unsigned long long)curr_time, control->node_count, nps, -(pvlen+1)/2, sPV);
    }
    if (eval <= alpha || eval >= beta) {
      alpha = -INFINITE;
      beta = INFINITE;
    } else {
      if (3*curr_time > control->wish_time*CPMS - (clock()-control->init_time)) {
        control->stop = 1;
      }
      alpha = eval - ASP_WINDOW;
      beta = eval + ASP_WINDOW;
      depth++;
    }
  }
  control->stop = 1;
  if (DrawMove(board, control->best_move)) {
    printf("info draw\n");
  }
  MoveToAlgeb(control->best_move, str_mov);
  printf("bestmove %s", str_mov);
  if (pvlen > 1 && iPV[0] == control->best_move) {
      MoveToAlgeb(iPV[1], str_mov);
      printf("ponder %s", str_mov);
  }
  printf("\n");
}
