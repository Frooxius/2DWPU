library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity VGA800x600withProgress is
	port( clkrd : in STD_LOGIC;
	
			xpos_rd, ypos_rd : in STD_LOGIC_VECTOR(15 downto 0);
			out_xpos_rd, out_ypos_rd : out STD_LOGIC_VECTOR(9 downto 0);
						
			Rin, Gin, Bin : in STD_LOGIC_VECTOR(7 downto 0);
			Rout, Gout, Bout : out STD_LOGIC_VECTOR(7 downto 0);
			
			value0, value1 : in STD_LOGIC_VECTOR(15 downto 0)
	);
end entity;

architecture default of VGA800x600withProgress is

	signal use_vram : STD_LOGIC;
	signal Rout_val, Gout_val, Bout_val : STD_LOGIC_VECTOR(7 downto 0);	
	
	signal count : STD_LOGIC_VECTOR(31 downto 0);
	signal runline : unsigned(15 downto 0);
	
	signal value_sel : STD_LOGIC_VECTOR(15 downto 0);
	signal color_sel : STD_LOGIC_VECTOR(2 downto 0);
	signal color_intensity : STD_LOGIC_VECTOR(7 downto 0);
	signal xpos : STD_LOGIC_VECTOR(15 downto 0);
	signal fullpixel : STD_LOGIC_VECTOR(7 downto 0);
	signal pixshift, limit_pixshift : integer;
	
	signal intern_ypos, intern_xpos : STD_LOGIC_VECTOR(15 downto 0);

begin

	intern_ypos <= STD_LOGIC_VECTOR( unsigned(ypos_rd) - 60 );
	intern_xpos <= xpos_rd;
	out_xpos_rd <= intern_xpos(9 downto 0);
	out_ypos_rd <= intern_ypos(9 downto 0);
	
	use_vram <= '1' when (unsigned(ypos_rd) >= 60) and (unsigned(ypos_rd) < 540) else '0';
	
	Rout <=	Rin when use_vram = '1' else Rout_val and ( STD_LOGIC_VECTOR(unsigned(xpos(5 downto 0)) + unsigned(ypos_rd(5 downto 0)) + unsigned(count(22 downto 17)) + unsigned(count(25 downto 21)) + 0 ) OR B"1100_0000" );
	Gout <=	Gin when use_vram = '1' else Gout_val and ( STD_LOGIC_VECTOR(unsigned(xpos(5 downto 0)) + unsigned(ypos_rd(5 downto 0)) + unsigned(count(22 downto 17)) - unsigned(count(25 downto 21)) + 2 ) OR B"1100_0000" );
	Bout <=	Bin when use_vram = '1' else Bout_val and ( STD_LOGIC_VECTOR(unsigned(xpos(5 downto 0)) + unsigned(ypos_rd(5 downto 0)) + unsigned(count(25 downto 21)) - unsigned(count(22 downto 17)) + 8 ) OR B"1100_0000" );	
	
	value_sel <= value0 when unsigned(ypos_rd) < 60 else value1;
	color_sel <= "110" when unsigned(ypos_rd) < 60 else "011";
	
	xpos <= xpos_rd when unsigned(ypos_rd) < 60 else STD_LOGIC_VECTOR( 799 - unsigned(xpos_rd) );
	
	fullpixel <= X"FF";
	pixshift <= to_integer( signed( unsigned(xpos) - (unsigned(value_sel) / 81)) );
	limit_pixshift <=  to_integer(to_unsigned(pixshift, 32) srl 4)  when pixshift > 0 else 0;
	
	color_intensity <= STD_LOGIC_VECTOR( unsigned(fullpixel) srl limit_pixshift) when ((runline/4) mod 100) /= ((unsigned(xpos)/4) mod 100) else X"FF";
	
	Rout_val <= X"00" when color_sel(2) = '0' else color_intensity;
	Gout_val <= X"00" when color_sel(1) = '0' else color_intensity;
	Bout_val <= X"00" when color_sel(0) = '0' else color_intensity;			

	process is
	begin
		wait until clkrd = '1';
		count <= STD_LOGIC_VECTOR( unsigned(count) + 1 );	
		
		if count(15 downto 0) = (not (value0(15 downto 8) or value1(15 downto 8))) & X"FF" then
			if runline < 800 then
				runline <= runline + 1;
			else
				runline <= (others => '0');
			end if;
		end if;
		
	end process;
	
end architecture;