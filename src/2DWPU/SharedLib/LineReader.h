#pragma once
#include "Global.h"
#include <string>

namespace WPU2D
{
	namespace Shared
	{
		class LineReader
		{
		public:
			virtual ~LineReader() { }

			virtual std::string FetchLine() = 0;
			virtual bool Eof() = 0;
		};
	}
}