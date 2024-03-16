#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>

namespace libdbc {

class exception : public std::exception {
public:
	const char* what() const throw() override {
		return "libdbc exception occurred";
	}
};

class validity_error : public exception {
public:
	const char* what() const throw() override {
		return "Invalid DBC file";
	}
};

} // libdbc

#endif // ERROR_HPP
