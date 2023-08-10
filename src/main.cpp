//#include <iostream>
//#include "../include/globals.h"
#include <chrono>
#include "../include/bitboards.h" //game.h
// #include "../include/attacks.h" //<iostream>  /<unordered_map> 
#include "../include/magics.h" 
#include "../include/prints.h" // iostream string globals
#include "../include/pins.h" // iostream unordered_map
#include "../include/evaluation.h"
#include "../include/openings.h"
#include "../include/Blackpixel.h"

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
    std::cout << "\u2654" << std::endl;
    init_globals(); 
    init_raw_attacks();
    init_magic_tables();
    init_sliding_attacks_tables();
    init_direction_attacks_tables();
    // need to provide openings.pgn if zobristvalues.txt or openingmaps.txt does not exist
    
    // if (!fileExists("openingmaps")) {
    // //if (!file.is_open()) {
    //     std::cerr << "Errors!\n";
    //     //return;
    // }
    init_zobrist("zobristvalues");
    //std::string openingmaps = "openingmaps.txt";
    initOpeningsTableFromFile("openingmaps", openings_table);
    // Game game(STANDARD_POSITION,1);
    // game_data gd = game.get_gd();
    // std::string pm [6] = {"1.Nc3", "2.Nf3", "3.a3", "4.d4", "5.f3", "6.h4"};
    // for(auto& m : pm){
    //     std::string move = PGN_to_UCI(gd,m);
    //     Myprintlog::log(move);
    // }
    // for(int sqr=32; sqr<63; sqr+=8){
    //     print_bitboard(direction_attack_tables[DIAG_RB][32][576460752303423488]);
    // }

    //game_data gd;
    // uint64_t attack_mask = raw_attacks[2][7];
    // print_bitboard(attack_mask);
    // for (int index = 0; index < 100; index++){
    //     // init occupancy
    //     uint64_t bb= generate_occupancy(index, count_bits(attack_mask), attack_mask);
    //     print_bitboard(bb);
    //     getchar();
    // }

 

    int depth=1;
    std::string input;
    // for(int i=0;i<12;i++){
    //     print_bitboard(standard_bits[i]);
    // }

    
    const char* command = "fen rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    const char* result1 = Blackpixel_engine(command , 4 , 0 , 0 , 0);
    const char* result2 = Blackpixel_engine("go 4" , 4 , 0 , 0 , 0);
    Myprintlog::log(result2);
    //const std::string message = "Best move :" + out.bm + " " + "Evaluation :" + std::to_string(out.e);
    //Myprintlog::log(message);

    // while (depth){

    //     Myprintlog::log("Enter your Blackpixel command");
    //     std::getline(std::cin, input);

    //     auto start = std::chrono::high_resolution_clock::now();

    //     //auto out = inout(input, depth);
    //     if(input == "q" || input == "exit" || input == "quit"){
    //         break;
    //     }
    //     const char* i = input.c_str();
    //     const char* r = Blackpixel_engine(i , 4 , 0 , 0 , 0);
    //     Myprintlog::log(r);
    //     auto end = std::chrono::high_resolution_clock::now();
    //     auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //     std::cout << " in " << milliseconds << " milliseconds" << std::endl;

    // }

    //Myprintlog::log((const int)depth);
    return 0;
}
