#include "util/utils.hpp"

#include <regex>

namespace utils {

	std::istream & StreamHandler::get_line( std::istream & stream, std::string & line ) {
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


	std::istream & StreamHandler::get_next_non_blank_line( std::istream & stream, std::string & line ) {
		bool is_blank = true;

		const std::regex whitespace_re("\\s*(.*)");
		std::smatch match;

		while(is_blank) {
			utils::StreamHandler::get_line(stream, line);

			std::regex_search(line, match, whitespace_re);

			if((!line.empty() && !match.empty()) || (stream.eof())){
				if((match.length(1) > 0) || (stream.eof())){
					is_blank = false;
				}
			}
		}

		return stream;
	}

	std::istream & StreamHandler::skip_to_next_blank_line( std::istream & stream, std::string & line ) {
		bool line_is_empty = false;

		const std::regex whitespace_re("\\s*(.*)");
		std::smatch match;

		while(!line_is_empty) {
			utils::StreamHandler::get_line(stream, line);

			std::regex_search(line, match, whitespace_re);

			if((match.length(1) == 0) || (stream.eof())){
				line_is_empty = true;
			}
		}

		return stream;
	}


}	// Namespace Utils