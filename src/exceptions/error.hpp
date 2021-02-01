#include <sstream>

namespace libdbc {

	class exception : public std::exception {
		const char * what() const throw() {
			return "libdbc exception occurred";
		}
	};

	class validity_error : public exception {
		const char * what() const throw() {
			return "Invalid DBC file...";
		}
	};

	class header_error : public validity_error {
		header_error(const char* line, const char* expected, const char * file) :
			line(line), expected(expected), file(file) {}

		const char * what() const throw() {
			std::ostringstream os;
			os << "Issue with the header line ( " << line << " ) in file ( ";
			os << file << " ). Expected to find: " << expected;
			return os.str().c_str();
		}

	private:
		const char * line;
		const char * expected;
		const char * file;
	};

} // libdbc