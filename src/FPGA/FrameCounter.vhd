use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity FrameCounter is
	port ( clk, swap : in STD_LOGIC;
		fps : out STD_LOGIC_VECTOR(31 downto 0)
	);
end entity;

architecture default of FrameCounter is
	signal count : STD_LOGIC_VECTOR(31 downto 0);
	signal frames : STD_LOGIC_VECTOR(7 downto 0);
	
	function to_bcd ( bin : std_logic_vector(7 downto 0) ) return std_logic_vector is
variable i : integer:=0;
variable bcd : std_logic_vector(11 downto 0) := (others => '0');
variable bint : std_logic_vector(7 downto 0) := bin;

begin
for i in 0 to 7 loop  -- repeating 8 times.
bcd(11 downto 1) := bcd(10 downto 0);  --shifting the bits.
bcd(0) := bint(7);
bint(7 downto 1) := bint(6 downto 0);
bint(0) :='0';


if(i < 7 and bcd(3 downto 0) > "0100") then --add 3 if BCD digit is greater than 4.
bcd(3 downto 0) := std_LOGIC_VECTOR(unsigned(bcd(3 downto 0)) + 3);
end if;

if(i < 7 and bcd(7 downto 4) > "0100") then --add 3 if BCD digit is greater than 4.
bcd(7 downto 4) := std_LOGIC_VECTOR(unsigned(bcd(7 downto 4)) + 3);
end if;

if(i < 7 and bcd(11 downto 8) > "0100") then  --add 3 if BCD digit is greater than 4.
bcd(11 downto 8) := std_LOGIC_VECTOR(unsigned(bcd(11 downto 8)) + 3);
end if;

end loop;
return bcd;

end function;
	
begin

	process is
	begin
		wait until clk = '1';
		
		if unsigned(count) = 15000000 then
			fps <= X"00000" & to_bcd(frames);
			frames <= X"00";
			count <= X"00000000";
		else
		
			if swap = '1' then
				frames <= STD_LOGIC_VECTOR(unsigned(frames) + 1);
			end if;
			count <= STD_LOGIC_VECTOR(unsigned(count) + 1);
			
		end if;
	end process;
end architecture;