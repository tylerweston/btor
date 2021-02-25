#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "bhttp.h"
#include <iostream>


std::string makeGetRequest(const std::string address, const std::string path)
{
    // we want to return the response as a string from here
    httplib::Client cli(address.c_str());
    if (auto res = cli.Get(path.c_str()))
    {
        if (res->status != 200)
        {
            std::cout << "Bad response from tracker:\n";
            std::cout << interpretTrackerResponseCode(res->status);
            // TODO: Move this exit up? Throw?
            exit(EXIT_FAILURE);
        }
        std::cout << "200 ";
        return res->body;
    }
    else
    {
        auto err = res.error();
        std::cerr << "Error: " << interpretHttpLibError(err) << '\n';
        std::cerr << "Ooops! Problem! Exit!";
        exit(EXIT_FAILURE);
    }
    return "DUMMY";
}

std::string buildAnnounceParameters(Metainfo& metainfo, std::string peer_id, std::string bEvent)
{
    // TODO: We only need to build this once, then append different events to the end of it
    const std::string hash = metainfo.info_hash_hex;
    const std::string encodedHash = urlEncode(hash);
    // build our announce path
    return "?info_hash=" + encodedHash + "&peer_id=" + peer_id +
        "&uploaded=0&downloaded=0&left=" + std::to_string(metainfo.totallength) + "&event=" + bEvent + "&port=6886";
}

std::string getServerAddress(std::string announceUrl)
{
    // takes in something like https://torrent.ubuntu.com/announce and just return the server address
    // part, in this case https://torrent.ubunutu.com
    const unsigned int pathSeperatorIndex = announceUrl.find_last_of('/');
    if (pathSeperatorIndex == std::string::npos)
    {
        std::cerr << "Error: Cannot extract server address from " << announceUrl << '\n';
        exit(EXIT_FAILURE);
    }
    return announceUrl.substr(0, pathSeperatorIndex);
}

std::string getAnnouncePath(std::string announceUrl)
{
    // takes in something like https://torrent.ubuntu.com/announce and just return the announce path part
    // part, in this case announce
    const unsigned int pathSeperatorIndex = announceUrl.find_last_of('/');
    if (pathSeperatorIndex == std::string::npos)
    {
        std::cerr << "Error: Cannot extract announce path from " << announceUrl << '\n';
        exit(EXIT_FAILURE);
    }
    return announceUrl.substr(pathSeperatorIndex + 1);
}

std::string interpretHttpLibError(int errorNo)
{
    const std::string Errors[12]{
        "Success",
        "Unknown",
        "Connection",
        "BindIPAddress",
        "Read",
        "Write",
        "ExceedRedirectCount",
        "Canceled",
        "SSLConnection",
        "SSLLoadingCerts",
        "SSLServerVerification",
        "UnsupportedMultipartBoundaryChars"
    };
    return Errors[errorNo];
}

std::string interpretTrackerResponseCode(int responseCode)
{
    switch (responseCode)
    {
    case 100:
        return "Invalid request type: Client request was not an HTTP GET";
    case 101:
        return "Missing info_hash";
    case 102:
        return "Missing peer_id";
    case 103:
        return "Missing port";
    case 150:
        return "Invalid info_hash: info_hash is not 20 bytes long";
    case 151:
        return "Invalid peerid: peerid is not 20 bytes long";
    case 152:
        return "Invalid numwant. Client requested more peers than allowed by tracker";
    case 200:
        return "info_hash not found in database.";
    case 500:
        return "Client sent an eventless request before the specified time.";
    case 900:
    default:
        return "Generic error";
    }
}
