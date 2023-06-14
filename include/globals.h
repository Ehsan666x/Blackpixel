#pragma once
#include <cstring>
#include <unordered_map>


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
    int r1;
    int r2;
    int k;
    MoveList piece_moves;

        // Copy constructor
    game_data(const game_data& other) {
        fen = other.fen;
        memcpy(bitboards, other.bitboards, sizeof(bitboards));
        memcpy(occupancy, other.occupancy, sizeof(occupancy));
        castles = other.castles;
        side_to_move = other.side_to_move;
        en_passant = other.en_passant;
        halfmove = other.halfmove;
        fullmove = other.fullmove;
        r1 = other.r1;
        r2 = other.r2;
        k = other.k;
        piece_moves = other.piece_moves;
    }
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




