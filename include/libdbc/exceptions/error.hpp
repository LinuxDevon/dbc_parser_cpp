#ifndef ERROR_HPP
#define ERROR_HPP

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

#endif // ERROR_HPP
