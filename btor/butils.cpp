#include "butils.h"
#include "btor.h"

int32_t changeEndianness(int32_t const num)
{
	return ((num >> 24 | (num & 0x00FF0000) >> 8 | (num & 0x0000FF00) << 8 | num << 24));
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




void fillMetainfo(Metainfo& metainfo, be::BParser& bParser)
{
	// fill in a given metainfo struct with information from the bParser

}