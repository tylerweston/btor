#pragma once
#include <string>
#include "btor.h"
#include "bencode.h"

// bitfield generation here?
/*
Bitfield is function of length of pieces in the info value.
 
The bitfield message may only be sent immediately after the handshaking sequence is completed, 
and before any other messages are sent. It is optional, and need not be sent if a client has no pieces.
(Since it is optional, let's not worry too much about it for now! Just say we don't have any pieces!)
(Next vertical slice will be to just get some greetings going on!)
The bitfield message is variable length, where X is the length of the bitfield.The payload is a 
bitfield representing the pieces that have been successfully downloaded.The high bit in the first 
byte corresponds to piece index 0. Bits that are cleared indicated a missing piece, and set bits 
indicate a validand available piece.Spare bits at the end are set to zero.
*/
std::string generateId();
void getSHA1(Metainfo& metainfo);
std::string urlEncode(std::string);
void fillMetainfo(Metainfo& metainfo, BParser& bParser);
// verify block hask?

// get total length of all files?