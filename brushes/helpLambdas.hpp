#ifndef ARRAY
assert(false);
#endif

#ifndef HEIGHT
assert(false);
#endif

#ifndef WIDTH
assert(false);
#endif

#ifndef OSCMES
assert(false);
#endif

#ifndef IMPORTDRAW
#define IMPORTDRAW 0
#endif

struct color{
	unsigned char colorR;
	unsigned char colorG;
	unsigned char colorB;
	unsigned char colorA;
};

/* Safetly get types */
auto sGetInt = [](const OSCmessage* message, unsigned int index) -> int{
	if(message->values.size() <= index) return 0;
	switch(message->values[index].type){
		case Int: return *message->values[index].get<int>();
		case Float: return (int)(*message->values[index].get<float>());
		case Char: return (int)(*message->values[index].get<char>());
		default: return 0;
	}
};

auto sGetFloat = [](const OSCmessage* message, unsigned int index) -> float{
	if(message->values.size() <= index) return 0;
	switch(message->values[index].type){
		case Int: return *message->values[index].get<int>();
		case Float: return (float)(*message->values[index].get<float>());
		case Char: return (float)(*message->values[index].get<char>());
		default: return 0;
	}
};




/* Helping funcs */
auto diatanceBetweenPoints = [&](int ax, int ay, int bx, int by) -> int{
	int cx = ax - bx;
	int cy = ay - by;
	return std::sqrt(cx * cx + cy * cy);
};



#if IMPORTDRAW
/* Drawing primitives */

// Colors
auto writeColor = [&](int x, int y, int RGBA, unsigned char color) {
	ARRAY[(y*WIDTH*4) + (x*4) + RGBA] = color;
};

auto writePixel = [&](int x, int y, int colorR, int colorG, int colorB, int colorA) {
	int cache = (y*WIDTH*4) + (x*4);
	ARRAY[cache + 0] = colorR;
	ARRAY[cache + 1] = colorG;
	ARRAY[cache + 2] = colorB;
	ARRAY[cache + 3] = colorA;
};

// Lines
auto drawLine = [&](int x0, int y0, int x1, int y1, int colorR, int colorG, int colorB, int colorA) {
	int dx, dy, p, x, y;
	dx=x1-x0;
	dy=y1-y0;
	p=2*dy-dx;

	while(x0<x1) {
		if(p>=0) {
			writePixel(x0, y0, colorR, colorG, colorB, colorA);
			++y0;
			p=p+2*dy-2*dx;
		}
		else {
			writePixel(x0, y0, colorR, colorG, colorB, colorA);
			p=p+2*dy;
		}
		++x0;
	}
};

auto drawLineProtected = [&](int x0, int y0, int x1, int y1, int colorR, int colorG, int colorB, int colorA) {
	int dx, dy, p, x, y;
	dx=x1-x0;
	dy=y1-y0;
	p=2*dy-dx;

	while(x0<x1 && x0 < WIDTH) {
		if(x0 > 0){
			if(p>=0) {
				if(y0 > 0 && y0 < HEIGHT) writePixel(x0, y0, colorR, colorG, colorB, colorA);
				++y0;
				p=p+2*dy-2*dx;
			}
			else {
				if(y0 > 0 && y0 < HEIGHT) writePixel(x0, y0, colorR, colorG, colorB, colorA);
				p=p+2*dy;
			}
		}
		++x0;
	}
};

// Circles
auto drawCircle = [&](int centerX, int centerY, int radius, int colorR, int colorG, int colorB, int colorA) {
	auto drawCirclePoints = [&](int x, int y){
		writePixel(x+ centerX,y+ centerY, colorR, colorG, colorB, colorA);
		writePixel(-x+ centerX,y+ centerY, colorR, colorG, colorB, colorA);
		writePixel(x+ centerX, -y+ centerY, colorR, colorG, colorB, colorA);
		writePixel(-x+ centerX, -y+ centerY,  colorR, colorG, colorB, colorA);
		writePixel(y+ centerX, x+ centerY,  colorR, colorG, colorB, colorA);
		writePixel(y+ centerX, -x+ centerY,  colorR, colorG, colorB, colorA);
		writePixel(-y+ centerX, x+ centerY,  colorR, colorG, colorB, colorA);
		writePixel(-y+ centerX, -x+ centerY,  colorR, colorG, colorB, colorA);
	};



	int pk = 3 - 2*radius;
	int x=0;
	int y = radius;
	drawCirclePoints(x,y);
	while(x < y) {
		if(pk <= 0) {
			pk = pk + (4*x) + 6;
			drawCirclePoints(++x,y);
		} else {
			pk = pk + (4*(x-y)) + 10;
			drawCirclePoints(++x,--y);
		}
	}
};

auto drawCircleProtected = [&](int centerX, int centerY, int radius, int colorR, int colorG, int colorB, int colorA) {
	// TODO: optimize this
	auto drawCirclePoints = [&](int x, int y){
		if(x+centerX > 0 && x+centerX < WIDTH){
			if(y+centerY > 0 && y+centerY < HEIGHT) writePixel(x+centerX, y+centerY, colorR, colorG, colorB, colorA);
			if(-y+centerY > 0 && -y+centerY < HEIGHT) writePixel(x+centerX, -y+centerY, colorR, colorG, colorB, colorA);
		}

		if(-x+centerX > 0 && -x+centerX < WIDTH){
			if(y+centerY > 0 && y+centerY < HEIGHT) writePixel(-x+centerX, y+centerY, colorR, colorG, colorB, colorA);
			if(-y+centerY > 0 && -y+centerY < HEIGHT) writePixel(-x+centerX, -y+centerY, colorR, colorG, colorB, colorA);
		}

		if(y+centerX > 0 && y+centerX < WIDTH){
			if(x+centerY > 0 && x+centerY < HEIGHT) writePixel(y+centerX, x+centerY, colorR, colorG, colorB, colorA);
			if(-x+centerY > 0 && -x+centerY < HEIGHT) writePixel(y+centerX, -x+centerY, colorR, colorG, colorB, colorA);
		}

		if(-y+centerX > 0 && -y+centerX < WIDTH){
			if(x+centerY > 0 && x+centerY < HEIGHT) writePixel(-y+centerX, x+centerY, colorR, colorG, colorB, colorA);
			if(-x+centerY > 0 && -x+centerY < HEIGHT) writePixel(-y+centerX, -x+centerY, colorR, colorG, colorB, colorA);
		}

		/*
		writePixel(x+centerX, y+centerY, colorR, colorG, colorB, colorA); // x+
		writePixel(-x+ centerX, y+centerY, colorR, colorG, colorB, colorA); // x-
		writePixel(x+centerX, -y+centerY, colorR, colorG, colorB, colorA); // x+
		writePixel(-x+ centerX, -y+centerY,  colorR, colorG, colorB, colorA); // x-
		writePixel(y+centerX, x+centerY,  colorR, colorG, colorB, colorA); // y+
		writePixel(y+centerX, -x+centerY,  colorR, colorG, colorB, colorA); // y+
		writePixel(-y+centerX, x+centerY,  colorR, colorG, colorB, colorA); // y-
		writePixel(-y+centerX, -x+centerY,  colorR, colorG, colorB, colorA); // y-
		*/
	};



	int pk = 3 - 2*radius;
	int x=0;
	int y = radius;
	drawCirclePoints(x,y);
	while(x < y) {
		if(pk <= 0) {
			pk = pk + (4*x) + 6;
			drawCirclePoints(++x,y);
		} else {
			pk = pk + (4*(x-y)) + 10;
			drawCirclePoints(++x,--y);
		}
	}
};
#endif // Import draw