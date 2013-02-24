#include <mdr/def/Mdr.h>

#include <stdio.h>

struct T {
    mdr_u64 a;
    mdr_u64 b;
};

extern "C" {
   
MDR_FFI_DECL void MDR_FFI_CALL say () {
    printf ("Hello World!\n");
}

MDR_FFI_DECL mdr_u64 MDR_FFI_CALL power (mdr_u64 a) {
    return a * a;
}

MDR_FFI_DECL mdr_u64 MDR_FFI_CALL negate (mdr_u64 a) {
    return ~a;
}

MDR_FFI_DECL mdr_f32 MDR_FFI_CALL add (mdr_f32 a, mdr_f32 b, mdr_f32 c, mdr_f32 d, mdr_f32 e, mdr_f32 f) {
    return a + b + c + d + e + f;
}

MDR_FFI_DECL T* MDR_FFI_CALL ret_t (mdr_u64 a, mdr_u64 b) {
    return new T ();
}

MDR_FFI_DECL mdr_u64 MDR_FFI_CALL test (mdr_u64 r) {
    mdr_u64 sum = ret_t (r, ~r)->a;
    sum += ret_t (r, ~r)->a;
    return sum;
}

MDR_FFI_DECL mdr_u64 MDR_FFI_CALL immediate_return () {
    mdr_u64 a = (mdr_u64) add (0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    mdr_u64 b = 100;
    mdr_u64 c = 100;
    mdr_u64 d = 100;
    mdr_u64 e = a + b + c + d;
    return test (e);
}

}