#include <random>
#include "../include/evaluation.h"
#include "../include/moves.h"
#include "../include/bitboards.h"
#include <cstring>





void init_zobrist(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist(0.5);

    for (int piece = 0; piece < 12; ++piece) {
        for (int sqr = 0; sqr < 64; ++sqr) {
            uint64_t rn = 0;
            for(int i=0; i<64; i++){
                if(dist(gen)){
                    rn |= (1ULL << i);
                }
                
            }
            zobristTable[piece][sqr] = rn;  // Generate a random number for each piece-square combination
            //log(rn);
        }
    }
}

uint64_t get_board_hash(game_data& gd){
    uint64_t zobristHash = 0;

    for (int sqr = 0; sqr < 64; sqr++) {
        //int piece = getPieceAtSquare(square);  // Get the piece at the current square (e.g., 0 for an empty square)
        //int side = getSideAtSquare(square);    // Get the side at the current square (e.g., 0 for white, 1 for black)

        // if (piece != INVALID) {  // If there is a piece on the square
        //     zobristHash ^= zobristTable[piece][square];
        // }
    }
    return zobristHash;
}


// void Best_line::search(game_data& gd) {
//     _size = alpha_beta(_depth,gd, best_line_list , _alpha , _beta , _greediness); 
//         _last = best_line_list + _size;
// }
Best_line::Best_line(int greediness, int aggression,int naivity) : _greediness(greediness) , _aggression(aggression), _naivity(naivity) {};
const uint32_t* Best_line::begin() const { return best_line_list; }
const uint32_t* Best_line::end() const { return _last; }
size_t Best_line::size() const { return _last - best_line_list; }
bool Best_line::contains(uint32_t move) const {
    return std::find(begin(), end(), move) != end();
};



void set_piece_values(int* piece_values , int greediness, int aggression, int naivity){
    
    piece_values[0] = 10000; // king
    piece_values[1] = 900 - naivity + (90  - (greediness * 30)); // queen
    piece_values[2] = 500 - aggression; // rook
    piece_values[3] = 320 + aggression; // bishop
    piece_values[4] = 300 - naivity; // knight
    piece_values[5] = 100 - naivity;
}

void set_position_values(int (&position_values)[6][64] , int greediness, int aggression, int naivity){ // 2 , 30 , 20

    for(int i=0; i<64;i++){
        pawn_endgame_position_values[i] = 0;
    }
        // pawn positional score
    const int pawn_score[64] = {
        0,                   0,   0,   0,   0,   0,   0,   0,
        0,                   0,   0, -10 + naivity/10, -10 + naivity/10,   0,   0,   0,
        0  + aggression/8,   0,   0,   5 + naivity/8,      5 + naivity/8,       5 + naivity/8,   0,   0  + aggression/8,
        5  + aggression/10,   5,  10,  20 + greediness, 20 + greediness,  5,   5,   5  + aggression/10,
        10 + aggression/10,  10,  10,  20 + aggression/10,  20 + aggression/10,  10,  10,  10 + aggression/10,
        20 + aggression/10,  20,  20,  30 + aggression/10,  30 + aggression/10,  30,  20,  20 + aggression/10,
        30 + aggression/10,  30 + aggression/10,  30 + aggression/8,  40 + aggression/8,  40 + aggression/8,  30 + aggression/8,  30 + aggression/10,  30 + aggression/10,
        90 + aggression/10,  90,  90,  90,  90,  90,  90,  90 + aggression/10,
    };

    memcpy(position_values[5], pawn_score, sizeof(pawn_score));

    // knight positional score
    const int knight_score[64] = {
        -5                           ,     -10+ naivity/10,                   0,   0,  0,   0,     -10+ naivity/10,  -5,
        -5                           ,                   0,                   0,   0,  0,   0,                   0,  -5,
        -5 + aggression/8 + naivity/8,                   5,                  20,  10, 10,  20,                   5,  -5 + aggression/8 + naivity/8,
        -5 + aggression/8 + naivity/8,  10 + aggression/10,  20 + aggression/10,  30, 30,  20,                  10,  -5 + aggression/8 + naivity/8,
        -5 + aggression/8 + naivity/8,  10 + aggression/8 ,  20 + aggression/10,  30, 30,  20,                  10,  -5 + aggression/8 + naivity/8,
        -5 + aggression/8 + naivity/8,  10 + aggression/8 ,  20 + aggression/10,  20, 20,  20,  10 + aggression/10,  -5 + aggression/8 + naivity/8,
        -5 + aggression/8 + naivity/8,  0  + aggression/8 ,   0 + aggression/10,  10, 10,   0,                   0,  -5 + aggression/8 + naivity/8,
        -5 + naivity/8,                                  0,                   0,   0,  0,   0,                   0,  -5 + naivity/8,
    };

    memcpy(position_values[4], knight_score, sizeof(knight_score));
 
    // bishop positional score
    const int bishop_score[64] = {
        0,   0, -10 + naivity/8,   0,   0, -10 + naivity/8,   0,   0,
        0,  30,               0,   0,   0,               0,  30,   0,
        0,  10,              20,   0,   0,              20,  10,   0,
        0,   0,              10,  20,  20,              10,   0,   0,
        0,   0,              10,  20,  20,              10,   0,   0,
        0,   0,              20,  10,  10,              20,   0,   0,
        0,  20,               0,   0,   0,               0,   20,   0,
        0,   0,               0,   0,   0,               0,   0,   0,

    };
 
    memcpy(position_values[3], bishop_score, sizeof(bishop_score));

    // rook positional score
    const int rook_score[64] ={
        0,   15,  20,  20,  20,  10,  15,  0,
        10,  15,  10,  10,  20,  10,  15,  10,
        10,  15,  10 + naivity/8,   0 + naivity/8,   0 + naivity/8,  10 + naivity/8,  15,  10,
        10,  15,  10 + naivity/8,   0 + naivity/8,   0 + naivity/8,  10 + naivity/8,  15,  10,
        10,  15,  10 + naivity/8,   0 + naivity/8,   0 + naivity/8,  10 + naivity/8,  15,  10,
        10,  15,  10 + naivity/8,   0 + naivity/8,   0 + naivity/8,  10 + naivity/8,  15,  10,
        20 + aggression/8,  20 + aggression/8,  20 + aggression/8,  20 + aggression/8,  20 + aggression/8,  20 + aggression/8,  20 + aggression/8,  20 + aggression/8,
        20 + aggression/10,  30 + aggression/8,  30 + aggression/8,  20 + aggression/8,  20 + aggression/8,  30 + aggression/8,   30 + aggression/8,  20 + aggression/10,

    };
 
    memcpy(position_values[2], rook_score, sizeof(rook_score));

    const int queen_score[64] ={
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
        0 , 0 , 0 + greediness , 0 + greediness , 0 + greediness , 0 + greediness , 0 , 0 ,
        0 + aggression/10 + naivity/10 , 0 + aggression/8 + naivity/10 , 0 + aggression/8 + naivity/10 , 0 + aggression/8 + naivity/8 , 0 , 0 , 0 , 0 ,
        0 + aggression/10 + naivity/8 , 0 + aggression/8 + naivity/8 , 0 + aggression/8 + naivity/8 , 0 + aggression/8 + naivity/8 , 0 , 0 , 0 , 0 ,
        0 + aggression/10 + naivity/8 , 0 + aggression/8 + naivity/8 , 0 + aggression/8 + naivity/8 , 0 + aggression/8 + naivity/8 , 0 , 0 , 0 , 0 ,
        0 + aggression/10 + naivity/8 , 0 + aggression/8 + naivity/8 , 0 + aggression/8 + naivity/8 , 0 + aggression/8 + naivity/8 , 0 , 0 , 0 , 0 ,
        0 + aggression/10 , 0 + aggression/8 , 0 + aggression/8 , 0 + aggression/8 + naivity/8 , 0 , 0 , 0 , 0 ,
        0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
    };

    memcpy(position_values[1], queen_score, sizeof(queen_score));

    // king positional score
    const int king_score[64] = {
        0,   6,   5,   0, -15,   6,  10,   0,
        0,   5,   5,  -5,  -5,   0,   5,   0,
        0,   0,   5,  10,  10,   5,   0,   0,
        0,   5,  10,  20,  20,  10,   5,   0,
        0,   5,  10,  20,  20,  10,   5,   0,
        0,   5,   5,  10,  10,   5,   5,   0,
        0,   0,   5,   5,   5,   5,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
    };

    memcpy(position_values[0], king_score, sizeof(king_score));
}


// uint64_t perft(int depth, game_data& gd, bool main = true){
//     uint64_t nodes = 0;
//     //int move_nodes =0;
//     MoveList move_list;
//     move_list.generate(gd);
//     uint64_t n_moves = move_list._size;
//     //log(n_moves);
//     if(depth == 1){return n_moves;}
 
//     game_data ori(gd);
//     for( uint32_t move : move_list){
//         make_move(gd, move);
//         uint64_t inner_node = perft(depth-1 ,gd, false);
//         nodes += inner_node;
//         if(main){log(notations[GET_SOURCE_SQUARE(move)]+notations[GET_TARGET_SQUARE(move)] + " " + std::to_string(inner_node));inner_node=0;}
//         //nodes += perft(depth-1 , gd, false);
//         gd = game_data(ori);
//     }
//     return nodes;
// };

int alpha_beta(int depth ,game_data& gd, Best_line& bl , int alpha, int beta , int (&piece_values)[6], int (&position_values)[6][64]){

    MoveList move_list;
    move_list.generate(gd);
    if(!move_list._size){
        //print_chessboard(gd);
        int sqr = !gd.side_to_move? gd.wking_sqr : gd.bking_sqr;
        int side = gd.side_to_move ^ 1;
        if ( is_square_attacked( gd,sqr ,side) ){ //checkmate
            return !gd.side_to_move ? -1000000 : 1000000;
        }else{
            return 0;
        };
    }
    int eval = 0;
    if(depth == 0){
        return evaluate(gd, bl._greediness, bl._aggression, bl._naivity , piece_values , position_values );
    }

    game_data ori(gd);
     
    if(!gd.side_to_move){ // white to move
        int maxeval = -2000000;
        int best_move = move_list[0];
        for(uint32_t move : move_list){
            make_move(gd, move);
            eval = alpha_beta(depth -1 ,gd, bl , alpha, beta , piece_values , position_values);
            gd = game_data(ori);
            if(eval > maxeval){
                best_move = move;
                maxeval = eval;
            }
            alpha = std::max(alpha , eval);

            if(beta <= alpha){

                break;
            }
            // else if(eval >= (bl._greediness * 100 )){
            //     maxeval = eval;
            //     best_move = move;
            //     break;
            // }
        }
        bl.best_line_list[depth] = best_move;
        //bl._size ++;
        return maxeval;

    }else{ // black to move
        int mineval = 2000000;
        int best_move = move_list[0];
        for(uint32_t move : move_list){
            make_move(gd, move);
            eval = alpha_beta(depth -1 ,gd, bl , alpha, beta , piece_values , position_values);
            if(eval < mineval){
                best_move = move;
                mineval = eval;
            }
            beta = std::min(beta , eval);
            gd = game_data(ori);
            if(beta <= alpha){
 
                break;
            }
            // else if(eval <= -bl._greediness){
            //     mineval = eval;
            //     best_move= move;
            //     break;
            // }
        }
        bl.best_line_list[depth] = best_move;
        //bl._size ++;
        return mineval;
    }

}; 


int search(int depth ,game_data& gd, Best_line& bl , int alpha, int beta , int (&piece_values)[6], int (&position_values)[6][64] , uint8_t (&last_move)[8], int main = false){

    ArrayMoveList ml;
    ml.generate(gd);
    if(!ml._first_size && !ml._second_size){
        //print_chessboard(gd);
        int sqr = !gd.side_to_move? gd.wking_sqr : gd.bking_sqr;
        int side = gd.side_to_move ^ 1;
        if ( is_square_attacked( gd,sqr ,side) ){ //checkmate
            return !gd.side_to_move ? -1000000 : 1000000;
        }else{
            return 0;
        };
    }
    int eval = 0;
    if(depth == 0 ){
        if(last_move[3] == 1 || last_move[4] == 1){ // if capture or promotion

            uint8_t m[8] = {0,0,0,0,0,0,0,0};
            return quiescence(0,gd, bl , alpha, beta , piece_values , position_values , last_move);
        }
        return evaluate(gd, bl._greediness, bl._aggression, bl._naivity , piece_values , position_values);
    }

    game_data ori(gd);
     
    if(!gd.side_to_move){ // white to move
        int maxeval = -2000000;
        uint8_t* best_move = ml.first_move_list[0];

        for( int i=0 ; i < ml._first_size ; i++){
            //uint8_t* move = ml.move_list[i];
            uint8_t (&move)[8] = ml.first_move_list[i];
            make_arraymove(gd, move);
            eval = search(depth -1 ,gd, bl , alpha, beta , piece_values , position_values , move);
            gd = game_data(ori);
            if(eval > maxeval){
                best_move = move;
                maxeval = eval;
            }
            alpha = std::max(alpha , eval);

            if(beta <= alpha){

                break;
            }
        }
        for( int i=0 ; i < ml._second_size ; i++){
            //uint8_t* move = ml.move_list[i];
            uint8_t (&move)[8] = ml.second_move_list[i];
            make_arraymove(gd, move);
            eval = search(depth -1 ,gd, bl , alpha, beta , piece_values , position_values , move);
            gd = game_data(ori);
            if(eval > maxeval){
                best_move = move;
                maxeval = eval;
            }
            alpha = std::max(alpha , eval);

            if(beta <= alpha){

                break;
            }

        }
        //bl._size ++;
        if(main){
            bl.best_move = best_move;
        }
        return maxeval;

    }else{ // black to move
        int mineval = 2000000;
        uint8_t* best_move = ml.first_move_list[0];

        for( int i=0 ; i < ml._first_size ; i++){
            //uint8_t* move = ml.move_list[i];
            uint8_t (&move)[8] = ml.first_move_list[i];
            make_arraymove(gd, move);
            eval = search(depth -1 ,gd, bl , alpha, beta , piece_values , position_values, move);
            if(eval < mineval){
                best_move = move;
                mineval = eval;
            }
            beta = std::min(beta , eval);
            gd = game_data(ori);
            if(beta <= alpha){
 
                break;
            }

        }
        for( int i=0 ; i < ml._second_size ; i++){
            //uint8_t* move = ml.move_list[i];
            uint8_t (&move)[8] = ml.second_move_list[i];
            make_arraymove(gd, move);
            eval = search(depth -1 ,gd, bl , alpha, beta , piece_values , position_values, move);
            if(eval < mineval){
                best_move = move;
                mineval = eval;
            }
            beta = std::min(beta , eval);
            gd = game_data(ori);
            if(beta <= alpha){
 
                break;
            }

        }

        if(main){
            bl.best_move = best_move;
        }
        //bl._size ++;
        return mineval;
    }

}; 


int quiescence(int depth,game_data& gd, Best_line& bl , int alpha, int beta , int (&piece_values)[6], int (&position_values)[6][64], uint8_t (&last_move)[8]){

    bool only_trades = false;
    ArrayMoveList ml;
    ml.generate(gd);
    if(!ml._first_size){
        if (!ml._second_size){
            //print_chessboard(gd);
            int sqr = !gd.side_to_move? gd.wking_sqr : gd.bking_sqr;
            int side = gd.side_to_move ^ 1;
            if ( is_square_attacked( gd,sqr ,side) ){ //checkmate
                return !gd.side_to_move ? -1000000 : 1000000;
            }else{
                return 0;
            };
        }

        return evaluate(gd, bl._greediness, bl._aggression, bl._naivity , piece_values , position_values );
    }


    game_data ori(gd);

    int eval = 0;
    if(depth == 0){
        eval = evaluate(gd, bl._greediness, bl._aggression, bl._naivity , piece_values , position_values);
        for( int i=0 ; i < ml._first_size ; i++){
            //uint8_t* move = ml.move_list[i];
            if( (ml.first_move_list[i][2] == last_move[2]) || ml.first_move_list[i][3]){ // only trade moves or promotions
                uint8_t (&move)[8] = ml.first_move_list[i];
                make_arraymove(gd, move);
                eval = quiescence(depth,gd, bl , alpha, beta , piece_values , position_values, move);
                gd = game_data(ori);
                break;
            }
        }
        return eval;
    }

     
    if(!gd.side_to_move){ // white to move
        int maxeval = -2000000;
        uint8_t* best_move = ml.first_move_list[0];

        for( int i=0 ; i < ml._first_size ; i++){
            //uint8_t* move = ml.move_list[i];
            uint8_t (&move)[8] = ml.first_move_list[i];
            make_arraymove(gd, move);
            eval = quiescence(depth-1,gd, bl , alpha, beta , piece_values , position_values, move);
            gd = game_data(ori);
            if(eval > maxeval){
                best_move = move;
                maxeval = eval;
            }
            alpha = std::max(alpha , eval);

            if(beta <= alpha){

                break;
            }
        }

        return maxeval;

    }else{ // black to move
        int mineval = 2000000;
        uint8_t* best_move = ml.first_move_list[0];

        for( int i=0 ; i < ml._first_size ; i++){
            //uint8_t* move = ml.move_list[i];
            uint8_t (&move)[8] = ml.first_move_list[i];
            make_arraymove(gd, move);
            eval = quiescence(depth-1,gd, bl , alpha, beta , piece_values , position_values,move);
            if(eval < mineval){
                best_move = move;
                mineval = eval;
            }
            beta = std::min(beta , eval);
            gd = game_data(ori);
            if(beta <= alpha){
 
                break;
            }

        }
     
        return mineval;
    }


};


// int evaluate(game_data& gd, int greediness , int aggression , int naivity , int (&piece_values)[6] , int (&position_values)[6][64]){
    
//     int eval = 0;

//     for(int i=0 ; i<6 ; i++){ // white
//         uint64_t bitboard = gd.bitboards[i];
//         while(bitboard){
//             int sqr = get_first_square(bitboard);
//             eval += piece_values[i];
//             eval += position_values[i][sqr];
//             if(i==0){ // king safety

//             }
//             DEL_BIT(bitboard,sqr);
//         }
//     }

//     for(int i=6 ; i<12 ; i++){// black
//         uint64_t bitboard = gd.bitboards[i];
//         while(bitboard){
//             int sqr = get_first_square(bitboard);
//             eval -= piece_values[i-6];
//             eval -= position_values[i-6][mirror_score[sqr]];
//             if(i==6){ // king safety

//             }
//             DEL_BIT(bitboard,sqr);
//         }
//     }

//     return eval;
// }

void endgame_king_proximity(game_data& gd, int (&position_values)[6][64]){

    int opponentKingSquare = gd.side_to_move ? gd.wking_sqr : gd.bking_sqr;
    
    for (int sqr = 0; sqr < 64; sqr++) {
        int file = sqr % 8;  // Get the file of the current square
        int rank = sqr / 8;  // Get the rank of the current square
         
        // Calculate the distance between the current square and the opponent king
        int distance = std::abs(file - (opponentKingSquare % 8)) + std::abs(rank - (opponentKingSquare / 8));

        // Adjust the position values based on the distance to the opponent king
        if (distance <= 3) {
            position_values[0][sqr] += 10;  // Increase the position value for closer squares
        } else if (distance <= 6) {
            position_values[0][sqr] += 5;   // Increase the position value for squares at a moderate distance
        } else if (distance <= 10) {
            position_values[0][sqr] += 2;   // Increase the position value for squares at a farther distance
        }
    }
}

int pawn_endgame_eval(game_data& gd ,int eval){
    
    int whiteKingSquare = gd.wking_sqr;
    int blackKingSquare = gd.bking_sqr;

    int white_rank = (int)whiteKingSquare / 8;
    int black_rank = (int)blackKingSquare / 8;

    int rank_difference = black_rank - white_rank;

    if(rank_difference > 0){
        int init_val = 0;

        if((int)whiteKingSquare % 8 == (int)blackKingSquare % 8){//same file
            init_val = 10;
        }

        if(rank_difference%2){ // odd kings opposition
            if(!gd.side_to_move){
                eval += 30 + init_val;
            }else{
                eval -= 30 + init_val;
            }
            
        }else{

            if(!gd.side_to_move){
                eval -= 30 + init_val;
            }else{
                eval += 30 + init_val;
            }
        }

    }

    return eval;

}


void king_edge_penalty(int (&position_values)[6][64]){

    for (int sqr = 0; sqr < 64; sqr++) {

        int file = sqr % 8;  // Get the file of the current square
        int rank = sqr / 8;  // Get the rank of the current square

        // if (file == 1 || file == 6 ) {
        //     // Apply corner penalty to the king position
        //     position_values[0][sqr] -= 20;
        // }else if(rank == 0  || rank == 7){

        //     position_values[0][sqr] -= 25;
        // }
        if (file == 0 || file == 7 || rank == 0 || rank == 7 ) {
            position_values[0][sqr] -= 40;
        }

    }
}



int evaluate(game_data& gd, int greediness , int aggression , int naivity , int (&piece_values)[6] , int (&position_values)[6][64]){
    
    int eval = 0;
    bool pawn_endgame = true;

    for(int i=1; i<5; i++){ 
        if(gd.piece_numbers[i]){
            pawn_endgame = false; 
            break; 
        } 
    }
    if(pawn_endgame){
        memcpy(position_values[0], pawn_endgame_position_values, sizeof(pawn_endgame_position_values));
        eval = pawn_endgame_eval(gd, eval);
    }

    if(!gd.piece_numbers[1] && !gd.piece_numbers[2]){//no queen and rook - end game
        
        endgame_king_proximity(gd, position_values);
    }else if( (gd.piece_numbers[1] + gd.piece_numbers[2]) <= 3 ){
        king_edge_penalty(position_values);
    }


    for(int i=0 ; i<64 ; i++){ 
    
        if(gd.mailbox[i]!= INVALID){
           
            
            if(gd.mailbox[i] < 6){
                eval += piece_values[gd.mailbox[i]];
                eval += position_values[gd.mailbox[i]][i];
            }else{

                eval -= piece_values[gd.mailbox[i] - 6];
                eval -= position_values[gd.mailbox[i] - 6][mirror_score[i]];
            }
        }
     
    }
    
    // king safety
    if(gd.piece_numbers[1]){// queen on the board
        
        eval += count_bits(raw_attacks[0][gd.wking_sqr] & gd.bitboards[5]) * 10;
        eval -= count_bits(raw_attacks[6][gd.bking_sqr] & gd.bitboards[11]) * 10;
    }


    return eval;
}