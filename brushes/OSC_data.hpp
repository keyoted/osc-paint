#pragma once
#include <_types.h>
#include <string>
#include <vector>

enum OSCvalueType{
	None,
	Float,
	Int,
	String,
	Blob,
	Char,
	Color
};

struct OSCvalue{
	OSCvalue(){}
	
	OSCvalue(const OSCvalue& other)
	{
		this->operator=(other);
	}
	
	OSCvalue(OSCvalueType ntype)
	{
		type = ntype;
	}
	
	OSCvalue& operator= (const OSCvalue& other)
	{
		if (this != &other) // protect against invalid self-assignment
		{
			destroy();
			type = other.type;
			if(!other._alocated) return *this;
			switch(type){
				case Color:
				case Int:
					copy<int32_t>(other);
					break;
				case Float:
					copy<int32_t>(other);
					break;
				case String:
					copy<std::string>(other);
					break;
				case Char:
					copy<char>(other);
					break;
				case Blob:
					copy<std::vector<unsigned char>>(other);
					break;
				case None:
					break;
			}
			_alocated = true;
		}
			// by convention, always return *this
		return *this;
	}
	
	template <typename T>
	bool set(T* arg){
		if(_alocated) return false;
		_data = (void*)arg;
		_alocated = true;
		return true;
	}
	
	template <typename T>
	T* get() const{
		if(!_alocated) return nullptr;
		return (T*) _data;
	}
	
	~OSCvalue(){
		destroy();
	}
	
	OSCvalueType type = OSCvalueType::None;
private:
	
	template <typename T>
	void copy(const OSCvalue& other){
		_data = (void*) new T;
		*(T*)_data = *(other.get<T>());
		_alocated = true;
	}
	
	void destroy(){
		if(!_alocated) return;
		switch(type){
			case None: break;
			case Color:
			case Int:
				delete (std::int32_t*) _data;
				break;
			case Float:
				delete (float*) _data;
				break;
			case String:
				delete (std::string*) _data;
				break;
			case Blob:
				delete (std::vector<unsigned char>*) _data;
				break;
			case Char:
				delete (char*) _data;
				break;
		}
		_alocated = false;
	}
	
	void* _data;
	bool _alocated = false;
};

struct OSCmessage{
	std::vector<std::string> address;
	std::vector<OSCvalue> values;
};
