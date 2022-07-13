library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity IOregister is
	port(
		clk, reset, wr, rd : in STD_LOGIC;
		dev_address, addr : in STD_LOGIC_VECTOR(15 downto 0);		
		
		write_val : in STD_LOGIC_VECTOR(15 downto 0);
		read_bus : out STD_LOGIC_VECTOR(15 downto 0);
		
		regval : out STD_LOGIC_VECTOR(15 downto 0)
	);
end entity;

architecture default of IOregister is
	signal wr_out, rd_out : STD_LOGIC;
	signal read_val : STD_LOGIC_VECTOR(15 downto 0);
begin

	regval <= read_val;

	IOwrapper : entity work.IOdeviceWrapper(default)
	port map( dev_address, addr, wr, rd, wr_out, rd_out, read_val, read_bus );
	
	Reg : entity work.GenReg16
	port map( clk, wr_out, reset, write_val, read_val);

end architecture;