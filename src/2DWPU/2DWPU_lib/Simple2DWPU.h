/*
	This is simple 2DWPU core that implements the basis needed
	for emulation, but doesn't provide any tools for cycle accurate
	analysis
*/
#pragma once
#include "WPU2Dcore.h"
#include "Registers.h"
#include "Simple2DWPUcore.h"
#include "IOinterface.h"
#include "Global.h"
#include "Simple2DWPUparallelismManager.h"

namespace WPU2D
{
	namespace Core
	{
		class Simple2DWPU : public WPU2Dcore
		{
			DEBUG_ALLPUBLIC

			// Required memories
			Memory *RAM;

			// IO interface for communication with external devices
			IOinterface *io;

			// Parallelism manager
			Simple2DWPU_PM *pm;

			// Registers
			GlobalRegisters reg;

			// All the cores
			Simple2DWPUcore **cores;
			uint ncores;

			// statistics and such
			ull cycles_count;

		public:
			Simple2DWPU(Memory *RAM, uint ncores);

			bool Cycle();
			bool Cycle(uint cycles);
			void Reset();

			Memory *GetMemory() { return RAM; }
			byte *GetMemoryPtr() { return RAM->data; }
			unsigned int GetMemorySize() { return RAM->size; }

			char *GetCoreName() { return "Simple2DWPU"; }
			char *GetCoreDescription() {
				return "Version 0.1 by Frooxius."; }

			// Accessing internal data
			PrivateRegisters *GetPrivateReg(uint core)
			{ return cores[core%ncores]->GetPrivateReg(); } 

			GlobalRegisters *GetPublicReg()
			{ return &reg; }

			// info about the core
			uint GetCoreCount() { return ncores; }
			ull GetCycles() { return cycles_count; }

			IOinterface *GetIOinterface() { return io; }

			void CheckParallelInvoke() { for(int i = 0; i < ncores; ++i) cores[i]->CheckParallelInvoke(); }
		};
	}
}

