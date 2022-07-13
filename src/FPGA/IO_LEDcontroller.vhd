use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity IO_LEDcontroller is
	port( clk : in STD_LOGIC;
		
		wrval : in STD_LOGIC_VECTOR(15 downto 0);
		wrlow, wrhigh : in STD_LOGIC;
		
		LEDG : out STD_LOGIC_VECTOR(8 downto 0);
		LEDR : out STD_LOGIC_VECTOR(17 downto 0)
		
		);
end entity;

architecture default of IO_LEDcontroller is
	signal store_val : STD_LOGIC_VECTOR(31 downto 0);
begin

	LEDG(7 downto 0) <= store_val(7 downto 0);
	LEDR <= store_val(24 downto 8);
	LEDG(8) <= store_val(25);
	
	process is
	begin
		wait until clk = '1';
		
		if wrlow = '1' then
			store_val(15 downto 0) <= wrval;
		end if;
		
		if wrhigh = '1' then
			store_val(31 downto 16) <= wrval;
		end if;		
		
	end process;

end architecture;