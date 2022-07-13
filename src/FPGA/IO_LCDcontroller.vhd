use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity IO_LCDcontroller is
	port ( clk, reset, wr, rd : in STD_LOGIC;
		baseaddr, addr, in_val : in STD_LOGIC_VECTOR(15 downto 0);
		out_val : out STD_LOGIC_VECTOR(15 downto 0);
		
		xpos, ypos, color : out STD_LOGIC_VECTOR(15 downto 0);
		touchx, touchy : in STD_LOGIC_VECTOR(11 downto 0);
		ready : in STD_LOGIC;
		write_px, swap, singlebuf : out STD_LOGIC
		);
end entity;

architecture default of IO_LCDcontroller is
	signal actual_addr : STD_LOGIC_VECTOR(15 downto 0);
	
	signal touched : STD_LOGIC_VECTOR(15 downto 0);
	signal last_touchx, last_touchy : STD_LOGIC_VECTOR(11 downto 0);
	
	signal xposReg_wr, yposReg_wr, colorReg_wr, rd_touch : STD_LOGIC;
begin
	actual_addr <= STD_LOGIC_VECTOR(unsigned(addr) - unsigned(baseaddr));
	
	xposRegWrap : entity work.IOdeviceWrapper
		port map( X"0000", actual_addr, wr, '0', xposReg_wr, open, X"0000", open);
	xposReg : entity work.GenReg16
		port map( clk, xposReg_wr, reset, in_val, xpos);
		
	yposRegWrap : entity work.IOdeviceWrapper
		port map( X"0001", actual_addr, wr, '0', yposReg_wr, open, X"0000", open);
	yposReg : entity work.GenReg16
		port map( clk, yposReg_wr, reset, in_val, ypos);
		
	colorRegWrap : entity work.IOdeviceWrapper
		port map( X"0002", actual_addr, wr, '0', colorReg_wr, open, X"0000", open);
	colorReg : entity work.GenReg16
		port map( clk, colorReg_wr, reset, in_val, color);	
		
	writeRegWrap : entity work.IOdeviceWrapper
		port map( X"0003", actual_addr, wr, '0', write_px, open, X"0000", open);	
		
	touchxRead : entity work.IOdeviceWrapper
		port map( X"0004", actual_addr, '0', rd, open, open, X"0" & touchx, out_val);
		
	touchyRead : entity work.IOdeviceWrapper
		port map( X"0005", actual_addr, '0', rd, open, open, X"0" & touchy, out_val);
		
	swapBuffer : entity work.IOdeviceWrapper
		port map( X"0006", actual_addr, wr, '0', swap, open, X"0000", open);
		
	touchDetect : entity work.IOdeviceWrapper
		port map( X"0007", actual_addr, '0', rd, open, rd_touch, touched, out_val);
		
	readyDetect : entity work.IOdeviceWrapper
		port map( X"0008", actual_addr, '0', rd, open, open, B"0000_0000_0000_000" & ready, out_val);
		
	disableDouble : entity work.IOdeviceWrapper
		port map( X"0009", actual_addr, wr, '0', singlebuf, open, X"0000", open);
		
	process is
	begin
		wait until clk = '1';
		
		if reset = '1' then
			touched <= X"0000";
			last_touchx <= touchx;
			last_touchy <= touchy;
		else
			if touched = X"0000" then
				if (touchx /= last_touchx) or (touchy /= last_touchy) then
					touched <= X"0001";
				end if;
			elsif rd_touch = '1' then
				touched <= X"0000";
				last_touchx <= touchx;
				last_touchy <= touchy; 
			end if;
		end if;
	end process;
	
end architecture;