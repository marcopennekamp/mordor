#include <coin/utils/Stream.h>

#include <mordor/runtime/loader.h>

#include <internal/runtime/Function.h>

using namespace coin;
using namespace mordor;


FunctionInterface* LoadFunction (StreamInterface* stream_interface) {
    Stream* stream = (Stream*) stream_interface;

    /* Create Function and read stack size. */
    Function* function = new Function ();
    stream->Read ((u8*) &function->stack_size, sizeof (function->stack_size));

    /* Read function ID. */
    // stream->Read ((u8*) &function->id, sizeof (function->id));

    /* Get length of operation data block. */
    mordor_u64 length = stream->Size () - stream->Position ();

    /* Read operations. */
    function->operations = new Operation [(u32) length / sizeof (Operation)];
    stream->Read ((u8*) function->operations, (u32) length);

    return (FunctionInterface*) function;
}
