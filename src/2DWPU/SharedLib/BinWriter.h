#pragma once
#include "Global.h"

namespace WPU2D
{
	namespace Shared
	{
		class BinWriter
		{
		public:
			virtual void Write(reg8 val) = 0;
			virtual void Write(reg16 val) = 0;
			virtual void Write(reg32 val) = 0;
			virtual void Close() = 0;
		};
	}
}