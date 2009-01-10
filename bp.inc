;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; bp.inc
; Breakpoints: set, clear, check, etc.
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
; 2008-03-26	0leg	Added comments to all procedures, some procedures
;			optimized.
; 2008-04-20	0leg	toggle_breakpoint -> toggle_break,
;			test_breakpoint -> test_break,
;			check_bp -> check_break.
;=============================================================================


;=============================================================================
; toggle_break
;-----------------------------------------------------------------------------
; Sets/clears a breakpoint under cursor.
; In:	---
; Out:	---
; Modf:	AX, BX, CX, SI
; Call:	check_break, test_break, _error_message, update_commands
; Use:	cur_line_ip, bp1, unasm_seg, bp_not_in_ram, many_bp_msg
;
toggle_break:
		mov	si,[cur_line_ip]
		call	check_break			;Is bp exist?
		jc	@@set				;No, set bp
		mov	byte [bx+BP_ACTIVE],BP_UNUSED	;Clear bp...
		jmp	@@ok				;and exit
@@set:
		mov	bx,bp1
		mov	cx,MAX_BREAKPOINTS
@@next_bp:
		cmp	byte [bx+BP_ACTIVE],BP_UNUSED
		jne	@@skip
		mov	ax,[unasm_seg]
		mov	[bx+BP_SEG],ax			;Set segment
		mov	ax,[cur_line_ip]
		mov	[bx+BP_OFFS],ax			;Set offset
		mov	byte [bx+BP_SET],0		;Bp is not set
		call	test_break
		mov	si,bp_not_in_ram
		jc	@@err_msg			;Can't set bp...
		mov	byte [bx+BP_ACTIVE],BP_ENABLED	;Enable bp
		jmp	@@ok
@@skip:
		add	bx,SIZEOF_BREAKPOINT
		loop	@@next_bp

		mov	si,many_bp_msg
@@err_msg:
		call	_error_message
@@ok:
		jmp	update_commands
;=============================================================================
; toggle_activity
;-----------------------------------------------------------------------------
; Enables or disables a breakpoint under cursor.
; In:	---
; Out:	---
; Modf:	~BX, SI
; Call:	check_break, beep, update_commands
; Use:	cur_line_bp
;
toggle_activity:
		mov	si,[cur_line_ip]
		call	check_break
		jnc	@@toggle
		jmp	beep
@@toggle:
		xor	byte [bx+BP_ACTIVE],BP_ENABLED ^ BP_DISABLED
		jmp	update_commands
;=============================================================================
; set_all_breaks
;-----------------------------------------------------------------------------
; Assigns specified value to BP_ACTIVE field of all used breakpoints.
; In:	AL -- value (BP_UNUSED, BP_ENABLED, BP_DISABLED)
; Out:	---
; Modf:	BX, CX
; Call:	update_commands
; Use:	bp1
;
set_all_breaks:
		mov	bx,bp1
		mov	cx,MAX_BREAKPOINTS

@@next_bp:
		cmp	byte [bx+BP_ACTIVE],BP_UNUSED
		je	@@skip
		mov	byte [bx+BP_ACTIVE],al
@@skip:
		add	bx,SIZEOF_BREAKPOINT
		loop	@@next_bp

		jmp	update_commands
;=============================================================================
; clear_all_breaks
;-----------------------------------------------------------------------------
; Clears all breakpoints.
; In:	---
; Out:	---
; Modf:	AX
; Call:	set_all_breaks
; Use:	---
;
clear_all_breaks:
		mov	al,BP_UNUSED
		jmp	set_all_breaks
;=============================================================================
; enable_all_breaks
;-----------------------------------------------------------------------------
; Enables all breakpoints.
; In:	---
; Out:	---
; Modf:	AX
; Call:	set_all_breaks
; Use:	---
;
enable_all_breaks:
		mov	al,BP_ENABLED
		jmp	set_all_breaks
;=============================================================================
; disable_all_breaks
;-----------------------------------------------------------------------------
; Disables all breakpoints.
; In:	---
; Out:	---
; Modf:	AX
; Call:	set_all_breaks
; Use:	---
;
disable_all_breaks:
		mov	al,BP_DISABLED
		jmp	set_all_breaks
;=============================================================================
; test_break
;-----------------------------------------------------------------------------
; Checks if a breakpoint can be set.
; In:	DS:BX -> breakpoint structure
; Out:	CF -- status:
;		CF == 0 -- BP can be set
;		CF == 1 -- BP can't be set
; Modf:	---
; Call:	---
; Use:	---
;
test_break:
		push	ax
		push	si
		push	ds
		cli
		mov	si,[bx+BP_OFFS]
		mov	ds,[bx+BP_SEG]
		mov	al,[si]
		mov	byte [si],0cch
		cmp	byte [si],0cch
		je	@@ok
		stc
@@ok:
		mov	[si],al
		sti
		pop	ds
		pop	si
		pop	ax
		ret
;=============================================================================
; check_break
;-----------------------------------------------------------------------------
; Checks if a breakpoint at specified offset is set and enabled.
; In:	SI -- offset
; Out:	CF, ZF -- status of a breakpoint:
;		CF == 1 -- breakpoint not exist
;		CF == 0, ZF == 1 -- breakpoint exist and enabled
;		CF == 0, ZF == 0 -- breakpoint exist and disabled
;	DS:BX -> breakpoint structure
; Modf:	!BX
; Call:	---
; Use:	bp1, unasm_seg
;
check_break:
		push	ax
		push	cx
		mov	bx,bp1			;Array of breakpoint structures
		mov	cx,MAX_BREAKPOINTS
@@next_bp:
		cmp	byte [bx+BP_ACTIVE],BP_UNUSED
		je	@@skip			;Unused...
		cmp	si,[bx+BP_OFFS]
		jne	@@skip			;Offset doesn't match...
		mov	ax,[unasm_seg]
		cmp	ax,[bx+BP_SEG]
		jne	@@skip			;Segment doesn't match...
		cmp	byte [bx+BP_ACTIVE],BP_ENABLED
		jmp	@@exit			;All Ok, flags are set, exit
@@skip:
		add	bx,SIZEOF_BREAKPOINT
		loop	@@next_bp
		stc				;Breakpoint not found
@@exit:
		pop	cx
		pop	ax
		ret
;=============================================================================

