#pragma once
#include "game.h"
#include "prints.h"

uint64_t perft_bit(int depth, game_data& gd, bool main);
uint64_t perft(int depth, game_data& gd, bool main);