library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

package declarations2DWPU is

	type direction is ( dirU, dirR, dirD, dirL, dirC );
	
	type register2DWPU is ( regNONE, regARG, regLI, regSQ, regRE, 
		regTR, regCI, regPE, regHE, regST, regEL, regPO,
		regSI, regSA, regIC, regIB, regBS, regTC, regSW,
		regTV0, regTV1, regTV2, regTV3, regTF0, regTF1, regTF2, regTF3);
		
	type dataLocation is ( locReg, locIndex, locMemST, locMemIndex, locIoHE, locIoIndex );
	
	type instrType is ( instrQRY, instrConditionQuery, instrOperation, instrAction, instrIndex);
	type instrActionType is ( actionMOV, actionPUSH, actionPOP, actionINC, actionDEC, actionSETB, actionCLR);
	type instrActionTime is ( timeEntry, timeReturn, timeBoth, timeImmediate);
		
	type operation2DWPU is ( opNONE, opIDENTITY,
		opALU_ADD,
		opALU_ADC,
		opALU_SUB,
		opALU_SBB,
		opALU_MUL,
		opALU_MULH,
		opALU_DIV,
		opALU_MOD,
		opALU_AND,
		opALU_OR,
		opALU_XOR,
		opALU_NOT,
		opALU_LAND,
		opALU_LOR,
		opALU_LXOR,
		opALU_LNOT,
		opALU_RL,
		opALU_RR,
		opALU_SL,
		opALU_SR,
		opALU_UMIN,
		opALU_UMAX,
		opALU_SMIN,
		opALU_SMAX,
		opALU_SDIV,
		
		opFPU_FADD,
		opFPU_FSUB,
		opFPU_FMUL,
		opFPU_FDIV,
		opFPU_SQR,
		opFPU_SQRT,
		opFPU_FPOW,
		opFPU_FROOT,
		opFPU_FSIN,
		opFPU_FCOS,
		opFPU_FTAN,
		opFPU_FLOG2,
		opFPU_FLOG10,
		opFPU_FLN,
		opFPU_FABS,
		opFPU_FTOINT,
		opFPU_FROMINT,
		opFPU_FMIN,
		opFPU_FMAX,
		opFPU_FFRAC);		
	
	
	attribute enum_encoding : string;
	attribute enum_encoding of direction : type is "sequential";
	attribute enum_encoding of dataLocation : type is "sequential";
	attribute enum_encoding of register2DWPU : type is "sequential";
	attribute enum_encoding of operation2DWPU : type is "johnson";
	
	attribute enum_encoding of instrType : type is "sequential";
	
	subtype int4 is integer range 0 to 15;
	
	type dirLength is ( len1, len2, len3, len4 );
	attribute enum_encoding of dirLength : type is "00 01 10 11";
	
	subtype axisPC is integer range 0 to 31;
	
end package;