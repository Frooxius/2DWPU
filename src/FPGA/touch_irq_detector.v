// --------------------------------------------------------------------
// Copyright (c) 2005 by Terasic Technologies Inc. 
// --------------------------------------------------------------------
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use
//   in synthesis for all Terasic Development Boards and Altera Development 
//   Kits made by Terasic.  Other use of this code, including the selling 
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  Terasic provides no warranty regarding the use 
//   or functionality of this code.
//
// --------------------------------------------------------------------
//           
//                     Terasic Technologies Inc
//                     356 Fu-Shin E. Rd Sec. 1. JhuBei City,
//                     HsinChu County, Taiwan
//                     302
//
//                     web: http://www.terasic.com/
//                     email: support@terasic.com
//
// --------------------------------------------------------------------
//
// Major Functions:	
//
// --------------------------------------------------------------------
//
// Revision History :
// --------------------------------------------------------------------
//   Ver  :| Author            		:| Mod. Date :| Changes Made:
//   V1.0 :| Johnny Fan				:| 07/06/30  :| Initial Revision
// --------------------------------------------------------------------
module touch_irq_detector	(
					iCLK,
					iRST_n,
					iTOUCH_IRQ,
					iX_COORD,
					iY_COORD,
					iNEW_COORD,
					oDISPLAY_MODE,
					);
					
//============================================================================
// PARAMETER declarations
//============================================================================					
parameter	TOUCH_CNT_CLEAR = 24'hffffff;  // total photo numbers 
                    
//===========================================================================
// PORT declarations
//===========================================================================                      
input			iCLK;				// system clock 50Mhz
input			iRST_n;				// system reset
input			iTOUCH_IRQ;		
input	[11:0]	iX_COORD;			// X coordinate form touch panel
input	[11:0]	iY_COORD;			// Y coordinate form touch panel
input			iNEW_COORD;			// new coordinates indicate
output	[1:0]	oDISPLAY_MODE;		// displaed photo number
//=============================================================================
// REG/WIRE declarations
//=============================================================================
reg				touch_en;
reg				touch_en_clr;
reg 	[24:0] 	touch_delay_cnt;
reg 	[1:0] 	oDISPLAY_MODE;
//=============================================================================
// Structural coding
//=============================================================================

always@(posedge iCLK or negedge iRST_n)
	begin
		if (!iRST_n)
			touch_en <= 0;
		else if (touch_en_clr)
			touch_en <= 0;	
		else if (iTOUCH_IRQ)
		    touch_en <= 1;
	end
	
	
always@(posedge iCLK or negedge iRST_n)
	begin	
		if (!iRST_n)
			begin
				touch_delay_cnt <= 0;
				touch_en_clr <= 0;
			end		
		else if (touch_delay_cnt == TOUCH_CNT_CLEAR)
			begin
				touch_delay_cnt <= 0;
				touch_en_clr <= 1;
			end	
		else if (touch_en)
			touch_delay_cnt <= touch_delay_cnt + 1;
		else
			begin
				touch_delay_cnt <= 0;
				touch_en_clr <= 0;
			end	
	end					

always@(posedge iCLK or negedge iRST_n)
	begin	
		if (!iRST_n)
			oDISPLAY_MODE <= 0;
		else if (iTOUCH_IRQ && !touch_en)	
			oDISPLAY_MODE <= oDISPLAY_MODE + 1;
	end		


endmodule




								