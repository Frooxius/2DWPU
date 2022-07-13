use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity Operation is 
	port(
		clk : in STD_LOGIC;
		op : in operation2DWPU;
		
		operandA, operandB : in STD_LOGIC_VECTOR(31 downto 0);
		result : out STD_LOGIC_VECTOR(31 downto 0);
		
		CB : in STD_LOGIC
	);
end entity;

architecture default of Operation is
	
	function signmult(opA, opB : signed(31 downto 0)) return STD_LOGIC_VECTOR is
		variable result : STD_LOGIC_VECTOR(63 downto 0);
	begin
		result := STD_LOGIC_VECTOR(opA * opB);
		return result(31 downto 0);
	end;
	
begin
	process is
	begin
		wait until clk = '1';
		case op is
			when opIDENTITY =>
				result <= operandA;
			when opALU_ADD =>
				result <= STD_LOGIC_VECTOR(unsigned(operandA) + unsigned(operandB));
			when opALU_ADC =>
				if CB = '0' then
					result <= STD_LOGIC_VECTOR(unsigned(operandA) + unsigned(operandB));
				else
					result <= STD_LOGIC_VECTOR(unsigned(operandA) + unsigned(operandB) + 1);
				end if;
			when opALU_SUB =>
				result <= STD_LOGIC_VECTOR(unsigned(operandA) - unsigned(operandB));
			when opALU_SBB =>
				if CB = '0' then
					result <= STD_LOGIC_VECTOR(unsigned(operandA) - unsigned(operandB));
				else
					result <= STD_LOGIC_VECTOR(unsigned(operandA) - unsigned(operandB) - 1);
				end if;
			when opALU_MUL =>
				result <= signmult( signed(operandA), signed(operandB));
			when opALU_MULH =>
				result <= X"00000000"; -- Not implemented yet
			when opALU_DIV =>
				result <= STD_LOGIC_VECTOR(unsigned(operandA) / unsigned(operandB));
			when opALU_MOD =>
				result <= STD_LOGIC_VECTOR(signed(operandA) mod signed(operandB));
			when opALU_AND =>
				result <= operandA and operandB;
			when opALU_OR =>
				result <= operandA or operandB;
			when opALU_XOR =>
				result <= operandA xor operandB;
			when opALU_NOT =>
				result <= not operandA;
			when opALU_LAND =>
				result <= X"00000000"; -- Not implemented yet
			when opALU_LOR =>
				result <= X"00000000"; -- Not implemented yet
			when opALU_LXOR =>
				result <= X"00000000"; -- Not implemented yet
			when opALU_LNOT =>
					if operandA = X"00000000" then
						result <= X"00000000";
					else
						result <= X"00000001";
					end if;
			when opALU_RL =>
					result <= STD_LOGIC_VECTOR( unsigned(operandA) rol to_integer(signed(operandB)) );
			when opALU_RR =>
					result <= STD_LOGIC_VECTOR( unsigned(operandA) ror to_integer(signed(operandB)) );
			when opALU_SL =>
					result <= STD_LOGIC_VECTOR( unsigned(operandA) sll to_integer(signed(operandB)) );
			when opALU_SR =>
					result <= STD_LOGIC_VECTOR( unsigned(operandA) srl to_integer(signed(operandB)) );
			when opALU_UMIN =>
					if unsigned(operandA) > unsigned(operandB) then
						result <= operandB;
					else
						result <= operandA;
					end if;
			when opALU_UMAX =>
					if unsigned(operandA) > unsigned(operandB) then
						result <= operandA;
					else
						result <= operandB;
					end if;
			when opALU_SMIN =>
					if signed(operandA) > signed(operandB) then
						result <= operandB;
					else
						result <= operandA;
					end if;
			when opALU_SMAX =>
					if signed(operandA) > signed(operandB) then
						result <= operandA;
					else
						result <= operandB;
					end if;
			when opALU_SDIV =>
					result <= STD_LOGIC_VECTOR(signed(operandA) / signed(operandB));
			when others =>
					result <= X"00000000";
			end case;
	end process;
end architecture;