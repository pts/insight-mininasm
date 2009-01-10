;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; install.inc
; Initialization procedures and messages.
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
; 2007-02-08	0leg	Added code for UMB presence checking and for determining
;			segment of 1st MCB.
; 2007-12-13	0leg	Fixed incorrect displaying of CPU type on P-4.
;=============================================================================


;=============================================================================
; install
;-----------------------------------------------------------------------------
install:
		mov	dx,copyright
		call	write_message

		xor	ax,ax
		mov	cx,udata_end-udata_start
		mov	di,udata_start
		cld
	rep	stosb

		push	es		;Get segment of 1st MCB
		mov	ah,52h
		int	21h
		mov	ax,[es:bx-2]
		mov	[first_mcb],ax
		pop	es

		mov	ax,5802h	;Get UMB link state
		int	21h
		cbw
		push	ax
		mov	ax,5803h
		mov	bx,1		;Link UMB for allocations
		int	21h
		jc	@@no_umbs
		mov	byte [umb_available],1
@@no_umbs:
		pop	bx
		mov	ax,5803h
		int	21h		;Restore UMB link state

		call	get_video_type
		mov	[video_type],al	;1 == VGA, 0 == EGA

		call	is_hma_exist
		jnc	@@command_line
		or	word [search_options],SRCH_EXCL_HMA
@@command_line:

		mov	word [my_cmd_line],0d00h
		mov	byte [cmd_line_string],0
		mov	byte [filename],0

		call	get_cmd_line
		call	full_prog_name

		mov	dx,help_msg
		test	byte [cmd_line_options],CMD_HELP
		jnz	@@display_error

		mov	dx,error_help_msg
		test	byte [cmd_line_options],CMD_ERROR
		jnz	display_error

		test	byte [cmd_line_options],CMD_EGA
		jz	@@not_ega
		mov	byte [video_type],0		;1 == VGA, 0 == EGA
@@not_ega:

		test	byte [cmd_line_options],CMD_UNLOAD
		jnz	..@unload

@@check_memory:
		xor	cx,cx
		call	calc_prog_size
		mov	sp,ax
		mov	bx,dx
		mov	ah,4ah
		int	21h

		mov	dx,fatal_memory_msg
		jc	display_error
		jmp	init

@@display_error:
		mov	byte [beep_char],'$'
display_error:
		call	write_message
		mov	dx,eol
		call	write_message

@@error_quit:
		mov	ax,4c01h
		int	21h

..@unload:
		call	is_resident
		jne	display_error

		mov	dx,unload_error_msg

		mov	si,3*4
		mov	di,bp_entry
		call	check_vector
		jne     display_error

		mov	si,9*4
		mov	di,int_09_handler
		call	check_vector
		jne     display_error

		mov	si,16h*4
		mov	di,keyboard_service
		call	check_vector
		jne     display_error

		push	es
		pop	ds
		call	restore_03_vector
		call	restore_09_vector
		call	restore_16_vector

		mov	es,[2ch]
		mov	ah,49h
		int	21h
		jc	@@skip_free
		push	ds
		pop	es
		mov	ah,49h
		int	21h

@@skip_free:
		push	cs
		pop	ds
		mov	dx,memory_free_msg
		jc	display_error

		mov	dx,unloaded_msg
		call	write_message
		jmp	@@error_quit
;-----------------------------------------------------------------------------
; init
;-----------------------------------------------------------------------------
init:
		call	save_int_table

		test	byte [cmd_line_options],CMD_RESIDENT
		jz	@@check_prog_name
		call	is_resident
		jne	@@create_program
		mov	dx,alredy_tsr_msg
		jmp	display_error

@@check_prog_name:
		cmp	byte [filename],0
		je	@@create_program
		call	set_command_line
		call	load_program
		jnc	@@loaded
		mov	byte [loaded],0

@@create_program:
		call	create_program

@@loaded:
		call	set_user_pid
		call	set_term_handler
		call	save_font
		call	save_screen
		in	al,21h
		and	al,11111100b
		mov	[my_port_21],al
		mov	[user_port_21],al

		call	get_cpu_type
		cmp	al,7fh
		jne	@@set_cpu_type

	cpu	586
		xor	eax,eax
		cpuid
		mov	[cpu_advanced_name+0],ebx
		mov	[cpu_advanced_name+4],edx
		mov	[cpu_advanced_name+8],ecx
		xor	eax,eax
		inc	ax
		cpuid
	cpu	8086

		and	ah,0fh
		cmp	ah,6			;ah == 0fh on Pentium-4
		jbe	@@cpu_type_ok
		mov	ah,6
@@cpu_type_ok:
		add	[cpu_advanced_id],ah
		mov	ax,4
		mov	word [cpu_names+8],cpu_advanced

@@set_cpu_type:
		mov	[system_cpu],al
		cmp	al,3
		jb	@@skip_enable
		mov	word [reg_mode_stat_1],0
		mov	word [reg_mode_stat_2],0

@@skip_enable:
		mov	bx,ax
		or	ax,ax
		jz	@@1
		dec	ax
@@1:
		mov	[opt_cpu],al
		add	al,a86
		mov	[cpu_type],al
		shl	bx,1
		mov	ax,[cpu_names+bx]
		mov	[cpu_name],ax

		mov	ax,[cpu_regs+REG_CS]
		mov	[unasm_seg],ax
		mov	ax,[cpu_regs+REG_IP]
		mov	[cur_ip],ax
		mov	ax,[cpu_regs+REG_DS]
		mov	[dump_seg],ax

		xor	ax,ax
		mov	[main_addr_string],al
		mov	[dump_addr_string],al
		mov	[asm_line],al
		mov	[cur_line],ax
		mov	[dump_offs],ax

		call	set_prefixes
		jmp	continue_init
;=============================================================================
; get_video_type
;-----------------------------------------------------------------------------
get_video_type:
		mov	ah,12h
		mov	bl,10h
		int	10h
		cmp	bl,10h
		je	@@old_video
		mov	ax,1a00h
		int	10h
		cmp	al,1ah
		jne	@@old_video
		cmp	bl,7
		jbe	@@old_video
		mov	al,1
		ret

@@old_video:
		xor	ax,ax
		ret
;=============================================================================
; is_hma_exist
;-----------------------------------------------------------------------------
is_hma_exist:
		mov	ax,4300h
		int	2fh
		cmp	al,80h
		jne	@@error_exit

		push	es
		mov	ax,4310h
		int	2fh

		push	cs
		mov	ax,@@ret_point
		push	ax
		mov	ah,0
		push	es
		push	bx
		retf

@@ret_point:
		pop	es
		or	ax,ax
		jz	@@error_exit
		or	dx,dx
		jz	@@error_exit
		clc
		ret

@@error_exit:
		stc
		ret
;=============================================================================
; set_prefixes
;-----------------------------------------------------------------------------
set_prefixes:
		cmp	byte [system_cpu],3
		jb	@@exit
		mov	cx,@@prefix_table_end - @@prefix_table
		mov	si,@@prefix_table
		xor	bx,bx
@@next:
		mov	bl,[si]
		lea	di,[first_prefix+bx]
		mov	byte [di],066h
		inc	si
		loop	@@next
@@exit:
		ret
;-----------------------------------------------------------------------------
@@prefix_table:
		db	prefix_01 - first_prefix
		db	prefix_02 - first_prefix
		db	prefix_03 - first_prefix
		db	prefix_04 - first_prefix
		db	prefix_05 - first_prefix
		db	prefix_06 - first_prefix
		db	prefix_07 - first_prefix
		db	prefix_08 - first_prefix
		db	prefix_09 - first_prefix
		db	prefix_10 - first_prefix
		db	prefix_11 - first_prefix
		db	prefix_12 - first_prefix
		db	prefix_13 - first_prefix
		db	prefix_14 - first_prefix
		db	prefix_15 - first_prefix
		db	prefix_16 - first_prefix
		db	prefix_17 - first_prefix
		db	prefix_18 - first_prefix
		db	prefix_19 - first_prefix
@@prefix_table_end:
;=============================================================================

available_options db	"ERUH?",0

options_bits	db	CMD_EGA, CMD_RESIDENT, CMD_UNLOAD, CMD_HELP, CMD_HELP

copyright	db	SERIAL_NUMBER, ", real-mode debugger for MS/PC/FreeDOS. Build ", BUILD, ".", 13, 10
		db	"Copyright (C) 1993, 96, 97, 2002, Victor Gamayunov, Sergey Pimenov", 13, 10
		db	"Modifications by Oleg O. Chukaev, 2006 - 2009", 13, 10, 13, 10
		db	"Insight comes with ABSOLUTELY NO WARRANTY. This is free software covered by", 13, 10
		db	"GNU GENERAL PUBLIC LICENSE 2, and you are welcome to redistribute it under", 13, 10
		db	"certain conditions. See http://www.gnu.org/ or COPYING file for details.", 13, 10, '$'

error_help_msg	db	"Error in command line parameters.", 13, 10
help_msg	db	"  Usage: INSIGHT [options] [filename [parameters]]", 13, 10
		db	"  Options:", 13, 10
		db	"	/h, /?		- display this help screen", 13, 10
		db	"	/e		- EGA mode (do not save font and VGA registers)", 13, 10
		db	"	/r		- terminate and stay resident in memory", 13, 10
		db	"	/u		- unload resident debugger$"

fatal_memory_msg db	"Memory allocation error$"

memory_free_msg	db	"Can't release debugger memory$"
not_installed_msg db	"Debugger not found in memory$"
version_msg	db	"Incorrect debugger version$"
unload_error_msg db	"Can't unload debugger$"
unloaded_msg	db	"Debugger unloaded successfully.", 13, 10, '$'
alredy_tsr_msg	db	"Debugger already installed resident.", 13, 10
		db	"  Press Ctrl+Esc to activate$"
eol		db	".", 13, 10
beep_char	db	7, '$'

;=============================================================================
; E0F
;=============================================================================


