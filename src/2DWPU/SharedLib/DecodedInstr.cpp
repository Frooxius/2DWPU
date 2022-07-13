#include "DecodedInstr.h"

namespace WPU2D
{
	namespace Shared
	{
		/*
			Decodes an instruction from the raw data
		*/
		void DecodedInstr::DecodeInstruction(reg16 instr)
		{
			if(instr == 0xFFFFU)
			{
				halt = true;
				return;
			}

			baseType = (InstrBase)((instr & 0xC000U) >> 14); // decode the base type
			
			// Further decoding is specific to each type
			switch(baseType)
			{
			case insBaseQuery:
			case insBaseOperation:
				// decode the direction
				direction = (instr & 0x3E00U) >> 9;
				dir_length = (instr & 0x0180U) >> 7;
				long_dir_second = ((instr & 0x0040U) >> 6) != 0;

				// decode the specific type (index)
				index = instr & 0x003FU;

				break;

			case insBasePasstrough:
				passType = (InstrPasstrough)((instr & 0x3000U) >> 12);
				direction = 25 + ((instr & 0x0380U) >> 7);
				dir_length = (instr & 0x0C00U) >> 10;
				long_dir_second = false;
				index = (instr & 0x007FU);

				break;

			case insBaseIndex:
				indexType = (InstrIndex)((instr & 0x3000U) >> 12);
				index = instr & 0x03FFU;

				switch(indexType)
				{
				case insIndexReturn:
					indexSubtype.returnType =
						(InstrIndexReturn)((instr & 0x0C00U) >> 10);

					// determine if it's TAKe
					retTAK = index&0x0200U;
					index &= ~0x0200U;

					break;
				case insIndexJump:
					indexSubtype.jumpUpper2b = (instr & 0x0C00U) >> 10;
					break;

				case insIndexDouble:
					indexSubtype.doubleType = 
						(InstrIndexDouble)((instr & 0x0800U) >> 11);
					AP = ((instr & 0x0400U) >> 10) != 0;
					break;

				case insIndexExtend:
					// Nothing?!
					break;
				}

				break;
			}
		}

		// Decodes single direction
		QueryDir DecodedInstr::DecodeDirection(bool second)
		{
			return QueryDir(
				(second)?
				((SimpleDir) (direction / 5)) :
				((SimpleDir) (direction % 5)),
					(second ^ long_dir_second)?
					0 : (dir_length) );
		}

		bool DecodedInstr::SingleDirection()
		{
			// detection of single direction, based on documentation
			return ((direction&0x18) == 0x18) && (direction&0x07);
		}

		reg16 DecodedInstr::EncodeInstruction()
		{
			if(halt)
				return (reg16)0xFFFFU;

			reg16 opcode = 0;
			
			// write the baseType
			opcode |= baseType << 14;

			switch(baseType)
			{
			case insBaseQuery:
			case insBaseOperation:
				// encode the direction
				opcode |= direction << 9;
				opcode |= dir_length << 7;
				opcode |= ((reg16)long_dir_second) << 6;

				// store the index (type)
				opcode |= index;
				break;

			case insBasePasstrough:
				// the subtype
				opcode |= passType << 12;
				// direction
				opcode |= dir_length << 10;
				opcode |= (direction%5) << 7;
				// type of action
				opcode |= index;				
				break;

			case insBaseIndex:
				// subtype
				opcode |= indexType << 12;

				switch(indexType)
				{
				case insIndexReturn:
					opcode |= ((reg16)retTAK) << 9;
						// fall trough intentionally					
				case insIndexJump:
					opcode |= indexSubtype.raw << 10;
					break;

				case insIndexDouble:
					opcode |= indexSubtype.doubleType << 11;
					opcode |= AP << 10;
					break;
				}

				// now the index key
				opcode |= index;			

				break;
			}

			return opcode;	// we're done :3
		}
	}
}