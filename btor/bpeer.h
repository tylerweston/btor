#pragma once
#include <array>
#include <stdint.h>
#include <string>
#include <WinSock2.h>

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

// choked = has the remote peer choked the client? when a peer chokes a client, it is a notification that NO requests will be answered until
// the client is unchoked. The client should not attempt to send requests for blocks, and it should consider all pending (unanswered) requests 
// to be discarded.
// interested = whether or not the remote peer is interested in somthing this client has to offer in the remote peer, and if it has the remote
// peer choked or unchoked.

// client connections start out as CHOKED and NOT INTERESTEED

// a block is downloaded by the client when the client is interested in a peer, and that peer is not choking the client.
// a block is uploaded by a client the client is not choking a peer, and that peer is interested in the client.
// IMPORTANT for the client to keep its peers informed as to whether or not it is interested in them.
// This state information should be kept up-to-date with each peer even when the client is choked.
// This will allow peers to know if the client will begin downloading when it is unchoked (and vice-versa)

// Data Types: All integers in the peer wire protocol are encoded as four byte big-endian values.
// This includes the length prefix on all messages after the handshake.
// note that winsock2 has a htonl (host to network long) and ntohl (network to host lost)
// check endianness to see if we need to use htonl and then ntohl


class BPeer
{
private:
	bool amChoking = true;
	bool amInterested = false;
	bool peerChoking = true;
	bool peerInterested = false;
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

