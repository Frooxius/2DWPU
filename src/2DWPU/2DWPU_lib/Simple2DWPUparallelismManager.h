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
		class Simple2DWPUparallelismManager
		{
			
			Simple2DWPUcore *cores;
			uint ncores;	// number of the managed cores

		public:
			Simple2DWPUparallelismManager(
				Simple2DWPUcore *cores, uint ncores );

		};
	}

}