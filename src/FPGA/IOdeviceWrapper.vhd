use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity IOdeviceWrapper is
	port( dev_address, addr : in STD_LOGIC_VECTOR(15 downto 0);
		wr, rd : in STD_LOGIC;
		wr_out, rd_out : out STD_LOGIC;
		
		read_val : in STD_LOGIC_VECTOR(15 downto 0);
		read_bus : out STD_LOGIC_VECTOR(15 downto 0)
		);
end entity;

architecture default of IOdeviceWrapper is
	signal selected : STD_LOGIC;
begin
	selected <= '1' when addr = dev_address else '0';
	
	wr_out <= wr and selected;
	rd_out <= rd and selected;
	
	process (rd, selected, read_val)
	begin
		if (rd and selected) = '1' then
			read_bus <= read_val;
		else
			read_bus <= (others => 'Z');
		end if;
	end process;
	
end architecture;

