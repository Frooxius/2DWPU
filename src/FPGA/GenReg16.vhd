use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity GenReg16 is
	port( clk, wr, reset : in STD_LOGIC;
		new_val : in STD_LOGIC_VECTOR(15 downto 0);
		out_val : out STD_LOGIC_VECTOR(15 downto 0));
end entity;

architecture default of GenReg16 is
begin
	process is
		--variable regval : integer := 0;
	begin
		wait until clk = '1';
		if reset = '1' then
			out_val <= X"0000";
		elsif wr = '1' then
			out_val <= new_val;
		end if;	
		
	end process;
end architecture;