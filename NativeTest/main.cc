#include <mdr/def/Mdr.h>

#include <stdio.h>

struct T {
    mdr_u64 a;
    mdr_u64 b;
};

extern "C" {
    
MDRI_DECL void MDRI_CALL say () {
    printf ("Hello World!\n");
}

MDRI_DECL mdr_u32 MDRI_CALL power (mdr_u32 a) {
    mdr_u32 result = a * a;
    printf ("%lli\n", result);
    return result;
}

MDRI_DECL mdr_u64 MDRI_CALL negate (mdr_u64 a) {
    return ~a;
}

MDRI_DECL mdr_f32 MDRI_CALL add (mdr_f32 a, mdr_f32 b, mdr_f32 c, mdr_f32 d, mdr_f32 e, mdr_f32 f) {
    return a + b + c + d + e + f;
}

MDRI_DECL T* MDRI_CALL ret_t (mdr_u64 a, mdr_u64 b) {
    return new T ();
}

MDRI_DECL mdr_u64 MDRI_CALL test (mdr_u64 r) {
    mdr_u64 sum = ret_t (r, ~r)->a;
    sum += ret_t (r, ~r)->a;
    return sum;
}

MDRI_DECL mdr_u64 MDRI_CALL immediate_return () {
    mdr_u64 a = (mdr_u64) add (0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
    mdr_u64 b = 100;
    mdr_u64 c = 100;
    mdr_u64 d = 100;
    mdr_u64 e = a + b + c + d;
    return test (e);
}

}