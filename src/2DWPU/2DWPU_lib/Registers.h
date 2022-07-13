#pragma once
#include "Global.h"
#include "2DWPUconfig.h"
#include "InstructionInfo.h"

namespace WPU2D
{
	namespace Core
	{
		struct regPC
		{
			reg5 xPC, yPC;
			reg24 BS;

			void Move(Shared::QueryDir dir)
			{
				switch(dir.dir)
				{
				case Shared::sdir_U:
					yPC -= dir.length+1;
					break;
				case Shared::sdir_R:
					xPC += dir.length+1;
					break;
				case Shared::sdir_D:
					yPC += dir.length+1;
					break;
				case Shared::sdir_L:
					xPC -= dir.length+1;
					break;
				}
			}
		};

		// this struct describes the shared portion of the SW register
		struct regS_SW
		{
			reg16 raw;	// raw data

			// reading bits
			bool UB0() { return (raw & 0x01) != 0; }
			bool UB1() { return (raw & 0x02) != 0; }
			bool UB2() { return (raw & 0x04) != 0; }
			bool UB3() { return (raw & 0x08) != 0; }

			// setting bits
			void UB0(bool set) { SetBit(&raw, 0, set); }
			void UB1(bool set) { SetBit(&raw, 1, set); }
			void UB2(bool set) { SetBit(&raw, 2, set); }
			void UB3(bool set) { SetBit(&raw, 3, set); }
		};

		// this struct describes the public portion of SW register
		struct regP_SW
		{
			reg16 raw;

			bool CB() { return	(raw & 0x0001) != 0; }
			bool SB() { return	(raw & 0x0002) != 0; }
			bool FSB() { return (raw & 0x0020) != 0; }
			bool SC() { return	(raw & 0x0040) != 0; }
			bool AC() { return  (raw & 0x0080) != 0; }
			bool PCB() { return (raw & 0x0100) != 0; }
			bool DR() { return (raw & 0x0200) != 0; }
			bool PDF() { return (raw & 0x0400) != 0; }
			uint DS() { return (raw & 0xF000) >> 12; }

			void CB(bool set) { SetBit(&raw, 0, set); }
			void SB(bool set) { SetBit(&raw, 1, set); }
			void FSB(bool set) { SetBit(&raw, 5, set); }
			void SC(bool set) { SetBit(&raw, 6, set); }
			void AC(bool set) { SetBit(&raw, 7, set); }
			void PCB(bool set) { SetBit(&raw, 8, set); }
			void DR(bool set) { SetBit(&raw, 9, set); }
			void PDF(bool set) { SetBit(&raw, 10, set); }
			void DS(uint val) {
				raw &= ~0xF000U;	// erase the three bits
				raw |= (val << 12) & 0xF00U;
			}
		};

		struct GlobalRegisters
		{
			// shared portion of status word
			regS_SW S_SW;
			// work registers
			reg32 LI, SQ, RE, TR, CI, HE, ST, EL;

			reg32 IC;	// interrupt control
			reg32 TC, TV[TIMERS], TF[TIMERS]; // timers
		};

		struct PrivateRegisters
		{
			reg32 ARG;
			reg4 IS;

			regP_SW P_SW; // private portion of status word
			reg32 PO, PE; // work registers

			// program counter related
			regPC PC;

			// stack related
			reg16 SI, SA;
		};

		enum QueryRegistersOrder
		{
			queryRegARG,
			queryRegPTR_ST,
			queryRegPTR_HE,
			queryReg_ST_PO,
			queryRegLI,
			queryRegSQ,
			queryRegRE,
			queryRegTR,
			queryRegCI,
			queryRegPE,
			queryRegHE,
			queryRegST,
			queryRegEL,
			queryRegPO
		};

		enum QueryBitOrder
		{
			queryBitUB0,
			queryBitUB1,
			queryBitUB2,
			queryBitUB3,

			queryBitTO0,
			queryBitTO1,
			queryBitTO2,
			queryBitTO3,

			queryBitCB,
			queryBitIQ,

			queryBit_reserved0,
			queryBit_reserved1,
		};

		enum PasstroughMOVRegOrder
		{
			passMOV_LI,
			passMOV_SQ,
			passMOV_RE,
			passMOV_TR,
			passMOV_CI,
			passMOV_PE,
			passMOV_HE,
			passMOV_ST,
			passMOV_EL,
			passMOV_PO,
			passMOV_IC,
			passMOV_IB,
			passMOV_SW,
			passMOV_BS,
			passMOV_SI,
			passMOV_SA,
			passMOV_TC,
			passMOV_TV0,
			passMOV_TV1,
			passMOV_TV2,
			passMOV_TV3,
			passMOV_TF0,
			passMOV_TF1,
			passMOV_TF2,
			passMOV_TF3
		};

		enum PasstroughSTACKRegOrder
		{
			passSTACK_ARG,
			passSTACK_SW,
			passSTACK_LI,
			passSTACK_SQ,
			passSTACK_RE,
			passSTACK_TR,
			passSTACK_CI,
			passSTACK_EL,
			passSTACK_HE,
			passSTACK_ST,
			passSTACK_PE,
			passSTACK_PO,
			passSTACK_ptrST
		};
	}
}
