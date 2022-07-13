#pragma once
#include "Global.h"
#include "IOdevice.h"
#include <vector>

namespace WPU2D
{
	namespace Core
	{
		class IOinterface
		{
			// holds all the devices
			std::vector<IOdevice*> devices[65536]; 

		public:
			IOinterface();

			reg16 Read16(reg16 addr);
			reg8 Read8(reg16 addr);

			void Write16(reg16 val, reg16 addr);
			void Write8(reg8 val, reg16 addr);

			void InstallDevice(IOdevice *dev, reg16 baseaddr);
			void ClearDevices();
		};
	}
}