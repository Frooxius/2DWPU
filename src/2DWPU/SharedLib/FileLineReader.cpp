#include "FileLineReader.h"

namespace WPU2D
{
	namespace Shared
	{
		FileLineReader::FileLineReader(char *filename)
		{
			// open the file
			file.open(filename, std::ios::in);
		}

		FileLineReader::~FileLineReader()
		{
			// close it
			file.close();
		}

		std::string FileLineReader::FetchLine()
		{
			std::string str;
			std::getline(file, str);
			return str;
		}

		bool FileLineReader::Eof()
		{
			return file.eof();
		}
	}
}