use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity ProgramCounter is
	port ( clk, reset : in STD_LOGIC;
	
			 --wr_xPC, wr_yPC : in axisPC;
			 --wr_BS : in STD_LOGIC_VECTOR(23 downto 0);
			 
			 flowDir, returnDir : in direction;
			 flowLength, returnLength : in dirLength;
			 instr_flow, instr_return : in STD_LOGIC;
			 
			 instr_address : out STD_LOGIC_VECTOR(12 downto 0)
			 );
end entity;

architecture default of ProgramCounter is
	signal xPC, yPC, xyChange : STD_LOGIC_VECTOR(4 downto 0);
	signal updateDir : direction;
	signal updateLength : dirLength;
begin

	updateDir <= flowDir when instr_flow = '1' else returnDir;
	updateLength <= flowLength when instr_flow = '1' else returnLength;

	with updateLength select
	xyChange <=
		B"00001" when len1,
		B"00010" when len2,
		B"00011" when len3,
		B"00100" when len4,
		B"00000" when others;
		
	instr_address <= B"00" & STD_LOGIC_VECTOR( 
		64 + unsigned( xPC & '0' ) + unsigned ( yPC & B"00000" & '0') );

	process is
	begin
		wait until clk = '1';
		
		if reset = '1' then
			xPC <= B"00000";
			yPC <= B"00000";
		elsif instr_flow = '1' or instr_return = '1' then
			if updateDir = dirU then
				yPC <= STD_LOGIC_VECTOR(unsigned(yPC) - unsigned(xyChange));
			elsif updateDir = dirR then
				xPC <= STD_LOGIC_VECTOR(unsigned(xPC) + unsigned(xyChange));
			elsif updateDir = dirD then
				yPC <= STD_LOGIC_VECTOR(unsigned(yPC) + unsigned(xyChange));
			elsif updateDir = dirL then
				xPC <= STD_LOGIC_VECTOR(unsigned(xPC) - unsigned(xyChange));
			end if;
		end if;
	end process;
end architecture;