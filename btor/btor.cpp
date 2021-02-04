#include "bencode.h"
#include "bfileio.h"
#include <iostream>

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
	//const char* filename = "bencode_dict_test.torrent";
	//const char* filename = "bencode_test_list.torrent";
	//const char* filename = "bencode_test_string.torrent";
	//const char* filename = "bencode_test_list_of_list.torrent";
	bf::fileio fileManager;
	fileManager.read_file(filename);
	be::BParser bParser;
	be::BMemoryManager bMemoryManager;
	be::BObject* root = bParser.parseBencodedString(fileManager.getFileBuf(), &bMemoryManager);
	// doesn't look like this root is being returned properly, have to figure out why!
	// printing it is throwing an error :(
	std::cout << "Root: " << *root << '\n';
}