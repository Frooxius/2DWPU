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
		mem_in : in STD_LOGIC_VECTOR(31 downto 0);
		
		io_addr : out STD_LOGIC_VECTOR(15 downto 0);
		io_wr, io_rd : out STD_LOGIC;
		io_rdy : in STD_LOGIC;
		io_in : in STD_LOGIC_VECTOR(15 downto 0);
		
		-- parallelism stuff
		primary : in STD_LOGIC;
		par_xPC, par_yPC : inout STD_LOGIC_VECTOR(4 downto 0);
		par_ARG, par_PO, par_PE : inout STD_LOGIC_VECTOR(31 downto 0);
		rqst, acpt, redy : in STD_LOGIC;
		fork, ftch, free : out STD_LOGIC
		);
end entity;

architecture default of Core2DWPU is
	signal active_status, not_active_status, activated, finished, block_return : STD_LOGIC;
	signal fork_intern : STD_LOGIC;
	
	signal IFWreset, IFWsecondary_reset : STD_LOGIC;

	signal opcode : STD_LOGIC_VECTOR(15 downto 0);
	signal instr_addr : STD_LOGIC_VECTOR(12 downto 0);
	signal instr_flow, instr_return : STD_LOGIC;
	signal IScurrent, ISnew, ISpush : STD_LOGIC_VECTOR(2 downto 0);
	signal IS_wr, ISzero : STD_LOGIC;
	
	signal IFWempty : STD_LOGIC;
	
	signal op_par_operand : STD_LOGIC;
	signal op : operation2DWPU;
	
	signal operandLoad, operandStore, operandInc, operandDec,
		operandReady : STD_LOGIC;
		
	signal operandAddr_offset : STD_LOGIC_VECTOR(8 downto 0);
	signal operandSelectReg : register2DWPU;
	signal operandSelectMem : dataLocation;
	signal wr_block : STD_LOGIC;
	
	signal index : STD_LOGIC_VECTOR(9 downto 0);
	signal newvalue, opresult, index_val : STD_LOGIC_VECTOR(31 downto 0);
	signal ARG, ARGtop, ARGnew, ARGpar, PO, PE, operandA, operandB : STD_LOGIC_VECTOR(31 downto 0);
	signal ARG_wr, ARG_wr_top, ARG_wr_newval, ARG_wr_opresult, PO_wr, PE_wr : STD_LOGIC;
	
	signal argStackPush, argStackPop, poptoARG : STD_LOGIC;
	signal ifwStackPush, ifwStackPop : STD_LOGIC;
	
	signal PDF, privateExtern_wr : STD_LOGIC;
	
	signal flowDir, newReturnDir, returnDir, secondDir : direction;
	signal flowLength, returnLength, secondLength : dirLength;
	
	signal second_xPC, second_yPC : STD_LOGIC_VECTOR(4 downto 0);
	
begin
	fork <= fork_intern;
	active_status <= '1' when primary = '1' else activated;
	not_active_status <= not active_status;
	free <= not active_status;
	
	IFWreset <= reset or IFWsecondary_reset;
	
	par_xPC <= second_xPC when fork_intern = '1' else (others => 'Z');
	par_yPC <= second_yPC when fork_intern = '1' else (others => 'Z');
	
	par_ARG <= ARG when (finished or fork_intern) = '1' else (others => 'Z');
	par_PO <= PO when fork_intern = '1' else (others => 'Z');
	par_PE <= PE when fork_intern = '1' else (others => 'Z');

	newval_out <= newvalue;

	controller : entity work.InstrDecoder(default)
	port map ( clk, reset, block_return, finished, opcode, IScurrent, ISpush, ISzero,
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
	
	op, ARG_wr_opresult,
	
	active_status, PDF, rqst, fork_intern, redy, ftch, secondDir, secondLength,
	op_par_operand);
	
	PC : entity work.ProgramCounter(default)
	port map ( clk, reset, flowDir, returnDir, flowLength, returnLength,
		instr_flow, instr_return, instr_addr,
		secondDir, secondLength,
		second_xPC, second_yPC, par_xPC, par_yPC, not_active_status);
	
	ProgramMem : entity work.ProgramMem(default)
	port map ( clk, instr_addr, index, opcode, index_val );
	
	operandA <= ARGtop when op_par_operand = '0' else ARG;
	operandB <= ARG when op_par_operand = '0' else ARGpar;
	
	ALU : entity work.Operation(default)
	port map ( clk, op, operandA, operandB, opresult, '0' );
	
	OperandMove : entity work.OperandInOut(default)
	port map ( clk, reset, operandLoad, operandStore, operandInc, operandDec,
		wr_block, operandReady,
		ARG, LI, SQ, RE, TR, CI, PE, HE, ST, EL, PO, index_val,
		ARG_wr_newval, LI_wr, SQ_wr, RE_wr, TR_wr, CI_wr, PE_wr, HE_wr, ST_wr, EL_wr, PO_wr,
		newvalue, operandAddr_offset, operandSelectReg, operandSelectMem,
		mem_addr, mem_wr, mem_rd, mem_rdy, mem_in,
		io_addr, io_wr, io_rd, io_rdy, io_in);	
	
	privateExtern_wr <= rqst;
	privateRegs : entity work.PrivateWorkRegisters(default)
	port map ( clk, reset, newvalue, PO_wr, PE_wr, PO, PE, par_PO, par_PE, privateExtern_wr);
	
	ARGreg : entity work.GenReg32(default)
		port map (clk, ARG_wr, reset, ARGnew, ARG);
		
	ISregister : entity work.ISreg(default)
		port map (clk, reset, IS_wr, ISzero, ISnew, IScurrent);
	
	ARGdemux : entity work.RegWriteDemux(default)
	port map ( ARG_wr_top, ARG_wr_newval, ARG_wr_opresult, '0', '0', '0', '0', '0', 
		ARGtop, newvalue, opresult, X"00000000", X"00000000", X"00000000", X"00000000", X"00000000",
		ARG_wr, ARGnew);
	
	IFWstack : entity work.IFW_Stack(default)
	port map ( clk, IFWreset, ifwStackPush, ifwStackPop, 
		ISpush, acpt, flowLength, newReturnDir,
		ISnew, PDF, returnLength, returnDir, IS_wr, IFWempty);
		
	ARGstack : entity work.ArgumentStack(default)
	port map ( clk, reset, ARG, argStackPush, argStackPop, poptoARG, ARGtop, ARG_wr_top);
	
	process is
	begin
		wait until clk = '1';
		
		-- monostable signals
		IFWsecondary_reset <= '0';
		
		if reset = '1' then
			activated <= '0';
			block_return <= '0';
		else
			if redy = '1' then
				ARGpar <= par_ARG;
			end if;
		
			if primary = '0' then
				if (activated = '0') and (rqst = '1') then
					activated <= '1';
				
				elsif (instr_return = '1') and (IFWempty = '1') then
					block_return <= '1';
				elsif finished = '1' then
					activated <= '0';
					IFWsecondary_reset <= '1';
					block_return <= '0';
				end if;
			end if;
		end if;
	end process;
	
end architecture;