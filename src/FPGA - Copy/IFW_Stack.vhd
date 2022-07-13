use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity IFW_Stack is
	port ( clk, reset : in STD_LOGIC;
	
		push, pop : in STD_LOGIC;
	
		IS_in : in STD_LOGIC_VECTOR(2 downto 0);
		PDF_in : in STD_LOGIC;
		RL_in : in dirLength;
		RD_in : in direction;
		
		IS_out : out STD_LOGIC_VECTOR(2 downto 0);
		PDF_out : out STD_LOGIC;
		RL_out : out dirLength;
		RD_out : out direction;
		
		IS_wr : out STD_LOGIC
		
		);
end entity;

architecture default of IFW_Stack is
	signal push_cmd, pop_cmd : bit := '0';
	
	signal addr : STD_LOGIC_VECTOR (8 DOWNTO 0);
	signal data : STD_LOGIC_VECTOR (7 DOWNTO 0);
	signal wren, rden : STD_LOGIC := '0';
	signal mout : STD_LOGIC_VECTOR (7 DOWNTO 0);

begin
	
	stackmem : entity work.InstrStack_RAM(SYN)
		port map (addr, clk, data, rden, wren, mout);
	
	process is
		--variable SI : reg10 := 0;
		--variable IS_bits : std_logic_vector(0 to 2);
	begin
		wait until clk = '1';
		
		if reset = '1' then
			--SI := 0;			
			wren <= '0';
			rden <= '0';
			addr <= B"1_1111_1111";
			IS_out <= "000";
			PDF_out <= '0';
			IS_wr <= '0';
			
		elsif push_cmd = '0' and pop_cmd = '0' then
			wren <= '0'; -- reset data write
			IS_wr <= '0';
			
			if push = '1' then 
			
				push_cmd <= '1';
				addr <= std_logic_vector(unsigned(addr) + 1);
				--SI := SI + 1;
				
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
						data (7 downto 6) <= "00";
				end case;
				
			elsif pop = '1' then
				pop_cmd <= '1';
				rden <= '1';
				IS_wr <= '1';
				addr <= std_logic_vector(unsigned(addr) - 1);
			end if;
			
		elsif push_cmd = '1' then
			wren <= '1'; -- write the data
			push_cmd <= '0';
		else
			if rden = '1' then
				rden <= '0';
			else
				pop_cmd <= '0';
				-- decode the data
				IS_out <= mout(2 downto 0);
				IS_wr <= '0';
				PDF_out <= mout(3);
			
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
			end if;	
			
		end if;
	end process;
end architecture;