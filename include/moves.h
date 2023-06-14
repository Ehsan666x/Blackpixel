
#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include "globals.h"
// #include "prints.h"



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
#define encode_move(sqr, target, piece, promoted, capture, double_push, enpassant, castling) \
    (sqr) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double_push << 21) |    \
    (enpassant << 22) | \
    (castling << 23) 



#define get_source_square(move) ((move) & 0x3F)
#define get_target_square(move) (((move) >> 6) & 0x3F)
#define get_piece(move) (((move) >> 12) & 0xF)
#define get_promoted_piece(move) (((move) >> 16) & 0xF)
#define is_capture(move) (((move) >> 20) & 0x1)
#define is_double_push(move) (((move) >> 21) & 0x1)
#define is_enpassant(move) (((move) >> 22) & 0x1)
#define is_castling(move) (((move) >> 23) & 0x1)




// //inline std::unordered_map<int, std::array<uint32_t, 100>> piece_moves;

uint64_t sqr_legal_moves(int& sqr,game_data& gd);
uint64_t refine_own_occupency_attack(int& kind , int& sqr,uint64_t& attack ,uint64_t& occupancy);
uint64_t refine_opponent_occupency_attack(int& kind , int& sqr ,uint64_t& attack ,uint64_t& occupancy);
int generate_moves(game_data& gd, uint32_t* move_list); // called by Movelist struct
inline uint64_t pin_mask(game_data& gd);

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
    void generate(game_data gd);

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