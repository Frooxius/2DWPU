-- The newvalue is currently shared, because I don't expect multiple cores to write into shared registers - only one can
use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity SharedWorkRegisters is
	port( clk, reset : in STD_LOGIC;
		newvalue : in STD_LOGIC_VECTOR(31 downto 0);
		
		LI, RE, SQ, CI, TR, HE, ST, EL : out STD_LOGIC_VECTOR(31 downto 0);
		LI_wr, RE_wr, SQ_wr, CI_wr, TR_wr, HE_wr, ST_wr, EL_wr : in STD_LOGIC);
end entity;

architecture default of SharedWorkRegisters is
begin
	LIreg : entity work.GenReg32(default)
		port map (clk, LI_wr, reset, newvalue, LI);
	REreg : entity work.GenReg32(default)
		port map (clk, RE_wr, reset, newvalue, RE);
	SQreg : entity work.GenReg32(default)
		port map (clk, SQ_wr, reset, newvalue, SQ);
	CIreg : entity work.GenReg32(default)
		port map (clk, CI_wr, reset, newvalue, CI);
	TRreg : entity work.GenReg32(default)
		port map (clk, TR_wr, reset, newvalue, TR);
	HEreg : entity work.GenReg32(default)
		port map (clk, HE_wr, reset, newvalue, HE);
	STreg : entity work.GenReg32(default)
		port map (clk, ST_wr, reset, newvalue, ST);
	ELreg : entity work.GenReg32(default)
		port map (clk, EL_wr, reset, newvalue, EL);		
end architecture;