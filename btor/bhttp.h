#pragma once
#include <map>
#include <string>
#include "sha1.hpp"
#include "btor.h"
#include "butils.h"

std::string makeGetRequest(std::string address, std::string path);
std::string buildAnnounceParameters(Metainfo& metainfo, std::string peer_id, std::string bEvent);
std::string getServerAddress(std::string announceUrl);
std::string getAnnouncePath(std::string announceUrl);
std::string helpfulHttpLibError(int errorNo);