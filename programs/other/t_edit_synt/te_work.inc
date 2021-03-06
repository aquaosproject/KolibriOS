struct FileInfoBlock
	Function dd ?
	Position dd ?
	Flags	 dd ?
	Count	 dd ?
	Buffer	 dd ?
		db ?
	FileName dd ?
ends

struct TexColViv
	Text  rb MAX_COLOR_WORD_LEN ; ����� ��� ���������
	f1    dd 0 ; ������� �� �����
	flags db ? ; f1+4 ����� ������������ ��� ���������
	endc  db ? ; f1+5 ������ ����� ��������� (������������ ��� flags&4)
	escc  db ? ; f1+6 ������������ ������ (������������ ��� flags&4)
	color db ? ; f1+7 ����� �����
ends

count_colors_text dd 1
ColWords dd 0
color_cursor dd ?
color_wnd_capt dd ?
color_wnd_work dd ?
color_wnd_bord dd ?
color_select dd ?
color_cur_text dd ?
color_wnd_text dd ?

options_file dd 0
options_file_end dd 0
tex_colors dd 0
tex_help_f1 dd 0

Col dd 0 ;��������� �� ��������� �������� ���� TexColViv

keyUpr dd 0
err_opn db 0 ;������ �������� �����
err_sav db 0 ;������ ���������� �����

sc system_colors

run_file_70 FileInfoBlock

align 4
InitColText:
	pushad

	mov  ebx,dword[fn_col_option]
	copy_path ebx,fn_syntax_dir,file_name_rez,0
	copy_path file_name_rez,sys_path,file_name,0

	mov eax,70
	mov [run_file_70.Function], 0
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov ebx,[options_file_end]
	sub ebx,[options_file]
	mov [run_file_70.Count], ebx
	m2m [run_file_70.Buffer], [options_file]
	mov byte[run_file_70+20], 0
	mov [run_file_70.FileName], file_name
	mov ebx,run_file_70
	int 0x40


	;init: count_colors_text, ColWords, ...
	mov dword[count_colors_text],1
	mov dword[ColWords],0
	mov dword[tex_help_f1],0

	cmp eax,6
	je @f
	cmp eax,0
	je @f
		cmp ax,10
		jl .zifra_0_9
			mov al,'?'
			sub ax,48
		.zifra_0_9:
		add ax,48
		jmp .no_colors
	@@:

	mov esi,[options_file]
	cmp dword[esi],'KPCK'
	jne @f
		notify_window_run txt_no_kpack ; ������� ���� @notify
		jmp .no_colors
	@@:
	mov edi,count_colors_text
	mov ecx,9*4
	rep movsb

	mov eax,[options_file]
	add eax,32
	mov dword[tex_colors],eax

	mov eax,dword[count_colors_text] ;init: Col (offset to key words)
	add eax,8
	shl eax,2
	add eax,[options_file]
	mov dword[Col],eax

	;init: tex_help_f1
	mov ecx,dword[ColWords]
	imul ecx,sizeof.TexColViv
	add ecx,dword[Col]
	mov dword[tex_help_f1],ecx

	;������������ ������ � ���� ���������� ���������
	stdcall [ted_clear], tedit0,1

	stdcall print_capt,capt_header

	stdcall print_capt,col_1
	stdcall print_color,[color_cursor]
	stdcall print_capt,col_2
	stdcall print_color,[color_wnd_capt]
	stdcall print_capt,col_3
	stdcall print_color,[color_wnd_work]
	stdcall print_capt,col_4
	stdcall print_color,[color_wnd_bord]
	stdcall print_capt,col_5
	stdcall print_color,[color_select]
	stdcall print_capt,col_6
	stdcall print_color,[color_cur_text]

	stdcall print_capt,col_7
	mov ecx,[count_colors_text] ;���������� ������ ������
	mov eax,[tex_colors]
	@@:
		stdcall print_capt,capt_type_dd
		stdcall print_color,[eax]
		add eax,4
	loop @b

	stdcall print_capt,capt_lbl_wo
	mov ecx,dword[ColWords]
	mov eax,[Col]
	xor ebx,ebx
	@@:
		inc ebx
		stdcall print_word,eax,ebx
		add eax,sizeof.TexColViv
	loop @b

	stdcall print_capt,capt_lbl_f1
	mov ecx,dword[ColWords]
	mov eax,[Col]
	xor ebx,ebx
	@@:
		inc ebx
		stdcall print_word_f1,eax,ebx
		add eax,sizeof.TexColViv
	loop @b
	
	; ���������� ���  PathShow
	push dword PathShow_data_1
	call [PathShow_prepare]
	call draw_window

	.no_colors:
	popad
	ret

align 4
proc print_color, color:dword
	pushad
	mov eax,dword[color]

	mov ebx,eax ;������ ������� red � blue
	shr ebx,16
	xchg al,bl
	and eax,0xffff
	and ebx,0xff
	shl ebx,16
	or eax,ebx

	ror al,4
	ror eax,8
	ror al,4
	ror eax,8
	ror al,4
	rol eax,16
	mov edi,text_buffer
	mov word[edi],'0x'
	add edi,2
	mov ecx,6
	@@:
		mov ebx,eax
		and ebx,0xf
		cmp bx,9
		jle .cifra
			add bx,39
		.cifra:
		add bx,48
		mov byte[edi],bl
		inc edi
		shr eax,4
	loop @b
	mov byte[edi],13
	mov ebx,ted_opt_ed_change_time+ted_opt_ed_move_cursor
	stdcall [ted_text_add], tedit0,text_buffer,9,ebx
	popad
	ret
endp

align 4
proc print_capt, capt:dword
	pushad
	mov esi,dword[capt]
	stdcall str_len,esi
	stdcall [ted_text_add], tedit0,esi,eax,ted_opt_ed_change_time+ted_opt_ed_move_cursor
	popad
	ret
endp

align 4
proc print_word, text:dword, index:dword
	pushad
	mov esi,text_buffer
	mov byte[esi],0
	stdcall str_cat,esi,capt_beg_word
	stdcall str_len,esi
	add esi,eax

	mov edi,dword[text]	
	stdcall code_string,edi,esi ;�������� ������ � �������� ������
	stdcall str_cat,esi,capt_end_word

	cmp dword[edi+MAX_COLOR_WORD_LEN],0 ;f1
	je @f
		stdcall str_cat,esi,capt_calc_f1
		stdcall str_cat_value,esi,[index]
		stdcall str_cat,esi,capt_lbl_f1_use
	@@:

	mov esi,text_buffer
	stdcall str_len,esi ;������ ������ ���� ������
	stdcall [ted_text_add], tedit0,esi,eax,ted_opt_ed_change_time+ted_opt_ed_move_cursor

mov ecx,edi
mov edi,text_buffer
mov byte[edi],','
inc edi
mov byte[edi],0
	xor eax,eax
	mov al,byte[ecx+MAX_COLOR_WORD_LEN+4] ;wwo
	stdcall str_cat_value,edi,eax
	stdcall str_len,edi
	add edi,eax
	mov byte[edi],','
	inc edi

	xor eax,eax
	mov ax,word[ecx+MAX_COLOR_WORD_LEN+4+1] ;(escc shl 8)+endc
	cmp ax,0 ;��������� ���� �� ����������� � ������������ ������
	je @f
		stdcall str_cat_value,edi,eax
		stdcall str_len,edi
		add edi,eax
	@@:
	mov byte[edi],','
	inc edi

	xor eax,eax
	mov al,byte[ecx+MAX_COLOR_WORD_LEN+4+3] ;color
	stdcall str_cat_value,edi,eax
	stdcall str_len,edi
	add edi,eax
	mov byte[edi],13
	inc edi
	mov byte[edi],0

	mov esi,text_buffer
	stdcall str_len,esi
	stdcall [ted_text_add], tedit0,esi,eax,ted_opt_ed_move_cursor

	popad
	ret
endp

align 4
proc print_word_f1, text:dword, index:dword
	pushad
	mov edi,dword[text]
	mov esi,dword[edi+MAX_COLOR_WORD_LEN] ;f1
	cmp esi,0
	je @f
		mov ebx,text_buffer
		mov byte[ebx],0
		stdcall str_cat,ebx,capt_point
		stdcall str_cat_value,ebx,[index]
		stdcall str_cat,ebx,capt_beg_db
		stdcall str_len,ebx
		add ebx,eax
		
		add esi,dword[tex_help_f1]
		stdcall code_string,esi,ebx ;�������� ������
		stdcall str_cat,ebx,capt_end_word_f1

		mov ebx,text_buffer
		stdcall str_len,ebx
		stdcall [ted_text_add], tedit0,ebx,eax,ted_opt_ed_move_cursor
	@@:
	popad
	ret
endp

align 4
proc code_string, text:dword, out_text:dword
	pushad
	mov esi,dword[text]
	stdcall str_len,esi
	mov ecx,eax
	mov edi,dword[out_text]

	.copy:
		mov al,byte[esi]
		inc esi
		cmp al,9
		je @f
		cmp al,13
		je @f
		cmp al,39
		je @f
			mov byte[edi],al
			inc edi
			loop .copy
			jmp .end_copy
		@@:
			mov byte[edi],39
			mov byte[edi+1],','
			add edi,2
			and eax,0xff
			mov dword[edi],0
			call tl_convert_to_str
			stdcall str_len,edi
			add edi,eax
			mov byte[edi],','
			mov byte[edi+1],39
			add edi,2
	loop .copy
	.end_copy:
	mov byte[edi],0
	popad
	ret
endp

align 4
proc mem_clear, mem:dword, len:dword
	push eax ecx edi
	cld
	xor al,al
	mov edi, dword[mem]
	mov ecx, dword[len]
	repne stosb
	pop edi ecx eax
	ret
endp

align 4
proc str_cat, str1:dword, str2:dword
	push eax ecx edi esi
	mov esi,dword[str2]
	stdcall str_len,esi
	mov ecx,eax
	inc ecx
	mov edi,dword[str1]
	stdcall str_len,edi
	add edi,eax
	cld
	repne movsb
	pop esi edi ecx eax
	ret
endp

align 4
proc str_cat_value, str1:dword, value:dword
	pushad
	mov edi,[str1]
	stdcall str_len,edi
	add edi,eax
	mov eax,[value]
	call tl_convert_to_str
	popad
	ret
endp

;output:
; eax = strlen
align 4
proc str_len, str1:dword
	mov eax,[str1]
	@@:
		cmp byte[eax],0
		je @f
		inc eax
		jmp @b
	@@:
	sub eax,[str1]
	ret
endp

;input:
; eax = value
; edi = string buffer
align 4
tl_convert_to_str:
	pushad
	stdcall mem_clear,edi,12
	call .str
	popad
	ret

align 4
.str:
	mov ecx,0x0a ;�������� ������� ��������� ���������� �������� ebx,eax,ecx,edx ������� ��������� eax - �����
		;�������� ����� � ASCII ������ ������� ������ ecx=������� �������� edi ����� ���� ����������, ����� ������, ������ ����� ���������� 
	cmp eax,ecx  ;�������� ���� � eax ������ ��� � ecx �� ������� �� @@-1 �.�. �� pop eax
	jb @f
		xor edx,edx ;�������� edx
		div ecx     ;��������� - ������� � edx
		push edx    ;�������� � ����
		;dec edi    ;�������� ����������� ��� ������ � ����� ������
		call .str   ;������� �� ���� ���� �.�. ������� ���� ���� � ��� �� ���� ������� ���� � eax �� ������ ������ ��� � ecx
		pop eax
	@@: ;cmp al,10 ;��������� �� ������ �� �������� � al ��� 10 (��� ������� �������� 10 ������ ������� - ������))
	or al,0x30  ;������ ������� ������  ��� ��� ���� 
	stosb	    ;�������� ������� �� �������� al � ����� ������ es:edi
	ret
