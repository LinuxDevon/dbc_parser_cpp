
namespace libdbc {

	class exception : public std::exception {
		const char * what() const throw() {
			return "libdbc exception occurred";
		}
	};

	class validity_error : public exception {
		const char * what() const throw() {
			return "invalid file issue";
		}
	};

} // libdbc