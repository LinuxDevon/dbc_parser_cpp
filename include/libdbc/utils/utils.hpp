
#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

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

	class String {
	public:

		static std::string trim(const std::string& line);

		template <class Container>
		static void split(const std::string& str, Container& cont, char delim = ' ') {
			std::stringstream ss(str);
			std::string token;

			while (std::getline(ss, token, delim)) {
				cont.push_back(token);
			}
		}


	};

}

#endif // __UTILS_HPP__
