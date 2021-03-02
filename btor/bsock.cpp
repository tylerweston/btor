#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <process.h>
#include "bsock.h"

BSock::~BSock()
{
	std::cout << "Closing TCP socket...";
	WSACleanup();
}

int BSock::OpenSocket()
{
	int iResult;
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;	//just try inet for now	// AF_UNSPEC = address family unspecified
	hints.ai_socktype = SOCK_STREAM;	// Reliable, two-way, connection-based byte streams (TCP)
	hints.ai_protocol = IPPROTO_TCP;	// TCP protocol
	hints.ai_flags = AI_PASSIVE;

	// resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);	// NULL = resolve localhost?
	if (iResult != 0)
	{
		std::cout << "getaddrinfo failed: " << iResult << '\n';
		WSACleanup();
		return -1;
	}
	else
	{
		std::cout << "Trying to connect on " << result->ai_addr << '\n';
	}

	/*
	If the server application wants to listen on IPv6, then the address family needs to be set to AF_INET6 in the hints parameter.
	If a server wants to listen on both IPv6 and IPv4, two listen sockets must be created, one for IPv6 and one for IPv4.
	These two sockets must be handled separately by the application.

	Windows Vista and later offer the ability to create a single IPv6 socket that is put in dual stack mode to listen on both IPv6 and IPv4.
	For more information on this feature, see Dual-Stack Sockets. https://docs.microsoft.com/en-us/windows/win32/winsock/dual-stack-sockets
	*/
	_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	// setsockopt(_socket, )

	// check for errors to make sure we have a valid socket
	if (_socket == INVALID_SOCKET)
	{
		std::cout << "Error at socket(): " << WSAGetLastError() << '\n';
		freeaddrinfo(result);	// make result a private member
		WSACleanup();			// call this when we destroy this object
		return -1;
	}

	// bind our socket, aka setup the TCP listening socket
	iResult = bind(_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "bind failed with error: " << WSAGetLastError() << '\n';
		freeaddrinfo(result);
		closesocket(_socket);
		WSACleanup();
		return -1;
	}

	// once we're bound, we no longer need result:
	freeaddrinfo(result);

	// place socket into listening mode
	if (listen(_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Listen failed: " << WSAGetLastError() << '\n';
		closesocket(_socket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	return 0;
}

BSock::BSock()
{
	// Move the socket opening to another function so that we can properly evaluate whether
	// we have been able to open a socket or not?
	std::cout << "Opening TCP socket... ";
	// so far this is all just based on the microsoft tutorials to get it up and running


	int iResult;

	// init Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &this->wsaData);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed: " << iResult << '\n';
		// TODO: Throw here? Try again? We can't do much if we don't have a socket?
		exit(EXIT_FAILURE);
	}

	std::cout << "done\n";
}

void BSock::MakeConnection(BPeer& peer)
{
	if (_socket == INVALID_SOCKET)
	{
		std::cout << "Socket is invalid or unitialized!\n";
		return;
	}
	if (connect(_socket, (sockaddr*)peer.getPeerAddress(), sizeof(peer.getPeerAddress()) == SOCKET_ERROR))
	{
		std::cout << "Failed to connect to peer: " << WSAGetLastError() << '\n';
		return;
	}
	// if we get here, we've made a connection to the peer?
}

