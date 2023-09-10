#include <sstream>
#include <cstring>
#include <future>
#include <array> 
#include "../include/Blackpixel.h"

#include <emscripten.h>



EMSCRIPTEN_KEEPALIVE
const char* Blackpixel_engine(const char* input, int depth , int greediness, int aggression,int naivity,int numWorkers ,int workerid ){
        const char* result;

        if(!depth){depth=1;}

        std::string userinput(input);
        std::string command;
        std::string argument;


        std::istringstream iss(userinput);

        iss >> command >> std::ws; // Extract the command and skip leading whitespaces
        std::getline(iss, argument); // Read the

        if (command == "move") {
            // Handle move command
            Game blueprint(game.get_fen(),1);
            old_game = blueprint;
            //std::cout << "Moving: " << argument << std::endl;
            game.move(argument);
            //print_chessboard(game.get_gd());
            result = strdup(argument.c_str());
        } else if (command == "fen") {

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

            if(command == "movego"){
                Game blueprint(game.get_fen(),1);
                old_game = blueprint;
                game.move(argument);
            }
            Best_line bl(greediness , aggression , naivity);
            int piece_values[6];
            int position_values[6][64];
            set_piece_values(piece_values , bl._greediness , bl._aggression, bl._naivity);
            set_position_values(position_values , bl._greediness , bl._aggression, bl._naivity);
            game_data current_gd = game.get_gd();
            
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
            
                resultStr = notations[bl.best_move[0]] + notations[bl.best_move[1]] + " " + std::to_string(eval) + " " + std::to_string(current_gd.side_to_move);

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
                        }
                    }
                }else{ //black to move
                    int mineval = 2000000;
                    for(int i=0;i<ml._size; i++){
                        if(top_search_results[i] < mineval){
                            index = i;
                            eval = top_search_results[i];
                            mineval = top_search_results[i];
                        }
                    }
                }

                resultStr = notations[GET_SOURCE_SQUARE(ml.move_list[index])] + notations[GET_TARGET_SQUARE(ml.move_list[index])] + " " + std::to_string(eval) + " " + std::to_string(current_gd.side_to_move);
                
            #endif
           
            result = strdup(resultStr.c_str());
            
        }else if(command == "show"){
            print_chessboard(game.get_gd());
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


EMSCRIPTEN_KEEPALIVE
void _free(const char* ptr) {

    if (ptr != nullptr) {
        free((void*)ptr); // Free the memory using free
    }
}



EMSCRIPTEN_KEEPALIVE
const char* costume_memory_allocation(){
    const char* str = new char[200]; // Allocate memory for 50 characters + null-terminator
    // Initialize the string (optional)
    // strcpy(str, "Your string here");
    return str;
}