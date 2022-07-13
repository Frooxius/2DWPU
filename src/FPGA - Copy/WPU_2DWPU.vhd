use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity WPU_2DWPU is
	port (clk, reset : in STD_LOGIC ;
		testout : out STD_LOGIC_VECTOR(31 downto 0)
	);
end entity;

architecture default of WPU_2DWPU is

	signal newval0 : STD_LOGIC_VECTOR(31 downto 0);
	signal LI, RE, SQ, CI, TR, HE, ST, EL : STD_LOGIC_VECTOR(31 downto 0);
	signal LI_wr, RE_wr, SQ_wr, CI_wr, TR_wr, HE_wr, ST_wr, EL_wr : STD_LOGIC;
	
	signal addr_in0, addr_in1, addr_in2, addr_in3, addr_in4, addr_in5, addr_in6, addr_in7 : STD_LOGIC_VECTOR(23 downto 0);
	signal data_out0, data_out1, data_out2, data_out3,	data_out4, data_out5, data_out6, data_out7 : STD_LOGIC_VECTOR(31 downto 0);
	signal data_in0, data_in1, data_in2, data_in3, data_in4, data_in5, data_in6, data_in7 : STD_LOGIC_VECTOR(31 downto 0);
	signal 	rd_0, rd_1, rd_2, rd_3, rd_4, rd_5, rd_6, rd_7,
				wr_0, wr_1, wr_2, wr_3, wr_4, wr_5, wr_6, wr_7 : STD_LOGIC;
	signal rdy_0, rdy_1, rdy_2, rdy_3, rdy_4, rdy_5, rdy_6, rdy_7 : STD_LOGIC;

begin
	testout <= TR;

	data_in0 <= newval0;
	
	rd_1 <= '0'; rd_2 <= '0'; rd_3 <= '0'; rd_4 <= '0'; rd_5 <= '0'; rd_6 <= '0'; rd_7 <= '0'; 
	wr_1 <= '0'; wr_2 <= '0'; wr_3 <= '0'; wr_4 <= '0'; wr_5 <= '0'; wr_6 <= '0'; wr_7 <= '0'; 

	MemInterface : entity work.DataRam8PortManager(default)
	port map ( clk,
		addr_in0, addr_in1, addr_in2, addr_in3, addr_in4, addr_in5, addr_in6, addr_in7,
		data_out0, data_out1, data_out2, data_out3,	data_out4, data_out5, data_out6, data_out7,
		data_in0, data_in1, data_in2, data_in3, data_in4, data_in5, data_in6, data_in7,
		rd_0, rd_1, rd_2, rd_3, rd_4, rd_5, rd_6, rd_7,
		wr_0, wr_1, wr_2, wr_3, wr_4, wr_5, wr_6, wr_7,
		rdy_0, rdy_1, rdy_2, rdy_3, rdy_4, rdy_5, rdy_6, rdy_7);
	
	Registers : entity work.SharedWorkRegisters(default)
	port map ( clk, reset,
		newval0,
		LI, RE, SQ, CI, TR, HE, ST, EL,
		LI_wr, RE_wr, SQ_wr, CI_wr, TR_wr, HE_wr, ST_wr, EL_wr);
	
	
	Core0 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		newval0,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		LI_wr, RE_wr, SQ_wr, CI_wr, TR_wr, HE_wr, ST_wr, EL_wr,
		addr_in0, wr_0, rd_0, rdy_0, data_out0);
	
end architecture;