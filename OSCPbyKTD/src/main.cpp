#include "OSC_Reciever.hpp"
#include <iostream>
#include <SDL2/SDL.h>

#include "window.hpp"
#include "PlugInBrush.hpp"
#include "ResourcePath.hpp"

int main(int, char const** argv) {

	setResourcePathBeguin(std::string{argv[0]});

	OSCclient client("localhost", 5433, 0xFFFF);

	if(client.getState() != good){
		std::cout << client.getError() << std::endl;
		return -1;
	}

	window _window((char *)"OSC by Keyoted");
	{
		auto state = _window.state();
		if(state.first != OK) {
			std::cerr << "Window could not be created! Eror: { " << state.second << " }" << std::endl;
			return 1;
		}
	}

	std::cout << "Started" << std::endl;
	while (true) {
		if(client.listen()) {
			OSCmessage _message = client.parse();

			if(_message.address.size() > 2 || _message.address[0] == "OSCP"){
				if(_message.address[1] == "window"){
					// remove OSCP and WINDOW
					_message.address.erase(_message.address.begin(), _message.address.begin()+2);
					_window.handleOSC(&_message);
				}
				else if(_message.address[1] == "reload_brushes"){
					surface::reloadBrushes();
				}
				else if(_message.address[1] == "quit") return 0;
			}
		}

		_window.handleInput();
		_window.draw();
		if(_window.state().first != OK) return 2;

	}
	return 0;
}
