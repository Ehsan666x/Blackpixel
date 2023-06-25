
#include "../include/perft.h"
        //perft//
/*
        
    u64 Perft(int depth // assuming >= 1 )
    {
    MOVE move_list[256];
    int n_moves, i;
    u64 nodes = 0;

    n_moves = GenerateLegalMoves(move_list);

    if (depth == 1) 
        return (u64) n_moves;

    for (i = 0; i < n_moves; i++) {
        MakeMove(move_list[i]); 
        nodes += Perft(depth - 1);
        UndoMove(move_list[i]);
    }
    return nodes;
    }


*/

uint64_t perft(int depth, game_data& gd, bool main = true){
    uint64_t nodes = 0;
    //int move_nodes =0;
    MoveList move_list;
    move_list.generate(gd);
    uint64_t n_moves = move_list._size;
    //log(n_moves);
    if(depth == 1){return n_moves;}
 
    game_data ori(gd);
    for( uint32_t move : move_list){
        make_move(gd, move);
        uint64_t inner_node = perft(depth-1 ,gd, false);
        nodes += inner_node;
        if(main){log(notations[GET_SOURCE_SQUARE(move)]+notations[GET_TARGET_SQUARE(move)] + " " + std::to_string(inner_node));inner_node=0;}
        //nodes += perft(depth-1 , gd, false);
        gd = game_data(ori);
    }
    return nodes;
};

