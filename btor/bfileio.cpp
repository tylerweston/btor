#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "btor.h"
#include "bfileio.h"
#include <fstream>
#include <iostream>
#include <filesystem>


fileio::fileio() : filebuf{}
{
	std::cout << "Initializing fileio...\n";
}

fileio::~fileio()
{
	std::cout << "Cleaning up fileio...\n";
}

void fileio::readFile(const std::string filename)
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
	// allocate space for reading the file
	filebuf = new std::vector<char>(length);	// todo: make this a uint8_t
	// rewind file pointer
	infile.seekg(0, std::ios::beg);

	// read from file
	infile.read(filebuf->data(), length);
}

bool fileio::doesBitfieldFileExist(const std::string filename)
{
	// we'll just append bf so it'll be torrentname.torrentbf
	return std::filesystem::exists(filename);
}

std::vector<uint8_t> fileio::readBitfield(const std::string filename)
{
	return {};
}

void fileio::writeBitfield(const std::string filename, BState& state)
{

}

std::vector<char>* const fileio::getFileBuf()	// let's make this not a char, but uint8_t
{
	return filebuf;
}

void writeFile(const std::string filename, std::vector<uint8_t> data_to_write)
{
	// TODO: write out to a file here
	// open files in "r+b" mode.
	// note that fsetpos() may fail, need to check return value, maybe pad, then try again?
	return;
}

void fileio::createEmptyFile(const std::string filename, unsigned long long filesize)
{
	// we'll need to error check this
	std::ofstream ofs(filename, std::ios::binary | std::ios::out);
	// 300 mb, we want this to be something else
	ofs.seekp(filesize - 1);
	ofs.write("", 1);
}

void fileio::createDirectory(const std::string dir)
{
	if (CreateDirectoryA((LPCSTR)dir.c_str(), NULL) != 0)
	{
		std::cout << "Error creating directory " << dir << ": " << GetLastError();
		exit(EXIT_FAILURE);
	}
}