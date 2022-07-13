#pragma once
#include "Global.h"
#include "LineReader.h"
#include <string>
#include <fstream>
#include <iostream>

namespace WPU2D
{
	namespace Shared
	{
		class FileLineReader : public LineReader
		{
			std::ifstream file;

		public:
			FileLineReader(char *filename);
			~FileLineReader();

			std::string FetchLine();
			bool Eof();
		};
	}
}