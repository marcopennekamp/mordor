.code

public CallNativeFunctionVoid
public CallNativeFunctionU32
public CallNativeFunctionU64
public CallNativeFunctionF32    ; Used to tell the C++ compiler that the value is returned in the xmm0 register.
public CallNativeFunctionF64

CallNativeFunctionF32 proc
    jmp     CallNativeFunctionU64
CallNativeFunctionF32 endp
 
CallNativeFunctionF64 proc
    jmp     CallNativeFunctionU64
CallNativeFunctionF64 endp

CallNativeFunctionU32 proc
    jmp     CallNativeFunctionU64
CallNativeFunctionU32 endp

CallNativeFunctionVoid proc
    jmp    QWORD PTR rcx       ; Call function directly.
CallNativeFunctionVoid endp


; mdr_u64 (Library::func, mdr_u64* register_stack, mdr_u32 stack_size, mdr_u64* stack)
CallNativeFunctionU64 proc
    ; Preserve r12
    push    r12

    ; r10 function, r11 register_stack, r12 stack_size
    mov     r10, rcx
    mov     r11, rdx
    mov     r12, r8     ; Move to non-volatile register.

    ; Push arguments 4 to n on stack.
LLoop:
    test    r8, r8          
    jz      LSetArgs                ; Jump to loop end when size == 0.

    push    QWORD PTR [r9]          ; Push next value on stack.

    add     r9, 8                   ; Increment stack pointer.
    dec     r8                      ; Decrement stack size.
    jmp     LLoop

LSetArgs:
    ; "Brute force" argument registers.
    movsd   xmm3, QWORD PTR [r11 + 24]
    movsd   xmm2, QWORD PTR [r11 + 16]
    movsd   xmm1, QWORD PTR [r11 + 8]
    movsd   xmm0, QWORD PTR [r11]
    mov     r9, [r11 + 24]
    mov     r8, [r11 + 16]
    mov     rdx, [r11 + 8]
    mov     rcx, [r11]

LCall:
    ; Reserve shadow space.
    sub     rsp, 32

    call    QWORD PTR r10           ; Call function.

    ; Clean stack.
    lea     rsp, QWORD PTR [rsp + r12 * 8 + 32]   ; Pushed arguments plus shadow stack.

    ; Pop r12
    pop     r12

    ret     0
CallNativeFunctionU64 endp


end
