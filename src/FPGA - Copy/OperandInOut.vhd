use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity OperandInOut is
	port( clk, reset, load, store, inc, dec, wr_block : in STD_LOGIC;
	
			operandReady : out STD_LOGIC;
	
			ARG, LI, SQ, RE, TR, CI, PE, HE, ST, EL, PO, index_val : in STD_LOGIC_VECTOR(31 downto 0);
			
			ARG_wr, LI_wr, SQ_wr, RE_wr, TR_wr, CI_wr, PE_wr, HE_wr, ST_wr, EL_wr, PO_wr : out STD_LOGIC;
			store_val : out STD_LOGIC_VECTOR(31 downto 0);
			
			addr_offset : in STD_LOGIC_VECTOR(7 downto 0);
			
			selectReg : in register2DWPU;
			selectMem : in dataLocation;
			
			-- Memory interface
			mem_addr : out STD_LOGIC_VECTOR(23 downto 0);
			mem_wr, mem_rd : out STD_LOGIC;
			mem_rdy : in STD_LOGIC;
			mem_in : in STD_LOGIC_VECTOR(31 downto 0)
			
			);
end entity;

architecture default of OperandInOut is
	signal loadbusy, storebusy, reg_wr : STD_LOGIC;
	signal regPick, zero, mem_baseaddr, mem_calcaddr : STD_LOGIC_VECTOR(31 downto 0);
begin

	with selectReg select
	regPick <=
		zero 	when regNONE,
		ARG  	when regARG,
		LI		when regLI,
		SQ		when regSQ,
		RE		when regRE,
		TR		when regTR,
		CI		when regCI,
		PE		when regPE,
		HE		when regHE,
		ST		when regST,
		EL		when regEL,
		PO		when regPO,
		zero	when others;
		
	--ARG_wr <= '1' when selectReg = regARG 	and reg_wr = '1' else '0';
	LI_wr <= '1' when selectReg = regLI 	and reg_wr = '1' else '0';
	SQ_wr <= '1' when selectReg = regSQ 	and reg_wr = '1' else '0';
	RE_wr <= '1' when selectReg = regRE 	and reg_wr = '1' else '0';
	TR_wr <= '1' when selectReg = regTR 	and reg_wr = '1' else '0';
	CI_wr <= '1' when selectReg = regCI 	and reg_wr = '1' else '0';
	PE_wr <= '1' when selectReg = regPE 	and reg_wr = '1' else '0';
	HE_wr <= '1' when selectReg = regHE 	and reg_wr = '1' else '0';
	ST_wr <= '1' when selectReg = regST 	and reg_wr = '1' else '0';
	EL_wr <= '1' when selectReg = regEL 	and reg_wr = '1' else '0';
	PO_wr <= '1' when selectReg = regPO 	and reg_wr = '1' else '0';
		
	with selectMem select
	mem_baseaddr <=
		ST when locMemST,
		index_val when locMemIndex,
		zero when others;

	mem_calcaddr <= STD_LOGIC_VECTOR(signed(mem_baseaddr) + signed(regPick) + signed(addr_offset));
	mem_addr <= mem_calcaddr(23 downto 0);
	
	process is
	begin
		wait until clk = '1';
		
		if reset = '1' then
			operandReady <= '0';
			store_val <= X"00000000";
			mem_rd <= '0';
			zero <= X"00000000";
			loadbusy <= '0';
			storebusy <= '0';
			reg_wr <= '0';
			mem_wr <= '0';
			mem_rd <= '0';
			ARG_wr <= '0';
			
		elsif loadbusy = '1' then
		
			if mem_rdy = '1' then
				loadbusy <= '0';
				mem_rd <= '0';
				operandReady <= '1';
				store_val <= mem_in;
				ARG_wr <= '1' and (not wr_block);
			end if;
			
		elsif storebusy = '1' then
		
			if mem_rdy = '1' then
				storebusy <= '0';
				mem_wr <= '0';
				operandReady <= '1';
			end if;
			
		elsif load = '1' then
		
			if selectMem = locReg then
				store_val <= regPick;
				ARG_wr <= '1' and (not wr_block);
				operandReady <= '1';
			elsif selectMem = locMemST or selectMem = locMemIndex then
				loadbusy <= '1';
				mem_rd <= '1';
			end if;
			
		elsif store = '1' then
			
			if selectMem = locReg then
				operandReady <= '1';
				store_val <= ARG;
				reg_wr <= '1';
			elsif selectMem = locMemST or selectMem = locMemIndex then
				storebusy <= '1';
				store_val <= ARG;
				mem_wr <= '1';				
			end if;
				
		else
			operandReady <= '0';
			reg_wr <= '0';
			ARG_wr <= '0';
			
		end if;
	end process;
end architecture;