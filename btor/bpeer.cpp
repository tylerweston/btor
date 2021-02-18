#include "bpeer.h"


std::array<uint8_t, 6> peerStringToByteArray(std::string peerString)
{
	// convert a peer string to an array of 6 bytes
	return { 0,0,0,0,0,0 };
}

// take in 6 bytes and return 6 bytes
// first 4 bytes need to be switched from big endian to little endian
// last 2 bytes are the port number
// 
std::array<uint8_t, 6> parsePeer(std::array<uint8_t, 6> peerBytes)
{
	return { 0,0,0,0,0,0 };
}