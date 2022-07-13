library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

package Parallel2DWPU is
	subtype cores is integer range 9 downto 0;
	subtype results is integer range 9 downto 0;
	
	type axisPCconnect is array(cores) of STD_LOGIC_VECTOR(4 downto 0);
	type reg32connect is array(cores) of STD_LOGIC_VECTOR(31 downto 0);
	subtype bitConnect is STD_LOGIC_VECTOR(cores);
	
	type coreResults is array(results) of STD_LOGIC_VECTOR(31 downto 0);
	type resultsStore is array(cores) of coreResults;
	
	type coreDone is array(results) of STD_LOGIC;
	type doneStore is array(cores) of coreDone;
	
	type forIDstore is array(cores) of cores;
	type resultIDstore is array(cores) of results;
end package;