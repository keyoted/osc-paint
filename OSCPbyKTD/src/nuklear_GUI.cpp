#define IS_IMPLEMENTATION
#include "nuklear_GUI.hpp"

guiBND::guiBND(char * title, std::vector<window*> *windows)
: _win(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), title, sf::Style::Default, sf::ContextSettings{24, 8, 4, 3, 3})
, _windows(windows)
{
	/* Platform */
	sf::ContextSettings settings(24, 8, 4, 3, 3);

	_win.setVerticalSyncEnabled(true);
	_win.setActive(true);
	if(!glewInit()) { /* Load OpenGL extensions */
		throw ("Failed to load OpenGL extensions!\n");
	}
	glViewport(0, 0, _win.getSize().x, _win.getSize().y);

	/* GUI */
	_ctx = nk_sfml_init(&_win);
	struct nk_font_atlas *atlas;
	nk_sfml_font_stash_begin(&atlas);
	nk_sfml_font_stash_end();
}

guiBND::~guiBND(){
	nk_sfml_shutdown();
	_win.close();
}

bool guiBND::draw(){


	/* Input */
	sf::Event evt;
	nk_input_begin(_ctx);
	while(_win.pollEvent(evt)) {
		if(evt.type == sf::Event::Closed)
			return false;
		else if(evt.type == sf::Event::Resized)
			glViewport(0, 0, evt.size.width, evt.size.height);

		nk_sfml_handle_event(&evt);
	}
	nk_input_end(_ctx);


	/* GUI */
	if (nk_begin(_ctx, "Windows", nk_rect(20, 20, WINDOW_WIDTH/3.5, WINDOW_HEIGHT/2),
				 NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE))
	{
		// Widgets code here
		nk_layout_row_dynamic(_ctx, 30, 1);

		static const char *weapons[] = {"Fist","Pistol","Shotgun","Plasma","BFG"};
		static int current_weapon = 0;
		current_weapon = nk_combo(_ctx, weapons, NK_LEN(weapons), current_weapon, 25, nk_vec2(200,200));
	}
	nk_end(_ctx);

	/* Draw */
	_win.setActive(true);
	nk_color background;
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.1,0.1,0.1,1);
	nk_sfml_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
	_win.display();
	return true;
}
