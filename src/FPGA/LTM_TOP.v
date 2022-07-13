module LTM_TOP(
       CLOCK_50,
       KEY,
		 posx,
		 posy,
		 Rin,
		 Gin,
		 Bin,
		 touchX,
		 touchY,
		 
       ////////////LTM_GPIO port
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
       HEX0,							//	Seven Segment Digit 0
		 HEX1,							//	Seven Segment Digit 1
	    HEX2,							//	Seven Segment Digit 2
	    HEX3,							//	Seven Segment Digit 3
	    HEX4,							//	Seven Segment Digit 4
	    HEX5,							//	Seven Segment Digit 5
	    HEX6,							//	Seven Segment Digit 6
	    HEX7 							//	Seven Segment Digit 7
       );
///port declaration
output [9:0] posx;
output [9:0] posy;
input [7:0] Rin;
input [7:0] Gin;
input [7:0] Bin;
output [11:0] touchX, touchY;

input                      CLOCK_50;
input                      KEY;
input		          		 	LTM_ADC_BUSY;
output		          		LTM_ADC_DCLK;
output		          		LTM_ADC_DIN;
input		          			LTM_ADC_DOUT;
input		          			LTM_ADC_PENIRQ_n;
output		     [7:0]		LTM_B;
output		          		LTM_DEN;
output		     [7:0]		LTM_G;
output		          		LTM_GREST;
output		          		LTM_HD;
output		          		LTM_NCLK;
output		     [7:0]		LTM_R;
output		          		LTM_SCEN;
inout		          			LTM_SDA;
output		          		LTM_VD;
///
output	[6:0]	HEX0;					//	Seven Segment Digit 0
output	[6:0]	HEX1;					//	Seven Segment Digit 1
output	[6:0]	HEX2;					//	Seven Segment Digit 2
output	[6:0]	HEX3;					//	Seven Segment Digit 3
output	[6:0]	HEX4;					//	Seven Segment Digit 4
output	[6:0]	HEX5;					//	Seven Segment Digit 5
output	[6:0]	HEX6;					//	Seven Segment Digit 6
output	[6:0]	HEX7;					//	Seven Segment Digit 7
///wire reg declaration
reg             div;
wire            adc_dclk;
wire            ltm_3wirebusy_n;
wire            ltm_sclk;
wire            touch_irq;
wire    [2:0]   display_mode;
wire    [11:0]  x_coord,y_coord;
wire	  [11:0]	 touchX, touchY;
wire            new_coord;


//////////assignments

assign touchX = 400 - (y_coord / 10);
assign touchY = 240 - (x_coord / 17);

assign LTM_ADC_DCLK	= ( adc_dclk & ltm_3wirebusy_n )  |  ( ~ltm_3wirebusy_n & ltm_sclk );
assign LTM_NCLK = div; // 25 Mhz
assign LTM_GREST = KEY;
always @(posedge CLOCK_50)
	begin
		div <= !div;
	end

SEG7_LUT_8 		u1	(	
					.oSEG0(HEX0),			
					.oSEG1(HEX1),	
					.oSEG2(HEX2),	
					.oSEG3(HEX3),	
					.oSEG4(HEX4),	
					.oSEG5(HEX5),	
					.oSEG6(HEX6),	
					.oSEG7(HEX7),	
					.iDIG({ 4'h0 , x_coord , 4'h0 , y_coord }),
					.ON_OFF(8'b01110111) 
					);

// lcd 3 wire interface configuration  //
lcd_spi_cotroller	u2	(	
					// Host Side
					.iCLK(CLOCK_50),
					.iRST_n(DLY0),
					// 3wire Side
					.o3WIRE_SCLK(ltm_sclk),
					.io3WIRE_SDAT(LTM_SDA),
					.o3WIRE_SCEN(LTM_SCEN),
					.o3WIRE_BUSY_n(ltm_3wirebusy_n)
					);	

// system reset  //
Reset_Delay		u3  (.iCLK(CLOCK_50),
					.iRST(KEY),
					.oRST_0(DLY0),
					.oRST_1(DLY1),
					.oRST_2(DLY2)
					);
// Touch Screen Digitizer ADC configuration //
adc_spi_controller		u4	(
					.iCLK(CLOCK_50),
					.iRST_n(DLY0),
					.oADC_DIN(LTM_ADC_DIN),
					.oADC_DCLK(adc_dclk),
					.oADC_CS(),
					.iADC_DOUT(LTM_ADC_DOUT),
					.iADC_BUSY(LTM_ADC_BUSY),
					.iADC_PENIRQ_n(LTM_ADC_PENIRQ_n),
					.oTOUCH_IRQ(touch_irq),
					.oX_COORD(x_coord),
					.oY_COORD(y_coord),
					.oNEW_COORD(new_coord)
					);

touch_irq_detector	u5	(
					.iCLK(CLOCK_50),
					.iRST_n(DLY0),
					.iTOUCH_IRQ(touch_irq),
					.iX_COORD(x_coord),
					.iY_COORD(y_coord),
					.iNEW_COORD(new_coord),
					.oDISPLAY_MODE(display_mode)
					);

lcd_timing_controller		u6  ( 
					.iCLK(div),
					.iRST_n(DLY2),
					// lcd side
					.oLCD_R(LTM_R),
					.oLCD_G(LTM_G),
					.oLCD_B(LTM_B), 
					.oHD(LTM_HD),
					.oVD(LTM_VD),
					.oDEN(LTM_DEN),
					.iDISPLAY_MODE(display_mode),
					.posx(posx),
					.posy(posy),
					.Rin(Rin),
					.Gin(Gin),
					.Bin(Bin)
					);

endmodule
