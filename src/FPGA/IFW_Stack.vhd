use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity IFW_Stack is
	port ( clk, reset : in STD_LOGIC;
	
		push, pop : in STD_LOGIC;
	
		IS_in : in STD_LOGIC_VECTOR(2 downto 0);
		PDF_in : in STD_LOGIC;
		RL_in : in dirLength;
		RD_in : in direction;
		
		IS_out : buffer STD_LOGIC_VECTOR(2 downto 0);
		PDF_out : buffer STD_LOGIC;
		RL_out : buffer dirLength;
		RD_out : buffer direction;
		
		IS_wr : out STD_LOGIC;
		IFWempty : out STD_LOGIC
		
		);
end entity;

architecture default of IFW_Stack is
	signal push_cmd, pop_cmd, PDF_cmd : STD_LOGIC := '0';
	signal PDF_latch : STD_LOGIC;
	
	signal addr : STD_LOGIC_VECTOR (8 DOWNTO 0);
	signal data, wrdata : STD_LOGIC_VECTOR (7 DOWNTO 0);
	signal wren, rden, transfer_data : STD_LOGIC := '0';
	signal mout : STD_LOGIC_VECTOR (7 DOWNTO 0);

begin
	
	stackmem : entity work.InstrStack_RAM(SYN)
		port map (addr, clk, wrdata, rden, wren, mout);
		
	--IFWempty <= '1' when addr = B"0_0000_0000" else '0';
		
	process is
		--variable SI : reg10 := 0;
		--variable IS_bits : std_logic_vector(0 to 2);
	begin
		wait until clk = '1';
		
		PDF_cmd <= '0';
		
		if PDF_in = '1' then
			PDF_cmd <= '1';
		end if;
		
		if PDF_cmd = '1' then
			wrdata(3) <= '1';
			--PDF_latch <= '1';
			IS_out <= "010";
			wrdata(2 downto 0) <= "010";
		end if;
		
		if reset = '1' then
			--SI := 0;			
			wren <= '0';
			rden <= '0';
			addr <= B"1_1111_1111";
			IS_out <= "000";
			PDF_latch <= '0';
			PDF_out <= '0';
			IS_wr <= '0';
			transfer_data <= '0';
			IFWempty <= '1';
			
		elsif push_cmd = '0' and pop_cmd = '0' then
			wren <= '0'; -- reset data write
			IS_wr <= '0';
			
			if push = '1' then 
			
				IFWempty <= '0';
			
				push_cmd <= '1';
				
				IS_out <= IS_in;
				PDF_out <= PDF_in;
				RL_out <= RL_in;
				RD_out <= RD_in;
				
				data(2 downto 0) <= IS_in;
				data(3) <= PDF_in;
				
				case RL_in is
					when len1 =>
						data(5 downto 4) <= "00";
					when len2 =>
						data(5 downto 4) <= "01";
					when len3 =>
						data(5 downto 4) <= "10";
					when len4 =>
						data(5 downto 4) <= "11";
				end case;
				
				case RD_in is
					when dirU =>
						data(7 downto 6) <= "00";
					when dirR =>
						data(7 downto 6) <= "01";
					when dirD =>
						data(7 downto 6) <= "10";
					when dirL =>
						data(7 downto 6) <= "11";
					when others =>
						data(7 downto 6) <= "00";
				end case;
				
				wren <= '1'; -- write the data from wrdata
				
			elsif pop = '1' then
				pop_cmd <= '1';
				rden <= '1';
				IS_wr <= '1';
				IS_out <= wrdata(2 downto 0);
				PDF_out <= wrdata(3);
				addr <= std_logic_vector(unsigned(addr) - 1);
				
				if addr = B"0_0000_0000" then
					IFWempty <= '1';
				end if;
				
			end if;
			
		elsif push_cmd = '1' then
			wren <= '0';
			push_cmd <= '0';
			wrdata <= data; -- these data will be written next
			addr <= std_logic_vector(unsigned(addr) + 1);
		elsif pop_cmd = '1' then
			if rden = '1' then
				rden <= '0';
			else
				pop_cmd <= '0';
				-- decode the data
				IS_wr <= '0';
				--PDF_latch <= wrdata(3);
				
				case mout(5 downto 4) is
					when B"00" =>
						RL_out <= len1;
					when B"01" =>
						RL_out <= len2;
					when B"10" =>
						RL_out <= len3;
					when B"11" =>
						RL_out <= len4;
					when others =>
						RL_out <= len1;
				end case;
			
				case mout(7 downto 6) is
					when B"00" =>
						RD_out <= dirU;
					when B"01" =>
						RD_out <= dirR;
					when B"10" =>
						RD_out <= dirD;
					when B"11" =>
						RD_out <= dirL;
					when others =>
						RD_out <= dirU;
				end case;
				
				wrdata <= mout;	-- assign the read data
			end if;	
			
		end if;
	end process;
end architecture;