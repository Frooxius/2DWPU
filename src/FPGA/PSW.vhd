use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity PSW is
	port( clk, reset : in STD_LOGIC;
	
		DS3_S, DS2_S, DS1_S, DS0_S,
		PDF_S, DR_S, PCB_S, AC_S, SC_S, FSB_S,
		SB_S, CB_S : in STD_LOGIC;
		
		DS3_R, DS2_R, DS1_R, DS0_R,
		PDF_R, DR_R, PCB_R, AC_R, SC_R, FSB_R,
		SB_R, CB_R : in STD_LOGIC;
	
		DS3, DS2, DS1, DS0,
		PDF, DR, PCB, AC, SC, FSB,
		SB, CB : out STD_LOGIC );
end entity;

architecture default of PSW is
begin
	process is
		variable DS3_val, DS2_val, DS1_val, DS0_val,
		PDF_val, DR_val, PCB_val, AC_val, SC_val, FSB_val,
		SB_val, CB_val : STD_LOGIC := '0';
	begin
		wait until clk = '1';
		
		if reset = '1' then
			DS3_val := '0';
			DS2_val := '0';
			DS1_val := '0';
			DS0_val := '0';
			
			PDF_val := '0';
			DR_val := '0';
			PCB_val := '0';
			AC_val := '0';
			SC_val := '0';
			FSB_val := '0';
			SB_val := '0';
			CB_val := '0';
		else
		
		if DS3_S = '1' then DS3_val := '1'; end if;
		if DS2_S = '1' then DS2_val := '1'; end if;
		if DS1_S = '1' then DS1_val := '1'; end if;
		if DS0_S = '1' then DS0_val := '1'; end if;
		
		if PDF_S = '1' then PDF_val := '1'; end if;
		if DR_S  = '1' then DR_val  := '1'; end if;
		if PCB_S = '1' then PCB_val := '1'; end if;
		if AC_S  = '1' then AC_val  := '1'; end if;
		if SC_S  = '1' then SC_val  := '1'; end if;
		if FSB_S = '1' then FSB_val := '1'; end if;
		
		if SB_S  = '1' then SB_val  := '1'; end if;
		if CB_S  = '1' then CB_val  := '1'; end if;

		if DS3_R = '1' then DS3_val := '0'; end if;
		if DS2_R = '1' then DS2_val := '0'; end if;
		if DS1_R = '1' then DS1_val := '0'; end if;
		if DS0_R = '1' then DS0_val := '0'; end if;
		
		if PDF_R = '1' then PDF_val := '0'; end if;
		if DR_R  = '1' then DR_val  := '0'; end if;
		if PCB_R = '1' then PCB_val := '0'; end if;
		if AC_R  = '1' then AC_val  := '0'; end if;
		if SC_R  = '1' then SC_val  := '0'; end if;
		if FSB_R = '1' then FSB_val := '0'; end if;
		
		if SB_R  = '1' then SB_val  := '0'; end if;
		if CB_R  = '1' then CB_val  := '0'; end if;

		DS3 <= DS3_val;
		DS2 <= DS2_val;
		DS1 <= DS1_val;
		DS0 <= DS0_val;
		
		PDF <= PDF_val;
		DR  <= DR_val;
		PCB <= PCB_val;
		AC  <= AC_val;
		SC  <= SC_val;
		FSB <= FSB_val;
		
		SB  <= SB_val;
		CB  <= CB_val;
		
		end if;
		
	end process;
end architecture;