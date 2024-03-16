#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>

namespace Libdbc {

class Exception : public std::exception {
public:
	const char* what() const throw() override {
		return "libdbc exception occurred";
	}
};

class ValidityError : public Exception {
public:
	const char* what() const throw() override {
		return "Invalid DBC file";
	}
};

} // libdbc

#endif // ERROR_HPP
