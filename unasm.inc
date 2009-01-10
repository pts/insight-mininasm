;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; unasm.inc
; Disassembler.
;-----------------------------------------------------------------------------
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
; 02110-1301 USA
;=============================================================================


cpu_type	db	a86

;=============================================================================
; unassemble
;-----------------------------------------------------------------------------
; Unassembles command from unasm_seg:SI
unassemble:
		push	ax
		push	bx
		push	cx
		push	dx
		push	di
		push	ds
		push	es
		cld
		xor	ax,ax
		mov	[memory_flag],al
		mov	[mem_disp_offs],ax
		mov	[jcnear],al
		mov	byte [mem_disp_type],8
		mov	[mem_prefix],ax
		mov	ds,[cs:unasm_seg]
		push	cs
		pop	es
		mov	[cs:unasm_cur_ip],si
		mov	[cs:ignore_prefix],al
get_next_cmd:
		call	clear_buffer
		mov	di,CMD_X
		xor	ax,ax
		mov	[cs:op_size_flag],al
		mov	[cs:addr_size_flag],al
		mov	[cs:str_flag],al
		mov	[cs:cur_rep],ax
		mov	[cs:db_flag],al
get_next_byte:
		lodsb
		mov	[cs:cur_cmd],al
		xor	bx,bx
		mov	bl,al
		shl	bx,1
		mov	byte [cs:inst_286],0
		mov	byte [cs:print_flag],0
		mov	dx,[cs:cmd_parameters+bx]
		mov	[cs:cur_param],dx
		mov	dx,[cs:cmd_addrs+bx]
		cmp	dx,size_prefixes
		jb	not_size_prefix
		jmp	analyze_size_prefix
not_size_prefix:
		cmp	dx,seg_prefixes
		jb	not_seg_prefix
		jmp	analyze_seg_prefix
not_seg_prefix:
		cmp	dx,unasm_rep_prefixes
		jb	get_code_name
		jmp	analyze_rep_prefix
get_code_name:
		call	analyze_code
		mov	dx,[cs:cur_param]
		cmp	byte [cs:inst_286],0
		je	param_8086

		mov	bl,dh
		mov	bh,0
		add	bx,bx
		mov	dx,[cs:table_286_param+bx]
		mov	[cs:cur_param],dx

param_8086:
		or	dx,dx
		jz	no_params
		or	dh,dh
		jne	@@no_print
		mov	byte [cs:print_flag],1
@@no_print:
		mov	al,dl
		call	analyze_param
		cmp	byte [cs:db_flag],0
		jne	make_db_command
		mov	dx,[cs:cur_param]
		or	dh,dh
		jz	unasm_one_param
		cmp	dh,aemp
		je	unasm_one_param
		call	delimit
		mov	al,dh
		call	analyze_param
		cmp	byte [cs:db_flag],0
		jne	make_db_command
no_params:
unasm_one_param:
		cmp	word [cs:cur_rep],0
		je	no_rep
		cmp	byte [cs:str_flag],1
		je	no_rep
		mov	word [cs:cur_rep],0
make_db_command:
		call	clear_buffer
		mov	dx,cdb
		mov	word [cs:cur_param],0
		mov	di,CMD_X
		mov	si,[cs:unasm_cur_ip]
		lodsb
		jmp	get_code_name
no_rep:
		cmp	word [cs:cur_prefix],0
		jne	prefix_not_used

prefix_now_used:
						;[1245 - 1.01]
		cmp	byte [cs:op_size_flag],0
		je	@@skip_param_32
		mov	byte [cs:memory_flag],3

@@skip_param_32:
						;[1245 - 1.01]
		push	di
		mov	cx,si
		sub	cx,[cs:unasm_cur_ip]
		mov	[cs:cmd_length],cx
		mov	di,buffer+6
		call	fill_codes
		mov	di,buffer
		call	fill_addr
		pop	di
		pop	es
		pop	ds
		lea	ax,[di-(CMD_X-offset_0100h+100h)]
;		lea	ax,[di-(CMD_X-OFFS100H+100h)]
		mov	[cmd_text_size],ax
		pop	di
		pop	dx
		pop	cx
		pop	bx
		pop	ax
		ret

cmd_length	dw	0

; Return from unassemble --------------------------
analyze_size_prefix:
		mov	ah,[cs:cur_param]
		cmp	ah,[cs:cpu_type]
		ja	bad_prefix
		cmp	dx,op_size
		je	set_op_size
		mov	byte [cs:addr_size_flag],1
		jmp	get_next_byte
set_op_size:
		mov	byte [cs:op_size_flag],1
		jmp	get_next_byte

prefix_not_used:
many_prefs:
		mov	si,[cs:unasm_cur_ip]
		mov	word [cs:cur_prefix],0
		mov	byte [cs:ignore_prefix],1
		jmp	get_next_cmd

analyze_seg_prefix:
		mov	ah,[cs:cur_param]
		cmp	ah,[cs:cpu_type]
		jbe	pref_ok
bad_prefix:
		mov	dx,cdb
loc_get_code_name:
		jmp	get_code_name
pref_ok:
		cmp	byte [cs:ignore_prefix],1
		je	loc_get_code_name
		cmp	word [cs:cur_prefix],0
		jne	many_prefs
		mov	[cs:cur_prefix],dx
		jmp	get_next_byte

analyze_rep_prefix:
		mov	bx,dx
		call	fill_csbx
		mov	di,OPER_X
		mov	[cs:cur_rep],dx
		jmp	get_next_byte

;=============================================================================
; analyze_code
;-----------------------------------------------------------------------------
analyze_code:
		cmp	al,0fh
		je	check_pm_inst
		mov	bx,[cs:cur_param]
		cmp	bl,a86
		jbe	not_286

check_286_inst:
		cmp	bl,80h
		jae	not_286
		cmp	bl,[cs:cpu_type]
		jbe	@@ok
		mov	dx,cdb
		jmp	..@go_ahead

@@ok:
		mov	byte [cs:inst_286],1
		jmp	..@test_ext

ch_second:
		lodsb
		cmp	al,0ah
		je	..@go_ahead
		mov	byte [cs:cur_param],aim8
		dec	si
		jmp	..@go_ahead

check_pm_inst:
		cmp	byte [cs:cpu_type],a86
		je	not_286
		jmp	analyze_pm_inst

not_286:
		cmp	al,0d4h
		je	ch_second
		cmp	al,0d5h
		je	ch_second
..@test_ext:
		cmp	byte [cs:addr_size_flag],0
		je	@@skip_addr
		cmp	al,0e3h
		je	check_jecxz
@@skip_addr:
		cmp	byte [cs:op_size_flag],0
		je	@@skip_op
		mov	bx,ext_table
@@next_ext:
		mov	ah,[cs:bx]
		or	ah,ah
		jz	..@go_ahead
		cmp	al,ah
		je	@@found_ext
		add	bx,3
		jmp	@@next_ext
@@found_ext:
		mov	dx,[cs:bx+1]
		jmp	..@go_ahead
@@skip_op:
;		cmp	al,0D8h
;		jb	@@Go_ahead
;		cmp	al,0DFh
;		jbe	Coprocessor
..@go_ahead:
		push	si
		cmp	dx,6
		jbe	groups
make_db:
		mov	si,dx
		cmp	dx,cdb
		jne	..@quit_00
		call	unasm_fill_str
		pop	si
		mov	si,[cs:unasm_cur_ip]
		lodsb
		mov	di,OPER_X
		call	fill_byte_str
		mov	word [cs:cur_param],0
		ret
check_jecxz:
		mov	dx,cjecxz
		jmp	..@go_ahead

groups:
		mov	byte [cs:print_flag],1
		mov	al,[si]
		xor	bx,bx
		mov	bl,al
		and	bl,00111000b
		cmp	dx,grp_1
		jne	@@1
		cmp	bl,00000000b
		jne	@@1
		mov	byte [cs:cur_param+1],aim8
		cmp	byte [cs:cur_param],arm8
		je	@@1
		mov	byte [cs:cur_param+1],aim16
@@1:
		shr	bl,1
		shr	bl,1
		shl	dx,1
		shl	dx,1
		shl	dx,1
		shl	dx,1
		add	bx,dx
		mov	si,[cs:grp_commands+bx]
		cmp	si,qqq
		jne	..@quit_00
		mov	dx,cdb
		mov	word [cs:cur_param],0
		jmp	make_db
..@quit_00:
		call	unasm_fill_str
		pop	si
		cmp	di,OPER_X
		ja	@@exit
		mov	di,OPER_X
		ret
@@exit:
		inc	di
		ret

analyze_pm_inst:
		push	si
		lodsb
		cmp	al,1
		jbe	pm_group
		cmp	al,6
		jbe	pm_cmd
		mov	bx,cud0
		cmp	al,0ffh			;ud0
		je	ud_cmd
		mov	bx,cud1
		cmp	al,0b9h			;ud1
		je	ud_cmd
		mov	bx,cud2
		cmp	al,00bh			;ud2
		je	ud_cmd
		cmp	byte [cs:cpu_type],a386
		jae	analyze_386_inst
bad_pm_command:
		pop	si
		jmp	..@go_ahead
ud_cmd:
		mov	dx,bx
		mov	ax,ano*100h+ano
		mov	[cs:cur_param],ax
		pop	si
		inc	si
		jmp	..@go_ahead
pm_cmd:
		add	al,14
		mov	bl,al
		mov	bh,0
		shl	bx,1
		jmp	get_pm_name
pm_group:
		mov	bl,al
		lodsb
		and	al,00111000b
		shr	al,1
		shr	al,1
		mov	cl,4
		shl	bl,cl
		or	bl,al
		mov	bh,0
get_pm_name:
		mov	dx,[cs:pm_commands+bx]
		cmp	dx,cdb
		je	bad_pm_command
		cmp	dx,cinvlpg
		je	check_invlpg
invlpg_ok:
		mov	ax,[cs:pm_param+bx]
		mov	[cs:cur_param],ax
		pop	si
		inc	si
		jmp	..@go_ahead
check_invlpg:
		cmp	byte [cpu_type],a486
		jae	invlpg_ok
		mov	dx,cdb
		jmp	bad_pm_command

analyze_386_inst:
		push	di
		push	es
		push	cs
		pop	es
		mov	di,table_386_loc
		mov	cx,T3_LENGTH
		cmp	byte [cs:cpu_type],a486
		jne	@@3
		mov	cx,T4_LENGTH
@@3:
	repne	scasb
		mov	bx,di
		pop	es
		pop	di
		jne	bad_pm_command
		sub	bx,table_386_loc+1
		add	bx,bx
		mov	dx,[cs:name_386+bx]
		mov	ax,[cs:param_386+bx]
		mov	[cs:cur_param],ax
		cmp	dx,bt_grp
		je	analyze_bt_grp
done_bt:
		pop	si
		inc	si
		mov	byte [cs:jcnear],1
		jmp	..@go_ahead
analyze_bt_grp:
		lodsb
		and	al,00111000b
		shr	al,1
		shr	al,1
		cbw
		mov	bx,ax
		mov	dx,[cs:bt_cmd+bx]
		jmp	done_bt
;=============================================================================
ext_table:
		db	60h
		dw	cpushad
		db	61h
		dw	cpopad
		db	6Dh
		dw	cinsd
		db	6Fh
		dw	coutsd
		db	98h
		dw	ccwde
		db	99h
		dw	ccdq
		db	9Ch
		dw	cpushfd
		db	9Dh
		dw	cpopfd
		db	0A5h
		dw	cmovsd
		db	0A7h
		dw	ccmpsd
		db	0ABh
		dw	cstosd
		db	0ADh
		dw	clodsd
		db	0AFh
		dw	cscasd
		db	0CFh
		dw	ciretd
		db	0
;=============================================================================
pm_commands:
		dw	csldt
		dw	cstr
		dw	clldt
		dw	cltr
		dw	cverr
		dw	cverw
		dw	cdb
		dw	cdb

		dw	csgdt
		dw	csidt
		dw	clgdt
		dw	clidt
		dw	csmsw
		dw	cdb
		dw	clmsw
		dw	cinvlpg

		; Skipped 2 words
		dw	clar
		dw	clsl
		dw	cdb
		dw	cloadall286
		dw	cclts
		; Skipped 1 word
;=============================================================================
pm_param:
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	ano, ano
		db	ano, ano

		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	arm16, aemp
		db	ano, ano
		db	arm16, aemp
		db	arm16, aemp

		; Skipped 2 words
		db	ar16rm, aemp
		db	ar16rm, aemp
		db	ano, ano
		db	ano, ano
		db	ano, ano
		; Skipped 1 word
;=============================================================================
bt_cmd:
		dw	cdb
		dw	cdb
		dw	cdb
		dw	cdb
		dw	cbt
		dw	cbts
		dw	cbtr
		dw	cbtc
;=============================================================================
table_386_loc:
		db	07				;loadall386

		db	20h, 21h, 22h, 23h, 24h, 26h	; mov CR, DR, TR

		db	80h, 81h, 82h, 83h, 84h, 85h, 86h, 87h
		db	88h, 89h, 8Ah, 8Bh, 8Ch, 8Dh, 8Eh, 8Fh

		db	90h, 91h, 92h, 93h, 94h, 95h, 96h, 97h
		db	98h, 99h, 9Ah, 9Bh, 9Ch, 9Dh, 9Eh, 9Fh

		db	0A0h, 0A1h
		db	0A3h, 0A4h, 0A5h
		db	0A8h, 0A9h
		db	0ABh, 0ACh, 0ADh, 0AFh, 0B2h
		db	0B3h, 0B4h, 0B5h, 0B6h, 0B7h, 0BAh, 0BBh
		db	0BCh, 0BDh, 0BEh, 0BFh

T3_LENGTH	equ	$-table_386_loc
;=============================================================================
table_486_loc:
		db	08h, 09h, 0B0h, 0B1h, 0C0h, 0C1h
		db	0C8h, 0C9h, 0CAh, 0CBh, 0CCh, 0CDh, 0CEh, 0CFh

T4_LENGTH	equ	$-table_386_loc
;=============================================================================
name_386:
		dw	cloadall386

		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov

		dw	cjo
		dw	cjno
		dw	cjb
		dw	cjnb
		dw	cjz
		dw	cjnz
		dw	cjbe
		dw	cja
		dw	cjs
		dw	cjns
		dw	cjp
		dw	cjnp
		dw	cjl
		dw	cjge
		dw	cjle
		dw	cjg

		dw	cseto
		dw	csetno
		dw	csetb
		dw	csetae
		dw	csete
		dw	csetne
		dw	csetbe
		dw	cseta
		dw	csets
		dw	csetns
		dw	csetp
		dw	csetnp
		dw	csetl
		dw	csetge
		dw	csetle
		dw	csetg

		dw	cpush
		dw	cpop

		dw	cbt
		dw	cshld
		dw	cshld

		dw	cpush
		dw	cpop

		dw	cbts
		dw	cshrd
		dw	cshrd
		dw	cimul
		dw	clss
		dw	cbtr
		dw	clfs
		dw	clgs
		dw	cmovzx
		dw	cmovzx
		dw	bt_grp
		dw	cbtc
		dw	cbsf
		dw	cbsr
		dw	cmovsx
		dw	cmovsx
;=============================================================================
name_486:
		dw	cinvd
		dw	cwbinvd
		dw	ccmpxchg
		dw	ccmpxchg
		dw	cxadd
		dw	cxadd

		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap
		dw	cbswap
;=============================================================================
param_386:
		db	ano,ano

		db	r_cr, aemp
		db	r_dr, aemp
		db	cr_r, aemp
		db	dr_r, aemp
		db	r_tr, aemp
		db	tr_r, aemp

		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano
		db	anear, ano

		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano
		db	arm8, ano

		db	afs, ano
		db	afs,ano

		db	arm16r, aemp
		db	arm16r, aim8
		db	arm16r, acl

		db	ags, ano
		db	ags, ano

		db	arm16r, aemp
		db	arm16r, aim8
		db	arm16r, acl
		db	ar16rm, aemp
		db	ar16rm, aemp
		db	arm16r, aemp
		db	ar16rm, aemp
		db	ar16rm, aemp
		db	ar8_16, aemp
		db	ar16_32, aemp
		db	arm16, aim8
		db	arm16r, aemp
		db	ar16rm, aemp
		db	ar16rm, aemp
		db	ar8_16, aemp
		db	ar16_32, aemp
;=============================================================================
param_486:
		db	ano, ano
		db	ano, ano
		db	arm8r, aemp
		db	arm16r, aemp
		db	arm8r, aemp
		db	arm16r, aemp

		db	aax, ano
		db	acx, ano
		db	adx, ano
		db	abx, ano
		db	aspa, ano
		db	abp, ano
		db	asi, ano
		db	adi, ano

;Coprocessor:
;		and	al,111b
;		mov	bl,al
;		mov	bh,0
;		lodsb
;		and	al,00111000b
;		or	bl,al
;		add	bx,bx
;		mov	si,cs:CoprocCmd[bx]
;		jmp	@@Quit

;=============================================================================
; analyze_param
;-----------------------------------------------------------------------------
analyze_param:
		mov	byte [cs:word_flag],0
		mov	byte [cs:dword_flag],0
		test	al,80h
		jz	@@byte
		inc	byte [cs:word_flag]
 @@byte:
		and	al,7fh

		cmp	al,40h
		jae	immed_reg

		mov	bx,sub_table
next_param:
		mov	ah,[cs:bx]
		cmp	al,ah
		je	execute
		add	bx,3
		or	ah,ah
		jnz	next_param
		ret
execute:
		jmp	[cs:bx+1]
;-----------------------------------------------------------------------------
sub_table:
		db	aim8
		dw	immed_value

		db	a1
		dw	immed_1

		db	amem8
		dw	memory

		db	asht
		dw	jump_short

		db	anear
		dw	jump_near

		db	afar
		dw	jump_far

		db	arm8
		dw	reg_mem

		db	arm8r
		dw	reg_mem_reg

		db	ar8rm
		dw	reg_reg_mem

		db	arm_seg
		dw	reg_mem_seg

		db	aseg_rm
		dw	seg_reg_mem

		db	aesc
		dw	esc_param

		db	aim8_16
		dw	im8_to_16

		db	ar8_16
		dw	reg8_to_16

		db	ar16_32
		dw	reg16_to_32

		db	r_cr
		dw	cr_to_reg

		db	cr_r
		dw	reg_to_cr

		db	r_dr
		dw	dr_to_reg

		db	dr_r
		dw	reg_to_dr

		db	r_tr
		dw	tr_to_reg

		db	tr_r
		dw	reg_to_tr

		db	astr
		dw	str_param

		db	astrn
		dw	str_nparam

		db	0
;=============================================================================
; immed_reg
;-----------------------------------------------------------------------------
immed_reg:
 		cmp	al,aal
 		jae	@@byte_reg
 		mov	byte [cs:word_flag],1
@@byte_reg:
		xor	bx,bx
		mov	bl,al
		sub	bl,40h
		shl	bx,1
		call	insert_op_size
		mov	ax,[cs:registers+bx]
		stosw
		ret
;=============================================================================
; immed_value
;-----------------------------------------------------------------------------
immed_value:
		cmp	byte [cs:word_flag],0
		je	im8
		cmp	byte [cs:op_size_flag],1
		je	im32
im8:
		jmp	fill_wb
im32:
		jmp	fill_dword_str
;=============================================================================
; im8_to_16
;-----------------------------------------------------------------------------
im8_to_16:
		lodsb
		cbw
		cmp	byte [cs:op_size_flag],1
		jne	im816
		cwd
		xchg	ax,dx
		call	fill_word_str
		xchg	ax,dx
im816:
		jmp	fill_word_str

immed_1:
		mov	al,'1'
		stosb
		ret
;=============================================================================
; memory
;-----------------------------------------------------------------------------
memory:
		call	print_ptr
		mov	al,'['
		stosb
		cmp	byte [cs:addr_size_flag],0
		je	@@1
		call	fill_dword_str
		jmp	@@2
@@1:
		lodsw
		mov	[cs:mem_disp_offs],ax
		mov	byte [cs:mem_disp_type],8
		mov	ch,[cs:word_flag]
		inc	ch
		mov	[cs:memory_flag],ch
		call	fill_word_str
@@2:
		mov	al,']'
		stosb
		ret
;=============================================================================
; reg8_to_16
;-----------------------------------------------------------------------------
reg8_to_16:
		lodsb
		mov	byte [cs:word_flag],1
		call	analyze_rm_reg
		call	delimit
		mov	byte [cs:word_flag],0
						;[1245 - 1.01]
		mov	byte [cs:op_size_flag],0
		mov	byte [cs:print_flag],1
						;[1245 - 1.01]
		jmp	analyze_rm_disp
;=============================================================================
; reg16_to_32
;-----------------------------------------------------------------------------
reg16_to_32:
		lodsb
		mov	byte [cs:op_size_flag],1
		mov	byte [cs:word_flag],1
		call	analyze_rm_reg
		call	delimit
		mov	byte [cs:op_size_flag],0
		mov	byte [cs:print_flag],1
		jmp	analyze_rm_disp
;=============================================================================
; str_param
;-----------------------------------------------------------------------------
str_param:
		cmp	word [cs:cur_prefix],0
		je	str_nparam
		mov	bx,[cs:cur_prefix]
		mov	word [cs:cur_prefix],0
		call	fill_csbx
str_nparam:
		mov	byte [cs:str_flag],1
		ret
;=============================================================================
; jump_short
;-----------------------------------------------------------------------------
jump_short:
		cmp	byte [cs:cur_cmd],0ebh
		jne	not_print
		mov	bx,short_str
		call	fill_csbx
not_print:
		lodsb
		cbw
..@print_jump:
		call	check$
		jc	@@ret1
		add	ax,si
		call	fill_word_str
@@ret1:
		ret
check$:
		or	ax,ax
		jz	@@$p
		mov	bx,[cs:unasm_cur_ip]
		sub	bx,si
		cmp	ax,bx
		je	@@$
		clc
		ret
@@$:
		mov	al,'$'
		stosb
		stc
		ret
@@$p:
		mov	ax,'$+'
		stosw
		mov	ax,si
		sub	ax,[cs:unasm_cur_ip]
		add	al,'0'
		stosb
		stc
		ret

short_str	db	'short ',0
;near_str	db	'near ',0
far_str		db	'far ',0

;=============================================================================
; jump_near
;-----------------------------------------------------------------------------
jump_near:
;		call	Fill_CS_BX
		cmp	byte [cs:addr_size_flag],1
		je	near_far
		lodsw
		jmp	..@print_jump
near_far:
		lodsw
		mov	bx,ax
		lodsw
		add	bx,si
		adc	ax,0
		call	fill_word_str
		mov	ax,bx
		jmp	fill_word_str
;=============================================================================
; jump_far
;-----------------------------------------------------------------------------
jump_far:
		mov	bx,far_str
		call	fill_csbx
		cmp	byte [cs:addr_size_flag],1
		je	far_far
		lodsw
		xchg	ax,bx
		lodsw
		call	fill_word_str
		mov	al,':'
		stosb
		xchg	ax,bx
		jmp	fill_word_str
far_far:
		lodsw
		mov	bx,ax
		lodsw
		mov	cx,ax
		lodsw
		call	fill_word_str
		mov	al,':'
		stosb
		mov	ax,cx
		call	fill_word_str
		mov	ax,bx
		jmp	fill_word_str

reg_mem:
		lodsb
		cmp	byte [cs:cur_cmd],0feh
		jae	@@jumps
@@reg_mem:
		jmp	analyze_rm_disp
@@jumps:
		push	ax
		and	al,00111000b
		cmp	al,00011000b
		jne	@@ch_jump
		mov	byte [cs:dword_flag],1
		jmp	@@ret
@@ch_jump:
		cmp	al,00101000b
		jne	@@ret
		mov	byte [cs:dword_flag],1
@@ret:
		pop	ax
		jmp	@@reg_mem
;=============================================================================
; reg_mem_reg
;-----------------------------------------------------------------------------
reg_mem_reg:
		lodsb
		call	analyze_rm_disp
		call	delimit
		jmp	analyze_rm_reg
;=============================================================================
; reg_reg_mem
;-----------------------------------------------------------------------------
reg_reg_mem:
		lodsb
		call	analyze_rm_reg
		call	delimit
		jmp	analyze_rm_disp
;=============================================================================
; reg_mem_seg
;-----------------------------------------------------------------------------
reg_mem_seg:
		lodsb
		mov	byte [cs:word_flag],1
		call	analyze_rm_disp
		call	delimit
		jmp	analyze_rm_seg
;=============================================================================
; seg_reg_mem
;-----------------------------------------------------------------------------
seg_reg_mem:
		lodsb
		mov	byte [cs:word_flag],1
		call	analyze_rm_seg
		call	delimit
		jmp	analyze_rm_disp
;=============================================================================
; esc_param
;-----------------------------------------------------------------------------
esc_param:
		dec	si
		lodsb
		and	al,00000111b
		mov	ah,al
		lodsb
		push	ax
		and	al,00111000b
		or	al,ah
		call	fill_byte_str
		pop	ax
		call	delimit
		jmp	analyze_rm_disp
;=============================================================================
; cr_to_reg
;-----------------------------------------------------------------------------
cr_to_reg:
		lodsb
		call	get_cr_reg
		mov	ah,'c'
		jmp	..@get_cr_num
dr_to_reg:
		lodsb
		call	get_cr_reg
		mov	ah,'d'
		jmp	..@get_cr_num
tr_to_reg:
		lodsb
		call	get_cr_reg
		mov	ah,'t'
..@get_cr_num:
		call	delimit
		jmp	get_cr_num
reg_to_cr:
		lodsb
		mov	ah,'c'
		jmp	..@get_cr_reg
reg_to_dr:
		lodsb
		mov	ah,'d'
		jmp	..@get_cr_reg
reg_to_tr:
		lodsb
		mov	ah,'t'
..@get_cr_reg:
		call	get_cr_num
		call	delimit
		jmp	get_cr_reg
get_cr_reg:
		push	ax
		and	al,00000111b
		shl	al,1
		cbw
		mov	bx,ax
		mov	al,'e'
		stosb
		mov	ax,[cs:registers+bx]
		stosw
		pop	ax
		ret
get_cr_num:
		push	ax
		and	al,00111000b
		shr	al,1
		shr	al,1
		shr	al,1
		add	al,'0'
		xchg	al,ah
		stosb
		mov	al,'r'
		stosw
		pop	ax
		ret
;=============================================================================
; analyze_rm_disp
;-----------------------------------------------------------------------------
analyze_rm_disp:
		push	ax
		mov	ch,[cs:word_flag]
		inc	ch
		add	ch,[cs:dword_flag]
		mov	[cs:memory_flag],ch
		cmp	byte [cs:addr_size_flag],1
;		jne	@@not_disp_32_bit
;		jmp	disp_32_bit
;@@not_disp_32_bit:
		je	disp_32_bit	;!!!
		mov	ch,al
		and	al,11000000b
		cmp	al,00000000b
		je	no_offs
		cmp	al,01000000b
		je	byte_offs
		cmp	al,10000000b
		je	word_offs
reg_2:
		mov	byte [cs:memory_flag],0
		mov	al,ch 			; r/m = Register
		and	al,00000111b
		shl	al,1
		cbw
		call	insert_op_size
		call	get_register
		stosw
		jmp	..@exit_00
word_offs:
		mov	al,ch
		call	get_disp
		mov	al,'+'
		stosb
		lodsw
		jmp	..@fill_word
byte_offs:
		mov	al,ch
		call	get_disp
		lodsb
		cbw
		mov	[cs:mem_disp_offs],ax
		mov	ah,'+'
		test	al,80h
		jz	plus_8
		mov	ah,'-'
		neg	al
plus_8:
		mov	[es:di],ah
		inc	di
		call	fill_byte_str
		jmp	..@quit_01
no_offs:
		mov	word [cs:mem_disp_offs],0
		mov	al,ch
		and	al,00000111b
		cmp	al,00000110b
		jne	read_disp
		call	print_ptr
		mov	al,'['
		stosb
		lodsw
		mov	byte [cs:mem_disp_type],8

..@fill_word:

		mov	[cs:mem_disp_offs],ax
		call	fill_word_str
		jmp	..@quit_01
read_disp:
		call	get_disp
..@quit_01:
		mov	al,']'
		stosb
..@exit_00:
		pop	ax
		ret

disp_32_bit:
		mov	ch,al
		and	al,11000000b
		cmp	al,11000000b
		je	reg_2

		mov	cl,al
		mov	al,ch
		and	al,00000111b
		cmp	al,00000100b
		je	analyze_sib

		xchg	al,cl
		cmp	al,00000000b
		je	no_ofs_32
		cmp	al,10000000b
		je	ofs_32
ofs_8:
		mov	al,cl
		call	get_32_disp
		mov	al,'+'
		stosb
		lodsb
		call	fill_byte_str
		jmp	..@quit_01
ofs_32:
		mov	al,cl
		call	get_32_disp
		mov	al,'+'
		stosb
		call	fill_dword_str
		jmp	..@quit_01
;;
no_ofs_32:
		cmp	cl,00000101b
		jne	no_disp_32
		mov	al,'['
		stosb
		call	fill_dword_str
		jmp	..@quit_01
no_disp_32:
		mov	al,cl
		call	get_32_disp
		jmp	..@quit_01
analyze_sib:
		lodsb
		mov	ch,al
		cmp	cl,00000000b
		je	sib_no_ofs
		cmp	cl,01000000b
		je	sib_ofs_8
sib_ofs_32:
		and	al,00000111b
		call	get_32_disp
		call	get_scale
		mov	al,'+'
		stosb
		call	fill_dword_str
		jmp	..@quit_01
sib_no_ofs:
		and	al,00000111b
		cmp	al,00000101b
		jne	get_sib_disp
		call	print_ptr
		mov	al,'['
		stosb
		call	fill_dword_str
		jmp	..@get_scale
get_sib_disp:
		call	get_32_disp
..@get_scale:
		call	get_scale
		jmp	..@quit_01
sib_ofs_8:
		and	al,00000111b
		call	get_32_disp
		call	get_scale
		mov	al,'+'
		stosb
		lodsb
		call	fill_byte_str
		jmp	..@quit_01
;=============================================================================
; analyze_rm_reg
;-----------------------------------------------------------------------------
analyze_rm_reg:
		push	ax
		and	al,00111000b
		shr	al,1
		shr	al,1
		cbw
		call	insert_op_size
		call	get_register
		stosw
		pop	ax
		ret
;=============================================================================
; get_register
;-----------------------------------------------------------------------------
get_register:
		push	bx
		mov	bl,[cs:word_flag]
		xor	bl,1
		xor	bh,bh
		shl	bx,1
		shl	bx,1
		shl	bx,1
		shl	bx,1
		add	bx,ax
		mov	ax,[cs:registers+bx]
		inc	bx
		pop	bx
		ret
;=============================================================================
; insert_op_size
;-----------------------------------------------------------------------------
insert_op_size:
		cmp	byte [cs:op_size_flag],0
		je	@@ret
		cmp	byte [cs:word_flag],0
		je	@@ret
		push	ax
		mov	al,'e'
		stosb
		pop	ax
@@ret:
		ret
;=============================================================================
; analyze_rm_seg
;-----------------------------------------------------------------------------
analyze_rm_seg:
		push	ax
		and	al,00111000b
		cmp	byte [cs:cpu_type],a386
		jae	@@3
		cmp	al,00011000b
		jbe	@@1
@@bad_seg_reg:
		mov	byte [cs:db_flag],1
		pop	ax
		ret
@@3:
		cmp	al,00101000b
		ja	@@bad_seg_reg
@@1:
		shr	al,1
		shr	al,1
		cbw
		mov	bx,ax
		mov	ax,[cs:seg_regs+bx]
		stosw
		pop	ax
		ret
;=============================================================================
; get_disp
;-----------------------------------------------------------------------------
get_disp:
		call	print_ptr
		and	al,00000111b
		cbw
		mov	[cs:mem_disp_type],al
		shl	ax,1
		mov	bx,ax
		mov	bx,[cs:disp_adrs+bx]
		jmp	fill_csbx
;=============================================================================
disp_adrs:
		dw	of000
		dw	of001
		dw	of010
		dw	of011
		dw	of100
		dw	of101
		dw	of110
		dw	of111

of000		db	'[bx+si',0
of001		db	'[bx+di',0
of010		db	'[bp+si',0
of011		db	'[bp+di',0
of100		db	'[si',0
of101		db	'[di',0
of110		db	'[bp',0
of111		db	'[bx',0

;=============================================================================
; get_32_disp
;-----------------------------------------------------------------------------
get_32_disp:
		cmp	byte [cs:op_size_flag],0
		je	@@1
		mov	byte [dword_flag],1
@@1:
		call	print_ptr
		cbw
		mov	bx,ax
		add	bx,bx
		mov	ax,'[e'
		stosw
		mov	ax,[cs:registers+bx]
		stosw
		ret
;=============================================================================
; get_scale
;-----------------------------------------------------------------------------
get_scale:
		mov	al,ch
		and	al,00111000b
		cmp	al,00100000b
		je	no_scale
		shr	al,1
		shr	al,1
		cbw
		mov	bx,ax
		mov	ax,'+e'
		stosw
		mov	ax,[cs:registers+bx]
		stosw
		mov	al,'*'
		stosb
		mov	al,ch
		and	al,11000000b
		rol	al,1
		rol	al,1
		cbw
		mov	bx,ax
		mov	al,[cs:scaler+bx]
		stosb
no_scale:
		ret

scaler		db	'1248'


pt_byte		db	'byte ',0
pt_dword	db	'd'
pt_word		db	'word ',0
pt_ptr		db	'ptr ',0
;=============================================================================
; print_ptr
;-----------------------------------------------------------------------------
print_ptr:
		push	ax
		push	si
		cmp	byte [cs:print_flag],0
		je	@@quit
		mov	si,pt_byte
		cmp	byte [cs:word_flag],0
		je	@@byte
		mov	si,pt_word
						;[1245 - 1.01]
		cmp	byte [cs:op_size_flag],0
		je	@@check_dword
		mov	byte [cs:dword_flag],1
;		mov	cs:memory_flag,3
@@check_dword:
						;[1245 - 1.01]
		cmp	byte [cs:dword_flag],0
		je	@@byte
		dec	si
@@byte:
		call	unasm_fill_str
		mov	si,pt_ptr
		call	unasm_fill_str
@@quit:
		cmp	word [cs:cur_prefix],0
		je	@@exit
		mov	si,[cs:cur_prefix]
		mov	[cs:mem_prefix],si
		call	unasm_fill_str
@@exit:
		mov	word [cs:cur_prefix],0
		pop	si
		pop	ax
		ret
;=============================================================================
; unasm_fill_str
;-----------------------------------------------------------------------------
unasm_fill_str:
		push	ds
		push	cs
		pop	ds
@@next:
		lodsb
		or	al,al
		jz	@@quit
		stosb
		jmp	@@next
@@quit:
		pop	ds
		ret
;=============================================================================
; fill_csbx
;-----------------------------------------------------------------------------
fill_csbx:
@@next:
		mov	al,[cs:bx]
		or	al,al
		jz	@@quit
		stosb
		inc	bx
		jmp	@@next
@@quit:
		ret
;=============================================================================
; delimit
;-----------------------------------------------------------------------------
delimit:
		push	ax
		mov	al,','
		stosb
		pop	ax
		ret
;=============================================================================
; fill_addr
;-----------------------------------------------------------------------------
fill_addr:
		mov	ax,[cs:unasm_cur_ip]
		jmp	fill_word_str
;=============================================================================
; fill_codes
;-----------------------------------------------------------------------------
fill_codes:
		push	si
		mov	si,[cs:unasm_cur_ip]
@@next_byte:
		cmp	di,CMD_X-1
		jae	@@trunc
		lodsb
		call	fill_byte_str
		loop	@@next_byte
@@quit:
		pop	si
		ret
@@trunc:
		mov	al,'+'
		stosb
		jmp	@@quit
;=============================================================================
; fill_wb
;-----------------------------------------------------------------------------
fill_wb:
		cmp	byte [cs:word_flag],1
		je	@@out_word
		lodsb
		jmp	fill_byte_str
@@out_word:
		lodsw
		jmp	fill_word_str
;-------------------------------------------------------------------------------
; Print_Command	proc	near
;		push	si
;		mov	ax,0A0Dh
;		stosw
;		mov	cx,di
;		mov	dx,offset Buffer
;		sub	cx,dx
;		mov	ah,40h
;		mov	bx,1
;		int	21h
;		pop	si
;		ret
; Print_Command	endp

;=============================================================================
; clear_buffer
;-----------------------------------------------------------------------------
clear_buffer:
		mov	di,buffer
		push	di
		mov	al,' '
		mov	cx,BUF_SIZE
	rep	stosb
		pop	di
		ret
;=============================================================================
registers	db	'axcxdxbxspbpsidi'
		db	'alcldlblahchdhbh'

seg_regs	db	'escsssdsfsgs'


commands:
 cadd		db	'add',0
 cor		db	'or',0
 cadc		db	'adc',0
 csbb		db	'sbb',0
 cand		db	'and',0
 csub		db	'sub',0
 cxor		db	'xor',0
 ccmp		db	'cmp',0
 cinc		db	'inc',0
 cdec		db	'dec',0
 cpush		db	'push',0
 cpop		db	'pop',0
 cmov		db	'mov',0
 cxchg		db	'xchg',0
 cshl		db	'shl',0
 cshr		db	'shr',0
 crcl		db	'rcl',0
 crcr		db	'rcr',0
 csar		db	'sar',0
 crol		db	'rol',0
 cror		db	'ror',0
 ctest		db	'test',0
 cnot		db	'not',0
 cneg		db	'neg',0
 cmul		db	'mul',0
 cimul		db	'imul',0
 cdiv		db	'div',0
 cidiv		db	'idiv',0
 ccbw		db	'cbw',0
 ccwd		db	'cwd',0
 caaa		db	'aaa',0
 caad		db	'aad',0
 caam		db	'aam',0
 caas		db	'aas',0
 cdaa		db	'daa',0
 cdas		db	'das',0
 csalc		db	'salc',0
 cmovsb		db	'movsb',0
 cmovsw		db	'movsw',0
 cstosb		db	'stosb',0
 cstosw		db	'stosw',0
 clodsb		db	'lodsb',0
 clodsw		db	'lodsw',0
 ccmpsb		db	'cmpsb',0
 ccmpsw		db	'cmpsw',0
 cscasb		db	'scasb',0
 cscasw		db	'scasw',0

 ccall		db	'call',0
 cjmp		db	'jmp',0
 cret		db	'ret',0
 cretf		db	'retf',0
 cloop		db	'loop',0
 cloope		db	'loope',0
 cloopne	db	'loopne',0
 cint		db	'int',0
 cinto		db	'into',0
 cint3		db	'int3',0
 ciret		db	'iret',0
 cpushf		db	'pushf',0
 cpopf		db	'popf',0
 csahf		db	'sahf',0
 clahf		db	'lahf',0
 cclc		db	'clc',0
 ccmc		db	'cmc',0
 cstc		db	'stc',0
 ccld		db	'cld',0
 cstd		db	'std',0
 ccli		db	'cli',0
 csti		db	'sti',0
 chlt		db	'hlt',0
 cnop		db	'nop',0
 clea		db	'lea',0
 cles		db	'les',0
 clds		db	'lds',0
 cxlat		db	'xlat',0
 cjcxz		db	'jcxz',0
 cin		db	'in',0
 cout		db	'out',0
 cesc		db	'esc',0
 cwait		db	'wait',0

 cjo		db	'jo',0
 cjno		db	'jno',0
 cjb		db	'jb',0
 cjnb		db	'jnb',0
 cjz		db	'jz',0
 cjnz		db	'jnz',0
 cjbe		db	'jbe',0
 cja		db	'ja',0
 cjs		db	'js',0
 cjns		db	'jns',0
 cjp		db	'jp',0
 cjnp		db	'jnp',0
 cjl		db	'jl',0
 cjge		db	'jge',0
 cjle		db	'jle',0
 cjg		db	'jg',0

; 286 instructions

 cpusha		db	'pusha',0
 cpopa		db	'popa',0
 cbound		db	'bound',0
 cinsb		db	'insb',0
 coutsb		db	'outsb',0
 cinsw		db	'insw',0
 coutsw		db	'outsw',0
 center		db	'enter',0
 cleave		db	'leave',0

 cud0		db	'ud0', 0
 cud1		db	'ud1', 0
 cud2		db	'ud2', 0

; 286 Protected Mode instructions

 clgdt		db	'lgdt',0
 csgdt		db	'sgdt',0
 clidt		db	'lidt',0
 csidt		db	'sidt',0
 clldt		db	'lldt',0
 csldt		db	'sldt',0
 clmsw		db	'lmsw',0
 csmsw		db	'smsw',0
 cltr		db	'ltr',0
 cstr		db	'str',0
 clar		db	'lar',0
 clsl		db	'lsl',0
 carpl		db	'arpl',0
 cverr		db	'verr',0
 cverw		db	'verw',0
 cclts		db	'clts',0
 cloadall286	db	'loadall286',0
; c		db	'',0

; 386 instructions

 cmovsx		db	'movsx',0
 cmovzx		db	'movzx',0
 clfs		db	'lfs',0
 clgs		db	'lgs',0
 clss		db	'lss',0
 ccwde		db	'cwde',0
 ccdq		db	'cdq',0
 cshld		db	'shld',0
 cshrd		db	'shrd',0
 cbt		db	'bt',0
 cbts		db	'bts',0
 cbtr		db	'btr',0
 cbtc		db	'btc',0
 cbsf		db	'bsf',0
 cbsr		db	'bsr',0
 clodsd		db	'lodsd',0
 cstosd		db	'stosd',0
 cinsd		db	'insd',0
 coutsd		db	'outsd',0
 cmovsd		db	'movsd',0
 ccmpsd		db	'cmpsd',0
 cscasd		db	'scasd',0

 cseta		db	'seta',0
 csetae		db	'setae',0
 csetb		db	'setb',0
 csetbe		db	'setbe',0
 csete		db	'sete',0
 csetne		db	'setne',0
 csetp		db	'setp',0
 csetnp		db	'setnp',0
 csetg		db	'setg',0
 csetge		db	'setge',0
 csetl		db	'setl',0
 csetle		db	'setle',0
 csets		db	'sets',0
 csetns		db	'setns',0
 cseto		db	'seto',0
 csetno		db	'setno',0

 cjecxz		db	'jecxz',0
 ciretd		db	'iretd',0
 cpushad	db	'pushad',0
 cpopad		db	'popad',0
 cpushfd	db	'pushfd',0
 cpopfd		db	'popfd',0
 
 cloadall386	db	'loadall386',0

; 486 instructions

 cinvd		db	'invd',0
 cwbinvd	db	'wbinvd',0
 cinvlpg	db	'invlpg',0
 ccmpxchg	db	'cmpxchg',0
 cxadd		db	'xadd',0
 cbswap		db	'bswap',0
; c		db	'',0

;~comment #
;~
;~ Cfld		db	'fld',0
;~ cfst		db	'fst',0
;~ cfstp		db	'fstp',0
;~ cfxch		db	'fxch',0
;~ cfcom		db	'fcom',0
;~ cfcomp		db	'fcomp',0
;~ cfcompp	db	'fcompp',0
;~ cftst		db	'ftst',0
;~ cfxam		db	'fxam',0
;~ cfadd		db	'fadd',0
;~ cfsub		db	'fsub',0
;~ cfmul		db	'fmul',0
;~ cfdiv		db	'fdiv',0
;~ cfsqrt		db	'fsqrt',0
;~ cfscale	db	'fscale',0
;~ cfprem		db	'fprem',0
;~ cfrndint	db	'frndint',0
;~ cfxtract	db	'fxtract',0
;~ cfabs		db	'fabs',0
;~ cfchs		db	'fchs',0
;~ cfptan		db	'fptan',0
;~ cfpatan	db	'fpatan',0
;~ cf2xm1		db	'f2xm1',0
;~ cfyl2x		db	'fyl2x',0
;~ cfyl2xp1	db	'fyl2xp1',0
;~ cfldz		db	'fldz',0
;~ cfld1		db	'fld1',0
;~ cfldpi		db	'fldpi',0
;~ cfldl2t	db	'fldl2t',0
;~ cfldl2e	db	'fldl2e',0
;~ cfldlg2	db	'fldlg2',0
;~ cfldln2	db	'fldln2',0
;~ cfinit		db	'finit',0
;~ cfeni		db	'feni',0
;~ cfdisi		db	'fdisi',0
;~ cfldcw		db	'fldcw',0
;~ cfstcw		db	'fstcw',0
;~ cfldsw		db	'fldsw',0
;~ cfstsw		db	'fstsw',0
;~ cfclex		db	'fclex',0
;~ cfstenv	db	'fstenv',0
;~ cfldenv	db	'fldenv',0
;~ cfsave		db	'fsave',0
;~ cfrstor	db	'frstor',0
;~ cfincstp	db	'fincstp',0
;~ cfdecstp	db	'fdecstp',0
;~ cffree		db	'ffree',0
;~ cffnop		db	'fnop',0
;~#

 cdb		db	'db',0
 qqq		db	'???',0

prefixes:

 plock		db	'lock',0
 unasm_rep_prefixes:
 prep		db	'rep ',0
 prepe		db	'repe ',0
 prepne		db	'repne ',0
 seg_prefixes:
 pcs		db	'cs:',0
 pds		db	'ds:',0
 pes		db	'es:',0
 pss		db	'ss:',0
 pfs		db	'fs:',0
 pgs		db	'gs:',0
size_prefixes:
 op_size		db	0
 AddrSize	db	0

;		db	'',0
 ArOp1		equ	0
 ArOp2		equ	1
 ShiftOp	equ	2
 grp_1		equ	3
 Grp2		equ	4
 Grp3		equ	5
 MovGrp		equ	6

 bt_grp		equ	6

cmd_addrs:
		dw	cadd 			; 0
		dw	cadd
		dw	cadd
		dw	cadd
		dw	cadd
		dw	cadd
		dw	cpush
		dw	cpop

		dw	cor
		dw	cor
		dw	cor
		dw	cor
		dw	cor
		dw	cor
		dw	cpush
		dw	cdb

		dw	cadc 			; 1
		dw	cadc
		dw	cadc
		dw	cadc
		dw	cadc
		dw	cadc
		dw	cpush
		dw	cpop

		dw	csbb
		dw	csbb
		dw	csbb
		dw	csbb
		dw	csbb
		dw	csbb
		dw	cpush
		dw	cpop

		dw	cand 			; 2
		dw	cand
		dw	cand
		dw	cand
		dw	cand
		dw	cand
		dw	pes
		dw	cdaa

		dw	csub
		dw	csub
		dw	csub
		dw	csub
		dw	csub
		dw	csub
		dw	pcs
		dw	cdas

		dw	cxor 			; 3
		dw	cxor
		dw	cxor
		dw	cxor
		dw	cxor
		dw	cxor
		dw	pss
		dw	caaa

		dw	ccmp
		dw	ccmp
		dw	ccmp
		dw	ccmp
		dw	ccmp
		dw	ccmp
		dw	pds
		dw	caas

		dw	cinc 			; 4
		dw	cinc
		dw	cinc
		dw	cinc
		dw	cinc
		dw	cinc
		dw	cinc
		dw	cinc

		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec
		dw	cdec

		dw	cpush 			; 5
		dw	cpush
		dw	cpush
		dw	cpush
		dw	cpush
		dw	cpush
		dw	cpush
		dw	cpush

		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop
		dw	cpop

		dw	cpusha 			; 6
		dw	cpopa
		dw	cbound
		dw	carpl
		dw	pfs
		dw	pgs
		dw	op_size
		dw	AddrSize

		dw	cpush
		dw	cimul
		dw	cpush
		dw	cimul
		dw	cinsb
		dw	cinsw
		dw	coutsb
		dw	coutsw

		dw	cjo 			; 7
		dw	cjno
		dw	cjb
		dw	cjnb
		dw	cjz
		dw	cjnz
		dw	cjbe
		dw	cja

		dw	cjs
		dw	cjns
		dw	cjp
		dw	cjnp
		dw	cjl
		dw	cjge
		dw	cjle
		dw	cjg

		dw	ArOp1 			; 8
		dw	ArOp1
		dw	ArOp2
		dw	ArOp2
		dw	ctest
		dw	ctest
		dw	cxchg
		dw	cxchg

		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	clea
		dw	cmov
		dw	cpop

		dw	cnop 			; 9
		dw	cxchg
		dw	cxchg
		dw	cxchg
		dw	cxchg
		dw	cxchg
		dw	cxchg
		dw	cxchg

		dw	ccbw
		dw	ccwd
		dw	ccall
		dw	cwait
		dw	cpushf
		dw	cpopf
		dw	csahf
		dw	clahf

		dw	cmov 			; A
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmovsb
		dw	cmovsw
		dw	ccmpsb
		dw	ccmpsw

		dw	ctest
		dw	ctest
		dw	cstosb
		dw	cstosw
		dw	clodsb
		dw	clodsw
		dw	cscasb
		dw	cscasw

		dw	cmov 			; B
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov

		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov
		dw	cmov

		dw	ShiftOp			; C
		dw	ShiftOp
		dw	cret
		dw	cret
		dw	cles
		dw	clds
		dw	MovGrp
		dw	MovGrp

		dw	center
		dw	cleave
		dw	cretf
		dw	cretf
		dw	cint3
		dw	cint
		dw	cinto
		dw	ciret

		dw	ShiftOp 		; D
		dw	ShiftOp
		dw	ShiftOp
		dw	ShiftOp
		dw	caam
		dw	caad
		dw	csalc
		dw	cxlat

		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc
		dw	cesc

		dw	cloopne 		; E
		dw	cloope
		dw	cloop
		dw	cjcxz
		dw	cin
		dw	cin
		dw	cout
		dw	cout

		dw	ccall
		dw	cjmp
		dw	cjmp
		dw	cjmp
		dw	cin
		dw	cin
		dw	cout
		dw	cout

		dw	plock 			; F
		dw	cdb
		dw	prepne
		dw	prepe
		dw	chlt
		dw	ccmc
		dw	grp_1
		dw	grp_1

		dw	cclc
		dw	cstc
		dw	ccli
		dw	csti
		dw	ccld
		dw	cstd
		dw	Grp2
		dw	Grp3

grp_commands:
 OpArOp1	dw	cadd, cor, cadc, csbb, cand, csub, cxor,ccmp
 OpArOp2	dw	cadd, cor, cadc, csbb, cand, csub, cxor,ccmp
 OpShiftOp	dw	crol, cror,crcl, crcr, cshl, cshr, qqq, csar
 OpGrp1		dw	ctest,qqq, cnot, cneg, cmul, cimul,cdiv,cidiv
 OpGrp2		dw	cinc, cdec,ccall,ccall,cjmp, cjmp, cpush,qqq
 OpGrp3		dw	cinc, cdec,ccall,ccall,cjmp, cjmp, cpush,qqq
 OpMovGrp	dw	cmov, qqq, qqq,  qqq,  qqq,  qqq,  qqq,  qqq


 ano		equ	00h
 aemp		equ	3Fh
 arm8		equ	01h
 arm16		equ	81h

 aim8		equ	03h
 aim16		equ	83h
 amem8		equ	04h
 amem16		equ	84h
 a1		equ	05h
 asht		equ	06h
 anear		equ	07h
 afar		equ	08h
;aseg		equ	09h
 arm8r		equ	0Ah
 arm16r		equ	8Ah
 ar8rm		equ	0Bh
 ar16rm		equ	8Bh
 arm_seg	equ	0Ch
 aseg_rm	equ	0Dh
 aesc		equ	0Eh
 aim8_16	equ	0Fh
 ar8_16		equ	10h
 ar16_32	equ	11h
 arm16r8	equ	12h
 r_cr		equ	13h
 cr_r		equ	14h
 r_dr		equ	15h
 dr_r		equ	16h
 r_tr		equ	17h
 tr_r		equ	18h
 astr		equ	19h
 astrn		equ	1Ah

 aax		equ	40h
 acx		equ	41h
 adx		equ	42h
 abx		equ	43h
 aspa		equ	44h
 abp		equ	45h
 asi		equ	46h
 adi		equ	47h

 aal		equ	48h
 acl		equ	49h
 adl		equ	4Ah
 abl		equ	4Bh
 aah		equ	4Ch
 ach		equ	4Dh
 adh		equ	4Eh
 abh		equ	4Fh

 aes		equ	50h
 acs		equ	51h
 ass		equ	52h
 ads		equ	53h
 afs		equ	54h
 ags		equ	55h

 a86		equ	70h
 a286		equ	71h
 a386		equ	72h
 a486		equ	73h
 aprot		equ	7Fh

table_286_param:

$nopar		equ	0
		db	 0,	 0
$bound		equ	1
		db	ar16rm,	aemp
$push16		equ	2
		db	aim16,	 0
$push8		equ	3
		db	aim8, 0
$imul16		equ	4
		db	ar16rm,	aim16
$imul8		equ	5
		db	ar16rm,	aim8
$enter		equ	6
		db	aim16,	aim8
$shift8		equ	7
		db	arm8,	aim8
$shift16	equ	8
		db	arm16,	aim8
$arpl		equ	9
		db	arm16r,	aemp
$strn		equ	10
		db	astrn, ano


cmd_parameters:
		db	arm8r,aemp 		; 0
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	aes,ano
		db	aes,ano

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	acs,ano
		db	aprot,ano

		db	arm8r,aemp 		; 1
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ass,ano
		db	ass,ano

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ads,ano
		db	ads,ano

		db	arm8r,aemp 		; 2
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ano,ano
		db	ano,ano

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ano,ano
		db	ano,ano

		db	arm8r,aemp 		; 3
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ano,ano
		db	ano,ano

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	aal,aim8
		db	aax,aim16
		db	ano,ano
		db	ano,ano

		db	aax,ano 		; 4
		db	acx,ano
		db	adx,ano
		db	abx,ano
		db	aspa,ano
		db	abp,ano
		db	asi,ano
		db	adi,ano

		db	aax,ano
		db	acx,ano
		db	adx,ano
		db	abx,ano
		db	aspa,ano
		db	abp,ano
		db	asi,ano
		db	adi,ano

		db	aax,ano 		; 5
		db	acx,ano
		db	adx,ano
		db	abx,ano
		db	aspa,ano
		db	abp,ano
		db	asi,ano
		db	adi,ano

		db	aax,ano
		db	acx,ano
		db	adx,ano
		db	abx,ano
		db	aspa,ano
		db	abp,ano
		db	asi,ano
		db	adi,ano

		db	a286,$nopar 		; 6
		db	a286,$nopar
		db	a286,$bound
		db	a286,$arpl
		db	a386,ano
		db	a386,ano
		db	a386,ano
		db	a386,ano

		db	a286,$push16
		db	a286,$imul16
		db	a286,$push8
		db	a286,$imul8
		db	a286,$strn
		db	a286,$strn
		db	a286,$strn
		db	a286,$strn

		db	asht,ano 		; 7
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano

		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	asht,ano

		db	arm8,aim8 		; 8
		db	arm16,aim16	; 8, 16
		db	arm8,aim8
		db	arm16,aim8_16	;;
		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp

		db	arm8r,aemp
		db	arm16r,aemp
		db	ar8rm,aemp
		db	ar16rm,aemp
		db	arm_seg,aemp
		db	ar16rm,aemp
		db	aseg_rm,aemp
		db	arm16,ano

		db	ano,ano 		; 9
		db	aax,acx
		db	aax,adx
		db	aax,abx
		db	aax,aspa
		db	aax,abp
		db	aax,asi
		db	aax,adi

		db	ano,ano
		db	ano,ano
		db	afar,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano

		db	aal,amem8 		; A
		db	aax,amem16
		db	amem8,aal
		db	amem16,aax
		db	astr,ano
		db	astr,ano
		db	astr,ano
		db	astr,ano

		db	aal,aim8
		db	aax,aim16
		db	astrn,ano
		db	astrn,ano
		db	astr,ano
		db	astr,ano
		db	astrn,ano
		db	astrn,ano

		db	aal,aim8 		; B
		db	acl,aim8
		db	adl,aim8
		db	abl,aim8
		db	aah,aim8
		db	ach,aim8
		db	adh,aim8
		db	abh,aim8

		db	aax,aim16
		db	acx,aim16
		db	adx,aim16
		db	abx,aim16
		db	aspa,aim16
		db	abp,aim16
		db	asi,aim16
		db	adi,aim16

		db	a286,$shift8 		; C
		db	a286,$shift16
		db	aim16,ano
		db	ano,ano
		db	ar16rm,aemp
		db	ar16rm,aemp
		db	arm8,aim8
		db	arm16,aim16

		db	a286,$enter
		db	a286,$nopar
		db	aim16,ano
		db	ano,ano
		db	ano,ano
		db	aim8,ano
		db	ano,ano
		db	ano,ano

		db	arm8,a1 		; D
		db	arm16,a1
		db	arm8,acl
		db	arm16,acl
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano

		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp
		db	aesc,aemp

		db	asht,ano 		; E
		db	asht,ano
		db	asht,ano
		db	asht,ano
		db	aal,aim8
		db	aax,aim8
		db	aim8,aal
		db	aim8,aax

		db	anear,ano
		db	anear,ano
		db	afar,ano
		db	asht,ano
		db	aal,adx
		db	aax,adx
		db	adx,aal
		db	adx,aax

		db	ano,ano 		; F
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	arm8,ano
		db	arm16,ano

		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	ano,ano
		db	arm8,ano
		db	arm16,ano

;~ifndef __ACT__
;~Buffer		db	BUF_SIZE dup (' ')
;~		db	-1
;~endif
;~
;~endif
;~
;~ifdef _CommandLength
;~
;~CommandLength	proc
;~		ret
;~endp
;~
;~LengthTable	label	byte
;~		db	-1, -1, -1, -1, -1, -1, 1, 1	; 0
;~		db	-1, -1, -1, -1, -1, -1, 1, 1
;~		db	-1, -1, -1, -1, -1, -1, 1, 1	; 1
;~		db	-1, -1, -1, -1, -1, -1, 1, 1
;~
;~		db	-1, -1, -1, -1, -1, -1, 0, 1	; 2
;~		db	-1, -1, -1, -1, -1, -1, 0, 1
;~		db	-1, -1, -1, -1, -1, -1, 0, 1	; 3
;~		db	-1, -1, -1, -1, -1, -1, 0, 1
;~
;~		db	1, 1, 1, 1, 1, 1, 1, 1		; 4
;~		db	1, 1, 1, 1, 1, 1, 1, 1
;~		db	1, 1, 1, 1, 1, 1, 1, 1		; 5
;~		db	1, 1, 1, 1, 1, 1, 1, 1
;~
;~		db	1, 1, 1, 1, 1, 1, 1, 1		; 6
;~		db	1, 1, 1, 1, 1, 1, 1, 1
;~
;~		db	2, 2, 2, 2, 2, 2, 2, 2		; 7
;~		db	2, 2, 2, 2, 2, 2, 2, 2
;~
;~		db	-2, -2, -2, -2, -1, -1, -1, -1	; 8
;~		db	-1, -1, -1, -1, -1, -1, -1, -1
;~
;~		db	1, 1, 1, 1, 1, 1, 1, 1		; 9
;~		db	1, 1, 5, 1, 1, 1, 1, 1
;~
;~		db	3, 3, 3, 3, 1, 1, 1, 1		; A
;~		db	-2, -2, 1, 1, 1, 1, 1, 1
;~
;~		db	2, 2, 2, 2, 2, 2, 2, 2		; B
;~		db	3, 3, 3, 3, 3, 3, 3, 3
;~
;~		db	1, 1, 3, 1, -1, -1, -1, -1	; C
;~		db	1, 1, 3, 1, 1, 2, 1, 1
;~
;~		db	-1, -1, -1, -1, 2, 2, 1, 1	; D
;~		db	-1, -1, -1, -1, -1, -1, -1, -1
;~
;~		db	2, 2, 2, 2, 2, 2, 2, 2		; E
;~		db	3, 3, 5, 2, 1, 1, 1, 1
;~
;~		db	0, 1, 0, 0, 1, 1, 81h, 81h	; F
;~		db	1, 1, 1, 1, 1, 1, 82h, 83h
;~
;~LengthTable2	label	byte
;~		db	1, 2, 3, 1
;~
;~endif

;=============================================================================
; E0F
;=============================================================================


