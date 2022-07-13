library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity MagController is 
	port( CLOCK_50, reset : in STD_LOGIC; coil_status : in STD_LOGIC_VECTOR(3 downto 0);
		player_status : buffer STD_LOGIC_VECTOR(3 downto 0)
		-- zero - negative dir
		-- one  - positive dir
		
		-- X0 Y0 X1 Y1
	);
end entity;

architecture default of MagController is
	signal reg0, reg1, reg2, reg3, cool0, cool1, cool2, cool3 : signed(31 downto 0);
	
begin
	
	process is
	begin
		wait until CLOCK_50 = '1';
		
		if reset = '1' then
			player_status <= "1100";
			cool0 <= (others => '0');
			cool1 <= (others => '0');
			cool2 <= (others => '0');
			cool3 <= (others => '0');
			reg0 <= (others => '0');
			reg1 <= (others => '0');
			reg2 <= (others => '0');
			reg3 <= (others => '0');
		else
			if cool0 > 0 then cool0 <= cool0 - 1; end if;
			if cool1 > 0 then cool1 <= cool1 - 1; end if;
			if cool2 > 0 then cool2 <= cool2 - 1; end if;
			if cool3 > 0 then cool3 <= cool3 - 1; end if;
			
			if (coil_status(0) = '0') or (cool0 > 0) then
				reg0 <= (others => '0');
			elsif (coil_status(0) = '1') and (reg0 < 12500000) then
				reg0 <= reg0 + 1;
			else
				cool0 <= X"00BEBC20";
				player_status(0) <= not player_status(0);
			end if;
			
			if (coil_status(1) = '0') or (cool1 > 0) then
				reg1 <= (others => '0');
			elsif (coil_status(1) = '1') and (reg1 < 12500000) then
				reg1 <= reg1 + 1;
			else
				cool1 <= X"00BEBC20";
				player_status(1) <= not player_status(1);
			end if;
			
			if (coil_status(2) = '0') or (cool2 > 0) then
				reg2 <= (others => '0');
			elsif (coil_status(2) = '1') and (reg2 < 12500000) then
				reg2 <= reg2 + 1;
			else
				cool2 <= X"00BEBC20";
				player_status(2) <= not player_status(2);
			end if;
			
			if (coil_status(3) = '0') or (cool3 > 0) then
				reg3 <= (others => '0');
			elsif (coil_status(3) = '1') and (reg3 < 12500000) then
				reg3 <= reg3 + 1;
			else
				cool3 <= X"00BEBC20";
				player_status(3) <= not player_status(3);
			end if;			
			
		end if;
		
	end process;
end architecture;