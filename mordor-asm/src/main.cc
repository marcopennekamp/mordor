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
    /*
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
          case TOKEN_UINT_32:
            printf ("U32: %u\n", token.nUInt32);
            break;
          case TOKEN_UINT_64:
            printf ("U64: %llu\n", token.nUInt64);
            break;
          case TOKEN_INT_32:
            printf ("I32: %i\n", token.nInt32);
            break;
          case TOKEN_INT_64:
            printf ("I64: %lli\n", token.nInt64);
            break;
          case TOKEN_FLOAT_32:
            printf ("F32: %f\n", token.nFloat32);
            break;
          case TOKEN_FLOAT_64:
            printf ("F64: %llf\n", token.nFloat64);
            break;
        }
    }
    */

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