#pragma once
#include <string>
#include "btor.h"
#include "bencode.h"

std::string generateId();
std::string urlEncode(std::string);
void fillMetainfo(Metainfo& metainfo, be::BParser& bParser);
bool is_big_endian(void);

// get total length of all files?