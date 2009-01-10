;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; resident.inc
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
; resident
;-----------------------------------------------------------------------------
resident:
		call	is_resident
		jne	_resident
		mov	si,already_resident
		jmp	_error_message

_resident:
		call	unload_program
		call	set_my_pid
		call	restore_int_table
		call	restore_screen

		mov	al,[my_port_21]
		out	21h,al
		cli
		call	set_breakpoints
		mov	dx,bp_entry
		call	set_03_vector
		call	set_09_vector
		call	set_16_vector
		mov	byte [resident_mode],1
		mov	word [resident_status],DISABLED
		mov	word [exit_status],DISABLED
		mov	byte [swap_mode],1
		save_act_regs
		sti
		mov	dx,resident_msg
		call	write_message

		xor	cx,cx
		call	calc_prog_size

		mov	ax,3100h
		int	21h
;=============================================================================
; set_16_vector
;-----------------------------------------------------------------------------
set_16_vector:
		mov	si,16h*4
		mov	di,save_int_16
		mov	dx,keyboard_service
		jmp	set_vector
;=============================================================================
; restore_16_vector
;-----------------------------------------------------------------------------
restore_16_vector:
		mov	si,save_int_16
		mov	di,16h*4
		jmp	restore_vector
;=============================================================================
; set_24_vector
;-----------------------------------------------------------------------------
set_24_vector:
		mov	si,24h*4
		mov	di,save_int_24
		mov	dx,critical_handler
		jmp	set_vector
;=============================================================================
; restore_24_vector
;-----------------------------------------------------------------------------
restore_24_vector:
		mov	si,save_int_24
		mov	di,24h*4
		jmp	restore_vector
;=============================================================================
; set_vector
;-----------------------------------------------------------------------------
set_vector:
		push	ds
		push	cs
		pop	es
		xor	ax,ax
		mov	ds,ax
		cld
		movsw
		movsw
		mov	[si-4],dx
		mov	[si-2],cs
		pop	ds
		ret
;=============================================================================
; restore_vector
;-----------------------------------------------------------------------------
restore_vector:
		push	es
		xor	ax,ax
		mov	es,ax
		cld
		movsw
		movsw
		pop	es
		ret
;=============================================================================
; check_vector
;-----------------------------------------------------------------------------
check_vector:
		push	ds
		xor	ax,ax
		mov	ds,ax
		cmp	[si],di
		jne	@@exit
		mov	ax,es
		cmp	[si+2],ax
@@exit:
		pop	ds
		ret
;=============================================================================
; keyboard_service
;-----------------------------------------------------------------------------
keyboard_service:
		cmp	ax,MAGIC_REQUEST_CODE
		jne	@@int_exit
		cmp	bx,DEVELOPER_ID
		jne	@@int_exit
		cmp	cx,APPLICATION_ID
		jne	@@int_exit
		mov	ax,MAGIC_ANSWER_CODE
		mov	bx,DEBUGGER_VERSION
		push	cs
		pop	es
		iret
@@int_exit:
		jmp	far [cs:save_int_16]
;=============================================================================
; critical_handler
;-----------------------------------------------------------------------------
critical_handler:
		mov	al,3
		iret
;=============================================================================
; is_resident
;-----------------------------------------------------------------------------
is_resident:
		mov	ax,MAGIC_REQUEST_CODE
		mov	bx,DEVELOPER_ID
		mov	cx,APPLICATION_ID
		int	16h
		mov	dx,not_installed_msg
		cmp	ax,MAGIC_ANSWER_CODE
		jne	@@exit
		mov	dx,version_msg
		cmp	bx,DEBUGGER_VERSION
@@exit:
		ret
;=============================================================================
; E0F
;=============================================================================


