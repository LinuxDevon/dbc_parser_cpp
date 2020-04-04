
#include <string>
#include <fstream>
#include <iostream>

namespace SafeStr {

/**
 * This is a safe non line ending specific getline function. This is to help with files
 * carried over from different systems. i.e Unix file comes to Windows with LF endings
 * instead of CRLF.
 *
 * @param  stream [description]
 * @param  line   [description]
 * @return        [description]
 */
std::istream & getline( std::istream & stream, std::string & line ) {
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

}


namespace dbc {

class parser {

};

}
