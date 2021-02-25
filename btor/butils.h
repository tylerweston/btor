#pragma once
#include <string>
#include "btor.h"
#include "bencode.h"

std::string generateId();
void getSHA1(Metainfo& metainfo);
std::string urlEncode(std::string);
void fillMetainfo(Metainfo& metainfo, BParser& bParser);
unsigned long long getBitfieldSize(Metainfo& metainfo);
// verify block hash?
