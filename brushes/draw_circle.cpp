#include "OSC_data.hpp"
#include "stencil.hpp"
#include <string>
#include <random>

#define ARRAY write
#define HEIGHT height
#define WIDTH width
#define OSCMES message

#include <cmath>
#define IMPORTDRAW 1

struct data{
	int centerX, centerY, Radius;
};

extern "C"{

	void* start(const unsigned char* read, unsigned int width, unsigned int height, void* customData, const OSCmessage* message){
		data & d = *(new data);
		if(message->values[0].type != String || *message->values[0].get<std::string>() != "relative"){
			d.centerX = *message->values[1].get<int>();
			d.centerY = *message->values[2].get<int>();
			d.Radius = *message->values[3].get<int>();
			return (void*)&d;
		} else {
			d.centerX = (*message->values[1].get<float>()) * (float)width;
			d.centerY = (*message->values[2].get<float>()) * (float)height;
			d.Radius = *message->values[3].get<int>();
			return (void*)&d;
		}
	}

	void* update(const unsigned char* read, unsigned int width, unsigned int height, void* customData, const OSCmessage* message) {
		data & d = *((data*)customData);
		if(message->values.size() >= 1) {
			d.Radius = *message->values[0].get<int>();
			if(message->values.size() >= 2) {
				d.centerX = *message->values[1].get<int>();
				if(message->values.size() >= 3)
					d.centerY = *message->values[2].get<int>();
			}
		}
		return customData;
	}

	void* destroy(void* customData) {
		delete (data*) customData;
		return nullptr;
	}

	void* resize(const unsigned char* read, unsigned int old_width, unsigned int old_height, unsigned int new_width, unsigned int new_height, void* customData) {
		return customData;
	}

	void* draw(const unsigned char* read, unsigned char* &write, unsigned int width, unsigned int height, void* customData) {
		#include "helpLambdas.hpp"

		data & d = *((data*)customData);
		drawCircleProtected(d.centerX, d.centerY, d.Radius, 0xff, 0, 0, 0xff);
		return customData;
	}

	char* name(){return (char*)"DRAW_CIRCLE";}

}

// g++ -std=c++17 draw_circle.cpp -o draw_circle.bru -shared

