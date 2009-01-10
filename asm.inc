;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; asm.inc
; Assembler.
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


;=============================================================================
; assemble
;-----------------------------------------------------------------------------
;ds:si -> assembling line
;es:di -> result buffer
;bx    =  jump/call offset origin
assemble:
		cld
		mov	[asm_seg],es
		mov	[asm_offs],di
		mov	[jump_origin_offs],bx
		mov	[asm_sp],sp
		push	cs
		pop	es
		mov	byte [rep_pref],0
@@find_next:
		call	find_name

		cmp	bx,rep_prefixes
		jb	not_rep
		mov	al,[bx+5]
		mov	[rep_pref],al
		jmp	@@find_next
not_rep:
		call	upline

		cmp	byte [rep_pref],0
		je	@@no_rep
		cmp	byte [bx+6],-1
		je	@@no_rep
		jmp	rep_error
@@no_rep:
		mov	byte [jump_flag],0
		cmp	bx,jumps_and_calls
		jb	not_jumps
		mov	byte [jump_flag],1
not_jumps:
		mov	byte [param_count],0
		mov	byte [curr_prefix],-1
		mov	di,parameter_1
		call	get_parameter
		jc	..@no_params
		inc	byte [param_count]
		mov	di,parameter_2
		call	get_parameter
		jc	one_param
		inc	byte [param_count]
		mov	al,[parameter_1+AP_WORD_FLAG]
		mov	ah,[parameter_2+AP_WORD_FLAG]
;		or	ax,ax
;		jz	TypeWarning
		cmp	bx,diff_types
		jae	@@1
		cmp	al,0
		je	@@1
		cmp	ah,0
		je	@@1
		cmp	al,ah
		jne	reg_error
@@1:
		call	skip_spaces
		jz	one_param
		jmp	extra_error
one_param:
;		cmp	Parameter1.WordFlag,0
;		jz	TypeWarning
..@no_params:
		mov	di,asm_buffer
		mov	cl,[bx]
		lea	si,[bx+3]
		mov	ch,0
next_proc:
		push	cx
		mov	bl,[parameter_1+AP_MEM_FLAG]
		mov	bh,[parameter_2+AP_MEM_FLAG]
		mov	cl,[parameter_1+AP_REG_NUMBER]
		mov	ch,[parameter_2+AP_REG_NUMBER]
		lodsw
		xchg	ax,dx
		lodsw
		call	dx
		pop	cx
		loopne	next_proc
		jne	wrong_param

		mov	es,[asm_seg]
		mov	di,[asm_offs]
		mov	si,asm_buffer
		lodsb
		mov	cl,al
		mov	ch,0
		cmp	byte [rep_pref],0
		jz	@@ch_pr
		mov	al,[rep_pref]
		stosb
@@ch_pr:
		cmp	byte [curr_prefix],-1
		je	@@copy
;		cmp	Parameter1.MemFlag,0
;		jne	@@Mem1
;		cmp	Parameter2.MemFlag,0
;		je	@@Copy
;@@Mem1:
		mov	al,[curr_prefix]
		mov	bx,pref_codes
		xlat
		stosb
@@copy:
	rep	movsb
		clc
		ret

rep_error:
		mov	ax,ER_REP_ERROR
		jmp	asm_error
;TypeWarning:
;		mov	ax,erParamType
;		jmp	AsmError
reg_error:
		mov	ax,ER_REG_BITS
		jmp	asm_error
wrong_param:
		mov	ax,ER_INV_PARAM
		jmp	asm_error
mnem_error:
		mov	ax,ER_MNEMONIC
asm_error:				; SI=error place
		mov	sp,[asm_sp]	; AX=error code
		stc
		ret
;=============================================================================
asm_seg		dw	0
asm_offs	dw	0
asm_sp		dw	0
jump_origin_offs dw	0
;=============================================================================
; find_name
;-----------------------------------------------------------------------------
find_name:
		call	skip_spaces
		jz	no_name
		mov	di,name_buf
		mov	cx,MAX_LETTER
@@next_letter:
		lodsb
		call	test_space
		jz	end_name
		call	lowcase
		stosb
		loop	@@next_letter
		jmp	mnem_error
end_name:
		dec	si
		mov	al,0
		stosb
		push	si
		mov	cx,di
		mov	bx,asm_table
		mov	si,name_buf
		sub	cx,si
next_mnem:
		mov	di,[bx+1]
		push	cx
		push	si
	repe	cmpsb
		pop	si
		pop	cx
		je	@@found
		mov	al,[bx]
		cbw
		shl	ax,1
		shl	ax,1
		add	bx,ax
		add	bx,3
		or	ax,ax
		jne	next_mnem
		jmp	mnem_error
@@found:
;		mov	CurCmd,bx
		pop	si
		ret
no_name:
		mov	ax,ER_EMPTY
		jmp	asm_error
;=============================================================================
; get_parameter
;-----------------------------------------------------------------------------
get_parameter:
		call	skip_spaces
		jnz	@@1
		stc
		jmp	get_parameter_quit
@@1:
		xor	ax,ax
		mov	[di+AP_MEM_FLAG],al
		mov	[di+AP_WORD_FLAG],al
		mov	[di+AP_VALUE],ax
		mov	byte [di+AP_REG_NUMBER],-1
		mov	[minus_flag],al
..@next:
		mov	ax,[si]
		call	get_xptr
		jz	..@next
		cmp	al,'$'
		je	@@j$
		cmp	al,'['
		jne	@@skip
		jmp	..@mem_start		;jump out of range
@@skip:
		call	find_seg_reg
		jz	seg_reg_param
		call	find_reg
		je	reg_param		;jump out of range
		cmp	al,'-'
		jne	@@nm
		mov	byte [minus_flag],1
		inc	si
@@next_val:
		mov	ax,[si]
@@nm:
		call	test_hex
		jc	jump_to_wrong_param_1		;jump out of range
		call	get_value
@@add_offs:
		cmp	byte [minus_flag],0
		je	@@pl
		neg	dx
		mov	byte [minus_flag],0
@@pl:
		add	[di+AP_VALUE],dx
		lodsb
		cmp	al,'+'
		je	@@next_val
		cmp	al,'-'
		jne	..@emp
		mov	byte [minus_flag],1
		jmp	@@next_val

@@j$:

;		mov	dx,asm_offs
		mov	dx,[jump_origin_offs]

		inc	si
		jmp	@@add_offs

seg_reg_param:
		cmp	byte [si+2],':'
		je	..@prefix
		mov	dx,[bx+1]
		cmp	dx,cmov
		je	..@okp
		cmp	dx,cpush
		je	..@okp
		cmp	dx,cpop
		je	..@okp
jump_to_wrong_param_1:
		jmp	wrong_param

reg_param:
		mov	[di+AP_REG_NUMBER],al
		mov	dl,1
		test	al,1000b
		jz	byte_reg
		inc	dl
byte_reg:
		mov	[di+AP_WORD_FLAG],dl
..@check_extra:
		inc	si
		inc	si
		jmp	check_extra

..@emp:
		dec	si
		cmp	byte [jump_flag],0
		je	check_extra
		lodsb
		cmp	al,':'
		je	far_jump
		dec	si
check_extra:
		call	skip_spaces
		jz	@@jump_to_ok_quit	;jump out of range
		cmp	al,','
		jnz	extra_error		;jump out of range
		inc	si
@@jump_to_ok_quit:
		jmp	..@ok_quit

extra_error:
		mov	ax,ER_EXTRA
		jmp	asm_error

far_jump:
		mov	al,[si]
		call	test_hex
		jc	check_extra
		call	get_value
		mov	[far_value],dx
		mov	byte [jump_flag],2
		jmp	check_extra

..@okp:
		add	al,10h
		mov	[di+AP_REG_NUMBER],al
		mov	byte [di+AP_WORD_FLAG],2
		jmp	..@check_extra
..@prefix:
		cmp	byte [curr_prefix],-1
		jne	jump_to_wrong_param_1		;jump out of range
		mov	[curr_prefix],al
		add	si,3
		call	skip_spaces
		jmp	..@next

..@mem_start:
		mov	byte [di+AP_MEM_FLAG],1
		xor	ax,ax
		mov	[di+AP_BX_FLAG],ax	;Also clear [di+AP_BP_FLAG]
		mov	[di+AP_SI_FLAG],ax	;Also clear [di+AP_DI_FLAG]
		inc	si
..@plus:
next_mem_param:
		call	skip_spaces
		jz	jump_to_bad_mem_param		;jump out of range

		mov	ax,[si]
		cmp	al,'$'
		je	@@$
		call	find_reg
		je	mem_reg
..@get_value:
		call	test_hex
		jc	jump_to_bad_mem_param		;jump out of range
		call	get_value
		cmp	byte [minus_flag],0
		je	@@2
		neg	dx
		mov	byte [minus_flag],0
@@2:
		add	[di+AP_VALUE],dx
		jmp	..@3
@@$:

;		mov	dx,asm_offs
		mov	dx,[jump_origin_offs]

		inc	si
		jmp	@@2
mem_next:
		inc	si
		inc	si
..@3:
		call	skip_spaces
		jz	jump_to_bad_mem_param

		lodsb
		cmp	al,']'
		je	mem_end
		cmp	al,'+'
		je	..@plus
		cmp	al,'-'
		je	..@minus
jump_to_bad_mem_param:
		jmp	bad_mem_param
..@minus:
		mov	byte [minus_flag],1
		call	skip_spaces
		jnz	..@get_value
		jmp	bad_mem_param
mem_reg:
		cmp	al,regBX
		jne	@@bp
		cmp	word [di+AP_BX_FLAG],0
		jne	bad_mem_param
		inc	byte [di+AP_BX_FLAG]
		jmp	mem_next
@@bp:
		cmp	al,regBP
		jne	@@si
		cmp	word [di+AP_BX_FLAG],0
		jne	bad_mem_param
		inc	byte [di+AP_BP_FLAG]
		jmp	mem_next
@@si:
		cmp	al,regSI
		jne	@@di
		cmp	word [di+AP_SI_FLAG],0
		jne	bad_mem_param
		inc	byte [di+AP_SI_FLAG]
		jmp	mem_next
@@di:
		cmp	al,regDI
		jne	bad_mem_param
		cmp	word [di+AP_SI_FLAG],0
		jne	bad_mem_param
		inc	byte [di+AP_DI_FLAG]
		jmp	mem_next

mem_end:
		jmp	check_extra
..@ok_quit:
		clc
get_parameter_quit:
		ret
bad_mem_param:
		mov	ax,ER_MEM_PARAM
		jmp	asm_error
;=============================================================================
; get_xptr
;-----------------------------------------------------------------------------
get_xptr:
		cmp	byte [di+AP_WORD_FLAG],0
		jne	@@4
		cmp	al,'W'
		je	..@check_word
		cmp	byte [jump_flag],1
		je	jump_xptr
		cmp	al,'B'
		je	..@check_byte
@@4:
		ret
jump_xptr:
		cmp	ax,'FA'
		je	@@check_far
		cmp	ax,'DW'
		je	@@check_dw
		cmp	ax,'SH'
		je	@@check_short
		ret
@@check_short:
		lodsw
		lodsw
		cmp	ax,'OR'
		je	@@cs_1
		ret
@@cs_1:
		lodsb
		cmp	al,'T'
		je	@@check_ptr
		ret
@@check_far:
		lodsw
		lodsb
		mov	dl,3
		cmp	al,'R'
		je	@@check_ptr
		ret
@@check_dw:
		lodsw
		lodsw
		mov	dl,3
		cmp	ax,'OR'
		je	@@cd_2
		ret
@@cd_2:
		lodsb
		cmp	al,'D'
		je	@@check_ptr
..@check_byte:
		mov	dl,1
		cmp	ah,'/'
		je	@@ptr_1
		cmp	ah,'Y'
		je	@@check_byte_1
		ret
..@check_word:
		mov	dl,2
		cmp	ah,'/'
		je	@@ptr_1
		cmp	ah,'O'
		je	@@check_word_1
		ret
@@ptr_1:
		mov	[di+AP_WORD_FLAG],dl
		inc	si
		inc	si
..@get_xptr_quit:
		call	skip_spaces
		jz	jump_to_wrong_param_2
		cmp	ax,ax
		ret
@@check_byte_1:
		lodsw
		lodsw
		cmp	ax,'TE'
		je	@@check_ptr
		ret
@@check_word_1:
		lodsw
		lodsw
		cmp	ax,'RD'
		je	@@check_ptr
		ret
@@check_ptr:
		call	skip_spaces
		jnz	..@not_spc
jump_to_wrong_param_2:
		jmp	wrong_param
..@not_spc:
		cmp	al,'['
		je	@@ok
		cmp	al,'$'
		je	@@ok
		call	test_hex
		jnc	@@ok
		lodsw
		cmp	ax,'PT'
		jne	@@quit_2
		lodsb
		cmp	al,'R'
		je	@@ok
@@quit_2:
		ret
@@ok:
		mov	[di+AP_WORD_FLAG],dl
		jmp	..@get_xptr_quit
;=============================================================================
; find_seg_reg
;-----------------------------------------------------------------------------
find_seg_reg:
		push	di
		mov	di,asm_seg_regs
		mov	cx,4
	repne	scasw
		jne	@@quit
		lea	ax,[di-(asm_seg_regs-offset_0100h+102h)]
		shr	ax,1
		cmp	ax,ax
@@quit:
		pop	di
		ret
;=============================================================================

asm_seg_regs	db	'ESCSSSDS'

;=============================================================================
; find_reg
;-----------------------------------------------------------------------------
find_reg:
		push	di
		mov	di,asm_regs
		mov	cx,16
	repne	scasw
		jne	@@quit
		lea	ax,[di-(asm_regs-offset_0100h+102h)]
		shr	ax,1
		cmp	ax,ax
@@quit:
		pop	di
		ret
;=============================================================================

asm_regs	db	'ALCLDLBLAHCHDHBH'
		db	'AXCXDXBXSPBPSIDI'

;=============================================================================
; test_hex
;-----------------------------------------------------------------------------
test_hex:
		cmp	al,'0'
		jb	@@not
		cmp	al,'9'
		jbe	@@hex
		cmp	al,'A'
		jb	@@not
		cmp	al,'F'
		ja	@@not
@@hex:
		clc
		ret
@@not:
		stc
		ret
;=============================================================================
; get_value
;-----------------------------------------------------------------------------
get_value:
		xor	dx,dx
@@next:
		lodsb
		sub	al,'0'
		jb	@@end
		cmp	al,9
		jbe	@@digit
		cmp	al,'A'-'0'
		jb	@@end
		cmp	al,'F'-'0'
		ja	@@end
		sub	al,7
@@digit:
		test	dh,0f0h
		jnz	big_num_error
		shl	dx,1
		shl	dx,1
		shl	dx,1
		shl	dx,1
		or	dl,al
		jmp	@@next
@@end:
		dec	si
		ret
big_num_error:
		mov	ax,ER_BIG_NUMBER
		jmp	asm_error
;=============================================================================
; skip_spaces
;-----------------------------------------------------------------------------
skip_spaces:
@@next:
		lodsb
		cmp	al,' '
		je	@@next
		cmp	al,9
		je	@@next
		dec	si
		cmp	al,0
		je	@@ok
		cmp	al,';'
@@ok:
		ret
;=============================================================================
; test_space
;-----------------------------------------------------------------------------
test_space:
		cmp	al,' '
		je	@@ok
		cmp	al,9
		je	@@ok
		cmp	al,0
		je	@@ok
		cmp	al,';'
@@ok:
		ret
;=============================================================================


;~ifndef __ACT__
;~;------------------------------------------------------------------------------
;~UpCase:		;~proc
;~		cmp	al,'a'
;~		jb	@@1
;~		cmp	al,'z'
;~		ja	@@1
;~		sub	al,20h
;~@@1:
;~		ret
;~		;~endp
;~endif

;=============================================================================
asm_table:
		db	3
		dw	cadd
		dw	_r_rm
		db	0, 0
		dw	_a_im
		db	4, 0
		dw	_rm_im
		db	80h, 0

		db	3
		dw	cor
		dw	_r_rm
		db	8, 0
		dw	_a_im
		db	0Ch, 0
		dw	_rm_im
		db	80h, 1

		db	3
		dw	cadc
		dw	_r_rm
		db	10h, 0
		dw	_a_im
		db	14h, 0
		dw	_rm_im
		db	80h, 2

		db	3
		dw	csbb
		dw	_r_rm
		db	18h, 0
		dw	_a_im
		db	1Ch, 0
		dw	_rm_im
		db	80h, 3

		db	3
		dw	cand
		dw	_r_rm
		db	20h, 0
		dw	_a_im
		db	24h, 0
		dw	_rm_im
		db	80h, 4

		db	3
		dw	csub
		dw	_r_rm
		db	28h, 0
		dw	_a_im
		db	2Ch, 0
		dw	_rm_im
		db	80h, 5

		db	3
		dw	cxor
		dw	_r_rm
		db	30h, 0
		dw	_a_im
		db	34h, 0
		dw	_rm_im
		db	80h, 6

		db	3
		dw	ccmp
		dw	_r_rm
		db	38h, 0
		dw	_a_im
		db	3Ch, 0
		dw	_rm_im
		db	80h, 7

		db	3
		dw	ctest
		dw	_r_rm
		db	84h, 0
		dw	_a_im
		db	0A8h, 0
		dw	_rm_im
		db	0F6h, 0

		db	5
		dw	cmov
		dw	_sr_rm
		db	8Ch, 0
		dw	_r_im
		db	0B0h, 0
		dw	_a_mem
		db	0A0h, 0
		dw	_r_rm
		db	88h, 0
		dw	_rm_im1
		db	0C6h, 0

		db	3
		dw	cpush
		dw	_sreg
		db	6, 0
		dw	_reg16
		db	50h,0
		dw	_rm
		db	0FFh, 6

		db	3
		dw	cpop
		dw	_sreg
		db	7, 0
		dw	_reg16
		db	58h,0
		dw	_rm
		db	08Fh, 6

		db	1
		dw	clea
		dw	_r_m16
		db	8Dh, 0

		db	1
		dw	cles
		dw	_r_m16
		db	0C4h, 0

		db	1
		dw	clds
		dw	_r_m16
		db	0C5h, 0

		db	2
		dw	cinc
		dw	_reg16
		db	40h,0
		dw	_rm
		db	0FEh, 0

		db	2
		dw	cdec
		dw	_reg16
		db	48h,0
		dw	_rm
		db	0FEh, 1

		db	1
		dw	cmul
		dw	_rm
		db	0F6h, 100b

		db	1
		dw	cimul
		dw	_rm
		db	0F6h, 101b

		db	1
		dw	cdiv
		dw	_rm
		db	0F6h, 110b

		db	1
		dw	cidiv
		dw	_rm
		db	0F6h, 111b

		db	1
		dw	cneg
		dw	_rm
		db	0F6h, 011b

		db	1
		dw	cnot
		dw	_rm
		db	0F6h, 010b

		db	2
		dw	cxchg
		dw	_a_reg
		db	90h, 0
		dw	_r_rm
		db	86h, 0

		db	1
		dw	cesc
		dw	_esc
		db	0D8h, 0

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

		db	1
		dw	cdb
		dw	_db
		db	0, 0

		db	1
		dw	cdw
		dw	_dw
		db	0, 0

		db	1
		dw	csalc
		dw	_noparam
		db	0d6h,0

		db	2
		dw	caam
		dw	_im8
		db	0D4h, 0
		dw	_imn
		db	0D4h, 0Ah

		db	2
		dw	caad
		dw	_im8
		db	0D5h, 0
		dw	_imn
		db	0D5h, 0Ah

		db	1
		dw	cdaa
		dw	_noparam
		db	27h,0

		db	1
		dw	cdas
		dw	_noparam
		db	2Fh,0

		db	1
		dw	caaa
		dw	_noparam
		db	37h,0

		db	1
		dw	caas
		dw	_noparam
		db	3Fh,0

		db	1
		dw	cnop
		dw	_noparam
		db	90h,0

		db	1
		dw	ccbw
		dw	_noparam
		db	98h,0

		db	1
		dw	ccwd
		dw	_noparam
		db	99h,0

		db	1
		dw	cwait
		dw	_noparam
		db	9Bh,0

		db	1
		dw	cpushf
		dw	_noparam
		db	9Ch,0

		db	1
		dw	cpopf
		dw	_noparam
		db	9Dh,0

		db	1
		dw	csahf
		dw	_noparam
		db	9Eh,0

		db	1
		dw	clahf
		dw	_noparam
		db	9Fh,0

		db	1
		dw	cmovsb
		dw	_noparam
		db	0A4h,-1

		db	1
		dw	cmovsw
		dw	_noparam
		db	0A5h,-1

		db	1
		dw	ccmpsb
		dw	_noparam
		db	0A6h,-1

		db	1
		dw	ccmpsw
		dw	_noparam
		db	0A7h,-1

		db	1
		dw	cstosb
		dw	_noparam
		db	0AAh,-1

		db	1
		dw	cstosw
		dw	_noparam
		db	0ABh,-1

		db	1
		dw	clodsb
		dw	_noparam
		db	0ACh,-1

		db	1
		dw	clodsw
		dw	_noparam
		db	0ADh,-1

		db	1
		dw	cscasb
		dw	_noparam
		db	0AEh,-1

		db	1
		dw	cscasw
		dw	_noparam
		db	0AFh,-1

		db	2
		dw	cret
		dw	_im16
		db	0C2h, 0
		dw	_noparam
		db	0C3h,0

		db	2
		dw	cretn
		dw	_im16
		db	0C2h, 0
		dw	_noparam
		db	0C3h,0

		db	2
		dw	cretf
		dw	_im16
		db	0CAh, 0
		dw	_noparam
		db	0CBh,0

		db	1
		dw	cint3
		dw	_noparam
		db	0CCh,0

		db	1
		dw	cint
		dw	_im8ch3
		db	0CDh, 0

		db	1
		dw	cinto
		dw	_noparam
		db	0CEh,0

		db	1
		dw	ciret
		dw	_noparam
		db	0CFh,0

		db	1
		dw	cxlat
		dw	_noparam
		db	0D7h,0

		db	1
		dw	plock
		dw	_noparam
		db	0F0h,0

		db	1
		dw	chlt
		dw	_noparam
		db	0F4h,0

		db	1
		dw	ccmc
		dw	_noparam
		db	0F5h,0

		db	1
		dw	cclc
		dw	_noparam
		db	0F8h,0

		db	1
		dw	cstc
		dw	_noparam
		db	0F9h,0

		db	1
		dw	ccli
		dw	_noparam
		db	0FAh,0

		db	1
		dw	csti
		dw	_noparam
		db	0FBh,0

		db	1
		dw	ccld
		dw	_noparam
		db	0FCh,0

		db	1
		dw	cstd
		dw	_noparam
		db	0FDh,0

diff_types:
		db	1
		dw	cshl
		dw	_rm_cl_1
		db	0D0h, 100b

		db	1
		dw	csal
		dw	_rm_cl_1
		db	0D0h, 100b

		db	1
		dw	cshr
		dw	_rm_cl_1
		db	0D0h, 101b

		db	1
		dw	csar
		dw	_rm_cl_1
		db	0D0h, 111b

		db	1
		dw	crol
		dw	_rm_cl_1
		db	0D0h, 000b

		db	1
		dw	cror
		dw	_rm_cl_1
		db	0D0h, 001b

		db	1
		dw	crcl
		dw	_rm_cl_1
		db	0D0h, 010b

		db	1
		dw	crcr
		dw	_rm_cl_1
		db	0D0h, 011b


		db	2
		dw	cin
		dw	_a_dx
		db	0ECh, 0
		dw	_a_im8
		db	0E4h, 0

		db	2
		dw	cout
		dw	_dx_a
		db	0EEh, 0
		dw	_im8_a
		db	0E6h, 0

jumps_and_calls:
		db	4
		dw	cjmp
		dw	_short
		db	0EBh, 0
		dw	_near
		db	0E9h, 0
		dw	_far
		db	0EAh, 0
		dw	_rmj
		db	0FFh, 100b

		db	3
		dw	ccall
		dw	_near
		db	0E8h, 0
		dw	_far
		db	9Ah, 0
		dw	_rmj
		db	0FFh, 010b

		db	1
		dw	cjo
		dw	_short
		db	070h, 0

		db	1
		dw	cjno
		dw	_short
		db	071h, 0

		db	1
		dw	cjb
		dw	_short
		db	072h, 0

		db	1
		dw	cjc
		dw	_short
		db	072h, 0

		db	1
		dw	cjnae
		dw	_short
		db	072h, 0

		db	1
		dw	cjnb
		dw	_short
		db	073h, 0

		db	1
		dw	cjnc
		dw	_short
		db	073h, 0

		db	1
		dw	cjae
		dw	_short
		db	073h, 0

		db	1
		dw	cje
		dw	_short
		db	074h, 0

		db	1
		dw	cjz
		dw	_short
		db	074h, 0


		db	1
		dw	cjne
		dw	_short
		db	075h, 0

		db	1
		dw	cjnz
		dw	_short
		db	075h, 0

		db	1
		dw	cjbe
		dw	_short
		db	076h, 0

		db	1
		dw	cjna
		dw	_short
		db	076h, 0

		db	1
		dw	cjnbe
		dw	_short
		db	077h, 0

		db	1
		dw	cja
		dw	_short
		db	077h, 0


		db	1
		dw	cjs
		dw	_short
		db	078h, 0

		db	1
		dw	cjns
		dw	_short
		db	079h, 0

		db	1
		dw	cjp
		dw	_short
		db	07Ah, 0

		db	1
		dw	cjpe
		dw	_short
		db	07Ah, 0

		db	1
		dw	cjnp
		dw	_short
		db	07Bh, 0

		db	1
		dw	cjpo
		dw	_short
		db	07Bh, 0

		db	1
		dw	cjl
		dw	_short
		db	07Ch, 0

		db	1
		dw	cjnge
		dw	_short
		db	07Ch, 0

		db	1
		dw	cjnl
		dw	_short
		db	07Dh, 0

		db	1
		dw	cjge
		dw	_short
		db	07Dh, 0

		db	1
		dw	cjle
		dw	_short
		db	07Eh, 0

		db	1
		dw	cjng
		dw	_short
		db	07Eh, 0

		db	1
		dw	cjnle
		dw	_short
		db	07Fh, 0

		db	1
		dw	cjg
		dw	_short
		db	07Fh, 0

		db	1
		dw	cloop
		dw	_short
		db	0E2h, 0

		db	1
		dw	cloopne
		dw	_short
		db	0E0h, 0

		db	1
		dw	cloopnz
		dw	_short
		db	0E0h, 0

		db	1
		dw	cloope
		dw	_short
		db	0E1h, 0

		db	1
		dw	cloopz
		dw	_short
		db	0E1h, 0

		db	1
		dw	cjcxz
		dw	_short
		db	0E3h, 0

rep_prefixes:
		db	1
		dw	crep
		dw	0
		db	0F3h, 0

		db	1
		dw	crepe
		dw	0
		db	0F3h, 0

		db	1
		dw	crepz
		dw	0
		db	0F3h, 0

		db	1
		dw	crepne
		dw	0
		db	0F2h, 0

		db	1
		dw	crepnz
		dw	0
		db	0F2h, 0

;~comment #
;~Asm286table	;~label	byte
;~		db	1
;~		dw	cpusha
;~		dw	_noparam
;~		db	60h, 0
;~
;~		db	1
;~		dw	cpopa
;~		dw	_noparam
;~		db	61h, 0
;~
;~		db	1
;~		dw	cbound
;~		dw	_r_m16
;~		db	62h, 0
;~
;~		db	1
;~		dw	carpl
;~		dw	_rm_r16
;~#
		db	0		; End of table


;=============================================================================
; _r_m16
;-----------------------------------------------------------------------------
_r_m16:
		cmp	bx,0100h
		jne	@@quit
		cmp	cl,-1
		je	@@quit
		test	cl,1000b
		jz	@@quit
		mov	byte [lxx_flag],1
		jmp	_r_rm2
@@quit:
		or	sp,sp
		ret
;=============================================================================
; _r_rm
;-----------------------------------------------------------------------------
_r_rm:
		mov	byte [lxx_flag],0
_r_rm2:
		cmp	byte [param_count],2
		jne	..@_r_rm_q		;jump out of range
		cmp	bx,0001h
		je	..@_r_rm_m_r
		cmp	bx,0100h
		je	..@_r_rm_r_m
		or	bx,bx
		jne	..@_r_rm_q
@@r_r:
		cmp	cl,-1
		je	..@_r_rm_q
		cmp	ch,-1
		je	..@_r_rm_q
		mov	byte [di],2
		cmp	byte [lxx_flag],1
		je	@@l1
		call	add_word_flag
@@l1:
		mov	[di+1],al
		call	make_modrm
;;		mov	al,cl
;		and	al,0111b
;		or	al,11000000b
		and	ch,0111b
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		mov	[di+2],al
		jmp	..@_r_rm_ok

..@_r_rm_q:
exit_zf_clear:
		or	sp,sp
		ret
..@_r_rm_ok:
		cmp	ax,ax
		ret

..@_r_rm_m_r:
		cmp	ch,-1
		je	..@_r_rm_q
@@make_modrm:
		cmp	byte [lxx_flag],1
		je	@@l2
		call	add_word_flag
@@l2:
		mov	[di+1],al

		call	make_modrm

		mov	dx,[parameter_1+AP_VALUE]
		add	ah,2
		mov	[di],ah
@@set_reg:
		mov	[di+3],dx
		mov	ah,[parameter_2+AP_REG_NUMBER]
		and	ah,0111b
		shl	ah,1
		shl	ah,1
		shl	ah,1
		or	al,ah
		mov	[di+2],al
		jmp	..@_r_rm_ok

..@_r_rm_r_m:
		cmp	cl,-1
		je	..@_r_rm_q
		cmp	byte [lxx_flag],1
		je	@@7
		or	al,2
@@7:
		call	swap_parameters
		xchg	cl,ch
		jmp	@@make_modrm
;=============================================================================
; _a_im
;-----------------------------------------------------------------------------
_a_im:
		cmp	byte [param_count],2
		jne	@@quit
		cmp	cl,0ffh
		je	exit_zf_clear
		and	cl,0111b
		cmp	cl,regAL
		jne	@@quit
		cmp	bh,0
		jne	@@quit
		call	add_word_flag
		mov	dx,[parameter_2+AP_VALUE]
		or	ah,ah
		jnz	@@1
		call	check_byte
		jz	@@1
		or	dh,dh
		jnz	jump_to_big_num_error		;jump out of range
@@1:
		add	ah,2
		mov	[di],ah
		mov	[di+1],al
		mov	[di+2],dx
		cmp	ax,ax
@@quit:
		ret
;=============================================================================
; _rm_im1
;-----------------------------------------------------------------------------
_rm_im1:
		mov	byte [mov_flag],1
		jmp	_rm_im2
;=============================================================================
; _db
;-----------------------------------------------------------------------------
_db:
		cmp	byte [param_count],1
		jne	@@quit
		or	bl,bl
		jnz	@@quit
		cmp	cl,-1
		jne	@@quit
		mov	ax,[parameter_1+AP_VALUE]
		or	ah,ah
		jnz	jump_to_big_num_error		;jump out of range
		mov	byte [di],1
		mov	[di+1],al
@@quit:
		ret
;=============================================================================
; _rm_im
;-----------------------------------------------------------------------------
_rm_im:
		mov	byte [mov_flag],0
_rm_im2:
		cmp	byte [param_count],2
		jne	@@quit
		cmp	ch,0ffh
		jne	@@quit
		cmp	bx,0001h
		je	@@m_i
		or	bx,bx
		jnz	@@quit
@@r_i:
		mov	ch,ah
		call	add_word_flag
		jmp	@@make
..@_rm_im_ok:
		cmp	ax,ax
@@quit:
		ret
@@m_i:
		mov	ch,ah
;		or	al,1		;; WordFlag:=1
@@make:
		mov	dx,[parameter_2+AP_VALUE]
		mov	bl,[parameter_1+AP_WORD_FLAG]
		mov	cl,4
		call	check_byte
		jz	..@byte_imm
		or	dh,dh
		jz	..@byte_imm_2
		or	al,1
		cmp	bl,1
		jne	..@word_imm
jump_to_big_num_error:
		jmp	big_num_error
..@byte_imm_2:
		dec	cl
		cmp	bl,2
		jb	@@byte
		inc	cl
		or	al,1
		jmp	..@word_imm
..@byte_imm:
		dec	cl
		cmp	bl,2
		jb	@@byte
		or	al,3
		cmp	byte [mov_flag],0
		je	@@byte
		inc	cl
@@byte:
..@word_imm:
		mov	[di+1],al
		call	make_modrm	; al=md000r/m, ah=size of offset(0,1,2)
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		add	cl,ah
		mov	[di],cl
		mov	[di+2],al
		mov	bl,ah
		mov	bh,0
		mov	cx,[parameter_1+AP_VALUE]
		mov	[di+3],cx
		mov	[di+bx+3],dx
		jmp	..@_rm_im_ok
;=============================================================================
; _esc
;-----------------------------------------------------------------------------
_esc:
		cmp	byte [param_count],2
		jne	@@quit
		or	bl,bl
		jnz	@@quit
		cmp	cl,-1
		jne	@@quit
		mov	bl,ch
		cmp	bx,00ffh
		je	@@quit
		xchg	cl,ch
		call	swap_parameters
		call	make_modrm
		add	ah,2
		mov	[di],ah
		mov	dx,[parameter_2+AP_VALUE]
		cmp	dx,64
		jae	jump_to_big_num_error		;jump out of range
		mov	dh,dl
		and	dh,00111000b
		or	al,dh
		mov	[di+2],al
		mov	al,dl
		and	al,0111b
		or	al,0D8h
		mov	[di+1],al
		mov	dx,[parameter_1+AP_VALUE]
		mov	[di+3],dx
		cmp	ax,ax
		ret
@@quit:
		or	sp,sp
		ret
;=============================================================================
; _a_mem
;-----------------------------------------------------------------------------
_a_mem:
		cmp	byte [param_count],2
		jne	@@quit
		cmp	bx,0001h
		je	@@m_a
		cmp	bx,0100h
		jne	@@quit
		and	cl,0111b
		cmp	cl,regAL
		jne	@@quit
		cmp	word [parameter_2+AP_BX_FLAG],0
		jne	@@quit
		cmp	word [parameter_2+AP_SI_FLAG],0
		jne	@@quit
		mov	dx,[parameter_2+AP_VALUE]
@@make:
		call	add_word_flag
		mov	[di+1],al
		mov	byte [di],3
		mov	[di+2],dx
		cmp	ax,ax
		ret
@@m_a:
		and	ch,0111b
		cmp	ch,regAL
		jne	@@quit
		cmp	word [parameter_1+AP_BX_FLAG],0
		jne	@@quit
		cmp	word [parameter_1+AP_SI_FLAG],0
		jne	@@quit
		or	al,0010b
		mov	dx,[parameter_1+AP_VALUE]
		jmp	@@make
@@quit:
		ret
;=============================================================================
; _r_im
;-----------------------------------------------------------------------------
_r_im:
		cmp	byte [param_count],2
		jne	@@quit
		or	bx,bx
		jnz	@@not
		cmp	cl,-1
		jz	@@not
		cmp	ch,-1
		jnz	@@not
		or	al,cl
		mov	ah,3
		mov	dx,[parameter_2+AP_VALUE]
		test	al,1000b
		jnz	@@word
		dec	ah
		call	check_byte
		jz	@@word
		or	dh,dh
		jz	@@word
		jmp	big_num_error
@@word:
		mov	[di],ah
		mov	[di+1],al
		mov	[di+2],dx
		cmp	ax,ax
		ret
@@not:
		or	sp,sp
@@quit:
		ret
;=============================================================================
; _sr_rm
;-----------------------------------------------------------------------------
_sr_rm:
		cmp	byte [param_count],2
		jne	@@quit
		cmp	bx,0001h
		je	@@m_r
		cmp	bx,0100h
		je	@@r_m
		or	bx,bx
		jnz	@@quit
@@r_r:
		cmp	cl,-1
		je	@@quit
		cmp	ch,-1
		je	@@quit
		cmp	cl,10h
		jae	@@5
		cmp	ch,10h
		jae	@@m_s
		jmp	@@quit
@@5:
		cmp	ch,10h
		jb	@@s_m
		jmp	wrong_param
@@m_r:
		cmp	ch,-1
		je	@@quit
		cmp	ch,10h
		jb	@@quit
@@m_s:
		mov	[di+1],al
		call	make_modrm
		add	ah,2
		mov	[di],ah
		and	ch,0111b
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		mov	[di+2],al
		mov	dx,[parameter_1+AP_VALUE]
		mov	[di+3],dx

		cmp	ax,ax
		ret

@@r_m:
		cmp	cl,-1
		je	@@quit
		cmp	cl,10h
		jb	@@quit
@@s_m:
		or	al,2
		xchg	cl,ch
		call	swap_parameters
		jmp	@@m_s
@@quit:
		or	sp,sp
		ret
;=============================================================================
; _rm
;-----------------------------------------------------------------------------
_rm:
		cmp	byte [param_count],1
		jne	@@quit
		cmp	bl,1
		je	@@m
		cmp	cl,-1
		je	@@quit
@@m:
		mov	ch,ah
		call	add_word_flag
		mov	[di+1],al
		call	make_modrm
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		mov	[di+2],al
		add	ah,2
		mov	[di],ah
		mov	ax,[parameter_1+AP_VALUE]
		mov	[di+3],ax
		cmp	ax,ax
		ret
@@quit:
		or	sp,sp
		ret
;=============================================================================
; _a_reg
;-----------------------------------------------------------------------------
_a_reg:
		cmp	byte [param_count],2
		jne	@@quit
		or	bx,bx
		jnz	@@quit
		cmp	cl,regAX
		je	@@a_r
		cmp	ch,regAX
		je	@@r_a
@@quit:
		call	swap_parameters	;; Only for XCHG
		or	sp,sp
		ret
@@a_r:
		cmp	ch,-1
		je	@@quit
		jmp	@@ax_r
@@r_a:
		cmp	cl,-1
		je	@@quit
		xchg	cl,ch
@@ax_r:
		and	ch,0111b
		or	al,ch
		mov	byte [di],1
		mov	[di+1],al
		cmp	ax,ax
		ret
;=============================================================================
; _reg16
;-----------------------------------------------------------------------------
_reg16:
		cmp	byte [param_count],1
		jne	@@quit
		cmp	cl,-1
		je	@@quit
		test	cl,1000b
		jz	@@quit
		and	cl,0111b
		or	al,cl
		mov	byte [di],1
		mov	[di+1],al
		cmp	sp,sp
		ret
@@quit:
		or	sp,sp
		ret
;=============================================================================
; _sreg
;-----------------------------------------------------------------------------
_sreg:
		cmp	byte [param_count],1
		jne	@@quit
		cmp	cl,-1
		je	@@quit
		cmp	cl,10h
		jb	@@quit
		and	cl,0111b
		shl	cl,1
		shl	cl,1
		shl	cl,1
		or	al,cl
		mov	byte [di],1
		mov	[di+1],al
		cmp	ax,ax
		ret
@@quit:
		or	sp,sp
		ret
;=============================================================================
; _rm_cl_1
;-----------------------------------------------------------------------------
_rm_cl_1:
		cmp	bh,1
		je	@@quit
		cmp	ch,-1
		je	@@check_1
		cmp	ch,regCL
		jne	@@quit
		mov	cl,10b
		jmp	@@make
@@check_1:
		cmp	word [parameter_2+AP_VALUE],1
		jne	@@quit
		mov	cl,0
@@make:
		mov	ch,ah
		call	add_word_flag
		or	al,cl
		mov	[di+1],al
		call	make_modrm
		add	ah,2
		mov	[di],ah
		and	ch,0111b
		shl	ch,1
		shl	ch,1
		shl	ch,1
		or	al,ch
		mov	[di+2],al
		mov	dx,[parameter_1+AP_VALUE]
		mov	[di+3],dx
		cmp	ax,ax
		ret
@@quit:
		or	sp,sp
		ret
;=============================================================================
; _im8ch3
;-----------------------------------------------------------------------------
_im8ch3:
		call	_im8
		jnz	@@quit
		cmp	byte [di+2],3
		jne	@@quit_1
		mov	word [di],0cc01h
@@quit_1:
		cmp	ax,ax
@@quit:
		ret
;=============================================================================
; _im8
;-----------------------------------------------------------------------------
_im8:
		cmp	byte [param_count],1
		jne	@@quit
		cmp	bl,0
		jne	@@quit
		cmp	cl,-1
		jne	@@quit
		mov	dx,[parameter_1+AP_VALUE]
		cmp	dh,0
		jnz	@@quit
		mov	byte [di],2
		mov	[di+1],al
		mov	[di+2],dl
@@quit:
		ret
;=============================================================================
; _im16
;-----------------------------------------------------------------------------
_im16:
		cmp	byte [param_count],1
		jne	@@quit
		cmp	bl,0
		jne	@@quit
		cmp	cl,-1
		jne	@@quit
		mov	dx,[parameter_1+AP_VALUE]
		mov	byte [di],3
		mov	[di+1],al
		mov	[di+2],dx
@@quit:
		ret
;=============================================================================
; _imn
;-----------------------------------------------------------------------------
_imn:
		cmp	byte [param_count],0
		jne	@@quit
		mov	byte [di],2
		mov	[di+1],ax
@@quit:
		ret
;=============================================================================
; _a_dx
;-----------------------------------------------------------------------------
_a_dx:
		cmp	byte [param_count],2
		jne	@@quit
		test	cl,1000b
		jz	@@1
		or	al,1
@@1:
		and	cl,0111b
		cmp	cx,regAL + (regDX << 8)
		jne	@@quit
		mov	byte [di],1
		mov	[di+1],al
@@quit:
		ret
;=============================================================================
; _dx_a
;-----------------------------------------------------------------------------
_dx_a:
		cmp	byte [param_count],2
		jne	@@quit
		test	ch,1000b
		jz	@@1
		or	al,1
@@1:
		and	ch,0111b
		cmp	cx,regDX + (regAL << 8)
		jne	@@quit
		mov	byte [di],1
		mov	[di+1],al
@@quit:
		ret
;=============================================================================
; _a_im8
;-----------------------------------------------------------------------------
_a_im8:
		cmp	byte [param_count],2
		jne	@@quit
		or	bx,bx
		jnz	@@quit
		cmp	ch,-1
		jne	@@quit
		test	cl,1000b
		jz	@@1
		or	al,1
@@1:
		and	cl,0111b
		cmp	cl,regAL
		jne	@@quit
		mov	dx,[parameter_2+AP_VALUE]
		or	dh,dh
		jnz	@@quit
		mov	byte [di],2
		mov	[di+1],al
		mov	[di+2],dl
@@quit:
		ret
;=============================================================================
; _im8_a
;-----------------------------------------------------------------------------
_im8_a:
		cmp	byte [param_count],2
		jne	@@quit
		or	bx,bx
		jnz	@@quit
		cmp	cl,-1
		jne	@@quit
		test	ch,1000b
		jz	@@1
		or	al,1
@@1:
		and	ch,0111b
		cmp	ch,regAL
		jne	@@quit
		mov	dx,[parameter_1+AP_VALUE]
		or	dh,dh
		jnz	@@quit
		mov	byte [di],2
		mov	[di+1],al
		mov	[di+2],dl
@@quit:
		ret
;=============================================================================
; _short
;-----------------------------------------------------------------------------
_short:
		cmp	byte [jump_flag],1
		jne	@@quit
		cmp	byte [param_count],1
		jne	@@quit
		or	bl,bl
		jnz	@@quit
		cmp	cl,-1
		jnz	@@quit
		mov	dx,[parameter_1+AP_VALUE]

;		sub	dx,asm_offs
		sub	dx,[jump_origin_offs]

		sub	dx,2
		call	check_byte
		jnz	@@quit
		mov	byte [di],2
		mov	[di+1],al
		mov	[di+2],dl
@@quit:
		ret
;=============================================================================
; _near
;-----------------------------------------------------------------------------
_near:
		cmp	byte [jump_flag],1
		jne	@@quit
		cmp	byte [param_count],1
		jne	@@quit
		or	bl,bl
		jnz	@@quit
		cmp	cl,-1
		jnz	@@quit
		mov	dx,[parameter_1+AP_VALUE]

;		sub	dx,asm_offs
		sub	dx,[jump_origin_offs]

		sub	dx,3
		mov	byte [di],3
		mov	[di+1],al
		mov	[di+2],dx
		cmp	ax,ax
@@quit:
		ret
;=============================================================================
; _far
;-----------------------------------------------------------------------------
_far:
		cmp	byte [param_count],1
		jne	@@quit
		cmp	byte [jump_flag],2
		jne	@@quit
		mov	dx,[parameter_1+AP_VALUE]
		mov	cx,[far_value]
		mov	byte [di],5
		mov	[di+1],al
		mov	[di+2],cx
		mov	[di+4],dx
@@quit:
		ret
;=============================================================================
; _rmj
;-----------------------------------------------------------------------------
_rmj:
		cmp	byte [parameter_1+AP_WORD_FLAG],3
		jne	loc_rm
		inc	ah
loc_rm:
		jmp	_rm
;=============================================================================
; _dw
;-----------------------------------------------------------------------------
_dw:
		cmp	byte [param_count],1
		jne	@@quit
		or	bl,bl
		jnz	@@quit
		cmp	cl,-1
		jne	@@quit
		mov	ax,[parameter_1+AP_VALUE]
		mov	byte [di],2
		mov	[di+1],ax
@@quit:
		ret
;=============================================================================
; _noparam
;-----------------------------------------------------------------------------
_noparam:
		cmp	byte [param_count],0
		jne	@@quit
		mov	byte [di],1
		mov	[di+1],al
@@quit:
		ret
;=============================================================================
; add_word_flag
;-----------------------------------------------------------------------------
add_word_flag:
		mov	ah,[parameter_1+AP_WORD_FLAG]
		or	ah,ah
		jz	@@2
@@1:
		dec	ah
		or	al,ah
		ret
@@2:
		cmp	byte [param_count],1
		jbe	@@3
		mov	ah,[parameter_2+AP_WORD_FLAG]
		or	ah,ah
		jnz	@@1
@@3:
		ret
;=============================================================================
; swap_parameters
;-----------------------------------------------------------------------------
swap_parameters:
		push	ax
		push	cx
		push	si
		push	di
		mov	si,parameter_1
		mov	di,parameter_2
		mov	cx,SIZEOF_ASM_PARAM
@@next:
		lodsb
		xchg	al,[es:di]
		mov	[si-1],al
		inc	di
		loop	@@next
		pop	di
		pop	si
		pop	cx
		pop	ax
		ret
;=============================================================================
; check_byte
;-----------------------------------------------------------------------------
check_byte:
		push	ax
		mov	al,dl
		cbw
		cmp	ax,dx
		pop	ax
		ret
;=============================================================================
; 
;-----------------------------------------------------------------------------
; Makes MODRM byte for 1st parameter
; Returns: al=md000r/m, ah=offset size (0, 1 or 2)
make_modrm:
		push	bx
		push	cx
		push	dx
		mov	al,[parameter_1+AP_REG_NUMBER]
		cmp	al,-1
		je	@@mem
		and	al,0111b
		or	al,11000000b
;;		and	ch,0111b
;		shl	ch,1
;		shl	ch,1
;		shl	ch,1
;		or	al,ch
		mov	ah,0
		jmp	@@ok
@@mem:
		cmp	byte [parameter_1+AP_MEM_FLAG],0
		je	@@error
@@make_modrm:
		mov	bx,[parameter_1+AP_BX_FLAG]
		mov	cx,[parameter_1+AP_SI_FLAG]
		mov	dx,[parameter_1+AP_VALUE]
		push	bx
		or	bx,cx
		pop	bx
		jz	@@no_base_index
		or	bx,bx
		jz	@@no_base
		or	cx,cx
		jz	@@no_index
		mov	al,bh
		add	al,bh
		add	al,ch
		jmp	@@make_mod
@@no_base:
		mov	al,4
		add	al,ch
		jmp	@@make_mod
@@no_index:
		mov	al,6
		add	al,bl
		cmp	bh,1
		jne	@@make_mod
		mov	bl,2
		jmp	@@ch_byte
@@no_base_index:
		mov	al,0110b
		mov	ah,2
		jmp	@@ok
@@make_mod:
		mov	bl,2
		or	dx,dx
		jz	@@no_offs
@@ch_byte:
		call	check_byte
		jz	@@byte_offs
@@word_offs:
		inc	bl
@@byte_offs:
		inc	bl
@@no_offs:
		mov	[di],bl
		sub	bl,2
		mov	ah,bl
		ror	bl,1
		ror	bl,1
		or	al,bl
@@ok:
		clc
@@quit:
		pop	dx
		pop	cx
		pop	bx
		ret
@@error:
		stc
		jmp	@@quit
;=============================================================================

pref_codes	db	26h, 2Eh, 36h, 3Eh

cretn		db	'retn',0
csal		db	'sal',0
cjnae		db	'jnae',0
cjc		db	'jc',0
cjnc		db	'jnc',0
cjae		db	'jae',0
cje		db	'je',0
cjne		db	'jne',0
cjna		db	'jna',0
cjnbe		db	'jnbe',0
cjpe		db	'jpe',0
cjpo		db	'jpo',0
cjnge		db	'jnge',0
cjnl		db	'jnl',0
cjng		db	'jng',0
cjnle		db	'jnle',0
cloopz		db	'loopz',0
cloopnz		db	'loopnz',0
crepz		db	'repz',0
crepnz		db	'repnz',0
crepe		db	'repe',0
crepne		db	'repne',0
crep		db	'rep',0
cdw		db	'dw',0

curr_prefix	db	-1

;~Parameter1	AsmParam	<>
;~Parameter2	AsmParam	<>
parameter_1	db	-1, 0, 0, 0, 0, 0, 0, 0, 0
parameter_2	db	-1, 0, 0, 0, 0, 0, 0, 0, 0

;=============================================================================
; E0F
;=============================================================================


