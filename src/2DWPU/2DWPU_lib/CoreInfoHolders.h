#pragma once
#include "Global.h"
#include "Registers.h"
#include "InstructionInfo.h"

namespace WPU2D
{
	using namespace Shared;

	namespace Core
	{
		/*	**************************************
					**** Statistics ****
			*************************************/	
		struct CoreStats
		{
			ull cycles,
				instructions,
				queries,
				returns,
				ioreads,
				iowrites,
				memreads,
				memwrites,

				parallel_forks,
				parallel_joins,
				parallel_failed_forks,
				parallel_failed_joins;
			void Reset() { *this = CoreStats(); }
		};


		/*	**************************************
				**** Program Block info ****
			*************************************/
		enum ProgramBlockInfoOffeset
		{
			offLMB = 8,
			offUMB = 12,
			offLIB = 16,
			offUIB = 18,
			offLCB = 20,
			offUCB = 24,
			offCRB_BS = 28,
			offCRB_XY = 31
		};

		struct ProgramBlockInfo
		{
			uint LMB, UMB;	// memory bounds
			ushort LIB, UIB;	// IO interface bounds
			uint LCB, UCB;	// call bounds
			regPC CRB;	// call return block
		};

		/*	**************************************
			  **** Double instruction info ****
			*************************************/

		enum DoublePhase
		{
			doubleNONE,
			double_FirstCall,
			double_FirstReturn,
			double_SecondCall,
			double_SecondReturn,
			double_Cleanup,
		};

		/*	**************************************
				**** Instruction stack info ****
			*************************************/

		class InstructionFlowWord
		{
			reg8 data;

		public:
			InstructionFlowWord() { data = 0; }
			InstructionFlowWord(reg8 raw) { data = raw; }
			InstructionFlowWord(SimpleDir dir, byte length, byte status, bool pdf = false)
			{
				RD(dir);
				RL(length);
				IS(status);
				PDF(pdf);
			}

			operator reg8()
			{
				return data;
			}

			InstructionFlowWord operator=(reg8 raw)
			{
				data = raw;
				return *this;
			}

			void RD(SimpleDir dir)
			{
				data &= 0x3FU;	// erase upper 2 bits
				data |= (dir << 6);	// store the direction
			}

			void RL(byte length)
			{
				length &= 0x03;	// allow only first two bits
				data &= 0xCFU;	// erase lower 2 bits of the upper nibble
				data |= (length << 4);	// store the length
			}

			void IS(byte status)
			{
				data &= 0xF8;	// erase lower 3 bits
				data |= status & 0x07;	// store the status there
			}

			void PDF(bool set)
			{
				data &= 0xF7U;	// erase the 4th bit
				data |= (set)?0x08U:0;
			}

			SimpleDir RD()
			{
				return (SimpleDir)(data >> 6);
			}

			byte RL()
			{
				return (data >> 4) & 0x03U;
			}

			byte IS()
			{
				return data & 0x07U;
			}

			bool PDF()
			{
				return (data & 0x08U) != 0;
			}
		};

	}
}