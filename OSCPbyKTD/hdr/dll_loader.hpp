#pragma once

#define DO_DEBBUGING 0


#if DO_DEBBUGING
	#include <iostream>
	#define DEBUG(X) std::cout << X; std::cin.get()
#else
	#define DEBUG(X);
#endif




#include "dlfcn.h"
#include <string>
#include <utility>



enum DLLSTATE{
	DLL_CLOSED,
	DLL_OPENED,
	DLL_ERROR,
};

class dllLoader{
	
public:
	dllLoader(){}
	
	dllLoader(const char* dllPath){
		path = dllPath;
		if(!load()){
			DEBUG("dllLoader: ERROR: returning");
			return;
		}
	}
	
	dllLoader (const dllLoader& other) {
		DEBUG("dllLoader: Copy constructor called");
		*this = other;
	}
	
	dllLoader& operator=(const dllLoader& other) {
		DEBUG("dllLoader: Assignment operator called");
		if(this != &other){
			if (other.state == DLL_OPENED) dllLoader(other.path.c_str());
			else{
				err = other.err;
				path = other.path;
				state = other.state;
			}
		}
		return *this;
	}
	
	void reload(const char* dllPath){
		path = dllPath;
		unload();
		if(!load()){
			DEBUG("dllLoader: ERROR: returning");
			return;
		}
	}
	
	void reload(){
		unload();
		if(!load()){
			DEBUG("dllLoader: ERROR: returning");
			return;
		}
	}
	
	void* getSymbol(const char* symbol){
		// load the symbol
		DEBUG("dllLoader: Loading symbol " << symbol);
		
		// reset errors
		dlerror();
		void* symbolObject = dlsym(handle, symbol);
		const char *dlsym_error = dlerror();
		if (dlsym_error) {
			DEBUG("dllLoader: Cannot load symbol '" << symbol << "': " << dlsym_error);
			err = dlsym_error;
			state = DLL_ERROR;
			return nullptr;
		}
		
		return symbolObject;
	}
	
	std::pair<DLLSTATE, std::string> getState(){
		return std::make_pair(state, err);
	}
	
	std::string getPath() const{
		return path;
	}
	
	~dllLoader(){
		unload();
	}
	
private:
	
	bool load(){
			// open the library
		DEBUG("dllLoader: Opening Lib");
		handle = dlopen(path.c_str(), RTLD_LAZY);
		if (!handle) {
			err = dlerror();
			state = DLL_ERROR;
			DEBUG("dllLoader: Cannot open library: " << err);
			return false;
		}
		
		state = DLL_OPENED;
		return true;
	}
	
	void unload(){
		DEBUG("dllLoader: Unloading");
			// Close dll if needed
		if(state == DLL_OPENED){
			DEBUG("dllLoader: Closing library");
			dlclose(handle);
			state = DLL_CLOSED;
		}
	}
	
	
	void* handle;
	DLLSTATE state = DLL_CLOSED;
	std::string err = "";
	std::string path = "";
};
