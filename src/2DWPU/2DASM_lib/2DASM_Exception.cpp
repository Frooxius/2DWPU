#include "2DASM_Exception.h"

namespace WPU2D
{
	namespace ASM2D
	{
		char *StrASM2D_Exception[] =
		{
			"Too many instructions in a row.",
			"Too many rows for an program block",
			"Unknown instruction found",
			"Instruction not cloed",
			"EOF",
			"Unexpected end of file",

			"Syntax error: Unknown RET instruction",
			"Syntax error: Illegal query length",
			"Syntax error: Illegal operand",
			"Syntax error: Unknown execution time symbol",

			"Only one direction can have query length longer than 1",
			"Index table is full"
		};
	}
}