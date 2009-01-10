;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; menus.inc
; Data: menu definitions.
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
; 2007-11-17	0leg	Menu structures optimized
;=============================================================================


;=============================================================================
menu_structure:
		dw	6, hmenu, hsub
hmenu		db	"~F~ile",0
		db	"~E~dit",0
		db	"~R~un",0
		db	"~B~reakpoints",0
		db	"~O~ptions",0
;		db	"~T~ools",0
		db	"~W~indow",0
hsub		dw	SUB_MENU, file_submenu
		dw	SUB_MENU, edit_submenu
		dw	SUB_MENU, run_submenu
		dw	SUB_MENU, bp_submenu
		dw	SUB_MENU, options_submenu
;		dw	SUB_MENU, tools_submenu
		dw	SUB_MENU, window_submenu
;=============================================================================
file_submenu:
		db	1, 1, 1+17, 9	; Coords
		dw	5		; Number
		db	0, 15		; Selected, Width
		dw	file_sub
		dw	file_str
file_sub	dw	0, load_file
		dw	0, new_program
		dw	0, about
resident_status	dw	0, resident
exit_status	dw	0, quit_proc
file_str	db	"~L~oad...`F3",0
		db	"~N~ew",0
		db	SEPARATOR
		db	"~A~bout...",0
		db	SEPARATOR
		db	"~R~esident",0
		db	"E~x~it`Alt+X",0
;=============================================================================
edit_submenu:
		db	7, 1, 7+21, 7	; Coords
		dw	5		; Number
		db	0, 19		; Selected, Width
		dw	edit_sub
		dw	edit_str
edit_sub	dw	0, edit_commands
		dw	0, edit_dump
		dw	0, edit_registers
		dw	0, edit_flags
		dw	0, edit_stack
edit_str	db	"~A~ssembler`Shift+A",0
		db	"~D~ump`Shift+D",0
		db	"~R~egisters`Shift+R",0
		db	"~F~lags`Shift+F",0
		db	"~S~tack`Shift+S",0
;=============================================================================
run_submenu:
		db	13, 1, 13+25, 11; Coords
		dw	8		; Number
		db	0, 23		; Selected, Width
		dw	run_sub
		dw	run_str
run_sub		dw	0, run_program
		dw	0, go_to_cursor
		dw	0, skip_instruction
		dw	0, single_step
		dw	0, proc_trace
		dw	0, animate
		dw	0, read_cmd_line
		dw	0, reset_program
run_str		db	"~R~un`Ctrl+F9", 0
		db	"~G~o to cursor`F4", 0
		db	"S~k~ip`F6", 0
		db	"~T~race into`F7", 0
		db	"~S~tep over`F8", 0
		db	"A~n~imate...", 0
		db	SEPARATOR
		db	"~A~rguments...", 0
		db	"~P~rogram reset`Ctrl+F2", 0
;=============================================================================
bp_submenu:
		db	18, 1, 18+20, 9	; Coords
		dw	6		; Number
		db	0, 18		; Selected, Width
		dw	bp_sub
		dw	bp_str
bp_sub		dw	0, toggle_break
		dw	0, toggle_activity
		dw	DISABLED, test_proc
		dw	0, clear_all_breaks
		dw	0, enable_all_breaks
		dw	0, disable_all_breaks
bp_str		db	"~S~et/Clear`F2",0
		db	"~A~ctivity`Alt+F2",0
		db	"~E~dit...          ",0
		db	SEPARATOR
		db	"~D~elete all",0
		db	"E~n~able all",0
		db	"D~i~sable all",0
;=============================================================================
options_submenu:
		db	31, 1, 31+26, 5	; Coords
		dw	3		; Number
		db	0, 24		; Selected, Width
		dw	options_sub
		dw	options_str
options_sub	dw	SUB_MENU, options_cpu
		dw	SUB_MENU, options_swap
;		dw	DISABLED, test_proc
;		dw	DISABLED, test_proc
		dw	DISABLED, test_proc
options_str	db	"~C~PU for disassembler`",0
		db	"~S~creen swapping`",0
;		db	"~E~mulation...",0
;		db	"C~o~lors...",0
		db	"Co~n~figuration...",0
;=============================================================================
options_cpu:
		db	45, 3, 45+14, 10	; Coords
		dw	5			; Number
opt_cpu		db	0, 12			; Selected, Width
		dw	options_cpu_sub
		dw	options_cpu_str
options_cpu_sub	dw	0, set_cpu_88
		dw	0, set_cpu_286
		dw	0, set_cpu_386
		dw	0, set_cpu_486
		dw	0, set_cpu_auto
options_cpu_str	db	"8~0~86/8088",0
		db	"80~2~86",0
		db	"80~3~86",0
		db	"80~4~86",0
		db	SEPARATOR
		db	"~A~uto detect",0
;=============================================================================
options_swap:
		db	47, 4, 47+11, 8	; Coords
		dw	3		; Number
swap_mode	db	1, 9		; Selected, Width
		dw	options_swap_sub
		dw	options_swap_str
options_swap_sub dw	0, set_swap_off
		dw	0, set_swap_smart
		dw	0, set_swap_on
options_swap_str db	"~N~one",0
		db	"~S~mart",0
		db	"~A~lways",0
;=============================================================================
;ToolsSubMenu	;~label	byte
;		db	40, 1, 40+26, 8	; Coords
;		dw	6		; Number
;		db	0, 24		; Selected, Width
;		dw	ToolsSub
;		dw	ToolsStr
;ToolsSub	dw	DISABLED, test_proc
;		dw	DISABLED, test_proc
;		dw	DISABLED, test_proc
;		dw	DISABLED, test_proc
;		dw	DISABLED, test_proc
;		dw	DISABLED, test_proc
;ToolsStr	db	"~M~emory blocks list...",0
;		db	"~T~SR programs...",0
;		db	"~D~evice drivers...",0
;		db	"~H~ooked vectors...",0
;		db	"~I~/O",0
;		db	"~C~alculator...",0
;=============================================================================
window_submenu:
		db	40, 1, 40+25, 4	; Coords
		dw	2		; Number
		db	0, 23		; Selected, Width
		dw	window_sub
		dw	window_str
window_sub	dw	0, draw_screen
		dw	0, exec_user_screen
window_str	db	"~R~efresh display`F5",0
		db	"~U~ser screen`Alt+F5",0

test_proc:	ret
;=============================================================================
cpu_local_struc:
		db	35, 1, 35+25, 2+14
		dw	11
		db	0
		db	23
		dw	cpu_loc_sub
		dw	cpu_loc_str
cpu_loc_str	db	"~A~ssembler`Shift+A",0
		db	"~N~ew CS:IP`Ctrl+N",0
regs_menu_1	db	"32-~b~it regs`Ctrl+R",0
		db	"Data ~w~atch`Ctrl+W",0
		db	SEPARATOR
		db	"~D~ump position`",0
		db	"~C~ode position`",0
		db	SEPARATOR
		db	"R~e~store CS:IP`Ctrl+C",0
		db	"~R~estore regs`Ctrl+A",0
		db	"Restore ~f~lags`Ctrl+T",0
		db	SEPARATOR
		db	"~S~earch bytes...`Ctrl+S",0
		db	"Search a~g~ain`Ctrl+L",0
cpu_loc_sub	dw	0, edit_commands
		dw	0, new_csip
reg_mode_stat_1	dw	DISABLED, switch_reg_mode
		dw	0, data_watch
		dw	SUB_MENU, dump_pos
		dw	SUB_MENU, code_pos
		dw	0, restore_csip
		dw	0, restore_all
		dw	0, restore_flags
		dw	0, search_bytes
		dw	0, search_next
;=============================================================================
code_pos:
		db	40, 9, 40+21, 9+6
		dw	4
		db	0
		db	19
		dw	code_pos_sub
		dw	code_pos_str
code_pos_str	db	"~A~ddress...`Ctrl+G",0
		db	SEPARATOR
		db	"~O~rigin`Ctrl+O",0
		db	"~F~ollow`Ctrl+F",0
		db	"~P~revious`Ctrl+P",0
code_pos_sub	dw	0, unasm_go_to
		dw	0, go_to_origin
		dw	0, follow
		dw	0, previous
;=============================================================================
dump_pos:
		db	40, 8, 40+21, 8+15
		dw	13
		db	0
		db	19
		dw	dump_pos_sub
		dw	dump_pos_str
dump_pos_str	db	"~A~ddress...`Ctrl+D",0
		db	"~R~eference`Alt+=",0
		db	"~U~ser screen`Alt+U",0
		db	SEPARATOR
		db	"Dump ~D~S:SI`Alt+D",0
		db	"Dump ~E~S:DI`Alt+E",0
		db	"Dump ~S~S:BP`Alt+S",0
		db	"Dump ~C~S:IP`Alt+C",0
		db	"Dump ES:~B~X`Alt+B",0
		db	"Dump ES:S~I~`Alt+I",0
		db	"Dump DS:DX`Alt+~F~",0
		db	"Dump DS:DI`Alt+~J~",0
		db	"Dump D~X~:AX`Alt+A",0
		db	"Dump code`Alt+~L~",0
dump_pos_sub	dw	0, dump_go_to
		dw	0, dump_reference
		dw	0, dump_user_screen
		dw	0, dump_dssi
		dw	0, dump_esdi
		dw	0, dump_ssbp
		dw	0, dump_csip
		dw	0, dump_esbx
		dw	0, dump_essi
		dw	0, dump_dsdx
		dw	0, dump_dsdi
		dw	0, dump_dxax
		dw	0, dump_cur_line
;=============================================================================
dump_menu_struc:
		db	35, 14, 35+25, 14+7
		dw	5
		db	0
		db	23
		dw	dump_loc_sub
		dw	dump_loc_str
dump_loc_str	db	"~D~ump position`",0
		db	"Dump ~f~ollow`",0
		db	"~B~lock`",0
		db	SEPARATOR
		db	"~S~earch bytes...`Ctrl+S",0
		db	"Search a~g~ain`Ctrl+L",0
dump_loc_sub	dw	SUB_MENU, dump_pos
		dw	SUB_MENU, follow_menu_struc
		dw	SUB_MENU, block_menu_struc
		dw	0, search_bytes
		dw	0, search_next
;=============================================================================
follow_menu_struc:
		db	46, 15, 46+15, 15+8
		dw	6
		db	0
		db	13
		dw	follow_loc_sub
		dw	follow_loc_str
follow_loc_str	db	"~N~ear data",0
		db	"~F~ar data",0
		db	"~D~ata segment",0
		db	SEPARATOR
		db	"N~e~ar code",0
		db	"F~a~r code",0
		db	"~C~ode segment",0
follow_loc_sub	dw	0, follow_near_data
		dw	0, follow_far_data
		dw	0, follow_data_seg
		dw	0, follow_near_code
		dw	0, follow_far_code
		dw	0, follow_code_seg
;=============================================================================
block_menu_struc:
		db	46, 18, 46+15, 18+5
		dw	4
		db	0
		db	13
		dw	block_loc_sub
		dw	block_loc_str
block_loc_str	db	"~C~opy...",0
		db	"~F~ill...",0
		db	"~W~rite...",0
		db	"~R~ead...",0
block_loc_sub	dw	0, copy_mem_block
		dw	0, fill_mem_block
		dw	0, write_mem_block
		dw	0, read_mem_block
;=============================================================================
regs_local_struc:
		db	47, 10, 47+25, 10+11
		dw	8
		db	0
		db	23
		dw	regs_loc_sub
		dw	regs_loc_str
regs_loc_str	db	"~I~ncrement`Ctrl+I",0
		db	"~D~ecrement`Ctrl+D",0
		db	"~Z~ero`Ctrl+Z",0
		db	SEPARATOR
		db	"Rest~o~re`Ctrl+O",0
		db	"~R~estore all`Ctrl+A",0
		db	"R~e~store CS:IP`Ctrl+C",0
		db	"Restore ~f~lags`Ctrl+T",0
		db	SEPARATOR
regs_menu_2	db	"32-~b~it regs`Ctrl+R",0
regs_loc_sub	dw	0, inc_reg
		dw	0, dec_reg
		dw	0, zero_reg
		dw	0, restore_reg
		dw	0, _restore_all
		dw	0, _restore_csip
		dw	0, _restore_flags
reg_mode_stat_2	dw	DISABLED, _switch_reg_mode
;=============================================================================
flags_local_struc:
		db	47, 12, 47+25, 12+3
		dw	2
		db	0
		db	23
		dw	flags_loc_sub
		dw	flags_loc_str
flags_loc_str	db	"~T~oggle`Space",0
		db	"~R~estore flags`Ctrl+T",0
flags_loc_sub	dw	0, toggle_flag
		dw	0, restore_flags
;=============================================================================
; E0F
;=============================================================================


