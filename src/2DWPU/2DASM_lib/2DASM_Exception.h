#pragma once
#include "Exception.h"

namespace WPU2D
{
	namespace ASM2D
	{
		enum eASM2D_Exception
		{
			ERR_ASM2D_TOO_MANY_COLUMNS,
			ERR_ASM2D_TOO_MANY_ROWS,
			ERR_ASM2D_UNKNOWN_INSTRUCTION,
			ERR_ASM2D_INSTRUCTION_NOT_CLOSED,
			ERR_ASM2D_EOF,
			ERR_ASM2D_UNEXPECTED_EOF,

			ERR_ASM2D_SYNTAXERR_RET,
			ERR_ASM2D_SYNTAXERR_LENGTH,
			ERR_ASM2D_SYNTAXERR_WRONG_OPERAND,
			ERR_ASM2D_SYNTAXERR_WRONG_EXECUTION_TIME,

			ERR_ASM2D_TWO_LENGTHS,
			ERR_ASM2D_INDEX_TABLE_FULL
		};

		extern char *StrASM2D_Exception[];

		class ASM2D_Exception : protected Exception
		{
		public:
			ASM2D_Exception(eASM2D_Exception exception, std::string append = "") : 
			  Exception(StrASM2D_Exception[(uint)exception], (uint)exception)
			  {
				  if(append.length())
					Append(append);
			  }

			  eASM2D_Exception GetCode() { 
				  return (eASM2D_Exception)Exception::GetCode(); }
			  const char *GetMsg() { return Exception::GetMsg(); }
		};

	}
}