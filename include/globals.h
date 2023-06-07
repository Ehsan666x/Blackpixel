#pragma once
#include <string>


extern std::string notations[64];

extern int ascii_pieces[12];

extern std::string str_pieces ;
extern std::string str_white ;
extern std::string str_black ;

//extern uint64_t raw_attacks[7];


inline uint64_t full_Square = 0ULL;
inline uint64_t empty_a = 0ULL;
inline uint64_t empty_h = 0ULL;
inline uint64_t empty_gh = 0ULL;
inline uint64_t empty_ab = 0ULL;


struct Out{ //
    float e; // eval
    std::string bm; //best move
};

struct game_data{
    char* fen;
    // ull wkbit=0ULL; 0
    // ull wqbit=0ULL; 1
    // ull wrbit=0ULL; 2
    // ull wbbit=0ULL; 3
    // ull wnbit=0ULL; 4
    // ull wpbit=0ULL; 5
    // ull bkbit=0ULL; 6
    // ull bqbit=0ULL; 7
    // ull brbit=0ULL; 8
    // ull bbbit=0ULL; 9
    // ull bnbit=0ULL; 10
    // ull bpbit=0ULL; 11
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
};






