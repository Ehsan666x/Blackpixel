#include <sstream>
#include <cstring>
#include "../include/Blackpixel.h"

#include <emscripten.h>



EMSCRIPTEN_KEEPALIVE
const char* Blackpixel_engine(const char* input, int depth , int greediness, int aggression,int naivity){
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
            int eval = search(std::stoi(argument), current_gd, bl , -1000000 , 1000000 , piece_values , position_values, last_move, true);
            
            std::string resultStr = notations[bl.best_move[0]] + notations[bl.best_move[1]];

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




