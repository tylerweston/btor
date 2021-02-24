#include "butils.h"
#include "btor.h"
#include "sha1.hpp"


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