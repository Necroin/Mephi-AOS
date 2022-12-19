#pragma once
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>

class Logger {
private:
	std::ostream& _out;
public:
	Logger(std::ostream& out) : _out(out) {}
	template<class... _LogArgs>
	void info(_LogArgs&&... logs) {
		((_out << std::forward<_LogArgs>(logs)), ...);
		_out << std::endl;
	}
	template<class... _LogArgs>
	void error(_LogArgs&&... logs) {
		((_out << std::forward<_LogArgs>(logs)), ...);
		_out << std::endl;
	}
};
#endif
