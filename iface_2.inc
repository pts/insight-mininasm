;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; iface_2.inc
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
; 2008-11-03	0leg	Fixed Alt+L in the Dump window, added Alt+F5, Alt+U.
;			Added Up/Down to the Flags window.
; 2009-01-08	0leg	GPF on BP/SP modification: fixed.
;=============================================================================


;=============================================================================
; draw_cpu_window
;-----------------------------------------------------------------------------
draw_cpu_window:
		call	pushr
		call	no_cursor
		mov	es,[video_seg]
		xor	di,di
		cld
		mov	cx,SCR_COLS*SCR_ROWS
		test	byte [cmd_line_options],CMD_EGA
		jz	@@not_ega
		add	cx,SCR_COLS*(MAX_SCR_ROWS-SCR_ROWS)
@@not_ega:
		mov	ax,atMainWindow*100h+20h
	rep	stosw

		test	byte [large_dump],1
		jz	@@not_quit
		jmp	@@quit
@@not_quit:
;;;;		jnz	@@quit			;!!!

		mov	di,CPU_START
		mov	al,'Í'
		mov	ah,atCPUborder
		mov	cx,CPU_WIDTH
	rep	stosw
		mov	al,'¸'
		stosw
		mov	al,'³'
		mov	cx,CPU_HEIGHT

@@next_bord_line:
		add	di,(SCR_COLS-1)*2
		stosw
		loop	@@next_bord_line

		mov	di,CPU_START+(CPU_HEIGHT+1)*SCR_COLS*2
		mov	al,'Ä'
		mov	cx,USED_COLS
	rep	stosw
		mov	di,CPU_START+((CPU_HEIGHT+1)*SCR_COLS+CPU_WIDTH)*2
		mov	al,'Á'
		stosw

		mov	bx,FLAG16_START
		cmp	byte [regs_mode],1
		jne	@@skip_32
		mov	bx,FLAG32_START
@@skip_32:
		lea	di,[bx-SCR_COLS*2-2]

		mov	al,'Ã'
		stosw
		mov	cx,USED_COLS-1-CPU_WIDTH
		mov	al,'Ä'
	rep	stosw

		lea	di,[bx+2*SCR_COLS*2-2]

		mov	al,'Ã'
		stosw
		mov	cx,USED_COLS-1-CPU_WIDTH
		mov	al,'Ä'
	rep	stosw

;flags names
		mov	ah,atRegName
		lea	di,[bx+2]
		mov	si,flags_str
		mov	cx,NUM_FLAGS
@@loop01:
		lodsb
		stosw
		mov	al,'F'
		stosw
		scasw
		loop	@@loop01

		mov	ax,40-TITLE_LEN/2
		mov	bh,atTitle
		mov	si,main_title
		call	write_string

		mov	ax,CPU_START+5
		mov	bh,atCPUName
		mov	si,[cpu_name]
		call	write_string

		cmp	byte [regs_mode],1
		je	@@regs_32
		call	draw_registers_16
@@exit:
		mov	si,flags_msg
		call	fill_str
@@quit:
		call	popr
		ret

@@regs_32:
		call	draw_registers_32
		jmp	@@exit
;=============================================================================
; draw_registers_16
;-----------------------------------------------------------------------------
draw_registers_16:
		mov	di,REG_START
		mov	si,rreg
		mov	ah,atRegName
		mov	cx,4

@@next_reg_line:
		push	cx
		mov	cx,3

@@next_reg_col:
		lodsb
		stosw
		lodsb
		stosw
		mov	al,'='
		stosw
		add	di,12
		add	si,6
		loop	@@next_reg_col

		add	di,SCR_COLS*2-18*3
		pop	cx
		sub	si,22
		loop	@@next_reg_line

		add	di,SCR_COLS*2
		mov	al,'I'
		stosw
		mov	al,'P'
		stosw
		mov	al,'='
		stosw
		add	di,24
		ret
;=============================================================================
; draw_registers_32
;-----------------------------------------------------------------------------
draw_registers_32:
		mov	di,REG_START
		mov	si,rreg		;1st column
		mov	bp,rreg_segs	;2nd column
		mov	ah,atRegName
		mov	cx,6

@@next_reg_32:
		call	@@display_names
		loop	@@next_reg_32

		add	di,SCR_COLS*2
		call	@@display_names
		call	@@display_names

		sub	di,(SCR_COLS-15)*2
		ret

@@display_names:
		push	di
		mov	al,'E'
		stosw
		lodsb
		stosw
		lodsb
		stosw
		mov	al,'='
		stosw
		add	di,4*2
		mov	al,':'
		stosw
		add	di,18
		xchg	si,bp
		lodsb
		stosw
		lodsb
		stosw
		mov	al,'='
		stosw
		xchg	si,bp
		pop	di
		add	di,SCR_COLS*2
		ret
;=============================================================================
; update_watch_label
;-----------------------------------------------------------------------------
update_watch_label:
		cmp	byte [large_dump],0
		jz	@@upd
		ret
@@upd:
		call	pushr

		mov	es,[video_seg]
		cld
		mov	di,WATCH_START

		cmp	word [data_watch_proc],0
		je	@@draw_line

		mov	si,[data_watch_label]
		mov	ax,atDataWatchMem*256 + ' '
		stosw

		lodsb
		stosw
		lodsb
		stosw

		mov	al,':'
		stosw

		lodsb
		stosw
		lodsb
		stosw

		mov	al,' '
		stosw
@@exit:
		call	popr
		ret

@@draw_line:
		mov	ax,atCPUborder*256 + 'Ä'
		mov	cx,7
	rep	stosw
		jmp	@@exit
;=============================================================================
; fill_str
;-----------------------------------------------------------------------------
fill_str:
@@next:
		lodsb
		cmp	al,0
		je	@@quit
		stosw
		jmp	@@next
@@quit:
		ret
;=============================================================================
; update_registers
;-----------------------------------------------------------------------------
update_registers:
		cmp	byte [large_dump],0
		jz	@@upd
		ret
@@upd:
		call	pushr
		mov	es,[video_seg]

		mov	ax,FLAG16_START+SCR_COLS*2+4
		mov	cx,NUM_16_REGS
		mov	si,reg16_pos_table
		cmp	byte [regs_mode],1
		jne	@@skip_32
		mov	ax,FLAG32_START+SCR_COLS*2+4
		mov	cl,NUM_32_REGS		;CL instead of CX
		mov	si,reg32_pos_table
@@skip_32:
		push	ax

@@next_reg:
		xor	dx,dx
		mov	dl,[si+REG_POS_X]
		shl	dx,1
		mov	al,SCR_COLS*2
		mul	byte [si+REG_POS_Y]
		add	dx,ax
		mov	di,[si+REG_POS_LINK]

		mov	ax,[di]

		mov	bh,atRegNewValue
		cmp	[di+save_cpu_regs-cpu_regs],ax
		jne	@@fill
		mov	bh,atRegValue
		cmp	byte [regs_mode],1
		jne	@@fill
		call	align_reg
		push	ax
cpu	386
		mov	eax,[di]
		cmp	[di+save_cpu_regs-cpu_regs],eax
cpu	8086
		pop	ax
		je	@@fill
		mov	bh,atRegNewValue
@@fill:
		mov	di,dx
		call	fill_word
		add	si,SIZEOF_REG_POS
		loop	@@next_reg

		pop	di
;-----------------------------------------------------------------------------
@@draw_flags:
		mov	si,0000111011010101b
		;	   ----oditsz-a-p-c
		mov	cx,NUM_FLAGS

		mov	bx,[cpu_regs+REG_FL]
		mov	bp,[save_cpu_regs+REG_FL]
@@next_flag:
@@next_bit:
		mov	ax,atFlagValue*100h+'0'
		mov	dx,ax
		shl	bp,1
		adc	dl,ch		;CH instead of 0
		shl	bx,1
		adc	al,0
		shl	si,1
		jnc	@@next_bit

		xor	dx,ax
		jz	@@display_flag
		mov	ah,atFlagNewValue
@@display_flag:
		stosw
		scasw
		scasw
		loop	@@next_flag

		call	popr
		ret
;=============================================================================
; align_reg
;-----------------------------------------------------------------------------
align_reg:
		sub	di,cpu_regs
		and	di,01111100b
		add	di,cpu_regs
		ret
;=============================================================================
; update_commands
;-----------------------------------------------------------------------------
update_commands:
		cmp	byte [large_dump],0
		jz	@@upd
		ret
@@upd:
		call	pushr
		mov	byte [cur_line_brk],0
		mov	word [cur_line_ip],0
		mov	word [cur_ip_line],-1
		mov	si,[cur_ip]
		mov	di,CMD_START
		mov	es,[video_seg]
		mov	cx,CPU_HEIGHT
		xor	bp,bp

next_cpu_line:
		xor	dx,dx
		mov	bx,[unasm_seg]
		cmp	bx,[cpu_regs+REG_CS]
		jne	@@1
		cmp	si,[cpu_regs+REG_IP]
		jne	@@1
		inc	dx
		mov	[cur_ip_line],bp
@@1:
		call	check_break
		jc	@@8
		mov	dh,1		;Breakpoint exist
		jz	@@8		;2008-03-26, JE => JZ
		mov	dh,2		;Breakpoint disabled
@@8:
		mov	bx,si
		call	unassemble
		mov	ax,cmd_normal
		cmp	dh,1
		jb	@@a
		mov	ax,cmd_breakpoint
		jz	@@a
		mov	ax,cmd_brk_disabled
@@a:
		or	dl,dl
		jz	@@2
		mov	word [buffer+4],1011h	;
@@2:
		cmp	bp,[cur_line]
		jne	@@display
		cmp	byte [focus],1
		jne	@@e
		cmp	ax,cmd_breakpoint
		mov	ax,cmd_selected
		jb	@@e
		mov	byte [cur_line_brk],1
		mov	ax,cmd_brk_selected
		je	@@e
		mov	ax,cmd_brk_dis_sel
@@e:
		mov	[cur_line_ip],bx
		call	update_cmd_mem

@@display:
		call	@@displayy
		call	get_ref_addresses
		call	set_direction
		call	display_cpu_line

		add	di,SCR_COLS*2
		inc	bp

		call	display_code_marks

		loop	next_cpu_line

		test	byte [cs:edit_mode],ED_STACK+ED_DUMP	;2008-03-23
		jnz	@@quit					;2008-03-23

		add	di,10
		mov	cx,[unasm_seg]
		mov	dx,[cur_line_ip]
		call	show_address
@@quit:
		call	popr
		ret
;-----------------------------------------------------------------------------
@@displayy:
		cmp	byte [focus],1
		je	@@focus
		cmp	bp,[cur_line]
		jne	@@focus
		cmp	ax,cmd_breakpoint
		jae	@@focus
		mov	ax,cmd_not_focus
@@focus:
		ret
;-----------------------------------------------------------------------------

show_address:
		mov	ax,atCurUnasmAddr*100h+' '
		stosw
show_address_2:
		mov	bh,atCurUnasmAddr
		mov	ax,cx
		call	fill_word
		mov	ax,atCurUnasmAddr*100h+':'
		stosw
		mov	ax,dx
		call	fill_word
		mov	ax,atCurUnasmAddr*100h+' '
		stosw
		ret

;=============================================================================
; display_cpu_line
;-----------------------------------------------------------------------------
display_cpu_line:
		call	pushr
		xchg	ax,bp
		mov	si,buffer
		mov	dx,[cur_x]
		mov	cx,CPU_WIDTH-1

		mov	ah,[bp]
		mov	bx,CPU_ADDR_SIZE
		call	@@display

		mov	ah,[bp+1]
		mov	bx,CPU_OPCODE_SIZE
		call	@@display

		call	@@display_command

		call	popr
		ret

@@display:
		xchg	bx,cx
@@next_char:
		lodsb
		or	dx,dx
		jz	@@write_char
		dec	dx
		jmp	@@update_cx

@@write_char:
		stosw
		dec	bx

@@update_cx:
		loop	@@next_char
		xchg	bx,cx
		ret

@@display_command:
		lodsb

		mov	ah,[bp+4]
		cmp	al,18h
		je	@@write
		cmp	al,19h
		je	@@write

		mov	ah,[bp+5]
		cmp	al,1eh
		je	@@disabled_arrow
		cmp	al,1fh
		je	@@disabled_arrow

		mov	ah,[bp+2]
		cmp	al,'0'
		jb	@@write
		cmp	al,'9'
		jbe	@@digit
		cmp	al,'A'
		jb	@@write
		cmp	al,'F'
		ja	@@write
@@digit:
		mov	ah,[bp+3]

@@write:
		or	dx,dx
		jz	@@stosw
		dec	dx
		jmp	@@display_command
@@stosw:
		stosw
		loop	@@display_command
		ret

@@disabled_arrow:
		sub	al,1eh-18h
		jmp	@@write
;=============================================================================
; update_cmd_mem
;-----------------------------------------------------------------------------
update_cmd_mem:
		call	pushr
		mov	es,[video_seg]
		mov	di,MEM_START
		mov	al,'Ä'
		mov	ah,atCPUborder
		mov	cx,21
	rep	stosw
		cmp	byte [memory_flag],0
		je	@@exit
		call	calc_mem_address
		call	disp_mem_addr
		mov	bl,[memory_flag]
		mov	bh,0
		dec	bx
		cmp	bx,2
		ja	@@exit
		mov	ds,dx
		add	bx,bx
		call	[cs:display_mem_disp+bx]
@@exit:
		call	popr
		ret

calc_mem_disp	dw	calc_bxsi
		dw	calc_bxdi
		dw	calc_bpsi
		dw	calc_bpdi
		dw	calc_si
		dw	calc_di
		dw	calc_bp
		dw	calc_bx
		dw	calc_mem

calc_bxsi:	mov	ax,[cpu_regs+REG_BX]
		add	ax,[cpu_regs+REG_SI]
		ret

calc_bxdi:	mov	ax,[cpu_regs+REG_BX]
		add	ax,[cpu_regs+REG_DI]
		ret

calc_bpsi:	mov	ax,[cpu_regs+REG_BP]
		add	ax,[cpu_regs+REG_SI]
..@ss:		mov	dx,[cpu_regs+REG_SS]
		mov	cl,'S'
		ret

calc_bpdi:	mov	ax,[cpu_regs+REG_BP]
		add	ax,[cpu_regs+REG_DI]
		jmp	..@ss

calc_si:	mov	ax,[cpu_regs+REG_SI]
		ret

calc_di:	mov	ax,[cpu_regs+REG_DI]
		ret

calc_bx:	mov	ax,[cpu_regs+REG_BX]
		ret

calc_bp:	mov	ax,[cpu_regs+REG_BP]
		jmp	..@ss

calc_mem:	xor	ax,ax
		ret
;=============================================================================
; calc_mem_address
;-----------------------------------------------------------------------------
calc_mem_address:
		mov	bl,[mem_disp_type]
		mov	bh,0
		shl	bx,1
		mov	dx,[cpu_regs+REG_DS]
		mov	cl,'D'
		call	[calc_mem_disp+bx]
		add	ax,[mem_disp_offs]
		mov	si,ax
		mov	ax,[mem_prefix]
		or	ax,ax
		jz	@@exit

		cmp	ax,pfs
		jb	@@set_prefix
		cmp	byte [system_cpu],3
		jb	@@exit
		cmp	byte [cpu_type],a386
		jb	@@exit

@@set_prefix:
		mov     bx,ax
		mov	cl,[bx]
		sub	cl,'a'-'A'
		add	bx,cpu_regs+REG_CS-pcs
		mov	dx,[bx]
@@exit:
		ret
;=============================================================================

display_mem_disp:
		dw	disp_byte
		dw	disp_word
		dw	disp_dword
disp_byte:
		mov	bh,ah
		lodsb
		call	fill_byte
		jmp	..@rest
disp_word:
		mov	bh,ah
		cmp	si,-1
		jne	@@lodsw
		lodsb
		mov	ah,al
		lodsb
@@lodsw:
		lodsw
		call	fill_word
		jmp	..@rest
disp_dword:
		mov	bh,ah
		lodsw
		xchg	ax,cx
		lodsw
		call	fill_word
		mov	ah,bh
		mov	al,':'
		stosw
		xchg	ax,cx
		call	fill_word
..@rest:
		mov	ah,bh
		mov	al,' '
		stosb
		ret
;=============================================================================
; disp_mem_addr
;-----------------------------------------------------------------------------
disp_mem_addr:
		mov	es,[video_seg]
		mov	di,MEM_START
		mov	ah,atMemAddr
		mov	al,' '
		stosw
		mov	al,cl
		stosw
		mov	al,'S'
		stosw
		mov	al,':'
		stosw
		mov	bh,ah
		mov	ax,si
		call	fill_word
		mov	ah,bh
		mov	al,' '
		stosw
		mov	al,'='
		stosw
		mov	al,' '
		stosw
		mov	ah,atMemValue
		ret
;-----------------------------------------------------------------------------
expand_dump:
		mov	word [dump_start],SCR_COLS*2	;1st row
		mov	ax,[screen_height]
		mov	[dump_height],ax
		mov	byte [dump_y],1
		mov	byte [large_dump],1
		ret
;-----------------------------------------------------------------------------
shrink_dump:
		mov	word [dump_start],DUMP_START
		mov	ax,[e_dump_height]
		mov	[dump_height],ax
		mov	byte [dump_y],DUMP_Y
		mov	byte [large_dump],0
		ret
;-----------------------------------------------------------------------------

zoom_dump:
		test	byte [large_dump],1
		jz	@@enlarge

		mov	ax,[e_dump_height]
		mov	cl,4
		shl	ax,cl
		add	ax,[dump_offs]
		cmp	si,ax
		jb	@@ok
		mov	ax,si
		sub	ax,[dump_offs]
		and	ax,0fff0h
		add	[dump_offs],ax
		mov	dh,0
@@ok:
		call	shrink_dump
		jmp	draw_screen
@@enlarge:
		mov	es,[video_seg]
		xor	di,di
		cld
		mov	cx,SCR_COLS*SCR_ROWS
		test	byte [cmd_line_options],CMD_EGA
		jz	@@not_ega
		add	cx,SCR_COLS*(MAX_SCR_ROWS-SCR_ROWS)
@@not_ega:
		mov	ax,atMainWindow*100h+20h
	rep	stosw
		call	expand_dump

;=============================================================================
; _update_dump
;-----------------------------------------------------------------------------
_update_dump:
		cmp	word [data_watch_proc],0
		je	update_dump
		jmp	word [data_watch_proc]
update_dump:
		call	pushr
		mov	si,[dump_offs]
		mov	es,[video_seg]

		mov	ax,si
;-----------------------------------------------------------------------------
; Draw line of hex digits above dump
;-----------------------------------------------------------------------------
		mov	ah,atDumpAddr
		mov	bp,[dump_start]
		mov	di,bp
		sub	bp,SCR_COLS*2-(DUMP_X+13+16*3)*2
		sub	di,SCR_COLS*2-(DUMP_X+12)*2
		mov	cx,16
@@next:
		and	al,0fh
		push	ax
		cmp	al,10
		sbb	al,69h
		das
		stosw
		xchg	di,bp
		stosw
		xchg	di,bp
		pop	ax
		scasw
		scasw
		inc	ax
		loop	@@next
;-----------------------------------------------------------------------------
		mov	ds,[dump_seg]
		mov	di,[cs:dump_start]
		mov	cx,[cs:dump_height]
		sub	dl,16
		neg	dl

		push	dx

@@next_row:
		mov	bh,atDumpAddr
		mov	ax,ds
		call	fill_word
		mov	al,':'
		mov	ah,bh
		stosw
		mov	ax,si
		call	fill_word
		scasw
		scasw
		mov	bl,atDumpValue
		mov	ah,atDumpCursor
		push	cx
		mov	cx,16
@@next_col:
		mov	bh,bl
		lodsb
		or	al,al
		jnz	@@nz
		mov	bh,atDumpZero
@@nz:
		test	byte [cs:edit_mode],ED_DUMP
		jz	@@not_cur_1
		cmp	dh,0
		jne	@@not_cur_1
		cmp	dl,cl
		jne	@@not_cur_1
		mov	bh,ah
@@not_cur_1:
		call	fill_byte
		scasw
		loop	@@next_col
		pop	cx
		dec	dh
		add	di,(SCR_COLS-59)*2
		loop	@@next_row

		pop	dx
		mov	bh,atDumpValue
		mov	bl,atDumpCursor

		mov	si,[cs:dump_offs]
		mov	di,[cs:dump_start]
		add	di,61*2			;!!!
		mov	cx,[cs:dump_height]
@@next_row_1:
		push	cx
		mov	cx,16
@@next_col_1:
		mov	ah,bh
		lodsb
		or	al,al
		jnz	@@1
		mov	ah,atDumpZero
		mov	al,'.'
@@1:
		test	byte [cs:edit_mode],ED_DUMP
		jz	@@not_cur_2
		cmp	dh,0
		jne	@@not_cur_2
		cmp	dl,cl
		jne	@@not_cur_2
		mov	ah,bl
@@not_cur_2:
		stosw
		loop	@@next_col_1

		pop	cx
		dec	dh
		add	di,(SCR_COLS-16)*2
		loop	@@next_row_1

;-----------------------------------------------------------------------------
		test	byte [cs:edit_mode],ED_DUMP
		jz	@@quit
		mov	cx,[cs:dump_seg]
		mov	dx,[cs:dump_address]
		mov	di,DUMP_START-SCR_COLS*4+10
		cmp	byte [cs:large_dump],1
		jne	@@show
		xor	di,di
		call	show_address_2
		jmp	@@quit
@@show:
		call	show_address
@@quit:
;-----------------------------------------------------------------------------

		call	popr
		ret
;=============================================================================
; update_stack
;-----------------------------------------------------------------------------
update_stack:
		cmp	byte [large_dump],0
		jz	@@ok
		jmp	@@quit
@@ok:

		call	pushr

		mov	ax,[cpu_regs+REG_SP]
		test	byte [edit_mode],ED_STACK
		jnz	@@0001
		mov	[initial_sp_val],ax
@@0001:

		mov	di,STK_START
		mov	cx,[stack_height]
		mov	si,[initial_sp_val]
		mov	bp,[initial_bp_disp]
		mov	es,[video_seg]
		mov	ds,[cpu_regs+REG_SS]

@@next:
		mov	ax,atStackAddr*256+'S'
		stosw
		stosw
		mov	al,':'
		stosw
		xchg	ax,bx		;Instead of MOV BH,AH
		mov	ax,si
		call	fill_word	;SP value

		mov	ax,atStackName*256+10h
		cmp	si,[cs:cpu_regs+REG_SP]
		je	@@st0
		mov	al,' '
@@st0:
		stosw
		mov	bh,atStackValue
		lodsb
		mov	ah,al
		lodsb
		xchg	ah,al
		call	fill_word	;[SS:SP]

		scasw
		scasw

		mov	ax,atStackAddr*256+'+'
		mov	bx,bp
		or	bp,bp
		jns	@@st1
		mov	al,'-'
		neg	bx
@@st1:
		stosw
		xchg	ax,bx		;BH := AH (color); AX := BX (displ-t)
		call	fill_word	;Displacement
		scasw
		mov	bx,bp
		add	bx,[cs:cpu_regs+REG_BP]
		mov	al,[bx]
		mov	ah,[bx+1]
		mov	bh,atStackValue
		call	fill_word	;[SS:BP+xxxx]
		add	bp,2

		sub	di,(SCR_COLS+24)*2
		loop	@@next

		test	byte [cs:edit_mode],ED_STACK
		jz	@@dont_show
		mov	cx,[cs:cpu_regs+REG_SS]
		mov	bp,[cs:stack_mode]
		mov	dx,word [cs:bp+3]
		mov	di,DUMP_START-SCR_COLS*4+10
		call	show_address
@@dont_show:

		call	popr
@@quit:
		ret
;=============================================================================
; update_regs_color
;-----------------------------------------------------------------------------
update_regs_color:
		cmp	byte [regs_saved],1
		je	@@exit
		inc	byte [regs_saved]
		call	save_regs
		call	update_registers
@@exit:
		ret
;=============================================================================
; display_focus
;-----------------------------------------------------------------------------
display_focus:
		inc	byte [focus]
		jmp	update_commands
;=============================================================================
; hide_focus
;-----------------------------------------------------------------------------
hide_focus:
		dec	byte [focus]
		jmp	update_commands
;=============================================================================
; edit_registers
;-----------------------------------------------------------------------------
edit_registers:
		call	hide_focus
		call	update_regs_color
		call	large_cursor

@@start_edit:
		mov	si,reg16_pos_table
		cmp	byte [regs_mode],1
		jne	@@skip_32
		mov	si,reg32_pos_table
@@skip_32:
		xor	dx,dx

@@next:
		push	dx
		add	dx,[si]
		call	gotoxy
		pop	dx

@@read_key:
		call	read_key

		mov	di,@@actions - 3
		call	action_short_jump
		jc	@@other
		add	di,@@first_action
		jmp	di

@@first_action:
; 0..9, A..F, a..f
@@other:
		call	ascii_to_bin
		jc	@@next
		mov	di,[si+REG_POS_LINK]
		call	set_half_byte
		inc	dl
		cmp	dl,4
		jae	@@move

@@update:
;		call	update_regs_and_cmds
		call	update_screen	;We need to update stack window also...
		jmp	@@next
; Tab
@@move:
		mov	al,[si+REG_POS_RIGHT]
		xor	dx,dx
		jmp	@@get_next
; Shift+Tab
@@move_back:
		mov	al,[si+REG_POS_LEFT]
		mov	dl,0
		jmp	@@get_next
; Right
@@right:
		inc	dl
		cmp	dl,3
		ja	@@move
		jmp	@@next
; Left
@@left:
		or	dl,dl
		jnz	@@dec
		mov	al,[si+REG_POS_LEFT]
		mov	dl,3
		jmp	@@get_next
@@dec:
		dec	dl
		jmp	@@next
; Up
@@up:
		mov	al,[si+REG_POS_UP]
		jmp	@@get_next
; Down
@@down:
		mov	al,[si+REG_POS_DOWN]
		jmp	@@get_next
; Alt+F10
@@local_menu:
		push	si
		mov	si,regs_local_struc
		call	_local_menu
		pop	si
		jc	@@next
		jmp	ax
; Ctrl+R
@@switch_reg_mode:
		call	switch_reg_mode
		jmp	@@start_edit
; Ctrl+A
@@restore_all:
		push	si
		call	restore_all
		pop	si
		jmp	@@next
; Ctrl+T
@@restore_flags:
		call	restore_flags
		jmp	@@next
; Ctrl+C
@@restore_csip:
		call	restore_csip		;[0leg, 2006-12-31]
		jmp	@@next			;Was: restore_flags
; Ctrl+I
@@inc_reg:
		mov	di,[si+REG_POS_LINK]
		cmp	byte [regs_mode],1
		je	@@inc_32
@@inc_16:
		inc	word [di]
		jmp	@@update
@@inc_32:
		cmp	di,cpu_regs+REG_CS
		jae	@@inc_16
		call	align_reg
cpu	386
		inc	dword [di]
cpu	8086
		jmp	@@update
; Ctrl+D
@@dec_reg:
		mov	di,[si+REG_POS_LINK]
		cmp	byte [regs_mode],1
		je	@@dec_32
@@dec_16:
		dec	word [di]
		jmp	@@update
@@dec_32:
		cmp	di,cpu_regs+REG_CS
		jae	@@dec_16
		call	align_reg
cpu	386
		dec	dword [di]
cpu	8086
		jmp	@@update
; Ctrl+Z
@@zero_reg:
		mov	di,[si+REG_POS_LINK]
		call	align_reg
		mov	word [di],0
		mov	word [di+2],0
		jmp	@@update
; Ctrl+O
@@restore_reg:
		mov	di,[si+REG_POS_LINK]
		call	align_reg
		mov	ax,[di+save_cpu_regs-cpu_regs]
		mov	[di],ax
		mov	ax,[di+2+save_cpu_regs-cpu_regs]
		mov	[di+2],ax
		jmp	@@update
; ESC, Enter
@@quit:
		call	no_cursor
		call	update_screen
		jmp	display_focus

@@get_next:	cbw
		shl	ax,1
		shl	ax,1
		shl	ax,1
		add	si,ax
		jmp	@@update

@@actions	dw	kbEsc
		db	@@quit		- @@first_action
		dw	kbEnter
		db	@@quit		- @@first_action
		dw	kbTab
		db	@@move		- @@first_action
		dw	kbShiftTab
		db	@@move_back	- @@first_action
		dw	kbRight
		db	@@right		- @@first_action
		dw	kbLeft
		db	@@left		- @@first_action
		dw	kbUp
		db	@@up		- @@first_action
		dw	kbDown
		db	@@down		- @@first_action
		dw	kbCtrlA
		db	@@restore_all	- @@first_action
		dw	kbCtrlO
		db	@@restore_reg	- @@first_action
		dw	kbCtrlT
		db	@@restore_flags	- @@first_action
		dw	kbCtrlC
		db	@@restore_csip	- @@first_action
		dw	kbCtrlR
		db	@@switch_reg_mode - @@first_action
		dw	kbCtrlI
		db	@@inc_reg	- @@first_action
		dw	kbCtrlD
		db	@@dec_reg	- @@first_action
		dw	kbCtrlZ
		db	@@zero_reg	- @@first_action
		dw	kbAltF10
		db	@@local_menu	- @@first_action
		dw	0

_switch_reg_mode	equ	@@switch_reg_mode
_restore_all		equ	@@restore_all
_restore_flags		equ	@@restore_flags
_restore_csip		equ	@@restore_csip
inc_reg			equ	@@inc_reg
dec_reg			equ	@@dec_reg
zero_reg		equ	@@zero_reg
restore_reg		equ	@@restore_reg

;=============================================================================
; set_half_byte
;-----------------------------------------------------------------------------
; Changes nybble in a word.
; In:	DL -- number of nybble: 0..3
;	DS:DI -> word to edit
;	AL -- value of nybble: 0..F
; Out:	DS:DI -> changed word
; Modf:	AX, BX, CL
; Call:	---
; Use:	---
;
set_half_byte:
		mov	bx,0fff0h
		mov	cl,3
		sub	cl,dl
		shl	cl,1
		shl	cl,1
		rol	bx,cl
		and	bx,[di]
		mov	ah,0
		shl	ax,cl
		or	ax,bx
		mov	[di],ax
		ret
;=============================================================================
; toggle_flag
;-----------------------------------------------------------------------------
toggle_flag:
		call	get_flag_mask
		xor	[cpu_regs+REG_FL],ax
		ret

get_flag_mask:
		mov	cl,[reg_shift+bx]
		mov	ax,1
		shl	ax,cl
		ret
;=============================================================================
; edit_flags
;-----------------------------------------------------------------------------
edit_flags:
		call	hide_focus
		call	update_regs_color
		call	large_cursor
		xor	bx,bx
@@next:
		mov	dx,(STK_BOT_Y-1)*256+FLAG16_X+2
		sub	dh,[stack_height]
		add	dl,bl
		add	dl,bl
		add	dl,bl
		call	gotoxy
		call	read_key

		mov	di,@@actions - 3
		call	action_short_jump
		jc	@@next
		add	di,@@first_action
		jmp	di

@@first_action:
; 0
@@clear:
		call	get_flag_mask
		not	ax
		and	[cpu_regs+REG_FL],ax
		jmp	@@update
; 1
@@set:
		call	get_flag_mask
		or	[cpu_regs+REG_FL],ax
		jmp	@@update
; Alt+F10
@@local_menu:
		mov	si,flags_local_struc
		call	_local_menu
		jc	@@next
		call	ax
		jmp	@@update
; Space
@@toggle:
		call	toggle_flag
@@update:
		call	update_regs_and_cmds
		jmp	@@next
; Right
@@right:
		inc	bl
		cmp	bl,NUM_FLAGS
		jb	@@next
		mov	bl,0
		jmp	@@next
; Left
@@left:
		dec	bl
		jns	@@next
		mov	bl,NUM_FLAGS-1
		jmp	@@next
; Ctrl+T
@@restore_flags:
		call	restore_flags
		jmp	@@next
; ESC, Enter
@@quit:
		call	no_cursor
		jmp	display_focus


@@actions	dw	kbEsc
		db	@@quit		- @@first_action
		dw	kbEnter
		db	@@quit		- @@first_action
		dw	kbRight
		db	@@right		- @@first_action
		dw	kbLeft
		db	@@left		- @@first_action
		dw	kbDown
		db	@@clear		- @@first_action
		dw	kbUp
		db	@@set		- @@first_action
		dw	0b00h+'0'
		db	@@clear		- @@first_action
		dw	0200h+'1'
		db	@@set		- @@first_action
		dw	3900h+' '
		db	@@toggle	- @@first_action
		dw	kbCtrlT
		db	@@restore_flags	- @@first_action
		dw	kbAltF10
		db	@@local_menu	- @@first_action
		dw	0
;=============================================================================
; edit_dump
;-----------------------------------------------------------------------------
edit_dump:
		or	byte [edit_mode],ED_DUMP
		call	hide_focus

		push	es

@@next_r:
		mov	si,[dump_offs]
		mov	[cs:dump_address],si	;2008-03-23
		mov	es,[dump_seg]
		xor	bx,bx
		xor	dx,dx
		xor	bp,bp

		call	update_dump

@@next:
		call	large_cursor
		push	dx
		or	bp,bp
		jz	@@hex_cur
		add	dl,DUMP_X+11+48+2
		jmp	@@set_cur
@@hex_cur:
		mov	al,dl
		add	dl,al
		add	dl,al
		add	dl,bl
		add	dl,DUMP_X+11
@@set_cur:
		add	dh,[dump_y]
		call	gotoxy
		pop	dx

@@read_key:
		call	read_key
		mov	di,@@actions - 4	;2008-11-03
		call	action_long_jump
		jc	@@not_found
		add	di,@@first_action
		mov	ax,dump_csip
		jmp	di
; 0..9, A..F, a..f
@@not_found:
		or	bp,bp
		jnz	@@fill_char
		call	ascii_to_bin
		jc	@@next
		mov	cl,bl
		xor	cl,1
		shl	cl,1
		shl	cl,1
		mov	ah,0f0h
		rol	ah,cl
		and	ah,[es:si]
		shl	al,cl
		or	ah,al
		mov	[es:si],ah
		cmp	[es:si],ah
		je	@@right
@@not_ram:
		push	si
		mov	si,dump_not_in_ram
		call	_error_message
		pop	si
		jmp	@@update

@@first_action:
; Ctrl+F
@@dump_follow:
		push	si
		mov	si,follow_menu_struc
		jmp	@@exec_menu
; Ctrl+B
@@block:
		push	si
		mov	si,block_menu_struc
		jmp	@@exec_menu
; Alt+F10
@@local_menu:
		push	si
		mov	si,dump_menu_struc

@@exec_menu:
		call	_local_menu
		pop	si
		jc	@@jump_to_next

@@call_action:
		call	pushr
		call	ax
		call	popr
		jmp	@@select_next_jump
; Ctrl+D
@@dump_go_to:
		call	dump_go_to

@@select_next_jump:
		jc	@@jump_to_next

@@jump_to_next_r:
		jmp	@@next_r
; Any char
@@fill_char:
		cmp	al,0
		je	@@jump_to_next
		mov	[es:si],al
		cmp	[es:si],al
		jne	@@not_ram
; Right
@@right:
		or	bp,bp
		jnz	@@cm_1
		xor	bl,1
		jnz	@@update		;jmp @@next ???
@@cm_1:
		inc	si
		inc	dl
		cmp	dl,15
		ja	@@next_line
@@update:
		mov	[cs:dump_address],si	;2008-03-23
		call	update_dump
		call	update_commands
		call	update_stack
		jmp	@@next
; Alt+=
@@dump_reference:
		mov	ax,dump_reference
		jmp	@@call_action
; Alt+L
@@dump_cur_line:
		mov	ax,dump_cur_line
		jmp	@@call_action
; Alt+U
@@dump_user_scr:
		mov	ax,dump_user_screen
		jmp	@@call_action
; Alt+E
@@dump_esdi:
		sub	ax,update_dump_window-dump_csip
; Alt+D
@@dump_dssi:
		sub	ax,update_dump_window-dump_csip
; Alt+B
@@dump_esbx:
		sub	ax,update_dump_window-dump_csip
; Alt+S
@@dump_ssbp:
		sub	ax,update_dump_window-dump_csip
; Alt+A
@@dump_dxax:
		sub	ax,update_dump_window-dump_csip
; Alt+F
@@dump_dsdx:
		sub	ax,update_dump_window-dump_csip
; Alt+I
@@dump_essi:
		sub	ax,update_dump_window-dump_csip
; Alt+J
@@dump_dsdi:
		sub	ax,update_dump_window-dump_csip
; Alt+C
@@dump_csip:
		jmp	@@call_action
; Ctrl+S
@@search:
		mov	ax,data_search
		jmp	@@call_action
; Ctrl+L
@@search_next:
		mov	ax,data_search_next
		jmp	@@call_action
; Alt+Z
@@zoom_dump:
		call	zoom_dump

@@jump_to_next:
		jmp	@@next

@@next_line:
		mov	dl,0
@@down_1:
		inc	dh
		cmp	dh,[dump_height]
		jb	@@update
		add	word [dump_offs],16
		mov	dh,[dump_height]
		dec	dh
		jmp	@@update
; Down
@@down:
		add	si,16
		jmp	@@down_1
; Left
@@left:
		or	bp,bp
		jnz	@@cm_2
		xor	bl,1
		jz	@@update		;jmp @@next ???
@@cm_2:
		dec	si
		dec	dl			;dl always >=0 before dec
		jns	@@update
		mov	dl,15
@@up_1:
		dec	dh
		jns	@@update		;dh always >=0 before dec
		mov	dh,0
		sub	word [dump_offs],16
		jmp	@@update
; Up
@@up:
		sub	si,16
		jmp	@@up_1
; Home
@@home:
		mov	al,dl
		cbw
		mov	dl,0
		jmp	@@upd0
; End
@@end:
		mov	al,dl
		cbw
		add	si,15
		mov	dl,15
@@upd0:
		sub	si,ax
		xor	bl,bl
		jmp	@@update
; Ctrl+Right
@@byte_right:
		inc	word [dump_offs]
		inc	si
		jmp	@@update
; Ctrl+Left
@@byte_left:
		dec	word [dump_offs]
		dec	si
		jmp	@@update
; Tab
@@switch:
		xor	bp,1
		jnz	@@jump_to_next
		xor	bx,bx
		jmp	@@jump_to_next
; Page Up
@@page_up:
		mov	al,-16
		jmp	@@up_dn
; Page Down
@@page_down:
		mov	al,16
@@up_dn:
		imul	byte [dump_height]
		add	[dump_offs],ax
		add	si,ax
		jmp	@@update
; ESC, Enter
@@quit:
		call	no_cursor
		and	byte [edit_mode],~ED_DUMP
		call	shrink_dump
		call	draw_screen
		call	display_focus
		pop	es
		ret
; Alt+F5
@@user_scr:
		call	restore_screen
		call	read_key
		call	save_screen
		jmp	@@next

@@follow_data_seg:
		mov	ax,[es:si]
		mov	[dump_seg],ax
		xor	ax,ax
		jmp	@@follow_data

@@follow_far_data:
		mov	ax,[es:si+2]
		mov	[dump_seg],ax

@@follow_near_data:
		mov	ax,[es:si]
@@follow_data:
		mov	[dump_offs],ax
		call	update_dump
		clc
		ret

@@follow_code_seg:
		mov	ax,[es:si]
		mov	[unasm_seg],ax
		xor	ax,ax
		jmp	@@follow_code

@@follow_far_code:
		mov	ax,[es:si+2]
		mov	[unasm_seg],ax

@@follow_near_code:
		mov	ax,[es:si]
@@follow_code:
		mov	[cur_ip],ax
		mov	word [cur_line],0
		call	update_commands
		stc
		ret

@@data_search:
		call	search_bytes
		clc
		ret

@@data_search_next:
		call	search_next
		clc
		ret

@@actions	dw	kbEsc
		dw	@@quit		- @@first_action
		dw	kbEnter
		dw	@@quit		- @@first_action
		dw	kbRight
		dw	@@right		- @@first_action
		dw	kbLeft
		dw	@@left		- @@first_action
		dw	kbUp
		dw	@@up		- @@first_action
		dw	kbDown
		dw	@@down		- @@first_action
		dw	kbPgUp
		dw	@@page_up	- @@first_action
		dw	kbPgDn
		dw	@@page_down	- @@first_action
		dw	kbCtrlLeft
		dw	@@byte_left	- @@first_action
		dw	kbCtrlRight
		dw	@@byte_right	- @@first_action
		dw	kbHome
		dw	@@home		- @@first_action
		dw	kbEnd
		dw	@@end		- @@first_action
		dw	kbTab
		dw	@@switch	- @@first_action
		dw	kbCtrlD
		dw	@@dump_go_to	- @@first_action
		dw	kbAltF10
		dw	@@local_menu	- @@first_action
		dw	kbAltEqu
		dw	@@dump_reference - @@first_action
		dw	kbAltU
		dw	@@dump_user_scr - @@first_action
		dw	kbAltE
		dw	@@dump_esdi	- @@first_action
		dw	kbAltD
		dw	@@dump_dssi	- @@first_action
		dw	kbAltB
		dw	@@dump_esbx	- @@first_action
		dw	kbAltS
		dw	@@dump_ssbp	- @@first_action
		dw	kbAltA
		dw	@@dump_dxax	- @@first_action
		dw	kbAltF
		dw	@@dump_dsdx	- @@first_action
		dw	kbAltI
		dw	@@dump_essi	- @@first_action
		dw	kbAltJ
		dw	@@dump_dsdi	- @@first_action
		dw	kbAltC
		dw	@@dump_csip	- @@first_action
		dw	kbAltL
		dw	@@dump_cur_line	- @@first_action
		dw	kbCtrlF
		dw	@@dump_follow	- @@first_action
		dw	kbCtrlB
		dw	@@block		- @@first_action
		dw	kbCtrlS
		dw	@@search	- @@first_action
		dw	kbCtrlL
		dw	@@search_next	- @@first_action
		dw	kbAltZ
		dw	@@zoom_dump	- @@first_action
		dw	kbAltF5
		dw	@@user_scr	- @@first_action
		dw	0			;end of table

follow_data_seg		equ	@@follow_data_seg
follow_far_data		equ	@@follow_far_data
follow_near_data	equ	@@follow_near_data
follow_code_seg		equ	@@follow_code_seg
follow_far_code		equ	@@follow_far_code
follow_near_code	equ	@@follow_near_code
data_search		equ	@@data_search
data_search_next	equ	@@data_search_next

;=============================================================================
; edit_stack
;-----------------------------------------------------------------------------
edit_stack:
		or	byte [edit_mode],ED_STACK
		call	hide_focus
		xor	dx,dx
		mov	bp,edit_sssp_params
		mov	[stack_mode],bp
		call	calc_ss_addr
		call	update_stack

@@next:
		call	large_cursor
		push	dx
		add	dl,[ds:bp+0]		;X
		neg	dh
		add	dh,STK_BOT_Y
		call	gotoxy
		pop	dx

@@read_key:
		call	read_key
		mov	di,@@actions - 3
		call	action_short_jump
		jc	@@not_found
		add	di,@@first_action
		jmp	di
@@not_found:
		call	ascii_to_bin
		jc	@@next
		call	calc_ss_addr		;DI -> word
		push	ds
		mov	ds,[cpu_regs+REG_SS]
		call	set_half_byte
		pop	ds
		jmp	@@right

@@first_action:

;@@local_menu:
;		push	si
;		mov	si,dump_menu_struc
;
;@@exec_menu:
;		call	_local_menu
;		pop	si
;		jc	@@jump_to_next
;

@@update:
		call	calc_ss_addr
		call	update_dump
		call	update_commands
		call	update_stack
		jmp	@@next
; Right
@@right:
		inc	dl
		cmp	dl,3
		jbe	@@update
		mov	dl,0
; Down
@@down:
		dec	dh
		jns	@@update
		sub	word [ds:bp+1],2
		mov	dh,0
		jmp	@@update
; Left
@@left:
		dec	dl
		jns	@@update
		mov	dl,3
; Up
@@up:
		inc	dh
		cmp	dh,[stack_height]
		jb	@@update
		mov	dh,[stack_height]
		dec	dh
		add	word [ds:bp+1],2
		jmp	@@update
; Page Up
@@page_up:
		mov	al,[stack_height]
		shl	al,1
		cbw
		add	word [ds:bp+1],ax
		jmp	@@update
; Page Down
@@page_down:
		mov	al,[stack_height]
		shl	al,1
		cbw
		sub	word [ds:bp+1],ax
		jmp	@@update
; Tab
@@switch:
		xor	dl,dl
		xor	bp,word ED_SSSP ^ ED_SSBP
		mov	[stack_mode],bp
		xchg	dx,[stack_wnd_xy]
		jmp	@@update		;@@next
; ESC, Enter
@@quit:
		call	no_cursor
		and	byte [edit_mode],~ED_STACK
		mov	word [stack_wnd_xy],0
		call	draw_screen
		call	display_focus
		ret

@@actions	dw	kbEsc
		db	@@quit		- @@first_action
		dw	kbEnter
		db	@@quit		- @@first_action
		dw	kbUp
		db	@@up		- @@first_action
		dw	kbDown
		db	@@down		- @@first_action
		dw	kbLeft
		db	@@left		- @@first_action
		dw	kbRight
		db	@@right		- @@first_action
		dw	kbPgUp
		db	@@page_up	- @@first_action
		dw	kbPgDn
		db	@@page_down	- @@first_action
		dw	kbTab
		db	@@switch	- @@first_action
		dw	0

;=============================================================================
; ---
; In:	DH -- line
; Out:	DI -> word
; Modf:	---
; Call:	---
; Use:	---
;
calc_ss_addr:
		xchg	ax,di
		mov	al,dh
		add	al,dh
		cbw
		add	ax,[ds:bp+1]
		cmp	bp,edit_sssp_params
		je	@@sssp_01
		add	ax,[cpu_regs+REG_BP]
@@sssp_01:
		mov	[ds:bp+3],ax
		xchg	ax,di
		ret

;=============================================================================
; block_menu
;-----------------------------------------------------------------------------
block_menu:
		mov	si,block_menu_struc
		call	_local_menu
		jc	@@exit
		call	ax
@@exit:
		ret
;=============================================================================
; dump_go_to
;-----------------------------------------------------------------------------
dump_go_to:
		mov	ax,16*256+40
		mov	di,dump_addr_string
		call	read_address
		jc	@@quit
		or	ch,ch
		jz	@@no_seg
		mov	[dump_seg],dx
		cmp	ch,2
		je	@@no_offs
@@no_seg:
		mov	[dump_offs],ax
@@no_offs:
		call	update_dump
		clc
@@quit:
		ret
;=============================================================================
; edit_commands
;-----------------------------------------------------------------------------
edit_commands:
		cmp	word [cur_x],0
		je	@@skip
		mov	word [cur_x],0
		call	update_commands
@@skip:
		cmp	byte [focus],1
		jae	@@skip2
		mov	byte [@@focus],1
		inc	byte [focus]
		call	update_commands
@@skip2:
		push	word [read_exit_keys]
		mov	word [read_exit_keys],asm_exit_keys
		push	cs
		pop	es
@@next_line:
		mov	si,[cur_line_ip]
		call	unassemble
		mov	di,buffer+CMD_POS+MAX_ASM_SIZE-2
		mov	cx,MAX_ASM_SIZE-2
		std
		mov	al,' '
	repe	scasb
		cld
		scasw
		mov	al,0
		stosb
		add	cx,3
		mov	si,buffer+CMD_POS
		mov	di,asm_line
	rep	movsb
;		mov	AsmLine+MaxAsmSize-1,al
@@again:
		mov	di,asm_line
		mov	ah,[cur_line]
;		add	ah,CPUy
		mov	al,CMD_POS-3
		mov	cx,MAX_ASM_SIZE*256+ASM_LINE_SIZE-1
		mov	bx,atsAssembler*256+atAssembler
		cmp	byte [cur_line_brk],1
		jne	@@read_line
		mov	bx,atsBrkAssembler*256+atBrkAssembler
@@read_line:
		call	read_line
		cmp	ax,kbEsc
		je	@@quit
		cmp	ax,kbDown
		jne	@@1
		call	cur_down
		jmp	@@next_line
@@1:		cmp	ax,kbUp
		jne	@@2
		call	cur_up
		jmp	@@next_line
@@2:		cmp	ax,kbPgDn
		jne	@@3
		call	page_down
		jmp	@@next_line
@@3:		cmp	ax,kbPgUp
		jne	@@4
		call	page_up
		jmp	@@next_line
@@4:
		mov	si,asm_line
		mov	di,[cur_line_ip]
		push	es
		mov	es,[unasm_seg]
		mov	al,[es:di]
		inc	byte [es:di]
		inc	al
		cmp	al,[es:di]
		jne	@@rom_error
		dec	byte [es:di]
		mov	bx,di
		call	assemble
		pop	es
		jnc	@@ok
		cmp	ax,ER_EMPTY
		je	@@ok_1
		dec	ax
		dec	ax
		cmp	ax,9-2
		ja	@@unk_err
		mov	bx,ax
		shl	bx,1
		mov	si,[asm_errors+bx]
		jmp	@@err_msg
@@unk_err:
		mov	si,asm_error_msg
@@err_msg:
		call	_error_message
		jmp	@@again
@@ok:
		call	update_dump
@@ok_1:
		call	cur_down
		jmp	@@next_line
@@rom_error:
		pop	es
		mov	dx,code_not_in_ram
		call	_error_message
@@quit:
		cmp	byte [@@focus],1
		jne	@@quittt
		mov	byte [@@focus],0
		dec	byte [focus]
@@quittt:
		call	update_commands
		pop	word [read_exit_keys]
		ret

@@focus		db	0


;=============================================================================
%include	"bp.inc"
;=============================================================================


;=============================================================================
; execute_menu
;-----------------------------------------------------------------------------
execute_menu:
		call	hide_focus
		mov	si,menu_structure
		call	exec_menu
		call	display_focus
		call	check_shared_area
		mov	al,[cpu_type]
		sub	al,a86
		mov	[opt_cpu],al
		cmp	byte [quit_flag],1
		jne	@@1
		jmp	quit_act
@@1:
		ret
;=============================================================================
; quit_proc
;-----------------------------------------------------------------------------
quit_proc:
		mov	byte [quit_flag],1
		ret
;=============================================================================
; set_cpu_88
;-----------------------------------------------------------------------------
set_cpu_88:
;		mov	OptCPU,0
		mov	byte [cpu_type],a86
		jmp	update_commands
;=============================================================================
; set_cpu_286
;-----------------------------------------------------------------------------
set_cpu_286:
;		mov	OptCPU,1
		mov	byte [cpu_type],a286
		jmp	update_commands
;=============================================================================
; set_cpu_386
;-----------------------------------------------------------------------------
set_cpu_386:
;		mov	OptCPU,2
		mov	byte [cpu_type],a386
		jmp	update_commands
;=============================================================================
; set_cpu_486
;-----------------------------------------------------------------------------
set_cpu_486:
;		mov	OptCPU,3
		mov	byte [cpu_type],a486
		jmp	update_commands
;=============================================================================
; set_cpu_auto
;-----------------------------------------------------------------------------
set_cpu_auto:
		mov	al,[system_cpu]
		sub	al,1
		adc	al,0
		mov	[opt_cpu],al
		add	al,a86
		mov	[cpu_type],al
		jmp	update_commands
;=============================================================================
; set_swap_off
;-----------------------------------------------------------------------------
set_swap_off:
		mov	byte [swap_mode],0
		ret
;=============================================================================
; set_swap_smart
;-----------------------------------------------------------------------------
set_swap_smart:
		mov	byte [swap_mode],1
		ret
;=============================================================================
; set_swap_on
;-----------------------------------------------------------------------------
set_swap_on:
		mov	byte [swap_mode],2
		ret
;=============================================================================
; cpu_local_menu
;-----------------------------------------------------------------------------
cpu_local_menu:
		mov	si,cpu_local_struc
		call	_local_menu
		jc	@@exit
		jmp	ax
@@exit:
		ret
;=============================================================================
; continue_init
;-----------------------------------------------------------------------------
continue_init:
		cmp	byte [loaded],1
		je	@@check_resident
		mov	si,load_error_msg
		call	_error_message

@@check_resident:
		test	byte [cmd_line_options],CMD_RESIDENT
		jz	start_key_loop
		call	_resident

start_key_loop:

		call	draw_screen
main_key_loop:
		mov	word [keys],main_loop
		call	key_loop
		cmp	byte [resident_mode],0
		jne	main_key_loop

;----- Exit from ACT ----------------------------------------------------------
quit_act:
		call	unload_program
		call	set_my_pid
		call	restore_int_table
		call	restore_screen

		mov	al,[my_port_21]
		out	21h,al
		mov	ax,4c00h
		int	21h
;=============================================================================
; _error_message
;-----------------------------------------------------------------------------
_error_message:
		call	error_message
		jmp	check_shared_area
;=============================================================================
; _msg_box
;-----------------------------------------------------------------------------
_msg_box:
		call	message_box
		jmp	check_shared_area
;=============================================================================
; _local_menu
;-----------------------------------------------------------------------------
_local_menu:
		call	local_menu
		pushf
		call	check_shared_area
		popf
		ret
;=============================================================================
; draw_screen
;-----------------------------------------------------------------------------
draw_screen:
		call	draw_cpu_window
update_screen:
		call	update_stack
		call	update_watch_label
update_regs_and_cmds:
		call	_update_dump
		call	update_registers
		jmp	update_commands

;=============================================================================
; E0F
;=============================================================================

