#include "btor.h"
#include "bencode.h"
#include "bfileio.h"
#include "bhttp.h"
#include "butils.h"
#include <iostream>
#include <stdlib.h>
#include <bit>


int main()
{
	std::cout << "btor torrent client\n2021 - Tyler Weston\n";
	// process command line arguments
	// at the bare minimum should take in a .torrent file
	//const char* torrentFilename = "mice_and_men.torrent";
	const char* torrentFilename = "ubuntu2010.torrent";
	
	// where should we put this?
	// might be better just to add some defined conditional compilation stuff since not a lot
	// of computers really change their endianness.
	bool bigEndian;

	// detect if we are big-endian or not
	std::cout << "Detecting endianness: ";
	if constexpr (std::endian::native == std::endian::big)
	{
		std::cout << "Big-endian detected\n";
		bigEndian = true;
	}
	else
	{
		std::cout << "Small-endian detected\n";
		bigEndian = false;
	}

	// we use ONE unique id per session, so let's generate it now
	std::string uniqueId = generateId();

	bf::fileio fileManager;
	fileManager.readFile(torrentFilename);

	// now that we have the torrent file loaded in memory, let's parse it
	be::BMemoryManager bMemoryManager;
	be::BParser bParser;
	be::BObject* root = bParser.parseBencodedString(fileManager.getFileBuf(), &bMemoryManager);
		
	Metainfo metainfo;
	const std::string input = bParser.getCollectedInfoDict();
	metainfo.infodict = bParser.getCollectedInfoDict();
	metainfo.totallength = bParser.getCollectedLength();
	metainfo.announce = *root->getByKey("announce")->getString();

	// gotta open a port to listen on

	// events
	// must send started in first request
	// stopped is sent if the client is shutting down gracefully
	// completed is sent when a tracker completes a download. DON'T send if the download was 100% when client started.
	
	std::cout << "Announcer: " << metainfo.announce << '\n';
	std::string address = metainfo.announce;
	// once we have our announcer, we have to find the server, split at the last '/'

	// send a start request
	auto builtAddress = buildAnnounceParameters(metainfo, uniqueId, "started");
	auto serverAddr = getServerAddress(metainfo.announce);
	auto announcePath = getAnnouncePath(metainfo.announce);

	//std::cout << "Server address: " << serverAddr << '\n';
	//std::cout << "Announce path: " << announcePath << '\n';
	//std::cout << "Get params: " << builtAddress << '\n';
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

	// There could be a response and still be a failure, we'll check for that here.
	std::vector<char> charResponse(response.begin(), response.end());
	be::BObject* serverResponseParser =  bParser.parseBencodedString(&charResponse, &bMemoryManager);
	// Gotta check if there is a "failure reason" key here? Will it have a space in it?

	// std::cout << "Server response: " << *serverResponseParser << '\n';
	int64_t complete = *serverResponseParser->getByKey("complete")->getInt();
	int64_t incomplete = *serverResponseParser->getByKey("incomplete")->getInt();
	int64_t interval = *serverResponseParser->getByKey("interval")->getInt();
	std::cout << "# of seeders: " << complete << '\n';
	std::cout << "# of leechers: " << incomplete << '\n';
	std::cout << "interval: " << interval << '\n';


	// In peers binary model, this will be a string, in dictionary mode this will be a dictionary
	be::BObject_t dictType = serverResponseParser->getByKey("peers")->getType();
	std::vector<std::string> peerList;

	if (dictType == be::BObject_t::BString)
	{
		std::string peers = *serverResponseParser->getByKey("peers")->getString();	// if this fails, we have a dictionary
		// We'll want to convert this string response into some uint_8s first
		// TODO:
		// deal with this later, for now
		std::cout << "Binary mode not supported yet!\n";
		exit(EXIT_FAILURE);
		//// TODO: handle this elsewhere
		//// extract peers
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
		// we have a dictionary of peers
		std::vector<be::BObject*> peers = *serverResponseParser->getByKey("peers")->getList();
		for (auto& p : peers)
		{
			std::string peerIP = *p->getByKey("ip")->getString();
			int port = *p->getByKey("port")->getInt();
			peerList.push_back(peerIP + ":" + std::to_string(port));
		}
		/*
peers: (dictionary model) The value is a list of dictionaries, each with the following keys:
peer id: peer's self-selected ID, as described above for the tracker request (string) (We don't care about peer ID!)
ip: peer's IP address either IPv6 (hexed) or IPv4 (dotted quad) or DNS name (string)
port: peer's port number (integer)
		*/
	}

	std::cout << "Peer addresses:\n";
	bool first = true;
	for (auto peer : peerList)
	{
		//for (char& ch : peer)
		//{
		//	std::cout << std::hex << "Ox" << (int)static_cast<unsigned char>(ch) << ' ';	// ewww... figure out a better way to do this
		//	// apparently our strings hold signed values, but we obvs. want them unsigned so we can do things like this
		//}
		//std::cout << '\n';
		if (not first)
			std::cout << ", ";
		first = false;
		std::cout << peer;
	}
	std::cout << '\n';




	// send a stop request
	std::cout << "Sending stop request to server...";
	builtAddress = buildAnnounceParameters(metainfo, uniqueId, "stopped");
	response = makeGetRequest(serverAddr, "/" + announcePath + builtAddress);
	std::cout << " done\n";
	exit(EXIT_SUCCESS);
}

