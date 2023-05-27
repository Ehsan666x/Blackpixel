
#include <iostream>
#include "../include/game.h"
#include "../include/structs.h"


Out inout(std::string& input, int depth){

    Game game(input,depth);
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

