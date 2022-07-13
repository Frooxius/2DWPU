use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity ProgramCounter is
	port ( clk, reset : in STD_LOGIC;
	
			 --wr_xPC, wr_yPC : in axisPC;
			 --wr_BS : in STD_LOGIC_VECTOR(23 downto 0);
			 
			 flowDir, returnDir : in direction;
			 flowLength, returnLength : in dirLength;
			 instr_flow, instr_return : in STD_LOGIC;
			 
			 instr_address : out STD_LOGIC_VECTOR(12 downto 0);
			 
			 secondDir : in direction;
			 secondLength : in dirLength;
			 second_xPC, second_yPC : out STD_LOGIC_VECTOR(4 downto 0);
			 extern_xPC, extern_yPC : in STD_LOGIC_VECTOR(4 downto 0);
			 extern_load : in STD_LOGIC
			 );
end entity;

architecture default of ProgramCounter is
	signal xPC, yPC, xyChange, second_xyChange : STD_LOGIC_VECTOR(4 downto 0);
	signal updateDir : direction;
	signal updateLength : dirLength;
	signal intern_addr, extern_addr : STD_LOGIC_VECTOR(12 downto 0);
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
		
	with secondLength select
	second_xyChange <=
		B"00001" when len1,
		B"00010" when len2,
		B"00011" when len3,
		B"00100" when len4,
		B"00000" when others;
		
	with secondDir select
	second_xPC <=
		STD_LOGIC_VECTOR(unsigned(xPC) + unsigned(second_xyChange)) when dirR,
		STD_LOGIC_VECTOR(unsigned(xPC) - unsigned(second_xyChange)) when dirL,
		xPC when others;
	
	with secondDir select
	second_yPC <=
		STD_LOGIC_VECTOR(unsigned(yPC) + unsigned(second_xyChange)) when dirD,
		STD_LOGIC_VECTOR(unsigned(yPC) - unsigned(second_xyChange)) when dirU,
		yPC when others;
		
	intern_addr <= B"00" & STD_LOGIC_VECTOR( 
		unsigned( xPC & '0' ) + unsigned ( yPC & B"00000" & '0') + 64 );
		
	extern_addr <= B"00" & STD_LOGIC_VECTOR( 
		unsigned( extern_xPC & '0' ) + unsigned ( extern_yPC & B"00000" & '0') + 64 );
		
	instr_address <= intern_addr when extern_load = '0' else extern_addr;

	process is
	begin
		wait until clk = '1';
		
		if reset = '1' then
			xPC <= B"00000";
			yPC <= B"00000";
		elsif extern_load = '1' then
			xPC <= extern_xPC;
			yPC <= extern_yPC;
		elsif (instr_flow = '1') or (instr_return = '1') then
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