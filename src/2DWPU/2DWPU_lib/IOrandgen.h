#pragma once
#include "IOdevice.h"
#include <cmath>

namespace WPU2D
{
	namespace Core
	{
		class IOrandgen : public IOdevice
		{
		public:
			reg16 Read16(reg16 addr) { return (reg16)rand(); }
			reg8 Read8(reg16 addr) { return (reg8)rand(); }

			void Write16(reg16 val, reg16 addr) { }
			void Write8(reg8 val, reg16 addr) { }

			void SetBaseaddr(reg16 addr) { }
			reg16 GetAddrRange() { return 0; }
		};
	}
}