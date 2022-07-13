#include "IOinterface.h"

namespace WPU2D
{
	namespace Core
	{
		IOinterface::IOinterface()
		{
			ClearDevices();
		}

		reg16 IOinterface::Read16(reg16 addr)
		{
			reg16 val = 0xFFFFU;
			for(int i = 0; i < devices[addr].size(); ++i)
				val &= devices[addr][i]->Read16(addr);
			return val;
		}

		reg8 IOinterface::Read8(reg16 addr)
		{
			reg8 val = 0xFFU;
			for(int i = 0; i < devices[addr].size(); ++i)
				val &= devices[addr][i]->Read8(addr);
			return val;
		}

		void IOinterface::Write16(reg16 val, reg16 addr)
		{
			for(int i = 0; i < devices[addr].size(); ++i)
				devices[addr][i]->Write16(val, addr);
		}

		void IOinterface::Write8(reg8 val, reg16 addr)
		{
			for(int i = 0; i < devices[addr].size(); ++i)
				devices[addr][i]->Write8(val, addr);
		}

		void IOinterface::InstallDevice(IOdevice *dev, reg16 baseaddr)
		{
			for(int i = 0; i <= dev->GetAddrRange(); ++i)
				devices[(baseaddr+i)%65536].push_back(dev);
			dev->SetBaseaddr(baseaddr);
		}

		void IOinterface::ClearDevices()
		{
			for(int i = 0; i < 65536; ++i)
				devices[i] = std::vector<IOdevice*>();
		}
	}
}