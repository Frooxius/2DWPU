use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity DisplayController is
	port ( clkwr, clkrd, reset : in STD_LOGIC;
		xpos_rd, xpos_wr, ypos_rd, ypos_wr : in STD_LOGIC_VECTOR(9 downto 0);
		wr, swap, double_on, double_off : in STD_LOGIC;
		
		Rin, Gin, Bin : in STD_LOGIC;
		Rout, Gout, Bout : out STD_LOGIC_VECTOR(7 downto 0);
		
		ready : out STD_LOGIC
	);
end entity;

architecture default of DisplayController is
	signal rd_addr, wr_addr, erase_addr : STD_LOGIC_VECTOR(17 downto 0);
	signal doublebuf, switch, wren, erase, fullerase : STD_LOGIC;
	
	signal next_xpos_rd, next_ypos_rd : STD_LOGIC_VECTOR(9 downto 0);
	signal rd_addr_product, rd_addr_shift, wr_addr_shift, wr_calc_addr, rd_swap_addr, wr_swap_addr : STD_LOGIC_VECTOR(19 downto 0);
	
	signal wr_val, rd_val : STD_LOGIC_VECTOR(2 downto 0);
	signal random : STD_LOGIC_VECTOR(2 downto 0);
	
begin
	ready <= not erase;
	
	wr_val <= Rin & Gin & Bin when erase = '0' else "000";
	Rout <= X"FF" when rd_val(2) = '1' else X"00";
	Gout <= X"FF" when rd_val(1) = '1' else X"00";
	Bout <= X"FF" when rd_val(0) = '1' else X"00";
	
	rd_swap_addr <= B"0000_0000_0000_0000_0000" when switch = '1' else B"0001_0111_0111_0000_0000";
	wr_swap_addr <= B"0000_0000_0000_0000_0000" when switch = '0' else B"0001_0111_0111_0000_0000";
	
	rd_addr_shift <= rd_swap_addr when doublebuf = '1' else B"0000_0000_0000_0000_0000";
	wr_addr_shift <= wr_swap_addr when doublebuf = '1' else B"0000_0000_0000_0000_0000";

	next_xpos_rd <= STD_LOGIC_VECTOR(unsigned(xpos_rd) + 1) when unsigned(xpos_rd) /= 799
		else "0000000000";
	next_ypos_rd <= ypos_rd when unsigned(xpos_rd) /= 799 else STD_LOGIC_VECTOR(unsigned(ypos_rd) + 1);
	
	rd_addr_product <= STD_LOGIC_VECTOR((unsigned(next_xpos_rd) srl 1) + ((unsigned(next_ypos_rd) srl 1) * 400)
	 + unsigned(rd_addr_shift));
	rd_addr <= rd_addr_product(17 downto 0);
	
	wr_calc_addr <= STD_LOGIC_VECTOR(unsigned(xpos_wr) + (unsigned(ypos_wr) * 400) + unsigned(wr_addr_shift) );
	wr_addr <= wr_calc_addr(17 downto 0) when erase = '0' else erase_addr;
	
	VideoRAM : entity work.VideoMem111(SYN)
	port map(wr_val, rd_addr, clkrd, wr_addr, clkwr, wren, rd_val);
	
	process is

	begin
		wait until clkwr = '1';
		
		if reset = '1' then
			erase_addr <= B"00_0000_0000_0000_0000";
			fullerase <= '1';
			erase <= '1';
			wren <= '1';			
			doublebuf <= '1';
			switch <= '0';
		elsif erase = '1' then
			if ((erase_addr = B"01_0111_0110_1111_1111") and (fullerase = '0'))
				or ( erase_addr = B"10_1110_1101_1111_1111" ) then
				wren <= '0';
				erase <= '0';
				fullerase <= '0';
			else
				erase_addr <= STD_LOGIC_VECTOR(unsigned(erase_addr) + 1);
			end if;
		elsif wr = '1' then
			wren <= '1';
		elsif double_on = '1' then
			doublebuf <= '1';
		elsif double_off = '1' then
			doublebuf <= '0';
		elsif swap = '1' then
			switch <= not switch;
			wren <= '1';
			erase <= '1';
			if switch = '1' then
				erase_addr <= B"00_0000_0000_0000_0000";
			else
				erase_addr <= B"01_0111_0111_0000_0000";
			end if;
		else
			wren <= '0';
		end if;
		
	end process;
	
end architecture;
