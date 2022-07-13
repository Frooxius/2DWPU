#pragma once
#include "IOdevice.h"
#include "Global.h"
#include "simplelcd.h"
#include <qimage.h>

namespace WPU2D
{
	namespace Core
	{
		enum IOsimpleLCDcommands
		{
			simpleLCD_command,
			simpleLCD_writeX,
			simpleLCD_writeY,
			simpleLCD_writeR,
			simpleLCD_writeG,
			simpleLCD_writeB,
			simpleLCD_TERMINATOR
		};

		class IOsimpleLCD : public IOdevice
		{
			SimpleLCD *lcd;
			byte buffer[400*240*3];
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