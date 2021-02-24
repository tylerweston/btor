#pragma once
#include <vector>


class fileio
{
	private:
		std::vector<char>* filebuf;
	public:
		fileio();
		~fileio();
		void readFile(const char* const filename);
		// void writeFile(const char* const filename);
		// write a block to a specific location in our output file? (files?)
		std::vector<char>* const getFileBuf();
};
