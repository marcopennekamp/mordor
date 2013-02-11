#ifndef MORDOR_ASSEMBLER_MAIN_H_
#define MORDOR_ASSEMBLER_MAIN_H_

#include <vector>

#include "Token.h"


namespace mdr {
namespace assembler {

void TokenizeFile (const char* file_path, std::vector<Token*>& tokens);
void Compile (const char* target_path, std::vector<Token*>& tokens);

}
}

#endif  /* MORDOR_ASSEMBLER_MAIN_H_ */