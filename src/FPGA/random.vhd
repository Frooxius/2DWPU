library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- from http://vhdlguru.blogspot.com/2010/03/random-number-generator-in-vhdl.html

--entity random is
--    generic ( width : integer :=  32 );
--port (
--      clk : in std_logic;
--      random_num : out std_logic_vector (width-1 downto 0)   --output vector           
--    );
--end random;
--
--architecture Behavioral of random is
--begin
--process(clk)
--variable rand_temp : std_logic_vector(width-1 downto 0):=(width-1 => '1',others => '0');
--variable temp : std_logic := '0';
--begin
--if(rising_edge(clk)) then
--temp := rand_temp(width-1) xor rand_temp(width-2);
--rand_temp(width-1 downto 1) := rand_temp(width-2 downto 0);
--rand_temp(0) := temp;
--end if;
--random_num <= rand_temp;
--end process;
--end architecture;

ENTITY random IS
  PORT (
    clk           : IN STD_LOGIC;
    noise      : OUT STD_Logic_vector(15 downto 0));
END;

ARCHITECTURE default OF random IS
  SIGNAL lfsr     :  STD_Logic_vector(17 DOWNTO 0) := "000000000000000000";

BEGIN
  noise <= lfsr(15 downto 0);

  PROCESS
  BEGIN
    WAIT UNTIL (clk'EVENT AND clk = '1');
    lfsr <= lfsr(16 DOWNTO 0) & (lfsr(17) XNOR lfsr(10));
  END PROCESS;

END;
