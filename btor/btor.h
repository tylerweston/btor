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
	std::string info_hash_hex;				// stored info_hash in hex
	std::vector<uint8_t> info_hash_raw;		// the raw info_hash in bytes
	std::vector<struct Fileinfo> fileinfo;	// one for single file, many for multifile
	long long int total_length;				// total size of the file in bytes
	uint64_t piece_length;					// number of bytes in each piece
	std::string pieces;						// Concatenation of all 20-byte SHA1 hash values, one per piece, byte string
};

struct BState
{
	std::string filename;
	bool big_endian;
	Metainfo* metainfo;
	std::string unique_id;
	std::vector<uint8_t> bitfield;				// want to load this if it already exists
};
