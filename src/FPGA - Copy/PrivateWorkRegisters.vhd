use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity PrivateWorkRegisters is
	port( clk, reset : in STD_LOGIC;
		newvalue : in STD_LOGIC_VECTOR(31 downto 0);
		
		PO_wr, PE_wr : in STD_LOGIC;
		
		PO, PE : out STD_LOGIC_VECTOR(31 downto 0));
end entity;

architecture default of PrivateWorkRegisters is
begin
	POreg : entity work.GenReg32(default)
		port map (clk, PO_wr, reset, newvalue, PO);
		
	PEreg : entity work.GenReg32(default)
		port map (clk, PE_wr, reset, newvalue, PE);
end architecture;