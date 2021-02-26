#include "butils.h"
#include "btor.h"
#include "sha1.hpp"
#include <math.h>


void setBitInBitfield(std::vector<uint8_t>& bitfield, const int bit_to_set)
{
	// This will set a value to 1. (We don't need to clear bits?)
	// or if we do, just do it in another function.
	// figure out which BYTE and BIT we're in.
	// 0 = byte 0, bit 8
	// we're storing bits in an ARRAY of UINT8_T
	// so

	// 0123 4567  8911 1111  1111 2222 ...
	//              01 2345  6789 0123
	int target_byte = bit_to_set / 8;
	int bit_offset = bit_to_set % 8;
	// so now we'll grab the n-th byte
	bitfield[target_byte] |= (0x80 >> bit_offset);
}

bool isBitInBitfieldSet(const std::vector<uint8_t>& bitfield, const int bit_to_check)
{
	// check if a bit in the bitfield is set
	int target_byte = bit_to_check / 8;
	int bit_offset = bit_to_check % 8;
	return bitfield[target_byte] & (0x80 >> bit_offset);
}

unsigned long long getBitfieldSize(Metainfo& metainfo)
{
	// Calculate size of a bitfield based on metainfo we've collected
	unsigned long long number_of_pieces = ceil(metainfo.total_length / metainfo.piece_length);
	unsigned long long size_of_bitfield = ceil(number_of_pieces / 8);
	return size_of_bitfield;
}


void getSHA1(Metainfo& metainfo)
{
	// get our SHA1 info hash as a vector of uint32
	sha1::SHA1 checksum;
	checksum.update(metainfo.infodict);
	std::vector<uint32_t> res;
	res = checksum.final();

	// we'll store it in hex that we send to the announcer
	std::ostringstream result;
	for (size_t i = 0; i < res.size(); i++)
	{
		result << std::hex << std::setfill('0') << std::setw(8);
		result << res[i];
	}
	metainfo.info_hash_hex = result.str();

	// and also store as a vector of bytes that we'll use as our tcp handshake
	std::vector<uint8_t> converted((std::uint8_t*)&res, (std::uint8_t*)&(res)+sizeof(std::uint32_t) * 5);
	metainfo.info_hash_raw = converted;
}

std::string generateId()
{
	// return our 20 char unique ID to use
	// -btor01- plus random numbers
	std::string unique_id = "-btor01-";
	while (unique_id.size() < 20)
	{
		unique_id += std::to_string(rand() % 10);
	}
	return unique_id;
}

std::string urlEncode(std::string inputString)
{
	// urlEncode a string, since our string is already hex, we just
	// have to toss a % in before every byte (2 nibbles)
	std::string encodedString;
	int unsigned i = 0;
	for (char ch : inputString)
	{
		if (i % 2 == 0) encodedString += "%";
		encodedString += ch;
		i++;
	}
	return encodedString;
}

void fillMetainfo(Metainfo& metainfo, BParser& bParser)
{
	// fill in a given metainfo struct with information from the bParser?

}

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

void show_status(float percentage)
{
	// erase last status bar
	std::string backspaces(22, '\b');
	std::cout << backspaces;
	int tens = (int)(percentage * 10);
	int ones = (int)(100 * percentage) % 10;
	std::string middle_bit{ "" };
	for (int i = 0; i < 10; ++i)
	{
		if (i == tens)
			if (ones <= 4)
			{
				middle_bit += std::to_string(ones) + ".";
			}
			else
			{
				middle_bit += (char)254u + std::to_string(ones);
			}
		else if (i < tens)
		{
			middle_bit += (char)254u;
			middle_bit += (char)254u;
		}
		else
			middle_bit += "..";
	}
	std::cout << "[" << middle_bit << "]";
}
