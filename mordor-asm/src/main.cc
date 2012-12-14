#include <stdio.h>

#include <vector>

#include "main.h"

using namespace std;


namespace mordor {
namespace assembler {

void start (int argc, char** argv) {
    vector<Token*> tokens;

    for (int i = 2; i < argc; ++i) {
        TokenizeFile (argv[i], tokens);
    }

    Compile (argv[1], tokens);
}

}
}


int main (int argc, char** argv) {
    if (argc <= 2) {
        printf ("Output file and at least one input file not specified!\nUsage: mordor-asm <output> <input> <input> ...");
    }

    mordor::assembler::start (argc, argv);

    return 0;
}