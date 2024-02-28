#pragma once
#include <string>
//#include "globals.h"
#include "moves.h"
#include "globals.h"


inline std::string STANDARD_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

struct game_data{
    char* fen;
    uint64_t bitboards[12]={
        0ULL,0ULL,0ULL,
        0ULL,0ULL,0ULL,
        0ULL,0ULL,0ULL,
        0ULL,0ULL,0ULL
    };
    uint64_t occupancy[3]={0ULL,0ULL,0ULL}; // white,black,both
    int castles=0; // 1111 in standard position
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
    uint8_t mailbox[64];
    uint8_t wking_sqr;
    uint8_t bking_sqr;
    int castling_rights[64]; // ex: 1111 for positions of rooks in fischer random
    int piece_numbers [6] = {0,0,0,0,0,0}; //king queens rooks bishops knights pawns
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
    bool move(const std::string& move);
    std::string get_fen();
    std::string get_current_fen();
    game_data get_gd();
    MoveList possible_moves();
    ArrayMoveList generate_arraymoves();

private:
   int _depth;
   std::string _fen;
   game_data _gd; 
};


