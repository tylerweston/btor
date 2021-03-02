#pragma once
#include <array>
#include <stdint.h>
#include <string>
#include <WinSock2.h>
#include <btor.h>


#define DEFAULT_BUFLEN 512	// move elsewhere


//Data Types
// Unless specified otherwise, all integers in the peer wire protocol are encoded as four byte big - endian values.
// This includes the length prefix on all messages that come AFTER the handshake.

// The peer wire protocol consists of an initial handshake. After that, peers communicate via an exchange of 
// length-prefixed messages. The length-prefix is an integer as described above.

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

// we'll make this peer object responsible for decoding and sending messages to the main loop?

// "suggest those X blocks which have been requested the fewest number of times, 
//	and have not been received"


class BPeer
{
private:
	int peer_num_id;
	// these are the initial values of a peer upon connection
	bool amChoking = true;
	bool amInterested = false;
	bool peerChoking = true;		// when a peer is choking us, we don't send them messages and discard unanswered requests
	bool peerInterested = false;
	std::string _id;
	sockaddr_in peerAddress;		// hold the connection info for this peer
	SOCKET peerSocket;				// one socket per connection
	// each peer should keep track of it's own has bitfield
	// connection state
	// incomplete message data?
	std::string readable_ip;
	int port;
public:
	sockaddr_in* getPeerAddress() { return &this->peerAddress; }
	BPeer(std::string ipaddr, unsigned short port, int peer_num_id);
	~BPeer();
	bool getAmChoking() { return this->amChoking; }
	bool getAmInterested() { return this->amInterested; }
	bool getPeerChoking() { return this->peerChoking; }
	bool getPeerInterested() { return this->peerInterested; }
	void setAmChoking(bool val) { this->amChoking = val; }
	void setAmInterested(bool val) { this->amInterested = val; }
	void setPeerChoking(bool val) { this->peerChoking = val; }
	void setPeerInterested(bool val) { this->peerInterested = val; }
	int tryConnect();
	// int tryAccept(SOCKET ourSocket);
	int tryReceive();
	int sendMessage(const char* msg);
	int closeConnection();
	static std::string getHandshake(BState* state);
	// A peer will want to gather bits of data until it has enough to fill a certain
	// sized buffer or something like that. After which it will hash the data, check
	// it against the stored piece hash, and write it out to the proper file?
};

// messages?

struct have
{
	UINT32 piece_index;		// 0 based index of piece we've received and verified via hash, IS THIS 32-bits?
};

// is this an ok way to do it?
// a message is a len, an id, and then a payload
enum class messages
{
	handshake = -2,			// say hello to a peer
	keepAlive = -1,			// don't get disconnected
	choke = 0,				// no payload
	unchoke = 1,			// no payload
	interested = 2,			// no payload
	notInterested = 3,		// no payload
	have = 4,				// we just got and verified a new piece
	bitfield = 5,			// which pieces we already have available
	request = 6,			// we're looking for a piece!
	piece = 7,				// some data that makes up a piece
	cancel = 8,				// cancel a block request we've made
	port = 9				// only if DHT tracker is supported? Don't implement?
};

/* REQUEST IDEAS:
Strict priortity:
once a single sub-piece has been requested, the remaining subpieces from that particular
block are requested before subpieces from any other block.
this meanss we get full pieces as quickly as possible.

Rarest first:
Want to get the pieces that fewest of our peers have FIRST. 

Random First Piece:
When we're starting, download a random piece until we have ONE full piece, then change to rarest first.

Endgame mode:
Once all sub-pieces which you don't have are actvely being requested, send requests for ALL sub-pieces
to ALL peers. Send cancels for sub-pieces which arrive so we don't download redundant data.

*/
