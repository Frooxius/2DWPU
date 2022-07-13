use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity ArgumentStack is
	port( clk, reset : in STD_LOGIC;
		ARG_in : in STD_LOGIC_VECTOR(31 downto 0);
		push, pop, poptoARG : in STD_LOGIC;
		
		ARG : out STD_LOGIC_VECTOR(31 downto 0);
		ARG_wr : out STD_LOGIC
		);
end entity;

architecture default of ArgumentStack is
	signal push_cmd, pop_cmd : bit := '0';

	signal addr : STD_LOGIC_VECTOR (7 DOWNTO 0);
	signal data : STD_LOGIC_VECTOR (31 DOWNTO 0);
	signal wren, rden : STD_LOGIC := '0';
	signal mout : STD_LOGIC_VECTOR (31 DOWNTO 0);

begin
	stackmem : entity work.ArgStack_RAM(SYN)
		port map (addr, clk, data, rden, wren, mout);

	process is
	begin
		wait until clk = '1';
		
		if reset = '1' then
			addr <= B"1111_1111";
			wren <= '0';
			rden <= '0';
			ARG <= X"00000000";
		elsif push_cmd = '0' and pop_cmd = '0' then
				wren <= '0';
				
			if push = '1' then 
			
				push_cmd <= '1';
				--addr <= std_logic_vector( to_unsigned( SA, 9 ) );
				--SA := SA + 1;
				addr <= std_logic_vector(unsigned(addr) + 1);
				ARG <= ARG_in;
				data <= ARG_in;
				
			elsif pop = '1' then
				pop_cmd <= '1';
				--addr <= std_logic_vector( to_unsigned( (SA - 1), 9 ) );
				--SA := SA - 1;
				ARG_wr <= poptoARG;
				addr <= std_logic_vector(unsigned(addr) - 1);
				rden <= '1';
			end if;
			
		elsif push_cmd = '1' then
			wren <= '1'; -- write the data
			push_cmd <= '0';
		else
			ARG_wr <= '0';
			if rden = '1' then
				rden <= '0';
			else
				ARG <= mout;	
				pop_cmd <= '0';
			end if;
		end if;
			
	end process;
end architecture;