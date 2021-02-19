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
        // TODO: check response status here!
        //std::cout << res->status;
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
    // TODO: We can use the openssl SHA1 thing so we don't need an extra library!
    sha1::SHA1 checksum;
    checksum.update(metainfo.infodict);
    const std::string hash = checksum.final();


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
