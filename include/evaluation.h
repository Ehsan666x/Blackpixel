
#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include "game.h"
#include "openings.h"
#include "globals.h"
#include "bitboards.h"
#include "prints.h"


#define COMPARE_MOVES(ml, depth, move1, move2) \
    ({ \
        bool result = false; \
        for (int i = 0; i < 2; i++) { \
            if ((move1)[0] == (ml).killer_moves[depth][i][0] && (move1)[1] == (ml).killer_moves[depth][i][1] && (move1)[2] == (ml).killer_moves[depth][i][2]) { \
                result = true; \
                break; \
            } else if ((move1)[0] == (ml).history_moves[depth][i][0] && (move1)[1] == (ml).history_moves[depth][i][1] && (move1)[2] == (ml).history_moves[depth][i][2]) { \
                result = true; \
                break; \
            } \
        } \
        result; \
    })

//inline bool COMPARE_MOVES( ArrayMoveList ml,int depth,uint8_t (& move1)[8], uint8_t (& move2)[8]);


inline int pawn_endgame_position_values[64];

const inline int mirror_score[64] =
{
    h8, g8, f8, e8, d8, c8, b8, a8, 
    h7, g7, f7, e7, d7, c7, b7, a7, 
    h6, g6, f6, e6, d6, c6, b6, a6, 
    h5, g5, f5, e5, d5, c5, b5, a5, 
    h4, g4, f4, e4, d4, c4, b4, a4, 
    h3, g3, f3, e3, d3, c3, b3, a3, 
    h2, g2, f2, e2, d2, c2, b2, a2, 
    h1, g1, f1, e1, d1, c1, b1, a1, 
};



struct Best_line{
    //void search(game_data& gd);
    Best_line(int greediness, int aggression,int naivity);
    const uint32_t* begin() const;
    const uint32_t* end() const;
    size_t size() const ;
    bool contains(uint32_t move) const;

    uint32_t& operator[](size_t index) {
        return best_line_list[index];
    }
    
    uint32_t best_line_list[MAX_DEPTH];
    uint8_t best_move[8];
    uint32_t* _last;
    int _size =0;
    int _depth;
    int _alpha;
    int _beta;
    int _greediness;
    int _aggression;
    int _naivity;
};


int alpha_beta(int depth , game_data& gd, Best_line& bl, int alpha , int beta , int (&piece_values)[6] , int (&position_values)[6][64]);
int search(int depth ,game_data& gd, Best_line& bl , int alpha, int beta , int (&piece_values)[6], int (&position_values)[6][64]  , uint8_t (&last_move)[8], int main); 

int quiescence(int depth,game_data& gd, Best_line& bl , int alpha, int beta , int (&piece_values)[6], int (&position_values)[6][64], uint8_t (&last_move)[8]);

void king_edge_penalty(int (&position_values)[6][64]);
void endgame_king_proximity(game_data& gd, int (&position_values)[6][64]);
int pawn_endgame_eval(game_data& gd ,int eval);
int evaluate(game_data& gd , int greediness, int aggression, int naivity , int (&piece_values)[6] , int (&position_values)[6][64]);


void set_piece_values(int* piece_values , int greediness ,int aggression, int naivity);

void set_position_values(int (&position_values)[6][64] , int greediness, int aggression, int naivity);



inline void sort_moves(ArrayMoveList& ml, int depth);
