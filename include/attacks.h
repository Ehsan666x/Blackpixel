#pragma once
#include <iostream>
#include <unordered_map>

//std::unordered_map<int, std::unordered_map<int, uint64_t>> raw_attacks;
inline std::unordered_map<int, std::unordered_map<int, uint64_t>> raw_attacks; // [i][sqr][attackbits]
//inline uint64_t sliding_attack_tables[2][64][512];
//inline std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, uint64_t>>> sliding_attack_tables;
inline std::unordered_map<int, std::unordered_map <int, std::unordered_map<uint64_t, uint64_t> > > sliding_attack_tables; // [p][sqr][occupancy][attackbits] 


//uint64_t refine_own_occupency_attack(int& kind , int& sqr,uint64_t& attack ,uint64_t& occupancy);
//uint64_t refine_opponent_occupency_attack(int& kind , int& sqr ,uint64_t& attack ,uint64_t& occupancy);


void init_raw_attacks();
uint64_t sliding_attacks_manually(int p,int sqr, uint64_t& block_mask);

void init_sliding_attacks_tables();
uint64_t get_sliding_attacks(int p,int sqr, uint64_t occupancy);
uint64_t get_queen_attacks(int sqr, uint64_t occupancy);

// #define get_sliding_attacks(p, sqr, occupancy) \
//     occupancy &= raw_attacks[(p==3 || p==0) ? 3 : 2][sqr]; \
//     sliding_attack_tables[(p==3 || p==0) ? 0 : 1][sqr][occupancy]

// #define get_queen_attacks(sqr, occupancy) \
//     uint64_t queen_attacks = 0ULL; \
//     uint64_t queen_occupancy = occupancy; \
//     occupancy &= raw_attacks[3][sqr]; \
//     queen_attacks |= sliding_attack_tables[0][sqr][occupancy]; \
//     queen_occupancy &= raw_attacks[2][sqr]; \
//     queen_attacks |= sliding_attack_tables[1][sqr][queen_occupancy]; \
//     queen_attacks
