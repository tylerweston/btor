#include "bpeer.h"
#include "butils.h"


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


std::string BPeer::getHandshake(BState* state)
{
	std::string hash = state->metainfo->info_hash_hex;	// this will need to be info_hash_raw
	// PROBLEM: Hash here is 40 chars long? That doesn't seem right?
	// hash is in HEX, it should be chars?
	std::string shake = "5BitTorrent protocol00000000" + hash + state->uniqueId;
	return shake;
}

//// messages?
//
//struct handshake
//{
//	// <pstrlen><pstr><reserved><info_hash><peer_d>
//	// in version 1.0 of bittorrent protocol, pstrlen = 49, pstr = "BitTorrent protocol"
//	UINT8 pstrlen;			// string length of pstr as a single raw byte
//	std::string pstr;		// string identifier of protocol
//	UINT8 reserved[8]{ 0 };	// 8 reserved bytes, currently all 0
//	std::string info_hah[20];		// 20 byte hash of info-dict
//	std::string peer_id;	// out unique ID we sent to the server
//};