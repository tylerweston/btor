#include "bfileio.h"
#include <fstream>
#include <iostream>

bf::fileio::fileio()
{
	std::cout << "Initializing fileio...\n";
}

bf::fileio::~fileio()
{
	std::cout << "Cleaning up fileio...\n";
}

void bf::fileio::read_file(const char* const filename)
{

	std::cout << "Opening file " << filename << '\n';
	std::ifstream infile(filename, std::ios_base::binary);	// read in binary mode
	if (not infile.is_open())
	{
		// TODO: Fail gracefully if file isn't found
		std::cout << "File not found: " << filename << '\n';
		exit(EXIT_FAILURE);
	}
	// get length of file
	infile.seekg(0, std::ios::end);
	size_t length = infile.tellg();
	std::cout << "Size of file: " << length << '\n';
	// allocate space for reading the file
	filebuf = new std::vector<char>(length);
	// rewind file pointer
	infile.seekg(0, std::ios::beg);

	// read from file
	infile.read(filebuf->data(), length);
}

std::vector<char>* const bf::fileio::getFileBuf()
{
	return filebuf;
}
