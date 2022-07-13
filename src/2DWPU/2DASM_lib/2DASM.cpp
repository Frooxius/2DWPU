#include "2DASM.h"
#include "2DASM_Exception.h"

namespace WPU2D
{
	namespace ASM2D
	{
		void Assembler2D::Assembly(Shared::LineReader *input,
			Shared::BinWriter *output)
		{
			// Get list of blocks first
			vector<ProgramBlock> programBlocks;

			try
			{
				do
				{
					programBlocks.push_back(
						GetProgramBlock(input));
				} while(!input->Eof());
			}
			catch(ASM2D_Exception err)
			{
				// unless its EOF, rethrow
				if(err.GetCode() != ERR_ASM2D_EOF)
					throw err;
			}

			// Now the list of blocks is loaded, so process it
			for(int i = 0; i < programBlocks.size(); ++i)
				WriteProgramBlock(programBlocks[i], output);

			// Close the output
			output->Close();
		}

		void Assembler2D::WriteProgramBlock(ProgramBlock block,
			Shared::BinWriter *output)
		{
			// first, the program block header
			for(int i = 0; i < 64; ++i)
				output->Write((reg8)0); // TODO

			for(int y = 0; y < 32; ++y)
				for(int x = 0; x < 32; ++x)
				{
					PreprocessInstruction(block.instructions[y][x],
						&block);
					output->Write( Mnemonic(block.instructions[y][x].c_str())
						.GetInstr().EncodeInstruction() );
				}

				// write the index data table
				for(int i = 0; i < block.index_table_items; ++i)
					output->Write(block.index_table[i]);
		}

		vector<string> Assembler2D::FetchInstructionList(string line)
		{
			vector<string> row;
			string instr;
			bool inside_instr = false;

			for(int i = 0; i < line.length(); ++i)
			{
				// if it's not inside
				if(!inside_instr)
				{
					// check if it's beginning 
					if(line[i] == '[')
						inside_instr = true;	// start reading
				}
				else
				{
					// unless it's end
					if(line[i] != ']')
						instr += line[i];	// add it to the current instr
					else
					{
						// if the instruciton is closed, then:
						row.push_back(instr);	// store it
						instr = "";	// clear the instr
						inside_instr = false;	// stop reading
					}
				}
			}

			// Now check for errors
			if(inside_instr)
				throw ASM2D_Exception(ERR_ASM2D_INSTRUCTION_NOT_CLOSED);

			if(row.size() > 32)
				throw ASM2D_Exception(ERR_ASM2D_TOO_MANY_COLUMNS);

			// fill it up
			while(row.size() != 32)
				row.push_back("RET 0");

			// everything's fine (hopefully x3) return it
			return row;
		}

		ProgramBlock Assembler2D::GetProgramBlock(
			Shared::LineReader *input)
		{
			ProgramBlock block;
			string line;

			// seach for the beginning of a block
			while(!input->Eof())
			{
				line = input->FetchLine();
				if( line.find("block") != line.npos )
					break;
			}

			// Normal eof, end the processing
			if(input->Eof())
				throw ASM2D_Exception(ERR_ASM2D_EOF);

			// get name and modifier
			block.block_modifier = 
				line.substr(0, line.find("block"));

			block.block_name = line.substr(
				line.find("block") + 5, line.npos);

			// keep reading lines
			do
			{
				if(input->Eof())
					throw ASM2D_Exception(ERR_ASM2D_UNEXPECTED_EOF);

				line = input->FetchLine();
				vector<string> instrList = FetchInstructionList(line);
				if(!instrList.empty())
					block.instructions.push_back(instrList);

			} while( line.find("<ENDBLOCK>") == line.npos);

			if(block.instructions.size() > 32)
				throw ASM2D_Exception(ERR_ASM2D_TOO_MANY_ROWS);

			// fill it up to match 32x32
			vector<string> emptyrow;
			for(int i = 0; i < 32; ++i)
				emptyrow.push_back("RET 0");
			while(block.instructions.size() != 32)
				block.instructions.push_back(emptyrow);

			return block;
		}

		void Assembler2D::PreprocessInstruction(string &instr, ProgramBlock *block)
		{
			// Check if it contains some of the stuff that is going to be preprocessed
			if(instr.find("RET") != instr.npos)
			{
				// Determine the type of return it is
				bool val, mem, io;
				Mnemonic::DecodeIndexReturnSubtype(instr,
					&val, &mem, &io);

				// syntax error
				if((int)val + (int)mem + (int)io > 1)
					throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_RET);

				// the value which will be stored in the index table
				if(val | mem | io)
				{
					reg32 cellval;
					reg16 index;
					if(val)
						cellval = (uint)FromString<int>(instr.substr(3, instr.npos));
					if(mem)
						cellval = (uint)FromString<int>(instr.substr(instr.find("*")+1, instr.npos));
					if(io)
						cellval = (uint)FromString<int>(instr.substr(instr.find("#")+1, instr.npos));
				
					index = IndexTableStore(cellval, block);

					// now replace the old instruction with a new one
					instr = "RET ";
					if(mem)
						instr += "*";
					if(io)
						instr += "#";
					instr += "@" + ToStringHEX(index, 4);
				}
			}
		}

		// Stores a value in the index table and return the index
		reg16 Assembler2D::IndexTableStore(reg32 val, ProgramBlock *block)
		{
			// save space
			for(int i = 0; i < block->index_table_items; ++i)
				if(block->index_table[i] == val)
					return i;

			// check if the table isn't full yet
			if(block->index_table_items == 1024)
				throw ASM2D_Exception(ERR_ASM2D_INDEX_TABLE_FULL);

			block->index_table[block->index_table_items++] = val;
			
			return block->index_table_items-1;
		}
	}
}