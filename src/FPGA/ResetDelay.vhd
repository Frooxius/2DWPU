use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity ResetDelay is
	port ( clk, resetagain : in STD_LOGIC; reset : OUT STD_LOGIC );
end entity;

architecture default of ResetDelay is
begin
	process is
		variable counter : STD_LOGIC_VECTOR(3 downto 0) := "1111";
	begin
		wait until clk = '1';
		
		if resetagain = '1' then
			reset <= '1';
			counter := "1111";
		elsif counter /= "0000" then
			reset <= '1';
			counter := STD_LOGIC_VECTOR(unsigned(counter) - 1);
		else
			reset <= '0';
		end if;
			
	end process;
end architecture;