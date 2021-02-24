#include "butils.h"
#include "btor.h"
#include "sha1.hpp"


void getSHA1(Metainfo& metainfo)
{
	sha1::SHA1 checksum;
	checksum.update(metainfo.infodict);
	std::vector<uint32_t> res;
	res = checksum.final();
	std::cout << "sizeof res: " << res.size() << '\n';
	std::ostringstream result;
	for (size_t i = 0; i < res.size(); i++)
	{
		result << std::hex << std::setfill('0') << std::setw(8);
		result << res[i];
	}
	std::cout << "Trying to build info_hash, got hex string: " << result.str() << '\n';
	metainfo.info_hash_hex = result.str();
	//metainfo.
	// todo: info_hash_raw should be converted to an array of uint8_t now
	//std::string hash = checksum.final();
	//return hash;
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
	// fill in a given metainfo struct with information from the bParser

}