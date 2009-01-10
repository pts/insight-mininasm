;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; tools.inc
; Various tools.
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
; 2007-02-06	0leg	list_memory_blocks added
;=============================================================================


;=============================================================================
; list_memory_blocks
;-----------------------------------------------------------------------------
; Lists DOS memory blocks. User can dump or disassemble selected block.
; In:	---
; Out:	---
; Modf:	---
; Call:	init_string_list, fill_word_str, add_string, init_dialog,
;	exec_dialog, get_address_, get_string_addr, push_unasm_pos,
;	draw_screen, pushr, popr
; Use:	first_mcb, string, msg_free, msg_noname, msg_dos, msg_prog,
;	msg_env, msg_data, umb_available, list_mem_blocks_dialog,
;	list_choice, dump_seg, dump_offs, unasm_seg, cur_line, cur_ip
;
list_memory_blocks:
		call	pushr
		mov	ax,41
		call	init_string_list

		mov	ds,[first_mcb]
@@next:
		mov	al,' '
		mov	cx,40
		mov	di,string
	rep	stosb
		mov	byte [es:di],0
		push	ds

		mov	ax,ds
		inc	ax
		mov	di,string
		call	fill_word_str	;Segment

		mov	si,msg_free	;Type: free
		mov	bp,msg_noname	;Name: <noname>
		mov	bx,cs		;Seg: our
		cmp	word [1],0	;Free block?
		je	@@set_type	;Yes

		mov	si,msg_dos	;Type: dos
		mov	bp,8		;Name: get name from MCB
		mov	bx,ds		;Seg: MCB
		cmp	word [1],8	;DOS code/data?
		je	@@set_type	;Yes

		mov	si,msg_prog	;Type: program
		mov	ax,ds
		inc	ax		;Seg of block
		cmp	[1],ax		;Seg of owner
		je	@@set_type	;It is really seg of PSP

		mov	bx,[1]		;PSP seg of owner
		mov	ds,bx
		dec	bx		;Seg of owner's MCB
		mov	si,msg_env	;Type: env. seg
		cmp	[2ch],ax	;Env. segment?
		je	@@set_type	;Yes
		mov	si,msg_data	;Type: data segment
@@set_type:
		push	cs
		pop	ds
		mov	di,string+24
		mov	cx,4
	rep	movsb			;Type of block

		mov	ds,bx
		mov	si,bp
		mov	di,string+32
		mov	cx,8
	rep	movsb			;Name of block

		pop	ds
		mov	ax,[1]		;Owner's seg
		mov	di,string+8
		call	fill_word_str

		mov	ax,[3]		;Size of block
		mov	di,string+16
		call	fill_word_str

		push	ds
		push	es
		push	cs
		pop	ds
		mov	si,string
		call	add_string	;Add string to list
		pop	es
		pop	ds

		cmp	byte [0],'Z'	;Last block?
		jne	@@m_block	;No
		cmp	byte [cs:umb_available],0
		je	@@quit		;UMB not available, quit
		mov	ax,ds
		cmp	ax,0a000h	;Z-block in UMB?
		jae	@@quit		;Yes, quit
@@m_block:
		mov	ax,ds
		add	ax,[3]
		inc	ax
		mov	ds,ax
		jmp	@@next		;Loop
@@quit:
		push	cs
		pop	ds

		mov	si,list_mem_blocks_dialog
		call	init_dialog
		call	exec_dialog
		jz	@@exit

		push	ax

		mov	ax,[list_choice]
		call	get_string_addr
		mov	si,ax
		mov	byte [si+4],0
		call	get_address_

		pop	bx
		cmp	bx,1		;Dump
		jne	@@go_to_code
		mov	word [dump_offs],0
		mov	[dump_seg],ax
		jmp	@@exit
@@go_to_code:
		call	push_unasm_pos
		mov	[unasm_seg],ax
		mov	word [cur_ip],0
		mov	word [cur_line],0
@@exit:
		call	draw_screen
		call	popr
		ret
;=============================================================================
; E0F
;=============================================================================


