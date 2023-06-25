#pragma once

#include <string>
#include <iostream>
#include "globals.h"


//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
//4k2r/6r1/8/8/8/8/3R4/R3K3 w kQ - 0 1

//r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1

inline std::string utf8_pieces [12] ={
  //UTF-8 Code   Chess Piece    
       "\u265A\uFE0E", // ♚︎ (Black King)
        "\u265B\uFE0E", // ♛︎ (Black Queen)
        "\u265C\uFE0E", // ♜︎ (Black Rook)
        "\u265D\uFE0E", // ♝︎ (Black Bishop)
        "\u265E\uFE0E", // ♞︎ (Black Knight)
        "\u265F\uFE0E", // ♟︎ (Black Pawn)
        "\u2654\uFE0E", // ♔︎ (White King)
        "\u2655\uFE0E", // ♕︎ (White Queen)
        "\u2656\uFE0E", // ♖︎ (White Rook)
        "\u2657\uFE0E", // ♗︎ (White Bishop)
        "\u2658\uFE0E", // ♘︎ (White Knight)
        "\u2659\uFE0E"  // ♙︎ (White Pawn)

};


inline std::string notations[64] = { // global extern
    "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1", 
    "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2", 
    "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3", 
    "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4", 
    "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5", 
    "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6", 
    "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7", 
    "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8",
};

inline std::string piece_names[12] = {
    "white King","white Queen","white Rook","white Bishop","white Knight","white Pawn",
    "black king","black queen","black rook","black bishop","black knight","black pawn"
};

Out inout(std::string& input, int& depth); // this function is declared in Entry.cpp
std::string calculate_new_fen(std::string& fen, const std::string& move);

template <typename T>
void log(const T& message) {
        std::cout << message << std::endl;
}



void print_bitboard(uint64_t bitboard);

void print_chessboard(game_data gd);













