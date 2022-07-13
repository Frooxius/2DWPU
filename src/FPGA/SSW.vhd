use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity SSW is
	port( clk, reset : in STD_LOGIC;
	
		UB0_S, UB1_S, UB2_S, UB3_S : in STD_LOGIC;
		UB0_R, UB1_R, UB2_R, UB3_R : in STD_LOGIC;
	
		UB0, UB1, UB2, UB3 : out STD_LOGIC);
end entity;

architecture default of SSW is
begin
	process is
		variable UB0_val, UB1_val, UB2_val, UB3_val : STD_LOGIC := '0';
	begin
		wait until clk = '1';
		
		if reset = '1' then
			UB0_val := '0';
			UB1_val := '0';
			UB2_val := '0';
			UB3_val := '0';
		else
		
			if UB0_S = '1' then UB0_val := '1'; end if;
			if UB1_S = '1' then UB1_val := '1'; end if;
			if UB2_S = '1' then UB2_val := '1'; end if;
			if UB3_S = '1' then UB3_val := '1'; end if;
		
			if UB0_R = '1' then UB0_val := '0'; end if;
			if UB1_R = '1' then UB1_val := '0'; end if;
			if UB2_R = '1' then UB2_val := '0'; end if;
			if UB3_R = '1' then UB3_val := '0'; end if;
		
			UB0 <= UB0_val;
			UB1 <= UB1_val;
			UB2 <= UB2_val;
			UB3 <= UB3_val;
		
		end if;
		
	end process;
end architecture;