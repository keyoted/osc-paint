#pragma once
#include "dll_loader.hpp"
#include "OSC_data.hpp"
#include "stencil.hpp"
#include <vector>
#include <string>

typedef void*(*OSCfunc)(const unsigned char* read, unsigned int width, unsigned int height, void* customData, const OSCmessage* message);

// Read and write will have new size
typedef void*(*resizeFunc)(const unsigned char* read, unsigned int old_width, unsigned int old_height, unsigned int new_width, unsigned int new_height, void* customData);

typedef void*(*func)(const unsigned char* read, unsigned char* &write, unsigned int width, unsigned int height, void* customData);
typedef void*(*delete_Func)(void* customData);
typedef char*(*nameFunc)();


class PlugInFunctions{
public:
	PlugInFunctions(const char* dllPath)
	:_dllPath(dllPath)
	{
		DEBUG("PlugInBrush: Initializer called");
		_isGood = load(dllPath);
	}
	PlugInFunctions(PlugInFunctions& other) = delete;
	PlugInFunctions& operator=(PlugInFunctions& other) = delete;

	void* destroy(void* dataToDestroy){
		return _destroy(dataToDestroy);
	}
	void* start(const unsigned char* read, unsigned int width, unsigned int height, void* customData, const OSCmessage* message){
		return _start(read, width, height, customData, message);
	}
	void* update(const unsigned char* read, unsigned int width, unsigned int height, void* customData, const OSCmessage* message){
		return _update(read, width, height, customData, message);
	}
	void* draw(const unsigned char* read, unsigned char* &write, unsigned int width, unsigned int height, void* customData){
		return _draw(read, write, width, height, customData);
	}

	void* resize(const unsigned char* read, unsigned int old_width, unsigned int old_height, unsigned int new_width, unsigned int new_height, void* customData){
		return _resize(read, old_width, old_height, new_width, new_height, customData);
	}

	void reload(){load(_dllPath.c_str());}
	void registerUser(){++_isBeingUsedBy;}
	void unregister(){--_isBeingUsedBy;}
	bool isGood() {return _isGood;}
	char* getName() {return _name;}
private:
	bool load(const char* dllPath){
		_dl.reload(dllPath);
		if(_dl.getState().first != DLL_OPENED){
			DEBUG("PlugInBrush: Failed to open dll");
			return false;
		}DEBUG("PlugInBrush: Opened dll\nLoading Functions:");

		_start = (OSCfunc)_dl.getSymbol("start");
		if(_dl.getState().first != DLL_OPENED){
			DEBUG("PlugInBrush: Failed to load start");
			return false;
		}DEBUG("PlugInBrush: Loaded start");

		_update = (OSCfunc)_dl.getSymbol("update");
		if(_dl.getState().first != DLL_OPENED){
			DEBUG("PlugInBrush: Failed to load update");
			return false;
		}DEBUG("PlugInBrush: Loaded draw");

		_destroy = (delete_Func)_dl.getSymbol("destroy");
		if(_dl.getState().first != DLL_OPENED){
			DEBUG("PlugInBrush: Failed to load destroy");
			return false;
		}DEBUG("PlugInBrush: Loaded destroy");

		_draw = (func)_dl.getSymbol("draw");
		if(_dl.getState().first != DLL_OPENED){
			DEBUG("PlugInBrush: Failed to load draw");
			return false;
		}DEBUG("PlugInBrush: Loaded destroy");

		_resize = (resizeFunc)_dl.getSymbol("resize");
		if(_dl.getState().first != DLL_OPENED){
			DEBUG("PlugInBrush: Failed to load resize");
			return false;
		}DEBUG("PlugInBrush: Loaded destroy");

		nameFunc _namef = (nameFunc)_dl.getSymbol("name");
		if(_dl.getState().first != DLL_OPENED){
			DEBUG("PlugInBrush: Failed to load name");
			return false;
		}DEBUG("PlugInBrush: Loaded name");
		_name = _namef();
		return true;
	}

	unsigned int _isBeingUsedBy = 0;
	delete_Func _destroy;
	OSCfunc _start, _update;
	func _draw;
	resizeFunc _resize;
	dllLoader _dl;
	char* _name;
	const std::string _dllPath;
	bool _isGood;
};








class PlugInBrush
{
public:
	PlugInBrush(PlugInFunctions* functions,unsigned int width, unsigned int height, const OSCmessage* message)
	:_funcs(functions)
	,_width(width)
	,_height (height)
	,_colorData(width, height)
	,_startingMessage(*message)
	{
		_funcs->registerUser();
		_packageData = _funcs->start(nullptr, _width, _height, _packageData, message);
	}
	void reset(){
		_packageData = _funcs->destroy(_packageData);
		OSCmessage temp = _startingMessage;
		_packageData = _funcs->start(_colorData.pixels, _width, _height, _packageData, &temp);
	}
	void start(const unsigned char* read, const OSCmessage* message){
		_packageData = _funcs->start(read, _width, _height, _packageData, message);
	}
	void update(const unsigned char* read, const OSCmessage* message){
		_packageData = _funcs->update(read, _width, _height, _packageData, message);

	}
	void draw(const unsigned char* read){
		_colorData.clear();
		draw(read, _colorData.pixels);
	}
	void draw(const unsigned char* read, unsigned char* &write){
		_packageData = _funcs->draw(read, write, _width, _height, _packageData);
		if(brushes.size() > 0){
			stencil tempStencil(_width, _height);
			for(auto brush:brushes)
				brush->draw(_colorData.pixels, tempStencil.pixels);
			_colorData += tempStencil;
		}
	}

	// Read must be in new size
	void resize(const unsigned char* read, unsigned int new_width, unsigned int new_height){
		_colorData.newSize(new_width, new_height);
		_packageData = _funcs->resize(read, _width, _height, new_width, new_height, _packageData);

		for(auto brush:brushes){
			brush->resize(_colorData.pixels, new_width, new_height);
		}

		_height = new_height;
		_width = new_width;
	}

	char* getName(){return _funcs->getName();}

	~PlugInBrush(){
		DEBUG("PlugInBrush: Destructor called");
		_funcs->unregister();
		_funcs->destroy(_packageData);
		_packageData = nullptr;
	}

	stencil _colorData;
private:
	PlugInFunctions* _funcs;
	const OSCmessage _startingMessage;
	void* _packageData;
	unsigned int _height, _width;
	std::vector<PlugInBrush*> brushes;
};
