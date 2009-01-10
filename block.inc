;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; block.inc
; Procedures for copying, filling, reading and writing memory blocks.
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
; copy_mem_block
;-----------------------------------------------------------------------------
copy_mem_block:
		mov	si,copy_data_dialog
		call	init_dialog

@@again_:
		mov	byte [copy_dlg_item],0
@@again:
		call	exec_dialog_
		jz	@@exit

		mov	si,from_string
		call	get_address_ds
		jc	@@again_

		call	adjust_address

		mov	[from_addr],ax
		mov	[from_addr+2],dx

		mov	si,to_string
		call	get_address_ds
		mov	byte [copy_dlg_item],1
		jc	@@again

		call	adjust_address

		mov	[to_addr],ax
		mov	[to_addr+2],dx

		call	get_counter
		mov	byte [copy_dlg_item],2
		jc	@@again

		xchg	ax,cx
		cld
		les	di,[to_addr]
		lds	si,[from_addr]
		mov	ax,ds
		mov	bx,es
		cmp	ax,bx
		ja	@@copy
		jb	@@change_dir
		cmp	si,di
		ja	@@copy
@@change_dir:
		add	si,cx
		add	di,cx
		dec	si
		dec	di
		std
@@copy:
	rep	movsb
		push	cs
		pop	ds

@@exit:
		call	draw_screen
		stc
		ret
;=============================================================================
; fill_mem_block
;-----------------------------------------------------------------------------
fill_mem_block:
		mov	si,fill_data_dialog
		call	init_dialog

@@again_:
		mov	byte [fill_dlg_item],0
@@again:
		call	exec_dialog_
		jz	@@exit

		mov	si,to_string
		call	get_address_ds
		jc	@@again_

		call	adjust_address

		mov	[to_addr],ax
		mov	[to_addr+2],dx

		call	get_counter
		mov	byte [fill_dlg_item],1
		jc	@@again

		mov	[count_value],ax

		mov	si,fill_string
		mov	di,fill_bin
		call	create_bin
		jc	@@error
		sub	di,fill_bin
		jz	@@exit

		xchg	ax,di
		mov	cx,[count_value]
		les	di,[to_addr]
		cld

@@next:
		push	cx
		mov	cx,ax
		mov	si,fill_bin
	rep	movsb
		pop	cx
		loop	@@next

@@exit:
		call	draw_screen
		stc
		ret

@@error:
		mov	si,string_error_msg
		call	_error_message
		mov	byte [fill_dlg_item],2
		jmp	@@again
;=============================================================================
; write_mem_block
;-----------------------------------------------------------------------------
write_mem_block:
		mov	word [block_dlg_title],write_title
		mov	word [block_dlg_label],from_label

		mov	si,read_data_dialog
		call	init_dialog

@@again_:
		mov	byte [block_dlg_item],0
@@again:
		call	exec_dialog_
		jz	rwb_exit

		mov	si,from_string
		mov	byte [block_dlg_item],1
		call	get_address_ds
		jc	@@again

		mov	[from_addr],ax
		mov	[from_addr+2],dx

		inc	byte [block_dlg_item]
		call	get_counter
		jc	@@again

		call	draw_screen

		call	set_24_vector

		mov	dx,filename_string
		mov	ax,3d02h
		int	21h
		jc	@@create

		xchg	ax,bx
		mov	si,question_dialog
		call	init_dialog
		call	exec_dialog
		call	draw_screen
		cmp	al,CM_OK
		jb	rwb_close
		jne	@@write
		cwd
		mov	ax,4202h
		xor	cx,cx
		int	21h
		jmp	@@write

@@create:
		mov	ah,3ch
		xor	cx,cx
		int	21h
		xchg	ax,bx
		jc	..@create_error

@@write:
		mov	cx,[count_value]
		push	ds
		lds	dx,[from_addr]
		mov	ah,40h
		int	21h
		pop	ds
		jc	rwb_error
		xor	cx,cx
		mov	ah,40h
		int	21h
		mov	si,write_error_msg
read_write_block_exit:
		jnc	rwb_close

rwb_error:
		call	_error_message

rwb_close:
		mov	ah,3eh
		int	21h

rwb_restore:
		call	restore_24_vector

rwb_exit:
		call	draw_screen
		stc
		ret

..@create_error:
		mov	si,create_error_msg
		call	_error_message
		jmp	rwb_restore
;=============================================================================
; read_mem_block
;-----------------------------------------------------------------------------
read_mem_block:
		mov	word [block_dlg_title],read_title
		mov	word [block_dlg_label],to_label

		mov	si,read_data_dialog
		call	init_dialog

@@again_:
		mov	byte [block_dlg_item],0
@@again:
		call	exec_dialog_
		jz	rwb_exit

		mov	si,from_string
		call	get_address_ds
		mov	byte [block_dlg_item],1
		jc	@@again

		mov	[from_addr],ax
		mov	[from_addr+2],dx

		call	get_counter
		mov	byte [block_dlg_item],2
		jc	@@again

		call	draw_screen

		call	set_24_vector

		mov	dx,filename_string
		mov	ax,3d00h
		int	21h
		mov	si,open_error_msg
		jc	rwb_error

		xchg	ax,bx
		mov	cx,[count_value]

		push	ds
		lds	dx,[from_addr]
		mov	ah,3fh
		int	21h
		pop	ds
		mov	si,read_error_msg
		jc	rwb_error
		cmp	ax,cx
		jmp	read_write_block_exit
;=============================================================================
; E0F
;=============================================================================

