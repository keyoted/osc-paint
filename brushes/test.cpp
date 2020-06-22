#include "OSC_data.hpp"
#include "stencil.hpp"
#include <string>
#include <random>
#include <cstdio>

#define ARRAY write
#define HEIGHT height
#define WIDTH width
#define OSCMES message
#include <cmath>

struct data{
	int x0, y0, x1, y1;
};

extern "C"{

	void* start(const unsigned char* read, unsigned int width, unsigned int height, void* customData, const OSCmessage* message){
		#define IMPORTDRAW 0
		#include "helpLambdas.hpp"
		#undef IMPORTDRAW

		data & d = *(new data);
		d.x0 = sGetInt(message, 0);
		d.y0 = sGetInt(message, 1);
		d.x1 = sGetInt(message, 2);
		d.y1 = sGetInt(message, 3);
		return (void*)&d;
	}

	void* update(const unsigned char* read, unsigned int width, unsigned int height, void* customData, const OSCmessage* message) {
		#define IMPORTDRAW 0
		#include "helpLambdas.hpp"
		#undef IMPORTDRAW

		data & d = *(new data);
		d.x0 = sGetInt(message, 0);
		d.y0 = sGetInt(message, 1);
		d.x1 = sGetInt(message, 2);
		d.y1 = sGetInt(message, 3);
		return (void*)&d;
	}

	void* destroy(void* customData) {
		delete (data*) customData;
		return nullptr;
	}

	void* resize(const unsigned char* read, unsigned int old_width, unsigned int old_height, unsigned int new_width, unsigned int new_height, void* customData) {
		return customData;
	}

	void* draw(const unsigned char* read, unsigned char* &write, unsigned int width, unsigned int height, void* customData) {
		#define IMPORTDRAW 1
		#include "helpLambdas.hpp"
		#undef IMPORTDRAW

		data & d = *((data*)customData);
		drawLineProtected(d.x0, d.y0, d.x1, d.y1, 0xff, 0, 0, 0xff);
		return customData;
	}

	char* name(){return (char*)"TEST";}

}

// g++ -std=c++17 test.cpp -o test.bru -shared -g

