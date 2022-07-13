use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity randomIOdev is
	port( clk : in STD_LOGIC;
		dev_addr, addr : in STD_LOGIC_VECTOR(15 downto 0);
		rd : in STD_LOGIC;
		
		read_bus : out STD_LOGIC_VECTOR(15 downto 0)		
		);
end entity;

architecture default of randomIOdev is
	signal read_val : STD_LOGIC_VECTOR(15 downto 0);
	
	signal val0, val1 : STD_LOGIC_VECTOR(15 downto 0);
	signal clk2 : STD_LOGIC;
begin
	
	randomDev0 : entity work.random
	port map( clk, val0 );
	
	randomDev1 : entity work.random
	port map( clk2, val1 );
	
	clk2 <= clk and rd;
	read_val <= val0 xor val1;
	
	wrapper : entity work.IOdeviceWrapper
	port map( dev_addr, addr, '0', rd, open, open,
		read_val, read_bus);
	
end architecture;