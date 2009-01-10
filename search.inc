;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; search.inc
; Procedures for searching.
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
; search_bytes
;-----------------------------------------------------------------------------
search_bytes:
		push	ds
		pop	es

		mov	ax,[search_options]
		mov	[search_bits],ax
		mov	si,search_dialog
		call	init_dialog

@@again_:
		mov	byte [search_dlg_item],0

@@again:
		call	exec_dialog_
		or	ax,ax
		jz	@@error_exit

		mov	si,search_string
		mov	di,search_bin
		call	create_bin
		jc	@@string_error

		sub	di,search_bin
		jz	@@error_exit

@@check_address:
		push	di
		mov	si,search_addr_str
		call	get_address
		pop	bx
		mov	byte [search_dlg_item],2
		jc	@@again

		mov	[search_len],bx

		mov	bx,[dump_seg]
		test	word [search_options],SRCH_RES_CODE
		jz	@@def_seg
		mov	bx,[unasm_seg]

@@def_seg:
		cmp	ch,0
		je	@@search
		cmp	ch,1
		je	@@set_seg
		xor	ax,ax

@@set_seg:
		mov	bx,dx

@@search:
		mov	dx,bx
		call	linear_address
		test	word [search_options],SRCH_EXCL_HMA
		jz	@@set_search_pos
		and	dl,0fh

@@set_search_pos:
		mov	[search_offs],ax
		mov	[search_seg],dx

		mov	ax,[search_bits]
		mov	[search_options],ax

		mov	ax,search_next
		push	ax

@@error_exit:
		call	draw_screen

@@exit:
		ret

@@string_error:
		mov	si,string_error_msg
		call	_error_message
		jmp	@@again_
;=============================================================================
; search_next
;-----------------------------------------------------------------------------
search_next:
		mov	cx,[search_len]
		jcxz	@@not_found_msg

		mov	byte [exec_msg_type],1
		mov	si,search_msg
		call	message_box

		xor	ax,ax
		cwd
		test	word [search_options],SRCH_BACKWARD
		jnz	@@start_search
		dec	dx
		sub	ax,cx
		test	word [search_options],SRCH_EXCL_HMA
		jz	@@start_search
		mov	dx,0f000h

@@start_search:
		call	linear_address
		xchg	[search_offs],ax
		xchg	[search_seg],dx
		xchg	ax,di
		cld

@@search_byte:
		mov	si,search_bin
		lodsb

		call	@@compare
		jne	@@next
		call	@@compare_bytes
		jne	@@next

		push	dx
		mov	ax,di
		call	phisical_address
		mov	bx,ax
		mov	si,dx
		mov	ax,search_bin
		mov	dx,cs
		call	adjust_address
		cmp	dx,si
		pop	dx
		jne	@@found
		cmp	ax,bx
		jne	@@found

@@next:
		call	@@next_byte
		jnc	@@search_byte

@@not_found:
		call	@@close_window
		mov	word [search_len],0

@@not_found_msg:
		mov	si,not_found_msg
		jmp	_msg_box

@@close_window:
		mov	byte [exec_msg_type],2
		call	_msg_box
		mov	byte [exec_msg_type],0
		ret

@@found:
		call	@@close_window
		push	di
		push	dx

		call	@@next_byte
		jnc	@@set_new_pos
		mov	word [search_len],0

@@set_new_pos:
		mov	[search_offs],di
		mov	[search_seg],dx

		pop	dx
		pop	ax

		call	phisical_address

		test	word [search_options],SRCH_RES_CODE
		jz	@@set_dump
		mov	word [cur_line],0
		mov	[cur_ip],ax
		mov	[unasm_seg],dx
		jmp	update_commands

@@set_dump:
		mov	[dump_offs],ax
		mov	[dump_seg],dx
		jmp	update_dump

@@compare:
		call	pushr
		call	@@upcase
		xchg	ax,bx
		xchg	ax,di
		call	phisical_address
		xchg	ax,di
		mov	es,dx
		mov	al,[es:di]
		call	@@upcase
		cmp	al,bl
		call	popr
		ret

@@compare_bytes:
		call	pushr
		dec	cx
		jz	@@exit_1

@@next_char:
		lodsb
		call	@@next_byte
		call	@@compare
		loope	@@next_char
@@exit_1:
		call	popr
		ret

@@upcase:
		test	word [search_options],SRCH_IGN_CASE
		jz	@@exit
		jmp	upcase

@@next_byte:
		test	word [search_options],SRCH_BACKWARD
		jnz	@@backward
		add	di,1
		adc	dx,0
		mov	bx,[search_offs]
		mov	bp,[search_seg]
		sub	bx,di
		sbb	bp,dx

@@exit:
		ret

@@backward:
		sub	di,1
		sbb	dx,0
		ret
;=============================================================================
; linear_address
;-----------------------------------------------------------------------------
linear_address:
		push	bx
		push	cx
		mov	cl,4
		mov	bx,dx
		shl	bx,cl
		rol	dx,cl
		and	dx,000fh
		add	ax,bx
		adc	dl,0
		pop	cx
		pop	bx
		ret
;=============================================================================
; phisical_address
;-----------------------------------------------------------------------------
phisical_address:
		cmp	dx,10h
		jb	@@skip_hma
		sub	ax,0fff0h
		mov	dx,0ffffh
		ret
@@skip_hma:
		push	bx
		push	cx
		mov	cl,4
		mov	bx,ax
		shr	bx,cl
		ror	dx,cl
		add	dx,bx
		and	ax,000fh
		pop	cx
		pop	bx
		ret
;=============================================================================
; create_bin
;-----------------------------------------------------------------------------
create_bin:
		mov	bp,di
		cld

@@next_2:
		lodsb

		cmp	al,' '
		je	@@next_2

		cmp	al,';'
		je	@@next_2

		or	al,al
		jnz	@@convert

@@eol:
		clc

@@exit:
		ret

@@convert:
		call	convert_number
		jnc	@@next_4

		call	convert_string
		jnc	@@next_4

		call	convert_command
		jc	@@exit

@@next_4:
		lodsb
		cmp	al,';'
		je	@@next_2
		or	al,al
		je	@@eol
		cmp	al,' '
		je	@@next_2
		stc
		ret
;=============================================================================
; convert_string
;-----------------------------------------------------------------------------
convert_string:
		cmp	al,'"'
		je	@@convert
		cmp	al,"'"
		jne	@@error_exit

@@convert:
		mov	dl,al

@@next:
		lodsb
		or	al,al
		jz	@@error_exit
		cmp	al,dl
		je	@@exit
		stosb
		jmp	@@next

@@error_exit:
		stc

@@exit:
		ret
;=============================================================================
; convert_number
;-----------------------------------------------------------------------------
convert_number:
		mov	bx,si
		mov	cx,di
		dec	bx
;		call	@@is_hex_number
call	ascii_to_bin
		jc	@@error_exit
		xchg	ax,dx
		lodsb
;		call	@@is_hex_number
call	ascii_to_bin
jc	@@error_exit
;		jnc	@@add
;		xchg	ax,dx
;		jmp	@@save

@@add:
		shl	dx,1
		shl	dx,1
		shl	dx,1
		shl	dx,1
		add	ax,dx

@@save:
		stosb
		mov	al,[si]
		cmp	al,' '
		je	@@ok
		cmp	al,';'
		je	@@ok
		cmp	al,0
		jne	@@error_exit

@@ok:
		clc
		ret

;@@is_hex_number:
;		call	upcase
;		sub	al,'0'
;		jb	@@error_exit
;		cmp	al,9
;		jbe	@@ok
;		sub	al,'A'-'0'
;		jb	@@error_exit
;		cmp	al,5
;		ja	@@error_exit
;		add	al,10
;		clc
;		ret

@@error_exit:
		mov	si,bx
		mov	di,cx
		lodsb
		stc
		ret
;=============================================================================
; convert_command
;-----------------------------------------------------------------------------
convert_command:
		dec	si
		mov	bx,si

@@next:
		lodsb
		cmp	al,';'
		je	@@eoa
		or	al,al
		jne	@@next

@@eoa:
		dec	si
		push	word [si]
		push	si

		mov	byte [si],0
		mov	si,bx
		mov	bx,di
		sub	bx,bp
		add	bx,[cpu_regs+REG_IP]

		call	assemble

		pop	si
		pop	word [si]
		ret
;=============================================================================
; E0F
;=============================================================================


