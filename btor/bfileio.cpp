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

//std::vector<uint8_t> fileio::readBitfield(const std::string filename)
//{
//	// We want to read in the bitfield from the file as unsigned chars
//	// basically? 
//	return {};
//}

std::vector<uint8_t> fileio::readBitfield(const std::string file_path)
{
	std::ifstream instream(file_path, std::ios::in | std::ios::binary);
	std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
	std::cout << "In readBitfield size of data is " << data.size() << '\n';
	return data;
}

void fileio::writeBitfield(const std::string filename, const BState& state)
{
	// We want to write out our bitfield to our saved file so we can 
	// resume our download later!
	// We'll call this when we're finished up
	std::ofstream outstream(filename, std::ios::out | std::ios::binary);
	size_t bytes_to_write = state.bitfield.size();
	std::vector<uint8_t> bitfield_data = state.bitfield;
	if (bitfield_data.size() == 0)
		return;	// nothing to write
	outstream.write(reinterpret_cast<char*>(&bitfield_data[0]), bytes_to_write);
}

std::vector<char>* const fileio::getFileBuf()	// let's make this not a char, but uint8_t?
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
	// Create an empty file of filesize bytes
	// We'll use this to create the files before we fill them in, and also
	//  to create our empty bitfield file
	std::ofstream ofs(filename, std::ios::binary | std::ios::out);
	ofs.seekp(filesize - 1);
	ofs.write("", 1);
}

void fileio::createDirectory(const std::string dir)
{
	// Create a directory, we'll use this to design the directories that
	// we're going to put all the files in.
	if (CreateDirectoryA((LPCSTR)dir.c_str(), NULL) != 0)
	{
		std::cout << "Error creating directory " << dir << ": " << GetLastError();
		// TODO: We don't want to exit out of ANY FUNCTIONS since we need to
		// send stop signals in our main program, gather threads, write out bitfield,
		// etc. etc.
		exit(EXIT_FAILURE);
	}
}