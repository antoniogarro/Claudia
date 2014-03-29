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

#include <stdlib.h>
#include "hashtable.h"
#include "board.h"

int AllocTable(HASHTABLE *ht, int table_size)
{
    unsigned int s = 1024*1024*table_size;
    do{
        ht->entries = (HashData*) malloc(s);
        ht->size = s/sizeof(HashData);
        s *= 0.8;
    }while(ht->entries == 0);

    return ht->size;
}

void DeleteTable(HASHTABLE *ht)
{
    free(ht->entries);
}

void ClearHashTable(HASHTABLE *ht)
{
    for(int i = 0;  i < ht->size; i++){
        ht->entries[i].zobrist_key = 0;
        ht->entries[i].data = 0;
    }
    ht->full = 0;
}

void UpdateTable(HASHTABLE *ht, KEY zob_key, int eval, MOVE best_move, int depth, int flag)
{
    int key = zob_key%ht->size;
    HashData *entry = &ht->entries[key];
    
    if(MOVEMASK(entry->data)){
        if(!best_move) return;
        if(DEPTHMASK(entry->data) > depth) return;
    }
    /*if(entry->depth >= depth && entry->best_move && !best_move) return;*/
    if(entry->zobrist_key != zob_key){
        if(entry->zobrist_key == 0) ht->full++;
        entry->zobrist_key = zob_key;
    }
    /*if(flag == HASHEXACT || flag == HASHBETA);*/
    entry->data = PUT_HASH_MOVE(best_move);
    entry->data |= PUT_HASH_EVAL(eval);
    entry->data |= PUT_HASH_DEPTH(depth);
    entry->data |=  PUT_HASH_FLAG(flag);
}

MOVE GetHashMove(HASHTABLE *ht, KEY zob_key)
{
    int key = zob_key%ht->size;
    if(ht->entries[key].zobrist_key == zob_key){
        return MOVEMASK(ht->entries[key].data);
    }else return 0;
}

int GetHashEval(HASHTABLE *ht, KEY zob_key, int depth, int alpha, int beta)
{
    int key = zob_key%ht->size;
    const HashData *entry = &ht->entries[key];
    if(entry->zobrist_key == zob_key && DEPTHMASK(entry->data) >= depth){
        int eval = EVALMASK(entry->data);
        char flag = FLAGMASK(entry->data);
        if(flag == HASH_EXACT){
            return eval;
        }
        if(flag == HASH_BETA && eval >= beta){
            return beta;
        }
        if(flag == HASH_ALPHA && eval < alpha){
            return alpha;
        }
    }
    return ERRORVALUE;
}
