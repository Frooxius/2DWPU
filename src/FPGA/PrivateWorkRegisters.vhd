use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity PrivateWorkRegisters is
	port( clk, reset : in STD_LOGIC;
		newvalue : in STD_LOGIC_VECTOR(31 downto 0);
		
		PO_wr, PE_wr : in STD_LOGIC;
		
		PO, PE : out STD_LOGIC_VECTOR(31 downto 0);
		
		externPO, externPE : in STD_LOGIC_VECTOR(31 downto 0);
		
		externWR : in STD_LOGIC);
end entity;

architecture default of PrivateWorkRegisters is
	signal PO_real_wr, PE_real_wr : STD_LOGIC;
	signal PO_real, PE_real : STD_LOGIC_VECTOR(31 downto 0);
begin
	PO_real <= newvalue when externWR = '0' else externPO;
	PE_real <= newvalue when externWR = '0' else externPE;
	PO_real_wr <= PO_wr or externWR;
	PE_real_wr <= PE_wr or externWR;

	POreg : entity work.GenReg32(default)
		port map (clk, PO_real_wr, reset, PO_real, PO);
		
	PEreg : entity work.GenReg32(default)
		port map (clk, PE_real_wr, reset, PE_real, PE);
end architecture;
