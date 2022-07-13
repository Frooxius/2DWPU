#include <iostream>
#include "Memory.h"
#include "Global.h"
#include "Simple2DWPU.h"
#include "DecodedInstr.h"
#include "Mnemonics.h"
#include <fstream>

using namespace std;
using namespace WPU2D::Core;
using namespace WPU2D::Shared;
using namespace WPU2D::ASM2D;

void main()
{
	ofstream file("instructions.txt", ios::out);

	for(int i = 0; i < 65636; ++i)
	{
		file << Mnemonic(DecodedInstr((reg16)i)).GetMnemonic()
			<< endl;
	}

	file.close();
	//cin.get();
}