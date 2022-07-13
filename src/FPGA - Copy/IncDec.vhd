use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity IncDec is
	port( inval : in STD_LOGIC_VECTOR(31 downto 0);
		mode : in STD_LOGIC;
		outval : out STD_LOGIC_VECTOR(31 downto 0));
end entity;

architecture default of IncDec is
begin
	outval <= STD_LOGIC_VECTOR(unsigned(inval) + 1) when mode = '0' else
		STD_LOGIC_VECTOR(unsigned(inval) - 1);
end architecture;