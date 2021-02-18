#include "btor.h"
#include "bencode.h"
#include "bfileio.h"
#include "bhttp.h"
#include "butils.h"
#include <iostream>
#include <stdlib.h>

// #define BREAK_EARLY 

int main()
{

	// process command line arguments
	// at the bare minimum should take in a .torrent file
	//const char* torrentFilename = "mice_and_men.torrent";
	const char* torrentFilename = "ubuntu2010.torrent";

	// we use ONE unique id per session, so let's generate it now
	std::string uniqueId = generateId();

	bf::fileio fileManager;
	fileManager.readFile(torrentFilename);

	// now that we have the torrent file loaded in memory, let's parse it
	be::BParser bParser;
	be::BMemoryManager bMemoryManager;
	be::BObject* root = bParser.parseBencodedString(fileManager.getFileBuf(), &bMemoryManager);
	// std::cout << "Torrent info: " << *root << '\n';
		
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
	
	std::string announcer = metainfo.announce;
	std::cout << "Announcer: " << announcer << '\n';
	std::string address = announcer;
	// once we have our announcer, we have to find the server, split at the last '/'

	// send a start request
	auto builtAddress = buildAnnounceParameters(metainfo, uniqueId, "started");
	auto serverAddr = getServerAddress(announcer);
	auto announcePath = getAnnouncePath(announcer);

	std::cout << "Server address: " << serverAddr << '\n';
	std::cout << "Announce path: " << announcePath << '\n';
	std::cout << "Get params: " << builtAddress << '\n';
	std::string response = "";
	std::cout << "Waiting for response from server...\n";
	response = makeGetRequest(serverAddr, "/" + announcePath + builtAddress);	// problem here sometimes, is the request response too big or something? maybe it isn't completed?
#ifdef BREAK_EARLY
	exit(1);
#endif
	if (response != "")
	{
		// maybe move the try up here, this can't be the best way to do this
		std::cout << "Got response from server: " << response << '\n';
	}
	else
	{
		// hmm.
		exit(1);
	}

	std::vector<char> charResponse(response.begin(), response.end());
	be::BObject* serverResponseParser =  bParser.parseBencodedString(&charResponse, &bMemoryManager);
	// Gotta check if there is a "failure reason" key here? Will it have a space in it?

	std::cout << "Server response: " << *serverResponseParser << '\n';
	int64_t complete = *serverResponseParser->getByKey("complete")->getInt();
	int64_t incomplete = *serverResponseParser->getByKey("incomplete")->getInt();
	int64_t interval = *serverResponseParser->getByKey("interval")->getInt();
	std::cout << "# of seeders: " << complete << '\n';
	std::cout << "# of leechers: " << incomplete << '\n';
	std::cout << "interval: " << interval << '\n';
	// peers are 4 bytes, then 2 bytes for a port
	// If we grab peers and they are a string, we are in binary mode
	// Deal with binary first

	// TODO: Find a tracker with more peers to test this
	std::string peers = *serverResponseParser->getByKey("peers")->getString();	// if this fails, we have a dictionary
	// We'll want to convert this string response into some uint_8s first
	std::cout << "Peers: ";

	std::vector<std::string> peerList;
	// extract peers
	for (unsigned int i = 0; i < peers.size(); i += 6)
	{
		peerList.push_back(peers.substr(i, i + 6));
	}
	unsigned int i = 0;
	for (auto& p : peerList)
	{
		std::cout << i++ << ':' << p << '\n';
	}

	for (char& ch : peers)
	{
		std::cout << std::hex << "Ox" << (int)static_cast<unsigned char>(ch) << ' ';	// ewww... figure out a better way to do this
		// apparently our strings hold signed values, but we obvs. want them unsigned so we can do things like this
	}
	std::cout << '\n';
	// otherwise we are a dictionary, and we are in full mode


	// send a stop request
	builtAddress = buildAnnounceParameters(metainfo, uniqueId, "stopped");
	response = makeGetRequest(builtAddress, {});
	exit(EXIT_SUCCESS);

}

