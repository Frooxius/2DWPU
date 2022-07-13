use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity RegWriteDemux is
	port( wr0, wr1, wr2, wr3, wr4, wr5, wr6, wr7 : in STD_LOGIC;
		val0, val1, val2, val3, val4, val5, val6, val7 : in STD_LOGIC_VECTOR(31 downto 0);
		
		wr : out STD_LOGIC;
		wrval : out STD_LOGIC_VECTOR(31 downto 0));
end entity;

architecture default of RegWriteDemux is
begin
	wr <= '1' when wr0 = '1' or wr1 = '1' or wr2 = '1' or wr3 = '1' or wr4 = '1' or wr5 = '1' or wr6 = '1' or wr7 = '1' else '0';
	
	wrval <=
		val0 when wr0 = '1' else
		val1 when wr1 = '1' else
		val2 when wr2 = '1' else
		val3 when wr3 = '1' else
		val4 when wr4 = '1' else
		val5 when wr5 = '1' else
		val6 when wr6 = '1' else
		val7 when wr7 = '1' else
		X"00000000";
end architecture;