#pragma once
#include <vector>
#include <string>


class fileio
{
	private:
		std::vector<char>* filebuf;
	public:
		fileio();
		~fileio();
		void readFile(const std::string filename);

		void writeFile(const std::string filename, std::vector<uint8_t> data_to_write);
		// void writeFile(const char* const filename);
		// write a block to a specific location in our output file? (files?)
		std::vector<char>* const getFileBuf();
		bool doesBitfieldFileExist(const std::string filename);

		std::vector<uint8_t> readBitfield(const std::string filename);
		void writeBitfield(const std::string filename, BState& state);

		void createEmptyFile(const std::string filename, unsigned long long filesize);
		void createDirectory(const std::string dir);
};


