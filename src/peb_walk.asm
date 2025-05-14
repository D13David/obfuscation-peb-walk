.code 

;------------------------------------------
; GetPEB - Retrieves the address of the PEB (Process Environment Block)
;------------------------------------------
GetPEB proc

ifdef _WIN64
    mov rax, gs:[60h]
else
    mov eax, fs:[30h]
endif

    ret

GetPEB endp

end