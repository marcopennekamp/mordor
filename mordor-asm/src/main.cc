#include <stdio.h>

#include <vector>

#include "main.h"

using namespace std;


namespace mdr {
namespace assembler {

void start (int argc, char** argv) {
    vector<Token*> tokens;

    for (int i = 2; i < argc; ++i) {
        TokenizeFile (argv[i], tokens);
    }

    /* Came into handy again. */
    
    for (int i = 0; i < tokens.size (); ++i) {
        Token& token = *tokens[i];

        switch (token.tag) {
          case TOKEN_STRING:
            printf ("String: %s\n", token.string);
            break;
          case TOKEN_LITERAL:
            printf ("Literal: %s\n", token.string);
            break;
          case TOKEN_CHARACTER:
            printf ("Character: %c\n", token.character);
            break;
          case TOKEN_CONSTANT:
            printf ("Constant: %u, %llX\n", token.constant.type, token.constant.value._u64);
            break;
        }
    }
    
    fflush (stdout);

    Compile (argv[1], tokens);
}

}
}


int main (int argc, char** argv) {
    if (argc <= 2) {
        printf ("Output file and at least one input file not specified!\nUsage: mordor-asm <output> <input> <input> ...");
    }

    mdr::assembler::start (argc, argv);

    return 0;
}