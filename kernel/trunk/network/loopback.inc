;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                 ;;
;; Copyright (C) KolibriOS team 2004-2010. All rights reserved.    ;;
;; Distributed under terms of the GNU General Public License       ;;
;;                                                                 ;;
;;  loopback.inc                                                   ;;
;;                                                                 ;;
;;  LoopBack device for KolibriOS                                  ;;
;;                                                                 ;;
;;    Written by hidnplayr@kolibrios.org                           ;;
;;                                                                 ;;
;;          GNU GENERAL PUBLIC LICENSE                             ;;
;;             Version 2, June 1991                                ;;
;;                                                                 ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

$Revision: 2891 $

iglobal

LOOPBACK_DEVICE:

        .device_type    dd NET_DEVICE_LOOPBACK
        .mtu            dd 4096
        .name           dd .namestr

        .unload         dd .dummy_fn
        .reset          dd .dummy_fn
        .transmit       dd LOOP_input

        .bytes_tx       dq 0
        .bytes_rx       dq 0
        .packets_tx     dd 0
        .packets_rx     dd 0

        .link_state     dd -1
        .hwacc          dd 0

        .namestr        db 'loopback', 0

        .dummy_fn:
                ret

endg


macro   LOOP_init {
local   .fail

        mov     ebx, LOOPBACK_DEVICE
        call    NET_add_device

        cmp     eax, -1
        je      .fail

        mov     [IP_LIST], 127 + 1 shl 24
        mov     [SUBNET_LIST], 255
        mov     [BROADCAST_LIST], 0xffffff00 + 127

  .fail:
}

;-----------------------------------------------------------------
;
; LOOP_input
;
;  IN:  [esp+4] = Pointer to buffer
;       [esp+8] = size of buffer
;
;  OUT: /
;
;-----------------------------------------------------------------
align 4
LOOP_input:
        pop     ebx
        pop     eax
        pop     ecx

        push    ebx
        push    ecx
        push    eax

        DEBUGF  DEBUG_NETWORK_VERBOSE, "LOOP_input: size=%u\n", ecx
        lea     edx, [eax + 4]
        mov     eax, dword[eax]
        mov     ebx, LOOPBACK_DEVICE

        cmp     eax, AF_INET4
        je      IPv4_input

        DEBUGF  DEBUG_NETWORK_VERBOSE, "LOOP_input: Unknown packet type=%x\n", ax

  .dump:
        DEBUGF  DEBUG_NETWORK_VERBOSE, "LOOP_input: dumping\n"
        call    kernel_free
        add     esp, 4
        ret

;-----------------------------------------------------------------
;
; LOOP_output
;
; IN:
;     ecx = packet size
;      di = protocol
;
; OUT: edi = 0 on error, pointer to buffer otherwise
;      eax = buffer start
;      ebx = to device structure
;      ecx = unchanged (packet size of embedded data)
;      edx = size of complete buffer
;
;-----------------------------------------------------------------
align 4
LOOP_output:

        DEBUGF  DEBUG_NETWORK_VERBOSE, "LOOP_output\n"

        push    ecx
        push    edi

        add     ecx, 4
        cmp     ecx, [LOOPBACK_DEVICE.mtu]
        ja      .out_of_ram
        stdcall kernel_alloc, ecx
        test    eax, eax
        jz      .out_of_ram
        mov     edi, eax
        pop     eax
        stosd

        lea     eax, [edi - 4]  ; Set eax to buffer start
        pop     ecx
        lea     edx, [ecx + 4]  ; Set edx to complete buffer size
        mov     ebx, LOOPBACK_DEVICE

  .done:
        DEBUGF  DEBUG_NETWORK_VERBOSE, "LOOP_output: ptr=%x size=%u\n", eax, edx
        ret

  .out_of_ram:
        DEBUGF  DEBUG_NETWORK_VERBOSE, "LOOP_output: failed\n"
        add     esp, 4+4
        xor     edi, edi
        ret

