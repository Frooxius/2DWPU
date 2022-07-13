#include <iostream>
#include "Memory.h"
#include "Global.h"
#include "Simple2DWPU.h"
#include "DecodedInstr.h"
#include "Mnemonics.h"

using namespace std;
using namespace WPU2D::Core;
using namespace WPU2D::Shared;
using namespace WPU2D::ASM2D;

void main()
{
	int i = FromString<int>(" 3209");
	cout << i;
	cin.get();
}