;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; data\colors_b.inc
; Colors definition.
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


%define	MAIN_BG		BLUE
%define	MAIN_FG		LIGHT_CYAN
%define	BORDER_FG	LIGHT_CYAN
%define	TITLE_FG	YELLOW
%define	CPU_NAME_FG	LIGHT_GRAY

%define	CODE_MARK_FG	LIGHT_GREEN
%define	UNASM_ADDR_FG	LIGHT_CYAN
%define	DATA_WATCH_FG	LIGHT_RED
%define	ADDR_FG		LIGHT_GRAY
%define	NAME_FG		LIGHT_GRAY
%define	VALUE_FG	LIGHT_CYAN
%define	NEW_VALUE_FG	YELLOW

%define	OP_FG		CYAN
%define	INS_FG		LIGHT_CYAN
%define	NUM_FG		CYAN
%define	ARR_FG		WHITE
%define	DIS_FG		LIGHT_GRAY

%define	CPU_S_BG	CYAN
%define	ADR_S_FG	BLACK
%define	OP_S_FG		BLACK
%define	INS_S_FG	BLACK
%define	NUM_S_FG	BLACK
%define	ARR_S_FG	LIGHT_CYAN
%define	DIS_S_FG	DARK_GRAY

%define	CPU_B_BG	RED
%define	ADR_B_FG	WHITE
%define	OP_B_FG		WHITE
%define	INS_B_FG	WHITE
%define	NUM_B_FG	WHITE
%define	ARR_B_FG	WHITE
%define	DIS_B_FG	DARK_GRAY

%define	ADR_BD_FG	DARK_GRAY
%define	OP_BD_FG	DARK_GRAY
%define	INS_BD_FG	DARK_GRAY
%define	NUM_BD_FG	DARK_GRAY
%define	ARR_BD_FG	WHITE
%define	DIS_BD_FG	DARK_GRAY

%define	ADR_BDS_FG	DARK_GRAY
%define	OP_BDS_FG	DARK_GRAY
%define	INS_BDS_FG	DARK_GRAY
%define	NUM_BDS_FG	DARK_GRAY
%define	ARR_BDS_FG	WHITE
%define	DIS_BDS_FG	DARK_GRAY

%define	CPU_BS_BG	MAGENTA
%define	ADR_BS_FG	WHITE
%define	OP_BS_FG	WHITE
%define	INS_BS_FG	WHITE
%define	NUM_BS_FG	WHITE
%define	ARR_BS_FG	WHITE
%define	DIS_BS_FG	DARK_GRAY

%define	ADR_NF_FG	YELLOW
%define	OP_NF_FG	YELLOW
%define	INS_NF_FG	YELLOW
%define	NUM_NF_FG	YELLOW
%define	ARR_NF_FG	WHITE
%define	DIS_NF_FG	LIGHT_GRAY

%define	DUMP_CUR_BG	CYAN
%define	DUMP_CUR_FG	BLACK
%define	DUMP_ZERO_FG	CYAN

%define	MEUN_BG		CYAN
%define	MENU_S_BG	BLACK
%define	MENU_BORDER_FG	BLACK
%define	MENU_TXT_FG	WHITE
%define	MENU_HOT_FG	YELLOW
%define	MENU_DIS_FG	LIGHT_GRAY
%define	MENU_S_TXT_FG	WHITE
%define	MENU_S_HOT_FG	YELLOW
%define	MENU_S_DIS_FG	LIGHT_GRAY

%define	MSG_BOX_BG	LIGHT_GRAY
%define	MSG_BOX_B_FG	BLACK
%define	MSG_BOX_T_FG	WHITE
%define	MSG_BOX_M_FG	BLACK

%define	ERR_BOX_BG	RED
%define	ERR_BOX_B_FG	WHITE
%define	ERR_BOX_T_FG	YELLOW
%define	ERR_BOX_M_FG	WHITE

%define	DIALOG_BG	LIGHT_GRAY
%define	DIALOG_B_FG	BLACK
%define	DIALOG_T_FG	BLACK

%define	WINDOW_SH_BG	BLACK
%define	WINDOW_SH_FG	LIGHT_GRAY

%define	INPUT_BG	CYAN
%define	INPUT_FG	BLACK
%define	INPUT_S_BG	BLUE
%define	INPUT_S_FG	WHITE

%define	BUTTON_BG	CYAN
%define	BUTTON_FG	BLACK
%define	BUTTON_L_FG	YELLOW
%define	BUTTON_S_FG	WHITE
%define	BUTTON_SL_FG	YELLOW
%define	BUTTON_DEF_FG	WHITE
%define	BUTTON_LDEF_FG	YELLOW
%define	BUTTON_SH_FG	BLACK

%define	LABEL_FG	BLACK
%define	LABEL_L_FG	YELLOW
%define	LABEL_S_FG	WHITE
%define	LABEL_SL_FG	YELLOW

%define	LIST_BG		CYAN
%define	LIST_S_BG	BLACK
%define	LIST_FG		WHITE
%define	LIST_S_FG	WHITE
%define	LIST_D_FG	YELLOW

%define	CB_RB_BG	CYAN
%define	CB_RB_T_FG	BLACK
%define	CB_RB_S_FG	WHITE
%define	CB_RB_H_FG	YELLOW

%define	ASM_FG		WHITE
%define	ASM_B_FG	LIGHT_CYAN

%define	MEM_ADDR_FG	WHITE
%define	MEM_VAL_FG	YELLOW

; HELP: 48 62 48 48 63

;=============================================================================
; E0F
;=============================================================================
