#pragma once
#include "Global.h"
#include "Simple2DWPUcore.h"

/* 
	This is a device that manages the parallelism -
	assigning threads to cores
*/

namespace WPU2D
{
	namespace Core
	{
		class Simple2DWPU_PM
		{
			struct ResultStore
			{
				bool done;
				reg32 val;
			};

			struct ParCoreInfo
			{
				bool processing;	// if the core is processing anything
				uint forID;	// for who is it processing?
				uint resultID;	// which result?
				ResultStore result[MAX_PARALLEL_RESULTS];
				uint result_top;	// how many results it expects
			};

			Simple2DWPUcore **cores;
			uint ncores;	// number of the managed cores
			ParCoreInfo *coreinfo;

			void AssignTask(uint coreid);
			void GiveResult(uint coreid);

		public:
			Simple2DWPU_PM( Simple2DWPUcore **cores, uint ncores );

			void Cycle();
			void Reset();
		};
	}

}