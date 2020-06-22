#include "window.hpp"

window::window(const char* title)
: _window(sf::VideoMode(sf::VideoMode::getDesktopMode().width/2,sf::VideoMode::getDesktopMode().height/2), title)
, _title(title)
{
	if (!_icon.loadFromFile(resourcePath() + "icon.png")) {
		_state = ERROR;
		_error = title;
		_error += ": Could not load icon onto window (";
		_error += resourcePath() + "icon.png";
		_error += ")";
		return;
	}
	_window.setIcon(_icon.getSize().x, _icon.getSize().y, _icon.getPixelsPtr());
	
	_state = OK;
}

void window::handleInput(){
	
	float smallestDist;
	sf::Vector2i mousePos;
	
	// Process events
	sf::Event event;
	while (_window.pollEvent(event))
	{
		switch (event.type) {
			// Close window on exit
			case sf::Event::Closed:
				_window.close();
				_state = CLOSED;
				break;
				
				
			// Track weather mouse is pressed on a vertex
			case sf::Event::MouseButtonPressed:
				if (!(event.mouseButton.button == sf::Mouse::Left && _surfaces.size() > 0)) break;
				
				mousePos = sf::Mouse::getPosition(_window);
				if(_selectedSurface >= _surfaces.size()) break;
				for (int curVertex = 0; curVertex < 4; ++curVertex) {
					if(_surfaces[_selectedSurface]->operator[](curVertex).distance(mousePos) <= _maxSizeToGrabVert){
						_movingVert = &_surfaces[_selectedSurface]->operator[](curVertex);
						_movingVert->set(mousePos.x, mousePos.y);
						_isMousePressed = true;
						break;
					}
				}
				break;
				
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left)
					_isMousePressed = false;
				break;
			case sf::Event::MouseLeft:
				_isMousePressed = false;
				break;
				
			
			// Move verticies
			case sf::Event::MouseMoved:
				if(!_isMousePressed || _movingVert == nullptr) break;
				_movingVert->set(event.mouseMove.x, event.mouseMove.y);
				break;
				
			default:
				break;
		}
	}
}

void window::draw(){
	
	_window.setActive();
	
	_window.clear(_bgColor);
	
	for (surface* currSurface: _surfaces) {
		_window.draw(*currSurface);
	}
	
	if(drawSelectedSurface && _selectedSurface < _surfaces.size())
		_surfaces[_selectedSurface]->drawOutline(&_window);
	
	_window.display();
}

void window::createSurface(float x, float y, float width, float height){
	createSurface({x, y}, {x+width, y}, {x+width, y+height}, {x, y+height});
}

void window::createSurface(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4){
	_surfaces.emplace_back(new surface(p1, p2, p3, p4));
}

void window::deleteSurface(unsigned int i) {
	if(i >= _surfaces.size()) return;
	if(i < _selectedSurface) --_selectedSurface;
	else if (i == _selectedSurface) _selectedSurface = 0;
	delete _surfaces[i];
	_surfaces.erase(_surfaces.begin() + i);
}

void window::toggleFullScreen(){
	if(!_isFullScreen){
		_window.close();
		_window.create(sf::VideoMode(sf::VideoMode::getDesktopMode().width,sf::VideoMode::getDesktopMode().height),
					   _title,
					   sf::Style::Fullscreen );
		_isFullScreen = true;
		_window.setMouseCursorVisible(false);
		return;
	}
	_window.close();
	_window.create(sf::VideoMode(sf::VideoMode::getDesktopMode().width/2,sf::VideoMode::getDesktopMode().height/2),_title);
	_window.setMouseCursorVisible(true);
	_isFullScreen = false;
}

window::~window(){
	for(surface* curSurface:_surfaces){
		delete curSurface;
	}
}

void window::handleOSC(OSCmessage* message){
	if(message->address.size() < 1) return;
	
	/* PASS TO SURFACE */
	if(message->address[0] == "surface"){
		if(message->address.size() < 2) return;
		int surf = std::stoi(message->address[1]);
		if( surf < _surfaces.size() && surf > -1){
			message->address.erase(message->address.begin(), message->address.begin()+2);
			_surfaces[surf]->handleOSC(message);
		}
	}
	
	/* CREATE A SURFACE */
	else if(message->address[0] == "create_surface"){
		if(message->values.size() < 4) return;;
		for (unsigned i = 0; i<4; ++i) {
			if(message->values[i].type != Float && message->values[i].type != Int) return;
		}
		createSurface(message->values[0].type == Int?*message->values[0].get<int>():*message->values[0].get<float>(),
					  message->values[1].type == Int?*message->values[1].get<int>():*message->values[1].get<float>(),
					  message->values[2].type == Int?*message->values[2].get<int>():*message->values[2].get<float>(),
					  message->values[3].type == Int?*message->values[3].get<int>():*message->values[3].get<float>());
	}
	
	/* DELETE A SURFACE*/
	else if(message->address[0] == "delete_surface"){
		deleteSurface(*message->values[0].get<int>());
	}
	
	/* TOGGLE FULLSCREEN */
	else if(message->address[0] == "fullscreen") toggleFullScreen();
	
	/* SELECT SURFACE */
	else if(message->address[0] == "select"){
		//for(auto a:message->address) std::cout << a << " / "; std::cout << std::endl;
		if(message->values.size() < 1 || message->values[0].type != Int) return;
		if( *message->values[0].get<int>() < _surfaces.size() && *message->values[0].get<int>() > -1)
			_selectedSurface = *message->values[0].get<int>();
	}
	
	/* TOGGLE SELECTED SURFACE OUTLINE */
	else if(message->address[0] == "outline")
		drawSelectedSurface = !drawSelectedSurface;
}
