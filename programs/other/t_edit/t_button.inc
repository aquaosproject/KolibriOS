struct symbol
	c db ?    ; +0 ᨬ���
	col db ?  ; +1 梥�
	perv dd ? ; +2
	next dd ? ; +6 㪠��⥫�
	tc dd ?   ;+10 �६. ᮧ�����
	td dd ?   ;+14 �६. 㤠�����
ends

bmp_icon dd 0


;input:
; al = ��� �訡��
align 4
ted_on_init_synt_err:
	mov byte[msgbox_3.err],'0'
	add byte[msgbox_3.err],al
	stdcall [mb_create],msgbox_3,thread ;message: Can-t open color options file!
	ret


align 4
ted_but_new_file:
	push eax ebx
	stdcall [ted_can_save], tedit0
	cmp al,1
	jne @f
		stdcall [mb_create],msgbox_2,thread ;message: save changes in file?
		stdcall [mb_setfunctions],msgbox_2N_funct
		jmp .ret_f
	@@:
	call On_NewFile
	.ret_f:
		mov byte[openfile_path],0
		mcall SF_SET_CAPTION,1,hed
	pop ebx eax
	ret

align 4
On_NewFile:
	stdcall [ted_clear], tedit0,1
	call draw_but_toolbar
	stdcall [ted_draw], tedit0
	ret

align 4
On_SaveAndNewFile:
	push edi
	mov edi, tedit0

	call ted_but_save_file
	cmp ted_err_save,0
	jne @f
		call On_NewFile
	@@:
	pop edi
	ret

align 4
On_SaveAndOpenFile:
	push edi
	mov edi, tedit0

	call ted_but_save_file
	cmp ted_err_save,0
	jne @f
		call ted_but_open_file.no_msg
	@@:
	pop edi
	ret

align 4
On_SaveAndExit:
	push edi
	mov edi, tedit0

	call ted_but_save_file
	cmp ted_err_save,0
	jne @f
		mov dword[exit_code],1
	@@:
	pop edi
	ret

align 4
On_Exit:
	mov dword[exit_code],1
	ret

;description:
; �㭪�� ������ 䠩�� ��� ���� ᮮ�饭��
align 4
but_no_msg_OpenFile:
	push eax ebx
	stdcall [ted_open_file], tedit0,run_file_70,openfile_path
	call ted_messages_after_open_file
	pop ebx eax
	ret

;description:
; �㭪�� ������ 䠩�� � �������� ����� ᮮ�饭�� (� ���������� �⬥���� ����⨥)
align 4
ted_but_open_file:
	pushad

	stdcall [ted_can_save], tedit0
	cmp al,1
	jne @f
		stdcall [mb_create],msgbox_2,thread ;message: save changes in file?
		stdcall [mb_setfunctions],msgbox_2O_funct
		jmp .ret_f
	.no_msg: ;��⪠ ��� �맮�� �㭪樨 �१ ������ � ᮮ�饭�� msgbox_5
		pushad
	@@:

	copy_path open_dialog_name,communication_area_default_path,file_name,0
	mov [OpenDialog_data.type],0
	stdcall [OpenDialog_Start],OpenDialog_data
	cmp [OpenDialog_data.status],2
	jne @f
		stdcall mem_spac, msgbox_9.fdp,100
		mov esi,file_name
		call strlen
		cmp eax,100
		jle .no_crop
			mov eax,100
		.no_crop:
		stdcall mem_copy, esi,msgbox_9.fdp,eax
		stdcall [mb_create],msgbox_9,thread ;message: error run 'open file dialog'
		jmp .ret_f
	@@:
	cmp [OpenDialog_data.status],1
	jne .ret_f
		stdcall auto_open_syntax,[OpenDialog_data.openfile_path]
		stdcall [ted_open_file], tedit0,run_file_70,openfile_path
		call ted_messages_after_open_file
	.ret_f:
	popad
	ret

proc auto_open_syntax, of_path:dword
pushad
	;�饬 ���� �� 䠩� ���ᢥ⪨ ��� ���뢠����� ⥪�⮢��� 䠩��
	mov esi,[of_path]
	call strlen
	mov edx,[of_path]
	add edx,eax
	.f_beg:
	cmp byte[edx],'.'
	je .found
		dec edx
		cmp edx,[of_path]
		jg .f_beg
	.found:
	inc edx ;edx - 㪠��⥫� �� ���७�� ����⮣� 䠩��

	mov ebx,synt_auto_open
	.cycle_0:
		add ebx,32
		stdcall strcmp,ebx,edx ;�ࠢ������ ���७�� ����⮣� 䠩�� � ���祭�ﬨ �����묨 � ini 䠩��
		test eax,eax
		jz .ok
		add ebx,32
		cmp byte[ebx],0
		jne .cycle_0
	jmp .end_0
	.ok: ;����⨥ 䠩�� ���ᢥ⪨ � ����ᨬ��� �� ���������� ���७��
		sub ebx,32
		stdcall open_unpac_synt_file,ebx
	.end_0:
popad
	ret
endp

;description:
; �㭪�� ��࠭���� 䠩�� � �맮��� ���� �������
align 4
proc ted_but_save_file
	;*** �맮� ����������� ���� ��� ��࠭���� 䠩��
	copy_path open_dialog_name,communication_area_default_path,file_name,0
	mov [OpenDialog_data.type],1
	stdcall [OpenDialog_Start],OpenDialog_data
	cmp [OpenDialog_data.status],1 ;if status==1 then save
	jne .end_save_file
		stdcall [ted_save_file],tedit0,run_file_70,openfile_path
	.end_save_file:
	ret
endp

;description:
; ����஢���� �����
align 4
proc mem_copy uses ecx esi edi, source:dword, destination:dword, len:dword
	cld
	mov esi, dword[source]
	mov edi, dword[destination]
	mov ecx, dword[len]
	rep movsb
	ret
endp

;description:
; ���������� ����� �஡�����
align 4
proc mem_spac uses eax ecx edi, mem:dword, len:dword
	cld
	mov al,' '
	mov edi, dword[mem]
	mov ecx, dword[len]
	repne stosb
	ret
endp

;input:
; eax = ��� �訡��
; ebx = �������⢮ ���⠭��� ����
align 4
ted_messages_after_open_file:
	push ecx edi
	cmp eax,0
	je @f
	cmp eax,6
	je @f
		cmp ax,10
		jl .zifra_0_9
			mov al,'?'
			sub ax,48
		.zifra_0_9:
		add ax,48

		mov byte[msgbox_4.err],al
		stdcall [mb_create],msgbox_4,thread ;message: Can-t open text file!
		jmp .ret_f
	@@:
	cmp ebx,-1
	je .ret_f
		;if open file
		mov edi,tedit0
		mov ecx,ted_max_chars
		sub ecx,2 ;ecx = ���ᨬ��쭮� �᫮ ����, ��� ������ �뫠 �뤥���� ������
		cmp ebx,ecx
		jl .ret_f
			stdcall [mb_create],msgbox_1,thread
	.ret_f:
	pop edi ecx
	ret

align 4
ted_save_err_msg:
	mov byte[msgbox_6.err],al
	stdcall [mb_create],msgbox_6,thread ;message: Can-t save text file!
	ret

;description:
; �㭪�� ������ �� ������ [�ਬ�����] � ������ ᨭ⠪��
align 4
proc ted_but_open_syntax uses eax ebx ecx edi, edit:dword
	mov edi,dword[edit]
	cmp ted_panel_id,TED_PANEL_SYNTAX
	jne @f
		stdcall dword[tl_node_get_data], tree1
		mov [fn_col_option],eax
		stdcall open_unpac_synt_file,eax
		cmp ebx,-1
		je @f

		call [ted_text_colored]
		stdcall [ted_draw],edi
	@@:
	ret
endp

;����⨥ � �ᯠ����� 䠩�� ���ᢥ⪨ ᨭ⠪��
;input:
; f_name - 㪠��⥫� �� ��� ���뢠����� 䠩�� ᨭ⠪�� (��� ��� � ����� info)
;output:
; ebx - �᫮ ���⠭��� ���� �� 䠩��
; ecx - ࠧ��蠥���
align 4
proc open_unpac_synt_file uses eax edi esi, f_name:dword
	mov ebx,[f_name]
	mov edi,last_open_synt_file
	stdcall strcmp,ebx,edi ;�ࠢ������ ��� ������祭���� 䠩�� � ������祭�� ࠭�� 䠩���
	test eax,eax
	jz @f

	;�����㥬 ��� ������祭���� 䠩�� (��� ��������� ������� ������祭��)
	mov esi,ebx
	mov ecx,32/4
	cld
	rep movsd

	copy_path ebx,fn_syntax_dir,syntax_path,0
	copy_path syntax_path,sys_path,file_name,0
	mov edi, tedit0
	mov [run_file_70.Function], SSF_READ_FILE
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov ecx, ted_syntax_file_size
	mov dword[run_file_70.Count], ecx
	m2m dword[run_file_70.Buffer], ted_syntax_file
	mov byte[run_file_70+20], 0
	mov [run_file_70.FileName], file_name
	mcall SF_FILE, run_file_70
	cmp ebx,-1
	jne .end_0
		call ted_on_init_synt_err
		jmp @f
	.end_0:
		mov eax,ted_syntax_file
		cmp dword[eax],'KPCK'
		jne .end_unpack
		cmp dword[eax+4],ecx
		jg @f ;�᫨ ��� �ᯠ�������� 䠩�� �� 墠⠥� �뤥������ �����
		cmp dword[unpac_mem],0
		jne .end_1
			;�뤥����� ����� ��� �ᯠ����� 䠩��
			stdcall mem.Alloc,ecx
			mov [unpac_mem],eax
		.end_1:
		stdcall unpack,ted_syntax_file,[unpac_mem]
		mov edi,ted_syntax_file
		mov esi,[unpac_mem]
		mov ecx,[edi+4]
		cld
		rep movsb
		.end_unpack:
		stdcall [ted_init_syntax_file], tedit0
	@@:
	ret
endp

;description:
; �맮�/���⨥ ������ ���᪠
align 4
proc ted_but_find uses edi
	mov edi,tedit0

	cmp ted_panel_id,TED_PANEL_NULL
	je @f
		mov ted_panel_id,TED_PANEL_NULL
		mov ted_wnd_l,0
		jmp .e_if
	@@:
		mov ted_panel_id,TED_PANEL_FIND
		mov ted_wnd_l,TED_PANEL_WIDTH
	.e_if:
	call EvSize
	stdcall [ted_draw],edi
	ret
endp

align 4
tbl_1251_866:
rb 128
db   0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0 ;128
db   0,  0,  0,  0,  0,  0,  0,  0,  0,0,0,0,0,0,0,0 ;144
db   0,  0,  0,  0,  0,  0,  0,  0,240,0,0,0,0,0,0,0 ;160
db   0,  0,  0,  0,  0,  0,  0,  0,241,0,0,0,0,0,0,0 ;176
db 128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143 ;192
db 144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159 ;208
db 160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175 ;224
db 224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239 ;240

align 4
tbl_866_1251:
rb 128
db 192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207 ;128
db 208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223 ;144
db 224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239 ;160
db   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ;176
db   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ;192
db   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ;208
db 240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255 ;224
db 168,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ;240

align 4
ted_on_find_err:
	stdcall [mb_create],msgbox_7,thread ;message: Can not find text
	ret

;description:
; ������ [��������] �� ������
align 4
but_replace:
	; ���� �� ᤥ����
	ret

;description:
; �㭪�� ���᪠ ���ଠ樨 � ᯨ᪥ ���祢�� ᫮�
align 4
but_find_key_w:
	cmp byte[wnd_k_words_run],0
	jne @f
		pushad
		mcall SF_CREATE_THREAD,1,prop_start,thread_coords
		popad
	@@:
	ret

;description:
; �뤥�����/���⨥ �����⠥��� ᨬ�����
align 4
proc but_sumb_invis uses edi, edit:dword
	mov edi,dword[edit]

	xor ted_mode_invis,1
	call draw_but_toolbar
	stdcall [ted_draw],edi
	ret
endp

;description:
; �뤥�����/���⨥ ���祢�� ᫮�
align 4
proc but_k_words_show uses edi, edit:dword
	mov edi,dword[edit]

	xor ted_mode_color,1
	cmp ted_mode_color,0
	je @f
		call [ted_text_colored]
	@@:
	call draw_but_toolbar
	stdcall [ted_draw],edi
	ret
endp

;description:
; �맮�/���⨥ ������ � 䠩���� ᨭ⠪��
align 4
proc but_synt_show uses edi, edit:dword
	mov edi,[edit]

	cmp ted_panel_id,TED_PANEL_NULL
	je @f
		mov ted_panel_id,TED_PANEL_NULL
		mov ted_wnd_l,0
		jmp .e_if
	@@:
		mov ted_panel_id,TED_PANEL_SYNTAX
		mov ted_wnd_l,TED_PANEL_WIDTH
	.e_if:
	call EvSize
	stdcall [ted_draw],edi
	ret
endp
