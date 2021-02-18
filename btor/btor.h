#pragma once
#include <string>
#include <vector>

typedef std::basic_string<unsigned char> ustring;

struct Fileinfo
{
	std::string name;	// can include a path
	int64_t length;		// size of file in bytes
};

struct Metainfo
{
	std::string announce;					// announce URL
	std::string infodict;					// the bencoded string that represents the info dictionary
	std::vector<struct Fileinfo> fileinfo;	// one for single file, many for multifile
	long long int totallength;					// total size of the file in bytes
	int64_t piecelength;					// number of bytes in each piece
	std::string pieces;						// Concatenation of all 20-byte SHA1 hash values, one per piece, byte string
};

