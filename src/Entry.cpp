
#include <iostream>
#include <sstream>
#include "../include/globals.h" // <string>
#include "../include/attacks.h" //iostream unordered_map
//#include "../include/game.h" // string moves.h
#include "../include/perft.h" // game
//#include "../include/moves.h"
#include "../include/prints.h" // string iostream globals


// #define empty_board "8/8/8/8/8/8/8/8 b - - "
// #define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
// #define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
//                         "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1"
// #define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
// #define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
// 4k2r/6r1/8/8/8/8/3R4/R3K3 w kQ - 0 1
// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

// check rnbqkbnr/ppppp1pp/8/5p1Q/3P4/8/PPP1PPPP/RNB1KBNR b KQkq - 0 1
// check rnbq1bnr/ppp1pppp/3p4/1k3R2/8/8/PPPPPPPP/1NBQKBNR b - - 0 1
// en_passant rnb1kbnr/ppq1pppp/3p4/2pP1R2/8/4K3/PPP1PPPP/1NBQ1BNR w - c6 0 1

Out inout(std::string& input, int& depth){ 
    if(!depth){depth=1;}
    Game old_game(input,depth);
    Game game(input,depth);

    game_data gd=game.get_gd();
    // for(int i=0; i<12; i++){
    //    std::cout << gd.bitboards[i] << std::endl;
    //    print_bitboard(gd.bitboards[i]);
    // }
    print_chessboard(game.get_gd());
    
    //print_bitboard(gd.occupancy[0]);
    //print_bitboard(gd.occupancy[1]);
    //print_bitboard(gd.occupancy[2]);
    //uint64_t m =sliding_attacks_manually(1, 57, gd.occupancy[2]);
    //uint64_t r = get_queen_attacks(c3, gd.occupancy[2]);
    //print_bitboard(m);
    //print_bitboard(r);
    
    game_data copy_gd = game_data(gd);
    uint64_t nodes = perft(depth, copy_gd, true);
    std::cout<<nodes<<std::endl;
    
    auto list = game.possible_moves();
    for(uint32_t p : list ){
        log(piece_names[GET_PIECE(p)]+" "+notations[GET_SOURCE_SQUARE(p)] + " to "+ notations[GET_TARGET_SQUARE(p)] + " is capture?: " + std::to_string(IS_CAPTURE(p))
        + " is en_passant: "+std::to_string(IS_ENPASSANT(p))+" is double_push?: "+std::to_string(IS_DOUBLE_PUSH(p))
        +" is castling?: "+std::to_string(IS_CASTLING(p))
        );
    }
    std::string move = "move";

   
    bool loop = true;
    std::string userinput;
    std::string command;
    std::string argument;

    while (loop) {
        log("Enter your command: ");
        //std::getline(std::cin, userinput);
        std::getline(std::cin, userinput);
        //std::getline(iss, argument);

        std::istringstream iss(userinput);
        //iss >> command >> argument;
        //iss >> argument;
        //std::string command, fen;
        iss >> command >> std::ws; // Extract the command and skip leading whitespaces
        std::getline(iss, argument); // Read the

        if (command == "move") {
            // Handle move command
            Game blueprint(game.get_fen(),1);
            old_game = blueprint;
            std::cout << "Moving: " << argument << std::endl;
            game.move(argument);
            print_chessboard(game.get_gd());
        } else if (command == "perft") {
            // Handle perft command
            print_chessboard(game.get_gd());
            std::cout << "Perft depth: " << argument << std::endl;
            game_data copy_gd = game_data(game.get_gd());
            int depth = std::stoi(argument);
            uint64_t nodes = perft(depth, copy_gd, true);
            std::cout<<nodes<<std::endl;
            
        } else if (command == "fen") {
            // Handle fen command
            std::cout << "FEN: " << argument << std::endl;
            Game new_game(argument,depth);
            game = new_game;
            print_chessboard(game.get_gd());
        }else if (command == "list"){
            auto list = game.possible_moves();
            for(uint32_t p : list ){
                log(piece_names[GET_PIECE(p)]+" "+notations[GET_SOURCE_SQUARE(p)] + " to "+ notations[GET_TARGET_SQUARE(p)] + " is capture?: " + std::to_string(IS_CAPTURE(p))
                + " is en_passant: "+std::to_string(IS_ENPASSANT(p))+" is double_push?: "+std::to_string(IS_DOUBLE_PUSH(p))
                +" is castling?: "+std::to_string(IS_CASTLING(p))
                );
            }
        }else if (command == "show"){
            print_chessboard(game.get_gd());
        }else if(command == "takeback"){
            Game blueprint(old_game.get_fen(),1);
            game = blueprint;
            print_chessboard(game.get_gd());
        }else if(command == "exit" || command =="q" || command =="quit"){
            loop = false;
        } else {
            // Invalid command
            log("Invalid command");
        }
    }



        // log("make a move");
        // std::getline(std::cin, move);
        // game.move(move);
        
        // auto list = game.possible_moves();
        // for(uint32_t p : list ){
        //     log(piece_names[GET_PIECE(p)]+" "+notations[GET_SOURCE_SQUARE(p)] + " to "+ notations[GET_TARGET_SQUARE(p)] + " is capture?: " + std::to_string(IS_CAPTURE(p))
        //     + " is en_passant: "+std::to_string(IS_ENPASSANT(p))+" is double_push?: "+std::to_string(IS_DOUBLE_PUSH(p))
        //     +" is castling?: "+std::to_string(IS_CASTLING(p))
        //     );
        // }
        // log(list.size());
        // print_chessboard(game.get_gd());

        //std::cin.ignore();
        //int prft = 3;
        //log("Perft Level 2");
        //std::getline(std::cin, prft);
        //std::cin.ignore();
        //std::cin >> prft;

        //while(prft!= 0 || move!="end"){
        // if((int)prft >0 && prft <10){
        //     game_data copy_gd = game.get_gd();
        //     uint64_t nodes = perft((int)prft, copy_gd, true);
        //     std::cout<<nodes<<std::endl;
        // }
        //std::cin.ignore();
        //}
   

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

