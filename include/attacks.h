#pragma once
#include <iostream>
#include <unordered_map>

//std::unordered_map<int, std::unordered_map<int, uint64_t>> raw_attacks;
inline std::unordered_map<int, std::unordered_map<int, uint64_t>> raw_attacks;
//inline uint64_t sliding_attack_tables[2][64][512];
//inline std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, uint64_t>>> sliding_attack_tables;
inline std::unordered_map<int, std::unordered_map <int, std::unordered_map<uint64_t, uint64_t> > > sliding_attack_tables;


//uint64_t refine_own_occupency_attack(int& kind , int& sqr,uint64_t& attack ,uint64_t& occupancy);
//uint64_t refine_opponent_occupency_attack(int& kind , int& sqr ,uint64_t& attack ,uint64_t& occupancy);


void init_raw_attacks();
uint64_t sliding_attacks_manually(int p,int sqr, uint64_t& block_mask);

void init_sliding_attacks_tables();
uint64_t get_sliding_attacks(int p,int sqr, uint64_t occupancy);
uint64_t get_queen_attacks(int sqr, uint64_t occupancy);

