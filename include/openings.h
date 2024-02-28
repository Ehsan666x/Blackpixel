#pragma once
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <cstring>
#include <random>
#include <fstream>
#include <vector>
#include "globals.h"
#include "game.h"
#include "evaluation.h"


struct Entry{
    uint16_t eval;//evaluation
    uint32_t best_move;//best move
    uint16_t alpha;//alpha 
    uint16_t beta;// beta
};

inline std::unordered_map<uint64_t, Entry> zobrist_positions;

inline uint64_t zobristTable[12][64];


inline constexpr uint8_t MAX_CANDIDATE = 20;

struct Variations{
    uint8_t candidate_size = 1; // 3 
    std::string candidate_moves[MAX_CANDIDATE]; // {d2d4, b1c3, ..}
    std::string move_line; // "1.e4 Nf6"
    int eval; // 50
    std::string name; // name +',' + variation + ',' + sub-variations name + ... 
    Variations(){candidate_moves[0] = "";};
    Variations(std::string candidate, std::string ml = "", int e =0, std::string n = ""): move_line(ml), eval(e), name(n) {
        candidate_moves[0] = candidate;
    }
};

inline std::unordered_map <uint64_t, Variations> openings_table; // zobrist hash , Variations


void generateOpeningsFile(std::string filename, std::unordered_map<uint64_t, Variations>& openings_table);
void initOpeningsTableFromFile(std::string filename, std::unordered_map<uint64_t, Variations>& openings_table);
void create_zobrist_values(std::string filename);

std::string PGN_to_UCI(game_data& gd,std::string& pgn_move);

void init_zobrist(std::string filename);
uint64_t get_board_hash(game_data& gd);
bool fileExists(std::string filename);