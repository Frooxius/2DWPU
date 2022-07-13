#pragma once
#include "Global.h"

namespace WPU2D
{
	namespace Core
	{
		class IOdevice
		{
		public:
			virtual reg16 Read16(reg16 addr) = 0;
			virtual reg8 Read8(reg16 addr) = 0;

			virtual void Write16(reg16 val, reg16 addr) = 0;
			virtual void Write8(reg8 val, reg16 addr) = 0;

			virtual void SetBaseaddr(reg16 addr) = 0;
			virtual reg16 GetAddrRange() = 0;
		};
	}
}