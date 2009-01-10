;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; trace.inc
; Trace-related procedures.
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
; Changelog
;-----------------------------------------------------------------------------
; 2007-02-06	0leg	animate added
; 2007-03-01	0leg	user_push, user_pop optimized
; 2008-02-15	0leg	skip_instruction added
;=============================================================================


%macro		save_act_regs		0
		mov	[save_sp],sp
%endmacro


%macro		restore_act_regs	0
		mov	ax,cs
		mov	ds,ax
		mov	es,ax
		mov	ss,ax
		mov	sp,[save_sp]
%endmacro


%macro		save_user_regs		0
		cmp	byte [cs:system_cpu],3
		jb	@@skip_esp
		db	66h
@@skip_esp:
		mov	[cs:cpu_regs+REG_SP],sp
		call	_save_user_regs
%endmacro


%macro		restore_user_regs	0
		call	_restore_user_regs
%endmacro


;=============================================================================
; _save_user_regs
;-----------------------------------------------------------------------------
_save_user_regs:
first_prefix:
prefix_01:
		nop
		mov	[cs:cpu_regs+REG_AX],ax

		mov	[cs:cpu_regs+REG_DS],ds
		mov	ax,cs
		mov	ds,ax
		pop	word [keep_ip]

prefix_02:
		nop
		mov	[cpu_regs+REG_BX],bx

prefix_03:
		nop
		mov	[cpu_regs+REG_CX],cx

prefix_04:
		nop
		mov	[cpu_regs+REG_DX],dx

prefix_05:
		nop
		mov	[cpu_regs+REG_SI],si

prefix_06:
		nop
		mov	[cpu_regs+REG_DI],di

prefix_07:
		nop
		mov	[cpu_regs+REG_BP],bp

		cmp	byte [system_cpu],3
		jb	@@skip
	cpu	386
		mov	[cpu_regs+REG_GS],gs
		mov	[cpu_regs+REG_FS],fs
	cpu	8086
@@skip:
		mov	[cpu_regs+REG_ES],es
		mov	[cpu_regs+REG_SS],ss
		mov	es,ax
		mov	ss,ax

prefix_16:
		nop
		mov	ax,[cpu_regs+REG_IP]

prefix_17:
		nop
		mov	[save_cpu_regs+REG_IP],ax

prefix_18:
		nop
		mov	ax,[cpu_regs+REG_CS]

prefix_19:
		nop
		mov	[save_cpu_regs+REG_CS],ax

		mov	byte [regs_saved],0
		mov	sp,[save_sp]
		jmp     word [keep_ip]
;=============================================================================
; _restore_user_regs
;-----------------------------------------------------------------------------
_restore_user_regs:
		pop	word [keep_ip]
		call	save_regs

prefix_08:
		nop
		mov	ax,[cpu_regs+REG_AX]

prefix_09:
		nop
		mov	bx,[cpu_regs+REG_BX]

prefix_10:
		nop
		mov	cx,[cpu_regs+REG_CX]

prefix_11:
		nop
		mov	dx,[cpu_regs+REG_DX]

prefix_12:
		nop
		mov	si,[cpu_regs+REG_SI]

prefix_13:
		nop
		mov	di,[cpu_regs+REG_DI]

prefix_14:
		nop
		mov	sp,[cpu_regs+REG_SP]

prefix_15:
		nop
		mov	bp,[cpu_regs+REG_BP]
		cmp	byte [system_cpu],3
		jb	@@skip
	cpu	386
		mov	gs,[cpu_regs+REG_GS]
		mov	fs,[cpu_regs+REG_FS]
	cpu	8086
@@skip:
		mov	es,[cpu_regs+REG_ES]
		mov	ss,[cpu_regs+REG_SS]
		mov	ds,[cpu_regs+REG_DS]
		jmp     word [cs:keep_ip]
;=============================================================================
; save_regs
;-----------------------------------------------------------------------------
save_regs:
		push	ds
		pop	es
		mov	si,cpu_regs
		mov	di,save_cpu_regs
		mov	cx,SIZEOF_REGISTERS_32
		cld
	rep	movsb
		ret
;=============================================================================
; restore_regs
;-----------------------------------------------------------------------------
restore_regs:
		push	ds
		pop	es
		push	word [cpu_regs+REG_FL]
		push	word [cpu_regs+REG_CS]
		push	word [cpu_regs+REG_IP]
		mov	di,cpu_regs
		mov	si,save_cpu_regs
		mov	cx,SIZEOF_REGISTERS_32
		cld
	rep	movsb
		pop	word [cpu_regs+REG_IP]
		pop	word [cpu_regs+REG_CS]
		pop	word [cpu_regs+REG_FL]
		ret
;=============================================================================
; skip_instruction
;-----------------------------------------------------------------------------
skip_instruction:
		mov	es,[unasm_seg]
		mov	si,[cpu_regs+REG_IP]
		call	unassemble
		mov	[cpu_regs+REG_IP],si
		mov	[save_cpu_regs+REG_IP],si
		jmp	trace_update
;=============================================================================
; single_step
;-----------------------------------------------------------------------------
single_step:
		mov	dl,0
		call	emulate
		jnc	@@1
		call	save_regs
		jmp	trace_update
@@1:		call	check_swap
		cmp	byte [swap_flag],0
		je	@@n1
		call	restore_screen
@@n1:
		cli
;		call	set_09_vector
		mov	dx,@@int_01_entry
		call	set_01_vector
		mov	al,[user_port_21]
		or	al,3
		out	21h,al
		save_act_regs
		restore_user_regs
		or	word [cs:cpu_regs+REG_FL],100h
		push	word [cs:cpu_regs+REG_FL]
		push	word [cs:cpu_regs+REG_CS]
		push	word [cs:cpu_regs+REG_IP]
		iret
@@int_01_entry:
		pop	word [cs:cpu_regs+REG_IP]
		pop	word [cs:cpu_regs+REG_CS]
		pop	word [cs:cpu_regs+REG_FL]
		save_user_regs
		in	al,21h
		and	al,~3
		and	byte [user_port_21],3
		or	[user_port_21],al
		mov	al,[my_port_21]
		out	21h,al
		and	word [cpu_regs+REG_FL],~100h
		call	restore_01_vector
		call	clear_breakpoints
		sti
		cmp	byte [user_screen_mode],0
		je	trace_update
		call	save_screen
		call	draw_cpu_window

trace_update:
		call	update_screen
		cmp	word [cur_ip_line],-1
		jne	@@2
		jmp	go_to_origin
@@2:
		mov	ax,[cur_ip_line]
		cmp	ax,CPU_HEIGHT-4
		ja	@@adjust
@@ok:
		mov	[cur_line],ax
		jmp	update_commands
@@adjust:
		mov	si,[cur_ip]
		sub	ax,6
		mov	cx,ax
@@next:
		call	unassemble
		loop	@@next
		mov	[cur_ip],si
		mov	ax,6
		jmp	@@ok
;=============================================================================
; proc_trace
;-----------------------------------------------------------------------------
proc_trace:
		call	check_jump
		jnc	@@3
		jmp	single_step
@@3:
		xor	bp,bp
		mov	ax,[cpu_regs+REG_CS]
		mov	[unasm_seg],ax
		mov	si,[cpu_regs+REG_IP]
		call	check_break
		jc	@@a
		mov	bp,bx
@@a:
		cli
		mov	es,ax
		call	unassemble
		call	check_break
		jnc	@@ok
		mov	bx,bp0
		call	set_breakpoint
		jnc	@@ok
		mov	si,code_not_in_ram
		call	error_message
		call	clear_breakpoint
		jmp	trace_update
@@ok:
		mov	dl,1
		call	check_swap
		cmp	byte [swap_flag],0
		je	@@1
		call	restore_screen
@@1:
		call	set_09_vector
		mov	dx,@@int_03_entry
		call	set_03_vector
		call	set_breakpoints
		or	bp,bp
		jz	@@b
		mov	bx,bp
		call	clear_breakpoint
@@b:
		mov	al,[user_port_21]
		and	al,11111110b
		out	21h,al
		save_act_regs
		restore_user_regs
		push	word [cs:cpu_regs+REG_FL]
		push	word [cs:cpu_regs+REG_CS]
		push	word [cs:cpu_regs+REG_IP]
		iret

@@int_03_entry:
		pop	word [cs:cpu_regs+REG_IP]
		pop	word [cs:cpu_regs+REG_CS]
		pop	word [cs:cpu_regs+REG_FL]
		save_user_regs
		mov	ax,[cpu_regs+REG_IP]
		dec	ax
		mov	[cpu_regs+REG_IP],ax
		mov	[save_cpu_regs+REG_IP],ax
		call	clear_breakpoints
		in	al,21h
		mov	[user_port_21],al
		mov	al,[my_port_21]
		out	21h,al
		sti
@@skip:
		cmp	byte [user_screen_mode],0
		je	@@2
		call	save_screen
		call	draw_cpu_window
@@2:
		jmp	trace_update
;=============================================================================
; animate
;-----------------------------------------------------------------------------
animate:
		call	pushr
		mov	si,animate_dialog
		call	init_dialog

		mov	byte [anim_dlg_item],0
@@again:
		call	exec_dialog_
		jz	@@exit

		call	get_delay
		mov	byte [anim_dlg_item],0
		jc	@@again

		call	draw_screen

		cmp	word [delay_value],0
		jne	@@ok
		inc	word [delay_value]
@@ok:

		call	display_focus
@@loop:
		call	single_step
		push	ds
		xor	ax,ax
		mov	ds,ax
		mov	bx,[46ch]
		mov	cx,[46eh]
		pop	ds
		add	bx,[delay_value]
		adc	cx,0
@@wait:
		mov	ah,1
		int	16h
		jnz	@@quit
		push	ds
		xor	ax,ax
		mov	ds,ax
		cmp	[46eh],cx
		ja	@@end
		cmp	[46ch],bx
		jae	@@end
		pop	ds
		jmp	@@wait
@@end:
		pop	ds
		jmp	@@loop
@@quit:
		mov	ah,0
		int	16h
		call	hide_focus
@@exit:
		call	draw_screen
		call	popr
		ret
;=============================================================================
; set_01_vector
;-----------------------------------------------------------------------------
set_01_vector:
		mov	si,1*4
		mov	di,save_int_01
		call	set_vector
		mov	byte [int_01_set],1
		ret
;=============================================================================
; restore_01_vector
;-----------------------------------------------------------------------------
restore_01_vector:
		cmp	byte [int_01_set],0
		je	@@quit
		mov	si,save_int_01
		mov	di,1*4
		call	restore_vector
		mov	byte [int_01_set],0
@@quit:
		ret
;=============================================================================
; set_03_vector
;-----------------------------------------------------------------------------
set_03_vector:
		mov	si,3*4
		mov	di,save_int_03
		call	set_vector
		mov	byte [int_03_set],1
		ret
;=============================================================================
; restore_03_vector
;-----------------------------------------------------------------------------
restore_03_vector:
		cmp	byte [int_03_set],0
		je	@@quit
		mov	si,save_int_03
		mov	di,3*4
		call	restore_vector
		mov	byte [int_03_set],0
@@quit:
		ret
;=============================================================================
; set_09_vector
;-----------------------------------------------------------------------------
set_09_vector:
		mov	si,9*4
		mov	di,save_int_09
		mov	dx,int_09_handler
		call	set_vector
		mov	byte [int_09_set],1
		ret
;=============================================================================
; restore_09_vector
;-----------------------------------------------------------------------------
restore_09_vector:
		cmp	byte [int_09_set],0
		je	@@quit
		mov	si,save_int_09
		mov	di,9*4
		call	restore_vector
		mov	byte [int_09_set],0
@@quit:
		ret
;=============================================================================
; run_program
;-----------------------------------------------------------------------------
run_program:
		mov	ax,[cpu_regs+REG_CS]
		mov	[unasm_seg],ax
		mov	si,[cpu_regs+REG_IP]
		call	check_break
		jc	@@a
		call	single_step
@@a:
		cmp	byte [swap_mode],0
		je	@@n
		call	restore_screen
@@n:
		cli
;		call	RestoreUserInts
		call	set_breakpoints
		call	set_09_vector
		mov	dx,bp_entry
		call	set_03_vector
		mov	al,[user_port_21]
		and	al,11111110b
		out	21h,al
		save_act_regs
		restore_user_regs
		push	word [cs:cpu_regs+REG_FL]
		push	word [cs:cpu_regs+REG_CS]
		push	word [cs:cpu_regs+REG_IP]
		iret
;=============================================================================
; go_to_cursor
;-----------------------------------------------------------------------------
go_to_cursor:
		mov	bx,bp0
		mov	ax,[unasm_seg]
		mov	[bx+BP_SEG],ax
		mov	ax,[cur_line_ip]
		mov	[bx+BP_OFFS],ax
		call	test_break
		jnc	@@ok
		mov	si,code_not_in_ram
		jmp	error_message
@@ok:
		push	word [unasm_seg]
		push	word [cur_line_ip]
		mov	ax,[cpu_regs+REG_CS]
		cmp	ax,[unasm_seg]
		jne	@@a
		mov	ax,[cur_line_ip]
		cmp	ax,[cpu_regs+REG_IP]
		jne	@@a
		call	single_step
		jmp	@@b
@@a:
		mov	ax,[cpu_regs+REG_CS]
		mov	[unasm_seg],ax
		mov	si,[cpu_regs+REG_IP]
		call	check_break
		jc	@@b
		call	single_step
@@b:
		cmp	byte [swap_mode],0
		je	@@n1
		call	restore_screen
@@n1:
		cli
;		call	RestoreUserInts
		call	set_09_vector
		mov	dx,@@int_03_entry
		call	set_03_vector
		pop	si
		pop	es
		call	set_breakpoints
		mov	es,[bp0+BP_SEG]
		mov	si,[bp0+BP_OFFS]
		mov	bx,bp0
		call	set_breakpoint
		mov	al,[user_port_21]
		and	al,11111110b
		out	21h,al
		save_act_regs
		restore_user_regs
		push	word [cs:cpu_regs+REG_FL]
		push	word [cs:cpu_regs+REG_CS]
		push	word [cs:cpu_regs+REG_IP]
		iret
@@int_03_entry:
bp_entry:
		pop	word [cs:cpu_regs+REG_IP]
		pop	word [cs:cpu_regs+REG_CS]
		pop	word [cs:cpu_regs+REG_FL]
		save_user_regs
;		restore_act_regs
		dec	word [cpu_regs+REG_IP]
act_popup:
		call	clear_breakpoints
		in	al,21h
		mov	[user_port_21],al
		mov	al,[my_port_21]
		out	21h,al
		sti
		cmp	byte [user_screen_mode],0
		je	@@n2
		call	save_screen
		call	draw_cpu_window
@@n2:
		call	update_screen
		mov	ax,[cur_ip_line]
		cmp	ax,-1
		jne	@@1
		jmp	go_to_origin
@@1:
		cmp	ax,[cur_line]
		je	@@quit
		mov	[cur_line],ax
		call	update_commands
@@quit:
		ret
;=============================================================================
; int_09_handler
;-----------------------------------------------------------------------------
int_09_handler:
		push	ax
		in	al,60h
		cmp	al,SCAN_CODE
		jne	@@quit
		push	ds
		xor	ax,ax
		mov	ds,ax
		mov	al,[417h]
		test	al,SHIFT_CODE
		jnz	int_09_execute
		pop	ds
@@quit:
		pop	ax
		jmp	far [cs:save_int_09]
int_09_execute:
		in	al,61h
		or	al,80h
		out	61h,al
		and	al,7Fh
		out	61h,al
		mov	al,20h
		out	20h,al
		pop	ds
		pop	ax
		pop	word [cs:cpu_regs+REG_IP]
		pop	word [cs:cpu_regs+REG_CS]
		pop	word [cs:cpu_regs+REG_FL]
		save_user_regs
;		restore_act_regs
		jmp	act_popup
;=============================================================================
; set_breakpoint
;-----------------------------------------------------------------------------
set_breakpoint:
		mov	[bx+BP_SEG],es
		mov	[bx+BP_OFFS],si
		mov	byte [bx+BP_SET],1
		mov	al,[es:si]
		mov	[bx+BP_BYTE],al
		mov	byte [es:si],0cch
		cmp	byte [es:si],0cch
		je	@@quit
		stc
@@quit:
		ret
;=============================================================================
; clear_breakpoint
;-----------------------------------------------------------------------------
clear_breakpoint:
		cmp	byte [bx+BP_SET],0
		je	@@quit
		push	ax
		push	si
		push	es
		mov	es,[bx+BP_SEG]
		mov	si,[bx+BP_OFFS]
		mov	al,[bx+BP_BYTE]
		mov	[es:si],al
		mov	byte [bx+BP_SET],0
		pop	es
		pop	si
		pop	ax
@@quit:
		ret
;=============================================================================
; clear_breakpoints
;-----------------------------------------------------------------------------
clear_breakpoints:
		call	restore_03_vector
		call	restore_09_vector
		mov	bx,bp0
		mov	cx,MAX_BREAKPOINTS+1

@@next_bp:
		call	clear_breakpoint
		add	bx,SIZEOF_BREAKPOINT
		loop	@@next_bp
		ret
;=============================================================================
; set_breakpoints
;-----------------------------------------------------------------------------
set_breakpoints:
		push	bx
		push	cx
		push	si
		push	es
		mov	bx,bp1
		mov	cx,MAX_BREAKPOINTS
@@next_bp:
		cmp	byte [bx+BP_ACTIVE],BP_UNUSED
		je	@@skip
		cmp	byte [bx+BP_ACTIVE],BP_DISABLED
		je	@@skip
		mov	es,[bx+BP_SEG]
		mov	si,[bx+BP_OFFS]
		call	set_breakpoint
@@skip:
		add	bx,SIZEOF_BREAKPOINT
		loop	@@next_bp
		pop	es
		pop	si
		pop	cx
		pop	bx
		ret
;=============================================================================
; emulate
;-----------------------------------------------------------------------------
emulate:
		cld
		push	ds
		push	es
		mov	si,[cpu_regs+REG_IP]
		mov	es,[cpu_regs+REG_CS]
		mov	bx,inst_desc
@@next_byte:
	es	lodsb
		xlat
		test	al,S_PREFIX
		jnz	@@next_byte
		test	al,S_EM
		jnz	@@emulate
@@quit:
		pop	es
		pop	ds
		ret
@@emulate:
		mov	al,[es:si-1]
		cmp	al,0cdh
		je	@@int
		cmp	al,0ceh
		je	@@into
		cmp	al,9ch
		je	@@pushf
		cmp	al,9dh
		je	@@popf
		cmp	al,0cfh
		je	@@iret
		clc
		jmp	@@quit
@@pushf:
		mov	ax,[cpu_regs+REG_FL]
		and	ax,~100h
		call	user_push
		jmp	@@skip
@@popf:
		call	user_pop
		and	ax,~100h
		mov	[cpu_regs+REG_FL],ax
		jmp	@@skip
@@iret:
		call	user_pop
		mov	[cpu_regs+REG_IP],ax
		call	user_pop
		mov	[cpu_regs+REG_CS],ax
		call	user_pop
		mov	[cpu_regs+REG_FL],ax
		stc
		jmp	@@quit
@@into:
		test	byte [cpu_regs+REG_FL+1],00001000b
		jz	@@skip
		mov	al,4
		jmp	@@int_n
@@int:
	es	lodsb
@@int_n:
		push	ax
		mov	ax,[cpu_regs+REG_FL]
		and	ax,~100h
		call	user_push
		and	ax,~200h
		mov	[cpu_regs+REG_FL],ax
		mov	ax,es
		call	user_push
		mov	ax,si
		call	user_push
		pop	ax
		mov	ah,0
		shl	ax,1
		shl	ax,1
		mov	si,ax
		xor	ax,ax
		mov	ds,ax
		lodsw
		mov	[cs:cpu_regs+REG_IP],ax
		lodsw
		mov	[cs:cpu_regs+REG_CS],ax
		stc
		jmp	@@quit
@@skip:
		mov	[cpu_regs+REG_IP],si
		stc
		jmp	@@quit
;=============================================================================
; user_push
;-----------------------------------------------------------------------------
user_push:
		push	bx
		push	ds
		sub	word [cpu_regs+REG_SP],2
		mov	bx,[cpu_regs+REG_SP]
		mov	ds,[cpu_regs+REG_SS]
		mov	[bx],ax
		pop	ds
		pop	bx
		ret
;=============================================================================
; user_pop
;-----------------------------------------------------------------------------
user_pop:
		push	bx
		push	ds
		mov	bx,[cpu_regs+REG_SP]
		add	word [cpu_regs+REG_SP],2
		mov	ds,[cpu_regs+REG_SS]
		mov	ax,[bx]
		pop	ds
		pop	bx
		ret
;------------------------------------------------------------------------------


;=============================================================================
; E0F
;=============================================================================


