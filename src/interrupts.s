[GLOBAL load_idt]
[GLOBAL isr_stub_table]
[EXTERN isr_dispatch]

load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    ret

; Exceções que a CPU empilha com código de erro: 8, 10-14, 17, 21, 29, 30.
; Nas outras empilhamos um 0 pra deixar o frame sempre com o mesmo formato.
%macro ISR_NOERR 1
isr%1:
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

%macro ISR_ERR 1
isr%1:
    push dword %1
    jmp isr_common
%endmacro

%assign i 0
%rep 48
    %if i == 8 || (i >= 10 && i <= 14) || i == 17 || i == 21 || i == 29 || i == 30
        ISR_ERR i
    %else
        ISR_NOERR i
    %endif
    %assign i i+1
%endrep

; Frame final (do topo pra base): ds, pusha, int_no, err_code, eip, cs, eflags
isr_common:
    pusha
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    cld
    push esp
    call isr_dispatch
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8
    iret

section .data
isr_stub_table:
%assign i 0
%rep 48
    dd isr %+ i
    %assign i i+1
%endrep
