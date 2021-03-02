#pragma once
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <string>
#include "btor.h"
#include "bencode.h"

// FormatMessage for getting readable results from WSAGetLastError?
// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-formatmessage

// verify block hash?
std::string generateId();
void getSHA1(Metainfo& metainfo);
std::string urlEncode(std::string);
void fillMetainfo(Metainfo& metainfo, BParser& bParser);

// Bitfield editing stuff
unsigned long long getBitfieldSize(Metainfo& metainfo);
void setBitInBitfield(std::vector<uint8_t>& bitfield, const int bit_to_set);
bool isBitInBitfieldSet(const std::vector<uint8_t>& bitfield, int bit_to_check);

// Console drawing stuff, maybe make this curses stuff?
void show_status(float percentage);
void ShowConsoleCursor(bool showFlag);
