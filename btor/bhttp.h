#pragma once
#include <map>
#include <string>

std::string testGet(std::string address, std::map<std::string, std::string> parameters);
std::string buildGetRequest(std::string addr, std::string info_hash, std::string id, int length, std::string bEvent);