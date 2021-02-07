#include "bhttp.h"
#pragma comment(lib, "ws2_32.lib")  // based on: https://github.com/elnormous/HTTPRequest/issues/17
#include "HTTPRequest.hpp"
#include <iostream>

std::string testGet(std::string address, std::map<std::string, std::string> parameters)
{
    // Ok, so this is sending and receiving messages, good stuff
    try
    {
        // you can pass http::InternetProtocol::V6 to Request to make an IPv6 request
        http::Request request(address);

        // send a get request
        // build parameters here
        const http::Response response = request.send("GET", parameters, {"Content-Type: none", "User-Agent: btor", "Accept: */*", "Connection: Close"});

        // get the result here
        // std::cout << std::string(response.body.begin(), response.body.end()) << '\n'; // print the result
        std::string res(response.body.begin(), response.body.end());
        return res;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
}


std::string //std::map<std::string, std::string> 
buildGetRequest(std::string addr, std::string info_hash, std::string peer_id, int length, std::string bEvent)
{
    return addr + "?info_hash=" + info_hash + "&peer_id=" + peer_id + "&uploaded=0&downloaded=0&left=" + std::to_string(length) + "&event=" + bEvent;
    
    //std::map<std::string, std::string> requestDict = { {"info_hash", info_hash}, 
    //    {"peer_id",  peer_id}, /*{"ip", "104.206.12.211"},*/ {"port", "6881"},  {"compact", "1"},
    //    {"uploaded", "0"}, {"downloaded", "0"}, {"left", std::to_string(length)},
    //    /*{"event", bEvent}*/ };
    //return requestDict;

    //std::cout << "SHA1 info hash: " << hash << '\n';
    //std::cout << "Unique id is: " << generateId() << '\n';
    //int port = 6881;
    //std::cout << "What port we're listening on: " << port << '\n';
    //// uploaded and downloaded both start at 0	?
    //std::cout << "Uploaded: 0\n";
    //std::cout << "Downloaded: 0\n";
    //std::cout << "Left: ???\n";		// figure out size of torrent we need to download
    //std::cout << "Compact: 1\n";	// beconded peer list is replaced by a peer string with 6 bytes/peer. 4 bytes host (network order), then 2 byte port (network order)
    //// events
    //// must send started in first request
    //// stopped is sent if the client is shutting down gracefully
    //// completed is sent when a tracker completes a download. DON'T send if the download was 100% when client started.
    //std::cout << "event: started / stopped / completed\n";


    // tracker get requests have the following keys
    // info_hash: 20 byte sha1 hash of the bencoded form of the info value from metainfo file. will have to be escaped (urlencoded?)
    // peer_id: string of length 20 randomly generated at the start of a new download
    // ip: optional
    // port: port we're listening on. start at 6881, then try 6882, etc., up to 6889
    // uploaded: the total amount uploaded so far, base 10 ascii
    // downloaded: the total amount downloaded so far, base 10 ascii
    // left: the number of bytes this peer still needs to download, in base 10
    // event: optional (started, completed, or stopped)

    // the trackers response will be a bencoded dictionary
    // if the tracker response has a key failure reason, then that maps to a human readable string that explains why the query failed, and no other keys
    // others there are two keys:
    //  - interval: maps to the number of seconds the downloader should wait between regular reqreuests
    //  - peers: peers maps to a list of dictionaries corresponding to peers, each of which contains the following keys:
    //      - peer id: the peers self-selected ID
    //      - ip: IP address or dns name as a string
    //      - port: the port number the machine is using
}