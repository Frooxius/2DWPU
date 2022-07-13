#pragma once
#include "Global.h"
#include "BinWriter.h"
#include "Memory.h"

namespace WPU2D
{
	namespace Shared
	{
		class MemoryBinWriter : public BinWriter
		{
			Memory *mem;	// used for storage
			uint offset;	// offset of the pointer

		public:
			//MemoryBinWriter(uint size);
			MemoryBinWriter(Memory *mem, uint offset = 0);

			Memory *GetMemory() { return mem; }

			void Write(reg8 val);
			void Write(reg16 val);
			void Write(reg32 val);
			void Close() { /* not much here really... */ }
		};
	}
}