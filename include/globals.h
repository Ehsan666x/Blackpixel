#pragma once
#include <string>
#include <unordered_map>
//#include <cstring>
//#include <unordered_map>
//#include "moves.h"

inline std::unordered_map<std::string, int> castlingValues = {
    {"-", 0},       // 0000
    {"K", 1},       // 0001
    {"Q", 2},       // 0010
    {"KQ", 3},      // 0011
    {"k", 4},       // 0100
    {"Kk",5},       // 0101
    {"kQ", 6},      // 0110
    {"KQk",7},      // 0111
    {"q", 8},       // 1000
    {"Kq",9},       // 1001
    {"Qq",10},      // 1010
    {"KQq",11},     // 1011
    {"kq", 12},     // 1100
    {"Kkq", 13},    // 1101
    {"Qkq", 14},    // 1110
    {"KQkq", 15},   // 1111
};

inline std::string castles_array[16] = {
    "-",    // 0000
    "K",    // 0001
    "Q",    // 0010
    "KQ",   // 0011
    "k",    // 0100
    "Kk",   // 0101
    "kQ",   // 0110
    "KQk",  // 0111
    "q",    // 1000
    "Kq",   // 1001
    "Qq",   // 1010
    "KQq",  // 1011
    "kq",   // 1100
    "Kkq",  // 1101
    "Qkq",  // 1110
    "KQkq"  // 1111
};

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

//constexpr int MAX_MOVES = 256;

struct Out{ //
    float e; // eval
    std::string bm; //best move
};


enum {
    h1, g1, f1, e1, d1, c1, b1, a1, 
    h2, g2, f2, e2, d2, c2, b2, a2, 
    h3, g3, f3, e3, d3, c3, b3, a3, 
    h4, g4, f4, e4, d4, c4, b4, a4, 
    h5, g5, f5, e5, d5, c5, b5, a5, 
    h6, g6, f6, e6, d6, c6, b6, a6, 
    h7, g7, f7, e7, d7, c7, b7, a7, 
    h8, g8, f8, e8, d8, c8, b8, a8, 
};


inline std::string zobristvalues;
inline std::string openingmaps;


void init_globals();