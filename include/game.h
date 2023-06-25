#pragma once
#include <string>
//#include "globals.h"
#include "moves.h"


struct game_data{
    char* fen;
    uint64_t bitboards[12]={
        0ULL,0ULL,0ULL,
        0ULL,0ULL,0ULL,
        0ULL,0ULL,0ULL,
        0ULL,0ULL,0ULL
    };
    uint64_t occupancy[3]={0ULL,0ULL,0ULL}; // white,black,both
    int castles=0;
    int side_to_move =0;
    int en_passant =0;
    int halfmove =0;
    int fullmove =0;
    int wr1;
    int wr2;
    int wk;
    int br1;
    int br2;
    int bk;
    MoveList piece_moves;

    game_data();
        // Copy constructor
    game_data(const game_data& other);
};


class Game {
public:
    Game(const std::string& fen, int depth); 
    float eval();
    std::string best_move();
    void move(const std::string& move);
    std::string get_fen();
    game_data get_gd();
    MoveList possible_moves();

private:
   int _depth;
   std::string _fen;
   game_data _gd; 
};


