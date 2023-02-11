#ifndef __ERROR_HPP__
#define __ERROR_HPP__

#include <sstream>

namespace libdbc {

class exception : public std::exception {
public:
	const char* what() const throw() {
		return "libdbc exception occurred";
	}
};

class validity_error : public exception {
public:
	const char* what() const throw() {
		return "Invalid DBC file";
	}
};

} // libdbc

#endif // __ERROR_HPP__