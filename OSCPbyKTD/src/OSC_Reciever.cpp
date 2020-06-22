#include "OSC_Reciever.hpp"

int OSCclient::usingSDLNet = 0;

inline bool OSCclient::connectionIsGood(){
	if(state == good)
		return true;
	return false;
}

OSCclientState OSCclient::getState(){
	return state;
}

std::string OSCclient::getError(){
	return error;
}

OSCclient::OSCclient(const char* host, unsigned int port, unsigned int bufferSize){
	
	if(state != uninitialized) return;
	
	// Set variables
	this->port = port;
	this->bufferSize = bufferSize;
	
	// Initialize SDLNet if this is the first class that's using it
	if(usingSDLNet != 0)
		if(SDLNet_Init()==-1){
			state = SDLNetInitERROR;
			error = SDLNet_GetError();
			return;
		}
	++usingSDLNet;

	
	// Try to open a socket
	socket = SDLNet_UDP_Open(port);
	if(!socket) {
		state = SDLNetBadSocketError;
		error = SDLNet_GetError();
		return;
	} needsToCloseSocket = true;

	
	// Set up IP address
	SDLNet_ResolveHost( &IP, host, port);
	
	
	// Bind Socket
	channel = SDLNet_UDP_Bind(socket, -1, &IP);
	if( channel == -1) {
		state = SDLNetSocketFailedToBind;
		error = SDLNet_GetError();
		return;
	}
	
	// Alocate packet
	packet = SDLNet_AllocPacket(bufferSize);
	if(!packet) {
		state = SDLNetPacketFailedToAlocate;
		error = SDLNet_GetError();
		return;
	} needsToFreePacket = true;
	
	state = good;
}

OSCclient::~OSCclient(){
	if(usingSDLNet == 1)
		SDLNet_Quit();
	
	--usingSDLNet;
	
	if (needsToFreePacket) SDLNet_FreePacket(packet);
	if (needsToCloseSocket) SDLNet_UDP_Close(socket);
	
	state = destroyed;
}

int OSCclient::listen(){
	return SDLNet_UDP_Recv(socket, packet);
}

int OSCclient::listen(int ms){
	auto startTime = std::chrono::steady_clock::now();
	
	int ret = 0;
	while (ret == 0) {
		ret = SDLNet_UDP_Recv(socket, packet);
		
		auto TimeInMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
		if( TimeInMs >= ms )return ret;
	}
	return ret;
}

int OSCclient::wait(){
	int ret = 0;
	while (ret == 0) {
		ret = SDLNet_UDP_Recv(socket, packet);
	}
	return ret;
}

const UDPpacket* OSCclient::data(){
	return packet;
}

std::string OSCclient::GetIP() {
	std::string ret;
	for (int x = 0; x<4; ++x)
		ret += std::to_string((IP.host & (0xff << 8 * x)) >> 8 * x) + " . ";
	ret.erase(ret.length()-3);
	return ret;
}

OSCmessage OSCclient::parse(){
	OSCmessage toRet;
	
	// Read the adress
	unsigned int x = 0;
	for(; x < packet->len; ++x){
		if (packet->data[x] == ','){
			++x;
			break;
		}
		if (packet->data[x] == '/')
			toRet.address.emplace_back("");
		else if (packet->data[x] != '\0'){
			toRet.address.back() += packet->data[x];
		}
	}
	
	// Quit if message only has addresses
	if (packet->data[x] == '\0') return toRet;
	
	// Create OSCvalues with the respective types for each one mentioned after the adress portion of the header
	// Node: The suported dataTypes are float, int, blob, string, char and color
	for(; x < packet->len; ++x){
		if (packet->data[x] == '\0'){
			while (++x % 4 != 0 && x < packet->len);
			break;
		}
		
		switch (packet->data[x]) {
			case 'f':
				toRet.values.emplace_back(Float);
				break;
			case 'i':
				toRet.values.emplace_back(Int);
				break;
			case 'b':
				toRet.values.emplace_back(Blob);
				break;
			case 's':
				toRet.values.emplace_back(String);
				break;
			case 'c':
				toRet.values.emplace_back(Char);
				break;
			case 'r':
				toRet.values.emplace_back(Color);
				break;
			default:
				break;
		}
	}
	
	// Create a simple lambda to extract a Uint8[4] from the data
	auto getUint8 = [&]() -> Uint8*{
		Uint8* newU8 = new Uint8[4];
		newU8[3] = packet->data[x];
		newU8[2] = packet->data[++x];
		newU8[1] = packet->data[++x];
		newU8[0] = packet->data[++x];
		return newU8;
	};
	
	// Indicates the OSCvalue we're writing to
	int valuePointer = 0;
	// Actually read the values from the stream
	for(; x < packet->len; ++x){
		while (x % 4 != 0 && x < packet->len)++x;
		
		switch (toRet.values[valuePointer].type){
				
			case Color:
			case Int:{
				Uint8* newint = getUint8();
				toRet.values[valuePointer].set<std::int32_t>((std::int32_t *)newint);
				++valuePointer;
				break;}
				
			case Float:{
				Uint8* newFloat = getUint8();
				toRet.values[valuePointer].set<float>((float*)newFloat);
				++valuePointer;
				break;}
				
			case Char:{
				x += 2;
				char* nchar = new char;
				*nchar = packet->data[++x];
				toRet.values[valuePointer].set<char>(nchar);
				++valuePointer;
				break;}
				
			case String:{
				std::string* nstring = new std::string;
				toRet.values[valuePointer].set<std::string>(nstring);
				while (packet->data[x] != 0) {
					*(toRet.values[valuePointer].get<std::string>()) += packet->data[x++];
				}
				++valuePointer;
				break;}
				
			case Blob:{
				Uint8* size = getUint8();
				std::vector<unsigned char>* nBlob = new std::vector<unsigned char>(*(std::int32_t*)size, '\0');
				delete [] size;
				std::vector<unsigned char>& newBlob = *nBlob;
				for (unsigned int Y = 0; Y < newBlob.size(); ++Y) {
					newBlob[Y] = packet->data[x++];
				}
				toRet.values[valuePointer].set<std::vector<unsigned char>>(nBlob);
				++valuePointer;
				break;}
				
			default:
				break;
		}
	}
	
	return toRet;
}
