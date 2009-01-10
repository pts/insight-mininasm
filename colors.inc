;=============================================================================
; Insight, real-mode debugger for MS DOS / PC DOS / FreeDOS.
; Copyright (c) Victor M. Gamayunov, Sergey Pimenov, 1993, 96, 97, 2002.
; Modifications by Oleg O. Chukaev (2006 - 2009).
;-----------------------------------------------------------------------------
; colors.inc
; Constants: colors.
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
; Main screen
;-----------------------------------------------------------------------------
atMainWindow		equ	MAIN_BG*16+MAIN_FG
atTitle			equ	MAIN_BG*16+TITLE_FG
atCPUborder		equ	MAIN_BG*16+BORDER_FG
atCPUName		equ	MAIN_BG*16+CPU_NAME_FG

;atCmdNormal		equ	atNormInstr
;atCmdSelected		equ	atSelInstr
;atBP_and_Cur		equ	atBrkSelInstr
;atBreakPoint		equ	atBrkInstr

atNormAddr		equ	MAIN_BG*16+ADDR_FG
atNormOpcode		equ	MAIN_BG*16+OP_FG
atNormInstr		equ	MAIN_BG*16+INS_FG
atNormNumber		equ	MAIN_BG*16+NUM_FG
atNormArrow		equ	MAIN_BG*16+ARR_FG
atNormDisable		equ	MAIN_BG*16+DIS_FG
                	
atSelAddr		equ	CPU_S_BG*16+ADR_S_FG
atSelOpcode		equ	CPU_S_BG*16+OP_S_FG
atSelInstr		equ	CPU_S_BG*16+INS_S_FG
atSelNumber		equ	CPU_S_BG*16+NUM_S_FG
atSelArrow		equ	CPU_S_BG*16+ARR_S_FG
atSelDisable		equ	CPU_S_BG*16+DIS_S_FG

atBrkAddr		equ	CPU_B_BG*16+ADR_B_FG
atBrkOpcode		equ	CPU_B_BG*16+OP_B_FG
atBrkInstr		equ	CPU_B_BG*16+INS_B_FG
atBrkNumber		equ	CPU_B_BG*16+NUM_B_FG
atBrkArrow		equ	CPU_B_BG*16+ARR_B_FG
atBrkDisable		equ	CPU_B_BG*16+DIS_B_FG

atBrkDisAddr		equ	CPU_B_BG*16+ADR_BD_FG
atBrkDisOpcode		equ	CPU_B_BG*16+OP_BD_FG
atBrkDisInstr		equ	CPU_B_BG*16+INS_BD_FG
atBrkDisNumber		equ	CPU_B_BG*16+NUM_BD_FG
atBrkDisArrow		equ	CPU_B_BG*16+ARR_BD_FG
atBrkDisDisable		equ	CPU_B_BG*16+DIS_BD_FG

atBrkSelAddr		equ	CPU_BS_BG*16+ADR_BS_FG
atBrkSelOpcode		equ	CPU_BS_BG*16+OP_BS_FG
atBrkSelInstr		equ	CPU_BS_BG*16+INS_BS_FG
atBrkSelNumber		equ	CPU_BS_BG*16+NUM_BS_FG
atBrkSelArrow		equ	CPU_BS_BG*16+ARR_BS_FG
atBrkSelDisable		equ	CPU_BS_BG*16+DIS_BS_FG

atBrkDSAddr		equ	CPU_BS_BG*16+ADR_BDS_FG
atBrkDSOpcode		equ	CPU_BS_BG*16+OP_BDS_FG
atBrkDSInstr		equ	CPU_BS_BG*16+INS_BDS_FG
atBrkDSNumber		equ	CPU_BS_BG*16+NUM_BDS_FG
atBrkDSArrow		equ	CPU_BS_BG*16+ARR_BDS_FG
atBrkDSDisable		equ	CPU_BS_BG*16+DIS_BDS_FG

atNFAddr		equ	MAIN_BG*16+ADR_NF_FG
atNFOpcode		equ	MAIN_BG*16+OP_NF_FG
atNFInstr		equ	MAIN_BG*16+INS_NF_FG
atNFNumber		equ	MAIN_BG*16+NUM_NF_FG
atNFArrow		equ	MAIN_BG*16+ARR_NF_FG
atNFDisable		equ	MAIN_BG*16+DIS_NF_FG

;atCmdNormal		equ	LIGHT_GREEN
;atCmdSelected		equ	LIGHT_GRAY*16
;atBreakPoint		equ	RED*16+WHITE
;atBP&Cur		equ	MAGENTA*16+WHITE

;atAsmLine		equ	BLACK*16+WHITE

atCodeMark		equ	MAIN_BG*16+CODE_MARK_FG

atRegName		equ	MAIN_BG*16+NAME_FG
atRegValue		equ	MAIN_BG*16+VALUE_FG
atRegNewValue		equ	MAIN_BG*16+NEW_VALUE_FG

atFlagValue		equ	MAIN_BG*16+VALUE_FG
atFlagNewValue		equ	MAIN_BG*16+NEW_VALUE_FG

atDumpAddr		equ	MAIN_BG*16+ADDR_FG
atDumpValue		equ	MAIN_BG*16+VALUE_FG
atDumpZero		equ	MAIN_BG*16+DUMP_ZERO_FG
atDumpCursor		equ	DUMP_CUR_BG*16+DUMP_CUR_FG

atStackName		equ	MAIN_BG*16+NAME_FG
atStackAddr		equ	MAIN_BG*16+ADDR_FG
atStackValue		equ	MAIN_BG*16+VALUE_FG

atMemAddr		equ	MAIN_BG*16+MEM_ADDR_FG
atMemValue		equ	MAIN_BG*16+MEM_VAL_FG

atCurUnasmAddr		equ	MAIN_BG*16+UNASM_ADDR_FG

atDataWatchMem		equ	MAIN_BG*16+DATA_WATCH_FG

atAssembler		equ	CPU_S_BG*16+ASM_FG
atsAssembler		equ	CPU_S_BG*16+ASM_FG
atBrkAssembler		equ	CPU_BS_BG*16+ASM_B_FG
atsBrkAssembler		equ	CPU_BS_BG*16+ASM_B_FG

;=============================================================================
; Menu.
;-----------------------------------------------------------------------------
atMenuBorder		equ	MEUN_BG*16+MENU_BORDER_FG
atMenuNorm		equ	MEUN_BG*16+MENU_TXT_FG
atMenuNormHot		equ	MEUN_BG*16+MENU_HOT_FG
atMenuNormDis		equ	MEUN_BG*16+MENU_DIS_FG
atMenuSel		equ	MENU_S_BG*16+MENU_S_TXT_FG
atMenuSelHot		equ	MENU_S_BG*16+MENU_S_HOT_FG
atMenuSelDis		equ	MENU_S_BG*16+MENU_S_DIS_FG

;=============================================================================
; Horizontal menu.
;-----------------------------------------------------------------------------
atHMenuNorm		equ	MEUN_BG*16+MENU_TXT_FG
atHMenuNormHot		equ	MEUN_BG*16+MENU_HOT_FG
atHMenuSel		equ	MENU_S_BG*16+MENU_S_TXT_FG
atHMenuSelHot		equ	MENU_S_BG*16+MENU_S_HOT_FG

;=============================================================================
; Input line.
;-----------------------------------------------------------------------------
atInputLine		equ	INPUT_BG*16+INPUT_FG
atsInputLine		equ	INPUT_S_BG*16+INPUT_S_FG

;=============================================================================
; Message box / Error box.
;-----------------------------------------------------------------------------
atMessageBox		equ	MSG_BOX_BG*16+MSG_BOX_B_FG
atMessageBoxTitle	equ	MSG_BOX_BG*16+MSG_BOX_T_FG
atMessage		equ	MSG_BOX_BG*16+MSG_BOX_M_FG

atErrorBox		equ	ERR_BOX_BG*16+ERR_BOX_B_FG
atErrorBoxTitle		equ	ERR_BOX_BG*16+ERR_BOX_T_FG
atErrorMsg		equ	ERR_BOX_BG*16+ERR_BOX_M_FG

;=============================================================================
; Dialog windows.
;-----------------------------------------------------------------------------
atDialogWindow		equ	DIALOG_BG*16+DIALOG_B_FG
atDialogWindowTitle	equ	DIALOG_BG*16+DIALOG_T_FG
atShadow		equ	WINDOW_SH_BG*16+WINDOW_SH_FG

;=============================================================================
; Buttons.
;-----------------------------------------------------------------------------
atPushButton		equ	BUTTON_BG*16+BUTTON_FG
atlPushButton		equ	BUTTON_BG*16+BUTTON_L_FG
atsPushButton		equ	BUTTON_BG*16+BUTTON_S_FG
atlsPushButton		equ	BUTTON_BG*16+BUTTON_SL_FG
atPushButtonDef		equ	BUTTON_BG*16+BUTTON_DEF_FG
atlPushButtonDef	equ	BUTTON_BG*16+BUTTON_LDEF_FG
atButtonShadow		equ	DIALOG_BG*16+BUTTON_SH_FG

;=============================================================================
; Labels.
;-----------------------------------------------------------------------------
atLabel			equ	DIALOG_BG*16+LABEL_FG
atlLabel		equ	DIALOG_BG*16+LABEL_L_FG
atsLabel		equ	DIALOG_BG*16+LABEL_S_FG
atlsLabel		equ	DIALOG_BG*16+LABEL_SL_FG

;=============================================================================
; String list.
;-----------------------------------------------------------------------------
atListNormal		equ	LIST_BG*16+LIST_FG
atListSelected		equ	LIST_S_BG*16+LIST_S_FG
atListDefault		equ	LIST_BG*16+LIST_D_FG

;=============================================================================
; Check boxes.
;-----------------------------------------------------------------------------
atCheckBoxes		equ	CB_RB_BG*16+CB_RB_T_FG
atsCheckBoxes		equ	CB_RB_BG*16+CB_RB_S_FG
atlCheckBoxes		equ	CB_RB_BG*16+CB_RB_H_FG

;=============================================================================
; Radio buttons.
;-----------------------------------------------------------------------------
atRadioButtons		equ	CB_RB_BG*16+CB_RB_T_FG
atsRadioButtons		equ	CB_RB_BG*16+CB_RB_S_FG
atlRadioButtons		equ	CB_RB_BG*16+CB_RB_H_FG

;=============================================================================
; E0F
;=============================================================================

