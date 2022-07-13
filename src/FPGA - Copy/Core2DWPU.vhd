use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity Core2DWPU is
	port (clk, reset : in STD_LOGIC;
	
		newval_out : out STD_LOGIC_VECTOR(31 downto 0);
		
		LI, SQ, RE, TR, CI, HE, ST, EL : in STD_LOGIC_VECTOR(31 downto 0);
		LI_wr, SQ_wr, RE_wr, TR_wr, CI_wr, HE_wr, ST_wr, EL_wr : out STD_LOGIC;
		
		mem_addr : out STD_LOGIC_VECTOR(23 downto 0);
		mem_wr, mem_rd : out STD_LOGIC;
		mem_rdy : in STD_LOGIC;
		mem_in : in STD_LOGIC_VECTOR(31 downto 0)
		);
end entity;

architecture default of Core2DWPU is
	signal opcode : STD_LOGIC_VECTOR(15 downto 0);
	signal instr_addr : STD_LOGIC_VECTOR(12 downto 0);
	signal instr_flow, instr_return : STD_LOGIC;
	signal IScurrent, ISnew, ISpush : STD_LOGIC_VECTOR(2 downto 0);
	signal IS_wr, ISzero : STD_LOGIC;
	
	signal op : operation2DWPU;
	
	signal operandLoad, operandStore, operandInc, operandDec,
		operandReady : STD_LOGIC;
		
	signal operandAddr_offset : STD_LOGIC_VECTOR(7 downto 0);
	signal operandSelectReg : register2DWPU;
	signal operandSelectMem : dataLocation;
	signal wr_block : STD_LOGIC;
	
	signal index : STD_LOGIC_VECTOR(9 downto 0);
	signal newvalue, opresult, index_val : STD_LOGIC_VECTOR(31 downto 0);
	signal ARG, ARGtop, ARGnew, PO, PE : STD_LOGIC_VECTOR(31 downto 0);
	signal ARG_wr, ARG_wr_top, ARG_wr_newval, ARG_wr_opresult, PO_wr, PE_wr : STD_LOGIC;
	
	signal argStackPush, argStackPop, poptoARG : STD_LOGIC;
	signal ifwStackPush, ifwStackPop : STD_LOGIC;

	
	signal flowDir, newReturnDir, returnDir : direction;
	signal flowLength, returnLength : dirLength;

	
begin
	newval_out <= newvalue;

	controller : entity work.InstrDecoder(default)
	port map ( clk, reset, opcode, IScurrent, ISpush, ISzero,
	operandLoad, operandStore, operandInc, operandDec,
	operandAddr_offset,
	operandSelectReg, operandSelectMem,
	operandReady, wr_block,
	newvalue,
	index,
	flowDir, newReturnDir,
	returnDir,
	flowLength,
	instr_flow, instr_return,
	
	argStackPush, argStackPop, poptoARG,
	ifwStackPush, ifwStackPop,
	
	op, ARG_wr_opresult);
	
	PC : entity work.ProgramCounter(default)
	port map ( clk, reset, flowDir, returnDir, flowLength, returnLength,
		instr_flow, instr_return, instr_addr);
	
	ProgramMem : entity work.ProgramMem(default)
	port map ( clk, instr_addr, index, opcode, index_val );
	
	ALU : entity work.Operation(default)
	port map ( clk, op, ARG, ARGtop, opresult, '0' );
	
	OperandMove : entity work.OperandInOut(default)
	port map ( clk, reset, operandLoad, operandStore, operandInc, operandDec,
		operandReady, wr_block,
		ARG, LI, SQ, RE, TR, CI, PE, HE, ST, EL, PO, index_val,
		ARG_wr_newval, LI_wr, SQ_wr, RE_wr, TR_wr, CI_wr, PE_wr, HE_wr, ST_wr, EL_wr, PO_wr,
		newvalue, operandAddr_offset, operandSelectReg, operandSelectMem,
		mem_addr, mem_wr, mem_rd, mem_rdy, mem_in);
	
	privateRegs : entity work.PrivateWorkRegisters(default)
	port map ( clk, reset, newvalue, PO_wr, PE_wr, PO, PE);
	
	ARGreg : entity work.GenReg32(default)
		port map (clk, ARG_wr, reset, ARGnew, ARG);
		
	ISregister : entity work.ISreg(default)
		port map (clk, reset, IS_wr, ISzero, ISnew, IScurrent);
	
	ARGdemux : entity work.RegWriteDemux(default)
	port map ( ARG_wr_top, ARG_wr_newval, ARG_wr_opresult, '0', '0', '0', '0', '0', 
		ARGtop, newvalue, opresult, X"00000000", X"00000000", X"00000000", X"00000000", X"00000000",
		ARG_wr, ARGnew);
	
	IFWstack : entity work.IFW_Stack(default)
	port map ( clk, reset, ifwStackPush, ifwStackPop, 
		ISpush, '0', flowLength, newReturnDir,
		ISnew, open, returnLength, returnDir, IS_wr);
		
	ARGstack : entity work.ArgumentStack(default)
	port map ( clk, reset, ARG, argStackPush, argStackPop, poptoARG, ARGtop, ARG_wr_top);
	
end architecture;