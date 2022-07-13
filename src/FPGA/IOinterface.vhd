use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity IOinterface is
	port( clk, wr, rd : in STD_LOGIC;
		rdy : out STD_LOGIC;

		in_addr, in_wr_val, in_rd_bus : in STD_LOGIC_VECTOR(15 downto 0);
		
		out_addr, out_wr_val, out_rd_val : out STD_LOGIC_VECTOR(15 downto 0);
		wr_out, rd_out : out STD_LOGIC
		);
end entity;

architecture default of IOinterface is
begin
	wr_out <= wr;
	rd_out <= rd;
	
	out_addr <= in_addr;
	out_wr_val  <= in_wr_val;
	
	process is 
	begin
		wait until clk = '1';
		if wr = '1' or rd = '1' then
			rdy <= '1';
		else
			rdy <= '0';
		end if;
		
		if rd = '1' then
			out_rd_val <= in_rd_bus;
		end if;
	end process;
	
end architecture;