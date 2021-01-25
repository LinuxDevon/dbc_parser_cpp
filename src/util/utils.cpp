#include "util/utils.hpp"

namespace Utils {

	std::istream & SafeString::get_line( std::istream & stream, std::string & line ) {
		std::string newline;

		std::getline( stream, newline );

		// Windows CRLF (\r\n)
		if ( newline.size() && newline[newline.size()-1] == '\r' ) {
			line = newline.substr( 0, newline.size() - 1 );
		// MacOS LF (\r)
		} else if (newline.size() && newline[newline.size()] == '\r') {
			line = newline.replace(newline.size(), 1, "\n");
		} else {
			line = newline;
		}

		return stream;
	}

}	// Namespace Utils