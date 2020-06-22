#pragma once
#include "nuklear/nuklearIncluder.h"
#include <vector>










struct brushes{
	std::string name;
};

struct surfaces{
	std::vector<brushes*> _brshs;
	std::string name;
};

struct window{
	std::vector<surfaces*> _surfs;
	std::string name;
};













class guiBND{
public:
	guiBND(char * title, std::vector<window*> *windows);
	~guiBND();
	// Returns false when the program should quit
	bool draw();
private:
	struct nk_context *_ctx;
	std::vector<window*> *_windows;
	sf::Window _win;
	unsigned int SWindow=0, SSurface=0, SBrush=0;
};
