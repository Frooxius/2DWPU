/*
	This core implements various functions that allow to examine
	ít's behavior under various cirmustances and configurations
*/
#include "WPU2Dcore.h"

namespace WPU2D
{
	namespace Core
	{
		class Analys2DWPU : public WPU2Dcore
		{
			/* Speed Execution analysis */
			// frequency of devices
			float	frqCore,
					frqMemory,
					frqIO;

			uint	latencyParallelRequest,
					latencyParallelDone,
					latencyParallelGet;
						
			ull		countMemoryWait,
					countIOWait,
					countInstructionWait,
					countExecutionWait;

		public:
			bool Cycle();
			bool Cycle(unsigned int cycles);

			byte *GetMemoryPtr();
			unsigned int GetMemorySize();

			char *GetCoreName() { return "Analys2DWPU"; }
			char *GetCoreDescription() {
				return "Version 0.1 by Frooxius."; }

			// Set parameters for performance analysis

		};
	}
}

