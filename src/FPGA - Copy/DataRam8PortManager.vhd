use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity DataRam8PortManager is
	port ( clk : in STD_LOGIC;
	
	addr_in0, addr_in1, addr_in2, addr_in3, addr_in4, addr_in5, addr_in6, addr_in7 : in STD_LOGIC_VECTOR(23 downto 0);
	
	data_out0, data_out1, data_out2, data_out3,
	data_out4, data_out5, data_out6, data_out7 : out STD_LOGIC_VECTOR(31 downto 0);
	
	data_in0, data_in1, data_in2, data_in3,
	data_in4, data_in5, data_in6, data_in7 : in STD_LOGIC_VECTOR(31 downto 0);
	
	rd_0, rd_1, rd_2, rd_3, rd_4, rd_5, rd_6, rd_7,
	wr_0, wr_1, wr_2, wr_3, wr_4, wr_5, wr_6, wr_7 : in STD_LOGIC;
	
	rdy_0, rdy_1, rdy_2, rdy_3, rdy_4, rdy_5, rdy_6, rdy_7 : out STD_LOGIC
	
	);
end entity;

architecture default of DataRam8PortManager is
	signal data_out_manager0, data_out_manager1 : STD_LOGIC_VECTOR(31 downto 0);
	
	signal mem0_addr, mem1_addr : STD_LOGIC_VECTOR(23 downto 0);
	signal mem0_data, mem0_out, mem1_data, mem1_out : STD_LOGIC_VECTOR(31 downto 0);
	signal mem0_rden, mem0_wren, mem1_rden, mem1_wren : STD_LOGIC;
begin
	data_out0 <= data_out_manager0;
	data_out2 <= data_out_manager0;
	data_out4 <= data_out_manager0;
	data_out6 <= data_out_manager0;
	
	data_out1 <= data_out_manager1;
	data_out3 <= data_out_manager1;
	data_out5 <= data_out_manager1;
	data_out7 <= data_out_manager1;
	
	memory : entity work.DataDual32Ram(default)
	port map( clk, mem0_wren, mem1_wren, mem0_rden, mem1_rden,
		mem0_addr, mem1_addr, mem0_data, mem1_data,
		mem0_out, mem1_out);

	manager0 : entity work.DataRamInterface(default)
	port map( clk => clk, data_out => data_out_manager0,
		data_in0 => data_in0, data_in1 => data_in2, data_in2 => data_in4, data_in3 => data_in6,
		addr_in0 => addr_in0, addr_in1 => addr_in2, addr_in2 => addr_in4, addr_in3 => addr_in6,
		rd_0 => rd_0, rd_1 => rd_2, rd_2 => rd_4, rd_3 => rd_6,
		wr_0 => wr_0, wr_1 => wr_2, wr_2 => wr_4, wr_3 => wr_6,
		rdy_0 => rdy_0, rdy_1 => rdy_2, rdy_2 => rdy_4, rdy_3 => rdy_6,
		ram_addr => mem0_addr, ram_data => mem0_data, ram_rden => mem0_rden, ram_wren => mem0_wren, ram_out => mem0_out);
		
	manager1 : entity work.DataRamInterface(default)
	port map( clk => clk, data_out => data_out_manager1,
		data_in0 => data_in1, data_in1 => data_in3, data_in2 => data_in5, data_in3 => data_in7,
		addr_in0 => addr_in1, addr_in1 => addr_in3, addr_in2 => addr_in5, addr_in3 => addr_in7,
		rd_0 => rd_1, rd_1 => rd_3, rd_2 => rd_5, rd_3 => rd_7,
		wr_0 => wr_1, wr_1 => wr_3, wr_2 => wr_5, wr_3 => wr_7,
		rdy_0 => rdy_1, rdy_1 => rdy_3, rdy_2 => rdy_5, rdy_3 => rdy_7,
		ram_addr => mem1_addr, ram_data => mem1_data, ram_rden => mem1_rden, ram_wren => mem1_wren, ram_out => mem1_out);
end architecture;