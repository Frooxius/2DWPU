#pragma once
#include "Global.h"
#include "IOdevice.h"

namespace WPU2D
{
	namespace Core
	{
		class IOcounter : public IOdevice
		{
			int i;

		public:
			reg16 Read16(reg16 addr) { return i++; }
			reg8 Read8(reg16 addr) { return i++; }

			void Write16(reg16 val, reg16 addr) { i = val; }
			void Write8(reg8 val, reg16 addr) { i = val; }

			void SetBaseaddr(reg16 addr) { }
			reg16 GetAddrRange() { return 0; }
		};
	}
}