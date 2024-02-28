#include <iostream>

#include "../include/globals.h"
#include "../include/bitboards.h"
#include "../include/moves.h" // attacks.h
//#include "../include/attacks.h"
#include "../include/pins.h"
#include "../include/prints.h"




uint64_t sqr_legal_moves(int& sqr,game_data& gd){
    if(! (gd.occupancy[gd.side_to_move] & (1ULL << sqr) ) ){ // checking if a right piece exists

        return 0;
    }
    int index=0; int i=0;
    if(!gd.side_to_move){
        index=6;
        i=0; 
    }else if(gd.side_to_move==1){
        index=12;
        i=6;
    }
    for( i ; i<index; i++){
        if(gd.bitboards[i] & (1ULL << sqr) ){ // found the piece kind
            int op=0;
            if(!gd.side_to_move){op=1;}
            //uint64_t raw_attack = raw_attacks[i][sqr];
            uint64_t lm = 0ULL;
            //lm = refine_own_occupency_attack(i , sqr ,raw_attack,gd.occupancy[gd.side_to_move]); // i = piece kind 
            // /lm = refine_own_occupency_attack(i , sqr ,lm, gd.occupancy[op]);

            // other logics (in check / pin)

            return 1ULL;
        }
    }
    return 0;
};



void MoveList::generate(game_data& gd) {
     _size=generate_moves(gd, move_list); 
        _last = move_list + _size;
}

const uint32_t* MoveList::begin() const { return move_list; }
const uint32_t* MoveList::end() const { return _last; }
size_t MoveList::size() const { return _last - move_list; }
bool MoveList::contains(uint32_t move) const {
    return std::find(begin(), end(), move) != end();
};



ArrayMoveList ::ArrayMoveList(){
    for (int i=0 ; i< MAX_MOVES; i++){
        for(int j=0;j<8;j++){
            first_move_list[i][j]=0;
            second_move_list[i][j]=0;
        }
    }
    for (int i=0 ; i< MAX_DEPTH; i++){
        for(int j=0;j<2;j++){
            for(int k=0;k<2;k++){
                history_moves[i][j][k] = 0;
                killer_moves[i][j][k] = 0;
            }
        }
    }
};

void ArrayMoveList::generate(game_data& gd) {
    Size_array sa = generate_arraymoves(gd, first_move_list, second_move_list); 
    //_last = move_list[0] + _size;
    _first_size = sa.first;
    _second_size = sa.second;
}

// const uint8_t* ArrayMoveList::begin() const { return move_list[0]; }
// const uint8_t* ArrayMoveList::end() const { return _last; }
// size_t ArrayMoveList::size() const { return _last - move_list[0]; }
// bool ArrayMoveList::contains(uint8_t (&move)[8]) const {
//     for (const auto& array : move_list) {
//         if (std::equal(std::begin(array), std::end(array), std::begin(move)))
//             return true;
//     }
//     return false;
// }


/////////////// move generation ////////////////




Size_array generate_arraymoves(game_data& gd, uint8_t (&first_move_list)[MAX_MOVES][8], uint8_t (&second_move_list)[MAX_MOVES][8]){
    // gd.side_tomove 0 white 1 black
    // legal moves //
    // 1- check if in check
    // 2- get enemy pin mask
    // 2- if check =>get own pin mask, generate specific moves based on enemy and own pin mask 
    //    else => generate moves based on enemy pin mask
    Size_array sa;
    //int move_count=0;
    uint64_t check_attacker[2]={INVALID, INVALID};
    get_checks((!gd.side_to_move? gd.wking_sqr: gd.bking_sqr),gd, check_attacker);

    if(check_attacker[0]==INVALID){ // not in check
        uint64_t pins = pin_mask(gd);
        for(int sqr=0; sqr<64; sqr++){
            uint8_t i = gd.mailbox[sqr];
            if(i != INVALID){
                if((!gd.side_to_move && i<6) || (gd.side_to_move && i >=6)){

                    uint64_t attack_mask = 0ULL;
                    //uint32_t moves[256]; 
                    if(i==5 || i==11){ // white pawn or black pawn
                        // uint64_t copy_bitboard = gd.bitboards[i];
                        // while (copy_bitboard){
                        //     int sqr = get_first_square(copy_bitboard);
                            attack_mask = raw_attacks[i==5 ? 5 :6][sqr]; // white pawn or black
                            int move = INVALID;
                            while (attack_mask){
                                int target = get_first_square(attack_mask);
                                //int capture = 0;
                                int en_passant = 0;
                                //int double_push = 0;

                                if((i<6 ? (target >= 56): (target<8) ) && (gd.occupancy[i<6 ? 1 : 0] & (1ULL << target) ) ) {
                                    // promotion capture
                                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, 1, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                    
                                        for(int j=(i< 6 ? 1 : 7);j<(i < 6 ? 5 : 11);j++){ // queen , rook , bishop, knight
                                            move = sqr;
                                            ASSIGN_MOVE(first_move_list, sa.first ,move, target, i, j, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                            sa.first++;
                                            //moves[move_count -1]=move;
                                        };
                                        move = INVALID;
                                    }
                                }else if(gd.occupancy[i<6 ? 1 : 0] & (1ULL << target)){ // 
                                    // pawn capture
                                    //capture = 1;
                                    move = sqr;

                                }else if(gd.en_passant && gd.en_passant== target && !(gd.occupancy[2] & (1ULL << target))){
                                    // en passant capture
                                    en_passant = 1;
                                    //capture = 1;
                                    move = sqr;
                                };

                                if(move!=INVALID){
                                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, 1, en_passant)){ //orig_gd,sqr,target,piece,capture,enpassant
                                        ASSIGN_MOVE(first_move_list, sa.first,move, target, i, 0, 1, 0, en_passant, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.first++;
                                    }
                                    move = INVALID;
                                }
                                
                                DEL_BIT(attack_mask,target);
                            };

                            if( !(gd.occupancy[2] & ( 1ULL << (sqr + (i==5 ? 8 : -8)) ) ) ){ 
                                
                                if( (i<6 ? (sqr<56 && sqr >= 48): (sqr>=8 && sqr<16) )  &&( !((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (sqr + (i==5 ? 8 : -8)), i, 0, 0) ) ){
                                    // promotion
                                    for(int j=(i< 6 ? 1 : 7);j<(i < 6 ? 5 : 11);j++){ // queen , rook , bishop, knight
                                        move = sqr;
                                        ASSIGN_MOVE(first_move_list, sa.first ,move, (sqr + (i==5 ? 8 : -8)), i, j, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.first++;
                                        //moves[move_count -1]=move;
                                    };

                                }else if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (sqr + (i==5 ? 8 : -8)), i, 0, 0)) {
                                    // single push_pawn
                                    if(check_potential(gd,sqr, (sqr + (i==5 ? 8 : -8)), i)){
                                        ASSIGN_MOVE( first_move_list, sa.first ,sqr, (sqr + (i==5 ? 8 : -8)), i, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.first++;
                                    }else{
                                        ASSIGN_MOVE( second_move_list, sa.second ,sqr, (sqr + (i==5 ? 8 : -8)), i, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++;
                                    }
                                };
                                
                                if( (((int)sqr/8 == 1 && i == 5) || ((int)sqr/8 == 6 && i == 11)) && ! (gd.occupancy[2] & ( 1ULL << (sqr + (i==5 ? 16 : -16)) )) ){
                                        // double push
                                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (sqr + (i==5 ? 16 : -16)), i, 0, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                    
                                        if(check_potential(gd,sqr, (sqr + (i==5 ? 16 : -16)), i)){
                                            ASSIGN_MOVE(first_move_list, sa.first ,sqr, (sqr + (i==5 ? 16 : -16)), i, 0, 0, 1, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                            sa.first++;
                                        }else{

                                            ASSIGN_MOVE(second_move_list, sa.second ,sqr, (sqr + (i==5 ? 16 : -16)), i, 0, 0, 1, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                            sa.second++;
                                        }
                                    }
                                }

                            };

                            //DEL_BIT(copy_bitboard,sqr);
                        //};


                    }else if( i == 0 || i == 4 || i == 6 || i == 10){ // king or knight

                            //attack_mask = raw_attacks[i][sqr];
                            attack_mask = raw_attacks[(i == 0 || i == 6)? 0 : 4][sqr]; //king or knight
                            
                            //int move = INVALID;
                            while (attack_mask){
                                int target = get_first_square(attack_mask);
                                //int capture=0;
                                
                                if(gd.occupancy[i<6 ? 1 : 0] & (1ULL << target)){ // 
                                    // capture
                                    //move = sqr;
                                    //capture = 1;

                                    if((i==0 || i==6) && !move_and_check(gd, sqr, target, i, 1, 0)){
                                        ASSIGN_MOVE( first_move_list, sa.first ,sqr, target, i, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.first++;
                                    }else if((i==4 || i==10) &&(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, 1, 0) )){ //orig_gd,sqr,target,piece,capture,enpassant
                                        

                                        ASSIGN_MOVE( first_move_list , sa.first ,sqr, target, i, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.first++;
                                    }
                                }else if ( !(gd.occupancy[2] & (1ULL << target) ) ){ 
                                    // regular move
                                    //move = sqr;
                                    if((i==0 || i==6) && !move_and_check(gd, sqr, target, i, 0, 0)){
                                        ASSIGN_MOVE( second_move_list, sa.second ,sqr, target, i, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++;
                                    }else if((i==4 || i==10) &&(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, 0, 0) )){ //orig_gd,sqr,target,piece,capture,enpassant
                                        bool pot = check_potential(gd,sqr,target,i);

                                        ASSIGN_MOVE((pot ? first_move_list : second_move_list) , (pot ? sa.first : sa.second) ,sqr, target, i, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        pot ? sa.first++ : sa.second++;
                                    }
                                };

                                // if(move!=INVALID){
                                //     if((i==0 || i==6) && !move_and_check(gd, sqr, target, i, capture, 0)){
                                //         ASSIGN_MOVE( (capture ? first_move_list : second_move_list) , (capture ? sa.first : sa.second) ,move, target, i, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                //         capture ? sa.first++ : sa.second++;
                                //     }else if((i==4 || i==10) &&(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, capture, 0) )){ //orig_gd,sqr,target,piece,capture,enpassant
                                //         ASSIGN_MOVE((capture ? first_move_list : second_move_list) , (capture ? sa.first : sa.second) ,move, target, i, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                //         capture ? sa.first++ : sa.second++;
                                //     }
                                //     move = INVALID;
                                // }
                                DEL_BIT(attack_mask,target);
                            };

                        if(gd.castles && (i==0||i==6)){//king
                            //check for castling regarding fischer random
                            if(i==0 ){//white
                                if(gd.castles & 1){ // king side
                                    int kingside=1;
                                    for(int f=gd.wr1+1; f<gd.wk; f++ ){ // between king and rook
                                        if(gd.occupancy[2] & (1ULL << f)){
                                            kingside = 0;
                                            break;
                                        };
                                    };
                                    if(kingside){
                                        if(gd.wr1>g1){
                                            for(int f=g1; f<gd.wr1; f++ ){ // between rook and g1 
                                                if(gd.occupancy[2] & (1ULL << f)){
                                                    kingside = 0;
                                                    break;
                                                };
                                            };
                                        };
                                    };
                                    if(kingside){
                                        for(int sq=g1; sq<gd.wk; sq++){ // attacks between king and g1
                                            if(IS_SQUARE_ATTACKED(gd,sq,1)){
                                                kingside = 0;
                                                break;                                           
                                            }
                                        }
                                    };
                                    if(kingside && !move_and_check(gd , gd.wk, g1, i, 0, 0, kingside) ){ //orig_gd,sqr,target,piece,capture,enpassant , castling
                                        ASSIGN_MOVE(second_move_list, sa.second,gd.wk, g1, i, 0, 0, 0, 0, kingside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++;
                                    };
                                
                                };
                                if(gd.castles & 2){ // queen side

                                    int queenside=1;
                                    if(gd.wk <=d1){
                                        for(int f=gd.wk+1; f<gd.wr2; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                        if(gd.wr2 <c1){
                                            for(int f=gd.wr2+1; f<=c1; f++ ){
                                                if(gd.occupancy[2] & (1ULL << f)){
                                                    queenside = 0;
                                                    break;
                                                };
                                            };
                                        }
                                    }else{
                                        for(int f=gd.wk+1; f<gd.wr2; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                        for(int f=d1; f<gd.wk; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                    }

                                    if(queenside){
                                        if(gd.wk <c1){
                                            for(int sq=gd.wk+1; sq<=c1; sq++){
                                                if(IS_SQUARE_ATTACKED(gd,sq,1)){
                                                    queenside = 0;
                                                    break;                                           
                                                }
                                            }
                                        }else{
                                            for(int sq=c1; sq<gd.wk; sq++){
                                                if(IS_SQUARE_ATTACKED(gd,sq,1)){
                                                    queenside = 0;
                                                    break;                                           
                                                }
                                            }
                                        }
                                    };
                                    if(queenside && !move_and_check(gd , gd.wk, c1, i, 0, 0, queenside)){
                                        ASSIGN_MOVE(second_move_list, sa.second,gd.wk, c1, i, 0, 0, 0, 0, queenside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++;
                                    };

                                }

                            }else{//black
                                if(gd.castles & 4){ // king side
                                    int kingside=1;
                                    for(int f=gd.br1+1; f<gd.bk; f++ ){
                                        if(gd.occupancy[2] & (1ULL << f)){
                                            kingside = 0;
                                            break;
                                        };
                                    };
                                    if(kingside){
                                        if(gd.br1>g8){
                                            for(int f=g8; f<gd.br1; f++ ){
                                                if(gd.occupancy[2] & (1ULL << f)){
                                                    kingside = 0;
                                                    break;
                                                };
                                            };
                                        };
                                    };
                                    if(kingside){
                                        for(int sq=g8; sq<gd.bk; sq++){
                                            if(IS_SQUARE_ATTACKED(gd,sq,0)){
                                                kingside = 0;
                                                break;                                           
                                            }
                                        }
                                    };                                
                                    if(kingside && !move_and_check(gd , gd.bk, g8, i, 0, 0, kingside)){
                                        ASSIGN_MOVE(second_move_list, sa.second,gd.bk, g8, i, 0, 0, 0, 0, kingside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++;
                                    };
                                
                                };
                                if(gd.castles & 8){ // queen side

                                    int queenside=1;
                                    if(gd.bk <=d8){
                                        for(int f=gd.bk+1; f<gd.br2; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                        if(gd.br2 <c8){
                                            for(int f=gd.br2+1; f<=c8; f++ ){
                                                if(gd.occupancy[2] & (1ULL << f)){
                                                    queenside = 0;
                                                    break;
                                                };
                                            };
                                        }
                                    }else{
                                        for(int f=gd.bk+1; f<gd.br2; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                        for(int f=d8; f<gd.bk; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                    }

                                    if(queenside){
                                        if(gd.bk < c8){
                                            for(int sq=gd.bk+1; sq<=c8; sq++){
                                                if(IS_SQUARE_ATTACKED(gd,sq,0)){
                                                    queenside = 0;
                                                    break;                                           
                                                }
                                            }
                                        }else{
                                            for(int sq=c8; sq<gd.bk; sq++){
                                                if(IS_SQUARE_ATTACKED(gd,sq,0)){
                                                    queenside = 0;
                                                    break;                                           
                                                }
                                            }
                                        }
                                    };
                                    if(queenside && !move_and_check(gd , gd.bk, c8, i, 0, 0, queenside)){
                                        ASSIGN_MOVE(second_move_list, sa.second,gd.bk, c8, i, 0, 0, 0, 0, queenside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++;
                                    };

                                }
                            }
                        }
                                
                    }else{ // sliding attack bishop , rook , queen

                            if(i==1 || i==7){ // queen
                                attack_mask = get_queen_attacks(sqr, gd.occupancy[2]);
                            }else{ // i=2,3,8,9
                                attack_mask = get_sliding_attacks( (i==3||i==9) ? 0 : 1,sqr, gd.occupancy[2]); // bishop or rook
                            };
                            
                            //int move = INVALID;
                            while (attack_mask){
                                int target = get_first_square(attack_mask);
                                //int capture=0;
                                if(gd.occupancy[i<6 ? 1 : 0] & (1ULL << target)){ // 
                                    // capture
                                    //move = sqr;
                                    //capture =1;
                                    
                                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, 1, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                        ASSIGN_MOVE( first_move_list , sa.first,sqr, target, i, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.first++;
                                    };

                                }else if ( !(gd.occupancy[2] & (1ULL << target) ) ){ 
                                    // regular move
                                    //move = sqr;
                                    
                                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, 0, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                        bool pot = check_potential(gd,sqr,target,i);
                                        ASSIGN_MOVE((pot ? first_move_list : second_move_list) , (pot ? sa.first : sa.second) ,sqr, target, i, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        pot ? sa.first++ : sa.second++;
                                    };
                                };
                                // if(move!=INVALID){
                                //     if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, capture, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                //         ASSIGN_MOVE((capture ? first_move_list : second_move_list) , (capture ? sa.first : sa.second) ,move, target, i, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                //         capture ? sa.first++ : sa.second++;
                                //     };
                                //     move = INVALID;
                                // };
                                DEL_BIT(attack_mask,target);
                            };

                    };

                }
            }
        }
        
    }else if(check_attacker[1]!=INVALID){// at least double check
        //move the king to a square thats not attacked
        
        if(!gd.side_to_move){//white king
            int king_sqr = gd.wking_sqr;
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move = INVALID; int capture=0;
            while(king_attacks){
                int target = get_first_square(king_attacks);
                if(gd.occupancy[1] & (1ULL << target)){ // 
                    // capture
                    move = king_sqr;
                    capture = 1;
                }else if ( !(gd.occupancy[0] & (1ULL << target) ) ){ 
                    // regular move
                    move = king_sqr;
                };
                if(move!=INVALID && !move_and_check(gd, move, target, 0, capture, 0)){ // black is attacker
                    // test if its still in check after the move
                    
                    ASSIGN_MOVE((capture ? first_move_list : second_move_list) , (capture ? sa.first : sa.second),move, target, 0, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    capture ? sa.first ++ : sa.second++; 
                }
                move = INVALID;
                capture =0;
                DEL_BIT(king_attacks,target);
            }
        }else{// black king
            int king_sqr = gd.bking_sqr;
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move = INVALID; int capture=0;
            while(king_attacks){
                int target = get_first_square(king_attacks);
                if(gd.occupancy[0] & (1ULL << target)){ // 
                    // capture
                    move = king_sqr;
                    capture = 1;
                }else if ( !(gd.occupancy[1] & (1ULL << target) ) ){ 
                    // regular move
                    move = king_sqr;
                };
                if(move!=INVALID && !move_and_check(gd, move, target, 6, capture, 0)){ // white is attacker
                    // test if its still in check after the move
                    
                    ASSIGN_MOVE((capture ? first_move_list : second_move_list) , (capture ? sa.first : sa.second) , move, target, 6, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    capture ? sa.first++ : sa.second++; 
                }
                move = INVALID;
                capture = 0;
                DEL_BIT(king_attacks,target);
            }
        }

    }else{ // one check

        // 1-eliminate the attacker 
        // 2-move the king to a square thats not attacked
        // 3-block the direction of attack
        // 4-filter the move out if after the move will be check again


        //eliminating the attacker square = check_attacker[0]
        uint64_t pins = pin_mask(gd);

        if(!gd.side_to_move){ // white to move
            
            uint64_t wpa = raw_attacks[6][check_attacker[0]] & gd.bitboards[5]; // we put a black pawn to get white attacks on the attacker
            while(wpa){
                int sqr = get_first_square(wpa);
                if((int)check_attacker[0]/8 ==7){
                    // promotions
                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 5, 1, 0)){ //orig_gd,sqr,target,piece,capture,enpassant

                        for(int j=1;j<5;j++){ // queen , rook , bishop, knight
                            ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 5, j, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                            sa.first++;
                            //moves[move_count -1]=move;
                        };
                    };
                
                }else if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 5, 1, 0)){
                    ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 5, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++;                
                }  
                DEL_BIT(wpa,sqr);
            };
            // en passant capture the attacker.
            if(gd.en_passant && (gd.en_passant == check_attacker[0] + 8)){
                uint64_t enpass_wpa = raw_attacks[6][check_attacker[0] + 8] & gd.bitboards[5];
                while(enpass_wpa){
                    int sqr = get_first_square(enpass_wpa);
                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (check_attacker[0] + 8), 5, 1, 1)){//orig_gd,sqr,target,piece,capture,enpassant
                        ASSIGN_MOVE(first_move_list, sa.first,sqr, (check_attacker[0] + 8), 5, 0, 1, 0, 1, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                        sa.first++; 
                    }
                    DEL_BIT(enpass_wpa,sqr);
                }
                               
            };

            uint64_t wna = raw_attacks[4][check_attacker[0]] & gd.bitboards[4];
            while(wna){ 
                int sqr = get_first_square(wna);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 4, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 4, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++; 
                }  
                DEL_BIT(wna,sqr);
            };
            
            

            uint64_t bishand = get_sliding_attacks(0,check_attacker[0],gd.occupancy[2])& gd.bitboards[3];
            uint64_t rookand = get_sliding_attacks(1,check_attacker[0],gd.occupancy[2])& gd.bitboards[2];
            uint64_t queenand = get_queen_attacks(check_attacker[0], gd.occupancy[2])& gd.bitboards[1];

            while (bishand){ 
                int sqr = get_first_square(bishand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 3, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 3, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++; 
                }  
                DEL_BIT(bishand,sqr);
            };
            while (rookand){ 
                int sqr = get_first_square(rookand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 2, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 2, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++; 
                }  
                DEL_BIT(rookand,sqr);
            };
            while (queenand){ 
                int sqr = get_first_square(queenand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 1, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    ASSIGN_MOVE(first_move_list, sa.first ,sqr, check_attacker[0], 1, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++;  
                }  
                DEL_BIT(queenand,sqr);
            };

        }else{ // black to move
            
            uint64_t bpa = raw_attacks[5][check_attacker[0]] & gd.bitboards[11];
            while(bpa){ 
                int sqr = get_first_square(bpa);
                if((int)check_attacker[0]/8 ==0){
                    // promotions
                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0],11, 1, 0)){ //orig_gd,sqr,target,piece,capture,enpassant

                        for(int j=7;j<11;j++){ // queen , rook , bishop, knight
                            ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 11, j, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                            sa.first++;
                            //moves[move_count -1]=move;
                        };
                    };
                }else if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 11, 1, 0)){
                    ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 11, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++;                
                }  
                DEL_BIT(bpa,sqr);
            };
            // en passant capture the attacker.
            if(gd.en_passant && (gd.en_passant == check_attacker[0] - 8)){
                uint64_t enpass_wpa = raw_attacks[5][check_attacker[0] - 8] & gd.bitboards[11];
                while(enpass_wpa){
                    int sqr = get_first_square(enpass_wpa);
                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (check_attacker[0] - 8), 11, 1, 1)){//orig_gd,sqr,target,piece,capture,enpassant
                        ASSIGN_MOVE(first_move_list, sa.first,sqr, (check_attacker[0] - 8), 11, 0, 1, 0, 1, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                        sa.first++; 
                    }
                    DEL_BIT(enpass_wpa,sqr);
                }
                               
            };

            uint64_t bna = raw_attacks[4][check_attacker[0]] & gd.bitboards[10];
            while(bna){ 
                int sqr = get_first_square(bna);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 10, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    ASSIGN_MOVE(first_move_list, sa.first ,sqr, check_attacker[0], 10, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++;  
                }  
                DEL_BIT(bna,sqr);
            };

            uint64_t bishand = (get_sliding_attacks(0,check_attacker[0],gd.occupancy[2]) & gd.bitboards[9]);
            uint64_t rookand = (get_sliding_attacks(1,check_attacker[0],gd.occupancy[2]) &  gd.bitboards[8]);
            uint64_t queenand = (get_queen_attacks(check_attacker[0], gd.occupancy[2]) & gd.bitboards[7]);

            while (bishand){ 
                int sqr = get_first_square(bishand); 
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 9, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 9, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++;  
                } 
                DEL_BIT(bishand,sqr);
            };
            while (rookand){ 
                int sqr = get_first_square(rookand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 8, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 8, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++;  
                }  
                DEL_BIT(rookand,sqr);
            };
            while (queenand){ 
                int sqr = get_first_square(queenand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 7, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    ASSIGN_MOVE(first_move_list, sa.first,sqr, check_attacker[0], 7, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    sa.first++;  
                }  
                DEL_BIT(queenand,sqr);
            };

        }
          
        // move the king to a square thats not attacked
        
        if(!gd.side_to_move){//white king
            int king_sqr = gd.wking_sqr;
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move = INVALID; int capture=0;
            while(king_attacks){
                int target = get_first_square(king_attacks);
                if(gd.occupancy[1] & (1ULL << target)){ // 
                    // capture
                    move = king_sqr;
                    capture = 1;
                }else if ( !(gd.occupancy[0] & (1ULL << target) ) ){ 
                    // regular move
                    move = king_sqr;
                };
                //if(IS_SQUARE_ATTACKED(gd, move, 1)){move = INVALID;}
                if(move!=INVALID && !move_and_check(gd,move, target, 0, capture, 0)){ // black is attacker
                    // test if its still in check after the move
                    
                    ASSIGN_MOVE((capture ? first_move_list : second_move_list) , (capture ? sa.first : sa.second), move, target, 0, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    capture ? sa.first++ : sa.second++;
                }
                move = INVALID; 
                capture = 0;
                DEL_BIT(king_attacks,target);
            }
        }else{// black king
            int king_sqr = gd.bking_sqr;
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move = INVALID; int capture=0;
            while(king_attacks){
                int target = get_first_square(king_attacks);
                if(gd.occupancy[0] & (1ULL << target)){ // 
                    // capture
                    move = king_sqr;
                    capture = 1;
                }else if ( !(gd.occupancy[1] & (1ULL << target) ) ){ 
                    // regular move
                    move = king_sqr;
                };
                //if(IS_SQUARE_ATTACKED(gd, move, 0)){move = INVALID;}
                if(move!=INVALID && !move_and_check(gd,move, target, 6, capture, 0)){ // white is attacker
                    // test if its still in check after the move
                    
                    ASSIGN_MOVE((capture ? first_move_list : second_move_list) , (capture ? sa.first : sa.second), move, target, 6, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    capture ? sa.first++ : sa.second++;
                }
                move = INVALID; 
                capture = 0;
                DEL_BIT(king_attacks,target);
            }
        }
        
        //block the direction of the attack
        

        for(int a_piece=(gd.side_to_move? 0 : 6); a_piece< (gd.side_to_move? 6 : 12); a_piece++){
            if(gd.bitboards[a_piece] & (1ULL << check_attacker[0])){ // found the attacker kind
                if(a_piece< (gd.side_to_move ? 4 : 10) && a_piece >(gd.side_to_move? 0 : 6) ){ // blocking sliding attacks only

                    int king_sqr =  !gd.side_to_move? gd.wking_sqr: gd.bking_sqr;
                    int king_rank = (int)king_sqr/8;
                    int attacker_rank = (int)check_attacker[0]/8; 

                    int king_file = (int)king_sqr%8;
                    int attacker_file = (int)check_attacker[0]%8;

                    uint64_t mask = 0ULL;

                    if(king_rank == attacker_rank){ //same rank
                        //mask |= (((1ULL << (king_sqr>check_attacker[0]? (king_sqr-1) : (check_attacker[0]-1) )) - 1) >> (king_sqr>check_attacker[0]?  check_attacker[0] : king_sqr )) << ((king_sqr>check_attacker[0]? check_attacker[0] : king_sqr )-1);
                        mask |= direction_attack_tables[king_sqr>check_attacker[0]? HORIZ_R : HORIZ_L][check_attacker[0]][1ULL << king_sqr];
                  
                    }else if (king_file == attacker_file){ // same file
                        
                        // for (int rank = std::min((int)king_sqr/8 , (int)check_attacker[0]/8 ) + 1; rank < std::max((int)king_sqr/8 , (int)check_attacker[0]/8); ++rank) {
                        //     mask |= (1ULL << ((int)king_sqr%8 + rank * 8));
                        // }
                        mask |= direction_attack_tables[king_sqr>check_attacker[0]? VERTIC_B : VERTIC_T][check_attacker[0]][1ULL << king_sqr];

                    }else{ // diagonal
                        if(check_attacker[0] > king_sqr){ // T
                        
                            if((int)check_attacker[0]%8 < (int)king_sqr%8){ // 0
                                mask |= direction_attack_tables[DIAG_RT][check_attacker[0]][1ULL << king_sqr];
                            }else{ // 3
                                mask |= direction_attack_tables[DIAG_LT][check_attacker[0]][1ULL << king_sqr];
                            }
                        }else{ // B

                            if((int)check_attacker[0]%8 < (int)king_sqr%8){ //2
                                mask |= direction_attack_tables[DIAG_RB][check_attacker[0]][1ULL << king_sqr];
                            }else{ //1
                                mask |= direction_attack_tables[DIAG_LB][check_attacker[0]][1ULL << king_sqr];
                            }
                        }
                    };

                    while(mask){
                        int target = get_first_square(mask);
                        
                        if(!gd.side_to_move){ // white to move - white king in check

                            uint64_t wpp = target - 8; // pawn push (cant be a capture)
                            if (wpp>0 && ((1ULL << wpp) & gd.bitboards[5]) && (!((1ULL << wpp) & pins) || !move_and_check(gd, wpp, target, 5, 0, 0))){ //orig_gd,sqr,target,piece,capture,enpassant
                                
                                if((int)target/8 == 7){ 
                                    // promotion
                                    for(int j=1;j<5;j++){ // queen , rook , bishop, knight
                                        ASSIGN_MOVE(second_move_list, sa.second,wpp, target, 5, j, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++;
                                        //moves[move_count -1]=move;
                                    };
                                }else{ // single push
                                    ASSIGN_MOVE(second_move_list, sa.second,wpp, target, 5, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    sa.second++; 
                                }

                            }
                           
                            if( ((target-8) >0)  && (int)target/8 == 3 && !(gd.occupancy[2] & (1ULL<< (target-8)))){

                                uint64_t wdp = target - 16;
                                if (wdp>0 && ((1ULL << wdp) & gd.bitboards[5]) && (!((1ULL << wdp) & pins) || !move_and_check(gd, wdp, target, 5, 0, 0))){ //orig_gd,sqr,target,piece,capture,enpassant
                                    ASSIGN_MOVE(second_move_list, sa.second,wdp, target, 5, 0, 0, 1, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    sa.second++;                                
                                }
                            }
                            
                            uint64_t wna = raw_attacks[4][target] & gd.bitboards[4];
                            while(wna){
                                int wna_s =get_first_square(wna);

                                    if(!((1ULL << wna_s) & pins) || !move_and_check(gd, wna_s, target, 4, 0, 0)){ 
                                        ASSIGN_MOVE(second_move_list, sa.second,wna_s, target, 4, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++; 
                                    }
                                DEL_BIT(wna,wna_s);
                            }
                            

                            uint64_t bishand = get_sliding_attacks(0,target,gd.occupancy[2])& gd.bitboards[3];
                            uint64_t rookand = get_sliding_attacks(1,target,gd.occupancy[2])& gd.bitboards[2];
                            uint64_t queenand = get_queen_attacks(target, gd.occupancy[2])& gd.bitboards[1];

                            while(bishand){
                                int bishand_s = get_first_square(bishand);

                                    if (!((1ULL << bishand_s) & pins) || !move_and_check(gd, bishand_s, target, 3, 0, 0)){
                                        ASSIGN_MOVE(second_move_list, sa.second,bishand_s, target, 3, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++; 
                                    }
                                DEL_BIT(bishand,bishand_s);
                            }
                            while(rookand){
                                int rookand_s = get_first_square(rookand);
                                
                                if (!((1ULL << rookand_s) & pins) || !move_and_check(gd, rookand_s, target, 2, 0, 0)){ 
                                    ASSIGN_MOVE(second_move_list, sa.second,rookand_s, target, 2, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    sa.second++;                                
                                }
                                DEL_BIT(rookand,rookand_s);
                            }
                            while(queenand){
                                int queenand_s = get_first_square(queenand);

                                    if (!((1ULL << queenand_s) & pins) || !move_and_check(gd, queenand_s, target, 1, 0, 0)){
                                        ASSIGN_MOVE(second_move_list, sa.second,queenand_s, target, 1, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++; 
                                    }
                                DEL_BIT(queenand,queenand_s);
                            }

                        }else{ // black to move - black king in check
                            
                            uint64_t bpp = target + 8; // pawn push (cant be a capture)
                            if ( (bpp<64) && ((1ULL << bpp) & gd.bitboards[11]) &&(!((1ULL << bpp) & pins) || !move_and_check(gd, bpp, target, 11, 0, 0))){ //orig_gd,sqr,target,piece,capture,enpassant
                                
                                if((int)target/8 == 0){
                                    for(int j=7;j<11;j++){ // queen , rook , bishop, knight
                                        ASSIGN_MOVE(second_move_list, sa.second,bpp, target, 11, j, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++;
                                        //moves[move_count -1]=move;
                                    };
                                }else{ // single push
                                    ASSIGN_MOVE(second_move_list, sa.second,bpp, target, 11, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    sa.second++; 
                                }

                            };

                            if( (int)target/8 == 4 && !(gd.occupancy[2] & (1ULL<< (target + 8)))){

                                uint64_t bdp = target + 16;
                                if (((1ULL << bdp) & gd.bitboards[11]) && (!((1ULL << bdp) & pins) || !move_and_check(gd, bdp, target, 11, 0, 0))){ //orig_gd,sqr,target,piece,capture,enpassant
                                    ASSIGN_MOVE(second_move_list, sa.second,bdp, target, 11, 0, 0, 1, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    sa.second++;                                
                                }
                            }

                            uint64_t bna = raw_attacks[4][target] & gd.bitboards[10];
                            while(bna){
                                int bna_s =get_first_square(bna);

                                    if(!((1ULL << bna_s) & pins) || !move_and_check(gd, bna_s, target, 10, 0, 0)){ 
                                        ASSIGN_MOVE(second_move_list, sa.second,bna_s, target, 10, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++; 
                                    }
                                DEL_BIT(bna,bna_s);
                            }
                            

                            uint64_t bishand = (get_sliding_attacks(0,target,gd.occupancy[2]) & gd.bitboards[9]);
                            uint64_t rookand = (get_sliding_attacks(1,target,gd.occupancy[2]) & gd.bitboards[8]);
                            uint64_t queenand = (get_queen_attacks(target, gd.occupancy[2]) & gd.bitboards[7]);


                            while(bishand){
                                int bishand_s = get_first_square(bishand);

                                    if (!((1ULL << bishand_s) & pins) || !move_and_check(gd, bishand_s, target, 9, 0, 0)){
                                        ASSIGN_MOVE(second_move_list, sa.second,bishand_s, target, 9, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++; 
                                    }
                                DEL_BIT(bishand,bishand_s);
                            }
                            while(rookand){
                                int rookand_s = get_first_square(rookand);
                                
                                if (!((1ULL << rookand_s) & pins) || !move_and_check(gd, rookand_s, target, 8, 0, 0)){ 
                                    ASSIGN_MOVE(second_move_list, sa.second,rookand_s, target, 8, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    sa.second++;                                
                                }
                                DEL_BIT(rookand,rookand_s);
                            }
                            while(queenand){
                                int queenand_s = get_first_square(queenand);

                                    if (!((1ULL << queenand_s) & pins) || !move_and_check(gd, queenand_s, target, 7, 0, 0)){
                                        ASSIGN_MOVE(second_move_list, sa.second,queenand_s, target, 7, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        sa.second++; 
                                    }
                                DEL_BIT(queenand,queenand_s);
                            }

                        }

                        DEL_BIT(mask,target);
                    }
                }

                break;
            }
        }

    }
    
    return sa;
};



int generate_moves(game_data& gd ,uint32_t* moves){
    // gd.side_tomove 0 white 1 black
    // legal moves //
    // 1- check if in check
    // 2- get enemy pin mask
    // 2- if check =>get own pin mask, generate specific moves based on enemy and own pin mask 
    //    else => generate moves based on enemy pin mask
    
    int move_count=0;
    uint64_t check_attacker[2]={INVALID, INVALID};
    get_checks((!gd.side_to_move? gd.wking_sqr: gd.bking_sqr),gd, check_attacker);

    if(check_attacker[0]==INVALID){ // not in check
        uint64_t pins = pin_mask(gd);

        for(int i=(gd.side_to_move? 6 : 0) ; i < (gd.side_to_move? 12 : 6) ; i++){
            if(gd.bitboards[i]){
                uint64_t attack_mask = 0ULL;
                //uint32_t moves[256]; 
                if(i==5 || i==11){ // white pawn or black pawn
                    uint64_t copy_bitboard = gd.bitboards[i];
                    while (copy_bitboard){
                        int sqr = get_first_square(copy_bitboard);
                        attack_mask = raw_attacks[i==5 ? 5 :6][sqr]; // white pawn or black
                        int move = INVALID;
                        while (attack_mask){
                            int target = get_first_square(attack_mask);
                            //int capture = 0;
                            int en_passant = 0;
                            //int double_push = 0;

                            if((i<6 ? (target >= 56): (target<8) ) && (gd.occupancy[i<6 ? 1 : 0] & (1ULL << target) ) ) {
                                // promotion capture
                                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, 1, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                
                                    for(int j=(i< 6 ? 1 : 7);j<(i < 6 ? 5 : 11);j++){ // queen , rook , bishop, knight
                                        move = sqr;
                                        moves[move_count]=encode_move(move, target, i, j, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++;
                                        //moves[move_count -1]=move;
                                    };
                                    move = INVALID;
                                }
                            }else if(gd.occupancy[i<6 ? 1 : 0] & (1ULL << target)){ // 
                                // pawn capture
                                //capture = 1;
                                move = sqr;

                            }else if(gd.en_passant && gd.en_passant== target && !(gd.occupancy[2] & (1ULL << target))){
                                // en passant capture
                                en_passant = 1;
                                //capture = 1;
                                move = sqr;
                            };

                            if(move!=INVALID){
                                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, 1, en_passant)){ //orig_gd,sqr,target,piece,capture,enpassant
                                    moves[move_count]=encode_move(move, target, i, 0, 1, 0, en_passant, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                }
                                move = INVALID;
                            }
                            
                            DEL_BIT(attack_mask,target);
                        };

                        if( !(gd.occupancy[2] & ( 1ULL << (sqr + (i==5 ? 8 : -8)) ) ) ){ 
                            
                            if( (i<6 ? (sqr<56 && sqr >= 48): (sqr>=8 && sqr<16) )  &&( !((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (sqr + (i==5 ? 8 : -8)), i, 0, 0) ) ){
                                // promotion
                                for(int j=(i< 6 ? 1 : 7);j<(i < 6 ? 5 : 11);j++){ // queen , rook , bishop, knight
                                    move = sqr;
                                    moves[move_count]=encode_move(move, (sqr + (i==5 ? 8 : -8)), i, j, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                    //moves[move_count -1]=move;
                                };

                            }else if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (sqr + (i==5 ? 8 : -8)), i, 0, 0)) {
                                // single push_pawn
                                moves[move_count]=encode_move(sqr, (sqr + (i==5 ? 8 : -8)), i, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                move_count++;
                            };
                            
                            if( (((int)sqr/8 == 1 && i == 5) || ((int)sqr/8 == 6 && i == 11)) && ! (gd.occupancy[2] & ( 1ULL << (sqr + (i==5 ? 16 : -16)) )) ){
                                    // double push
                                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (sqr + (i==5 ? 16 : -16)), i, 0, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                
                                    moves[move_count]=encode_move(sqr, (sqr + (i==5 ? 16 : -16)), i, 0, 0, 1, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                }
                            }

                        };

                        DEL_BIT(copy_bitboard,sqr);
                    };


                }else if( i == 0 || i == 4 || i == 6 || i == 10){ // king or knight
                    uint64_t copy_bitboard = gd.bitboards[i];
                    while (copy_bitboard){
                        int sqr = get_first_square(copy_bitboard);
                        //attack_mask = raw_attacks[i][sqr];
                        attack_mask = raw_attacks[(i == 0 || i == 6)? 0 : 4][sqr]; //king or knight
                        
                        int move = INVALID;
                        while (attack_mask){
                            int target = get_first_square(attack_mask);
                            int capture=0;
                            
                            if(gd.occupancy[i<6 ? 1 : 0] & (1ULL << target)){ // 
                                // capture
                                move = sqr;
                                capture = 1;
                            }else if ( !(gd.occupancy[2] & (1ULL << target) ) ){ 
                                // regular move
                                move = sqr;
                            };

                            if(move!=INVALID){
                                if((i==0 || i==6) && !move_and_check(gd, sqr, target, i, capture, 0)){
                                    moves[move_count]=encode_move(move, target, i, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                }else if((i==4 || i==10) &&(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, capture, 0) )){ //orig_gd,sqr,target,piece,capture,enpassant
                                    moves[move_count]=encode_move(move, target, i, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                }
                                move = INVALID;
                            }
                            DEL_BIT(attack_mask,target);
                        };
                

                        DEL_BIT(copy_bitboard,sqr);
                    };
                    if(gd.castles && (i==0||i==6)){//king
                        //check for castling regarding fischer random
                        if(i==0 ){//white
                            if(gd.castles & 1){ // king side
                                int kingside=1;
                                for(int f=gd.wr1+1; f<gd.wk; f++ ){ // between king and rook
                                    if(gd.occupancy[2] & (1ULL << f)){
                                        kingside = 0;
                                        break;
                                    };
                                };
                                if(kingside){
                                    if(gd.wr1>g1){
                                        for(int f=g1; f<gd.wr1; f++ ){ // between rook and g1 
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                kingside = 0;
                                                break;
                                            };
                                        };
                                    };
                                };
                                if(kingside){
                                    for(int sq=g1; sq<gd.wk; sq++){ // attacks between king and g1
                                        if(IS_SQUARE_ATTACKED(gd,sq,1)){
                                            kingside = 0;
                                            break;                                           
                                        }
                                    }
                                };
                                if(kingside && !move_and_check(gd , gd.wk, g1, i, 0, 0, kingside) ){ //orig_gd,sqr,target,piece,capture,enpassant , castling
                                    moves[move_count]=encode_move(gd.wk, g1, i, 0, 0, 0, 0, kingside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                };
                            
                            };
                            if(gd.castles & 2){ // queen side

                                int queenside=1;
                                if(gd.wk <=d1){
                                    for(int f=gd.wk+1; f<gd.wr2; f++ ){
                                        if(gd.occupancy[2] & (1ULL << f)){
                                            queenside = 0;
                                            break;
                                        };
                                    };
                                    if(gd.wr2 <c1){
                                        for(int f=gd.wr2+1; f<=c1; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                    }
                                }else{
                                    for(int f=gd.wk+1; f<gd.wr2; f++ ){
                                        if(gd.occupancy[2] & (1ULL << f)){
                                            queenside = 0;
                                            break;
                                        };
                                    };
                                    for(int f=d1; f<gd.wk; f++ ){
                                        if(gd.occupancy[2] & (1ULL << f)){
                                            queenside = 0;
                                            break;
                                        };
                                    };
                                }

                                if(queenside){
                                    if(gd.wk <c1){
                                        for(int sq=gd.wk+1; sq<=c1; sq++){
                                            if(IS_SQUARE_ATTACKED(gd,sq,1)){
                                                queenside = 0;
                                                break;                                           
                                            }
                                        }
                                    }else{
                                        for(int sq=c1; sq<gd.wk; sq++){
                                            if(IS_SQUARE_ATTACKED(gd,sq,1)){
                                                queenside = 0;
                                                break;                                           
                                            }
                                        }
                                    }
                                };
                                if(queenside && !move_and_check(gd , gd.wk, c1, i, 0, 0, queenside)){
                                    moves[move_count]=encode_move(gd.wk, c1, i, 0, 0, 0, 0, queenside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                };

                            }

                        }else{//black
                            if(gd.castles & 4){ // king side
                                int kingside=1;
                                for(int f=gd.br1+1; f<gd.bk; f++ ){
                                    if(gd.occupancy[2] & (1ULL << f)){
                                        kingside = 0;
                                        break;
                                    };
                                };
                                if(kingside){
                                    if(gd.br1>g8){
                                        for(int f=g8; f<gd.br1; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                kingside = 0;
                                                break;
                                            };
                                        };
                                    };
                                };
                                if(kingside){
                                    for(int sq=g8; sq<gd.bk; sq++){
                                        if(IS_SQUARE_ATTACKED(gd,sq,0)){
                                            kingside = 0;
                                            break;                                           
                                        }
                                    }
                                };                                
                                if(kingside && !move_and_check(gd , gd.bk, g8, i, 0, 0, kingside)){
                                    moves[move_count]=encode_move(gd.bk, g8, i, 0, 0, 0, 0, kingside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                };
                            
                            };
                            if(gd.castles & 8){ // queen side

                                int queenside=1;
                                if(gd.bk <=d8){
                                    for(int f=gd.bk+1; f<gd.br2; f++ ){
                                        if(gd.occupancy[2] & (1ULL << f)){
                                            queenside = 0;
                                            break;
                                        };
                                    };
                                    if(gd.br2 <c8){
                                        for(int f=gd.br2+1; f<=c8; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                    }
                                }else{
                                    for(int f=gd.bk+1; f<gd.br2; f++ ){
                                        if(gd.occupancy[2] & (1ULL << f)){
                                            queenside = 0;
                                            break;
                                        };
                                    };
                                    for(int f=d8; f<gd.bk; f++ ){
                                        if(gd.occupancy[2] & (1ULL << f)){
                                            queenside = 0;
                                            break;
                                        };
                                    };
                                }

                                if(queenside){
                                    if(gd.bk < c8){
                                        for(int sq=gd.bk+1; sq<=c8; sq++){
                                            if(IS_SQUARE_ATTACKED(gd,sq,0)){
                                                queenside = 0;
                                                break;                                           
                                            }
                                        }
                                    }else{
                                        for(int sq=c8; sq<gd.bk; sq++){
                                            if(IS_SQUARE_ATTACKED(gd,sq,0)){
                                                queenside = 0;
                                                break;                                           
                                            }
                                        }
                                    }
                                };
                                if(queenside && !move_and_check(gd , gd.bk, c8, i, 0, 0, queenside)){
                                    moves[move_count]=encode_move(gd.bk, c8, i, 0, 0, 0, 0, queenside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                };

                            }
                        }
                    }
                            
                }else{ // sliding attack bishop , rook , queen
                    
                    uint64_t copy_bitboard = gd.bitboards[i];
                    while (copy_bitboard){
                        int sqr = get_first_square(copy_bitboard);
                        if(i==1 || i==7){ // queen
                            attack_mask = get_queen_attacks(sqr, gd.occupancy[2]);
                        }else{ // i=2,3,8,9
                            attack_mask = get_sliding_attacks( (i==3||i==9) ? 0 : 1,sqr, gd.occupancy[2]); // bishop or rook
                        };
                        
                        int move = INVALID;
                        while (attack_mask){
                            int target = get_first_square(attack_mask);
                            int capture=0;
                            if(gd.occupancy[i<6 ? 1 : 0] & (1ULL << target)){ // 
                                // capture
                                move = sqr;
                                capture =1;
                            }else if ( !(gd.occupancy[2] & (1ULL << target) ) ){ 
                                // regular move
                                move = sqr;
                            };
                            if(move!=INVALID){
                                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, target, i, capture, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                    moves[move_count]=encode_move(move, target, i, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                };
                                move = INVALID;
                            };
                            DEL_BIT(attack_mask,target);
                        };
                        DEL_BIT(copy_bitboard,sqr);
                    };
                };

            }
        }
    
    }else if(check_attacker[1]!=INVALID){// at least double check
        //move the king to a square thats not attacked
        
        if(!gd.side_to_move){//white king
            int king_sqr = gd.wking_sqr;
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move = INVALID; int capture=0;
            while(king_attacks){
                int target = get_first_square(king_attacks);
                if(gd.occupancy[1] & (1ULL << target)){ // 
                    // capture
                    move = king_sqr;
                    capture = 1;
                }else if ( !(gd.occupancy[0] & (1ULL << target) ) ){ 
                    // regular move
                    move = king_sqr;
                };
                if(move!=INVALID && !move_and_check(gd, move, target, 0, capture, 0)){ // black is attacker
                    // test if its still in check after the move
                    
                    moves[move_count]=encode_move(move, target, 0, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }
                move = INVALID;
                capture =0;
                DEL_BIT(king_attacks,target);
            }
        }else{// black king
            int king_sqr = gd.bking_sqr;
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move = INVALID; int capture=0;
            while(king_attacks){
                int target = get_first_square(king_attacks);
                if(gd.occupancy[0] & (1ULL << target)){ // 
                    // capture
                    move = king_sqr;
                    capture = 1;
                }else if ( !(gd.occupancy[1] & (1ULL << target) ) ){ 
                    // regular move
                    move = king_sqr;
                };
                if(move!=INVALID && !move_and_check(gd, move, target, 6, capture, 0)){ // white is attacker
                    // test if its still in check after the move
                    
                    moves[move_count]=encode_move(move, target, 6, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }
                move = INVALID;
                capture = 0;
                DEL_BIT(king_attacks,target);
            }
        }

    }else{ // one check

        // 1-eliminate the attacker 
        // 2-move the king to a square thats not attacked
        // 3-block the direction of attack
        // 4-filter the move out if after the move will be check again


        //eliminating the attacker square = check_attacker[0]
        uint64_t pins = pin_mask(gd);

        if(!gd.side_to_move){ // white to move
            
            uint64_t wpa = raw_attacks[6][check_attacker[0]] & gd.bitboards[5]; // we put a black pawn to get white attacks on the attacker
            while(wpa){
                int sqr = get_first_square(wpa);
                if((int)check_attacker[0]/8 ==7){
                    // promotions
                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 5, 1, 0)){ //orig_gd,sqr,target,piece,capture,enpassant

                        for(int j=1;j<5;j++){ // queen , rook , bishop, knight
                            moves[move_count]=encode_move(sqr, check_attacker[0], 5, j, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                            move_count++;
                            //moves[move_count -1]=move;
                        };
                    };
                
                }else if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 5, 1, 0)){
                    moves[move_count]=encode_move(sqr, check_attacker[0], 5, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;                
                }  
                DEL_BIT(wpa,sqr);
            };
            // en passant capture the attacker.
            if(gd.en_passant && (gd.en_passant == check_attacker[0] + 8)){
                uint64_t enpass_wpa = raw_attacks[6][check_attacker[0] + 8] & gd.bitboards[5];
                while(enpass_wpa){
                    int sqr = get_first_square(enpass_wpa);
                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (check_attacker[0] + 8), 5, 1, 1)){//orig_gd,sqr,target,piece,capture,enpassant
                        moves[move_count]=encode_move(sqr, (check_attacker[0] + 8), 5, 0, 1, 0, 1, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                        move_count++; 
                    }
                    DEL_BIT(enpass_wpa,sqr);
                }
                               
            };

            uint64_t wna = raw_attacks[4][check_attacker[0]] & gd.bitboards[4];
            while(wna){ 
                int sqr = get_first_square(wna);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 4, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 4, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }  
                DEL_BIT(wna,sqr);
            };
            
            

            uint64_t bishand = get_sliding_attacks(0,check_attacker[0],gd.occupancy[2])& gd.bitboards[3];
            uint64_t rookand = get_sliding_attacks(1,check_attacker[0],gd.occupancy[2])& gd.bitboards[2];
            uint64_t queenand = get_queen_attacks(check_attacker[0], gd.occupancy[2])& gd.bitboards[1];

            while (bishand){ 
                int sqr = get_first_square(bishand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 3, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 3, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }  
                DEL_BIT(bishand,sqr);
            };
            while (rookand){ 
                int sqr = get_first_square(rookand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 2, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 2, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }  
                DEL_BIT(rookand,sqr);
            };
            while (queenand){ 
                int sqr = get_first_square(queenand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 1, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 1, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;  
                }  
                DEL_BIT(queenand,sqr);
            };

        }else{ // black to move
            
            uint64_t bpa = raw_attacks[5][check_attacker[0]] & gd.bitboards[11];
            while(bpa){ 
                int sqr = get_first_square(bpa);
                if((int)check_attacker[0]/8 ==0){
                    // promotions
                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0],11, 1, 0)){ //orig_gd,sqr,target,piece,capture,enpassant

                        for(int j=7;j<11;j++){ // queen , rook , bishop, knight
                            moves[move_count]=encode_move(sqr, check_attacker[0], 11, j, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                            move_count++;
                            //moves[move_count -1]=move;
                        };
                    };
                }else if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 11, 1, 0)){
                    moves[move_count]=encode_move(sqr, check_attacker[0], 11, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;                
                }  
                DEL_BIT(bpa,sqr);
            };
            // en passant capture the attacker.
            if(gd.en_passant && (gd.en_passant == check_attacker[0] - 8)){
                uint64_t enpass_wpa = raw_attacks[5][check_attacker[0] - 8] & gd.bitboards[11];
                while(enpass_wpa){
                    int sqr = get_first_square(enpass_wpa);
                    if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, (check_attacker[0] - 8), 11, 1, 1)){//orig_gd,sqr,target,piece,capture,enpassant
                        moves[move_count]=encode_move(sqr, (check_attacker[0] - 8), 11, 0, 1, 0, 1, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                        move_count++; 
                    }
                    DEL_BIT(enpass_wpa,sqr);
                }
                               
            };

            uint64_t bna = raw_attacks[4][check_attacker[0]] & gd.bitboards[10];
            while(bna){ 
                int sqr = get_first_square(bna);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 10, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 10, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;  
                }  
                DEL_BIT(bna,sqr);
            };

            uint64_t bishand = (get_sliding_attacks(0,check_attacker[0],gd.occupancy[2]) & gd.bitboards[9]);
            uint64_t rookand = (get_sliding_attacks(1,check_attacker[0],gd.occupancy[2]) &  gd.bitboards[8]);
            uint64_t queenand = (get_queen_attacks(check_attacker[0], gd.occupancy[2]) & gd.bitboards[7]);

            while (bishand){ 
                int sqr = get_first_square(bishand); 
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 9, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 9, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;  
                } 
                DEL_BIT(bishand,sqr);
            };
            while (rookand){ 
                int sqr = get_first_square(rookand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 8, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 8, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;  
                }  
                DEL_BIT(rookand,sqr);
            };
            while (queenand){ 
                int sqr = get_first_square(queenand);
                if(!((1ULL << sqr) & pins) || !move_and_check(gd, sqr, check_attacker[0], 7, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 7, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;  
                }  
                DEL_BIT(queenand,sqr);
            };

        }
          
        // move the king to a square thats not attacked
        
        if(!gd.side_to_move){//white king
            int king_sqr = gd.wking_sqr;
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move = INVALID; int capture=0;
            while(king_attacks){
                int target = get_first_square(king_attacks);
                if(gd.occupancy[1] & (1ULL << target)){ // 
                    // capture
                    move = king_sqr;
                    capture = 1;
                }else if ( !(gd.occupancy[0] & (1ULL << target) ) ){ 
                    // regular move
                    move = king_sqr;
                };
                //if(IS_SQUARE_ATTACKED(gd, move, 1)){move = INVALID;}
                if(move!=INVALID && !move_and_check(gd,move, target, 0, capture, 0)){ // black is attacker
                    // test if its still in check after the move
                    
                    moves[move_count]=encode_move(move, target, 0, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;
                }
                move = INVALID; 
                capture = 0;
                DEL_BIT(king_attacks,target);
            }
        }else{// black king
            int king_sqr = gd.bking_sqr;
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move = INVALID; int capture=0;
            while(king_attacks){
                int target = get_first_square(king_attacks);
                if(gd.occupancy[0] & (1ULL << target)){ // 
                    // capture
                    move = king_sqr;
                    capture = 1;
                }else if ( !(gd.occupancy[1] & (1ULL << target) ) ){ 
                    // regular move
                    move = king_sqr;
                };
                //if(IS_SQUARE_ATTACKED(gd, move, 0)){move = INVALID;}
                if(move!=INVALID && !move_and_check(gd,move, target, 6, capture, 0)){ // white is attacker
                    // test if its still in check after the move
                    
                    moves[move_count]=encode_move(move, target, 6, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;
                }
                move = INVALID; 
                capture = 0;
                DEL_BIT(king_attacks,target);
            }
        }
        
        //block the direction of the attack
        

        for(int a_piece=(gd.side_to_move? 0 : 6); a_piece< (gd.side_to_move? 6 : 12); a_piece++){
            if(gd.bitboards[a_piece] & (1ULL << check_attacker[0])){ // found the attacker kind
                if(a_piece< (gd.side_to_move ? 4 : 10) && a_piece >(gd.side_to_move? 0 : 6) ){ // blocking sliding attacks only

                    int king_sqr =  !gd.side_to_move? gd.wking_sqr: gd.bking_sqr;
                    int king_rank = (int)king_sqr/8;
                    int attacker_rank = (int)check_attacker[0]/8; 

                    int king_file = (int)king_sqr%8;
                    int attacker_file = (int)check_attacker[0]%8;

                    uint64_t mask = 0ULL;

                    if(king_rank == attacker_rank){ //same rank
                        //mask |= (((1ULL << (king_sqr>check_attacker[0]? (king_sqr-1) : (check_attacker[0]-1) )) - 1) >> (king_sqr>check_attacker[0]?  check_attacker[0] : king_sqr )) << ((king_sqr>check_attacker[0]? check_attacker[0] : king_sqr )-1);
                        mask |= direction_attack_tables[king_sqr>check_attacker[0]? HORIZ_R : HORIZ_L][check_attacker[0]][1ULL << king_sqr];
                  
                    }else if (king_file == attacker_file){ // same file
                        
                        // for (int rank = std::min((int)king_sqr/8 , (int)check_attacker[0]/8 ) + 1; rank < std::max((int)king_sqr/8 , (int)check_attacker[0]/8); ++rank) {
                        //     mask |= (1ULL << ((int)king_sqr%8 + rank * 8));
                        // }
                        mask |= direction_attack_tables[king_sqr>check_attacker[0]? VERTIC_B : VERTIC_T][check_attacker[0]][1ULL << king_sqr];

                    }else{ // diagonal
                        if(check_attacker[0] > king_sqr){ // T
                        
                            if((int)check_attacker[0]%8 < (int)king_sqr%8){ // 0
                                mask |= direction_attack_tables[DIAG_RT][check_attacker[0]][1ULL << king_sqr];
                            }else{ // 3
                                mask |= direction_attack_tables[DIAG_LT][check_attacker[0]][1ULL << king_sqr];
                            }
                        }else{ // B

                            if((int)check_attacker[0]%8 < (int)king_sqr%8){ //2
                                mask |= direction_attack_tables[DIAG_RB][check_attacker[0]][1ULL << king_sqr];
                            }else{ //1
                                mask |= direction_attack_tables[DIAG_LB][check_attacker[0]][1ULL << king_sqr];
                            }
                        }
                    };

                    while(mask){
                        int target = get_first_square(mask);
                        
                        if(!gd.side_to_move){ // white to move - white king in check

                            uint64_t wpp = target - 8; // pawn push (cant be a capture)
                            if (wpp>0 && ((1ULL << wpp) & gd.bitboards[5]) && (!((1ULL << wpp) & pins) || !move_and_check(gd, wpp, target, 5, 0, 0))){ //orig_gd,sqr,target,piece,capture,enpassant
                                
                                if((int)target/8 == 7){ 
                                    // promotion
                                    for(int j=1;j<5;j++){ // queen , rook , bishop, knight
                                        moves[move_count]=encode_move(wpp, target, 5, j, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++;
                                        //moves[move_count -1]=move;
                                    };
                                }else{ // single push
                                    moves[move_count]=encode_move(wpp, target, 5, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++; 
                                }

                            }
                           
                            if( ((target-8) >0)  && (int)target/8 == 3 && !(gd.occupancy[2] & (1ULL<< (target-8)))){

                                uint64_t wdp = target - 16;
                                if (wdp>0 && ((1ULL << wdp) & gd.bitboards[5]) && (!((1ULL << wdp) & pins) || !move_and_check(gd, wdp, target, 5, 0, 0))){ //orig_gd,sqr,target,piece,capture,enpassant
                                    moves[move_count]=encode_move(wdp, target, 5, 0, 0, 1, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;                                
                                }
                            }
                            
                            uint64_t wna = raw_attacks[4][target] & gd.bitboards[4];
                            while(wna){
                                int wna_s =get_first_square(wna);

                                    if(!((1ULL << wna_s) & pins) || !move_and_check(gd, wna_s, target, 4, 0, 0)){ 
                                        moves[move_count]=encode_move(wna_s, target, 4, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++; 
                                    }
                                DEL_BIT(wna,wna_s);
                            }
                            

                            uint64_t bishand = get_sliding_attacks(0,target,gd.occupancy[2])& gd.bitboards[3];
                            uint64_t rookand = get_sliding_attacks(1,target,gd.occupancy[2])& gd.bitboards[2];
                            uint64_t queenand = get_queen_attacks(target, gd.occupancy[2])& gd.bitboards[1];

                            while(bishand){
                                int bishand_s = get_first_square(bishand);

                                    if (!((1ULL << bishand_s) & pins) || !move_and_check(gd, bishand_s, target, 3, 0, 0)){
                                        moves[move_count]=encode_move(bishand_s, target, 3, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++; 
                                    }
                                DEL_BIT(bishand,bishand_s);
                            }
                            while(rookand){
                                int rookand_s = get_first_square(rookand);
                                
                                if (!((1ULL << rookand_s) & pins) || !move_and_check(gd, rookand_s, target, 2, 0, 0)){ 
                                    moves[move_count]=encode_move(rookand_s, target, 2, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;                                
                                }
                                DEL_BIT(rookand,rookand_s);
                            }
                            while(queenand){
                                int queenand_s = get_first_square(queenand);

                                    if (!((1ULL << queenand_s) & pins) || !move_and_check(gd, queenand_s, target, 1, 0, 0)){
                                        moves[move_count]=encode_move(queenand_s, target, 1, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++; 
                                    }
                                DEL_BIT(queenand,queenand_s);
                            }

                        }else{ // black to move - black king in check
                            
                            uint64_t bpp = target + 8; // pawn push (cant be a capture)
                            if ( (bpp<64) && ((1ULL << bpp) & gd.bitboards[11]) &&(!((1ULL << bpp) & pins) || !move_and_check(gd, bpp, target, 11, 0, 0))){ //orig_gd,sqr,target,piece,capture,enpassant
                                
                                if((int)target/8 == 0){
                                    for(int j=7;j<11;j++){ // queen , rook , bishop, knight
                                        moves[move_count]=encode_move(bpp, target, 11, j, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++;
                                        //moves[move_count -1]=move;
                                    };
                                }else{ // single push
                                    moves[move_count]=encode_move(bpp, target, 11, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++; 
                                }

                            };

                            if( (int)target/8 == 4 && !(gd.occupancy[2] & (1ULL<< (target + 8)))){

                                uint64_t bdp = target + 16;
                                if (((1ULL << bdp) & gd.bitboards[11]) && (!((1ULL << bdp) & pins) || !move_and_check(gd, bdp, target, 11, 0, 0))){ //orig_gd,sqr,target,piece,capture,enpassant
                                    moves[move_count]=encode_move(bdp, target, 11, 0, 0, 1, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;                                
                                }
                            }

                            uint64_t bna = raw_attacks[4][target] & gd.bitboards[10];
                            while(bna){
                                int bna_s =get_first_square(bna);

                                    if(!((1ULL << bna_s) & pins) || !move_and_check(gd, bna_s, target, 10, 0, 0)){ 
                                        moves[move_count]=encode_move(bna_s, target, 10, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++; 
                                    }
                                DEL_BIT(bna,bna_s);
                            }
                            

                            uint64_t bishand = (get_sliding_attacks(0,target,gd.occupancy[2]) & gd.bitboards[9]);
                            uint64_t rookand = (get_sliding_attacks(1,target,gd.occupancy[2]) & gd.bitboards[8]);
                            uint64_t queenand = (get_queen_attacks(target, gd.occupancy[2]) & gd.bitboards[7]);


                            while(bishand){
                                int bishand_s = get_first_square(bishand);

                                    if (!((1ULL << bishand_s) & pins) || !move_and_check(gd, bishand_s, target, 9, 0, 0)){
                                        moves[move_count]=encode_move(bishand_s, target, 9, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++; 
                                    }
                                DEL_BIT(bishand,bishand_s);
                            }
                            while(rookand){
                                int rookand_s = get_first_square(rookand);
                                
                                if (!((1ULL << rookand_s) & pins) || !move_and_check(gd, rookand_s, target, 8, 0, 0)){ 
                                    moves[move_count]=encode_move(rookand_s, target, 8, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;                                
                                }
                                DEL_BIT(rookand,rookand_s);
                            }
                            while(queenand){
                                int queenand_s = get_first_square(queenand);

                                    if (!((1ULL << queenand_s) & pins) || !move_and_check(gd, queenand_s, target, 7, 0, 0)){
                                        moves[move_count]=encode_move(queenand_s, target, 7, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++; 
                                    }
                                DEL_BIT(queenand,queenand_s);
                            }

                        }

                        DEL_BIT(mask,target);
                    }
                }

                break;
            }
        }

    }
    
    return move_count;
};

void make_move(game_data& gd , uint32_t move){ // move list
    // assuming that if the side is on check the wrong moves are filterd out and only remains blocking or attacker capturing moves.
    // if not on check go ahead and calculate proper moves and make them


    int sqr = GET_SOURCE_SQUARE(move);
    int target = GET_TARGET_SQUARE(move);
    int piece = GET_PIECE(move);
    int promote = GET_PROMOTED_PIECE(move);
    int capture = IS_CAPTURE(move);
    int double_push = IS_DOUBLE_PUSH(move);
    int enpassant = IS_ENPASSANT(move);
    int castling = IS_CASTLING(move);

    if(enpassant){
        // algorith =>  delete own piece / delete own occupancy / delete total occupancy (sqr)
        //              delete enemy piece/ delete enemy occupancy / delete total occupancy (enpassant_target)
        //              put own square / put own occupancy / put total occupancy (target) 
        int en_passant_target = target + (piece<6 ? -8 : 8);
        DEL_BIT(gd.bitboards[piece],sqr); // delete own sqr
        DEL_BIT(gd.occupancy[piece<6 ? 0 : 1],sqr); // delete own occupancy
        DEL_BIT(gd.occupancy[2],sqr); // delete total occupancy

        for(int j=(piece<6? 6: 0); j<(piece<6 ? 12 : 6); j++){
            if(gd.bitboards[j] & (1ULL << en_passant_target)){
                DEL_BIT(gd.bitboards[j],en_passant_target); // delete enemy piece
                gd.mailbox[en_passant_target] = INVALID;
                j>5 ? gd.piece_numbers[j-6]-- : gd.piece_numbers[j]--;
                break;
            }
        }
        DEL_BIT(gd.occupancy[piece<6 ? 1 : 0],en_passant_target); // delete enemy occupancy
        DEL_BIT(gd.occupancy[2],en_passant_target); // delete total occupancy

        PUT_BIT(gd.bitboards[piece],target); // put own square
        PUT_BIT(gd.occupancy[piece<6 ? 0 : 1],target); // put own occupancy
        PUT_BIT(gd.occupancy[2],target); // put total occupancy

        gd.mailbox[sqr] = INVALID;
        gd.mailbox[target] = piece;
    
    }else if (promote && capture){
        DEL_BIT(gd.bitboards[piece],sqr); // delete sqr
        DEL_BIT(gd.occupancy[piece<6 ? 0 : 1],sqr); // delete occupancy
        DEL_BIT(gd.occupancy[2],sqr); // delete total occupancy

        for(int j=(piece<6? 6: 0); j<(piece<6 ? 12 : 6); j++){
            if(gd.bitboards[j] & (1ULL << target)){
                DEL_BIT(gd.bitboards[j],target); // delete enemy piece and occupancy but not the total occupancy
                j>5 ? gd.piece_numbers[j-6]-- : gd.piece_numbers[j]--;
                break;
            }
        }
        DEL_BIT(gd.occupancy[piece<6 ? 1 : 0],target); // delete enemy occupancy

        PUT_BIT(gd.bitboards[promote],target); // put square
        PUT_BIT(gd.occupancy[piece<6 ? 0 : 1],target); // put in occupancy

        gd.mailbox[sqr] = INVALID;
        gd.mailbox[target] = promote;

    }else if (promote && !capture){
        DEL_BIT(gd.bitboards[piece],sqr); // delete sqr
        DEL_BIT(gd.occupancy[piece<6 ? 0 : 1],sqr); // delete occupancy
        DEL_BIT(gd.occupancy[2],sqr); // delete total occupancy

        PUT_BIT(gd.bitboards[promote],target); // put square
        PUT_BIT(gd.occupancy[piece<6 ? 0 : 1],target); // put in occupancy
        PUT_BIT(gd.occupancy[2],target); // put in total occupancy

        gd.mailbox[sqr] = INVALID;
        gd.mailbox[target] = promote;
    
    }else if(castling){
        //1-remove the king , 2-remove the rook , 3- remove king bitboard 4- remove rook bitboard 5- remove occupancy 6-remove total occupancy 
        //7- add all the previous steps

       int rook_sqr;  int rook_i; int rook_target;

       if (target == g1){// white kingside
           rook_sqr = gd.wr1; rook_i = 2; rook_target = f1;    
       }else if (target == c1){
            rook_sqr = gd.wr2; rook_i = 2; rook_target = d1; 
       }else if (target == g8){
            rook_sqr = gd.br1; rook_i = 8; rook_target = f8; 
       }else if (target == c8){
            rook_sqr = gd.br2; rook_i = 8; rook_target = d8; 
       }
        DEL_BIT(gd.bitboards[piece],sqr); // delete sqr
        DEL_BIT(gd.occupancy[piece<6 ? 0 : 1],sqr); // delete occupancy
        DEL_BIT(gd.occupancy[2],sqr); // delete total occupancy

        PUT_BIT(gd.bitboards[piece],target); // put square
        PUT_BIT(gd.occupancy[piece<6 ? 0 : 1],target); // put in occupancy
        PUT_BIT(gd.occupancy[2],target); // put in total occupancy
        
        ////////

        DEL_BIT(gd.bitboards[rook_i],rook_sqr); // delete sqr
        DEL_BIT(gd.occupancy[piece<6 ? 0 : 1],rook_sqr); // delete occupancy
        DEL_BIT(gd.occupancy[2],rook_sqr); // delete total occupancy

        PUT_BIT(gd.bitboards[rook_i],rook_target); // put square
        PUT_BIT(gd.occupancy[piece<6 ? 0 : 1],rook_target); // put in occupancy
        PUT_BIT(gd.occupancy[2],rook_target); // put in total occupancy

        gd.mailbox[sqr] = INVALID;
        gd.mailbox[target] = piece;
        gd.mailbox[rook_sqr] = INVALID;
        gd.mailbox[rook_target] = rook_i;
    
    }else if(!capture){ // simple moves

        DEL_BIT(gd.bitboards[piece],sqr); // delete sqr
        DEL_BIT(gd.occupancy[piece<6 ? 0 : 1],sqr); // delete occupancy
        DEL_BIT(gd.occupancy[2],sqr); // delete total occupancy

        PUT_BIT(gd.bitboards[piece],target); // put square
        PUT_BIT(gd.occupancy[piece<6 ? 0 : 1],target); // put in occupancy
        PUT_BIT(gd.occupancy[2],target); // put in total occupancy

        gd.mailbox[sqr] = INVALID;
        gd.mailbox[target] = piece;

    }else{// capture
    
        DEL_BIT(gd.bitboards[piece],sqr); // delete sqr
        DEL_BIT(gd.occupancy[piece<6 ? 0 : 1],sqr); // delete occupancy
        DEL_BIT(gd.occupancy[2],sqr); // delete total occupancy

        for(int j=(piece<6? 6: 0); j<(piece<6 ? 12 : 6); j++){
            if(gd.bitboards[j] & (1ULL << target)){
                DEL_BIT(gd.bitboards[j],target); // delete enemy piece and occupancy but not the total occupancy
                j>5 ? gd.piece_numbers[j-6]-- : gd.piece_numbers[j]--;
                break;
            }
        }
        DEL_BIT(gd.occupancy[piece<6 ? 1 : 0],target); // delete enemy occupancy

        PUT_BIT(gd.bitboards[piece],target); // put square
        PUT_BIT(gd.occupancy[piece<6 ? 0 : 1],target); // put in occupancy

        gd.mailbox[sqr] = INVALID;
        gd.mailbox[target] = piece;
    };

    if(gd.side_to_move){gd.fullmove++;};

    if(double_push){ gd.en_passant = (piece < 6 ? (target - 8) : (target + 8) ); }else{gd.en_passant = 0;}
    
    if(piece == 5 || piece == 11 || capture){
        gd.halfmove = 0;
    }else{
        gd.halfmove ++;
    };
    if(gd.castles){
        
        // if(piece == 0 || piece == 6){
        //     gd.side_to_move ? gd.castles &= ~12 : gd.castles &= ~3;
        // }
        // if(sqr == gd.wr1 || target == gd.wr1){
        //     gd.castles &= ~1;
        // }
        // if(sqr == gd.wr2 || target == gd.wr2){
        //     gd.castles &= ~2;           
        // }
        // if(sqr == gd.br1 || target == gd.br1){
        //     gd.castles &= ~4;            
        // }
        // if(sqr == gd.br2 || target == gd.br2){
        //     gd.castles &= ~8;           
        // }

        gd.castles &= gd.castling_rights[sqr];
        gd.castles &= gd.castling_rights[target];
    }
    
    if(piece == 0){ 
        gd.wking_sqr = target;
    }else if(piece == 6){
        gd.bking_sqr = target;
    }

    gd.side_to_move ^= 1;
}


void make_arraymove(game_data& gd , uint8_t (&move)[8]){ // move list
    // assuming that if the side is on check the wrong moves are filterd out and only remains blocking or attacker capturing moves.
    // if not on check go ahead and calculate proper moves and make them


    // uint8_t sqr = move[0];
    // uint8_t target = move[1];
    // uint8_t piece = move[2];
    // uint8_t promote = move[3];
    // uint8_t capture = move[4];
    // uint8_t double_push = move[5];
    // uint8_t enpassant = move[6];
    // uint8_t castling = move[7];

    if(move[6]){
        // algorith =>  delete own piece / delete own occupancy / delete total occupancy (sqr)
        //              delete enemy piece/ delete enemy occupancy / delete total occupancy (enpassant_target)
        //              put own square / put own occupancy / put total occupancy (target) 
        int en_passant_target = move[1] + (move[2]<6 ? -8 : 8);
        DEL_BIT(gd.bitboards[move[2]],move[0]); // delete own sqr
        DEL_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[0]); // delete own occupancy
        DEL_BIT(gd.occupancy[2],move[0]); // delete total occupancy

        for(int j=(move[2]<6? 6: 0); j<(move[2]<6 ? 12 : 6); j++){
            if(gd.bitboards[j] & (1ULL << en_passant_target)){
                DEL_BIT(gd.bitboards[j],en_passant_target); // delete enemy piece
                gd.mailbox[en_passant_target] = INVALID;
                j>5 ? gd.piece_numbers[j-6]-- : gd.piece_numbers[j]--;
                break;
            }
        }
        DEL_BIT(gd.occupancy[move[2]<6 ? 1 : 0],en_passant_target); // delete enemy occupancy
        DEL_BIT(gd.occupancy[2],en_passant_target); // delete total occupancy

        PUT_BIT(gd.bitboards[move[2]],move[1]); // put own square
        PUT_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[1]); // put own occupancy
        PUT_BIT(gd.occupancy[2],move[1]); // put total occupancy
        
        gd.mailbox[move[0]] = INVALID;
        gd.mailbox[move[1]] = move[2];
        
    
    }else if (move[3] && move[4]){
        DEL_BIT(gd.bitboards[move[2]],move[0]); // delete sqr
        DEL_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[0]); // delete occupancy
        DEL_BIT(gd.occupancy[2],move[0]); // delete total occupancy

        for(int j=(move[2]<6? 6: 0); j<(move[2]<6 ? 12 : 6); j++){
            if(gd.bitboards[j] & (1ULL << move[1])){
                DEL_BIT(gd.bitboards[j],move[1]); // delete enemy piece and occupancy but not the total occupancy
                j>5 ? gd.piece_numbers[j-6]-- : gd.piece_numbers[j]--;
                break;
            }
        }
        DEL_BIT(gd.occupancy[move[2]<6 ? 1 : 0],move[1]); // delete enemy occupancy

        PUT_BIT(gd.bitboards[move[3]],move[1]); // put square
        PUT_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[1]); // put in occupancy

        gd.mailbox[move[0]] = INVALID;
        gd.mailbox[move[1]] = move[3];

    }else if (move[3] && !move[4]){
        DEL_BIT(gd.bitboards[move[2]],move[0]); // delete sqr
        DEL_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[0]); // delete occupancy
        DEL_BIT(gd.occupancy[2],move[0]); // delete total occupancy

        PUT_BIT(gd.bitboards[move[3]],move[1]); // put square
        PUT_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[1]); // put in occupancy
        PUT_BIT(gd.occupancy[2],move[1]); // put in total occupancy

        gd.mailbox[move[0]] = INVALID;
        gd.mailbox[move[1]] = move[3];
    
    }else if(move[7]){
        //1-remove the king , 2-remove the rook , 3- remove king bitboard 4- remove rook bitboard 5- remove occupancy 6-remove total occupancy 
        //7- add all the previous steps

       int rook_sqr;  int rook_i; int rook_target;

       if (move[1] == g1){// white kingside
           rook_sqr = gd.wr1; rook_i = 2; rook_target = f1;    
       }else if (move[1] == c1){
            rook_sqr = gd.wr2; rook_i = 2; rook_target = d1; 
       }else if (move[1] == g8){
            rook_sqr = gd.br1; rook_i = 8; rook_target = f8; 
       }else if (move[1] == c8){
            rook_sqr = gd.br2; rook_i = 8; rook_target = d8; 
       }
        DEL_BIT(gd.bitboards[move[2]],move[0]); // delete sqr
        DEL_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[0]); // delete occupancy
        DEL_BIT(gd.occupancy[2],move[0]); // delete total occupancy

        PUT_BIT(gd.bitboards[move[2]],move[1]); // put square
        PUT_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[1]); // put in occupancy
        PUT_BIT(gd.occupancy[2],move[1]); // put in total occupancy
        
        ////////

        DEL_BIT(gd.bitboards[rook_i],rook_sqr); // delete sqr
        DEL_BIT(gd.occupancy[move[2]<6 ? 0 : 1],rook_sqr); // delete occupancy
        DEL_BIT(gd.occupancy[2],rook_sqr); // delete total occupancy

        PUT_BIT(gd.bitboards[rook_i],rook_target); // put square
        PUT_BIT(gd.occupancy[move[2]<6 ? 0 : 1],rook_target); // put in occupancy
        PUT_BIT(gd.occupancy[2],rook_target); // put in total occupancy

        gd.mailbox[move[0]] = INVALID;
        gd.mailbox[move[1]] = move[2];
        gd.mailbox[rook_sqr] = INVALID;
        gd.mailbox[rook_target] = rook_i;
    
    }else if(!move[4]){ // simple moves

        DEL_BIT(gd.bitboards[move[2]],move[0]); // delete sqr
        DEL_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[0]); // delete occupancy
        DEL_BIT(gd.occupancy[2],move[0]); // delete total occupancy

        PUT_BIT(gd.bitboards[move[2]],move[1]); // put square
        PUT_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[1]); // put in occupancy
        PUT_BIT(gd.occupancy[2],move[1]); // put in total occupancy

        gd.mailbox[move[0]] = INVALID;
        gd.mailbox[move[1]] = move[2];

    }else{// capture
    
        DEL_BIT(gd.bitboards[move[2]],move[0]); // delete sqr
        DEL_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[0]); // delete occupancy
        DEL_BIT(gd.occupancy[2],move[0]); // delete total occupancy

        for(int j=(move[2]<6? 6: 0); j<(move[2]<6 ? 12 : 6); j++){
            if(gd.bitboards[j] & (1ULL << move[1])){
                DEL_BIT(gd.bitboards[j],move[1]); // delete enemy piece and occupancy but not the total occupancy
                j>5 ? gd.piece_numbers[j-6]-- : gd.piece_numbers[j]--;
                break;
            }
        }
        DEL_BIT(gd.occupancy[move[2]<6 ? 1 : 0],move[1]); // delete enemy occupancy

        PUT_BIT(gd.bitboards[move[2]],move[1]); // put square
        PUT_BIT(gd.occupancy[move[2]<6 ? 0 : 1],move[1]); // put in occupancy

        gd.mailbox[move[0]] = INVALID;
        gd.mailbox[move[1]] = move[2];
    };

    if(gd.side_to_move){gd.fullmove++;};

    if(move[5]){ gd.en_passant = (move[2] < 6 ? (move[1] - 8) : (move[1] + 8) ); }else{gd.en_passant = 0;}
    
    if(move[2] == 5 || move[2] == 11 || move[4]){
        gd.halfmove = 0;
    }else{
        gd.halfmove ++;
    };
    if(gd.castles){
        
        //  if(move[2] == 0 || move[2] == 6){
        //     gd.side_to_move ? gd.castles &= ~12 : gd.castles &= ~3;
        // }
        // if(move[0] == gd.wr1 || move[1] == gd.wr1){
        //     gd.castles &= ~1;
        // }
        // if(move[0] == gd.wr2 || move[1] == gd.wr2){
        //     gd.castles &= ~2;           
        // }
        // if(move[0] == gd.br1 || move[1] == gd.br1){
        //     gd.castles &= ~4;            
        // }
        // if(move[0] == gd.br2 || move[1] == gd.br2){
        //     gd.castles &= ~8;           
        // }

        gd.castles &= gd.castling_rights[move[0]];
        gd.castles &= gd.castling_rights[move[1]];
    }
    
    if(move[2] == 0){ 
        gd.wking_sqr = move[1];
    }else if(move[2] == 6){
        gd.bking_sqr = move[1];
    }

    gd.side_to_move ^= 1;
}



// is square current given attacked by the current given side
inline void get_checks(int sqr, game_data& gd, uint64_t* arr){
    
    if(gd.side_to_move){ // black to move - black king in check

        uint64_t wpa = raw_attacks[6][sqr] & gd.bitboards[5]; // we put a black pawn in kings square to get white pawns attacks
        if(wpa){ arr[0] = get_first_square(wpa);}
        
        uint64_t wna = raw_attacks[4][sqr] & gd.bitboards[4];
        if(wna){ if(arr[0]==INVALID){ arr[0]= get_first_square(wna);}else{arr[1]=1;}}
        

        uint64_t bishand = get_sliding_attacks(0,sqr,gd.occupancy[2])& gd.bitboards[3];
        uint64_t rookand = get_sliding_attacks(1,sqr,gd.occupancy[2])& gd.bitboards[2];
        uint64_t queenand = get_queen_attacks(sqr, gd.occupancy[2])& gd.bitboards[1];

        if (bishand ){ if(arr[0]==INVALID){ arr[0]= get_first_square(bishand) ;}else{arr[1]=1;}}
        //if (rookand ){ if(arr[0]==INVALID){ arr[0]= get_first_square(rookand) ;}else{arr[1] = 1;}}
        while(rookand){
            int rookand_s = get_first_square(rookand);
            if(arr[0]==INVALID){ arr[0]= rookand_s;}else{arr[1] = 1;};
            DEL_BIT(rookand,rookand_s);
        }
        //if (queenand ){ if(arr[0]==INVALID){ arr[0]= get_first_square(queenand) ;}else{arr[1]=1;}}
        
        while(queenand){
            int queenand_s = get_first_square(queenand);
            if(arr[0]==INVALID){ arr[0]= queenand_s;}else{arr[1] = 1;};
            DEL_BIT(queenand,queenand_s);
        }

    }else{
        
        uint64_t bpa = raw_attacks[5][sqr] & gd.bitboards[11]; // we put a white pawn in kings square to get black pawns attacks
        if(bpa){ if(arr[0]==INVALID){ arr[0]= get_first_square(bpa);}else{arr[1]=1;}}

        uint64_t bna = raw_attacks[4][sqr] & gd.bitboards[10];
        if(bna){ if(arr[0]==INVALID){ arr[0]= get_first_square(bna);}else{arr[1]=1;}}

        uint64_t bishand = (get_sliding_attacks(0,sqr,gd.occupancy[2]) & gd.bitboards[9]);
        uint64_t rookand = (get_sliding_attacks(1,sqr,gd.occupancy[2]) &  gd.bitboards[8]);
        uint64_t queenand = (get_queen_attacks(sqr, gd.occupancy[2]) & gd.bitboards[7]);

        if (bishand){ if(arr[0]==INVALID){ arr[0]= get_first_square(bishand);}else{arr[1]=1;}}
                while(rookand){
            int rookand_s = get_first_square(rookand);
            if(arr[0]==INVALID){ arr[0]= rookand_s;}else{arr[1] = 1;};
            DEL_BIT(rookand,rookand_s);
        }
        while(queenand){
            int queenand_s = get_first_square(queenand);
            if(arr[0]==INVALID){ arr[0]= queenand_s;}else{arr[1] = 1;};
            DEL_BIT(queenand,queenand_s);
        }

    }
    //if(arr[0]!=INVALID){print_chessboard(gd);}
    
}

    
bool move_and_check(game_data& orig_gd,int sqr,int target,int piece,int capture,int enpassant,int castling){
    
    game_data copy_gd = game_data(orig_gd); // using copy constructor
    
    if(enpassant){
        // algorith =>  delete own piece / delete own occupancy / delete total occupancy (sqr)
        //              delete enemy piece/ delete enemy occupancy / delete total occupancy (enpassant_target)
        //              put own square / put own occupancy / put total occupancy (target) 
        int en_passant_target = target + (piece<6 ? -8 : 8);
        DEL_BIT(copy_gd.bitboards[piece],sqr); // delete own sqr
        DEL_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],sqr); // delete own occupancy
        DEL_BIT(copy_gd.occupancy[2],sqr); // delete total occupancy
 
        for(int j=(piece<6? 6: 0); j<(piece<6 ? 12 : 6); j++){
            if(copy_gd.bitboards[j] & (1ULL << en_passant_target)){
                DEL_BIT(copy_gd.bitboards[j],en_passant_target); // delete enemy piece
                break;
            }
        }
        DEL_BIT(copy_gd.occupancy[piece<6 ? 1 : 0],en_passant_target); // delete enemy occupancy
        DEL_BIT(copy_gd.occupancy[2],en_passant_target); // delete total occupancy

        PUT_BIT(copy_gd.bitboards[piece],target); // put own square
        PUT_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],target); // put own occupancy
        PUT_BIT(copy_gd.occupancy[2],target); // put total occupancy
    
    }else if(!capture){ // simple moves

        DEL_BIT(copy_gd.bitboards[piece],sqr); // delete sqr
        DEL_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],sqr); // delete occupancy
        DEL_BIT(copy_gd.occupancy[2],sqr); // delete total occupancy

        PUT_BIT(copy_gd.bitboards[piece],target); // put square
        PUT_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],target); // put in occupancy
        PUT_BIT(copy_gd.occupancy[2],target); // put in total occupancy
    
    }else if(castling){
        //1-remove the king , 2-remove the rook , 3- remove king bitboard 4- remove rook bitboard 5- remove occupancy 6-remove total occupancy 
        //7- add all the previous steps

       int rook_sqr;  int rook_i; int rook_target;

       if (target == g1){// white kingside
           rook_sqr = copy_gd.wr1; rook_i = 2; rook_target = f1;    
       }else if (target == c1){
            rook_sqr = copy_gd.wr2; rook_i = 2; rook_target = d1; 
       }else if (target == g8){
            rook_sqr = copy_gd.br1; rook_i = 8; rook_target = f8; 
       }else if (target == c8){
            rook_sqr = copy_gd.br2; rook_i = 8; rook_target = d8; 
       }
        DEL_BIT(copy_gd.bitboards[piece],sqr); // delete sqr
        DEL_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],sqr); // delete occupancy
        DEL_BIT(copy_gd.occupancy[2],sqr); // delete total occupancy

        PUT_BIT(copy_gd.bitboards[piece],target); // put square
        PUT_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],target); // put in occupancy
        PUT_BIT(copy_gd.occupancy[2],target); // put in total occupancy
        
        ////////

        DEL_BIT(copy_gd.bitboards[rook_i],rook_sqr); // delete sqr
        DEL_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],rook_sqr); // delete occupancy
        DEL_BIT(copy_gd.occupancy[2],rook_sqr); // delete total occupancy

        PUT_BIT(copy_gd.bitboards[rook_i],rook_target); // put square
        PUT_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],rook_target); // put in occupancy
        PUT_BIT(copy_gd.occupancy[2],rook_target); // put in total occupancy

        

    }else{// capture
     
        DEL_BIT(copy_gd.bitboards[piece],sqr); // delete sqr
        DEL_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],sqr); // delete occupancy
        DEL_BIT(copy_gd.occupancy[2],sqr); // delete total occupancy

        for(int j=(piece<6? 6: 0); j<(piece<6 ? 12 : 6); j++){
            if(copy_gd.bitboards[j] & (1ULL << target)){
                DEL_BIT(copy_gd.bitboards[j],target); // delete enemy piece and occupancy but not the total occupancy
                break;
            }
        }
        DEL_BIT(copy_gd.occupancy[piece<6 ? 1 : 0],target); // delete enemy occupancy

        PUT_BIT(copy_gd.bitboards[piece],target); // put square
        PUT_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],target); // put in occupancy
    }

    if(piece == 0){ 
        copy_gd.wking_sqr = target;
    }else if(piece == 6){
        copy_gd.bking_sqr = target;
    }
    return IS_SQUARE_ATTACKED(copy_gd, (piece<6 ? copy_gd.wking_sqr : copy_gd.bking_sqr), (piece <6 ? 1 : 0));

}



bool is_square_attacked(game_data& gd, int sqr, int attacker_side){ // 0 - white attacker_side / 1 - black attacker side
    
    if(!attacker_side){ // white is attacker
       
        if(raw_attacks[0][sqr] & gd.bitboards[0]){return true;} // king
  
        if(raw_attacks[6][sqr] & gd.bitboards[5]){ return true;} //pawn
        
        if(raw_attacks[4][sqr] & gd.bitboards[4]){ return true;} // knight

        if (get_sliding_attacks(0,sqr,gd.occupancy[2])& gd.bitboards[3] ){ return true;}
        if (get_sliding_attacks(1,sqr,gd.occupancy[2])& gd.bitboards[2] ){ return true;}
        if (get_queen_attacks(sqr, gd.occupancy[2])& gd.bitboards[1] ){ return true;}
        

    }else{
        
        if(raw_attacks[0][sqr] & gd.bitboards[6]){return true;} // king

        if(raw_attacks[5][sqr] & gd.bitboards[11]){return true;}

        if(raw_attacks[4][sqr] & gd.bitboards[10]){ return true;}

        if (get_sliding_attacks(0,sqr,gd.occupancy[2]) & gd.bitboards[9]){ return true;}
        if (get_sliding_attacks(1,sqr,gd.occupancy[2]) & ( gd.bitboards[8])){ return true;}
        if (get_queen_attacks(sqr, gd.occupancy[2]) & gd.bitboards[7]){ return true;}

    }

    return false;
    
}




#define GET_SLIDING_ATTACKS(p, sqr, occupancy) \
    ((occupancy) &= raw_attacks[(p==3 || p==0) ? 3 : 2][sqr], \
    sliding_attack_tables[(p==3 || p==0) ? 0 : 1][sqr][occupancy])

#define GET_QUEEN_ATTACKS(sqr, occupancy) \
    ({ uint64_t queen_attacks = 0ULL; \
    uint64_t queen_occupancy = (occupancy); \
    (occupancy) &= raw_attacks[3][sqr]; \
    queen_attacks |= sliding_attack_tables[0][sqr][(occupancy)]; \
    queen_occupancy &= raw_attacks[2][sqr]; \
    queen_attacks |= sliding_attack_tables[1][sqr][queen_occupancy]; \
    queen_attacks; })




bool check_potential(game_data& gd , int sqr, int target , int piece){


    uint64_t bitboard = 1ULL << target; 
    //DEL_BIT(bitboard, sqr);
    
    int king = gd.side_to_move ? gd.wking_sqr : gd.bking_sqr;
    
    if( piece == 4 || piece == 5 || piece == 10 || piece == 11 ){ //

        if(raw_attacks[piece < 6 ? piece : piece - 6][king] & bitboard){ return true;} // knight
    
    }else{
        uint64_t occupancy = 0ULL;
        if(piece == 1 || piece == 7){
            if (GET_QUEEN_ATTACKS(king, occupancy) & bitboard){ return true;}
        }else{

            if (GET_SLIDING_ATTACKS( ((piece == 3 || piece == 9) ? 0 : 1 ) , king ,occupancy) & bitboard){ return true;}
        }
    }
    return false;
}

//#define CHECK_POTENTIAL(king, piece, target) (potential_table[(king)][(piece)][(target)])




