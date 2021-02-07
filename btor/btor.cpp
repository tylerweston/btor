#include "bencode.h"
#include "bfileio.h"
#include "bhttp.h"
#include "butils.h"
#include "sha1.hpp"	// move this to a different scope?
#include <iostream>
#include <stdlib.h>


/*
// Algorithm: template<typename T> void lock( T& t, F f ) { lock_guard hold(t); f(); }
lock( mut_x, [&]{
	... use x ...
});
*/

/*
// C++98
rectangle       w( origin(), extents() );   // oops, declares a function, if origin and extents are types
complex<double> c( 2.71828, 3.14159 );
int             a[] = { 1, 2, 3, 4 };
vector<int>     v;
for( int i = 1; i <= 4; ++i ) v.push_back(i);

// C++11
rectangle       w   { origin(), extents() };
complex<double> c   { 2.71828, 3.14159 };
int             a[] { 1, 2, 3, 4 };
vector<int>     v   { 1, 2, 3, 4 };
*/

// The contents of the metainfo file is a bencoded dictionary, containing the following keys:
// info: the dict. described below
// announce: the announce URL of the tracker
// announce-list (optional)
// creation date (optional)
// comment (optional)
// created by (optional)
// encoding (optional)

// info dictionary:
// piece length: number of bytes in each piece (integer)
// pieces: string consisting of the concatentation of all 20-byte SHA1 hash values, one per piece (byte string)
// private (optional)

// single-file mode:
// info dict contains
// name: the filename (string)
// length: length of file in bytes (integer)
// md5sum (optional) 32-char hex string corresponding to MD5 sum of file

// multi-file mode
// name: the name of directorry in which to store all the files
// files: a list of dictionaries, one for each file.
// each dictionary contains the following keys:
//	length: length of file in bytes (integer)
//  md5sum (optional) - 32-char hex string corresponding to MD5 sum of file
//  path: list containing one or more string elements that represnt the path and filename
//		each element in the list corresponds to either a directory name or (in the case of final element),
//		the filename. ie. the file "dir1/dir2/file.ext" would consist of three string elements: "dir1", "dir2", and "file.ext". 
//		This is encoded as a bencoded list of strings such as l4:dir14:dir28:file.exte

// Every piece is of equal length except for the final piece, which is irregular. 
// The number of pieces is thus determined by 'ceil( total length / piece size )'.


int main()
{

	// process command line arguments
	// at the bare minimum should take in a .torrent file

	const char* filename = "mice_and_men.torrent";

	bf::fileio fileManager;
	fileManager.readFile(filename);
	be::BParser bParser;
	be::BMemoryManager bMemoryManager;
	be::BObject* root = bParser.parseBencodedString(fileManager.getFileBuf(), &bMemoryManager);
	// std::cout << "Root: " << *root << '\n';
		
	const std::string input = bParser.getCollectedInfoDict();
	SHA1 checksum;
	checksum.update(input);
	const std::string hash = checksum.final();

	// events
	// must send started in first request
	// stopped is sent if the client is shutting down gracefully
	// completed is sent when a tracker completes a download. DON'T send if the download was 100% when client started.
	std::string uniqueId = generateId();
	int totalLength = bParser.getCollectedLength();
	std::string announcer = *root->getByKey("announce")->getString();
	std::cout << "Announcer: " << announcer << '\n';
	std::string address = announcer;

	// send a start request
	std::string encodedHash = urlEncode(hash);

	auto builtAddress = buildGetRequest(address, encodedHash, uniqueId, totalLength, "started");
	std::cout << "Address: " << builtAddress << '\n';
	std::string response;
	std::cout << "Waiting for response from server...\n";
	response = testGet(builtAddress, {});
	std::cout << "Got response from server: " << response << '\n';
	std::vector<char> charResponse(response.begin(), response.end());
	be::BObject* serverResponseParser =  bParser.parseBencodedString(&charResponse, &bMemoryManager);
	std::cout << "Server response: " << *serverResponseParser << '\n';
	// send a stop request
	builtAddress = buildGetRequest(address, encodedHash, uniqueId, totalLength, "stopped");
	response = testGet(builtAddress, {});
	exit(EXIT_SUCCESS);
	
	// OK so the Root is a metainfo file structure that will contain some useful information for us:
	// info: a dictionary describing the file(s) in the torrent (this will be either single-file or multi-file) (how to tell? if it's a list or not?)
	// announce: the announce URL of the tracker
	// announce-list (optional): list of list of strings (for backwards compat.) DON'T DEAL WITH IT!
	// creation-date (optional): (UNIX epoch format)
	// comment (optional): free form comment from author of torrent
	// created by (optional): name and version of the program used to create the .torrent
	// encoding (optional): string encoding format used to generate the pieces part of the info dictionary

	// info dictionary:
	// piece length: number of bytes in each piece
	// pieces: string consisting of the concat. of all 20-byte sha1 hash values, one per piece (byte string)
	// private (optional)

	// single-file:
	// name: filename (advisory, you can choose a different name?)
	// length: length of file in bytes (integer)
	// md5sum (optional)

	// multi-file
	// name: name of directory to store all files to (advisory)
	// files: list of dictionaries, one per file. Each dictionary contains:
	//	length: length of this file in bytes (integer)
	//	md5sum (optional)
	//	path: a list containing one or more string elements that together represent the path and filename.
	//		each element corresponds to a directory name, and the final element is the name of the file.
	//		ie. the file "dir1/dir2/file.ext" would consist of three string elements: "dir1", "dir2", and 
	//		"file.ext". This is encoded as a bencoded list of strings such as l4:dir14:dir28:file.exte

	// so next step will be reading the bencoded string and putting the information into a more friendly
	// format for us to continue to use

	// create a metainfo class to deal with this!
}

