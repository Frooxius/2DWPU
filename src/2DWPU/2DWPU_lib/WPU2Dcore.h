/*
	This is an abstract class that describes the basic interface for the
	2DWPU core
*/
#pragma once
#include "Global.h"
#include "Registers.h"
#include "IOinterface.h"

namespace WPU2D
{
	namespace Core
	{
		class WPU2Dcore
		{

		public:

			// Cycle function return false if processor is halted, otherwise true
			virtual bool Cycle() = 0;	// performs one cycle step
			virtual bool Cycle(unsigned int cycles) = 0;	// perfoms given number of cycles
			virtual void Reset() = 0;	// resets the circuit

			virtual byte *GetMemoryPtr() = 0;			// get access to the memory
			virtual unsigned int GetMemorySize() = 0;	// how big is the memory	

			virtual char *GetCoreName() = 0; // get name of the current core
			virtual char *GetCoreDescription() = 0; // get the description of the current core

			// Accessing internal data
			virtual PrivateRegisters *GetPrivateReg(uint core) = 0;
			virtual GlobalRegisters *GetPublicReg() = 0;

			// info about the core
			virtual uint GetCoreCount() = 0;
			virtual ull GetCycles() = 0;

			virtual IOinterface *GetIOinterface() = 0;

			virtual void CheckParallelInvoke() = 0;
		};
	}
}