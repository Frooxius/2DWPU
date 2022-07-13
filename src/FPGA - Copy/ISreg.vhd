use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity ISreg is
	port( clk, reset, wr, zero : in STD_LOGIC;
		newval : in STD_LOGIC_VECTOR(2 downto 0);
		outval : out STD_LOGIC_VECTOR(2 downto 0));
end entity;

architecture default of ISreg is
begin
	process is
	begin
		wait until clk = '1';
		if reset = '1' or zero = '1' then
			outval <= "000";
		elsif wr = '1' then
			outval <= newval;
		end if;
	end process;
end architecture;