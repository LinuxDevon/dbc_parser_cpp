#include <string>
#include <fstream>
#include <iostream>

namespace utils {

	class StreamHandler {
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


		static std::istream & get_next_non_blank_line( std::istream & stream, std::string & line );

		static std::istream & skip_to_next_blank_line( std::istream & stream, std::string & line );

	};

}