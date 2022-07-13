#pragma once
#include "MemoryBinWriter.h"

namespace WPU2D
{
	namespace Shared
	{
		MemoryBinWriter::MemoryBinWriter(Memory *mem, uint offset)
		{
			this->mem = mem;
			this->offset = offset;
		}

		void MemoryBinWriter::Write(reg8 val)
		{
			if(offset > this->mem->size-sizeof(reg8))
				return;

			*(this->mem->data+offset) = val;
			offset += sizeof(reg8);
		}

		void MemoryBinWriter::Write(reg16 val)
		{
			if(offset > this->mem->size-sizeof(reg16))
				return;

			*((reg16*)(this->mem->data+offset)) = val;
			offset += sizeof(reg16);
		}

		void MemoryBinWriter::Write(reg32 val)
		{
			if(offset > this->mem->size-sizeof(reg32))
				return;

			*((reg32*)(this->mem->data+offset)) = val;
			offset += sizeof(reg32);
		}
	}
}