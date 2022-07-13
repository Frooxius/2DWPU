#include "Simple2DWPU.h"

namespace WPU2D
{
	namespace Core
	{
		Simple2DWPU::Simple2DWPU(Memory *RAM, uint ncores)
		{
			this->RAM = RAM;

			// Create the IO interface
			this->io = new IOinterface();

			// Create required number of cores
			this->ncores = ncores;
			cores = new Simple2DWPUcore*[ncores];

			for(int i = 0; i < (int)ncores; ++i)
				cores[i] = new Simple2DWPUcore(RAM, io, i!=0, &reg, &stats);

			// parallelism manager
			this->pm = new Simple2DWPU_PM(cores, ncores, &stats);

			// Debugging stuff
			ClearBreakpoints();

			Reset();

			cores[0]->Activate(true);
		}

		bool Simple2DWPU::Cycle()
		{
			bool halt = false;

			stats.cycles++;

			for(uint i = 0; i < ncores; ++i)
				if(!cores[i]->Cycle())
				{
					halt = true;
					break;
				}

			pm->Cycle();

			if(check_breakpoint)
			{
				for(uint i = 0; i < ncores; ++i)
				{
					regPC *temp = &cores[i]->GetPrivateReg()->PC;
					if(breakpoint_map[temp->xPC][temp->yPC])
					{
						halt = true;
						break;
					}
				}
			}

			return !halt;	// TODO
		}

		bool Simple2DWPU::Cycle(uint cycles)
		{
			while(cycles--)
				if(!Cycle())
					return false;

			return true;
		}

		void Simple2DWPU::Reset()
		{
			reg.LI = reg.SQ = reg.RE = reg.TR
				= reg.CI = reg.HE = reg.ST = reg.EL
				= 0;

			reg.S_SW.raw = 0;

			for(int i = 0; i < ncores; ++i)
				cores[i]->Reset();

			pm->Reset();

			stats.Reset();

			cores[0]->Activate(true);
		}

		void Simple2DWPU::ClearBreakpoints()
		{
			for(int y = 0; y < 32; ++y)
				for(int x = 0; x < 32; ++x)
					breakpoint_map[x][y] = false;
			check_breakpoint = false;
		}

		void Simple2DWPU::SetBreakpoint(reg5 x, reg5 y)
		{
			breakpoint_map[x][y] = true;
			check_breakpoint = true;
		}
	}
}