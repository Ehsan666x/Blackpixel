#pragma once

#include <string>
#include <iostream>
#include <globals.h>


//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
//4k2r/6r1/8/8/8/8/3R4/R3K3 w kQ - 0 1

//r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1



Out inout(std::string& input, int& depth); // this function is declared in Entry.cpp
std::string calculate_new_fen(std::string& fen, const std::string& move);

template <typename T>
void log(const T& message) {
        std::cout << message << std::endl;
}



void print_bitboard(uint64_t& bitboard);

void print_chessboard(game_data& gd);













