#pragma once
#include "Global.h"

/*
	Contains configuration constants for 2DWPU
*/


namespace WPU2D
{
	namespace Core
	{
		const int TIMERS = 4;
		const int PROGRAM_BLOCK_HEADER_SIZE = 64;
		const int PROGRAM_BLOCK_WIDTH =  32;
		const int PROGRAM_BLOCK_HEIGHT =  32;
		const int INSTRUCTION_WIDTH = 2;

		const int DEFAULT_INSTR_STACK_SIZE = 1024;
		const int DEFAULT_ARG_STACK_SIZE = 256;

		const int MAX_PARALLEL_RESULTS = 16;
	}
}