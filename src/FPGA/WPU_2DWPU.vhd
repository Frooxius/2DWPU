use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity WPU_2DWPU is
	port (clk, reset : in STD_LOGIC ;
		store_val : out STD_LOGIC_VECTOR(31 downto 0);
		io_addr : out STD_LOGIC_VECTOR(15 downto 0);
		io_wr, io_rd : out STD_LOGIC;
		io_rdy : in STD_LOGIC;
		io_in : in STD_LOGIC_VECTOR(15 downto 0);
		
		core_en : in work.Parallel2DWPU.bitConnect;
		core_active : out work.Parallel2DWPU.bitConnect
	);
end entity;

architecture default of WPU_2DWPU is

	signal newval0 : STD_LOGIC_VECTOR(31 downto 0);
	signal LI, RE, SQ, CI, TR, HE, ST, EL : STD_LOGIC_VECTOR(31 downto 0);
	signal LI_wr, RE_wr, SQ_wr, CI_wr, TR_wr, HE_wr, ST_wr, EL_wr : STD_LOGIC;
	
	signal addr_in0, addr_in1, addr_in2, addr_in3, addr_in4, addr_in5, addr_in6, addr_in7, addr_in8, addr_in9, addr_in10, addr_in11 : STD_LOGIC_VECTOR(23 downto 0);
	signal data_out0, data_out1, data_out2, data_out3,	data_out4, data_out5, data_out6, data_out7, data_out8, data_out9, data_out10, data_out11 : STD_LOGIC_VECTOR(31 downto 0);
	signal data_in0, data_in1, data_in2, data_in3, data_in4, data_in5, data_in6, data_in7, data_in8, data_in9, data_in10, data_in11 : STD_LOGIC_VECTOR(31 downto 0);
	signal 	rd_0, rd_1, rd_2, rd_3, rd_4, rd_5, rd_6, rd_7, rd_8, rd_9, rd_10, rd_11,
				wr_0, wr_1, wr_2, wr_3, wr_4, wr_5, wr_6, wr_7, wr_8, wr_9, wr_10, wr_11 : STD_LOGIC;
	signal rdy_0, rdy_1, rdy_2, rdy_3, rdy_4, rdy_5, rdy_6, rdy_7, rdy_8, rdy_9, rdy_10, rdy_11 : STD_LOGIC;
	
	signal addrIO_in0, addrIO_in1, addrIO_in2, addrIO_in3, addrIO_in4, addrIO_in5, addrIO_in6, addrIO_in7, addrIO_in8, addrIO_in9, addrIO_in10, addrIO_in11 : STD_LOGIC_VECTOR(15 downto 0);
	--signal dataIO_out0, dataIO_out1, dataIO_out2, dataIO_out3, dataIO_out4, dataIO_out5, dataIO_out6, dataIO_out7, dataIO_out8, dataIO_out9, dataIO_out10, dataIO_out11 : STD_LOGIC_VECTOR(15 downto 0);
	--signal dataIO_in0, dataIO_in1, dataIO_in2, dataIO_in3, dataIO_in4, dataIO_in5, dataIO_in6, dataIO_in7, dataIO_in8, dataIO_in9, dataIO_in10, dataIO_in11 : STD_LOGIC_VECTOR(15 downto 0);
	signal 	rdIO_0, rdIO_1, rdIO_2, rdIO_3, rdIO_4, rdIO_5, rdIO_6, rdIO_7, rdIO_8, rdIO_9, rdIO_10, rdIO_11,
				wrIO_0, wrIO_1, wrIO_2, wrIO_3, wrIO_4, wrIO_5, wrIO_6, wrIO_7, wrIO_8, wrIO_9, wrIO_10, wrIO_11 : STD_LOGIC;
	signal rdyIO_0, rdyIO_1, rdyIO_2, rdyIO_3, rdyIO_4, rdyIO_5, rdyIO_6, rdyIO_7, rdyIO_8, rdyIO_9, rdyIO_10, rdyIO_11 : STD_LOGIC;
	
	signal io_data : STD_LOGIC_VECTOR(15 downto 0);
	
	signal par_xPC, par_yPC : work.Parallel2DWPU.axisPCconnect;
	signal par_ARG, par_PO, par_PE : work.Parallel2DWPU.reg32connect;
	signal par_rqst, par_fork : work.Parallel2DWPU.bitConnect;
	signal par_acpt, par_redy : work.Parallel2DWPU.bitConnect;
	signal par_ftch, par_free : work.Parallel2DWPU.bitConnect;

begin
	--data_in0 <= newval0;	
	store_val <= X"0000" & io_data;
	
	rd_11 <= '0';
	wr_11 <= '0';
	
	ParallelManager : entity work.ParallelismManager(default)
	port map ( clk, reset,
		par_xPC, par_yPC, par_ARG, par_PO, par_PE,
		par_rqst, par_acpt, par_redy, par_fork, par_ftch, par_free, core_en, core_active);

	MemInterface : entity work.DataRam8PortManager(default)
	port map ( clk,
		addr_in0, addr_in1, addr_in2, addr_in3, addr_in4, addr_in5, addr_in6, addr_in7, addr_in8, addr_in9, addr_in10, addr_in11,
		data_out0, data_out1, data_out2, data_out3,	data_out4, data_out5, data_out6, data_out7, data_out8, data_out9, data_out10, data_out11,
		data_in0, data_in1, data_in2, data_in3, data_in4, data_in5, data_in6, data_in7, data_in8, data_in9, data_in10, data_in11,
		rd_0, rd_1, rd_2, rd_3, rd_4, rd_5, rd_6, rd_7, rd_8, rd_9, rd_10, rd_11,
		wr_0, wr_1, wr_2, wr_3, wr_4, wr_5, wr_6, wr_7, wr_8, wr_9, wr_10, wr_11,
		rdy_0, rdy_1, rdy_2, rdy_3, rdy_4, rdy_5, rdy_6, rdy_7, rdy_8, rdy_9, rdy_10, rdy_11);
	
	Registers : entity work.SharedWorkRegisters(default)
	port map ( clk, reset,
		data_in0,
		LI, RE, SQ, CI, TR, HE, ST, EL,
		LI_wr, SQ_wr, RE_wr, TR_wr, CI_wr, HE_wr, ST_wr, EL_wr);
		
	IOportInterface : entity work.IOportInterface
	port map (clk, 
		data_in0(15 downto 0), data_in1(15 downto 0), data_in2(15 downto 0), data_in3(15 downto 0), data_in4(15 downto 0), data_in5(15 downto 0), data_in6(15 downto 0), data_in7(15 downto 0), data_in8(15 downto 0), data_in9(15 downto 0), 
		addrIO_in0, addrIO_in1, addrIO_in2, addrIO_in3, addrIO_in4, addrIO_in5, addrIO_in6, addrIO_in7, addrIO_in8, addrIO_in9,
		rdIO_0, rdIO_1, rdIO_2, rdIO_3, rdIO_4, rdIO_5, rdIO_6, rdIO_7, rdIO_8, rdIO_9,
		wrIO_0, wrIO_1, wrIO_2, wrIO_3, wrIO_4, wrIO_5, wrIO_6, wrIO_7, wrIO_8, wrIO_9,
		rdyIO_0, rdyIO_1, rdyIO_2, rdyIO_3, rdyIO_4, rdyIO_5, rdyIO_6, rdyIO_7, rdyIO_8, rdyIO_9,
		
		io_addr, io_data, io_rd, io_wr);
	
	Core0 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in0,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		LI_wr, SQ_wr, RE_wr, TR_wr, CI_wr, HE_wr, ST_wr, EL_wr,
		addr_in0, wr_0, rd_0, rdy_0, data_out0,
		addrIO_in0, wrIO_0, rdIO_0, rdyIO_0, io_in,
		-- parallel stuff
		'1', par_xPC(0), par_yPC(0), par_ARG(0), par_PO(0), par_PE(0),
		par_rqst(0), par_acpt(0), par_redy(0), par_fork(0), par_ftch(0), par_free(0));
		
	Core1 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in1,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		open, open, open, open, open, open, open, open,
		addr_in1, wr_1, rd_1, rdy_1, data_out1,
		addrIO_in1, wrIO_1, rdIO_1, rdyIO_1, io_in,
		-- parallel stuff
		'0', par_xPC(1), par_yPC(1), par_ARG(1), par_PO(1), par_PE(1),
		par_rqst(1), par_acpt(1), par_redy(1), par_fork(1), par_ftch(1), par_free(1));
		
	Core2 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in2,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		open, open, open, open, open, open, open, open,
		addr_in2, wr_2, rd_2, rdy_2, data_out2,
		addrIO_in2, wrIO_2, rdIO_2, rdyIO_2, io_in,
		-- parallel stuff
		'0', par_xPC(2), par_yPC(2), par_ARG(2), par_PO(2), par_PE(2),
		par_rqst(2), par_acpt(2), par_redy(2), par_fork(2), par_ftch(2), par_free(2));
		
	Core3 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in3,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		open, open, open, open, open, open, open, open,
		addr_in3, wr_3, rd_3, rdy_3, data_out3,
		addrIO_in3, wrIO_3, rdIO_3, rdyIO_3, io_in,
		-- parallel stuff
		'0', par_xPC(3), par_yPC(3), par_ARG(3), par_PO(3), par_PE(3),
		par_rqst(3), par_acpt(3), par_redy(3), par_fork(3), par_ftch(3), par_free(3));
		
	Core4 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in4,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		open, open, open, open, open, open, open, open,
		addr_in4, wr_4, rd_4, rdy_4, data_out4,
		addrIO_in4, wrIO_4, rdIO_4, rdyIO_4, io_in,
		-- parallel stuff
		'0', par_xPC(4), par_yPC(4), par_ARG(4), par_PO(4), par_PE(4),
		par_rqst(4), par_acpt(4), par_redy(4), par_fork(4), par_ftch(4), par_free(4));
		
	Core5 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in5,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		open, open, open, open, open, open, open, open,
		addr_in5, wr_5, rd_5, rdy_5, data_out5,
		addrIO_in5, wrIO_5, rdIO_5, rdyIO_5, io_in,
		-- parallel stuff
		'0', par_xPC(5), par_yPC(5), par_ARG(5), par_PO(5), par_PE(5),
		par_rqst(5), par_acpt(5), par_redy(5), par_fork(5), par_ftch(5), par_free(5));
		
	Core6 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in6,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		open, open, open, open, open, open, open, open,
		addr_in6, wr_6, rd_6, rdy_6, data_out6,
		addrIO_in6, wrIO_6, rdIO_6, rdyIO_6, io_in,
		-- parallel stuff
		'0', par_xPC(6), par_yPC(6), par_ARG(6), par_PO(6), par_PE(6),
		par_rqst(6), par_acpt(6), par_redy(6), par_fork(6), par_ftch(6), par_free(6));
		
	Core7 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in7,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		open, open, open, open, open, open, open, open,
		addr_in7, wr_7, rd_7, rdy_7, data_out7,
		addrIO_in7, wrIO_7, rdIO_7, rdyIO_7, io_in,
		-- parallel stuff
		'0', par_xPC(7), par_yPC(7), par_ARG(7), par_PO(7), par_PE(7),
		par_rqst(7), par_acpt(7), par_redy(7), par_fork(7), par_ftch(7), par_free(7));
		
	Core8 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in8,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		open, open, open, open, open, open, open, open,
		addr_in8, wr_8, rd_8, rdy_8, data_out8,
		addrIO_in8, wrIO_8, rdIO_8, rdyIO_8, io_in,
		-- parallel stuff
		'0', par_xPC(8), par_yPC(8), par_ARG(8), par_PO(8), par_PE(8),
		par_rqst(8), par_acpt(8), par_redy(8), par_fork(8), par_ftch(8), par_free(8));
		
	Core9 : entity work.Core2DWPU(default)
	port map ( clk, reset,
		data_in9,
		LI, SQ, RE, TR, CI, HE, ST, EL,
		open, open, open, open, open, open, open, open,
		addr_in9, wr_9, rd_9, rdy_9, data_out9,
		addrIO_in9, wrIO_9, rdIO_9, rdyIO_9, io_in,
		-- parallel stuff
		'0', par_xPC(9), par_yPC(9), par_ARG(9), par_PO(9), par_PE(9),
		par_rqst(9), par_acpt(9), par_redy(9), par_fork(9), par_ftch(9), par_free(9));
		
--	Core10 : entity work.Core2DWPU(default)
--	port map ( clk, reset,
--		data_in10,
--		LI, SQ, RE, TR, CI, HE, ST, EL,
--		open, open, open, open, open, open, open, open,
--		addr_in10, wr_10, rd_10, rdy_10, data_out10,
--		open, open, open, '0', X"FFFF",
--		-- parallel stuff
--		'0', par_xPC(10), par_yPC(10), par_ARG(10), par_PO(10), par_PE(10),
--		par_rqst(10), par_acpt(10), par_redy(10), par_fork(10), par_ftch(10), par_free(10));
	
end architecture;