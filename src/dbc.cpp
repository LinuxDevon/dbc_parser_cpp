#include "exceptions/error.hpp"
#include "utils/utils.hpp"
#include "dbc.hpp"

#include <regex>

namespace libdbc {

	DbcParser::DbcParser() : version(""), version_re("^(VERSION)\\s\"(.*)\""),
				  bit_timing_re("^(BS_:)"), name_space_re("^(NS_)\\s\\:") {

	}

	void DbcParser::parse_file(const std::string& file) {
		std::ifstream s(file.c_str());
		std::string line;

		parse_dbc_header(s);

		while(!s.eof()) {
			utils::StreamHandler::get_line(s, line);

		}

	}

	std::string DbcParser::get_version() const {
		return version;
	}

	void DbcParser::parse_dbc_header(std::istream& file_stream) {
		std::string line;
		std::smatch match;
		bool is_blank = true;
		bool not_blank = true;

		utils::StreamHandler::get_line(file_stream, line);

		if(!std::regex_search(line, match, version_re)) {
			throw validity_error();
		}

		version = match.str(2);

		utils::StreamHandler::get_next_non_blank_line( file_stream, line );

		utils::StreamHandler::skip_to_next_blank_line( file_stream, line );

		utils::StreamHandler::get_next_non_blank_line( file_stream, line );

		if(!std::regex_search(line, match, bit_timing_re))
			throw validity_error();

	}

}
