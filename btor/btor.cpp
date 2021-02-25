#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "btor.h"
#include "bencode.h"
#include "bfileio.h"
#include "bhttp.h"
#include "butils.h"
#include "bpeer.h"
#include <iostream>
#include <stdlib.h>
#include <bit>

#define DEBUG	// make #define NDEBUG to remove assertions!
#include <assert.h>


// TODO: ugly, fix this up
namespace
{
	int exitRequested = 0;
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	if (fdwCtrlType == CTRL_C_EVENT)
	{
		std::cout << "Ctrl-c pressed\n";
		exitRequested = 1;
		return true;
	}
	return false;
}

// otherwise, use a regular ol' signal handler?

int main()
{
	// Initialization

	// Objects
	BState state;
	BMemoryManager bMemoryManager;
	BParser bParser;
	fileio fileManager;

	if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
		std::cout << "Cannot install signal handler!\n";
		exit(EXIT_FAILURE);
	}

	// detect if we are big-endian or not
	// todo: will we need to do this or always just use ntohl or whatnot?
	std::cout << "Detecting endianness: ";
	if constexpr (std::endian::native == std::endian::big)
	{
		std::cout << "Big-endian detected\n";
		state.bigEndian = true;
	}
	else
	{
		std::cout << "Small-endian detected\n";
		state.bigEndian = false;
	}

	// TODO: Process command line args, take in at least a torrent
	state.filename = "ubuntu2010.torrent";		//"mice_and_men.torrent";


	// we use ONE unique id per session, so let's generate it now
	state.uniqueId = generateId();

	std::cout << "\n";
	std::cout << "- - - - - - - - - -\n";
	std::cout << "btor torrent client\n2021 - Tyler Weston\n";
	std::cout << "- - - - - - - - - -\n\n";


	fileManager.readFile(state.filename);



	// now that we have the torrent file loaded in memory, let's parse it
	BObject* root = bParser.parseBencodedString(fileManager.getFileBuf(), &bMemoryManager);
		
	Metainfo metainfo;
	const std::string input = bParser.getCollectedInfoDict();
	metainfo.infodict = bParser.getCollectedInfoDict();
	metainfo.totallength = bParser.getCollectedLength();
	metainfo.piecelength = *root->getByKey("info")->getByKey("piece length")->getInt();
	metainfo.announce = *root->getByKey("announce")->getString();
	getSHA1(metainfo);	// this will fill our metainfo dict with the info_hash_hex
	state.metainfo = &metainfo;

	// check if we alread have an existing bitfield (ie, we are resuming this file)
	std::string bitfield_file_name = state.filename + "bf";
	if (fileManager.doesBitfieldFileExist(bitfield_file_name))
	{
		// read out bitfield file here
		std::cout << "Bitfield file exists, reading it... ";
		state.bitfield = fileManager.readBitfield(bitfield_file_name);
		std::cout << "done\n";
	}
	else
	{
		// make a new empty bitfield file
		std::cout << "Can't find bitfield file, creating it... ";
		unsigned long long bitfield_size = getBitfieldSize(metainfo);
		std::string bitfield_file_name = state.filename + "bf";
		fileManager.createEmptyFile(bitfield_file_name, bitfield_size);
		std::cout << "Created file " << bitfield_file_name << " with size " << bitfield_size << " bytes\n";
		// now read bitfield into out state struct
		state.bitfield = fileManager.readBitfield(bitfield_file_name);
	}
	// OK, by the time we get here, we have our bitfield loaded into our struct

	// events
	// must send started in first request
	// stopped is sent if the client is shutting down gracefully
	// completed is sent when a tracker completes a download. DON'T send if the download was 100% when client started.
	
	std::cout << "Announcer: " << metainfo.announce << '\n';
	std::string address = metainfo.announce;

	// We need to initialize our socket up here so we know what port we are listening on and we can send
	// it to the announcer!

	// send a start request
	auto builtAddress = buildAnnounceParameters(metainfo, state.uniqueId, "started");
	auto serverAddr = getServerAddress(metainfo.announce);
	auto announcePath = getAnnouncePath(metainfo.announce);

	std::string response = "";
	std::cout << "Waiting for response from server... ";
	response = makeGetRequest(serverAddr, "/" + announcePath + builtAddress);	// problem here sometimes, is the request response too big or something? maybe it isn't completed?

	if (response != "")
	{
		// maybe move the try up here, this can't be the best way to do this
		// std::cout << "Got response from server: " << response << '\n';
		std::cout << "done\n";
	}
	else
	{
		std::cout << "\nNo response from server!\n";
		// hmm.
		exit(1);
	}

	std::vector<char> charResponse(response.begin(), response.end());
	BObject* serverResponseParser =  bParser.parseBencodedString(&charResponse, &bMemoryManager);
	
	// Gotta check if there is a "failure reason" key here? Will it have a space in it?
	if (serverResponseParser->checkIfHasKey("failure reason"))
	{
		std::cout << "Failure response from server: " << *serverResponseParser->getByKey("failure reason")->getString() << '\n';
		exit(EXIT_FAILURE);
	}

	int64_t complete = *serverResponseParser->getByKey("complete")->getInt();
	int64_t incomplete = *serverResponseParser->getByKey("incomplete")->getInt();
	int64_t interval = *serverResponseParser->getByKey("interval")->getInt();
	std::cout << "# of seeders: " << complete << '\n';
	std::cout << "# of leechers: " << incomplete << '\n';
	std::cout << "interval: " << interval << '\n';

	// In peers binary model, this will be a string, in dictionary mode this will be a dictionary
	BObject_t dictType = serverResponseParser->getByKey("peers")->getType();

	std::vector<BObject*> peers;
	std::vector<BPeer> bpeers;

	if (dictType == BObject_t::BString)	// binary packed peers
	{
		std::string peers = *serverResponseParser->getByKey("peers")->getString();
		// We'll want to convert this string response into some uint_8s first
		// TODO:
		// deal with this later, for now
		std::cout << "Binary mode not supported yet!\n";
		exit(EXIT_FAILURE);
		//// TODO: handle this elsewhere
		//// extract peers
		// for.each em!
		//for (unsigned int i = 0; i < peers.size(); i += 6)
		//{
		//	// need to translate these here
		//	// first four bytes are the ip address
		//	std::string thisPeer = peers.substr(i, i + 6);
		//	unsigned long bytes = *(unsigned long*)ipv;
		//	unsigned long flipped_bytes = _byteswap_ulong(bytes);
		//	unsigned short port = *(unsigned short*)p;
		//	unsigned short portflipped = _byteswap_ushort(port);
		//	std::cout << "> " << flipped_bytes << ':' << port << '\n';
		//	//for (auto& ch : thisPeer)
		//	//{
		//	//	// peers are 4 bytes, then 2 bytes for a port
		//	//	// interpret first 4 bytes as a long
		//	//	int n = static_cast<int>(static_cast<unsigned char>(ch));
		//	//	std::cout << n << " ";
		//	//}
		//	//	std::cout << std::hex << "Ox" << (int)static_cast<unsigned char>(ch) << ' ';	// ewww... figure out a better way to do this
		//	peerList.push_back(thisPeer);
		//}

	}
	else
	{
		/*
		we have a dictionary of peers:
			peers: (dictionary model) The value is a list of dictionaries, each with the following keys:
			peer id: peer's self-selected ID, as described above for the tracker request (string) (We don't care about peer ID?)
			ip: peer's IP address either IPv6 (hexed) or IPv4 (dotted quad) or DNS name (string)
			port: peer's port number (integer)
		*/
		peers = *serverResponseParser->getByKey("peers")->getList();
		for (auto& p : peers)
		{
			std::string peerIP = *p->getByKey("ip")->getString();
			int port = *p->getByKey("port")->getInt();
			bpeers.push_back(BPeer(peerIP, port));
		}
	}

	// once we get here, we'll have a vector of BPeer objects that will hold their own
	// addresses

	/*
	std::cout << "Peer addresses:\n";
	bool first = true;
	for (auto& peer : peerList)
	{
		if (not first)
			std::cout << ", ";
		first = false;
		std::cout << peer;
	}
	std::cout << '\n';
	*/

	std::string shake = BPeer::getHandshake(&state);	// can we use a string here?
	// or should we use uint8_t or something?
	assert(shake.size() == 68);
	std::cout << "Generated handshake...\n";

	// main loop
	std::cout << "In main loop (ctrl-c to break)\n";
	// when we're here we need a socket that we can listen in on and send messages out of?
	// initiator of a handshake is expected to transmit a handshake immediately.
	// note for the future: separate torrents are on separate TCP connections! Since we're only going to do
	// one torrent at a time, this isn't something for us to worry about!
	
	// We send out handshakes to all the peers first and then wait to hear back from them? 
	while (exitRequested != 1)
	{
		// here is where we try to meet peers. We'll send them a handshake first.
		// this will happen in threads

		// connect to a peer discovered by the announcer
		// create a socket for the peer
		// set socket to non-blocking mode
		// invoke connect
		// if we connect, send our handshake
		// messages that can be at a socket:
		// - incoming handshake response (if we sent a handshake)
		// - incoming handshake request? what is this?
		// - any other of the possible messages
	}
	// clean up threads?


	// send stop message
	// todo: refactor messaging the announcer here since the only difference between these bottom couple
	// lines and the initail started event is the last param. of this buildAnnounceParameters thing.
	builtAddress = buildAnnounceParameters(metainfo, state.uniqueId, "stopped");
	std::cout << "Sending stop request... ";
	response = makeGetRequest(serverAddr, "/" + announcePath + builtAddress);	// problem here sometimes, is the request response too big or something? maybe it isn't completed?
	// TODO: check response
	std::cout << " done\n";
	exit(EXIT_SUCCESS);
}

