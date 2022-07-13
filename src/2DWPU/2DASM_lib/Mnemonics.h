#pragma once
#include "Global.h"
#include "DecodedInstr.h"
#include <string>

namespace WPU2D
{
	using namespace Shared;

	namespace ASM2D
	{
		extern char *QueryRegisterOperands[];
		extern char *QueryBitOperands[];
		extern char *SimpleDirNames[];
		extern char *DirLength[];
		extern char *OperationALUNames[];
		extern char *OperationFPUNames[];
		extern char *PasstroughExectime[];
		extern char *PasstroughMOVRegisters[];
		extern char *PasstroughSTACKRegisters[];
		extern char *PasstroughINCDECRegisters[];
		extern char *PasstroughBits[];
		extern char *ReturnRegisters[];

		// This class provides conversion between a mnemonic and decoded instruction
		class Mnemonic
		{
			DecodedInstr instr;
			std::string mnemonic;

			void ComposeMnemonic();
			void ComposeInstruction();

			std::string ComposeDirection();

			char *GetQueryBaseName(reg16 index);
			char *GetQueryOperandName(reg16 index);
			char *GetOperationBaseName(reg16 index);
			char *GetPasstroughBaseName(reg16 index);
			char *GetPasstroughOperandName(reg16 index);
			std::string ComposeReturnRegister(reg16 index);
			char *GetReturnRegBaseName(reg16 index);

			void DecomposeMnemonic(
				std::string mnemonic,
				std::string *base,
				std::string *direction,
				std::string *operand);

			void DecodeBase(std::string base);
			void DecodeDirection(std::string direction);
			int GetQueryOperandOffset(reg16 index, std::string operand);
			int GetPasstroughOperandOffset(reg16 index, std::string operand);
			bool PasstroughIndexIndirectMatch(std::string base, std::string operand);

		public:
			Mnemonic(DecodedInstr instr)
			{
				this->instr = instr;
				ComposeMnemonic();
			}
			Mnemonic(const char *mnemonic)
			{
				this->mnemonic = mnemonic;
				ComposeInstruction();
			}

			DecodedInstr GetInstr()
			{
				return instr;
			}
			std::string GetMnemonic()
			{
				return mnemonic;
			}

			static void DecodeIndexReturnSubtype(std::string ins,
			bool *val, bool *mem, bool *io);
		};
	}
}