/*#pragma once
#include "IOdevice.h"
#include "Global.h"
#include "Memory.h"

namespace WPU2D
{
	namespace Core
	{
		enum IOsimpleLCDcommands
		{
			memmgr_command,
			memmgr_
		};

		class IOmemmgr : public IOdevice
		{
			reg16 baseaddr;

			reg8 r, g, b;
			reg16 x, y;

			void ClearBuffer();
			void Display();
			void WritePixel();

		public:
			IOsimpleLCD(SimpleLCD *lcd);

			reg16 Read16(reg16 addr) { return 0; }
			reg8 Read8(reg16 addr) { return 0; }

			void Write16(reg16 val, reg16 addr);
			void Write8(reg8 val, reg16 addr) { Write16(val, addr); }

			void SetBaseaddr(reg16 addr) { baseaddr = addr; }
			reg16 GetAddrRange() { return simpleLCD_TERMINATOR; }
		};
	}
}