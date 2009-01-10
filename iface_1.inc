;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; iface_1.inc
; User interface and other procedures.
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
; 2007-02-??	0leg	Too many changes...
; 2007-03-25	0leg	reset_program and term_handler fixed.
; 2007-04-01	0leg	draw_cpu_window optimized. _update_dump modified
;			(highlighting of NUL bytes).
; 2007-12-02	0leg	update_stack, edit_flags, edit_registers, edit_dump
;			cleaned up.
;=============================================================================


;=============================================================================
; about
;-----------------------------------------------------------------------------
; Shows `About' or `Info' window.
;
about:
		mov	si,about_dialog
_about:
		call	init_dialog
		call	exec_dialog
		jmp	draw_screen
;=============================================================================
; show_help
;-----------------------------------------------------------------------------
; Shows help screen.
;
show_help:
		call	pushr
		mov	es,[video_seg]
		mov	si,help_page1
		call	show_help_page
		call	read_key
		cmp	ax,kbF1
		jne	@@quit
		mov	si,help_page2
		call	show_help_page
		call	read_key
@@quit:
		call	draw_screen
		call	popr
		ret
;=============================================================================
; show_help_page
;-----------------------------------------------------------------------------
show_help_page:
		xor	di,di
		mov	bx,800fh
		xor	cx,cx
		mov	bp,1
		mov	dx,ds

		jmps	decompr_start_n2b
;Decompressor start
%include	 "n2b_d8e.inc"		;Slightly modified part of UPX
;Decompressor end
		ret
;=============================================================================
; any_key
;-----------------------------------------------------------------------------
any_key:
		call	check_shift
		jz	@@check_cheat_code
		mov	cx,EDIT_PROCS_NUM
		mov	bx,edit_procs
@@next:
		cmp	[bx],ah
		jne	@@skip
		call	[bx+1]
		clc
		ret
@@skip:
		add	bx,3
		loop	@@next

@@exit:
		stc
		ret

@@check_cheat_code:
		call	upcase
		xor	al,45h
		mov	bx,[magic_offs]
		cmp	[magic_code+bx],al
		je	@@next_char
		xor	bx,bx
		cmp	[magic_code],al
		jne	@@exit
@@next_char:
		inc	bx
		mov	[magic_offs],bx
		cmp	bl,5
		jne	@@exit
		mov	si,info_dialog
		call	_about
		clc
		ret
;=============================================================================
; data_watch
;-----------------------------------------------------------------------------
data_watch:
		cmp	word [data_watch_proc],0
		mov	word [data_watch_proc],0
		jne	@@update_watch
		mov	ax,[data_watch_temp]
		or	ax,ax
		jnz	@@on
		jmp	beep

@@on:
		mov	[data_watch_proc],ax
		call	ax
@@update_watch:
		jmp	update_watch_label
;=============================================================================
; switch_reg_mode
;-----------------------------------------------------------------------------
switch_reg_mode:
		cmp	byte [system_cpu],3
		jae	@@toggle
		jmp	beep
@@toggle:
		call	pushr
		xor	byte [regs_mode],1
		xor	word [regs_menu_1],'16' ^ '32'
		xor	word [regs_menu_2],'16' ^ '32'
		xor	word [stack_height],STK16_LEN ^ STK32_LEN
		call	get_cursor
		call	draw_screen	;CX preserved
		call	set_cursor
		call	popr
		ret
;=============================================================================
; cur_down
;-----------------------------------------------------------------------------
cur_down:
		cmp	word [cur_line],CPU_HEIGHT-1
		jb	@@inc
		mov	si,[cur_ip]
		call	unassemble
		mov	[cur_ip],si
		jmp	@@quit
@@inc:
		inc	word [cur_line]
@@quit:
		jmp	update_commands
;=============================================================================
; cur_up
;-----------------------------------------------------------------------------
cur_up:
		cmp	word [cur_line],0
		ja	@@dec
		cmp	word [cur_ip],0
		je	@@quit
		mov	word [@@c],20		;???
@@next_1:
		mov	si,[cur_ip]
		sub	si,[@@c]
		jnc	@@next
		xor	si,si
@@next:
		mov	bp,si
		call	unassemble
		cmp	si,[cur_ip]
		je	@@found
		cmp	si,[cur_ip]
		jb	@@next
		dec	word [@@c]
		jnz	@@next_1
		dec	word [cur_ip]
		jmp	@@quit
@@found:
		mov	[cur_ip],bp
		jmp	@@quit
@@dec:
		dec	word [cur_line]
@@quit:
		jmp	update_commands

@@c		dw	20
;=============================================================================
; page_down
;-----------------------------------------------------------------------------
page_down:
		mov	si,[cur_ip]
		mov	cx,CPU_HEIGHT-1
@@next:
		call	unassemble
		loop	@@next

		mov	[cur_ip],si
		jmp	update_commands
;=============================================================================
; page_up
;-----------------------------------------------------------------------------
page_up:
		mov	si,[cur_ip]
		or	si,si
		je	cur_home
		sub	si,5*CPU_HEIGHT
		jnc	@@3
		xor	si,si
@@3:
		mov	cx,CPU_HEIGHT-1
		xor	bx,bx
@@next:
		mov	[instr_table+bx],si
		call	unassemble
		inc	bx
		inc	bx
		loop	@@next
@@2:
		cmp	si,[cur_ip]
		jae	@@found
		mov	cx,CPU_HEIGHT-2
		cld
		push	si
		mov	si,instr_table
		mov	di,si
		lodsw
	rep	movsw
		pop	si
		mov	ax,si
		stosw
		call	unassemble
		jmp	@@2
@@found:
		mov	si,[instr_table]
		mov	[cur_ip],si
		jmp	update_commands
;=============================================================================
; cur_home
;-----------------------------------------------------------------------------
cur_home:
		mov	word [cur_line],0
		jmp	update_commands
;=============================================================================
; cur_end
;-----------------------------------------------------------------------------
cur_end:
		mov	word [cur_line],CPU_HEIGHT-1
		jmp	update_commands
;=============================================================================
; cur_right
;-----------------------------------------------------------------------------
cur_right:
		cmp	word [cur_x],MAX_CUR_X
		jae	@@quit
		inc	word [cur_x]
		call	update_commands
@@quit:
		ret
;=============================================================================
; cur_left
;-----------------------------------------------------------------------------
cur_left:
		cmp	word [cur_x],0
		je	@@quit
		dec	word [cur_x]
		call	update_commands
@@quit:
		ret
;=============================================================================
; go_to_origin
;-----------------------------------------------------------------------------
go_to_origin:
		mov	ax,[cpu_regs+REG_CS]
		mov	[unasm_seg],ax
		mov	ax,[cpu_regs+REG_IP]
		mov	[cur_ip],ax
		mov	word [cur_line],0
		jmp	update_commands
;=============================================================================
; new_csip
;-----------------------------------------------------------------------------
new_csip:
		mov	ax,[cur_line_ip]
		mov	[cpu_regs+REG_IP],ax
		mov	ax,[unasm_seg]
		mov	[cpu_regs+REG_CS],ax
		jmp	update_regs_and_cmds
;=============================================================================
; byte_up
;-----------------------------------------------------------------------------
byte_up:
		cmp	word [cur_ip],0
		je	@@quit
		dec	word [cur_ip]
		call	update_commands
@@quit:
		ret
;=============================================================================
; byte_down
;-----------------------------------------------------------------------------
byte_down:
		inc	word [cur_ip]
		jmp	update_commands
;=============================================================================
; exec_user_screen
;-----------------------------------------------------------------------------
exec_user_screen:
		call	restore_screen
		call	read_key
		call	save_screen
		jmp	draw_screen
;=============================================================================
; restore_all
;-----------------------------------------------------------------------------
restore_all:
		call	restore_regs
;		jmp	update_regs_and_cmds	;2007-12-01
		jmp	update_screen
;=============================================================================
; restore_flags
;-----------------------------------------------------------------------------
restore_flags:
		mov	ax,[save_cpu_regs+REG_FL]
		mov	[cpu_regs+REG_FL],ax
		jmp	update_regs_and_cmds
;=============================================================================
; restore_csip
;-----------------------------------------------------------------------------
restore_csip:
		mov	ax,[save_cpu_regs+REG_IP]
		mov	[cpu_regs+REG_IP],ax
		mov	ax,[save_cpu_regs+REG_CS]
		mov	[cpu_regs+REG_CS],ax
		jmp	update_regs_and_cmds
;=============================================================================
; unasm_go_to
;-----------------------------------------------------------------------------
unasm_go_to:
		mov	ax,9*256+44
		mov	di,main_addr_string
		call	read_address
		jc	@@quit

		call	push_unasm_pos

		or	ch,ch
		jz	@@no_seg
		mov	[unasm_seg],dx
		cmp	ch,2
		je	@@no_offs
@@no_seg:
		mov	[cur_ip],ax
@@no_offs:
		mov	word [cur_line],0
		call	update_commands
@@quit:
		ret
;=============================================================================
; read_address
;-----------------------------------------------------------------------------
read_address:
		push	si
		push	di
		push	bp
		push	es
		push	ds
		pop	es
		mov	[d_addr_str],di
		mov	si,addr_dialog
		mov	[si],ax
		add	ax,31+7*256
		mov	[si+2],ax
		call	init_dialog

@@again:
		call	exec_dialog
		jz	@@error_exit

		mov	si,[d_addr_str]
		call	get_address
		jc	@@again

		call	draw_screen
		clc
		jmp	@@exit

@@error_exit:
		call	draw_screen
		stc
@@exit:
		pop	es
		pop	bp
		pop	di
		pop	si
		ret
;=============================================================================
; get_address_ds
;-----------------------------------------------------------------------------
get_address_ds:
		push	ds
		pop	es
		call	get_address
		jc	@@exit

		mov	bx,[cpu_regs+REG_DS]
		cmp	ch,1
		je	@@exit
		cmp	ch,0
		je	@@def_seg
		xor	ax,ax
		jmp	@@exit

@@def_seg:
		mov	dx,bx

@@exit:
		ret
;=============================================================================
; get_address
;-----------------------------------------------------------------------------
get_address:
		call	get_address_
		jnc	@@exit

		mov	si,addr_error_msg
		call	_error_message
		stc
@@exit:
		mov	si,addr_dialog
		ret
;=============================================================================
; get_address_
;-----------------------------------------------------------------------------
get_address_:
		xor	bp,bp
		xor	dx,dx
		xor	cx,cx
		mov	[@@op],cl

@@next:
		lodsb
		cmp	al,' '
		je	@@next
		dec	si
		cmp	al,0
		je	@@done_1
		call	get_reg_value
		je	@@do_oper
		call	get_hex_value
		jc	@@error

@@do_oper:
		cmp	byte [@@op],'-'
		jne	@@plus
		neg	bx

@@plus:
		add	bp,bx
		mov	byte [@@op],0

@@skip:
		lodsb
		cmp	al,' '
		je	@@skip
		cmp	al,0
		je	@@done
		cmp	al,':'
		je	@@seg
		cmp	al,'+'
		je	@@p
		cmp	al,'-'
		jne	@@error

@@p:
		mov	[@@op],al
		jmp	@@next

@@seg:
		or	cl,cl
		jne	@@error
		inc	cx
		mov	dx,bp
		xor	bp,bp
		jmp	@@next

@@done_1:
		cmp	byte [@@op],0
		jne	@@error
		cmp	cx,1
		jne	@@ok
		inc	cx
@@ok:
		clc
		jmp	@@done

@@error:
@@error_exit:
		stc

@@done:
		mov	ax,bp
		mov	ch,cl
		ret

@@op		db	0
;=============================================================================
; get_reg_value
;-----------------------------------------------------------------------------
get_reg_value:
		push	ax
		push	cx
		mov	di,rreg
		mov	ax,[si]
		xchg	al,ah
		call	upcase
		xchg	al,ah
		call	upcase
		mov	cx,16
	repne	scasw
		jne	@@not
		cmp	ax,'GS'
		je	@@check_386
		cmp	ax,'FS'
		je	@@check_386
@@get_value:
		lodsw
		sub	di,rreg+2
		shl	di,1
		mov	bx,[cpu_regs+di]
		cmp	ax,ax
@@not:
		pop	cx
		pop	ax
		ret
@@check_386:
		cmp	byte [system_cpu],3
		jb	@@not
		jmp	@@get_value
;=============================================================================
; get_hex_value
;-----------------------------------------------------------------------------
get_hex_value:
		push	ax
		push	cx
		mov	cl,4
		xor	bx,bx
		mov	al,[si]
		call	ascii_to_bin
		jc	@@not
@@set:
		test	bh,0f0h
		stc
		jnz	@@not
		shl	bx,cl
		or	bl,al
		inc	si
		mov	al,[si]
		call	ascii_to_bin
		jnc	@@set
		clc
@@not:
		pop	cx
		pop	ax
		ret
;=============================================================================
; get_counter
;-----------------------------------------------------------------------------
get_counter:
		mov	si,count_string
		call	get_address_
		jc	@@error
		cmp	ch,0
		jne	@@error
		mov	[count_value],ax
		clc
		ret
@@error:
		mov	si,count_error_msg
		call	_error_message
		stc
		ret
;=============================================================================
; get_delay
;-----------------------------------------------------------------------------
get_delay:
		mov	si,delay_string
		call	get_address_
		jc	@@error
		cmp	ch,0
		jne	@@error
		mov	[delay_value],ax
		clc
		ret
@@error:
		mov	si,count_error_msg
		call	_error_message
		stc
		ret
;=============================================================================
; save_int_table
;-----------------------------------------------------------------------------
save_int_table:
		call	pushr
		mov	di,interrupt_table
;SaveInts:
		cli
		xor	si,si
		mov	ds,si
		cld
		mov	cx,512
	rep	movsw
		sti
		call	popr
		ret
;=============================================================================
; restore_int_table
;-----------------------------------------------------------------------------
restore_int_table:
		call	pushr
		mov	si,interrupt_table
;RestoreInts:
		cli
		xor	di,di
		mov	es,di
		cld
		mov	cx,512
	rep	movsw
		sti
		call	popr
		ret
;=============================================================================
; create_program
;-----------------------------------------------------------------------------
create_program:
		call	set_my_pid
		call	make_environment
		mov	bp,es
		mov	ah,48h
		mov	bx,0ffffh
		int	21h
		mov	ah,48h
		int	21h
		mov	[user_pid],ax
		mov	[save_dta_seg],ax
		mov	[cpu_regs+REG_CS],ax
		mov	[cpu_regs+REG_DS],ax
		mov	[cpu_regs+REG_ES],ax
		mov	[cpu_regs+REG_SS],ax
		mov	word [cpu_regs+REG_IP],100h
		mov	word [cpu_regs+REG_SP],-2
		mov	es,ax
		push	ax			;
		mov	word [es:0fffeh],0
		dec	ax
		mov	es,ax
		inc	ax
		mov	[es:1],ax
		mov	si,[2]
		mov	dx,ax
		mov	ah,55h
		int	21h
		pop	es			;
		mov	di,80h
		mov	si,my_cmd_line
		lodsb
		stosb
		mov	cl,al
		mov	ch,0
	rep	movsb
		movsb
		mov	cx,100h
		sub	cx,di
		mov	al,0
	rep	stosb
		mov	[es:2ch],bp
		dec	bp
		mov	ax,es
		mov	es,bp
		mov	[es:1],ax
		push	cs
		pop	es
		call	init_registers
		mov	al,[my_port_21]
		mov	[user_port_21],al
		call	set_user_pid
		mov	word [save_dta_offs],80h
		jmp	restore_dta
;=============================================================================
; make_environment
;-----------------------------------------------------------------------------
make_environment:
		mov	es,[2ch]
		xor	di,di
		mov	al,0
		cld
		mov	cx,-1
@@again:
	repne	scasb
		jne	@@error
		dec	cx
		scasb
		jne	@@again
		not	cx
		mov	bx,cx
		add	bx,30
		shr	bx,1
		shr	bx,1
		shr	bx,1
		shr	bx,1
		mov	ah,48h
		int	21h
		jc	@@error
		push	es
		pop	ds
		mov	es,ax
		xor	di,di
		xor	si,si
	rep	movsb
		mov	ax,1
		stosw
		mov	si,env_name
		push	cs
		pop	ds
@@next:
		lodsb
		stosb
		cmp	al,0
		jne	@@next
@@error:
		ret
;=============================================================================
; load_program
;-----------------------------------------------------------------------------
load_program:
		call	set_my_pid
		mov	[cmd_line_seg],cs
		mov	[fcb1_seg],cs
		mov	[fcb2_seg],cs
		mov	dx,filename
		mov	bx,epb
		mov	ax,4b01h
		int	21h
		jc	@@quit
		mov	ax,[user_ss]
		mov	[cpu_regs+REG_SS],ax
		mov	ax,[user_sp]
		cmp	ax,0fffeh
		jae	@@001
		inc	ax
		inc	ax
@@001:
		mov	[cpu_regs+REG_SP],ax
		mov	ax,[user_cs]
		mov	[cpu_regs+REG_CS],ax
		mov	ax,[user_ip]
		mov	[cpu_regs+REG_IP],ax

		mov	ah,51h
		int	21h
		mov	[user_pid],bx
		mov	[cpu_regs+REG_DS],bx
		mov	[cpu_regs+REG_ES],bx
		mov	es,bx
		mov	al,[es:80h]
		mov	ah,0
		inc	ax
		inc	ax
		mov	di,80h
		add	di,ax
		mov	cx,80h
		sub	cx,ax
		mov	al,0
		cld
	rep	stosb
		push	cs
		pop	es
		mov	byte [file_specified],1
		call	init_registers
		mov	al,[my_port_21]
		mov	[user_port_21],al
@@quit:
		ret
;=============================================================================

epb:
env_seg		dw	0
cmd_line_offs	dw	my_cmd_line
cmd_line_seg	dw	0
fcb1_offs	dw	my_fcb1
fcb1_seg	dw	0
fcb2_offs	dw	my_fcb2
fcb2_seg	dw	0
user_sp		dw	0
user_ss		dw	0
user_ip		dw	0
user_cs		dw	0

;MyCmdLine	db	0, 0Dh
my_fcb1:
my_fcb2:
		db	0
times	11	db	' '
;=============================================================================
; init_registers
;-----------------------------------------------------------------------------
init_registers:
		xor	ax,ax
		mov	[cpu_regs+REG_AX],ax
		mov	[cpu_regs+REG_BX],ax
		mov	word [cpu_regs+REG_CX],00ffh
		mov	ax,[cpu_regs+REG_DS]
		mov	[cpu_regs+REG_DX],ax
		mov	ax,[cpu_regs+REG_IP]
		mov	[cpu_regs+REG_SI],ax
		mov	ax,[cpu_regs+REG_SP]
		mov	[cpu_regs+REG_DI],ax
		mov	word [cpu_regs+REG_BP],91ch
		mov	word [cpu_regs+REG_FL],07202h
		jmp	save_regs
;=============================================================================
; read_cmd_line
;-----------------------------------------------------------------------------
read_cmd_line:
		call	pushr
		cmp	byte [file_specified],0
		jne	@@1
		mov	si,no_specif_msg
		call	_error_message
		jmp	@@quit
@@1:
		mov	si,arg_dialog
		call	init_dialog
		call	exec_dialog
		call	draw_screen
		or	ax,ax
		jz	@@quit
		call	set_command_line
		call	popr
		jmp	reset_program
@@quit:
		call	popr
		ret
;=============================================================================
; set_command_line
;-----------------------------------------------------------------------------
set_command_line:
		call	pushr
		mov	si,cmd_line_string
		call	strlen
		mov	di,my_cmd_line
		mov	al,cl
		stosb
	rep	movsb
		mov	al,13
		stosb
		call	popr
		ret
;=============================================================================
; load_file
;-----------------------------------------------------------------------------
load_file:
		mov	dx,file_mask
		call	init_file_list
		mov	si,load_dialog
		call	init_dialog
		call	exec_dialog
		call	draw_screen

		mov	si,[string_ptr]
		call	_check_shared_area

		or	ax,ax
		je	@@quit
		call	full_prog_name
		mov	byte [file_specified],1
		jmp	reset_program

@@quit:
		ret
;=============================================================================
; full_prog_name
;-----------------------------------------------------------------------------
full_prog_name:
		mov	si,filename
		mov	di,filename
		mov	ah,60h
		int	21h
		ret
;=============================================================================
; init_file_list
;-----------------------------------------------------------------------------
init_file_list:
		call	pushr
		call	save_dta
		call	set_my_pid
		call	set_my_dta
		mov	ax,16
		call	init_string_list
		mov	ah,4eh
		mov	cx,27h		;!!!
@@find_next:
		int	21h
		jc	@@end
		mov	si,9eh
		call	add_string
		mov	ah,4fh
		jmp	@@find_next
@@end:
		call	set_user_pid
		call	restore_dta
		call	popr
		ret
;=============================================================================
; reset_program
;-----------------------------------------------------------------------------
reset_program:
		call	pushr
		cmp	byte [file_specified],0
		je	@@no_spec
		call	unload_program
		call	load_program
		jnc	@@next
		mov	si,load_error_msg
		call	_error_message
		call	create_program
		mov	byte [file_specified],0
@@next:
		mov	ax,[cpu_regs+REG_IP]	;0leg, 2007-03-25
		mov	[cur_ip],ax		;Set initial addresses
		mov	ax,[cpu_regs+REG_CS]	;for Code and Dump windows
		mov	[unasm_seg],ax		;
		mov	ax,[cpu_regs+REG_DS]	;
		mov	[dump_seg],ax		;
		mov	word [cur_line],0	;
		mov	word [dump_offs],0	;
..@program_created:
		call	set_user_pid
		call	set_term_handler
		call	update_screen
@@quit:
		call	popr
		ret
@@no_spec:
		mov	si,no_specif_msg
		call	_error_message
		jmp	@@quit
;=============================================================================
; new_program
;-----------------------------------------------------------------------------
new_program:
		call	pushr
		mov	byte [cmd_line_string],0
		mov	word [my_cmd_line],0d00h
		call	unload_program
		call	clear_all_breaks
		call	create_program
		mov	ax,[cpu_regs+REG_IP]
		mov	[cur_ip],ax
		mov	ax,[cpu_regs+REG_CS]
		mov	[unasm_seg],ax
		mov	word [cur_line],0
		mov	byte [file_specified],0
		jmp	..@program_created
;=============================================================================
; set_user_pid
;-----------------------------------------------------------------------------
set_user_pid:
		mov	ah,50h
		mov	bx,[user_pid]
		int	21h
		ret
;=============================================================================
; set_my_pid
;-----------------------------------------------------------------------------
set_my_pid:
		mov	ah,50h
		mov	bx,cs
		int	21h
		ret
;=============================================================================
; save_dta
;-----------------------------------------------------------------------------
save_dta:
		push	bx
		push	es
		mov	ah,2fh
		int	21h
		mov	[save_dta_seg],es
		mov	[save_dta_offs],bx
		pop	es
		pop	bx
		ret
;=============================================================================
; set_my_dta
;-----------------------------------------------------------------------------
set_my_dta:
		push	dx
		push	cs
		pop	ds
		mov	dx,80h
		mov	ah,1ah
		int	21h
		pop	dx
		ret
;=============================================================================
; restore_dta
;-----------------------------------------------------------------------------
restore_dta:
		mov	ah,1ah
		push	ds
		mov	dx,[save_dta_offs]
		mov	ds,[save_dta_seg]
		int	21h
		pop	ds
		ret
;=============================================================================
; unload_program
;-----------------------------------------------------------------------------
unload_program:
		call	set_user_pid
		mov	es,[user_pid]
		mov	[es:0ch],cs
		mov	ax,@@1
		mov	[es:0ah],ax
		pushf
		push	cs
		push	ax
		push	es
		push	ds
		push	bp
		push	di
		push	si
		push	dx
		push	cx
		push	bx
		push	ax
		mov	[2eh],sp
		mov	[30h],ss
		mov	ax,4c00h
		int	21h
@@1:
		push	cs
		pop	ds
		push	cs
		pop	es
		ret
;=============================================================================
; set_term_handler
;-----------------------------------------------------------------------------
set_term_handler:
		mov	es,[user_pid]
		mov	[es:0ch],cs
		mov	word [es:0ah],term_handler

;		mov	ds:[2Eh],StackPtr-24
		xor	cx,cx
		call	calc_prog_size
		sub	ax,24
		mov	[2eh],ax

		mov	[30h],ss
		push	cs
		pop	es
		ret
;=============================================================================
; term_handler
;-----------------------------------------------------------------------------
term_handler:
;		mov	sp,StackPtr

		mov	ax,cs
		mov	ds,ax
		mov	es,ax

		xor	cx,cx
		call	calc_prog_size
		mov	sp,ax

		call	restore_01_vector
		call	restore_03_vector
;		call	SaveUserInts
;		call	RestoreIntTable
		call	clear_breakpoints
		call	save_screen
		call	create_program
;		call	SetUserPID
		call	set_term_handler
;		mov	cur_line,0
		mov	byte [focus],1	;0leg, 2007-03-25
		call	draw_screen
;-----------------------------------------------------------------------------
; Get return code
;-----------------------------------------------------------------------------
		mov	di,return_code
		mov	ax,DOS_GET_RETURN_CODE * 256 + '0'
		stosb
		stosb
		stosb
		int	21h
		add	ah,'0'
		mov	[term_method],ah
@@loop99:
		dec	di
		aam			;AH = AL / 10;  AL = AL % 10
		add	[di],al
		xchg	al,ah
		or	al,al
		jnz	@@loop99
;-----------------------------------------------------------------------------
		mov	si,terminate_msg
		call	_msg_box
		jmp	main_key_loop
;=============================================================================
; E0F
;=============================================================================


