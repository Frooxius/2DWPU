use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity InstrDecoder is
	port ( clk, reset : in STD_LOGIC;
		opcode : in STD_LOGIC_VECTOR(15 downto 0);
		ISin : in STD_LOGIC_VECTOR(2 downto 0);
		ISnew :  out STD_LOGIC_VECTOR(2 downto 0);
		ISzero : out STD_LOGIC;
		
		-- control signals
		operandLoad, operandStore, operandInc, operandDec : out STD_LOGIC;
		operandAddr_offset : out STD_LOGIC_VECTOR(7 downto 0);
		operandSelectReg : out register2DWPU;
		operandSelectMem : out dataLocation;
		operandReady : in STD_LOGIC;
		wr_block : out STD_LOGIC;
		operandIn : in STD_LOGIC_VECTOR(31 downto 0);
		
		tableIndex : out STD_LOGIC_VECTOR(9 downto 0);
		
		updateDir, newReturnDir : out direction;
		currentReturnDir : in direction;
		updateLength : out dirLength;
		flowPC, returnPC : out STD_LOGIC;
		
		argStackPush, argStackPop, poptoARG : out STD_LOGIC;
		ifwStackPush, ifwStackPop : out STD_LOGIC;
		
		op : out operation2DWPU;
		ARG_wr_opresult : out STD_LOGIC
		
		);
end entity;

architecture default of InstrDecoder is

	signal decodedUpdateDir, resolvedUpdateDir : direction;
	signal valIsZero : STD_LOGIC;

	function decodeDir (code : STD_LOGIC_VECTOR(4 downto 0); second : STD_LOGIC)
	return direction is
	begin
		if second = '0' then
			return direction'val( to_integer(unsigned(code) mod 5) );
		else
			return direction'val( to_integer(unsigned(code) / 5) );
		end if;
	end;
	
	function decodeLength ( code : STD_LOGIC_VECTOR(1 downto 0) )
	return dirLength is
	begin
		case code is
			when "00" => return len1;
			when "01" => return len2;
			when "10" => return len3;
			when "11" => return len4;
		end case;
	end;
	
	function pickLength ( code : STD_LOGIC_VECTOR(1 downto 0); LD, isfirst : STD_LOGIC)
	return dirLength is
	begin
		if (LD xnor isfirst) = '1' then
			return decodeLength(code);
		else
			return len1;
		end if;
	end;
	
	function isSingleDir (code : STD_LOGIC_VECTOR(4 downto 0))
	return boolean is
	begin
		if ( (code(4) and code(3)) and (code(0) or code(1) or code(2)) ) = '1' then
			return true;
		else
			return false;
		end if;
	end;

begin

	valIsZero <= '1' when operandIn = X"00000000" else '0';

	resolvedUpdateDir <= 
		decodedUpdateDir when decodedUpdateDir /= dirC else
		dirU when currentReturnDir = dirD else
		dirR when currentReturnDir = dirL else
		dirD when currentReturnDir = dirU else
		dirL;		
		
	updateDir <= resolvedUpdateDir;

	with resolvedUpdateDir select
	newReturnDir <=
		dirU when dirD,
		dirR when dirL,
		dirD when dirU,
		dirL when dirR,
		dirU when others;

	process is
		variable phase : int4 := 0;
		variable instr : instrType;
		variable zeroTest : STD_LOGIC;
		variable currentIS : STD_LOGIC_VECTOR(2 downto 0);
	begin
		wait until clk = '1';
		
		if reset = '1' then
			phase := 0;
			
			ISnew <= "000";
			ISzero <= '0';
			operandLoad <= '0';
			operandStore <= '0';
			operandInc <= '0';
			operandDec <= '0';
			operandAddr_offset <= X"00";
			operandSelectReg <= regNONE;
			operandSelectMem <= locReg;
			
			tableIndex <= "0000000000";
			flowPC <= '0';
			returnPC <= '0';
			argStackPush <= '0';
			argStackPop <= '0';
			poptoARG <= '0';
			ifwStackPush <= '0';
			ifwStackPop <= '0';
			op <= opNONE;
			ARG_wr_opresult <= '0';
	
		elsif phase = 0 then
			-- phase 0 - Decode the Instruction
			
			-- decode and store the base type
			case opcode(15 downto 14) is
				when "00" =>
					-- determine if it's QRY or conditional Query
					if opcode(5 downto 0) = "000000" then
						instr := instrQRY;
						currentIS := ISin;
						phase := 1;
						
						if isSingleDir(opcode(13 downto 9)) or ISin = "001" then
							ISnew <= "010";
						else
							ISnew <= "001";
						end if;
						
						updateLength <= pickLength( opcode(8 downto 7), opcode(6), ISin(0) );
						decodedUpdateDir <= decodeDir(opcode(13 downto 9), ISin(0));
						
						if ISin /= "010" then
							flowPC <= '1';
							ifwStackPush <= '1';
						else
							returnPC <= '1';
							ifwStackPop <= '1';
						end if;
						
					else
						instr := instrConditionQuery;
						phase := 1;
						currentIS := ISin;
						
						if (unsigned(opcode(5 downto 0)) >= 2 and unsigned(opcode(5 downto 0)) <= 15) or
							(unsigned(opcode(5 downto 0)) >= 30 and unsigned(opcode(5 downto 0)) <= 41) then
							zeroTest := '0';
						else
							zeroTest := '1';
						end if;
						
						if ISin = "000" then
							-- decode which value should OperandInOut load
							operandAddr_offset <= X"00";
							operandLoad <= '1';
							wr_block <= '1';
							
							case opcode(5 downto 0) is
								-- ARG
								when "000010" | "010000" =>
									operandSelectReg <= regARG;
									operandSelectMem <= locReg;
								-- *ST
								when "000011" | "010001" =>
									operandSelectReg <= regNONE;
									operandSelectMem <= locMemST;
								-- *HE
								when "000100" | "010010" =>
									operandSelectReg <= regNONE;
									operandSelectMem <= locIoHE;
								-- *(ST+PO)
								when "000101" | "010011" =>
									operandSelectReg <= regPO;
									operandSelectMem <= locMemST;
								-- LI
								when "000110" | "010100" =>
									operandSelectReg <= regLI;
									operandSelectMem <= locReg;
								-- SQ
								when "000111" | "010101" =>
									operandSelectReg <= regSQ;
									operandSelectMem <= locReg;
								-- RE
								when "001000" | "010110" =>
									operandSelectReg <= regLI;
									operandSelectMem <= locReg;
								-- TR
								when "001001" | "010111" =>
									operandSelectReg <= regTR;
									operandSelectMem <= locReg;
								-- CI
								when "001010" | "011000" =>
									operandSelectReg <= regCI;
									operandSelectMem <= locReg;
								-- PE
								when "001011" | "011001" =>
									operandSelectReg <= regPE;
									operandSelectMem <= locReg;
								-- HE
								when "001100" | "011010" =>
									operandSelectReg <= regHE;
									operandSelectMem <= locReg;
								-- PO
								when "001101" | "011011" =>
									operandSelectReg <= regPO;
									operandSelectMem <= locReg;
								when others =>
									operandSelectReg <= regNONE;
									operandSelectMem <= locReg;
							end case;
							
						else
							-- basically return
							ifwStackPop <= '1';
							returnPC <= '1';						
						end if;						
						
					end if;
					
				when "01" =>
				
				when "10" =>
				
				when "11" =>
				
			end case;
		
		else
		
		case instr is
			when instrQRY =>
				if phase = 1 then
					flowPC <= '0';
					returnPC <= '0';
					ifwStackPop <= '0';
					ifwStackPush <= '0';
					phase := 2;
				else
					phase := 0;					
				end if;
				
			when instrConditionQuery =>
				if phase = 1 then
					if currentIS = "000" and operandReady = '1' then
						wr_block <= '0';
						phase := 2;
						ISnew <= "001";
						
						updateLength <= decodeLength(opcode(8 downto 7));						
						-- now test the conditional
						if (zeroTest xnor valIsZero) = '1' then
							decodedUpdateDir <= decodeDir(opcode(13 downto 9), '0');
							flowPC <= '1';
							ifwStackPush <= '1';
						elsif isSingleDir(opcode(13 downto 9)) then
							decodedUpdateDir <= decodeDir(opcode(13 downto 9), '1');
							flowPC <= '1';
							ifwStackPush <= '1';
						else
							-- basically return
							returnPC <= '1';
							ifwStackPop <= '1';
						end if;
						
						
					end if;
				else
					phase := 0;
					flowPC <= '0';
					returnPC <= '0';
					ifwStackPop <= '0';
					ifwStackPush <= '0';
				end if;
				
				when others =>
				-- do nothing
		end case;
		
		end if;
		
		
	end process;
end architecture;