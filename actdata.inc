;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; actdata.inc
; Data: initialized variables.
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
; 2007-02-06	0leg	Some variables added.
; 2007-02-08	0leg	Added entry w/ kbCtrlM into `main_loop'.
; 2007-03-18	0leg	Added items_table.  Changed flags_str.
; 2007-04-01	0leg	reg_names removed.
; 2007-12-09	0leg	'string' -> "string"
; 2008-01-02	0leg	point_options, string_changed moved to actudata.inc
;=============================================================================


;=============================================================================
main_loop	dw	kbAltX,		0
		dw	kbF10,		execute_menu
		dw	kbAltAlone,	execute_menu
		dw	kbAltF10,	cpu_local_menu
		dw	kbF1,		show_help

		dw	kbDown,		cur_down
		dw	kbUp,		cur_up
		dw	kbPgDn,		page_down
		dw	kbPgUp,		page_up
		dw	kbHome,		cur_home
		dw	kbEnd,		cur_end
		dw	kbRight,	cur_right
		dw	kbLeft,		cur_left
		dw	kbCtrlLeft,	byte_up
		dw	kbCtrlRight,	byte_down

		dw	kbCtrlEnter,	go_to_origin
		dw	kbCtrlO,	go_to_origin
		dw	kbCtrlN,	new_csip
		dw	kbCtrlG,	unasm_go_to
		dw	kbCtrlD,	dump_go_to
		dw	kbCtrlB,	block_menu
		dw	kbCtrlF,	follow
		dw	kbEnter,	follow
		dw	kbCtrlP,	previous
		dw	kbBackSpace,	previous

		dw	kbCtrlA,	restore_all
		dw	kbCtrlT,	restore_flags
		dw	kbCtrlC,	restore_csip
		dw	kbCtrlR,	switch_reg_mode

		dw	kbCtrlS,	search_bytes
		dw	kbCtrlL,	search_next

		dw	kbAltF5,	exec_user_screen
		dw	kbF5,		draw_screen

		dw	kbF6,		skip_instruction
		dw	kbF7,		single_step
		dw	kbF8,		proc_trace
		dw	kbCtrlF9,	run_program
		dw	kbF4,		go_to_cursor
		dw	kbF2,		toggle_break
		dw	kbAltF2,	toggle_activity

		dw	kbCtrlF2,	reset_program
		dw	kbF3,		load_file

		dw	kbCtrlM,	list_memory_blocks

		dw	kbCtrlW,	data_watch

		dw	kbAltE,		dump_esdi
		dw	kbAltD,		dump_dssi
		dw	kbAltB,		dump_esbx
		dw	kbAltS,		dump_ssbp
		dw	kbAltC,		dump_csip
		dw	kbAltA,		dump_dxax
		dw	kbAltF,		dump_dsdx
		dw	kbAltI,		dump_essi
		dw	kbAltJ,		dump_dsdi
		dw	kbAltL,		dump_cur_line
		dw	kbAltEqu,	dump_reference
		dw	kbAltU,		dump_user_screen

		dw	kbAlt1,		set_code_mark_1
		dw	kbAlt2,		set_code_mark_2
		dw	kbAlt3,		set_code_mark_3
		dw	kbAlt4,		set_code_mark_4
		dw	kbAlt5,		set_code_mark_5
		dw	kbAlt6,		set_code_mark_6
		dw	kbAlt7,		set_code_mark_7
		dw	kbAlt8,		set_code_mark_8
		dw	kbAlt9,		set_code_mark_9
		dw	kbAlt0,		set_code_mark_0

		dw	0200h + '1',	code_mark_1
		dw	0300h + '2',	code_mark_2
		dw	0400h + '3',	code_mark_3
		dw	0500h + '4',	code_mark_4
		dw	0600h + '5',	code_mark_5
		dw	0700h + '6',	code_mark_6
		dw	0800h + '7',	code_mark_7
		dw	0900h + '8',	code_mark_8
		dw	0A00h + '9',	code_mark_9
		dw	0B00h + '0',	code_mark_0

		dw	0200h + '!',	clear_code_mark_1
		dw	0300h + '@',	clear_code_mark_2
		dw	0400h + '#',	clear_code_mark_3
		dw	0500h + '$',	clear_code_mark_4
		dw	0600h + '%',	clear_code_mark_5
		dw	0700h + '^',	clear_code_mark_6
		dw	0800h + '&',	clear_code_mark_7
		dw	0900h + '*',	clear_code_mark_8
		dw	0A00h + '(',	clear_code_mark_9
		dw	0B00h + ')',	clear_code_mark_0

		dw	0,		any_key

magic_code	db	'I'^45h, 'D'^45h, 'D'^45h, 'Q'^45h, 'D'^45h

;-----------------------------------------------------------------------------
;ax bx cx dx si di bp sp cs ds es ss ip fl
; 0  1  2  3  4  5  6  7  8  9 10 11 12 13
;
; Distance between this reg. struc and linked struc --.
; Pointer to reg's content ----.    ,----,---,----,---'  
; Reg's position (X/Y) --.--.  |    L    R   U    D
;                        |  |  |    |    |   |    |
reg16_pos_table:;        V  V  V    V    V   V    V
		reg_pos	 3, 0, AX, 13,   4, 12,   1	;FL, SI, IP, BX
		reg_pos	 3, 1, BX,  7,   4, -1,   1	;CS, DI, AX, CX
		reg_pos	 3, 2, CX,  7,   4, -1,   1	;DS, BP, BX, DX
		reg_pos	 3, 3, DX,  7,   4, -1,   9	;ES, SP, CX, IP
		reg_pos	12, 0, SI, -4,   4,  3,   1	;AX, CS, SP, DI
		reg_pos	12, 1, DI, -4,   4, -1,   1	;BX, DS, SI, BP
		reg_pos	12, 2, BP, -4,   4, -1,   1	;CX, ES, DI, SP
		reg_pos	12, 3, SP, -4,   4, -1,   5	;DX, SS, BP, IP
		reg_pos	21, 0, CS, -4,  -7,  5,   1	;SI, BX, FL, DS
		reg_pos	21, 1, DS, -4,  -7, -1,   1	;DI, CX, CS, ES
		reg_pos	21, 2, ES, -4,  -7, -1,   1	;BP, DX, DS, SS
		reg_pos	21, 3, SS, -4,   1, -1,   2	;SP, IP, ES, FL
		reg_pos	 3, 5, IP, -1,   1, -9, -12	;SS, FL, DX, AX
		reg_pos	21, 5, FL, -1, -13, -2,  -5	;IP, AX, SS, CS

;-----------------------------------------------------------------------------
;eaxh eaxl ebxh ebxl ecxh ecxl edxh edxl esih esil edih edil
;   0    1    2    3    4    5    6    7    8    9   10   11
;ebph ebpl esph espl ecs eds ees ess efs egs eip efl
;  12   13   14   15  16  17  18  19  20  21  22  23
reg32_pos_table:
		reg_pos	 4, 0, AX+2,  23,   1, 14,   2	;EFL, EAXL, ESPH, EBXH
		reg_pos	 9, 0,   AX,  -1,  15, 14,   2	;EAXH, ECS, ESPL, EBXL
		reg_pos	 4, 1, BX+2,  14,   1, -2,   2	;ECS, EBXL, EAXH, ECXH
		reg_pos	 9, 1,   BX,  -1,  14, -2,   2	;EBXH, EDS, EAXL, ECXL
		reg_pos	 4, 2, CX+2,  13,   1, -2,   2	;EDS, ECXL, EBXH, EDXH
		reg_pos	 9, 2,   CX,  -1,  13, -2,   2	;ECXH, EES, EBXL, EDXL
		reg_pos	 4, 3, DX+2,  12,   1, -2,   2	;EES, EDXL, ECXH, ESIH
		reg_pos	 9, 3,   DX,  -1,  12, -2,   2	;EDXH, ESS, ECXL, ESIL
		reg_pos	 4, 4, SI+2,  11,   1, -2,   2	;ESS, ESIL, EDXH, EDIH
		reg_pos	 9, 4,   SI,  -1,  11, -2,   2	;ESIH, EFS, EDXL, EDIL
		reg_pos	 4, 5, DI+2,  10,   1, -2,   2	;EFS, EDIL, ESIH, EBPH
		reg_pos	 9, 5,   DI,  -1,  10, -2,   2	;EDIH, EGS, ESIL, EBPL
		reg_pos	 4, 7, BP+2,   9,   1, -2,   2	;EGS, EBPL, EDIH, ESPH
		reg_pos	 9, 7,   BP,  -1,   9, -2,   2	;EBPH, EIP, EDIL, ESPL
		reg_pos	 4, 8, SP+2,   8,   1, -2, -14	;EIP, ESPL, EBPH, EAXH
		reg_pos	 9, 8,   SP,  -1,   8, -2, -14	;ESPH, EFL, EBPL, EAXL
		reg_pos	21, 0,   CS, -15, -14,  7,   1	;EAXL, EBXH, EFL, EDS
		reg_pos	21, 1,   DS, -14, -13, -1,   1	;EBXL, ECXH, ECS, EES
		reg_pos	21, 2,   ES, -13, -12, -1,   1	;ECXL, EDXH, EDS, ESS
		reg_pos	21, 3,   SS, -12, -11, -1,   1	;EDXL, ESIH, EES, EFS
		reg_pos	21, 4,   FS, -11, -10, -1,   1	;ESIL, EDIH, ESS, EGS
		reg_pos	21, 5,   GS, -10,  -9, -1,   1	;EDIL, EBPH, EFS, EIP
		reg_pos	21, 7,   IP,  -9,  -8, -1,   1	;EBPL, ESPH, EGS, EFL
		reg_pos	21, 8,   FL,  -8, -23, -1,  -7	;ESPL, EAXH, EIP, ECS
;=============================================================================
;			address/opcode/instruction/number/arrow/disabled
cmd_normal	db	atNormAddr,	atNormOpcode,	atNormInstr
		db	atNormNumber,	atNormArrow,	atNormDisable

cmd_selected	db	atSelAddr,	atSelOpcode,	atSelInstr
		db	atSelNumber,	atSelArrow,	atSelDisable

cmd_breakpoint	db	atBrkAddr,	atBrkOpcode,	atBrkInstr
		db	atBrkNumber,	atBrkArrow,	atBrkDisable

cmd_brk_disabled db	atBrkDisAddr,	atBrkDisOpcode,	atBrkDisInstr
		db	atBrkDisNumber,	atBrkDisArrow,	atBrkDisDisable

cmd_brk_selected db	atBrkSelAddr,	atBrkSelOpcode,	atBrkSelInstr
		db	atBrkSelNumber,	atBrkSelArrow,	atBrkSelDisable

cmd_brk_dis_sel	db	atBrkDSAddr,	atBrkDSOpcode,	atBrkDSInstr
		db	atBrkDSNumber,	atBrkDSArrow,	atBrkDSDisable

cmd_not_focus	db	atNFAddr, atNFOpcode, atNFInstr
		db	atNFNumber, atNFArrow, atNFDisable
;-----------------------------------------------------------------------------

asm_exit_keys	dw	kbEnter, kbEsc, kbDown, kbUp, kbPgDn, kbPgUp, 0

asm_errors:
		dw	ms_mnemonic		;	2
		dw	ms_invalid_param	;	3
		dw	ms_mem_param		;	4
		dw	ms_extra		;	5
		dw	ms_big_number		;	6
		dw	ms_reg_bits		;	7
		dw	ms_param_type		;	8
		dw	ms_rep_error		;	9

ms_mnemonic	db	"Unknown mnemonic", 0
ms_invalid_param db	"Invalid parameter(s)", 0
ms_mem_param	db	"Invalid register addressing", 0
ms_extra	equ	ms_invalid_param	;db	'Too many parameters',0
ms_big_number	db	"Number too big", 0
ms_reg_bits	equ	ms_invalid_param
ms_param_type	equ	ms_invalid_param
ms_rep_error	db	"Incorrect prefix use", 0

main_title	db	" ", SERIAL_NUMBER, " ", 0
TITLE_LEN	equ	$-main_title

resident_msg	db	"Debugger has been installed resident.", 13, 10
		db	"  Press Ctrl+Esc to activate.", 13, 10, '$'
already_resident db	"Debugger already resident", 0
addr_error_msg	db	"Invalid address value", 0
count_error_msg	db	"Invalid count value", 0
write_error_msg	db	"Can't write block on disk", 0
read_error_msg	db	"Can't read block from disk", 0
open_error_msg	db	"Can't open file", 0
create_error_msg db	"Can't create file", 0
no_specif_msg	db	"No program loaded", 0
load_error_msg	db	"Can't load program", 0
terminate_msg	db	"Program terminated ("
term_method	db	"*). Return code: "
return_code	db	"000", 0
dump_not_in_ram	db	"This address is not in RAM", 0
many_bp_msg	db	"Too many breakpoints", 0
bp_not_in_ram	db	"Breakpoint not in RAM", 0
code_not_in_ram	db	"Code not in RAM", 0
asm_error_msg	db	"Invalid assembler instruction", 0
code_mark_msg	db	"Code marks and "
follow_stack_msg db	"Follow stack cleared.", 0
string_error_msg db	"Invalid string.", 0
not_found_msg	db	"Bytes not found.", 0
search_msg	db	"Searching bytes...", 0

env_name	db	"TEMP.COM", 0		;!!!
rreg		db 	"AXBXCXDXSIDIBPSP"	;!!!
rreg_segs	db	"CSDSESSSFSGSIPFL"	;!!!
flags_msg	db	"Flags=", 0		;!!!
flags_str	db	"ODISZAPC"		;First letters of flags names

data_watch_labels:
		db	"ESDI"
		db	"DSSI"
		db	"ESBX"
		db	"SSBP"
		db	"DXAX"
		db	"DSDX"
		db	"ESSI"
		db	"DSDI"
		db	"CSIP"

file_mask	db	"*.*", 0	;!!!

cpu_name	dw	cpu_8086	;!!!

cpu_names:
		dw	cpu_8086
		dw	cpu_8088
		dw	cpu_80286
		dw	cpu_80386
		dw	cpu_80486

cpu_8086	db	" 8086 ", 0
cpu_8088	db	" 8088 ", 0
cpu_80286	db	" 80286 ", 0
cpu_80386	db	" 80386 ", 0
cpu_80486	db	" 80486 ", 0
cpu_advanced	db	" "
cpu_advanced_name db	"???????????? "
cpu_advanced_id	db	"086 ", 0

reg_shift	db	11, 10, 9, 7, 6, 4, 2, 0	;!!!

follow_stack_ptr dw	follow_stack	;!!!

code_end	dw	program_end	;!!!

save_dta_offs	dw	80h		;!!!

cur_ip		dw	100h		;IP of _first_ instruction in Code Wnd
cur_ip_line	dw	-1		;Number of line w/ <> sign, -1 if hidden
user_screen_mode db	1		;1 if user screen is displayed

screen_height	dw	SCR_ROWS-1	;(Height of screen) - 1
dump_height	dw	DUMP_HEIGHT	;Actual height of the Dump window
e_dump_height	dw	DUMP_HEIGHT	;Height of normal (shrinked) Dump window

stack_height	db	STK16_LEN	;Height of the Stack window
stack_wnd_xy	dw	0		;Saved X and Y (relative) of cursor

ED_SSSP		equ	$-offset_0100h
ED_SSBP		equ	ED_SSSP+5
edit_sssp_params:
		db	STK16_X+8	;Init. X position of cursor in SS:SP
initial_sp_val	dw	0fffeh		;Init. SP value (bottom of SS:SP wnd)
current_sp	dw	0		;Current offset in SP part of Stack wnd

edit_ssbp_params:
		db	STK16_X+20	;Init. X position of cursor in SS:BP
initial_bp_disp	dw	-4		;Init. BP displ-t (bottom of SS:BP wnd)
current_bp	dw	0		;Current offset in BP part of Stack wnd

stack_mode	dw	edit_sssp_params

large_dump	db	0		;1 if the Dump window is expanded
dump_y		db	DUMP_Y		;First row of the Dump window
dump_start	dw	DUMP_START	;Initial offset of Dump in video mem.

loaded:
ref_flags	db	1		;!!!
focus		db	1		;!!!
regs_saved	db	0		;!!!

;=============================================================================

edit_procs:
		db	kbAltR / 256
		dw	edit_registers
		db	kbAltA / 256
		dw	edit_commands
		db	kbAltF / 256
		dw	edit_flags
		db	kbAltD / 256
		dw	edit_dump
		db	kbAltS / 256
		dw	edit_stack
edit_procs_end:

EDIT_PROCS_NUM	equ	(edit_procs_end - edit_procs) / 3

;=============================================================================
help_page1:
		incbin	"data\page1.xlp"	;Compressed help screen
help_page2:
		incbin	"data\page2.xlp"	;Compressed help screen

;=============================================================================
; 2007-02-06
;-----------------------------------------------------------------------------
delay_value	dw	0		;Delay for `animate'
umb_available	db	0		;0 if UMB is not available

msg_free	db	"free"		;Strings for List Memory Blocks
msg_dos		db	"dos "		;Exactly 4 chars!
msg_prog	db	"prog"		;...
msg_env		db	"env "
msg_data	db	"data"
msg_noname	db	"<noname>"	;Exactly 8 chars!

;=============================================================================
; keyboard.inc
;-----------------------------------------------------------------------------
test_key	dw	def_test_key	;Ptr to default key testing procedure

;=============================================================================
; dialogs.inc
;-----------------------------------------------------------------------------
inp_line_exit_keys:
		dw	kbEnter, kbEsc, kbTab, kbShiftTab, 0

items_table	dw	init_push_button, run_push_button
		dw	init_input_line, run_input_line
		dw	init_static_text, run_static_text
		dw	init_check_boxes, run_check_boxes
		dw	init_list, run_list

;=============================================================================
; windows.inc
;-----------------------------------------------------------------------------
message_title	db	9, atMessageBoxTitle, " Message "	;Msg box title
error_title	db	7, atErrorBoxTitle, " Error "		;Error box title
empty_title	db	0, 0, 0					;Empty title
exec_msg_type	db	0		;!!!

double_border	db	"ÉÍ»ººÈÍ¼"	;Shars for double...
single_border	db	"ÚÄ¿³³ÀÄÙ"	;...and single borders

fake_table	dw	SUB_MENU, 0	;!!!
h_pos		dw	0		;!!!

test_char	dw	default_test_char	;!!!

read_exit_keys	dw	default_exit_keys	;!!!
default_exit_keys dw	kbEsc, kbEnter, 0	;!!!

menu_keys	dw	kbEsc,   0
		dw	kbEnter, 0
		dw	kbLeft,  0
		dw	kbRight, 0
		dw	kbUp,   vert_menu_up
		dw	kbDown, vert_menu_down
		dw	kbHome, vert_menu_home
		dw	kbEnd,  vert_menu_end
		dw	kbPgUp, vert_menu_home
		dw	kbPgDn, vert_menu_end
		dw	0,	0

video_seg	dw	COLOR_SEG	;Segment of video buffer
buf_pos		dw	window_buffer	;Ptr to windows buffer
buf_pos_mark	dw	window_buffer	;2nd ptr to windows buffer

;=============================================================================
; E0F
;=============================================================================


