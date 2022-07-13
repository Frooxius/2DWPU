use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity ProgramMem is
	port( clk : in STD_LOGIC;
	-- doesn't currently use BS, assumes BS = 0
		addr : in STD_LOGIC_VECTOR(12 downto 0); -- 8 kB
		index : in STD_LOGIC_VECTOR(9 downto 0);
		
		opcode : out STD_LOGIC_VECTOR(15 downto 0);
		tableval : out STD_LOGIC_VECTOR(31 downto 0)
	
		);
end entity;

architecture default of ProgramMem is
	signal index_addr : STD_LOGIC_VECTOR(12 downto 0);
begin

	programmem : entity work.ProgramROM(SYN)
	port map ( addr(12 downto 1), index_addr(12 downto 2),
		clk, opcode, tableval);
		
	index_addr <= std_logic_vector( 64 + 2048 + unsigned('0' & index & B"00") );

end architecture;