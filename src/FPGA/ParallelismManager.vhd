use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity ParallelismManager is
	port(	clk, reset : in STD_LOGIC;
	
		xPC, yPC : inout work.Parallel2DWPU.axisPCconnect;
		ARG, PO, PE : inout work.Parallel2DWPU.reg32connect;
		rqst, acpt, redy : out work.Parallel2DWPU.bitConnect;
		fork, ftch, free : in work.Parallel2DWPU.bitConnect;
		
		core_en : in work.Parallel2DWPU.bitConnect;
		core_active : out work.Parallel2DWPU.bitConnect
	);
end entity;

architecture default of ParallelismManager is

	signal fetchend : work.Parallel2DWPU.bitConnect;
	signal juststarted : work.Parallel2DWPU.bitConnect; -- is the core processing anything?
	signal result : work.Parallel2DWPU.resultsStore;	-- stored results
	signal done : work.Parallel2DWPU.doneStore;			-- if the result is ready
	
	signal forID : work.Parallel2DWPU.forIDstore;	-- for what core is it calculating the result?
	signal resultID, resultTop : work.Parallel2DWPU.resultIDstore;

begin
	
	process is
		variable break : boolean;
		variable processing, cooldown, cooldown2 : work.Parallel2DWPU.bitConnect;
	begin
		wait until clk = '1';
		
		-- Monostable signals		
		acpt <= (others => '0');
		redy <= (others => '0');
		
		for i in work.Parallel2DWPU.cores loop
			ARG(i) <= (others => 'Z');
			PO(i) <= (others => 'Z');
			PE(i) <= (others => 'Z');
			xPC(i) <= (others => 'Z');
			yPC(i) <= (others => 'Z');
			rqst(i) <= '0';
			
			core_active(i) <= processing(i);
		end loop;
		
		if reset = '1' then
			for i in work.Parallel2DWPU.cores loop
				if i /= 0 then
					processing(i) := '0';
				else
					processing(i) := '1';
				end if;
				
				juststarted(i) <= '0';
				cooldown(i) := '0';
				cooldown2(i) := '0';
				
				for n in work.Parallel2DWPU.results loop
					done(i)(n) <= '0';
				end loop;
				
				resultTop(i) <= 0;
				fetchend(i) <= '0';
				
				break := false;
				
			end loop;
		else
		
			-- scan if any core needs to cool downto
			for i in work.Parallel2DWPU.cores loop
				if cooldown2(i) = '1' then
					cooldown2(i) := '0';
				elsif cooldown(i) = '1' then
					cooldown(i) := '0';
				end if;
			end loop;
			
			-- scan if any of the cores has result ready
			for i in work.Parallel2DWPU.cores loop
				if (processing(i) = '1') and (free(i) = '1') and (juststarted(i) = '0') then
					result(forID(i))(resultID(i)) <= ARG(i);
					done(forID(i))(resultID(i)) <= '1';
					
					processing(i) := '0';
					cooldown(i) := '1';
					cooldown2(i) := '1';
				end if;
				
				if juststarted(i) = '1' then
					juststarted(i) <= '0';
				end if;
			end loop;
			
			-- now check if any of the cores needs some calculation
			for i in work.Parallel2DWPU.cores loop
				if fork(i) = '1' then
					-- -------------------- ASSIGN TASK --------------------
					if resultTop(i) /= work.Parallel2DWPU.results'high then
						-- find some free core
						break := false;
						for n in work.Parallel2DWPU.cores loop
							if (free(n) = '1') and (processing(n) = '0') and (cooldown(n) = '0') and (not break) and (core_en(n) = '1') then
								-- found a free core, assign it a task
								ARG(n) <= ARG(i);
								xPC(n) <= xPC(i);
								yPC(n) <= yPC(i);
								PO(n) <= PO(i);
								PE(n) <= PE(i);
								rqst(n) <= '1';
								
								-- store information about the new core
								forID(n) <= i;
								resultID(n) <= resultTop(i);
								processing(n) := '1';
								juststarted(n) <= '1';
								
								-- make space for the result
								done(i)(resultTop(i)) <= '0';
								resultTop(i) <= resultTop(i) + 1;
								
								-- notify the core that it was accepted
								acpt(i) <= '1';
								
								break := true;
							end if;
						end loop;
					end if;						
					-- -------------------- ASSIGN TASK --------------------
				end if;
				
				if (ftch(i) = '1') and (fetchend(i) = '0') then
					-- -------------------- GIVE RESULT --------------------
					if done(i)(resultTop(i)-1) = '1' then
						redy(i) <= '1';
						ARG(i) <= result(i)(resultTop(i)-1);
						fetchend(i) <= '1';
					end if;
					-- -------------------- GIVE RESULT --------------------
				end if;
				
				if (ftch(i) = '0') and (fetchend(i) = '1') then
					resultTop(i) <= resultTop(i)-1;
					fetchend(i) <= '0';
				end if;
			end loop;
			
		end if;
		
	end process;

end architecture;