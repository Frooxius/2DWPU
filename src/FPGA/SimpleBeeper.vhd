library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity SimpleBeeper is
	port( clk, reset, wr : in STD_LOGIC;
			wrvalue : in STD_LOGIC_VECTOR(15 downto 0);
			beep : buffer STD_LOGIC
	);
end entity;

architecture default of SimpleBeeper is

	signal front, back : unsigned(5 downto 0);	-- addresses 
	signal freqcount, timecount : unsigned(31 downto 0);
	
begin

	process is
		variable memory : work.simpleBeeperDeclarations.beeperMem;
	begin
		wait until clk = '1';

		if reset = '1' then
			beep <= '0';
			front <= (others => '0');
			back <= (others => '0');		
			freqcount <= (others => '0');
			timecount <= (others => '0');
		else
			if wr = '1' and front /= (back-1) then
				memory(to_integer(front)) := wrvalue;
				front <= front + 1;
			end if;
			
			if front /= back then
				if timecount >= unsigned( X"0" & memory(to_integer(back))(15 downto 8) & X"00002" ) then
					timecount <= (others => '0');
					freqcount <= (others => '0');
					back <= back + 1;
				else
					timecount <= timecount + 1;
				end if;
				
				if freqcount >= unsigned( X"000" & (not memory(to_integer(back))(7 downto 0)) & B"0000_0000_0000" ) then
					freqcount <= (others => '0');
					beep <= not beep;
				else
					freqcount <= freqcount + 1;
				end if;
				
			else
				beep <= '0';
			end if;
			
		end if;
		
	end process;
end architecture;