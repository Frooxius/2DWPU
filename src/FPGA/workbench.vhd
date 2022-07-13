use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity workbench is
	port ( clk : in STD_LOGIC; in_val, in_val2 : in STD_LOGIC_VECTOR(4 downto 0); out_val : out STD_LOGIC_VECTOR(4 downto 0));
end entity;

architecture default of workbench is

	
begin

	out_val <= STD_LOGIC_VECTOR(unsigned(in_val) / unsigned(in_val2));

end architecture;