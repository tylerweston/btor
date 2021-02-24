#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

// todo: need to get this from somewhere?
#define DEFAULT_PORT "6886"

class BSock
{
private:
	// create a socket to listen for client connections
	SOCKET _socket = INVALID_SOCKET;
public:
	BSock();
	// SOCKET getListenSocket() { return this->listenSocket; }	// Don't do this, this socket will contain all socket info
};

BSock::BSock()
{
	// so far this is all just based on the microsoft tutorials to get it up and running
	WSADATA wsaData;

	int iResult;

	// init Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		std::cout << "WSAStartup failed: " << iResult << '\n';
		// TODO: Throw here? Try again? We can't do much if we don't have a socket?
		exit(EXIT_FAILURE);
	}
	// next step for servers: https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-server

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET6;	//IPv6 (so create a DUAL-MODE socket?)	// AF_UNSPEC = address family unspecified
	hints.ai_socktype = SOCK_STREAM;	// Reliable, two-way, connection-based byte streams (TCP)
	hints.ai_protocol = IPPROTO_TCP;	// TCP protocol
	hints.ai_flags = AI_PASSIVE;

	// resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cout << "getaddrinfo failed: " << iResult << '\n';
		WSACleanup();
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	}

	// bind our socket, aka setup the TCP listening socket
	iResult = bind(_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "bind failed with error: " << WSAGetLastError() << '\n';
		freeaddrinfo(result);
		closesocket(_socket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	// once we're bound, we no longer need result:
	freeaddrinfo(result);

	// to listen on a socket:
	if (listen(_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Listen failed: " << WSAGetLastError() << '\n';
		closesocket(_socket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	// here this is a single thread, we will accept connection in multiple threads though!
	// TODO: Multiple threads!
	SOCKET clientSocket;

	clientSocket = INVALID_SOCKET;

	// accept!
	clientSocket = accept(_socket, NULL, NULL);
	// after we've connected we should get a handshake?
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "Accept failed: " << WSAGetLastError() << '\n';
		closesocket(_socket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}
	// look at: https://docs.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock <- advanced winsock samples
	// for ideas of how to make this multi-threaded. For now, once we accept a connection, just start using it!
	// eventually though, a new connection will spawn a new thread to handle that connection?
	
	// to receive and send data on a socket

#define DEFAULT_BUFLEN 512

	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// receive until peer shuts down connection
	do
	{
		// recv and send both return number of bytes transmitted
		// both take a socket, the receiving buffer, the number of bytes to send
		// and any flags to use.
		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			std::cout << "Received " << iResult << " bytes\n";

			// echo results back to sender
			iSendResult = send(clientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				std::cout << "Send failed! " << WSAGetLastError() << '\n';
				closesocket(clientSocket);
				WSACleanup();
				exit(EXIT_FAILURE);
			}
			std::cout << "Sent: " << iResult << " bytes.\n";
		}
		else if (iResult == 0)
		{
			std::cout << "Connection closing...\n";
		}
		else
		{
			std::cout << "Recv failed! " << WSAGetLastError() << '\n';
			closesocket(clientSocket);
			WSACleanup();
			exit(EXIT_FAILURE);
		}
	} while (iResult > 0);

	// shutdown the sending side of the socket (ie, the other side)
	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Shutdown failed! " << WSAGetLastError() << '\n';
		closesocket(clientSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	closesocket(clientSocket);
	WSACleanup();
	

	// next step for clients: https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client



}