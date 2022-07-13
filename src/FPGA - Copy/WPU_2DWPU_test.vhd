use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity WPU_2DWPU_Test is
	port( CLOCK_50 : in STD_LOGIC; KEY : in STD_LOGIC_VECTOR(3 downto 0); LEDR : out STD_LOGIC_VECTOR(17 downto 0) );
end entity;

architecture default of WPU_2DWPU_Test is
	signal clk, reset : STD_LOGIC;
	signal outval : STD_LOGIC_VECTOR(31 downto 0);
begin
	LEDR <= outval(17 downto 0);
	
	GlobReset : entity work.ResetDelay(default)
	port map ( clk, KEY(0), reset);

	PLLclock : entity work.PLL(SYN)
	port map( CLOCK_50, clk);
	
	Processor : entity work.WPU_2DWPU(default)
	port map( clk, reset, outval );
	
end architecture;