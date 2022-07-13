use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity AddrSelect is
	port ( wr_addr, rd_addr : in STD_LOGIC_VECTOR(23 downto 0);
		wr, rd : in STD_LOGIC;
		addr : out STD_LOGIC_VECTOR(31 downto 0));
end entity;

architecture default of AddrSelect is
begin
	addr <= wr_addr when wr = '1' else
		rd_addr when rd = '1' else X"000000";
end architecture;