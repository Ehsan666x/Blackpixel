
#include <iostream>
#include "../include/globals.h"
#include "../include/attacks.h"
#include "../include/game.h"
#include "../include/moves.h"
#include "../include/prints.h"


// #define empty_board "8/8/8/8/8/8/8/8 b - - "
// #define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
// #define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
// #define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
// #define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
// 4k2r/6r1/8/8/8/8/3R4/R3K3 w kQ - 0 1
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

Out inout(std::string& input, int& depth){ 
    if(!depth){depth=1;}
    Game game(input,depth);

    game_data gd=game.get_gd();
    // for(int i=0; i<12; i++){
    //    std::cout << gd.bitboards[i] << std::endl;
    //    print_bitboard(gd.bitboards[i]);
    // }
    print_chessboard(game.get_gd());
    //print_bitboard(gd.occupancy[2]);
    //uint64_t m =sliding_attacks_manually(1, 57, gd.occupancy[2]);
    //uint64_t r = get_queen_attacks(c3, gd.occupancy[2]);
    //print_bitboard(m);
    //print_bitboard(r);
    auto list = game.possible_moves();
    for(uint32_t p : list ){
        log(piece_names[get_piece(p)]+" "+notations[get_source_square(p)] + " to "+ notations[get_target_square(p)] + " is capture?: " + std::to_string(is_capture(p))
        + " is en_passant: "+std::to_string(is_enpassant(p))+" is double_push?: "+std::to_string(is_double_push(p))
        +" is castling?: "+std::to_string(is_castling(p))
        );
    }

    // std::unordered_map moves_map = game.possible_moves();
    // for(std::pair pair : moves_map){
    //     for(uint32_t move : pair.second){
    //         //std::string message = "piece number: " + std::to_string(pair.first) + "sqr number: "+ std::to_string(move & ((1ULL<<6)-1));
    //         std::string message = "piece number: " + std::to_string(pair.first) + " sqr number: " + std::to_string(move & ((1ULL << 6) - 1));

    //         log(message);
    //     }
    // }

    //log(r);
    //std::cout << gd.en_passant << gd.castles << gd.fullmove << gd.halfmove << std::endl;

    Out out;
    out.e=game.eval();
    out.bm=game.best_move();

    return out;
}

// for web assembly

// EMSCRIPTEN_BINDINGS(module) {
//     emscripten::function("inout", &inout);
// }

// in js:

// fetch('myModule.wasm')
//   .then(response => response.arrayBuffer())
//   .then(bytes => WebAssembly.instantiate(bytes))
//   .then(result => {
//     const wasmModule = result.instance;

//     // Now you can access the exported functions
//     const inout = wasmModule.exports.inout;

//     // Call the exported function
//     const input = "your input";
//     const depth = 10;
//     const out = inout(input, depth);

//     // Use the result as needed
//     console.log(out);
// });

