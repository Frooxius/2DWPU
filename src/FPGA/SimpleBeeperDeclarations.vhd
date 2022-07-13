library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

package simpleBeeperDeclarations is
		type beeperMem is array(63 downto 0) of STD_LOGIC_VECTOR(15 downto 0);
end package;