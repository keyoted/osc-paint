#pragma once
#include <string>
#include <vector>
#include <utility>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "ResourcePath.hpp"
#include "surface.hpp"
#include "OSC_data.hpp"

enum windowState{
	UNINIT,
	OK,
	ERROR,
	CLOSED
};

class window{
public:
	window(const char* title);
	~window();
	void handleOSC( OSCmessage* message );
	void handleInput();
	void draw();
	void createSurface(float x, float y, float width, float height);
	void createSurface(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4);
	void deleteSurface(unsigned int i);
	void toggleFullScreen();
	std::pair<windowState, std::string> state(){return std::make_pair(_state, _error); }
	bool drawSelectedSurface = true;
private:
	sf::RenderWindow _window;
	sf::Image _icon;
	
	vertex* _movingVert = nullptr;
	unsigned int _selectedSurface = 0;
	unsigned int _maxSizeToGrabVert = 10;
	
	sf::Color _bgColor = sf::Color::Black;
	std::string _title;
	windowState _state = UNINIT;
	std::string _error = "";
	
	std::vector<surface*> _surfaces;
	
	bool _isMousePressed = false, _isFullScreen = false;
	
};
