PUBLIC Enable_VMX_Operation

.code _text

Enable_VMX_Operation PROC PUBLIC
push rax; Save the Store

xor rax,rax ; Clear the RAX
mov rax,cr4
or rax,02000h
mov cr4,rax

pop rax ;
ret
Enable_VMX_Operation ENDP

END
