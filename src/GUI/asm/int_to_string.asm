section .text
global int_to_string_asm

int_to_string_asm:
    push    rbx 
    push    rdi

    mov     rdi, rdx ;rdi = buffer
    mov     eax, ecx ;eax = int

    test    eax, eax ;if(value == 0)
    jnz     .not_zero
    mov     byte [rdi], '0'
    mov     byte [rdi+1], 0
    mov     eax, 1
    jmp     .fin

.not_zero:
    xor     rbx, rbx ;rbx = cantidad de digitos escritos
    mov     r8, rdi  ;r8 = puntero 

.divide_loop:
    xor     edx, edx ;edx:eax = dividendo, edx=0 
    mov     ecx, 10
    idiv    ecx ;eax = eax/10, edx = eax%10
    add     edx, '0'
    mov     [r8], dl
    inc     r8
    inc     rbx
    test    eax, eax          
    jnz     .divide_loop

    mov     byte [r8], 0         
    mov     rax, rbx          



    ;Hay que invertir los digitos para que queden en el orden que es
    mov     rsi, rdi
    lea     rcx, [rdi + rbx - 1]

.reverse_loop:
    cmp     rsi, rcx
    jge     .fin
    mov     dl, [rsi]
    mov     r9b, [rcx]
    mov     [rsi], r9b
    mov     [rcx], dl
    inc     rsi
    dec     rcx
    jmp     .reverse_loop

.fin:
    pop     rdi
    pop     rbx
    ret