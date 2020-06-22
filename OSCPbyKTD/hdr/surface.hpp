#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>
#include "ResourcePath.hpp"
#include "OSC_data.hpp"
#include "PlugInBrush.hpp"

class vertex;

class surface : public sf::Drawable {
	friend class vertex;
public:
	surface(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4);
	~surface();
	vertex& operator[](int i);
	void setOpacity(sf::Uint8 opacity){this->opacity = opacity;}
	void handleOSC(OSCmessage* message);
	void drawOutline(sf::RenderWindow* window);
	static void reloadBrushes();
private:
	sf::Uint8 opacity = 0xFF;
	vertex* _vPointer[4];
	std::vector<PlugInBrush*> _brushes;
	static std::vector<PlugInFunctions*> _brushFunctionsAtlas;
	
	// These get changed in draw
	mutable sf::Texture _texture;
	mutable float _vertWeights[4];
	mutable sf::VertexArray _verts;
	mutable bool _isTextureUpdated = false, _isDefaultTextureLoaded = false;
	mutable sf::Shader _perspectiveShader;
	
	void update() const;
	void updateTexture() const;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	inline sf::Color encodeFloatAsColor(const float f) const {
		return{
			static_cast<sf::Uint8>(static_cast<unsigned int>(f / 256) & 0xFF),
			static_cast<sf::Uint8>(static_cast<unsigned int>(f) & 0xFF),
			static_cast<sf::Uint8>(static_cast<unsigned int>(f * 256) & 0xFF),
			opacity };
	}
	
	inline sf::Vector2f linesIntersection(const sf::Vector2f aStart, const sf::Vector2f aEnd, const sf::Vector2f bStart, const sf::Vector2f bEnd) const {
		const sf::Vector2f a{ aEnd - aStart };
		const sf::Vector2f b{ bEnd - bStart };
		const sf::Vector2f c{ aStart - bStart };
		const float alpha{ ((b.x * c.y) - (b.y * c.x)) / ((b.y * a.x) - (b.x * a.y)) };
		return aStart + (a * alpha);
	}
	
	inline float distanceBetweenPoints(const sf::Vector2f a, const sf::Vector2f b) const {
		const sf::Vector2f c{ a - b };
		return std::sqrt(c.x * c.x + c.y * c.y);
	}
};








class vertex{
public:
	vertex(surface* s, sf::Vertex* v):s(s), v(v){};
	void set(float x, float y);
	void operator=(sf::Vertex V);
	void setx(float x);
	void sety(float y);
	sf::Vector2f get() const;
	float distance(sf::Vector2i point) const;
	float distance(float x, float y) const;
	float getx() const;
	float gety() const;
private:
	surface* s;
	sf::Vertex* v;
};
