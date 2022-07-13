#pragma once
#include "Global.h"
#include "LineReader.h"
#include "BinWriter.h"
#include <vector>
#include "Mnemonics.h"
#include "DecodedInstr.h"

namespace WPU2D
{
	namespace ASM2D
	{
		using namespace std; 

		struct ProgramBlock
		{
			string block_name, block_modifier;
			vector<vector<string>> instructions;

			reg32 index_table[1024];
			uint index_table_items;

			ProgramBlock()
			{
				block_name = block_modifier = string();
				instructions = vector<vector<string>>();
				index_table_items = 0;
			}
		};

		static class Assembler2D
		{
			static vector<string> FetchInstructionList(string line);
			static ProgramBlock GetProgramBlock(
				Shared::LineReader *input);
			static void WriteProgramBlock(ProgramBlock block,
				Shared::BinWriter *output);

			static void PreprocessInstruction(string &instr, ProgramBlock *block);
			static reg16 IndexTableStore(reg32 val, ProgramBlock *block);

		public:
			static void Assembly(Shared::LineReader *input,
				Shared::BinWriter *output);
		};
	}
}