#pragma once

#include <SDL2/SDL_net.h>
#include <string>
#include <chrono>
#include <vector>
#include <utility>
#include "OSC_data.hpp"

enum OSCclientState{

	uninitialized,
	good,
	destroyed,

	SDLNetInitERROR,
	SDLNetBadSocketError,
	SDLNetSocketFailedToBind,
	SDLNetPacketFailedToAlocate
};

class OSCclient{
public:
	OSCclient(const char* host, unsigned int port, unsigned int bufferSize);
	~OSCclient();

	OSCclientState getState();
	std::string getError();



	// 1 is returned when a packet is received. 0 is returned when no packets are received. -1 is returned on errors.
	int listen();

	// 1 is returned when a packet is received. 0 is returned when no packets are received for ms milliseconds. -1 is returned on errors.
	int listen(int ms);

	// 1 is returned when a packet is received. -1 is returned on errors.
	int wait();

	// Parse the data gatherd by listen
	OSCmessage parse();

	// Returns the raw data returned by paket
	const UDPpacket* data();

	// Get IP
	std::string GetIP();

private:

	inline bool connectionIsGood();

	unsigned int port, bufferSize;
	int channel;
	IPaddress IP;
	UDPsocket socket;
	UDPpacket *packet;

	bool needsToFreePacket = false;
	bool needsToCloseSocket = false;
	static int usingSDLNet;
	OSCclientState state = uninitialized;
	std::string error = "";
};
