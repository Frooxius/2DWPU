#include "Simple2DWPU.h"

namespace WPU2D
{
	namespace Core
	{
		Simple2DWPU::Simple2DWPU(Memory *RAM, uint ncores)
		{
			cycles_count = 0;

			this->RAM = RAM;

			// Create the IO interface
			this->io = new IOinterface();

			// Create required number of cores
			this->ncores = ncores;
			cores = new Simple2DWPUcore*[ncores];

			cores[0] = new Simple2DWPUcore(RAM, io, false, &reg);

			for(int i = 1; i < (int)ncores; ++i)
				cores[i] = new Simple2DWPUcore(RAM, io, true, &reg);

			// parallelism manager
			this->pm = new Simple2DWPU_PM(cores, ncores);

			Reset();

			cores[0]->Activate(true);
		}

		bool Simple2DWPU::Cycle()
		{
			cycles_count++;

			for(uint i = 0; i < ncores; ++i)
				cores[i]->Cycle();

			pm->Cycle();

			return true;	// TODO
		}

		bool Simple2DWPU::Cycle(uint cycles)
		{
			while(cycles--)
				if(!Cycle())
					break;

			return true;	// TODO
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

			cores[0]->Activate(true);
		}
	}
}