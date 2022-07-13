#include <iostream>
#include <fstream>
#include "2DASM.h"
#include "FileLineReader.h"
#include "MemoryBinWriter.h"
#include "Memory.h"
#include "2DASM_Exception.h"

using namespace std;
using namespace WPU2D::Shared;

int main(int argc, char *argv[])
{
	if(argc != 2)
		return 1;

	Memory *mem = new Memory(8192);
	FileLineReader *reader = new FileLineReader(argv[1]);
	try
	{
	WPU2D::ASM2D::Assembler2D::Assembly(
			reader,
			new MemoryBinWriter(mem));
	}
	catch(WPU2D::ASM2D::ASM2D_Exception err)
	{
		cout << "2DASM Error" << err.GetMsg();
	}

	cout << "Compiled.";

	

	ofstream file("out.2D", ios::binary | ios::out );
	file.write((const char *)mem->data, mem->size);
	file.close();

	return 0;
}