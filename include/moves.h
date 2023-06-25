
#pragma once

//#include <string>
//#include <iostream>
// /#include <unordered_map>
//#include "game.h"
#include "attacks.h"
// #include "prints.h"
struct game_data;


// /*
//           binary move bits                               hexidecimal constants
    
//     0000 0000 0000 0000 0011 1111    source square       0x3f
//     0000 0000 0000 1111 1100 0000    target square       0xfc0
//     0000 0000 1111 0000 0000 0000    piece               0xf000
//     0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
//     0001 0000 0000 0000 0000 0000    capture flag        0x100000
//     0010 0000 0000 0000 0000 0000    double push flag    0x200000
//     0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
//     1000 0000 0000 0000 0000 0000    castling flag       0x800000

// */

// // encode move
#define INVALID (65)

// may use macros im not sure about using it with webassembly

#define IS_SQUARE_ATTACKED(gd, sqr, attacker_side) \
    (!attacker_side ? \
        (raw_attacks[0][sqr] & gd.bitboards[0]) || \
        (raw_attacks[6][sqr] & gd.bitboards[5]) || \
        (raw_attacks[4][sqr] & gd.bitboards[4]) || \
        (get_sliding_attacks(0, sqr, gd.occupancy[2]) & gd.bitboards[3]) || \
        (get_sliding_attacks(1, sqr, gd.occupancy[2]) & gd.bitboards[2]) || \
        (get_queen_attacks(sqr, gd.occupancy[2]) & gd.bitboards[1]) \
        : \
        (raw_attacks[0][sqr] & gd.bitboards[6]) || \
        (raw_attacks[5][sqr] & gd.bitboards[11]) || \
        (raw_attacks[4][sqr] & gd.bitboards[10]) || \
        (get_sliding_attacks(0, sqr, gd.occupancy[2]) & gd.bitboards[9]) || \
        (get_sliding_attacks(1, sqr, gd.occupancy[2]) & gd.bitboards[8]) || \
        (get_queen_attacks(sqr, gd.occupancy[2]) & gd.bitboards[7]))


#define encode_move(sqr, target, piece, promoted, capture, double_push, enpassant, castling) \
    (sqr) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double_push << 21) |    \
    (enpassant << 22) | \
    (castling << 23) 



#define GET_SOURCE_SQUARE(move) ((move) & 0x3F)
#define GET_TARGET_SQUARE(move) (((move) >> 6) & 0x3F)
#define GET_PIECE(move) (((move) >> 12) & 0xF)
#define GET_PROMOTED_PIECE(move) (((move) >> 16) & 0xF)
#define IS_CAPTURE(move) (((move) >> 20) & 0x1)
#define IS_DOUBLE_PUSH(move) (((move) >> 21) & 0x1)
#define IS_ENPASSANT(move) (((move) >> 22) & 0x1)
#define IS_CASTLING(move) (((move) >> 23) & 0x1)




// //inline std::unordered_map<int, std::array<uint32_t, 100>> piece_moves;

// /uint64_t sqr_legal_moves(int& sqr,game_data& gd);
int generate_moves(game_data& gd, uint32_t* move_list); // called by Movelist struct
int legal_moves(game_data& gd, uint32_t* moves);
void get_checks(int sqr, game_data& gd, uint64_t* arr);
inline bool move_and_check(game_data& orig_gd,int sqr,int target,int piece,int capture,int enpassant, int castling=0);
inline bool is_square_attacked(game_data& gd, int sqr, int attacker_side);
void make_move(game_data& gd , uint32_t move);
// // struct Move {
// //     public:
// //         //Move(uint32_t value):_value(value){};
// //         void set_value(uint32_t v){ _value = v;};
// //         uint32_t* get_value(){return &_value;};
// //         static int get_source_square(uint32_t* value){ return *value & ((1ULL<<6)-1);};
// //         static int get_piece(uint32_t* value){ return *value & ((1ULL<<12)-1);};
// //         static int get_target(uint32_t* value){ return *value & ((1ULL<<6)-1);};
// //         static int is_capture(uint32_t* value);
// //         static int is_double_push(uint32_t* value);
// //         static int get_en_passant(uint32_t* value);
// //         static int get_castles(uint32_t* value);

        

// //     //private:
// //         uint32_t _value;
// // };

constexpr int MAX_MOVES = 256;

struct MoveList {
    void generate(game_data& gd);

    const uint32_t* begin() const;
    const uint32_t* end() const;
    size_t size() const ;
    bool contains(uint32_t move) const;

    uint32_t& operator[](size_t index) {
        return move_list[index];
    }
    
    uint32_t move_list[MAX_MOVES];
    uint32_t* _last;
    int _size;
};

struct SqrList{
    const uint32_t* begin() const;
    const uint32_t* end() const;
    size_t size() const ;
    bool contains(uint32_t move) const;

    uint32_t& operator[](size_t index) {
        return square_list[index];
    }
    
    uint32_t square_list[MAX_MOVES];
    uint32_t* _last;
    int _size;
};



/*
uint64_t mask = (((1ULL << (king_sqr-1) ) - 1) >> rook_sqr) << (rook_sqr-1); 
uint64_t mask = (1ULL << (king_sqr-1)) & ~(1ULL << rook_sqr);


in javascript

king_sqr = 10;
rook_sqr = 13;

mask = (((1n << BigInt(king_sqr-1)) - 1n) >> BigInt(rook_sqr)) << BigInt(rook_sqr);

console.log(mask.toString(2));



*/