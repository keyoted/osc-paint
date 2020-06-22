#pragma once
struct stencil{
	stencil(unsigned width, unsigned height)
	{
		init(width,height);
	}
	
	void init(unsigned width, unsigned height)
	{
		_width = width;
		_height = height;
		pixels = new unsigned char[width * height * 4]();
	}
	
	void clear(){
		for(unsigned int g = 0; g < _width*_height*4; g+=4){
			pixels[g] = 0;
			pixels[g+1] = 0;
			pixels[g+2] = 0;
			pixels[g+3] = 0;
		}
	}
	
	stencil& operator +=(const stencil& other){
		/*
		 The end color will be calculated assuming other is on top of this
		 end = (top * top_alpha) + (btm * (1.0-top_alpha))
		 */
		
		if(other._width != _width || other._height != _height) return *this;
		
		unsigned char* &btm = pixels;
		unsigned char* const &top = other.pixels;
		float btm_alpha, top_alpha, oneMinusTA;
		for(unsigned int g = 0; g < _width*_height*4; g+=4){
			btm_alpha = btm[g+3] / 255.0f;
			top_alpha = top[g+3] / 255.0f;
			oneMinusTA = 1.0f - top_alpha;
			
			btm[g] = (top[g] * top_alpha) + (btm[g] * oneMinusTA);
			btm[g+1] = (top[g+1] * top_alpha) + (btm[g+1] * oneMinusTA);
			btm[g+2] = (top[g+2] * top_alpha) + (btm[g+2] * oneMinusTA);
			btm[g+3] = btm[g+3] + (0xff-btm[g+3])*top_alpha;
		}
		
		return *this;
	}
	
	void newSize(unsigned width, unsigned height)
	{
		delete[] pixels;
		pixels = new unsigned char[width * height * 4];
	}
	
	~stencil(){
		delete[] pixels;
	}
	
	unsigned char& operator() (unsigned x, unsigned y, unsigned RGBA) const{
		return pixels[ (y * _width * 4) + (x * 4) + RGBA ];
	}
	
	unsigned char* pixels;
private:
	unsigned _width, _height, _n=0;
};








class stencilWraper{
	stencilWraper(unsigned char* p) {
		init(p);
	}
	
	void init(unsigned char* p) {
		pixels = p;
	}
	
	stencilWraper& operator +=(const stencilWraper& other){
		/*
		 The end color will be calculated assuming other is on top of this
		 end = (top * top_alpha) + (btm * (1.0-top_alpha))
		 */
		
		if(other._width != _width || other._height != _height) return *this;
		
		unsigned char* &btm = pixels;
		unsigned char* const &top = other.pixels;
		float btm_alpha, top_alpha, oneMinusTA;
		for(unsigned int g = 0; g < _width*_height*4; g+=4){
			btm_alpha = btm[g+3] / 255.0f;
			top_alpha = top[g+3] / 255.0f;
			oneMinusTA = 1.0f - top_alpha;
			
			btm[g] = (top[g] * top_alpha) + (btm[g] * oneMinusTA);
			btm[g+1] = (top[g+1] * top_alpha) + (btm[g+1] * oneMinusTA);
			btm[g+2] = (top[g+2] * top_alpha) + (btm[g+2] * oneMinusTA);
			btm[g+3] = btm[g+3] + (0xff-btm[g+3])*top_alpha;
		}
		
		return *this;
	}
	
	void newSize(unsigned width, unsigned height) {
		delete[] pixels;
		pixels = new unsigned char[width * height * 4];
	}
	
	~stencilWraper(){}
	
	unsigned char& operator() (unsigned x, unsigned y, unsigned RGBA) const{
		return pixels[ (y*_width) + (x*4) + RGBA ];
	}
	
	unsigned char* pixels;
private:
	unsigned _width, _height;
};
