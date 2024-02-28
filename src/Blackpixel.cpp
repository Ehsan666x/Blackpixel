#include <sstream>
#include <cstring>
#include <future>
#include <array> 
#include "../include/Blackpixel.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
const char* Blackpixel_engine(const char* input, int depth , int greediness, int aggression,int naivity,int numWorkers ,int workerid ){
        const char* result;

        if(!depth){depth=1;}

        std::string userinput(input);
        std::string command;
        std::string argument;


        std::istringstream iss(userinput);

        iss >> command >> std::ws; // Extract the command and skip leading whitespaces
        std::getline(iss, argument); // Read the rest of the line into the argument

        if (command == "move") {
            // Handle move command
            Game blueprint(game.get_fen(),1);
            old_game = blueprint;
            //std::cout << "Moving: " << argument << std::endl;
            game.move(argument);
            //print_chessboard(game.get_gd());
            result = strdup(argument.c_str());
        }else if (command == "movepgn"){
                game_data current_gd = game.get_gd();
                //std::string pgnmove;
                //std::istringstream iss2(userinput);
                //iss2 >> command >> pgnmove;// >> argument;
                //Game blueprint(game.get_fen(),1);
                //old_game = blueprint;
                std::string uci_move = PGN_to_UCI(current_gd,argument);
                bool correct_move = game.move(uci_move);
                //current_gd = game.get_gd();
                //print_chessboard(game.get_gd());
                //std::string fen = game.get_current_fen();
                //Myprintlog::log(pgnmove + " " + uci_move + "fen: " + fen);

                return strdup(game.get_current_fen().c_str());

        }else if (command == "getfen"){
            
            return strdup(game.get_current_fen().c_str());

        } else if (command == "fen") {

            set_piece_values(piece_values , greediness , aggression, naivity);
            set_position_values(position_values , greediness , aggression, naivity);

            Game new_game(argument,depth);
            game = new_game;

            result = strdup(argument.c_str());

        }else if(command == "takeback"){
            std::string fenstring = old_game.get_fen();
            Game blueprint(fenstring,1);
            game = blueprint;
            //print_chessboard(game.get_gd());
            result = strdup(fenstring.c_str());
        }else if( command == "go" || command == "movego"){
            std::string moved;
            game_data current_gd = game.get_gd();
            bool correct_move = false;
            if(command == "movego"){
                std::string pgnmove;
                std::istringstream iss2(userinput);
                iss2 >> command >> pgnmove >> argument;
                Game blueprint(game.get_fen(),1);
                old_game = blueprint;
                std::string uci_move = PGN_to_UCI(current_gd,pgnmove);
                Myprintlog::log(uci_move);
                correct_move = game.move(uci_move);
                current_gd = game.get_gd();
            }
            Best_line bl(greediness , aggression , naivity);

            
            uint8_t last_move[8] = {0,0,0,0,0,0,0,0};
            //auto start = std::chrono::high_resolution_clock::now();

            MoveList ml;
            ml.generate(current_gd);
            
            std::string resultStr;
            int eval;
            int workers_moves_size;
            int workers_last_work;

            #ifdef __EMSCRIPTEN__
                // Code specific to emcc (Emscripten)

                eval = search(std::stoi(argument), current_gd, bl , -1000000 , 1000000 , piece_values , position_values, last_move, true);
                moved = arraymove_to_uci(bl.best_move);
                resultStr = moved + " " + std::to_string(eval) + " " + std::to_string(current_gd.side_to_move);

                /*
                workers_moves_size = ml._size / numWorkers;
                if(!workers_moves_size){workers_moves_size = 1;}
                if(workerid == numWorkers - 1){
                    workers_last_work = ml._size;
                }else{
                    workers_last_work = workerid + workers_moves_size;
                }
                if(workers_last_work > ml._size){ workers_last_work = ml._size;}
                if(workerid >= workers_last_work) { workerid = workers_last_work - 1;}
                
                std::array<int, MAX_MOVES> top_search_results;
                int custom_depth = std::stoi(argument) - 1;
                for(int i=workerid ; i < workers_last_work; i++){
                    top_search_results[i] =  searchMove(custom_depth, greediness, aggression, naivity, ml, i);
                }

                int index;
                if(!current_gd.side_to_move){ // white to move
                    int maxeval = -2000000;
                    for(int i=workerid;i<workers_last_work; i++){
                        if(top_search_results[i] > maxeval){
                            index = i;
                            eval = top_search_results[i];
                            maxeval = top_search_results[i];
                        }
                    }
                }else{ //black to move
                    int mineval = 2000000;
                    for(int i=workerid ; i<workers_last_work; i++){
                        if(top_search_results[i] < mineval){
                            index = i;
                            eval = top_search_results[i];
                            mineval = top_search_results[i];
                        }
                    }
                }

                resultStr = notations[GET_SOURCE_SQUARE(ml.move_list[index])] + notations[GET_TARGET_SQUARE(ml.move_list[index])] + " " + std::to_string(eval) + " " + std::to_string(current_gd.side_to_move);
                */
            #else
                // Code specific to g++ (GNU C++)

                //int top_search_results[MAX_MOVES];
                //for(uint32_t move : ml){
                
                std::array<std::future<int>, MAX_MOVES> futures;
                std::array<int, MAX_MOVES> top_search_results;
                int custom_depth = std::stoi(argument) - 1;

                uint64_t bhash = get_board_hash(current_gd);
                auto pair = openings_table.find(bhash);
                
                if (pair != openings_table.end()){
                    std::uniform_int_distribution<int> distribution(0, pair->second.candidate_size-1);
                    // Generate a random integer between 0 and candidate_size
                    int randomNum = distribution(gen);
                    Myprintlog::log(randomNum);
                    for(int i=0 ; i<ml._size; i++){
                        std::string movestring = notations[GET_SOURCE_SQUARE(ml.move_list[i])] + notations[GET_TARGET_SQUARE(ml.move_list[i])];
                        if(pair->second.candidate_moves[randomNum] == movestring){
                            moved = move_to_uci(ml.move_list[i]);
                            resultStr = moved + " " + "0" + " " + std::to_string(current_gd.side_to_move);
                            result = strdup(resultStr.c_str());
                            if(command == "movego"){
                                if(correct_move){
                                    game.move(moved);
                                }else{
                                    std::string invalid_move = "invalid_move";
                                    return strdup(invalid_move.c_str());
                                }
                            }
                            return result;
                        }
                    }
                
                }

                for(int i=0 ; i<ml._size; i++){
                    futures[i] = std::async(std::launch::async, 
                    [custom_depth, greediness, aggression, naivity, &ml, i]() {
                        return searchMove(custom_depth, greediness, aggression, naivity, ml, i);
                    });
                }

                for(int i=0 ; i<ml._size; i++){
                    // wait until the workers finish their jobs
                    top_search_results[i] = futures[i].get();
                }
                int index;
                if(!current_gd.side_to_move){ // white to move
                    int maxeval = -2000000;
                    for(int i=0;i<ml._size; i++){
                        if(top_search_results[i] > maxeval){
                            index = i;
                            eval = top_search_results[i];
                            maxeval = top_search_results[i];
                        }else if(top_search_results[i] == maxeval){

                        }
                    }
                }else{ //black to move
                    int mineval = 2000000;
                    for(int i=0;i<ml._size; i++){
                        if(top_search_results[i] < mineval){
                            index = i;
                            eval = top_search_results[i];
                            mineval = top_search_results[i];
                        }else if(top_search_results[i] == mineval){

                        }
                    }
                }
                std::vector <std::string> string_results;
                for(int i=0 ;i<ml._size;i++){
                    if(top_search_results[i] == eval){
                        string_results.push_back( move_to_uci(ml.move_list[i]) + " " + std::to_string(eval) + " " + std::to_string(current_gd.side_to_move));
                        //Myprintlog::log(notations[GET_SOURCE_SQUARE(ml.move_list[i])] + notations[GET_TARGET_SQUARE(ml.move_list[i])] + " " + std::to_string(eval) + " " + std::to_string(current_gd.side_to_move));
                    }
                }
                // Define the range for the distribution (0 to 5 inclusive)
                std::uniform_int_distribution<int> distribution(0, string_results.size()-1);

                // Generate a random integer between 0 and 5
                int randomNum = distribution(gen);
                Myprintlog::log(randomNum);
                resultStr = string_results[randomNum];
                std::istringstream issmoved(resultStr);
                issmoved >> moved >> std::ws; 
                //resultStr = notations[GET_SOURCE_SQUARE(ml.move_list[index])] + notations[GET_TARGET_SQUARE(ml.move_list[index])] + " " + std::to_string(eval) + " " + std::to_string(current_gd.side_to_move);
                
            #endif
            
            if(command == "movego" ){
                Myprintlog::log(moved);
                if(correct_move){
                    game.move(moved);
                }else{
                    std::string invalid_move = "invalid_move";
                    return strdup(invalid_move.c_str());
                }
            }
            result = strdup(resultStr.c_str());
            
        }else if(command == "show"){
            print_chessboard(game.get_gd());
            result = strdup(command.c_str());
        }else if(command == "list"){
            auto list = game.possible_moves();
            for(uint32_t p : list ){
                Myprintlog::log(piece_names[GET_PIECE(p)]+" "+notations[GET_SOURCE_SQUARE(p)] + " to "+ notations[GET_TARGET_SQUARE(p)] + " is capture?: " + std::to_string(IS_CAPTURE(p))
                + " is en_passant: "+std::to_string(IS_ENPASSANT(p))+" is double_push?: "+std::to_string(IS_DOUBLE_PUSH(p))
                +" is castling?: "+std::to_string(IS_CASTLING(p))
                );
            }
            Myprintlog::log("****************************");
            ArrayMoveList alist = game.generate_arraymoves();
            for(int i = 0 ; i< alist._first_size; i++ ){
                Myprintlog::log(piece_names[alist.first_move_list[i][2]]+" "+notations[alist.first_move_list[i][0]] + " to "+ notations[alist.first_move_list[i][1]] + " is capture?: " + std::to_string(alist.first_move_list[i][4])
                + " is en_passant: "+std::to_string(alist.first_move_list[i][6])+" is double_push?: "+std::to_string(alist.first_move_list[i][5])
                +" is castling?: "+std::to_string(alist.first_move_list[i][7])
                );
            }
            for(int i = 0 ; i< alist._second_size; i++ ){
                Myprintlog::log(piece_names[alist.second_move_list[i][2]]+" "+notations[alist.second_move_list[i][0]] + " to "+ notations[alist.second_move_list[i][1]] + " is capture?: " + std::to_string(alist.second_move_list[i][4])
                + " is en_passant: "+std::to_string(alist.second_move_list[i][6])+" is double_push?: "+std::to_string(alist.second_move_list[i][5])
                +" is castling?: "+std::to_string(alist.second_move_list[i][7])
                );
            }
            result = strdup(command.c_str());
        }else if (command == "perft") {
            // Handle perft command
            print_chessboard(game.get_gd());
            std::cout << "Perft depth: " << argument << std::endl;
            game_data copy_gd = game_data(game.get_gd());
            int depth = std::stoi(argument);

                // Start the timer
            auto start = std::chrono::high_resolution_clock::now();
            uint64_t nodes = perft(depth, copy_gd, true);
                // Stop the timer
            auto end = std::chrono::high_resolution_clock::now();
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            
            std::string resultStr = std::to_string(nodes) + " in" + std::to_string(milliseconds) + " milliseconds";
            Myprintlog::log(resultStr);

            result = strdup(resultStr.c_str());

        }else if(command == "uci"){
            
            game_data current_gd = game.get_gd();
            std::string uci = PGN_to_UCI(current_gd,argument);
            result = strdup(uci.c_str());
        }else if(command == "zobrist"){
            
            game_data current_gd = game.get_gd();
            uint64_t bhash = get_board_hash(current_gd);
            std::string hashstring = std::to_string(bhash);
            result = strdup(hashstring.c_str());

        }else if(command == "exit" || command =="q" || command =="quit"){

            old_game = Game(STANDARD_POSITION,4);
            game = Game(STANDARD_POSITION,4);

            result = strdup(STANDARD_POSITION.c_str());

        } else {

            std::string resultStr = "Invalid command " + command + " " + argument;
            result = strdup(resultStr.c_str());
        }


        return result;
    }


int searchMove(int depth , int greediness, int aggression,int naivity  , MoveList& ml, int i){
    uint8_t last_move[8] = {0,0,0,0,0,0,0,0};
    Best_line bl(greediness , aggression , naivity);
    int piece_values[6];
    int position_values[6][64];
    set_piece_values(piece_values , bl._greediness , bl._aggression, bl._naivity);
    set_position_values(position_values , bl._greediness , bl._aggression, bl._naivity);
    game_data move_gd = game.get_gd();
    make_move(move_gd, ml.move_list[i]);
    return search(depth - 1, move_gd, bl , -1000000 , 1000000 , piece_values , position_values, last_move, true);
}


#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
void _free(const char* ptr) {

    if (ptr != nullptr) {
        free((void*)ptr); // Free the memory using free
    }
}



#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
const char* costume_memory_allocation(){
    const char* str = new char[200]; // Allocate memory for 50 characters + null-terminator
    // Initialize the string (optional)
    // strcpy(str, "Your string here");
    return str;
}


std::string move_to_uci(uint32_t move){
   
    std::string pgn = "";
    pgn += notations[GET_SOURCE_SQUARE(move)];
    pgn += notations[GET_TARGET_SQUARE(move)];
    if(IS_CASTLING(move)){pgn += "*";}
    int pro = GET_PROMOTED_PIECE(move);
    if(pro){
        std::string pp [5] = {"k", "q", "r", "b", "n"};
        pgn += "="+pp[pro>6 ? pro-6 : pro];
    }
    return pgn;
}

std::string arraymove_to_uci(uint8_t (&move)[8]){
   
    std::string pgn = "";
    pgn += notations[move[0]];
    pgn += notations[move[1]];
    if(move[7]){pgn += "*";}
    uint8_t pro = move[3];
    if(pro){
        std::string pp [5] = {"k", "q", "r", "b", "n"};
        pgn += "="+pp[pro>6 ? pro-6 : pro];
    }
    return pgn;
}