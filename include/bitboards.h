#pragma once

#include <string>
#include <iostream>
#include "game.h"
#include "globals.h"

inline std::string str_pieces ="KQRBNPkqrbnp";
inline std::string str_white ="KQRBNP";
inline std::string str_black ="kqrbnp";
inline std::string promotion_pieces = "Iqrbn"; // 01234

constexpr inline int relevant_array[2][64] = {
    // bishop
    {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6},
    // rook
    {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    }
};

// #define COUNT_FIRST_BIT(num)({ int count = 0; while ((num) > 0) { if ((num) & 1) { break; } (num) >>= 1; count++;} count; })
#define GCC_GET_BIT_SQUARE(num) ((num) ? __builtin_ctzll(num) : 0)

#define PUT_BIT(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define IS_BIT(bitboard, square) ((bitboard) & (1ULL << (square)))
#define DEL_BIT(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define DEL_PAT(bitboard, pattern) ((bitboard) &= ~(pattern))



#define get_first_square(bitboard) \
    ({ \
        int count = 0; \
        uint64_t tempBitboard = (bitboard); \
        while ((tempBitboard) > 0) { \
            if ((tempBitboard) & 1) { \
                break; \
            } \
            (tempBitboard) >>= 1; \
            count++; \
        } \
        count; \
    })

#define count_bits(bitboard) \
    ({ \
        int count = 0; \
        uint64_t tempBitboard = (bitboard); \
        while (tempBitboard) { \
            count++; \
            tempBitboard &= tempBitboard - 1; \
        } \
        count; \
    })



game_data parse_fen(const char *fen);
//int get_first_square(uint64_t bitboard);
//int count_bits(uint64_t bitboard);
uint64_t generate_occupancy(int index, int relevant_bits, uint64_t attack_mask);