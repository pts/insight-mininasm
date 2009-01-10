;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; follow.inc
; ...
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
; dump_reference
;-----------------------------------------------------------------------------
dump_reference:
		call	unasm_cur_line
		cmp	byte [memory_flag],0
		je	@@exit
		call	calc_mem_address
		push	si
		push	dx
		call	update_dump_window
@@exit:
		clc
		ret
;=============================================================================
; dump_cur_line
;-----------------------------------------------------------------------------
dump_cur_line:
		push	word [cur_line_ip]
		push	word [unasm_seg]
		call	update_dump_window
;=============================================================================
; dump_user_screen
;-----------------------------------------------------------------------------
dump_user_screen:
		mov	ax,old_screen
		push	ax
		push	cs
		call	update_dump_window
;=============================================================================
; dump_esdi
;-----------------------------------------------------------------------------
dump_esdi:
		push	word [cpu_regs+REG_DI]
		push	word [cpu_regs+REG_ES]
		call	update_dump_window
;=============================================================================
; dump_dssi
;-----------------------------------------------------------------------------
dump_dssi:
		push	word [cpu_regs+REG_SI]
		push	word [cpu_regs+REG_DS]
		call	update_dump_window
;=============================================================================
; dump_esbx
;-----------------------------------------------------------------------------
dump_esbx:
		push	word [cpu_regs+REG_BX]
		push	word [cpu_regs+REG_ES]
		call	update_dump_window
;=============================================================================
; dump_ssbp
;-----------------------------------------------------------------------------
dump_ssbp:
		push	word [cpu_regs+REG_BP]
		push	word [cpu_regs+REG_SS]
		call	update_dump_window
;=============================================================================
; dump_dxax
;-----------------------------------------------------------------------------
dump_dxax:
		push	word [cpu_regs+REG_AX]
		push	word [cpu_regs+REG_DX]
		call	update_dump_window
;=============================================================================
; dump_dsdx
;-----------------------------------------------------------------------------
dump_dsdx:
		push	word [cpu_regs+REG_DX]
		push	word [cpu_regs+REG_DS]
		call	update_dump_window
;=============================================================================
; dump_essi
;-----------------------------------------------------------------------------
dump_essi:
		push	word [cpu_regs+REG_SI]
		push	word [cpu_regs+REG_ES]
		call	update_dump_window
;=============================================================================
; dump_dsdi
;-----------------------------------------------------------------------------
dump_dsdi:
		push	word [cpu_regs+REG_DI]
		push	word [cpu_regs+REG_DS]
		call	update_dump_window
;=============================================================================
; dump_csip
;-----------------------------------------------------------------------------
dump_csip:
		push	word [cpu_regs+REG_IP]
		push	word [cpu_regs+REG_CS]
		call	update_dump_window
;=============================================================================
; update_dump_window
;-----------------------------------------------------------------------------
update_dump_window:
		pop	word [dump_proc]
		pop	word [new_dump_seg]
		pop	word [new_dump_offs]

		call	pushr

		mov	ax,[dump_proc]
		mov	bx,update_dump_window-dump_csip
		sub	ax,bx
		cmp	ax,dump_esdi
		jb	@@set_dump
		mov	[data_watch_temp],ax
		sub	ax,dump_esdi
		div	bl
		mov	bl,4
		mul	bl
		add	ax,data_watch_labels
		mov	[data_watch_label],ax

@@set_dump:
		mov	ax,[new_dump_offs]
		mov	dx,[new_dump_seg]

;		cmp     dump_offs,ax
;		jne	@@UpdateDump
;		cmp	dump_seg,dx
;		je	@@Exit

@@update_dump:
		mov	[dump_offs],ax
		mov	[dump_seg],dx
		call	update_dump

@@exit:
		call	popr
		clc
		ret
;=============================================================================
; set_code_mark_[0-9]
;-----------------------------------------------------------------------------
set_code_mark_0:
		call	set_code_mark

set_code_mark_1:
		call	set_code_mark

set_code_mark_2:
		call	set_code_mark

set_code_mark_3:
		call	set_code_mark

set_code_mark_4:
		call	set_code_mark

set_code_mark_5:
		call	set_code_mark

set_code_mark_6:
		call	set_code_mark

set_code_mark_7:
		call	set_code_mark

set_code_mark_8:
		call	set_code_mark

set_code_mark_9:
		call	set_code_mark
;=============================================================================
; set_code_mark
;-----------------------------------------------------------------------------
set_code_mark:
		pop	di
		mov	bx,set_code_mark_1
		call	find_code_mark
		or	[code_mark_exist],ax
		call	check_shift
		jnz	@@set_data_mark

		mov	ax,[cur_line]
		stosw
		mov	ax,[cur_ip]
		stosw
		mov	ax,[unasm_seg]
		stosw
		mov	ax,[cur_line_ip]
		stosw
		jmp	update_commands

@@set_data_mark:
		mov	ax,-1
		stosw
		mov	ax,[dump_offs]
		stosw
		mov	ax,[dump_seg]
		stosw
		jmp	update_commands
;=============================================================================
; clear_code_mark_[0-9]
;-----------------------------------------------------------------------------
clear_code_mark_0:
		call	clear_code_mark

clear_code_mark_1:
		call	clear_code_mark

clear_code_mark_2:
		call	clear_code_mark

clear_code_mark_3:
		call	clear_code_mark

clear_code_mark_4:
		call	clear_code_mark

clear_code_mark_5:
		call	clear_code_mark

clear_code_mark_6:
		call	clear_code_mark

clear_code_mark_7:
		call	clear_code_mark

clear_code_mark_8:
		call	clear_code_mark

clear_code_mark_9:
		call	clear_code_mark
;=============================================================================
; clear_code_mark
;-----------------------------------------------------------------------------
clear_code_mark:
		pop	di
		mov	bx,clear_code_mark_1
		call	find_code_mark
		mov	si,di
		jz	jump_beep_return
		not	ax
		and	[code_mark_exist],ax
		jmp	update_commands
;=============================================================================
; code_mark_[0-9]
;-----------------------------------------------------------------------------
code_mark_0:
		call	code_mark

code_mark_1:
		call	code_mark

code_mark_2:
		call	code_mark

code_mark_3:
		call	code_mark

code_mark_4:
		call	code_mark

code_mark_5:
		call	code_mark

code_mark_6:
		call	code_mark

code_mark_7:
		call	code_mark

code_mark_8:
		call	code_mark

code_mark_9:
		call	code_mark
;=============================================================================
; code_mark
;-----------------------------------------------------------------------------
code_mark:
		pop	di
		mov	bx,code_mark_1
		call	find_code_mark
		mov	si,di

jump_beep_return:
		jz	beep_return

		lodsw
		cmp	ax,0ffffh
		je	@@set_dump_pos

		call	push_unasm_pos

		mov	[cur_line],ax
		lodsw
		mov	[cur_ip],ax
		lodsw
		mov	[unasm_seg],ax
		jmp	update_commands

@@set_dump_pos:
		lodsw
		mov	[dump_offs],ax
		lodsw
		mov	[dump_seg],ax
		jmp	update_dump
;=============================================================================
; find_code_mark
;-----------------------------------------------------------------------------
find_code_mark:
		sub	di,bx
		xchg	ax,di
		mov	bl,3
		div	bl
		mov	di,ax
		shl	di,1
		shl	di,1
		shl	di,1
		add	di,code_mark_buff
		xchg	ax,cx
		mov	ax,1
		shl	ax,cl
		test	[code_mark_exist],ax
		cld
		ret
;=============================================================================
; follow
;-----------------------------------------------------------------------------
follow:
		test	byte [ref_flags],RF_FOLLOW
		jz	beep_return

		call	push_unasm_pos

		mov	ax,[follow_seg]
		mov	[unasm_seg],ax
		mov	ax,[follow_offs]
		mov	[cur_ip],ax
		mov	word [cur_line],0
		jmp	update_commands
;=============================================================================
; previous
;-----------------------------------------------------------------------------
previous:
		cld
		mov	si,[follow_stack_ptr]
		cmp	si,follow_stack		;??? offset
		jae	beep_return
		lodsw
		mov	[cur_line],ax
		lodsw
		mov	[cur_ip],ax
		lodsw
		mov	[unasm_seg],ax
		mov	[follow_stack_ptr],si
		jmp	update_commands

beep_return:
		jmp	beep
;=============================================================================
; push_unasm_pos
;-----------------------------------------------------------------------------
push_unasm_pos:
		push	ax
		push	si
		mov	si,[follow_stack_ptr]
		cmp	si,follow_stack_btm
		jbe	@@stack_full
		sub	si,2
		mov	ax,[unasm_seg]
		mov	[si],ax
		sub	si,2
		mov	ax,[cur_ip]
		mov	[si],ax
		sub	si,2
		mov	ax,[cur_line]
		mov	[si],ax
		mov	[follow_stack_ptr],si
@@exit:
		pop	si
		pop	ax
		ret

@@stack_full:
		call	beep
		jmp	@@exit
;=============================================================================
; check_shared_area
;-----------------------------------------------------------------------------
check_shared_area:
		push	si
		mov	si,[buf_pos_mark]
		mov	word [buf_pos_mark],window_buffer
		jmp	..@check_code_mark

_check_shared_area:
		push	si

..@check_code_mark:
		cmp	si,code_mark_buff
		jbe	@@check_stack
		mov	word [code_mark_exist],0
		mov	si,code_mark_msg
		call	error_message
		jmp	@@exit_2

@@check_stack:
		cmp	si,[follow_stack_ptr]
		jbe	@@exit_1
		mov	si,follow_stack_msg
		call	message_box

@@exit_2:
		mov	word [follow_stack_ptr],follow_stack

@@exit_1:
		pop	si
		ret
;=============================================================================
; unasm_cur_line
;-----------------------------------------------------------------------------
unasm_cur_line:
		push	si
		push	bp
		mov	si,[cur_ip]
		xor	bp,bp
@@next_cpu_line:
		call	unassemble
		inc	bp
		cmp	bp,[cur_line]
		jbe	@@next_cpu_line
		pop	bp
		pop	si
		ret
;=============================================================================
; find_first_byte
;-----------------------------------------------------------------------------
find_first_byte:

		xor	cx,cx
@@next_byte:
	es	lodsb

		mov	dl,RF_PREFIX
		cmp	al,26h
		je	@@next
		cmp	al,36h
		je	@@next
		cmp	al,2Eh
		je	@@next
		cmp	al,3Eh
		je	@@next

		cmp	al,64h
		je	@@check_cpu
		cmp	al,65h
		je	@@check_cpu
		cmp	al,67h
		je	@@check_cpu
		cmp	al,66h
		jne	@@normal_exit
		mov	dl,RF_ADDRESS_32

@@check_cpu:
		cmp	byte [cpu_type],a386
		jb	@@exit
		or	[ref_flags],dl

@@next:
		loop	@@next_byte
		stc
		ret

@@normal_exit:
		clc
@@exit:
		ret
;=============================================================================
; get_ref_addresses
;-----------------------------------------------------------------------------
get_ref_addresses:
		call	pushr

		and	byte [ref_flags],RF_FOLLOW
		mov	si,bx
		mov	bp,bx
		mov	es,[unasm_seg]
		mov	di,@@follow_table - 2

		cld

@@command:
		call	find_first_byte
		jc	@@jump_to_error_exit

		cmp	al,0ffh			;call/jmp mem/reg
		je	@@ref_jump

		test	byte [ref_flags],RF_PREFIX
		jnz	@@error_exit

		cmp	al,70h
		jb	@@next
		cmp	al,7fh
		jbe	@@short_condit_jump

@@next:
		scasw				;add	di,2
		cmp	byte [di],0
		je	@@error_exit
		cmp	byte [di],al
		jne	@@next
		mov	bl,[di+1]
		mov	bh,0
		lea	bx,[@@first_jump+bx]
		jmp	bx

@@first_jump:

@@ref_jump:
		test	byte [ref_flags],RF_32
		jnz	@@error_exit
	es	lodsb
		mov	bl,al
		mov	cl,3
		shr	al,cl
		mov	ah,al
		and	al,111b
		cmp	al,010b

@@jump_to_error_exit:
		jb	@@error_exit
		cmp	al,101b
		ja	@@error_exit
		cmp	al,011b
		ja	@@ref_call
		mov	di,@@last_jump

@@ref_call:
		shr	ah,cl
		cmp	ah,11b
		je	@@reg_ref
		call	calc_mem_address
		mov	es,dx
		cmp	byte [memory_flag],3
		je	@@far_jump
	es	lodsw
		jmp	@@set_cur_seg

@@reg_ref:
		and	bx,111b
		shl	bx,1
		mov	bx,[reg_offs_table+bx]
		mov	ax,[bx]
		jmp	@@set_cur_seg

@@condit_jump:
		cmp	byte [cpu_type],a386
		jb	@@error_exit
	es	lodsb
		cmp	al,80h
		jb	@@error_exit
		cmp	al,8Fh
		jbe	@@near_condit_jump

@@error_exit:
		cmp	bp,[cur_line_ip]
		jne	@@exit
		and	byte [ref_flags],~RF_FOLLOW

@@exit:
		call	popr
		ret

@@short_condit_jump:
		call	@@set_condition

@@short_jump:
		xor	ax,ax
	es	lodsb
		cbw
		jmp	@@near_jump_

@@near_return:
		mov	si,[cpu_regs+REG_SP]
		mov	es,[cpu_regs+REG_SS]
		jmp	@@near_jump

@@far_return:
		mov	si,[cpu_regs+REG_SP]
		mov	es,[cpu_regs+REG_SS]
		jmp	@@far_jump

@@int3:
		mov	ax,3*4
		jmp	@@get_vector

@@into:
		mov	ax,4*4
		jmp	@@get_vector

@@int:
		xor	ax,ax
	es	lodsb
		shl	ax,1
		shl	ax,1

@@get_vector:
		xchg	ax,si
		xor	ax,ax
		mov	es,ax
		jmp	@@far_jump

@@near_condit_jump:
		call	@@set_condition

@@near_jump:
	es	lodsw

@@near_jump_:
		add	ax,si

@@set_cur_seg:
		mov	dx,[unasm_seg]
		jmp	@@normal_exit

@@far_jump:
	es	lodsw
		xchg	ax,dx
	es	lodsw
		xchg	ax,dx

@@normal_exit:
		cmp	bp,[cur_line_ip]
		jne	@@skip
		mov	[follow_seg],dx
		mov	[follow_offs],ax
		or	byte [ref_flags],RF_FOLLOW
@@skip:
		cmp	di,@@last_jump
		jae	@@exit
		or	byte [ref_flags],RF_JUMP
		call	adjust_address
		xchg	ax,bx
		xchg	dx,cx
		mov	dx,[unasm_seg]
		mov	ax,bp
		call	adjust_address
		cmp	dx,cx
		jb	@@exit
		ja	@@set_dir
		cmp	ax,bx
		jb	@@jump_to_exit

@@set_dir:
		or	byte [ref_flags],RF_BACKWARD

@@jump_to_exit:
		jmp	@@exit

@@loop_jump:
		sub	al,0e0h-16
		mov	ah,0
		call	@@set_condition_
		jmp	@@short_jump

@@set_condition:
		and	ax,0fh
@@set_condition_:
		or	byte [ref_flags],RF_CONDITION
		mov	dx,ax
		xchg	ax,bx
		mov	bl,[@@condition_table+bx]
		mov	bh,0
		add	bx,@@jx

@@check_flag:
		mov	cx,[cpu_regs+REG_FL]
		and	cx,0000100011010101b
		push	cx
		popf
		call	bx
		mov	ax,dx
		ret

@@set_result:
		or	byte [ref_flags],RF_TRUE

@@return:
		ret

@@jx:
@@loopne:
		mov	bx,@@jnz
		jmp	@@check_cx

@@loope:
		mov	bx,@@jz
		jmp	@@check_cx

@@loop:
		mov	bx,@@set_result

@@check_cx:
		test	byte [ref_flags],RF_OPERAND_32
		jnz	@@2
		cmp	word [cpu_regs+REG_CX],1
		jmp	@@1

@@2:
	cpu	386
		cmp	dword [cpu_regs+REG_CX],1
	cpu	8086
@@1:
		je	@@return
		jmp	@@check_flag



@@jcxz:
		test	byte [ref_flags],RF_OPERAND_32
		jnz	@@4
		cmp	word [cpu_regs+REG_CX],0
		jmp	@@3

@@4:
	cpu	386
		cmp	dword [cpu_regs+REG_CX],0
	cpu	8086
@@3:
		je	@@set_result
		ret

@@jo:
		jo	@@set_result
		ret

@@jno:
		jno	@@set_result
		ret

@@jb:
		jb	@@set_result
		ret

@@jnb:
		jnb	@@set_result
		ret

@@jz:
		jz	@@set_result
		ret

@@jnz:
		jnz	@@set_result
		ret

@@jna:
		jna	@@set_result
		ret

@@ja:
		ja	@@set_result
		ret

@@js:
		js	@@set_result
		ret

@@jns:
		jns	@@set_result
		ret

@@jp:
		jp	@@set_result
		ret

@@jnp:
		jnp	@@set_result
		ret

@@jl:
		jl	@@set_result
		ret

@@jnl:
		jnl	@@set_result
		ret

@@jng:
		jng	@@set_result
		ret

@@jg:
		jg	@@set_result
		ret


@@follow_table:
		db	0fh				;286+
		db	@@condit_jump	- @@first_jump

		db	0e0h				;loopne/loopnz
		db	@@loop_jump	- @@first_jump

		db	0e1h				;loope/loopz
		db	@@loop_jump	- @@first_jump

		db	0e2h				;loop
		db	@@loop_jump	- @@first_jump

		db	0e3h				;jcxz
		db	@@loop_jump	- @@first_jump

		db	0e9h				;jmp near
		db	@@near_jump	- @@first_jump

		db	0eah				;jmp far
		db	@@far_jump	- @@first_jump

		db	0ebh				;jmp short
		db	@@short_jump	- @@first_jump

@@last_jump:
		db	9ah				;call far
		db	@@far_jump	- @@first_jump

		db	0e8h				;call near
		db	@@near_jump	- @@first_jump

		db	0c2h				;ret near
		db	@@near_return	- @@first_jump

		db	0c3h				;ret near
		db	@@near_return	- @@first_jump

		db	0cah				;ret far
		db	@@far_return	- @@first_jump

		db	0cbh				;ret far
		db	@@far_return	- @@first_jump

		db	0cch				;int 3
		db	@@int3		- @@first_jump

		db	0cdh				;int
		db	@@int		- @@first_jump

		db	0ceh				;into
		db	@@into		- @@first_jump

		db	0cfh				;iret
		db	@@far_return	- @@first_jump

		db	0

@@condition_table:
		db	@@jo	- @@jx		;jo
		db	@@jno	- @@jx		;jno
		db	@@jb	- @@jx		;jb/jnae
		db	@@jnb	- @@jx		;jae/jnb
		db	@@jz	- @@jx		;je/jz
		db	@@jnz	- @@jx		;jne/jnz
		db	@@jna	- @@jx		;jbe/jna
		db	@@ja	- @@jx		;ja/jnbe
		db	@@js	- @@jx		;js
		db	@@jns	- @@jx		;jns
		db	@@jp	- @@jx		;jp/jpe
		db	@@jnp	- @@jx		;jnp/jpo
		db	@@jl	- @@jx		;jl/jnge
		db	@@jnl	- @@jx		;jge/jnl
		db	@@jng	- @@jx		;jle/jng
		db	@@jg	- @@jx		;jg/jnle
		db	@@loopne- @@jx		;loopne
		db	@@loope	- @@jx		;loope
		db	@@loop	- @@jx		;loop
		db	@@jcxz	- @@jx		;jcxz

reg_offs_table:
		dw	cpu_regs+REG_AX
		dw	cpu_regs+REG_CX
		dw	cpu_regs+REG_DX
		dw	cpu_regs+REG_BX
		dw	cpu_regs+REG_SP
		dw	cpu_regs+REG_BP
		dw	cpu_regs+REG_SI
		dw	cpu_regs+REG_DI
;=============================================================================
; adjust_address
;-----------------------------------------------------------------------------
adjust_address:
		push	ax
		shr	ax,1
		shr	ax,1
		shr	ax,1
		shr	ax,1
		add	dx,ax
		pop	ax
		and	ax,000fh
		ret
;=============================================================================
; set_direction
;-----------------------------------------------------------------------------
set_direction:
		call	pushr
		test	byte [ref_flags],RF_JUMP
		jz	@@exit

		cld
		mov	si,buffer + BUF_SIZE	;-offset

@@next_char:
		dec	si
		cmp	byte [si],' '
		je	@@next_char
		mov	di,si
		push	ds
		pop	es
		scasw
		mov	al,18h
		test	byte [ref_flags],RF_BACKWARD
		jnz	@@check_condition
		mov	al,19h

@@check_condition:
		test	byte [ref_flags],RF_CONDITION
		jz	@@set_direction
		test	byte [ref_flags],RF_TRUE
		jnz	@@set_direction
		add	al,1Eh-18h

@@set_direction:
		stosb

@@exit:
		call	popr
		ret
;=============================================================================
; display_code_marks
;-----------------------------------------------------------------------------
display_code_marks:
		call	pushr

		xchg	ax,bp
		mov	ah,al
		mov	al,CPU_WIDTH-1
		call	get_addr
		push	ax

		mov	cx,10
		mov	bp,code_mark_buff
		mov	di,1

@@next_mark:
		test	[code_mark_exist],di
		jz	@@next
		cmp	word [bp],0ffffh
		je	@@next
		mov	ax,[bp+6]
		mov	dx,[bp+4]
		call	adjust_address
		push	bx
		xchg	ax,bx
		mov	si,[unasm_seg]
		xchg	dx,si
		call	adjust_address
		cmp	ax,bx
		pop	bx
		jne	@@next
		mov	ax,atCodeMark*256+'0'+10
		sub	al,cl
		cmp	dx,si
		je	@@write

@@next:
		shl	di,1
		add	bp,4*2
		loop	@@next_mark
		mov	ax,atCodeMark*256+' '

@@write:
		pop	di
		stosw
		call	popr
		ret
;=============================================================================
; E0F
;=============================================================================

