#pragma once
#include "Simple2DWPUparallelismManager.h"

namespace WPU2D
{
	namespace Core
	{
		Simple2DWPU_PM::Simple2DWPU_PM(
			Simple2DWPUcore **cores, uint ncores, CoreStats *stats)
		{
			this->cores = cores;
			this->ncores = ncores;
			this->stats = stats;
			// now allocate enough ParCoreInfo's
			coreinfo = new ParCoreInfo[ncores];

			// do a cleanup
			Reset();
		}

		void Simple2DWPU_PM::Reset()
		{
			for(int i = 0; i < ncores; ++i)
			{
				coreinfo[i].forID = 0;
				coreinfo[i].processing = false;
				for(int n = 0; n < MAX_PARALLEL_RESULTS; n++)
				{
					coreinfo[i].result[n].val = 0;
					coreinfo[i].result[n].done = false;
				}
				coreinfo[i].result_top = 0;
			}

			// it's always processing
			coreinfo[0].processing = true;
		}

		void Simple2DWPU_PM::Cycle()
		{
			// first, scan all cores and check if any of them has a result ready
			for(int i = 0; i < ncores; ++i)
				if(coreinfo[i].processing && cores[i]->C_FREE())
				{
					// store the result
					coreinfo[coreinfo[i].forID]
					.result[coreinfo[i].resultID].val = cores[i]->C_ARG();
					coreinfo[coreinfo[i].forID]
					.result[coreinfo[i].resultID].done = true;

					coreinfo[i].processing = false; // not processing anymore, duh
				}

			// now check if any of the cores needs some calculation
			for(int i = 0; i < ncores; ++i)
			{
				if(cores[i]->C_RQST() && !cores[i]->C_FREE())
					AssignTask(i);
				if(cores[i]->C_FTCH())
					GiveResult(i);
			}
		}

		void Simple2DWPU_PM::AssignTask(uint coreid)
		{
			// if it doesn't have any more storage, then it can't start
			if(coreinfo[coreid].result_top == MAX_PARALLEL_RESULTS)
				return;

			stats->parallel_failed_forks++;

			// find a free core
			for(int i = 0; i < ncores; ++i)
				if(cores[i]->C_FREE() && !coreinfo[i].processing)
				{
					// found a free core, so make it non-free now x3
					cores[i]->C_ARG( cores[coreid]->C_ARG() );
					cores[i]->C_PC( cores[coreid]->C_PC() );
					cores[i]->C_RQST(true);

					// copy the PE and PO registers
					cores[i]->GetPrivateReg()->PE = cores[coreid]->C_PE();
					cores[i]->GetPrivateReg()->PO = cores[coreid]->C_PO();

					// store information about the new core
					coreinfo[i].forID = coreid;
					coreinfo[i].resultID = coreinfo[coreid].result_top;
					coreinfo[i].processing = true;

					// make space for the result
					coreinfo[coreid].result[coreinfo[coreid].result_top].done = false;
					coreinfo[coreid].result_top++;

					// notify the calling core that the task has been assigned
					cores[coreid]->C_ACPT(true);

					stats->parallel_forks++;
					stats->parallel_failed_forks--;

					break;
				}
		}

		void Simple2DWPU_PM::GiveResult(uint coreid)
		{
			bool ready = coreinfo[coreid].result[coreinfo[coreid].result_top-1].done;
			cores[coreid]->C_REDY(ready);
			if(ready)
			{
				cores[coreid]->C_ARG(
					coreinfo[coreid].result[coreinfo[coreid].result_top-1].val);
				coreinfo[coreid].result_top--;
				cores[coreid]->C_REDY(true);

				stats->parallel_joins++;
			}
			else
				stats->parallel_failed_joins++;
		}

	}
}