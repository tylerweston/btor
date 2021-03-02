#include "bpeer.h"
#include "butils.h"
#include <WS2tcpip.h>
#include <iostream>


// don't choke and unchoke quickly. 
BPeer::BPeer(std::string ipaddr, unsigned short port, int peer_num_id)
{
	this->readable_ip = ipaddr;
	this->port = port;
	this->peer_num_id = peer_num_id;
	this->peerSocket = INVALID_SOCKET;
	memset(&this->peerAddress.sin_zero, 0, sizeof(this->peerAddress.sin_zero));
	InetPton(AF_INET, (PCWSTR)ipaddr.c_str(), &this->peerAddress.sin_addr.s_addr);
	this->peerAddress.sin_port = htons(port);
	this->peerAddress.sin_family = AF_INET;
	this->peerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->peerSocket == INVALID_SOCKET)
	{
		std::cout << "Couldn't create socket in peer #" << this->peer_num_id << "\n";
	}
	else
	{
		std::cout << "Socket for peer# " << this->peer_num_id << " sucesfully created.\n";
	}
}

BPeer::~BPeer()
{
	std::cout << "Closing socket for peer# " << this->peer_num_id << "\n";
	closesocket(this->peerSocket);
}

//// this should be tryConnect since WE are trying to connect with a PEER
//int BPeer::tryAccept(SOCKET ourSocket)
//{
//	// WE NEED OUR PARENT SOCKET
//	// we don't wait to hear from a peer here?!
//	this->peerSocket = accept(ourSocket, NULL, NULL);
//	return 0;
//}

int BPeer::tryConnect()
{
	std::cout << "Attempting to conect to peer #" << this->peer_num_id << "\n";
	if (connect(this->peerSocket, (sockaddr*)&this->peerAddress, sizeof(this->peerAddress)) < 0)
	{
		// Here, we'll close the socket and mark peer at (temporarily?) inactive.
		std::cout << "Cannot connect to peer #" << this->peer_num_id <<  "\n";
		std::cout << "Address: " << this->readable_ip << ":" << this->port << '\n';
		std::cout << "Reason: " << WSAGetLastError() << "\n\n";
		return -1;
	}
	else
	{
		std::cout << "Connected sucesfully to peer #" << this->peer_num_id <<  "\n";
		return 0;
	}
}

int BPeer::sendMessage(const char* msg)
{
	int iResult;

	iResult = send(this->peerSocket, msg, (int)strlen(msg), 0);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Send failed with error: " << WSAGetLastError();
		// We need to do something here!
		closesocket(this->peerSocket);
		return -1;
	}
	return 0;
}

int BPeer::tryReceive()
{
	fd_set check_this;
	const timeval t = { .tv_sec = 0, .tv_usec = 50 };
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	FD_ZERO(&check_this);
	FD_SET(this->peerSocket, &check_this);
	select(0, &check_this, NULL, NULL, &t);
	// receive until peer shuts down connection
	// do NOT do this in a loop since it will be blocking, we need to just see
	// if we have any data, and if we do, read it and process it, otherwise
	// just move onto the next peer. Keep spinning!
	//do
	//{

	// recv and send both return number of bytes transmitted
	// both take a socket, the receiving buffer, the number of bytes to send
	// and any flags to use.

	// We need to make sure that we are ready to receive data on this socket FIRST
	// or else recv will return something bad.
	// We can do this using select!
	if (FD_ISSET(this->peerSocket, &check_this))	// this is not the best way to do this, or is it?
	{
		iResult = recv(this->peerSocket, recvbuf, recvbuflen, 0);	// try to receive
		if (iResult > 0)
		{
			std::cout << "Received " << iResult << " bytes\n";

			// echo results back to sender
			iSendResult = send(this->peerSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				std::cout << "Send failed! " << WSAGetLastError() << '\n';
				closesocket(this->peerSocket);
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
			closesocket(this->peerSocket);
			WSACleanup();
			exit(EXIT_FAILURE);
		}

		//} while (iResult > 0);
	}
}

int BPeer::closeConnection()
{
	int iResult;
	// This will happen in an individual peer block
	// shutdown the sending side of the socket (ie, the other side)
	iResult = shutdown(this->peerSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		std::cout << "Shutdown failed! " << WSAGetLastError() << '\n';
		closesocket(this->peerSocket);
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	closesocket(this->peerSocket);
}


std::array<uint8_t, 6> peerStringToByteArray(std::string peerString)
{
	// convert a peer string to an array of 6 bytes
	return { 0,0,0,0,0,0 };
}

// take in 6 bytes and return 6 bytes
// first 4 bytes need to be switched from big endian to little endian
// last 2 bytes are the port number
// 
std::array<uint8_t, 6> parsePeer(std::array<uint8_t, 6> peerBytes)
{
	return { 0,0,0,0,0,0 };
}


std::string BPeer::getHandshake(BState* state)
{
	// <pstrlen><pstr><reserved><info_hash><peer_d>
	// in version 1.0 of bittorrent protocol, pstrlen = 49, pstr = "BitTorrent protocol"
	// PROBLEM: Hash here is 40 chars long? That doesn't seem right?
	// hash is in HEX, it should be chars?
	std::string shake = "5BitTorrent protocol00000000";
	for (auto& c : state->metainfo->info_hash_raw)	// TODO: better way to do this, memcpy or something?
		shake += c;
	shake += state->unique_id;
	return shake;
}
