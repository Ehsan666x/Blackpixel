


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

