#include "../include/openings.h"



// Function to check if a file exists
bool fileExists(std::string filename) {
    std::ifstream file(filename);
    bool exists = file.good();
    //file.close();
    return exists;
}

void create_zobrist_values(std::string filename){

        std::ofstream outputfile(filename);
        if (!outputfile.is_open()){
            //std::cerr << "Error init_zobrist file for reading!\n";

            std::istringstream stream(zobristvalues);
            std::string wholeline;
            
            while (std::getline(stream, wholeline)) { 
                int piece;
                int sqr;
                uint64_t hash;
                std::stringstream ss(wholeline);
                ss >> piece;
                ss >> sqr;
                ss >> hash;
                zobristTable[piece][sqr] = hash;
            }
            return;
        }
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
                outputfile << std::to_string(piece) + " " + std::to_string(sqr) + " " + std::to_string(rn) + "\n";

                //log(rn);
            }
        }
        outputfile.close();
}

void init_zobrist(std::string filename){ //zobristvalues.txt
    //std::ifstream inputfile(filename);
    std::istream* file;
    if (!fileExists(filename)) {
        file = new std::istringstream(zobristvalues);

    }else{ // file exist
        
        file = new std::ifstream(filename);
        //std::ifstream file(filename);
    }
    
    std::string wholeline;
    while (std::getline(*file, wholeline)) { 
        int piece;
        int sqr;
        uint64_t hash;
        std::stringstream ss(wholeline);
        ss >> piece;
        ss >> sqr;
        ss >> hash;
        zobristTable[piece][sqr] = hash;
        //Myprintlog::log(std::to_string(piece)+ " "+ std::to_string(sqr)+ " "+ std::to_string(hash));
    }
    
    delete file;

    // if(!fileExists("openingmaps.txt")){
    //     generateOpeningsFile("openings.pgn", openings_table);
    // }

}

uint64_t get_board_hash(game_data& gd){
    uint64_t zobristHash = 0;

    //for (int sqr = 0; sqr < 64; sqr++) {
        //int piece = getPieceAtSquare(square);  // Get the piece at the current square (e.g., 0 for an empty square)
        //int side = getSideAtSquare(square);    // Get the side at the current square (e.g., 0 for white, 1 for black)

        // if (piece != INVALID) {  // If there is a piece on the square
        //     zobristHash ^= zobristTable[piece][square];
        // }
    //}
    for(int sqr=0 ; sqr<64; sqr++){
        if(gd.mailbox[sqr] != INVALID){
            zobristHash ^= zobristTable[gd.mailbox[sqr]][sqr];
        }
    }
    return zobristHash;
}



void generateOpeningsFile0(std::string filename, std::unordered_map<uint64_t, Variations>& openings_table){ //openings.pgn
    //reads pgn openings file, generates opening table in memory and saves it in a file with the format bellow:
    // "zobristHash -  candidate_size (2) - candidate_moves (e2e4 e2e3) - move_line (1.d3 Nf6) - eval - name

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error pgn file for reading!\n";
        return;
    }

    std::vector <std::string> stringvec;
    
    std::string pgngame = "";
    //std::string openingname = "";
    std::string line;

    while (std::getline(file,line)){// line = "1.e4 e5 2.Nf3"
        stringvec.push_back(line);
    }

    for(int i=0;i<stringvec.size(); i++) {
        //std::string wholeline = stringvec[i]; 

        if(stringvec[i][0] == '{' || stringvec[i][stringvec[i].length()] == '}'){
            //pgngame = "";
            //continue;
        }else if(stringvec[i][0] == '[' || stringvec[i][stringvec[i].length()] == ']'){
            
            if(!stringvec[i].empty() && (stringvec[i].back() == '\n' || stringvec[i].back() == '\r')){
                stringvec[i].pop_back();
            }
            //pgngame = "";
            //continue;
        }else if(stringvec[i][0] == '(' || stringvec[i][stringvec[i].length()] == ')'){
            //pgngame = "";
            //continue;
        }else{
            pgngame += stringvec[i] + " ";
            if(i < stringvec.size()-1){
                continue;
            }
        }
        if(pgngame == "" || pgngame == " "){ 
            //openingname += stringvec[i];
            continue;
        }

        Game game(STANDARD_POSITION, 1);
        std::string pgn_move;
        // Read the Zobrist hash
        std::stringstream ss(pgngame);
        std::string move_line = "S";
        int count = 0;
        while ( ss >> pgn_move && count <15) {
            if(pgn_move[pgn_move.length()-1] == '.'){
                std::string first = pgn_move;
                ss >> pgn_move;
                pgn_move = first + pgn_move;
            }else if(pgn_move[0] == '{' || pgn_move[pgn_move.length()] == '}'){
                continue;
            }else if(pgn_move[0] == '[' || pgn_move[pgn_move.length()] == ']'){
                continue;
            }else if(pgn_move[0] == '(' || pgn_move[pgn_move.length()] == ')'){
                continue;
            }else if(pgn_move.length()<2){
                std::string first = pgn_move;
                ss >> pgn_move;
                pgn_move = first + pgn_move;
            }
            game_data gd = game.get_gd();
            Myprintlog::log(pgn_move);
            std::string move = PGN_to_UCI(gd,pgn_move);
            Myprintlog::log(move);
            uint64_t zobrist_hash = get_board_hash(gd);
            auto pair = openings_table.find(zobrist_hash);

            game.move(move); // make the move

            if (pair != openings_table.end()){
            // search in openings table if zobrist exist if move is not included in candidate moves add moves

                if(pair->second.candidate_size < MAX_CANDIDATE){
                    bool found = false;
                    for(uint8_t i=0; i<pair->second.candidate_size; i++ ){
                        if(pair->second.candidate_moves[i] == move){
                            found = true;
                            break;
                        };
                    }
                    if(!found){
                        pair->second.candidate_moves[pair->second.candidate_size] = move;
                        pair->second.move_line = move_line; 
                        pair->second.candidate_size++;
                    }
                }
            }else{
            //  if the zobrist doesnt exit create zobrist / push move to candidate moves

                
                // Best_line bl(0 , 0 , 0);
                // int piece_values[6];
                // int position_values[6][64];
                // set_piece_values(piece_values , bl._greediness , bl._aggression, bl._naivity);
                // set_position_values(position_values , bl._greediness , bl._aggression, bl._naivity);
                // game_data current_gd = game.get_gd();
                
                // uint8_t last_move[8] = {0,0,0,0,0,0,0,0};
                //int eval = search(6, current_gd, bl , -1000000 , 1000000 , piece_values , position_values, last_move, true);
                
                Variations var(move,move_line);

                openings_table[zobrist_hash] = var;
            }
            if(move_line == "S"){move_line = "";}
            move_line += pgn_move + " ";
            count++;
        }

        //openingname = stringvec[i];
        pgngame = "";
    }
    
    file.close();

    // now read the opening tables in the memory and save it in a file called opening_maps

    std::ofstream opening_maps("openingmaps.txt");
    if (!opening_maps.is_open()) {
        std::cerr << "Error openingmaps file for reading!\n";
        return;
    }

    for (const auto& pair : openings_table) {
        const uint64_t hash = pair.first;
        const Variations& var = pair.second;
        uint8_t candidate_size = var.candidate_size; // 3
        const std::string* candidates = var.candidate_moves; 
        std::string move_line = var.move_line; // "e2e5 b8c6"
        int eval = var.eval; // 50
        std::string name = var.name; // name +',' + variation + ',' + sub-variations name + ...
 

        //opening_maps << hash << " -";
        //opening_maps << candidate_size << " - ";
        std::string cand = "";
        for (uint8_t i = 0; i < candidate_size; ++i) {
            cand += candidates[i]+" ";
            //opening_maps << candidates[i] << " ";
        }
        //opening_maps << "- " << move_line << " -";
        //opening_maps << eval << " - ";
        //opening_maps << name << "\n";

        std::string line = std::to_string(hash) +" - "+ std::to_string(candidate_size) +" - "+ cand +"- "+ move_line +"- "+ std::to_string(eval) +" - "+ name + "\n";
        //Myprintlog::log(line);
        opening_maps << line;
    }
    opening_maps.close();
}





void generateOpeningsFile(std::string filename, std::unordered_map<uint64_t, Variations>& openings_table){ //openings.pgn
    //reads pgn openings file, generates opening table in memory and saves it in a file with the format bellow:
    // "zobristHash -  candidate_size (2) - candidate_moves (e2e4 e2e3) - move_line (1.d3 Nf6) - eval - name

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error pgn file for reading!\n";
        return;
    }

    std::vector <std::string> stringvec;
    
    std::string pgngame = "";
    //std::string openingname = "";
    std::string line;

    while (std::getline(file,line)){// line = "1.e4 e5 2.Nf3"

        if(!line.empty() && line != "\n" && line != "\r"){
            stringvec.push_back(line);
        }
    }

    Game game(STANDARD_POSITION, 1);
    std::string move_line = "";

    for(int i=0;i<stringvec.size(); i++) {
        //std::string wholeline = stringvec[i]; 

        if( stringvec[i][0] == '{' || stringvec[i][0] == '[' || stringvec[i][0] == '('){
            if( stringvec[i] == "[Opening \"King's pawn Opening\"]\r" ){
                Myprintlog::log(stringvec[i]);
            }
            game = Game(STANDARD_POSITION, 1); // reset everything and read a new game
            pgngame = "";
            move_line = "";
            continue;
        }else if(stringvec[i].empty() || stringvec[i] == "\n" || stringvec[i] == "\r"){
            continue;
        }


        for(int j=i ; j<stringvec.size(); j++){
            
            if( stringvec[j] == "[Opening \"King's pawn Opening\"]\r" ){
                Myprintlog::log(stringvec[i]);
            }

            if(stringvec[j].empty() || stringvec[j] == "\n" || stringvec[j] == "\r"){
                continue;
            }

            if(stringvec[j][0] == '{' || stringvec[j][0] == '[' || stringvec[j][0] == '('){
                i = j - 1;
                break;
            }
            if(!stringvec[j].empty() && (stringvec[j].back() == '\n' || stringvec[j].back() == '\r')){
                stringvec[j].pop_back();
            }
            pgngame += stringvec[j] + " ";
        }
        if(pgngame == ""){
            continue;
        }

        std::stringstream ss(pgngame);
        std::string pgn_move;


        int count = 0;
        while ( ss >> pgn_move && count <40) {
            if(pgn_move[pgn_move.length()-1] == '.'){
                std::string first = pgn_move;
                ss >> pgn_move;
                pgn_move = first + pgn_move;
            }else if(pgn_move[0] == '{' || pgn_move[pgn_move.length()-1] == '}'){
                continue;
            }else if(pgn_move[0] == '[' || pgn_move[pgn_move.length()-1] == ']'){
                continue;
            }else if(pgn_move[0] == '(' || pgn_move[pgn_move.length()-1] == ')'){
                continue;
            }else if(pgn_move.length()<2){
                std::string first = pgn_move;
                ss >> pgn_move;
                pgn_move = first + pgn_move;
            }
            game_data gd = game.get_gd();
            Myprintlog::log(pgn_move);
            std::string move = PGN_to_UCI(gd,pgn_move);
            Myprintlog::log(move);
            uint64_t zobrist_hash = get_board_hash(gd);
            auto pair = openings_table.find(zobrist_hash);

            game.move(move); // make the move

            if (pair != openings_table.end()){
            // search in openings table if zobrist exist if move is not included in candidate moves add moves

                if(pair->second.candidate_size < MAX_CANDIDATE){
                    bool found = false;
                    for(uint8_t i=0; i<pair->second.candidate_size; i++ ){
                        if(pair->second.candidate_moves[i] == move){
                            found = true;
                            break;
                        };
                    }
                    if(!found){
                        pair->second.candidate_moves[pair->second.candidate_size] = move;
                        pair->second.move_line = move_line; 
                        pair->second.candidate_size++;
                    }
                }
            }else{
                
                Variations var(move,move_line);

                openings_table[zobrist_hash] = var;
            }

            move_line += pgn_move + " ";
            count++;
        }

        pgngame = "";
    }
    
    file.close();

    // now read the opening tables in the memory and save it in a file called opening_maps

    std::ofstream opening_maps("openingmaps2.txt");
    if (!opening_maps.is_open()) {
        std::cerr << "Error openingmaps file for reading!\n";
        return;
    }

    for (const auto& pair : openings_table) {
        const uint64_t hash = pair.first;
        const Variations& var = pair.second;
        uint8_t candidate_size = var.candidate_size; // 3
        const std::string* candidates = var.candidate_moves; 
        std::string move_line = var.move_line; // "e2e5 b8c6"
        int eval = var.eval; // 50
        std::string name = var.name; // name +',' + variation + ',' + sub-variations name + ...
 

        std::string cand = "";
        for (uint8_t i = 0; i < candidate_size; ++i) {
            cand += candidates[i]+" ";
        }

        std::string line = std::to_string(hash) +" - "+ std::to_string(candidate_size) +" - "+ cand +"- "+ move_line +"- "+ std::to_string(eval) +" - "+ name + "\n";
        //Myprintlog::log(line);
        opening_maps << line;
    }
    opening_maps.close();
}


// std::string PGN_arranger(std::string filename){
    
//     std::ifstream pgnfile(filename);
//     if (!pgnfile.is_open()){
//         std::cerr << "Error init_zobrist file for reading!\n";
//         return;
//     }

    
// }

void initOpeningsTableFromFile(std::string filename, std::unordered_map<uint64_t, Variations>& openings_table) {//openingmaps.txt
    //convert back the generated openings file into memory
    Myprintlog::log(filename);
    //std::ifstream file;
    std::istream* file;
    if (!fileExists(filename)) {
    //if (!file.is_open()) {
        //std::cerr << "Error OpeningsTable file for reading!\n";
        //std::istringstream file(openingmaps);
        file = new std::istringstream(openingmaps);
        //return;
    }else{
        //std::ifstream file(filename);
        file = new std::ifstream(filename);
    }

    std::string line;
    while (std::getline(*file, line)) { // "zobristHash -  candidate_size (2) - candidate_moves (e2e4 e2e3) - move_line (1.d3 Nf6) - eval - name
        std::stringstream ss(line);
        uint64_t zobristHash;
        Variations variation("");

        // Read the Zobrist hash
        if (!(ss >> zobristHash)) {
            break; // Error reading Zobrist hash
        }

        std::string hyphen;
        while (ss >> hyphen && hyphen != "-") {
            // Skip the hyphen
        }

        // Read candidate_size
        std::string token ;
        if (!(ss >> token)) {
            break; // Error reading candidate_size
        }
        variation.candidate_size = std::stoi(token);

        hyphen="";
        while (ss >> hyphen && hyphen != "-") {
            // Skip the hyphen
        }

        // Read candidate moves
        for (uint8_t i = 0; i < variation.candidate_size; ++i) {
            if (!(ss >> variation.candidate_moves[i])) {
                break; // No more candidate moves
            }
        }

        hyphen="";
        while (ss >> hyphen && hyphen != "-") {
            // Skip the hyphen
        }

        // Read the move line, eval, and name
        std::string move_line="";
        while (ss >> move_line){
            if(move_line == "-"){
                break;
            }
            variation.move_line += move_line + " ";
        }
        
        // ss >> variation.move_line;
        // ss.ignore(); // Skip the space after move_line

        ss >> variation.eval;
        
        hyphen = "";
        while (ss >> hyphen && hyphen != "-") {
            // Skip the hyphen
        }

        std::string name;
        while (ss >> name){
            variation.name += name;
        };

        openings_table[zobristHash] = variation;
    }
    delete file;
}

struct PN{
    std::string color = "black";
    std::string srcid = "";
    std::string trgid = "";
    std::string letter = "";
    int number = INVALID;
    std::string piece_name = "";
    char promotion = 'I';
    bool castle = false;
};


std::string PGN_to_UCI(game_data& gd,std::string& pgn_move){
    const std::string numbers = "12345678";
    const std::string letters = "hgfedcba";
    const std::string capitals = "KQRBN";

    std::string uci("");
    PN pn;

    int i=0;
    if(std::isdigit(pgn_move[0])){ // white
        pn.color = "white";
        for(int j=0;j<pgn_move.length();j++){
            if(pgn_move[j] == '.'){
               i = j+1;
               break;
            }
        }
    }
    //else{ // black
    //}
    std::size_t piece = capitals.find(pgn_move[i]);
    
    if(piece != std::string::npos){//R
        pn.piece_name = capitals[piece];
        if(letters.find(pgn_move[i+1]) != std::string::npos){ // Re
            if(letters.find(pgn_move[i+2]) != std::string::npos){//Rea
                pn.letter = pgn_move[i+1];
                //pn.trgid = pgn_move[i+2] + pgn_move[i+3];
                pn.trgid = std::string(1,pgn_move[i+2]) + std::string(1,pgn_move[i+3]);
            }else if( numbers.find(pgn_move[i+2]) != std::string::npos ){ //Re7
                //pn.trgid = pgn_move[i+1] + pgn_move[i+2];
                pn.trgid = std::string(1,pgn_move[i+1]) + std::string(1,pgn_move[i+2]);
            }else if(pgn_move[i+2] == 'X' || pgn_move[i+2] == 'x'){ //Rex
                //pn.trgid = pgn_move[i+3] + pgn_move[i+4];
                pn.letter = pgn_move[i+1];
                pn.trgid = std::string(1,pgn_move[i+3]) + std::string(1,pgn_move[i+4]);
            }
        }else if(numbers.find(pgn_move[i+1]) != std::string::npos ){ // R3
            pn.number = pgn_move[i+1] - '0' - 1;
            if(pgn_move[i+2] == 'X' || pgn_move[i+2] == 'x'){ // R3x
                //pn.trgid = pgn_move[i+3] + pgn_move[i+4]; // R3xe4
                pn.trgid = std::string(1,pgn_move[i+3]) + std::string(1,pgn_move[i+4]);
            }else if( letters.find(pgn_move[i+2]) != std::string::npos){//R3a
                //pn.trgid = pgn_move[i+2] + pgn_move[i+3];  //R3a3
                pn.trgid = std::string(1,pgn_move[i+2]) + std::string(1,pgn_move[i+3]);
            }
        }else if(pgn_move[i+1] == 'X' || pgn_move[i+1] == 'x'){//Rx
            //pn.trgid = pgn_move[i+2] + pgn_move[i+3];
            pn.trgid = std::string(1,pgn_move[i+2]) + std::string(1,pgn_move[i+3]);
        }

    }else if(letters.find(pgn_move[i]) != std::string::npos){ // e pawn
        pn.piece_name = "P";
        pn.letter = pgn_move[i];
        if(pgn_move[i+1] == 'X' || pgn_move[i+1] == 'x'){//ex
            if(pgn_move.length()>4 && pgn_move[i+4] == '='){ //exd8=
                //pn.trgid = pgn_move[i+2] + pgn_move[i+3];
                pn.trgid = std::string(1,pgn_move[i+2]) + std::string(1,pgn_move[i+3]);
                pn.promotion = std::tolower(pgn_move[i+5]);
            }else{//exd5
                //pn.trgid = pgn_move[i+2] + pgn_move[i+3];
                pn.trgid = std::string(1,pgn_move[i+2]) + std::string(1,pgn_move[i+3]);
            }
        }else if(numbers.find(pgn_move[i+1]) != std::string::npos){//e4
            if(pgn_move[i+2] == 'X' || pgn_move[i+2] == 'x'){//e4x
                if(pgn_move.length()>5 && pgn_move[i+5] == '='){//e7xd8=
                    //pn.number = pgn_move[i+1] - '0' -1;
                    //pn.trgid = pgn_move[i+3] + pgn_move[i+4];
                    pn.trgid = std::string(1,pgn_move[i+3]) + std::string(1,pgn_move[i+4]);
                    pn.promotion = std::tolower(pgn_move[i+6]);
                }else{
                    //pn.letter = pgn_move[i+1] - 'a' -1;
                    //pn.trgid = pgn_move[i+3] + pgn_move[i+4];
                    pn.trgid = std::string(1,pgn_move[i+3]) + std::string(1,pgn_move[i+4]);
                }
            }else if(pgn_move.length()>2 && pgn_move[i+2] == '='){//e8=
                //pn.trgid = pgn_move[i] + pgn_move[i+1];
                pn.trgid = std::string(1,pgn_move[i]) + std::string(1,pgn_move[i+1]);
                pn.promotion = std::tolower(pgn_move[i+3]);
            }else{
                //pn.trgid = pgn_move[i] + pgn_move[i+1];
                pn.trgid = std::string(1,pgn_move[i]) + std::string(1,pgn_move[i+1]);
            }
        }
    }else if(pgn_move[i] == 'O'){//castles
        pn.castle = true;
        pn.piece_name = "K";
        
        if(pgn_move[i+3] == '-' && pgn_move[i+4] == 'O'){//O-O-
            pn.trgid = (pn.color == "white" ? "c1" : "c8");
        }else if(pgn_move[i+2] == 'O'){// O-O
            pn.trgid = (pn.color == "white" ? "g1" : "g8");
        }
    }
    
    uint64_t src_bitboard = gd.bitboards[pn.color == "white" ? str_pieces.find(pn.piece_name) : str_pieces.find(pn.piece_name) + 6 ];
    int sqr = INVALID;
    uint64_t bitmap;
    int src = INVALID;
    for(int trg = 0 ; trg < 64; trg++){
        if(notations[trg] == pn.trgid){
            sqr = trg;
            break;
        }
    }
        
    if(pn.piece_name == "P"){
        bitmap = src_bitboard; 
        //if(wpa){ bitmap = wpa;}

    }else if(pn.piece_name == "K" ){

        bitmap = src_bitboard;

    }else if( pn.piece_name == "N"){
        bitmap = raw_attacks[4][sqr] & src_bitboard;
        //if(wna){ bitmap = wna;}

    }else if(pn.piece_name == "B"){
        bitmap = get_sliding_attacks(0,sqr,gd.occupancy[2])& src_bitboard;
        //if(bishand){ bitmap = bishand;}
    }else if(pn.piece_name == "R"){
        bitmap = get_sliding_attacks(1,sqr,gd.occupancy[2])& src_bitboard;
        //if(rookand){ bitmap = rookand;}
    }else if(pn.piece_name == "Q"){
        bitmap = get_queen_attacks(sqr, gd.occupancy[2])& src_bitboard;
        //if(queenand){ bitmap = queenand;}
    }
       
    if(pn.letter == "" && pn.number == INVALID){
        pn.srcid = notations[get_first_square(bitmap)];

    }else if (pn.number != INVALID){ // number row
        while(bitmap){
            int s = get_first_square(bitmap);
            if((int)s/8 == pn.number){
                pn.srcid = notations[s];
                break;
            }
            DEL_BIT(bitmap,s);
        }
    }else{ // letter column
        if(pn.piece_name == "P"){
            ArrayMoveList ml;
            ml.generate(gd);
            bool found = false;
            for(int i=0;i< ml._first_size;i++){
                if(notations[ml.first_move_list[i][1]] == pn.trgid && ml.first_move_list[i][2] == (pn.color == "white" ? 5 : 11) && ml.first_move_list[i][0]%8 == letters.find(pn.letter)){
                    pn.srcid = notations[ml.first_move_list[i][0]];
                    found = true;
                    break;
                }
            }
            if(!found){
                for(int i=0;i< ml._second_size;i++){
                    if(notations[ml.second_move_list[i][1]] == pn.trgid && ml.second_move_list[i][2] == (pn.color == "white" ? 5 : 11) && ml.second_move_list[i][0]%8 == letters.find(pn.letter)){
                        pn.srcid = notations[ml.second_move_list[i][0]];
                        break;
                    }
                }
            }
        }else{
            while(bitmap){
                int s = get_first_square(bitmap);
                if((int)s%8 == letters.find(pn.letter)){
                    pn.srcid = notations[s];
                    break;
                }
                DEL_BIT(bitmap,s);
            }
        }
    }

    uci = pn.srcid + pn.trgid;
    if(pn.promotion != 'I'){uci = uci + "=" + pn.promotion;}
    if(pn.castle){uci = uci + '*';}
    return uci;
};