#include <mordor/api/Program.h>
#include <mordor/runtime/Program.h>

using namespace mdr;


extern "C" {

mdrProgram* mdrProgCreate () {
    return (mdrProgram*) new Program ();
}

void mdrProgDestroy (mdrProgram* prog) {
    delete (Program*) prog;
}

}