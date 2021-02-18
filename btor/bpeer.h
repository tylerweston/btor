#pragma once
#include <array>
#include <stdint.h>
#include <string>

//Data Types
// Unless specified otherwise, all integers in the peer wire protocol are encoded as four byte big - endian values.
// This includes the length prefix on all messages that come after the handshake.

// The peer wire protocol consists of an initial handshake. After that, peers communicate via an exchange of 
// length-prefixed messages. The length-prefix is an integer as described above.

//
//Handshake
//The handshake is a required messageand must be the first message transmitted by the client.It is(49 + len(pstr)) bytes long.
//
//handshake: <pstrlen><pstr><reserved><info_hash><peer_id>
//
//pstrlen : string length of <pstr>, as a single raw byte
//pstr : string identifier of the protocol
//reserved : eight(8) reserved bytes.All current implementations use all zeroes.Each bit in these bytes can be used to 
//change the behavior of the protocol.An email from Bram suggests that trailing bits should be used first, so that leading bits 
//may be used to change the meaning of trailing bits.
//info_hash : 20 - byte SHA1 hash of the info key in the metainfo file.This is the same info_hash that is transmitted in tracker requests.
//peer_id : 20 - byte string used as a unique ID for the client.This is usually the same peer_id that is transmitted in tracker 
//requests(but not always e.g.an anonymity option in Azureus).

// so we will have a vector of peers?
// each will run in it's own thread?
// but only one can write to our files at the same time?

// will the peers keep track of their own address? I suppose so?

class BPeer
{
private:
	bool amChoking;
	bool amInterested;
	bool peerChoking;
	bool peerInterested;
	std::string _id;
	std::array<uint8_t, 4> addr;
	std::array<uint8_t, 2> port;
public:
	bool getAmChoking() { return this->amChoking; }
	bool getAmInterested() { return this->amInterested; }
	bool getPeerChoking() { return this->peerChoking; }
	bool getPeerInterested() { return this->peerInterested; }
	void setAmChoking(bool val) { this->amChoking = val; }
	void setAmInterested(bool val) { this->amInterested = val; }
	void setPeerChoking(bool val) { this->peerChoking = val; }
	void setPeerInterested(bool val) { this->peerInterested = val; }
};

