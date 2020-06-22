#include "surface.hpp"
#include <iostream>

std::vector<PlugInFunctions*> surface::_brushFunctionsAtlas{};

surface::surface(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4)
: _verts(sf::Quads, 4)
{
	_vPointer[0] = new vertex(this, &_verts[0]);
	_vPointer[1] = new vertex(this, &_verts[1]);
	_vPointer[2] = new vertex(this, &_verts[2]);
	_vPointer[3] = new vertex(this, &_verts[3]);

	_verts[0].position = p1;
	_verts[1].position = p2;
	_verts[2].position = p3;
	_verts[3].position = p4;

	_texture.setSmooth(true);

	_perspectiveShader.loadFromMemory("void main(){gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;gl_TexCoord[0]=gl_TextureMatrix[0]*gl_MultiTexCoord0;gl_TexCoord[0].z=((gl_Color.r*256.0)+gl_Color.g+(gl_Color.b/256.0))*256.0;gl_FrontColor=gl_Color;}", "uniform sampler2D texture;void main(){vec4 color=vec4(1.0,1.0,1.0,gl_Color.a);vec2 texCoord=gl_TexCoord[0].xy/gl_TexCoord[0].z;gl_FragColor=color*texture2D(texture,texCoord);}");


	_texture.loadFromFile(resourcePath() + "test.png");
	_isDefaultTextureLoaded = true;
}

surface::~surface(){
	delete _vPointer[0];
	delete _vPointer[1];
	delete _vPointer[2];
	delete _vPointer[3];
	_vPointer[0] = _vPointer[1] = _vPointer[2] = _vPointer[3] = nullptr;
}

vertex& surface::operator[](int i){
	switch (i%4) {
		case 0: return *_vPointer[0];
		case 1: return *_vPointer[1];
		case 2: return *_vPointer[2];
		case 3: return *_vPointer[3];
		default: return *_vPointer[3]; // Shut up warning. Control never reaches this.
	}
}

void surface::updateTexture() const{
	_isTextureUpdated = true;

	const sf::Vector2f centrePoint{ linesIntersection(_verts[0].position, _verts[2].position, _verts[1].position, _verts[3].position) };
	const float distanceToCentre0{ distanceBetweenPoints(_verts[0].position, centrePoint) };
	const float distanceToCentre1{ distanceBetweenPoints(_verts[1].position, centrePoint) };
	const float distanceToCentre2{ distanceBetweenPoints(_verts[2].position, centrePoint) };
	const float distanceToCentre3{ distanceBetweenPoints(_verts[3].position, centrePoint) };

	_vertWeights[0] = (distanceToCentre0 + distanceToCentre2) / distanceToCentre2;
	_vertWeights[1] = (distanceToCentre1 + distanceToCentre3) / distanceToCentre3;
	_vertWeights[2] = (distanceToCentre2 + distanceToCentre0) / distanceToCentre0;
	_vertWeights[3] = (distanceToCentre3 + distanceToCentre1) / distanceToCentre1;

	_verts[0].texCoords = { 0, 0 };
	_verts[1].texCoords = { _vertWeights[1] * _texture.getSize().x, 0 };
	_verts[2].texCoords = { _vertWeights[2] * _texture.getSize().x, _vertWeights[2] * _texture.getSize().y };
	_verts[3].texCoords = { 0, _vertWeights[3] * _texture.getSize().y};

	_verts[0].color = encodeFloatAsColor(_vertWeights[0]);
	_verts[1].color = encodeFloatAsColor(_vertWeights[1]);
	_verts[2].color = encodeFloatAsColor(_vertWeights[2]);
	_verts[3].color = encodeFloatAsColor(_vertWeights[3]);

	_perspectiveShader.setUniform( "texture", _texture);
}

void surface::draw(sf::RenderTarget& target, sf::RenderStates states) const{
	if(!_isTextureUpdated) updateTexture();

	// Update texture
	if(_brushes.size() == 0){
		if(!_isDefaultTextureLoaded){
			_texture.loadFromFile(resourcePath() + "test.png");
			_isDefaultTextureLoaded = true;
		}
	}
	else{
		stencil newTexture(_texture.getSize().x, _texture.getSize().y);
		for (auto brush: _brushes) {
			brush->draw(newTexture.pixels);
			newTexture += brush->_colorData;
		}
		_texture.update(newTexture.pixels);
	}

	states.texture = &_texture;
	states.shader = &_perspectiveShader;
	target.setActive();
	target.draw(_verts, states);
}

void surface::drawOutline(sf::RenderWindow* window){
	sf::Color tmpColors[4] = {_verts[0].color,_verts[1].color,_verts[2].color,_verts[3].color};

	_verts[0].color = sf::Color::Red;
	_verts[1].color = sf::Color::Red;
	_verts[2].color = sf::Color::Red;
	_verts[3].color = sf::Color::Red;

	sf::Vertex line[] = { _verts[0], _verts[1] };
	window->draw(line, 2, sf::Lines);
	line[0] = _verts[2];
	window->draw(line, 2, sf::Lines);
	line[1] = _verts[3];
	window->draw(line, 2, sf::Lines);
	line[0] = _verts[0];
	window->draw(line, 2, sf::Lines);

	_verts[0].color = tmpColors[0];
	_verts[1].color = tmpColors[1];
	_verts[2].color = tmpColors[2];
	_verts[3].color = tmpColors[3];
}

void surface::handleOSC(OSCmessage *message){

	if (message->address.size() < 1) return;

	/* ADD BRUSH */
		// ex: /OSCP/window/surface/0/add_brush/my_brush_name "./path/to/my brush/b.bru" 45 75 36
		// the path to the brush is only necessary if the brush is not in the atlas
		// all values after the " will be passed to the brush's start function
	if (message->address[0] == "create_brush"){

		if (message->address.size() < 2) return;
		bool isInAtlas = false;
		int placeInAtlas = 0; // if isInAtlas this is the index of the functions
		for(unsigned i = 0; i<_brushFunctionsAtlas.size(); ++i){
			if(_brushFunctionsAtlas[i]->getName() == message->address[1]){
				isInAtlas = true;
				break;
			}
			++placeInAtlas;
		}

		// Add functions to atlas
		if(!isInAtlas){
			if(message->values.size() <= 0) return;
			std::string path_to_lib = "";
			unsigned removeMessagesUpTo = 0;
			for(auto a:message->values){
				if(a.type != String) break;
				path_to_lib += *a.get<std::string>();
				++removeMessagesUpTo;
				if(path_to_lib[path_to_lib.size()-1] == '"') break;
				else path_to_lib += ' ';
			}
			if(path_to_lib.size() == 0) return;
			if(path_to_lib[0] != '"' || path_to_lib[path_to_lib.size()-1] != '"') return;
			message->values.erase(message->values.begin(), message->values.begin() + removeMessagesUpTo);
			path_to_lib.erase(path_to_lib.begin());
			path_to_lib.erase(path_to_lib.end()-1);
			_brushFunctionsAtlas.emplace_back(new PlugInFunctions(path_to_lib.c_str()));
			if(!_brushFunctionsAtlas.back()->isGood()){
				_brushFunctionsAtlas.pop_back();
				return;
			}
			placeInAtlas = _brushFunctionsAtlas.size() - 1;
		}
		// Remove path if it was not omited
		else if(message->values.size() <= 0 || message->values[0].type != String || message->values[0].get<std::string>()->at(0) != '"') {
			unsigned removeMessagesUpTo = 0;
			for(auto a:message->values){
				if(a.type != String) break;
				++removeMessagesUpTo;
				if( (*a.get<std::string>())[(*a.get<std::string>()).length()-1] == '"') break;
			}
			message->values.erase(message->values.begin(), message->values.begin() + removeMessagesUpTo);
		}
		// Add brush to brushes
		_brushes.emplace_back(new PlugInBrush(_brushFunctionsAtlas[placeInAtlas], _texture.getSize().x, _texture.getSize().y, message));
		_isDefaultTextureLoaded = false;
	}

	/* DELETE BRUSH */
	else if(message->address[0] == "delete_brush"){
		if (message->address.size() < 2) return;
		int brushindex = std::stoi(message->address[1]);
		if(brushindex >= _brushes.size()) return;
		delete _brushes[brushindex];
		_brushes.erase(_brushes.begin() + brushindex);
	}

	/* UPDATE BRUSH */
	else if(message->address[0] == "update_brush"){
		if (message->address.size() < 2) return;
		int brushindex = std::stoi(message->address[1]);
		if(brushindex >= _brushes.size()) return;
		message->address.erase(message->address.begin(), message->address.begin() + 2);
		_brushes[brushindex]->update(nullptr, message);
	}
}

void surface::reloadBrushes(){
	for(auto func:_brushFunctionsAtlas){
		func->reload();
	}
}


void vertex::set(float x, float y){s->_isTextureUpdated = false; v->position.x = x; v->position.y = y;};
void vertex::operator=(sf::Vertex V){s->_isTextureUpdated = false; *v = V;}
void vertex::setx(float x){s->_isTextureUpdated = false; v->position.x = x;};
void vertex::sety(float y){s->_isTextureUpdated = false; v->position.y = y;};
sf::Vector2f vertex::get() const {return v->position;}
float vertex::getx() const {return v->position.x;}
float vertex::gety() const {return v->position.y;}
float vertex::distance(sf::Vector2i point) const { return distance(point.x, point.y);}
float vertex::distance(float x, float y) const {
	float x_diff = (x - v->position.x);
	float y_diff = (y - v->position.y);
	return std::sqrt(x_diff * x_diff + y_diff * y_diff);
}
