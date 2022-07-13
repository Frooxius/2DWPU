use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity IOportInterface is
	port ( clk : in STD_LOGIC;
		--data_out : out STD_LOGIC_VECTOR(15 downto 0);
		
		data_in0, data_in1, data_in2, data_in3, data_in4, data_in5, data_in6, data_in7, data_in8, data_in9 : in STD_LOGIC_VECTOR(15 downto 0);
		addr_in0, addr_in1, addr_in2, addr_in3, addr_in4, addr_in5, addr_in6, addr_in7, addr_in8, addr_in9 : in STD_LOGIC_VECTOR(15 downto 0);
		rd_0, rd_1, rd_2, rd_3, rd_4, rd_5, rd_6, rd_7, rd_8, rd_9,
		wr_0, wr_1, wr_2, wr_3, wr_4, wr_5, wr_6, wr_7, wr_8, wr_9 : in STD_LOGIC;
		rdy_0, rdy_1, rdy_2, rdy_3, rdy_4, rdy_5, rdy_6, rdy_7, rdy_8, rdy_9 : out STD_LOGIC;
		
		-- interface for the ram
		io_addr : out STD_LOGIC_VECTOR(15 downto 0);
		io_data : out STD_LOGIC_VECTOR(15 downto 0);
		io_rden, io_wren : out STD_LOGIC
		--io_out : in STD_LOGIC_VECTOR(15 downto 0)
		
		);
end entity;

architecture default of IOportInterface is
	signal access_0, access_1, access_2, access_3, access_4, access_5, access_6, access_7, access_8, access_9 : STD_LOGIC;
	signal cooldown : STD_LOGIC;
begin
	--data_out <= io_out;

	access_0 <= (rd_0 or wr_0) and cooldown;
	access_1 <= (rd_1 or wr_1) and cooldown;
	access_2 <= (rd_2 or wr_2) and cooldown;
	access_3 <= (rd_3 or wr_3) and cooldown;
	access_4 <= (rd_4 or wr_4) and cooldown;
	access_5 <= (rd_5 or wr_5) and cooldown;
	access_6 <= (rd_6 or wr_6) and cooldown;
	access_7 <= (rd_7 or wr_7) and cooldown;
	access_8 <= (rd_8 or wr_8) and cooldown;
	access_9 <= (rd_9 or wr_9) and cooldown;
	
	io_addr <= 
		addr_in0 when access_0 = '1' else
		addr_in1 when access_1 = '1' else
		addr_in2 when access_2 = '1' else
		addr_in3 when access_3 = '1' else
		addr_in4 when access_4 = '1' else
		addr_in5 when access_5 = '1' else
		addr_in6 when access_6 = '1' else
		addr_in7 when access_7 = '1' else
		addr_in8 when access_8 = '1' else
		addr_in9 when access_9 = '1' else
		X"0000";
		
	io_data <= 
		data_in0 when access_0 = '1' else
		data_in1 when access_1 = '1' else
		data_in2 when access_2 = '1' else
		data_in3 when access_3 = '1' else
		data_in4 when access_4 = '1' else
		data_in5 when access_5 = '1' else
		data_in6 when access_6 = '1' else
		data_in7 when access_7 = '1' else
		data_in8 when access_8 = '1' else
		data_in9 when access_9 = '1' else
		X"0000";
		
	io_rden <= rd_0 when access_0 = '1' else
					rd_1 when access_1 = '1' else
					rd_2 when access_2 = '1' else
					rd_3 when access_3 = '1' else
					rd_4 when access_4 = '1' else
					rd_5 when access_5 = '1' else
					rd_6 when access_6 = '1' else
					rd_7 when access_7 = '1' else
					rd_8 when access_8 = '1' else
					rd_9 when access_9 = '1' else
					'0';
					
	io_wren <= wr_0 when access_0 = '1' else
					wr_1 when access_1 = '1' else
					wr_2 when access_2 = '1' else
					wr_3 when access_3 = '1' else
					wr_4 when access_4 = '1' else
					wr_5 when access_5 = '1' else
					wr_6 when access_6 = '1' else
					wr_7 when access_7 = '1' else
					wr_8 when access_8 = '1' else
					wr_9 when access_9 = '1' else
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
		rdy_6 <= access_6 and (not (access_0 or access_1 or access_2 or access_3 or access_4 or access_5));
		rdy_7 <= access_7 and (not (access_0 or access_1 or access_2 or access_3 or access_4 or access_5 or access_6));
		rdy_8 <= access_8 and (not (access_0 or access_1 or access_2 or access_3 or access_4 or access_5 or access_6 or access_7));
		rdy_9 <= access_9 and (not (access_0 or access_1 or access_2 or access_3 or access_4 or access_5 or access_6 or access_7 or access_8));
		
		if (access_0 or access_1 or access_2 or access_3 or access_4 or access_5 or access_6 or access_7 or access_8 or access_9) = '0' then
			cooldown <= '1';
		else
			cooldown <= '0';
		end if;
	end process;
					
end architecture; 