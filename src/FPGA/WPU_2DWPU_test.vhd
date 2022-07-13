use work.declarations2DWPU.all;
library ieee; use ieee.std_logic_1164.all; use ieee.numeric_std.all;

entity WPU_2DWPU_Test is
	port( CLOCK_50 : in STD_LOGIC; KEY : in STD_LOGIC_VECTOR(3 downto 0); LEDR : out STD_LOGIC_VECTOR(17 downto 0); LEDG : out STD_LOGIC_VECTOR(7 downto 0);
		SW : in STD_LOGIC_VECTOR(17 downto 0);
		
		LTM_ADC_BUSY, LTM_ADC_DOUT, LTM_ADC_PENIRQ_n : in STD_LOGIC;
		LTM_ADC_DCLK, LTM_ADC_DIN, LTM_DEN, LTM_GREST, LTM_HD, LTM_NCLK, LTM_SCEN, LTM_VD : out STD_LOGIC;
		LTM_B, LTM_G, LTM_R : out STD_LOGIC_VECTOR(7 downto 0);
		LTM_SDA : inout STD_LOGIC;
		HEX0, HEX1, HEX2, HEX3, HEX4, HEX5, HEX6, HEX7 : out STD_LOGIC_VECTOR(6 downto 0);
		
		VGA_B, VGA_G, VGA_R : out STD_LOGIC_VECTOR(7 downto 0);
		VGA_BLANK_N, VGA_CLK, VGA_HS,	VGA_SYNC_N,	VGA_VS : out STD_LOGIC;
		
		EX_IO : inout STD_LOGIC_VECTOR(6 downto 0)
		);
end entity;

architecture default of WPU_2DWPU_Test is
	signal clk, CLOCK_40, reset, resetKey, beeper_wr : STD_LOGIC;
	signal outval, fps : STD_LOGIC_VECTOR(31 downto 0);
	
	signal LCDposx, LCDposy, ramVGAposx, ramVGAposy : STD_LOGIC_VECTOR(9 downto 0);
	signal VGAposx, VGAposy : STD_LOGIC_VECTOR(15 downto 0);
	signal LCDwr_x, LCDwr_y, LCDwr_color : STD_LOGIC_VECTOR(15 downto 0);
	signal LCD_Rin, LCD_Gin, LCD_Bin, VGA_Rin, VGA_Gin, VGA_Bin : STD_LOGIC_VECTOR(7 downto 0);
	signal LCDwr, LCDswap, ready, LCDsinglebuf : STD_LOGIC;
	
	signal coil_status, player_status : STD_LOGIC_VECTOR(3 downto 0);
	
	signal counter : STD_LOGIC_VECTOR(31 downto 0);
	
	signal progress0, progress1 : STD_LOGIC_VECTOR(15 downto 0);
	
	COMPONENT LTM_TOP
	PORT
	(
		CLOCK_50		:	 IN STD_LOGIC;
		KEY		:	 IN STD_LOGIC;
		posx		:	 OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
		posy		:	 OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
		Rin		:	 IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		Gin		:	 IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		Bin		:	 IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		touchX, touchY : OUT STD_LOGIC_VECTOR(11 downto 0);
		LTM_ADC_BUSY		:	 IN STD_LOGIC;
		LTM_ADC_DCLK		:	 OUT STD_LOGIC;
		LTM_ADC_DIN		:	 OUT STD_LOGIC;
		LTM_ADC_DOUT		:	 IN STD_LOGIC;
		LTM_ADC_PENIRQ_n		:	 IN STD_LOGIC;
		LTM_B		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		LTM_DEN		:	 OUT STD_LOGIC;
		LTM_G		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		LTM_GREST		:	 OUT STD_LOGIC;
		LTM_HD		:	 OUT STD_LOGIC;
		LTM_NCLK		:	 OUT STD_LOGIC;
		LTM_R		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		LTM_SCEN		:	 OUT STD_LOGIC;
		LTM_SDA		:	 INOUT STD_LOGIC;
		LTM_VD		:	 OUT STD_LOGIC;
		HEX0		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		HEX1		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		HEX2		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		HEX3		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		HEX4		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		HEX5		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		HEX6		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		HEX7		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0)
	);
END COMPONENT;

COMPONENT VGA_Ctrl
	PORT
	(
		iRed		:	 IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		iGreen		:	 IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		iBlue		:	 IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		oCurrent_X		:	 OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		oCurrent_Y		:	 OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
		oAddress		:	 OUT STD_LOGIC_VECTOR(21 DOWNTO 0);
		oRequest		:	 OUT STD_LOGIC;
		oVGA_R		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		oVGA_G		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		oVGA_B		:	 OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		oVGA_HS		:	 OUT STD_LOGIC;
		oVGA_VS		:	 OUT STD_LOGIC;
		oVGA_SYNC		:	 OUT STD_LOGIC;
		oVGA_BLANK		:	 OUT STD_LOGIC;
		oVGA_CLOCK		:	 OUT STD_LOGIC;
		iCLK		:	 IN STD_LOGIC;
		iRST_N		:	 IN STD_LOGIC
	);
END COMPONENT;

COMPONENT SEG7_LUT_8
	PORT
	(
		oSEG0		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		oSEG1		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		oSEG2		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		oSEG3		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		oSEG4		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		oSEG5		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		oSEG6		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		oSEG7		:	 OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
		iDIG		:	 IN STD_LOGIC_VECTOR(31 DOWNTO 0);
		ON_OFF		:	 IN STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
END COMPONENT;

	signal io_wr, io_rd, io_rdy, io_out_wr, io_out_rd : STD_LOGIC;
	signal store_val : STD_LOGIC_VECTOR(31 downto 0);
	signal io_addr, io_out_addr, io_out_val, io_bus, io_read_val : STD_LOGIC_VECTOR(15 downto 0);
	
	signal touchX, touchY : STD_LOGIC_VECTOR(11 downto 0);
	
begin
	resetKey <= not KEY(0);

	GlobReset : entity work.ResetDelay(default)
	port map ( clk, resetKey, reset);

	PLLclock : entity work.PLL(SYN)
	port map( CLOCK_50, clk);
	
	PLLclock40 : entity work.PLL40Mhz(SYN)
	port map( CLOCK_50, CLOCK_40);
	
	Processor : entity work.WPU_2DWPU(default)
	port map( clk, reset, store_val, io_addr, io_wr, io_rd, io_rdy, io_read_val, SW(work.Parallel2DWPU.cores), open);
	
	IOinterface : entity work.IOInterface(default)
	port map( clk, io_wr, io_rd, io_rdy, io_addr, store_val(15 downto 0), io_bus, io_out_addr, io_out_val, io_read_val, io_out_wr, io_out_rd);
	
	IO_LCDcontroller : entity work.IO_LCDcontroller(default)
	port map( clk, reset, io_out_wr, io_out_rd, X"0000", io_out_addr, io_out_val, io_bus, LCDwr_x, LCDwr_y, LCDwr_color, touchX, touchY, ready, LCDwr, LCDswap, LCDsinglebuf);
	
	IO_random : entity work.randomIOdev
	port map( clk, X"000A", io_out_addr, io_out_rd, io_bus);
	
	LCDInteface : entity work.DisplayController(default)
		port map( clk, CLOCK_50, reset, LCDposx, LCDwr_x(9 downto 0), LCDposy, LCDwr_y(9 downto 0), LCDwr, LCDswap, '0', LCDsinglebuf, LCDwr_color(2) and SW(17), LCDwr_color(1) and SW(16), LCDwr_color(0) and SW(15), LCD_Rin, LCD_Gin, LCD_Bin, ready);
		
	VGAInteface : entity work.DisplayController(default)
		port map( clk, CLOCK_40, reset, ramVGAposx, LCDwr_x(9 downto 0), ramVGAposy, LCDwr_y(9 downto 0), LCDwr, LCDswap, '0', LCDsinglebuf, LCDwr_color(2) and SW(17), LCDwr_color(1) and SW(16), LCDwr_color(0) and SW(15), VGA_Rin, VGA_Gin, VGA_Bin, open);
		
	ProgressReg0 : entity work.IOregister(default)
		port map( clk, reset, io_out_wr, io_out_rd, X"0010", io_out_addr, io_out_val, io_bus, progress0);
	ProgressReg1 : entity work.IOregister(default)
		port map( clk, reset, io_out_wr, io_out_rd, X"0011", io_out_addr, io_out_val, io_bus, progress1);
	
	LCDcontroller : LTM_TOP
		port map (CLOCK_50, KEY(0), LCDposx, LCDposy, LCD_Rin, LCD_Gin, LCD_Bin, touchX, touchY,
		LTM_ADC_BUSY,
       LTM_ADC_DCLK,
       LTM_ADC_DIN,
       LTM_ADC_DOUT,
       LTM_ADC_PENIRQ_n,
       LTM_B,
       LTM_DEN,
       LTM_G,
       LTM_GREST,
       LTM_HD,
       LTM_NCLK,
       LTM_R,
       LTM_SCEN,
       LTM_SDA,
       LTM_VD,
		 open, open, open, open, open, open, open, open);
		 
	VGAdisplay : entity work.VGA800x600withProgress(default)
		port map(CLOCK_50, VGAposx, VGAposy, ramVGAposx, ramVGAposy, VGA_Rin, VGA_Gin, VGA_Bin, VGA_R, VGA_G, VGA_B, progress0, progress1 );
		 
	VGAcontroller : VGA_Ctrl
		port map (
			X"00", X"00", X"00", VGAposx, VGAposy, open, open, open, open, open,
			VGA_HS, VGA_VS, VGA_SYNC_N, VGA_BLANK_N, VGA_CLK, CLOCK_40, KEY(0)
		);
		 
	FPScounter : entity work.FrameCounter
		port map( clk, LCDswap, fps);
		
	SEGDISP : SEG7_LUT_8
		port map (HEX0, HEX1, HEX2, HEX3, HEX4, HEX5, HEX6, HEX7, fps, "11111111");
		
	Beeper : entity work.SimpleBeeper(default)
		port map( clk, reset, beeper_wr, io_out_val, EX_IO(3));
		
	BeeperIO : entity work.IOdeviceWrapper
		port map ( X"0020", io_out_addr, io_out_wr, '0', beeper_wr, open, X"FFFF", open );
		
	MagControl : entity work.MagController
	 port map ( CLOCK_50, reset, coil_status, player_status );
	 
	--coil_status(0) <= EX_IO(0);
	--coil_status(1) <= not EX_IO(1);
	--coil_status(2) <= not EX_IO(2);
	--coil_status(3) <= EX_IO(5);
	
	coil_status(0) <= SW(0) or EX_IO(0);
	coil_status(1) <= SW(1) or (not EX_IO(1));
	coil_status(2) <= SW(2) or (not EX_IO(2));
	coil_status(3) <= SW(3)	or EX_IO(5);
	
	LEDG(3 downto 0) <= coil_status;
	LEDR(3 downto 0) <= player_status;
	 
	MagControlIO : entity work.IOdeviceWrapper
		port map (X"0030", io_out_addr, '0', io_out_rd, open, open, X"000" & coil_status, io_bus);
		 
	process is
	begin
		wait until clk = '1';
		
		counter <= STD_LOGIC_VECTOR(unsigned(counter) + 1);

	end process;
	
end architecture;