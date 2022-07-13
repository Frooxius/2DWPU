use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity DataRamInterface is
	port ( clk : in STD_LOGIC;
		data_out : out STD_LOGIC_VECTOR(31 downto 0);
		
		data_in0, data_in1, data_in2, data_in3, data_in4, data_in5 : in STD_LOGIC_VECTOR(31 downto 0);
		addr_in0, addr_in1, addr_in2, addr_in3, addr_in4, addr_in5 : in STD_LOGIC_VECTOR(23 downto 0);
		rd_0, rd_1, rd_2, rd_3, rd_4, rd_5,
		wr_0, wr_1, wr_2, wr_3, wr_4, wr_5 : in STD_LOGIC;
		rdy_0, rdy_1, rdy_2, rdy_3, rdy_4, rdy_5 : out STD_LOGIC;
		
		-- interface for the ram
		ram_addr : out STD_LOGIC_VECTOR(23 downto 0);
		ram_data : out STD_LOGIC_VECTOR(31 downto 0);
		ram_rden, ram_wren : out STD_LOGIC;
		ram_out : in STD_LOGIC_VECTOR(31 downto 0)
		
		);
end entity;

architecture default of DataRamInterface is
	signal access_0, access_1, access_2, access_3, access_4, access_5 : STD_LOGIC;
begin
	data_out <= ram_out;

	access_0 <= rd_0 or wr_0;
	access_1 <= rd_1 or wr_1;
	access_2 <= rd_2 or wr_2;
	access_3 <= rd_3 or wr_3;
	access_4 <= rd_4 or wr_4;
	access_5 <= rd_5 or wr_5;
	
	ram_addr <= 
		addr_in0 when access_0 = '1' else
		addr_in1 when access_1 = '1' else
		addr_in2 when access_2 = '1' else
		addr_in3 when access_3 = '1' else
		addr_in4 when access_4 = '1' else
		addr_in5 when access_5 = '1' else
		X"000000";
		
	ram_data <= 
		data_in0 when access_0 = '1' else
		data_in1 when access_1 = '1' else
		data_in2 when access_2 = '1' else
		data_in3 when access_3 = '1' else
		data_in4 when access_4 = '1' else
		data_in5 when access_5 = '1' else
		X"00000000";
		
	ram_rden <= rd_0 when access_0 = '1' else
					rd_1 when access_1 = '1' else
					rd_2 when access_2 = '1' else
					rd_3 when access_3 = '1' else
					rd_4 when access_4 = '1' else
					rd_5 when access_5 = '1' else
					'0';
					
	ram_wren <= wr_0 when access_0 = '1' else
					wr_1 when access_1 = '1' else
					wr_2 when access_2 = '1' else
					wr_3 when access_3 = '1' else
					wr_4 when access_4 = '1' else
					wr_5 when access_5 = '1' else
					'0';
					
	process is
	begin
		wait until clk = '1';
		rdy_0 <= access_0;
		rdy_1 <= access_1 and (not access_0);
		rdy_2 <= access_2 and (not (access_0 or access_1));
		rdy_3 <= access_3 and (not (access_0 or access_1 or access_2));
		rdy_4 <= access_4 and (not (access_0 or access_1 or access_2 or access_3));
		rdy_5 <= access_5 and (not (access_0 or access_1 or access_2 or access_3 or access_4));
	end process;
					
end architecture; 