#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <process.h>
#include "bpeer.h"

// todo: need to get this from somewhere?
#define DEFAULT_PORT "6886"


class BSock
{
private:
	WSADATA wsaData;
	SOCKET _socket = INVALID_SOCKET;
public:
	BSock();
	~BSock();
	int OpenSocket();
	void MakeConnection(BPeer& peer);
};

