#include <iostream>
#include "../include/globals.h"
#include "../include/bitboards.h"
#include "../include/attacks.h"
#include "../include/magics.h"
#include "../include/prints.h"

uint64_t standard_bits[12]={
    8ULL,
    16ULL,
    129ULL,
    36ULL,
    66ULL,
    65280ULL,
    576460752303423488ULL,
    1152921504606846976ULL,
    9295429630892703744ULL,
    2594073385365405696ULL,
    4755801206503243776ULL,
    71776119061217280ULL,
};

int main(){

    init_raw_attacks();
    init_magic_tables();
    init_sliding_attacks_tables();
    
    game_data gd;
    // uint64_t attack_mask = raw_attacks[2][7];
    // print_bitboard(attack_mask);
    // for (int index = 0; index < 100; index++){
    //     // init occupancy
    //     uint64_t bb= generate_occupancy(index, count_bits(attack_mask), attack_mask);
    //     print_bitboard(bb);
    //     getchar();
    // }

 

    int depth;
    std::string input;
    // for(int i=0;i<12;i++){
    //     print_bitboard(standard_bits[i]);
    // }
    log("Enter your fen");
    //std::cin >> input;
    std::getline(std::cin, input);
    log("Enter Depth");
    std::cin >> depth;
    auto out = inout(input, depth);
    const std::string message = "Best move :" + out.bm + " " + "Evaluation :" + std::to_string(out.e);
    log(message);

    //log((const int)depth);
    return 0;
}
