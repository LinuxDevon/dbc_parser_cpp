#include "exceptions/error.hpp"
#include "utils/utils.hpp"
#include "dbc.hpp"

#include <regex>

namespace libdbc {

	Message::Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node) :
		id(id), name(name), size(size), node(node) {}

	bool Message::operator==(const Message& rhs) const {
		return (this->id == rhs.id) && (this->name == rhs.name) &&
			   (this->size == rhs.size) && (this->node == rhs.node);
	}

	std::ostream& operator<< (std::ostream &out, const Message& msg) {
		out << "Message: {id: " << msg.id << ", ";
		out << "name: " << msg.name << ", ";
		out << "size: " << msg.size << ", ";
		out << "node: " << msg.node << "}";
		return out;
	}


	Signal::Signal(std::string name, bool is_multiplexed, uint32_t start_bit, uint32_t size, bool is_bigendian, bool is_signed, double factor, double offset, double min, double max, std::string unit, std::vector<std::string> receivers) :
		name(name), is_multiplexed(is_multiplexed), start_bit(start_bit), size(size), is_bigendian(is_bigendian), is_signed(is_signed), offset(offset), min(min), max(max), unit(unit), receivers(receivers) {}

	bool Signal::operator==(const Signal& rhs) const {
		return (this->name == rhs.name) && (this->is_multiplexed == rhs.is_multiplexed) &&
			   (this->start_bit == rhs.start_bit) && (this->size == rhs.size) &&
			   (this->is_bigendian == rhs.is_bigendian) && (this->is_signed == rhs.is_signed) &&
			   (this->offset == rhs.offset) && (this->min == rhs.min) && (this->max == rhs.max) &&
			   (this->unit == rhs.unit) && (this->receivers == rhs.receivers);
	}


	std::ostream& operator<< (std::ostream &out, const Signal& sig) {
		out << "Signal {name: " << sig.name << ", ";
		out << "Multiplexed: " << (sig.is_multiplexed ? "True" : "False") << ", ";
		out << "Start bit: " << sig.start_bit << ", ";
		out << "Size: " << sig.size << ", ";
		out << "Endianness: " << (sig.is_bigendian ? "Big endian" : "Little endian") << ", ";
		out << "Value Type: " << (sig.is_signed ? "Signed" : "Unsigned") << ", ";
		out << "Min: " << sig.min << ", Max: " << sig.max << ", ";
		out << "Unit: (" << sig.unit << "), ";
		out << "receivers: ";
		for(const auto &r : sig.receivers)
			out << r;
		return out << "}";
	}


	DbcParser::DbcParser() : version(""), nodes(),
				version_re("^(VERSION)\\s\"(.*)\""), bit_timing_re("^(BS_:)"),
				name_space_re("^(NS_)\\s\\:"), node_re("^(BU_:)\\s((?:[\\w]+?\\s?)*)"),
				message_re("^(BO_)\\s(\\d+)\\s(\\w+)\\:\\s(\\d+)\\s(\\w+|Vector__XXX)") {

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
		}

	}


}
