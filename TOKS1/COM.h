#pragma once

#include <iostream>
#include <string>
#include <windows.h>
#include <WinBase.h>
#include <string>
#include <string_view>	

#define TIMEOUT 1500
typedef unsigned Baud;

class COM {
private:
	HANDLE _handle;
public:
	COM(const std::string_view& port_name, const Baud speed);
	void Disconnect();
	
	void Write(const uint8_t* buf, const size_t buf_size);
	void Read(uint8_t* buf, const size_t buf_size);

	~COM();
};

class ComPortException : public std::exception {
public:
	ComPortException(const std::string& description =
		"Unspecified port error occured")
		:exception(description.c_str()) {}

	const char* what()const override{ return this->what(); }
};