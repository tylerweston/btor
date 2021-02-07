#pragma once
#include <vector>

namespace bf
{
	class fileio
	{
		private:
			std::vector<char>* filebuf;
		public:
			fileio();
			~fileio();
			void readFile(const char* const filename);
			std::vector<char>* const getFileBuf();
	};
}