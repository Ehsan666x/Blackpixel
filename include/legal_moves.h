#pragma once

#include <string>
#include <iostream>
#include <globals.h>


uint64_t sqr_legal_moves(int& sqr,game_data& gd);
uint64_t refine_own_occupency_attack(int& kind , int& sqr,uint64_t& attack ,uint64_t& occupancy);
uint64_t refine_opponent_occupency_attack(int& kind , int& sqr ,uint64_t& attack ,uint64_t& occupancy);