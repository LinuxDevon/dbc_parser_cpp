#include <string>
#include <fstream>
#include <iostream>

namespace Utils {

	class SafeString {
	public:
		/**
		 * This is a safe non line ending specific get_ine function. This is to help with files
		 * carried over from different systems. i.e Unix file comes to Windows with LF endings
		 * instead of CRLF.
		 *
		 * @param  stream [description]
		 * @param  line   [description]
		 * @return        [description]
		 */
		static std::istream & get_line( std::istream & stream, std::string & line );

	};

}