#pragma once
#include "Global.h"
#include "InstructionInfo.h"

namespace WPU2D
{
	namespace Shared
	{
		class DecodedInstr
		{
			// Methods
			void DecodeInstruction(reg16 instr);

		public:
			bool halt;	// is it half instruction?

			InstrBase baseType;

			// index of specific instruction - type
			ushort index;

			// direction and its length
			byte direction, dir_length;
			bool long_dir_second;	// which direction is the long one?

			// type of passtrough instruction
			InstrPasstrough passType;

			// type of index instruction
			InstrIndex indexType;
			bool AP;	// argument protect
			InstrIndexSub indexSubtype;	// subtype of index instruction
			bool retTAK;	// if it's a take one

			// make a clean instruction
			void CleanInit()
			{
				baseType = insBaseNONE;
				indexType = insIndexNONE;
				passType = insPassNONE;
				indexSubtype.raw = 0;
				halt = false;
				retTAK = false;

				index = direction = dir_length =
				long_dir_second = AP = 0;
			}

			DecodedInstr()
			{
				CleanInit();
			}	

			DecodedInstr(reg16 raw)
			{
				CleanInit();
				DecodeInstruction(raw);
			}

			// Public methods
			QueryDir DecodeDirection(bool second);
			bool SingleDirection();
			reg16 EncodeInstruction();	// returns opcode
		};
	}
}