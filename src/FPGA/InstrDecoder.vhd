use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity InstrDecoder is
	port ( clk, reset, block_return : in STD_LOGIC;
		finished : out STD_LOGIC;
		
		opcode : in STD_LOGIC_VECTOR(15 downto 0);
		ISin : in STD_LOGIC_VECTOR(2 downto 0);
		ISnew :  out STD_LOGIC_VECTOR(2 downto 0);
		ISzero : out STD_LOGIC;
		
		-- control signals
		operandLoad, operandStore, operandInc, operandDec : out STD_LOGIC;
		operandAddr_offset : out STD_LOGIC_VECTOR(8 downto 0);
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
		ARG_wr_opresult : out STD_LOGIC;
		
		activated : in STD_LOGIC;
		
		PDF : in STD_LOGIC;
		rqst : in STD_LOGIC;
		fork : out STD_LOGIC;
		redy : in STD_LOGIC;
		ftch : out STD_LOGIC;
		
		secondDir : out direction;
		secondLength : out dirLength;
		op_par_operand : out STD_LOGIC
		
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
			when others => return len1; -- make ModelSim happy
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
	
	function decodeOperation( code : STD_LOGIC_VECTOR(5 downto 0))
	return operation2DWPU is
	begin
		return operation2DWPU'val(to_integer(unsigned(code) + 2));
	end;
	
	function decodeReg( code : STD_LOGIC_VECTOR)
	return register2DWPU is
	begin
		if signed(code) < 0 then
			return regNONE;
		else
		case to_integer(unsigned(code) mod 25) is
			when 0  => return regLI;
			when 1  => return regSQ;
			when 2  => return regRE;
			when 3  => return regTR;
			when 4  => return regCI;
			when 5  => return regPE;
			when 6  => return regHE;
			when 7  => return regST;
			when 8  => return regEL;
			when 9  => return regPO;
			when 10 => return regIC;
			when 11 => return regIB;
			when 12 => return regSW;
			when 13 => return regBS;
			when 14 => return regSI;
			when 15 => return regSA;
			when 16 => return regTC;
			when 17 => return regTV0;
			when 18 => return regTV1;
			when 19 => return regTV2;
			when 20 => return regTV3;
			when 21 => return regTF0;
			when 22 => return regTF1;
			when 23 => return regTF2;
			when 24 => return regTF3;
			when others => return regNONE;
		end case;
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
		variable zeroTest, currentPDF : STD_LOGIC;
		variable currentIS : STD_LOGIC_VECTOR(2 downto 0);
		variable actionTime : instrActionTime;
		variable waitForReady : STD_LOGIC;
		variable waitCycle : STD_LOGIC;
	begin
		wait until clk = '1';
		
		-- monostable signals
		fork <= '0';
		
		--rqst <= '0' when activated = '1' else 'Z';
		
		ISzero <= '0';
		operandLoad <= '0';
		operandStore <= '0';
		operandInc <= '0';
		operandDec <= '0';
		flowPC <= '0';
		returnPC <= '0';
		argStackPush <= '0';
		argStackPop <= '0';
		poptoARG <= '0';
		ifwStackPush <= '0';
		ifwStackPop <= '0';
		ARG_wr_opresult <= '0';
		finished <= '0';
		
		if (reset = '1') or (activated = '0') then
			phase := 0;
			
			ISnew <= "000";
			ISzero <= '0';
			operandLoad <= '0';
			operandStore <= '0';
			operandInc <= '0';
			operandDec <= '0';
			operandAddr_offset <= "000000000";
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
			
			waitCycle := '0';
			
			wr_block <= '0'; -- NOT MONOSTABLE!
			ftch <= '0';
			op_par_operand <= '0';
			
		elsif block_return = '1' and phase = 0 then
			finished <= '1';
		elsif phase = 0 and waitCycle = '1' then
			waitCycle := '0';
			-- Reset some signals
			ftch <= '0';
			op_par_operand <= '0';
		elsif phase = 0 then
			-- phase 0 - Decode the Instruction
			
			waitCycle := '1';
			
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
							ISzero <= '1';
						else
							returnPC <= '1';
							ifwStackPop <= '1';
						end if;
						
					else
						instr := instrConditionQuery;
						currentIS := ISin;
						
						if ((unsigned(opcode(5 downto 0)) >= 2) and (unsigned(opcode(5 downto 0)) <= 15)) or
							((unsigned(opcode(5 downto 0)) >= 30) and (unsigned(opcode(5 downto 0)) <= 41)) then
							zeroTest := '0';
						else
							zeroTest := '1';
						end if;
						
						if ISin = "000" then
							-- decode which value should OperandInOut load
							operandAddr_offset <= "000000000";
							operandLoad <= '1';
							wr_block <= '1';
							phase := 1;
							
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
							phase := 2;
						end if;						
						
					end if;
					
				when "01" =>
					-- It's operation all right
					instr := instrOperation;
					currentIS := ISin;
					phase := 1;
					currentPDF := PDF;
					
					updateLength <= pickLength( opcode(8 downto 7), opcode(6), ISin(0) );
					decodedUpdateDir <= decodeDir(opcode(13 downto 9), ISin(0));
					
					secondDir <= decodeDir(opcode(13 downto 9), '1');
					secondLength <= pickLength( opcode(8 downto 7), opcode(6), '1');
					
					case ISin is
						when "000" =>
							if isSingleDir(opcode(13 downto 9)) or ISin = "001" then
								ISnew <= "010";
								argStackPush <= '1';
							else
								ISnew <= "001";
								fork <= '1';								
							end if; 
							ifwStackPush <= '1';
							flowPC <= '1';
							ISzero <= '1';
						when "001" =>
							ISnew <= "010";
							ISzero <= '1';
							argStackPush <= '1';
							ifwStackPush <= '1';
							flowPC <= '1';
						when "010" =>
							-- perform the calculation
							if PDF = '0' then
								argStackPop <= '1';
							else
								ftch <= '1';
								op_par_operand <= '1';
							end if;
							ifwStackPop <= '1';
							returnPC <= '1';
							op <= decodeOperation(opcode(5 downto 0));							
						when others =>
						-- nothing, shouldn't ever happen
					end case;
					
				when "10" =>
					-- it's action!
					instr := instrAction;
					currentIS := ISin;
					phase := 1;
					
					case opcode(13 downto 12) is
						when "00" =>
							actionTime := timeEntry;
						when "01" =>
							actionTime := timeReturn;
						when "10" =>
							actionTime := timeBoth;
						when "11" =>
							actionTime := timeReturn;
						when others => -- make ModelSim Happy
					end case;
					
					-- do generic stuff
					if (ISin = "000") and (opcode(13 downto 12) /= "11") then
						-- query
						flowPC <= '1';
						ifwStackPush <= '1';
						ISzero <= '1';
						ISnew <= "001";
						decodedUpdateDir <= decodeDir("00" & opcode(9 downto 7), '0');
						updateLength <= decodeLength(opcode(11 downto 10));
					else
						returnPC <= '1';
						ifwStackPop <= '1';
					end if;
					
					operandAddr_offset <= "000000000";
					-- if the action has to happen now, happen now
					if (opcode(13) = '1') or ((opcode(12) xnor ISin(0)) = '1') then
						if unsigned(opcode(6 downto 0)) < 70 then
							-- It's MOV
							waitForReady := '1';
							-- Check if it's MOVT or MOVF
							if (unsigned(opcode(6 downto 0)) <= 24) or
								((unsigned(opcode(6 downto 0)) >= 50) and (opcode(0) = '0')) then
								-- MOVF
								operandLoad <= '1';
							else
								operandStore <= '1';
							end if;
							
							-- Decode the register and memory
							if unsigned(opcode(6 downto 0)) < 50 then
								operandSelectMem <= locReg;
								operandSelectReg <= decodeReg(opcode(6 downto 0));
							elsif opcode(6 downto 1) = "011001" then
								operandSelectMem <= locMemST;
								operandSelectReg <= regNONE;
							elsif opcode(6 downto 1) = "011010" then
								-- Restrict to 16 bits!
								operandSelectMem <= locMemST;
								operandSelectReg <= regNONE;
							elsif opcode(6 downto 1) = "011011" then
								-- Restrict to 8 bits!
								operandSelectMem <= locMemST;
								operandSelectReg <= regNONE;
							elsif opcode(6 downto 1) = "011100" then
								operandSelectMem <= locIoHE;
								operandSelectReg <= regNONE;
							elsif opcode(6 downto 1) = "011101" then
								-- Restrict to 8 bits!
								operandSelectMem <= locIoHE;
								operandSelectReg <= regNONE;
							-- Version with +PO
							elsif opcode(6 downto 1) = "011110" then
								operandSelectMem <= locMemST;
								operandSelectReg <= regPO;
							elsif opcode(6 downto 1) = "011111" then
								-- Restrict to 16 bits!
								operandSelectMem <= locMemST;
								operandSelectReg <= regPO;
							elsif opcode(6 downto 1) = "100000" then
								-- Restrict to 8 bits!
								operandSelectMem <= locMemST;
								operandSelectReg <= regPO;
							elsif opcode(6 downto 1) = "100001" then
								operandSelectMem <= locIoHE;
								operandSelectReg <= regPO;
							elsif opcode(6 downto 1) = "100010" then
								-- Restrict to 8 bits!
								operandSelectMem <= locIoHE;
								operandSelectReg <= regPO;
							end if;
							
						elsif unsigned(opcode(6 downto 0)) < 82 then
							-- PUSH not implemented yet (need to resolve some trouble!)
						elsif unsigned(opcode(6 downto 0)) < 96 then
							-- POP not implemented yet (need to resolve some trouble!)
						elsif unsigned(opcode(6 downto 0)) < 110 then
							waitForReady := '0';
							if unsigned(opcode(6 downto 0)) < 103 then
								operandInc <= '1';
							else
								operandDec <= '1';
							end if;
							
							case to_integer((unsigned(opcode(6 downto 0)) - 96) mod 7) is
								when 0 => operandSelectReg <= regARG;
								when 1 => operandSelectReg <= regLI;
								when 2 => operandSelectReg <= regSQ;
								when 3 => operandSelectReg <= regHE;
								when 4 => operandSelectReg <= regST;
								when 5 => operandSelectReg <= regPE;
								when 6 => operandSelectReg <= regPO;
								when others => -- nothing obviously
							end case;
							
						end if;
							
					else
						-- if it's not happening now there's nothing to wait for
						waitForReady := '0';
					end if;
				
				when "11" =>
					-- it's an index instruction
					case opcode(13 downto 12) is
						when "00" =>
							-- It's a RET/TAK!
							returnPC <= '1';
							ifwStackPop <= '1';
							instr := instrIndex;
							
							tableIndex <= opcode(9 downto 0);
							
							case opcode(11 downto 10) is
								when "00" =>
									-- Register
									phase := 2;
									if opcode(9) = '1' then
										operandStore <= '1';	-- TAK
									else
										operandLoad <= '1'; -- RET
									end if;
									
									if unsigned(opcode(8 downto 0)) < 25 then
										operandSelectReg <= decodeReg('0' & opcode(8 downto 0));
										operandSelectMem <= locReg;
									elsif unsigned(opcode(8 downto 0)) < 128 then
									
										operandSelectReg <= decodeReg( STD_LOGIC_VECTOR(
											(((unsigned(opcode(8 downto 0)) - 25)) mod 12) - 1 ));
											
										if (((unsigned(opcode(8 downto 0)) - 25) / 12) and "000000001") = "000000000" then
											operandSelectMem <= locMemST;
										else
											operandSelectMem <= locIoHE;
										end if;
										operandAddr_offset <= "000000000";
									else
										-- it's indirect addressing with offset
										operandSelectReg <= regNONE;
										operandSelectMem <= locMemST;
										operandAddr_offset <= STD_LOGIC_VECTOR(signed(opcode(8 downto 0)) - 256);
									end if;
								when "01" =>
									-- Value
									phase := 1;
									operandSelectReg <= regNONE;
									operandSelectMem <= locIndex;
								when "10" =>
									-- Memory
									phase := 1;
									operandSelectReg <= regNONE;
									operandSelectMem <= locMemIndex;
								when "11" =>
									-- IO
									phase := 1;
									operandSelectReg <= regNONE;
									operandSelectMem <= locIoIndex;
								when others =>
									-- Modelsim. Happy
							end case;
						when others =>
							-- Don't care about others yet, implement them later
					end case;
				when others =>
				  -- make model sim happy				
			end case;
		
		else
		
		case instr is
			when instrQRY =>
				if phase = 1 then
					phase := 2;
				else
					phase := 0;					
				end if;
				
			when instrConditionQuery =>
				if phase = 1 then
					if currentIS = "000" and operandReady = '1' then
						wr_block <= '0';
						phase := 2;
						
						updateLength <= decodeLength(opcode(8 downto 7));	
	
						if isSingleDir(opcode(13 downto 9)) then
							ISnew <= "000";
						else
							ISnew <= "001";
						end if;
	
						-- now test the conditional
						if (zeroTest xnor valIsZero) = '1' then
							decodedUpdateDir <= decodeDir(opcode(13 downto 9), '0');
							flowPC <= '1';
							ifwStackPush <= '1';
							ISzero <= '1';
						elsif not isSingleDir(opcode(13 downto 9)) then
							decodedUpdateDir <= decodeDir(opcode(13 downto 9), '1');
							flowPC <= '1';
							ifwStackPush <= '1';
							ISzero <= '1';
						else
							-- basically return
							returnPC <= '1';
							ifwStackPop <= '1';
						end if;						
						
					end if;
				elsif phase = 2 then
					phase := 3;
				else
					phase := 0;
				end if;
				
			when instrOperation =>
				if phase = 1 then
					-- basically wait
					if ((currentPDF xnor redy) = '1') and (currentIS = "010") then
						phase := 2;
					elsif currentIS /= "010" then
						phase := 2;
					end if;
				else
					phase := 0;
					-- write new result to the ARG
					if currentIS = "010" then
						ARG_wr_opresult <= '1';
					end if;
				end if;
				
			when instrAction =>
				if phase = 1 then
					if (operandReady xnor waitForReady) = '1' then
						phase := 3;
					else
						phase := 2;
					end if;
				elsif phase = 2 then
					if operandReady = '1' then
						phase := 0;
					end if;
				else
					phase := 0;
				end if;
				
			when instrIndex =>
				if phase = 1 then
					operandLoad <= '1';
					phase := 2;
					
					if operandReady = '1' then
						phase := 3;
					end if;
				elsif phase = 2 then
					if operandReady = '1' then
						phase := 3;
					end if;
				else
					phase := 0;
				end if;
			when others =>
			-- do nothing
		end case;
		
		end if;
		
		
	end process;
end architecture;