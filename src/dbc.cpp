#include <libdbc/exceptions/error.hpp>
#include <libdbc/utils/utils.hpp>
#include <libdbc/dbc.hpp>

#include <regex>

namespace libdbc {

	DbcParser::DbcParser() : version(""), nodes(),
				version_re("^(VERSION)\\s\"(.*)\""), bit_timing_re("^(BS_:)"),
				name_space_re("^(NS_)\\s\\:"), node_re("^(BU_:)\\s((?:[\\w]+?\\s?)*)"),
				message_re("^(BO_)\\s(\\d+)\\s(\\w+)\\:\\s(\\d+)\\s(\\w+|Vector__XXX)"),
				// NOTE: No multiplex support yet
				signal_re("\\s(SG_)\\s(\\w+)\\s\\:\\s(\\d+)\\|(\\d+)\\@(\\d+)(\\+|\\-)\\s\\((-?\\d+\\.?(\\d+)?)\\,(-?\\d+\\.?(\\d+)?)\\)\\s\\[(-?\\d+\\.?(\\d+)?)\\|(-?\\d+\\.?(\\d+)?)\\]\\s\"(\\w*)\"\\s([\\w\\,]+|Vector__XXX)*") {

	}

	void DbcParser::parse_file(const std::string& file) {
		std::ifstream s(file.c_str());
		std::string line;
		std::vector<std::string> lines;

		parse_dbc_header(s);

		parse_dbc_nodes(s);

		while(!s.eof()) {
			utils::StreamHandler::get_next_non_blank_line( s, line );
			lines.push_back(line);
		}

		parse_dbc_messages(lines);

	}

	std::string DbcParser::get_version() const {
		return version;
	}

	std::vector<std::string> DbcParser::get_nodes() const {
		return nodes;
	}

	std::vector<libdbc::Message> DbcParser::get_messages() const {
		return messages;
	}


	void DbcParser::parse_dbc_header(std::istream& file_stream) {
		std::string line;
		std::smatch match;

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

	void DbcParser::parse_dbc_nodes(std::istream& file_stream) {
		std::string line;
		std::smatch match;

		utils::StreamHandler::get_next_non_blank_line( file_stream, line );

		if(!std::regex_search(line, match, node_re))
			throw validity_error();

		if(match.length() > 2) {
			std::string n = match.str(2);
			utils::String::split(n, nodes);
		}

	}

	void DbcParser::parse_dbc_messages(const std::vector<std::string>& lines) {
		std::smatch match;

		for(const auto &line : lines) {
			if(std::regex_search(line, match, message_re)) {
				uint32_t id = std::stoul(match.str(2));
				std::string name = match.str(3);
				uint8_t size = std::stoul(match.str(4));
				std::string node = match.str(5);

				Message msg(id, name, size, node);

				messages.push_back(msg);
			}

			if(std::regex_search(line, match, signal_re)) {
				std::string name = match.str(2);
				bool is_multiplexed = false; // No support yet
				uint32_t start_bit = std::stoul(match.str(3));
				uint32_t size = std::stoul(match.str(4));
				bool is_bigendian = (std::stoul(match.str(5)) == 1);
				bool is_signed = (match.str(6) == "-");
				// Alternate groups because a group is for the decimal portion
				double factor = std::stod(match.str(7));
				double offset = std::stod(match.str(9));
				double min = std::stod(match.str(11));
				double max = std::stod(match.str(13));
				std::string unit = match.str(15);

				std::vector<std::string> receivers;
				utils::String::split(match.str(16), receivers, ',');

				Signal sig(name, is_multiplexed, start_bit, size, is_bigendian, is_signed, factor, offset, min, max, unit, receivers);
				messages.back().signals.push_back(sig);
			}
		}

	}


}
