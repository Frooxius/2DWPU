use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity DataDual32Ram is
	port ( clk, wrena, wrenb, rdena, rdenb : in STD_LOGIC;
		addra, addrb : in STD_LOGIC_VECTOR(23 downto 0);
		vala_in, valb_in : in STD_LOGIC_VECTOR(31 downto 0);
		vala_out, valb_out : out STD_LOGIC_VECTOR(31 downto 0)
		);
end entity;

architecture default of DataDual32Ram is
	signal addra0, addra1, addra2, addra3 : STD_LOGIC_VECTOR(23 downto 0);
	signal addrb0, addrb1, addrb2, addrb3 : STD_LOGIC_VECTOR(23 downto 0);
	
	signal vala_in0, vala_in1, vala_in2, vala_in3 : STD_LOGIC_VECTOR(7 downto 0);
	signal vala_out0, vala_out1, vala_out2, vala_out3 : STD_LOGIC_VECTOR(7 downto 0);
	signal valb_in0, valb_in1, valb_in2, valb_in3 : STD_LOGIC_VECTOR(7 downto 0);
	signal valb_out0, valb_out1, valb_out2, valb_out3 : STD_LOGIC_VECTOR(7 downto 0);
begin
	addra0 <= addra;
	addra1 <= STD_LOGIC_VECTOR( unsigned(addra) + 1 );
	addra2 <= STD_LOGIC_VECTOR( unsigned(addra) + 2 );
	addra3 <= STD_LOGIC_VECTOR( unsigned(addra) + 3 );
	
	with addra(1 downto 0) select
	vala_in0 <= vala_in(7  downto 0 ) when "00",
					vala_in(15 downto 8 ) when "01",
					vala_in(23 downto 16) when "10",
					vala_in(31 downto 24) when "11",
					X"00" when others; -- make ModelSim happy...

	with addra(1 downto 0) select
	vala_in1 <= vala_in(7  downto 0 ) when "11",
					vala_in(15 downto 8 ) when "00",
					vala_in(23 downto 16) when "01",
					vala_in(31 downto 24) when "10",
					X"00" when others; -- make ModelSim happy...					
					
	with addra(1 downto 0) select
	vala_in2 <= vala_in(7  downto 0 ) when "10",
					vala_in(15 downto 8 ) when "11",
					vala_in(23 downto 16) when "00",
					vala_in(31 downto 24) when "01",
					X"00" when others; -- make ModelSim happy...
					
	with addra(1 downto 0) select
	vala_in3 <= vala_in(7  downto 0 ) when "01",
					vala_in(15 downto 8 ) when "10",
					vala_in(23 downto 16) when "11",
					vala_in(31 downto 24) when "00",
					X"00" when others; -- make ModelSim happy...				
					
	with addra(1 downto 0) select
	vala_out <= vala_out3 & vala_out2 & vala_out1 & vala_out0 when "00",
					vala_out2 & vala_out1 & vala_out0 & vala_out3 when "01",
					vala_out1 & vala_out0 & vala_out3 & vala_out2 when "10",
					vala_out0 & vala_out3 & vala_out2 & vala_out1 when "11",
					X"00000000" when others; -- make ModelSim happy...
					
	addrb0 <= addrb;
	addrb1 <= STD_LOGIC_VECTOR( unsigned(addrb) + 1 );
	addrb2 <= STD_LOGIC_VECTOR( unsigned(addrb) + 2 );
	addrb3 <= STD_LOGIC_VECTOR( unsigned(addrb) + 3 );
	
	with addrb(1 downto 0) select
	valb_in0 <= valb_in(7  downto 0 ) when "00",
					valb_in(15 downto 8 ) when "01",
					valb_in(23 downto 16) when "10",
					valb_in(31 downto 24) when "11",
					X"00" when others; -- make ModelSim happy...

	with addrb(1 downto 0) select
	valb_in1 <= valb_in(7  downto 0 ) when "11",
					valb_in(15 downto 8 ) when "00",
					valb_in(23 downto 16) when "01",
					valb_in(31 downto 24) when "10",					
					X"00" when others; -- make ModelSim happy...					
					
	with addrb(1 downto 0) select
	valb_in2 <= valb_in(7  downto 0 ) when "10",
					valb_in(15 downto 8 ) when "11",
					valb_in(23 downto 16) when "00",
					valb_in(31 downto 24) when "01",
					X"00" when others; -- make ModelSim happy...				
					
	with addrb(1 downto 0) select
	valb_in3 <= valb_in(7  downto 0 ) when "01",
					valb_in(15 downto 8 ) when "10",
					valb_in(23 downto 16) when "11",
					valb_in(31 downto 24) when "00",
					X"00" when others; -- make ModelSim happy...
					
	with addrb(1 downto 0) select
	valb_out <= valb_out3 & valb_out2 & valb_out1 & valb_out0 when "00",
					valb_out2 & valb_out1 & valb_out0 & valb_out3 when "01",
					valb_out1 & valb_out0 & valb_out3 & valb_out2 when "10",
					valb_out0 & valb_out3 & valb_out2 & valb_out1 when "11",
					X"00000000" when others; -- make ModelSim happy...
					
	mem0 : entity work.DataRAM(SYN)
		port map( addra0(16 downto 2), addrb0(16 downto 2), clk, vala_in0, valb_in0, rdena, rdenb, wrena, wrenb, vala_out0, valb_out0 );
	mem1 : entity work.DataRAM(SYN)
		port map( addra1(16 downto 2), addrb1(16 downto 2), clk, vala_in1, valb_in1, rdena, rdenb, wrena, wrenb, vala_out1, valb_out1 );
	mem2 : entity work.DataRAM(SYN)
		port map( addra2(16 downto 2), addrb2(16 downto 2), clk, vala_in2, valb_in2, rdena, rdenb, wrena, wrenb, vala_out2, valb_out2 );
	mem3 : entity work.DataRAM(SYN)
		port map( addra3(16 downto 2), addrb3(16 downto 2), clk, vala_in3, valb_in3, rdena, rdenb, wrena, wrenb, vala_out3, valb_out3 );		
	
end architecture;