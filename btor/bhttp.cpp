#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "bhttp.h"
#include <iostream>

std::string makeGetRequest(const std::string address, const std::string path)
{
    // we want to return the response as a string from here
    std::cout << "Server address is: " << address.c_str() << '\n';
    httplib::Client cli(address.c_str());
    std::cout << "Built client, making get request\n";
    std::cout << "path is: " << path.c_str() << '\n';
    if (auto res = cli.Get(path.c_str()))
    {
        std::cout << res->status;
        std::cout << res->body;
        return res->body;
    }
    else
    {
        auto err = res.error();
        std::cerr << "Error: " << helpfulHttpLibError(err) << '\n';
        std::cerr << "Ooops! Problem! Exit!";
        exit(EXIT_FAILURE);
    }
    return "DUMMY";
}

std::string buildAnnounceParameters(Metainfo& metainfo, std::string peer_id, std::string bEvent)
{
    // TODO: This SHA calculation should happen somewhere else
    // TODO: hmm, is this a namespace clash thing with something else now?
    sha1::SHA1 checksum;
    checksum.update(metainfo.infodict);
    const std::string hash = checksum.final();
    //// TODO: We need to move this SHA1 stuff somewhere else now since it's an issue with
    //// the openssl library
    //const unsigned char* unsignedHashStr = reinterpret_cast<const unsigned char*> (metainfo.infodict.c_str());
    //size_t hashSize = metainfo.infodict.size();
    //unsigned char shaOut[20];

    //sha1::SHA1(unsignedHashStr, hashSize, shaOut);
    //unsigned char hash[20];
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

std::string helpfulHttpLibError(int errorNo)
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
