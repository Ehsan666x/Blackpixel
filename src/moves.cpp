#include <iostream>

#include "../include/globals.h"
#include "../include/bitboards.h"
#include "../include/attacks.h"
#include "../include/moves.h"
#include "../include/prints.h"
#include "../include/pins.h"



uint64_t refine_own_occupency_attack(int& kind , int& sqr,uint64_t& attack ,uint64_t& occupancy){
    
    
    return 0;
};

uint64_t refine_opponent_occupency_attack(int& kind , int& sqr ,uint64_t& attack ,uint64_t& occupancy){
    

    return 0;
};

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
            lm = refine_own_occupency_attack(i , sqr ,lm, gd.occupancy[op]);

            // other logics (in check / pin)

            return 1ULL;
        }
    }
    return 0;
};



void MoveList::generate(game_data gd) {
     _size=generate_moves(gd, move_list); 
        _last = move_list + _size;
}

const uint32_t* MoveList::begin() const { return move_list; }
const uint32_t* MoveList::end() const { return _last; }
size_t MoveList::size() const { return _last - move_list; }
bool MoveList::contains(uint32_t move) const {
    return std::find(begin(), end(), move) != end();
};


/////////////// move generation ////////////////





int generate_moves(game_data& gd ,uint32_t* moves){
    // gd.side_tomove 0 white 1 black
    // legal moves //
    // 1- check if in check
    // 2- get enemy pin mask
    // 2- if check =>get own pin mask, generate specific moves based on enemy and own pin mask 
    //    else => generate moves based on enemy pin mask
    
    int move_count=0;
    uint64_t check_attacker[2]={0,0};
    get_checks(get_first_square((!gd.side_to_move? gd.bitboards[0]: gd.bitboards[6])),gd, check_attacker);

    

    if(!check_attacker[0]){ // not in check
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
                        uint32_t move=0;
                        while (attack_mask){
                            int target = get_first_square(attack_mask);
                            int capture = 0;
                            int en_passant = 0;
                            int double_push = 0;
                            int castling=0;
                            if(gd.occupancy[i<6 ? 1 : 0] & (1ULL << target)){ // 
                                // pawn capture
                                capture = 1;
                                move = sqr;

                            }else if(gd.en_passant== (i==5 ? (target - 8) : (target + 8) ) ){
                                // en passant move
                                en_passant = gd.en_passant;
                                move = sqr;
                            };
                            if(target <8 || target >55) {
                                // promotions
                                if(!(sqr & pins) || !move_and_check(gd, sqr, target, i, capture, en_passant)){ //orig_gd,sqr,target,piece,capture,enpassant
                                
                                    for(int j=1;j<5;j++){ // queen , rook , bishop, knight
                                        move = sqr;
                                        moves[move_count]=encode_move(move, target, i, j, capture, 0, en_passant, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                        move_count++;
                                        //moves[move_count -1]=move;
                                    };
                                    move=0;
                                }
                            };
                            if(move){
                                if(!(sqr & pins) || !move_and_check(gd, sqr, target, i, capture, en_passant)){ //orig_gd,sqr,target,piece,capture,enpassant
                                    moves[move_count]=encode_move(move, target, i, 0, capture, double_push, en_passant, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                }
                                move=0;
                            }
                            
                            DEL_BIT(attack_mask,target);
                        };

                        if( ((int)sqr/8 == 1 && i == 5) || ((int)sqr/8 == 6 && i == 11)){
                            if(  !(gd.occupancy[2] & ( 1ULL << (sqr + (i==5 ? 8 : -8)) ) ) && ! (gd.occupancy[2] & ( 1ULL << (sqr + (i==5 ? 16 : -16)) )) ){
                                // double push
                                if(!(sqr & pins) || !move_and_check(gd, sqr, (sqr + (i==5 ? 16 : -16)), i, 0, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                
                                    moves[move_count]=encode_move(sqr, (sqr + (i==5 ? 16 : -16)), i, 0, 0, 1, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                }
                            };
                        };
                        if( !(gd.occupancy[2] & ( 1ULL << (sqr + (i==5 ? 8 : -8)) ) ) ){ 
                            // single push_pawn
                            if(!(sqr & pins) || !move_and_check(gd, sqr, (sqr + (i==5 ? 8 : -8)), i, 0, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                moves[move_count]=encode_move(sqr, (sqr + (i==5 ? 8 : -8)), i, 0, 0, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                move_count++;
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
                        
                        uint32_t move=0;
                        while (attack_mask){
                            int target = get_first_square(attack_mask);
                            int capture=0;
                            
                            if(gd.occupancy[i<6 ? 1 : 0] & (1ULL << target)){ // 
                                // capture
                                move = sqr;
                                capture = 1;
                            }else if ( !(gd.occupancy[i<6 ? 0 : 1] & (1ULL << target) ) ){ 
                                // regular move
                                move = sqr;
                            };

                            if(move){
                                if(!(sqr & pins) || !move_and_check(gd, sqr, target, ((i==0||i==6)?0:4), capture, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                    moves[move_count]=encode_move(move, target, ((i==0||i==6)?0:4), 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                }
                                move=0;
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
                                for(int f=gd.r1+1; f<gd.k; f++ ){
                                    if(gd.occupancy[2] & (1ULL << f)){
                                        kingside = 0;
                                        break;
                                    };
                                };
                                if(kingside){
                                    if(gd.r1>g1){
                                        for(int f=g1; f<gd.r1; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                kingside = 0;
                                                break;
                                            };
                                        };
                                    };
                                };
                                if(kingside && !is_square_attacked(gd , g1 , 1)){ // needs to cjeck also the middle road for checks
                                    moves[move_count]=encode_move(gd.k, g1, ((i==0||i==6)?0:4), 0, 0, 0, 0, kingside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                };
                            
                            };
                            if(gd.castles & 2){ // queen side

                                int queenside=1;
                                for(int f=gd.k+1; f<gd.r2; f++ ){
                                    if(gd.occupancy[2] & (1ULL << f)){
                                        queenside = 0;
                                        break;
                                    };
                                };
                                if(queenside){
                                    if(gd.r2<c1){
                                        for(int f=c1; f<gd.r2; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                    }else if(gd.r2>c1){
                                        for(int f=gd.r2+1; f<c1; f++ ){ // c1 is not checked in case of king on c1
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                    };
                                };
                                if(queenside && !is_square_attacked(gd , c1 , 1)){
                                    moves[move_count]=encode_move(gd.k, c1, ((i==0||i==6)?0:4), 0, 0, 0, 0, queenside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                };

                            }

                        }else{//black
                            if(gd.castles & 4){ // king side
                                int kingside=1;
                                for(int f=gd.r1+56+1; f<gd.k+65; f++ ){
                                    if(gd.occupancy[2] & (1ULL << f)){
                                        kingside = 0;
                                        break;
                                    };
                                };
                                if(kingside){
                                    if(gd.r1+56>g8){
                                        for(int f=g8; f>gd.r1+56; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                kingside = 0;
                                                break;
                                            };
                                        };
                                    };
                                };
                                if(kingside && !is_square_attacked(gd , g8 , 0)){
                                    moves[move_count]=encode_move(gd.k+56, g8, ((i==0||i==6)?0:4), 0, 0, 0, 0, kingside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                };
                            
                            };
                            if(gd.castles & 8){ // queen side

                                int queenside=1;
                                for(int f=gd.k+56+1; f<gd.r2+56; f++ ){
                                    if(gd.occupancy[2] & (1ULL << f)){
                                        queenside = 0;
                                        break;
                                    };
                                };
                                if(queenside){
                                    if(gd.r2+56<c8){
                                        for(int f=c8; f<gd.r2+56; f++ ){
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                    }else if(gd.r2+56>c8){
                                        for(int f=gd.r2+56+1; f<c8; f++ ){ // c1 is not checked in case of king on c1
                                            if(gd.occupancy[2] & (1ULL << f)){
                                                queenside = 0;
                                                break;
                                            };
                                        };
                                    };
                                };
                                if(queenside && !is_square_attacked(gd , c8 , 0)){
                                    moves[move_count]=encode_move(gd.k+56, c8, ((i==0||i==6)?0:4), 0, 0, 0, 0, queenside);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
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
                            attack_mask = get_sliding_attacks( (i==3||i==10) ? 0 : 1,sqr, gd.occupancy[2]); // bishop or rook
                        };
                        
                        uint32_t move=0;
                        while (attack_mask){
                            int target = get_first_square(attack_mask);
                            int capture=0;
                            if(gd.occupancy[i<6 ? 1 : 0] & (1ULL << target)){ // 
                                // capture
                                move = sqr;
                                capture =1;
                            }else if ( !(gd.occupancy[i<6 ? 0 : 1] & (1ULL << target) ) ){ 
                                // regular move
                                move = sqr;
                            };
                            if(move){
                                int piece;
                                if(i==1 || i==7){//queen
                                    piece=1;
                                }else if(i==2 || i==8){//rook
                                    piece=2;
                                }else{
                                    piece=3;
                                };
                                if(!(sqr & pins) || !move_and_check(gd, sqr, target, piece, capture, 0)){ //orig_gd,sqr,target,piece,capture,enpassant
                                    moves[move_count]=encode_move(move, target, piece, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                                    move_count++;
                                };
                                move=0;
                            };
                            DEL_BIT(attack_mask,target);
                        };
                        DEL_BIT(copy_bitboard,sqr);
                    };
                };

            }
        }
    
    }else if(check_attacker[1]){// double check
        //move the king to a square thats not attacked
        uint64_t attacker_attacks;
        if(!gd.side_to_move){//white king
            int king_sqr = get_first_square(gd.bitboards[0]);
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move=0; int capture=0;
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
                if(move && !is_square_attacked(gd, move, 1)){ // black is attacker
                    // test if its still in check after the move
                    
                    moves[move_count]=encode_move(move, target, 0, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }

                DEL_BIT(king_attacks,target);
            }
        }else{// black king
            int king_sqr = get_first_square(gd.bitboards[6]);
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move=0; int capture=0;
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
                if(move && !is_square_attacked(gd, move, 0)){ // white is attacker
                    // test if its still in check after the move
                    
                    moves[move_count]=encode_move(move, target, 6, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }

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
                    if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 5, 1, 0)){ //orig_gd,sqr,target,piece,capture,enpassant

                        for(int j=1;j<5;j++){ // queen , rook , bishop, knight
                            moves[move_count]=encode_move(sqr, check_attacker[0], 5, 1, j, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                            move_count++;
                            //moves[move_count -1]=move;
                        };
                    };
                
                }else if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 5, 1, 0)){
                    moves[move_count]=encode_move(sqr, check_attacker[0], 5, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;                
                }  
                DEL_BIT(wpa,sqr);
            };
            // en passant capture the attacker.
            if(gd.en_passant== (check_attacker[0] + 8)){
                uint64_t enpass_wpa = raw_attacks[6][check_attacker[0] + 8] & gd.bitboards[5];
                while(enpass_wpa){
                    int sqr = get_first_square(enpass_wpa);
                    if(!(sqr & pins) || !move_and_check(gd, sqr, (check_attacker[0] + 8), 5, 1, 1)){//orig_gd,sqr,target,piece,capture,enpassant
                        moves[move_count]=encode_move(sqr, (check_attacker[0] + 8), 5, 0, 1, 0, 1, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                        move_count++; 
                    }
                    DEL_BIT(enpass_wpa,sqr);
                }
                               
            };

            uint64_t wna = raw_attacks[4][check_attacker[0]] & gd.bitboards[4];
            while(wna){ 
                int sqr = get_first_square(wna);
                if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 4, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
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
                if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 3, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 3, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }  
                DEL_BIT(bishand,sqr);
            };
            while (rookand){ 
                int sqr = get_first_square(rookand);
                if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 2, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 2, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }  
                DEL_BIT(rookand,sqr);
            };
            while (queenand){ 
                int sqr = get_first_square(queenand);
                if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 1, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
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
                    if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0],11, 1, 0)){ //orig_gd,sqr,target,piece,capture,enpassant

                        for(int j=1;j<5;j++){ // queen , rook , bishop, knight
                            moves[move_count]=encode_move(sqr, check_attacker[0], 11, 1, j, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                            move_count++;
                            //moves[move_count -1]=move;
                        };
                    };
                }else if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 11, 1, 0)){
                    moves[move_count]=encode_move(sqr, check_attacker[0], 11, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;                
                }  
                DEL_BIT(bpa,sqr);
            };
            // en passant capture the attacker.
            if(gd.en_passant== (check_attacker[0] - 8)){
                uint64_t enpass_wpa = raw_attacks[5][check_attacker[0] - 8] & gd.bitboards[11];
                while(enpass_wpa){
                    int sqr = get_first_square(enpass_wpa);
                    if(!(sqr & pins) || !move_and_check(gd, sqr, (check_attacker[0] - 8), 11, 1, 1)){//orig_gd,sqr,target,piece,capture,enpassant
                        moves[move_count]=encode_move(sqr, (check_attacker[0] - 8), 11, 0, 1, 0, 1, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                        move_count++; 
                    }
                    DEL_BIT(enpass_wpa,sqr);
                }
                               
            };

            uint64_t bna = raw_attacks[4][check_attacker[0]] & gd.bitboards[10];
            while(bna){ 
                int sqr = get_first_square(bna);
                if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 10, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 10, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;  
                }  
                DEL_BIT(bna,sqr);
            };

            uint64_t bishand = (get_sliding_attacks(0,check_attacker[0],gd.occupancy[2]) & gd.bitboards[9]);
            uint64_t rookand = (get_sliding_attacks(1,check_attacker[0],gd.occupancy[2]) & ( gd.bitboards[8] ));
            uint64_t queenand = (get_queen_attacks(check_attacker[0], gd.occupancy[2] & gd.bitboards[7]));

            while (bishand){ 
                int sqr = get_first_square(bishand); 
                if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 9, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 9, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;  
                } 
                DEL_BIT(bishand,sqr);
            };
            while (rookand){ 
                int sqr = get_first_square(rookand);
                if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 8, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 8, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;  
                }  
                DEL_BIT(rookand,sqr);
            };
            while (queenand){ 
                int sqr = get_first_square(queenand);
                if(!(sqr & pins) || !move_and_check(gd, sqr, check_attacker[0], 7, 1, 0)){//orig_gd,sqr,target,piece,capture,enpassant
                    moves[move_count]=encode_move(sqr, check_attacker[0], 7, 0, 1, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++;  
                }  
                DEL_BIT(queenand,sqr);
            };

        }
          
        // move the king to a square thats not attacked
        uint64_t attacker_attacks;
        if(!gd.side_to_move){//white king
            int king_sqr = get_first_square(gd.bitboards[0]);
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move=0; int capture=0;
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
                if(move && !is_square_attacked(gd, move, 1)){ // black is attacker
                    // test if its still in check after the move
                    
                    moves[move_count]=encode_move(move, target, 0, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }

                DEL_BIT(king_attacks,target);
            }
        }else{// black king
            int king_sqr = get_first_square(gd.bitboards[6]);
            uint64_t king_attacks = raw_attacks[0][king_sqr];
            int move=0; int capture=0;
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
                if(move && !is_square_attacked(gd, move, 0)){ // white is attacker
                    // test if its still in check after the move
                    
                    moves[move_count]=encode_move(move, target, 6, 0, capture, 0, 0, 0);//sqr, target, piece, promoted, capture, double_push, enpassant, castling
                    move_count++; 
                }

                DEL_BIT(king_attacks,target);
            }
        }
        

    }
    
    return move_count;
};




inline bool make_move(game_data& gd, int sqr, int target){ // move list
    // assuming that if the side is on check the wrong moves are filterd out and only remains blocking or attacker capturing moves.
    // if not on check go ahead and calculate proper moves and make them
    uint64_t pins = pin_mask(gd);
    for(uint32_t move : gd.piece_moves){

    }

}



// is square current given attacked by the current given side
void get_checks(int sqr, game_data& gd, uint64_t* arr){
    
    if(gd.side_to_move){ // black to move - black king in check

        uint64_t wpa = raw_attacks[6][sqr] & gd.bitboards[5]; // we put a black pawn in kings square to get white pawns attacks
        if(wpa){ arr[0] = wpa;}
        
        uint64_t wna = raw_attacks[4][sqr] & gd.bitboards[4];
        if(wna){ if(!arr[0]){ arr[0]= wna;}else{arr[1]=wna;}}
        

        uint64_t bishand = get_sliding_attacks(0,sqr,gd.occupancy[2])& gd.bitboards[3];
        uint64_t rookand = get_sliding_attacks(1,sqr,gd.occupancy[2])& gd.bitboards[2];
        uint64_t queenand = get_queen_attacks(sqr, gd.occupancy[2])& gd.bitboards[1];

        if (bishand ){ if(!arr[0]){ arr[0]= bishand ;}else{arr[1]=bishand;}}
        if (rookand ){ if(!arr[0]){ arr[0]= rookand ;}else{arr[1] = rookand;}}
        if (queenand ){ if(!arr[0]){ arr[0]= queenand ;}else{arr[1]=queenand;}}

    }else{
        
        uint64_t bpa = raw_attacks[5][sqr] & gd.bitboards[11]; // we put a white pawn in kings square to get black pawns attacks
        if(bpa){ if(!arr[0]){ arr[0]= bpa;}else{arr[1]=bpa;}}

        uint64_t bna = raw_attacks[4][sqr] & gd.bitboards[10];
        if(bna){ if(!arr[0]){ arr[0]= bna;}else{arr[1]=bna;}}

        uint64_t bishand = (get_sliding_attacks(0,sqr,gd.occupancy[2]) & gd.bitboards[9]);
        uint64_t rookand = (get_sliding_attacks(1,sqr,gd.occupancy[2]) & ( gd.bitboards[8] ));
        uint64_t queenand = (get_queen_attacks(sqr, gd.occupancy[2] & gd.bitboards[7]));

        if (bishand){ if(!arr[0]){ arr[0]= bishand;}else{arr[1]=bishand;}}
        if (rookand){ if(!arr[0]){ arr[0]= rookand;}else{arr[1]=rookand;}}
        if (queenand){ if(!arr[0]){ arr[0]= queenand;}else{arr[1]=queenand;}}

    }
    
}

bool is_square_attacked(game_data& gd, int sqr, int attacker_side){ // 0 - white attacker_side / 1 - black attacker side
    
    if(!attacker_side){ // white is attacker
       
        uint64_t wka = raw_attacks[0][sqr] & gd.bitboards[0];
        if(wka){return true;}
  
        uint64_t wpa = raw_attacks[6][sqr] & gd.bitboards[5]; // we put a black pawn in kings square to get white pawns attacks
        if(wpa){ return true;}
        
        uint64_t wna = raw_attacks[4][sqr] & gd.bitboards[4];
        if(wna){ return true;}
        

        uint64_t bishand = get_sliding_attacks(0,sqr,gd.occupancy[2])& gd.bitboards[3];
        uint64_t rookand = get_sliding_attacks(1,sqr,gd.occupancy[2])& gd.bitboards[2];
        uint64_t queenand = get_queen_attacks(sqr, gd.occupancy[2])& gd.bitboards[1];

        if (bishand ){ return true;}
        if (rookand ){ return true;}
        if (queenand ){ return true;}
        

    }else{
        
        uint64_t bpa = raw_attacks[5][sqr] & gd.bitboards[11]; // we put a white pawn in kings square to get black pawns attacks
        if(bpa){return true;}

        uint64_t bna = raw_attacks[4][sqr] & gd.bitboards[10];
        if(bna){ return true;}

        uint64_t bishand = (get_sliding_attacks(0,sqr,gd.occupancy[2]) & gd.bitboards[9]);
        uint64_t rookand = (get_sliding_attacks(1,sqr,gd.occupancy[2]) & ( gd.bitboards[8] ));
        uint64_t queenand = (get_queen_attacks(sqr, gd.occupancy[2] & gd.bitboards[7]));

        if (bishand){ return true;}
        if (rookand){ return true;}
        if (queenand){ return true;}

    }

    return false;
    
}
    
bool move_and_check(game_data& orig_gd,int sqr,int target,int piece,int capture,int enpassant){
    
    game_data copy_gd = game_data(orig_gd); // using copy constructor
    
    if(enpassant){
        // algorith =>  delete own piece / delete own occupancy / delete total occupancy (sqr)
        //              delete enemy piece/ delete enemy occupancy / delete total occupancy (enpassant_target)
        //              put own square / put own occupancy / put total occupancy (target) 
        int en_passant_target = target + (piece<6 ? 8 : -8);
        DEL_BIT(copy_gd.bitboards[piece],sqr); // delete own sqr
        DEL_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],sqr); // delete own occupancy
        DEL_BIT(copy_gd.occupancy[2],sqr); // delete total occupancy
        int j;
        for(j=(piece<6? 6: 0); j<(piece<6 ? 6 : 12); j++){
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

    }else{// capture
     
        DEL_BIT(copy_gd.bitboards[piece],sqr); // delete sqr
        DEL_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],sqr); // delete occupancy
        DEL_BIT(copy_gd.occupancy[2],sqr); // delete total occupancy
        int j;
        for(j=(piece<6? 6: 0); j<(piece<6 ? 6 : 12); j++){
            if(copy_gd.bitboards[j] & (1ULL << target)){
                DEL_BIT(copy_gd.bitboards[j],target); // delete enemy piece and occupancy but not the total occupancy
                break;
            }
        }
        DEL_BIT(copy_gd.occupancy[piece<6 ? 1 : 0],target); // delete enemy occupancy

        PUT_BIT(copy_gd.bitboards[piece],target); // put square
        PUT_BIT(copy_gd.occupancy[piece<6 ? 0 : 1],target); // put in occupancy
    }
    
    return is_square_attacked(copy_gd, get_first_square(copy_gd.bitboards[piece<6 ? 0 : 6]), (piece <6 ? 1 : 0));

}