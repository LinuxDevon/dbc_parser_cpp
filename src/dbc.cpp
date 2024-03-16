#include <cstdint>
#include <libdbc/dbc.hpp>
#include <libdbc/exceptions/error.hpp>
#include <libdbc/utils/utils.hpp>

#include <regex>
#include <vector>

namespace libdbc {

const auto floatPattern = "(-?\\d+\\.?(\\d+)?)"; // Can be negative

const auto signalIdentifierPattern = "(SG_)";
const auto namePattern = "(\\w+)";
const auto bitStartPattern = "(\\d+)"; // Cannot be negative
const auto lengthPattern = "(\\d+)"; // Cannot be negative
const auto byteOrderPattern = "([0-1])";
const auto signPattern = "(\\+|\\-)";
const auto scalePattern = "(\\d+\\.?(\\d+)?)"; // Non negative float
const auto offsetPattern = floatPattern;
// NOLINTNEXTLINE -- Disable warning for runtime initialization and can throw. Can't fix until newer c++ version with constexpr
const auto offsetScalePattern = std::string("\\(") + scalePattern + "\\," + offsetPattern + "\\)";
const auto minPattern = floatPattern;
const auto maxPattern = floatPattern;
// NOLINTNEXTLINE -- Disable warning for runtime initialization and can throw. Can't fix until newer c++ version with constexpr
const auto minMaxPattern = std::string("\\[") + minPattern + "\\|" + maxPattern + "\\]";
const auto unitPattern = "\"(.*)\""; // Random string
const auto receiverPattern = "([\\w\\,]+|Vector__XXX)*";
const auto whiteSpace = "\\s";

constexpr unsigned SIGNAL_NAME_GROUP = 2;
constexpr unsigned SIGNAL_START_BIT_GROUP = 3;
constexpr unsigned SIGNAL_SIZE_GROUP = 4;
constexpr unsigned SIGNAL_ENDIAN_GROUP = 5;
constexpr unsigned SIGNAL_SIGNED_GROUP = 6;
constexpr unsigned SIGNAL_FACTOR_GROUP = 7;
constexpr unsigned SIGNAL_OFFSET_GROUP = 9;
constexpr unsigned SIGNAL_MIN_GROUP = 11;
constexpr unsigned SIGNAL_MAX_GROUP = 13;
constexpr unsigned SIGNAL_UNIT_GROUP = 15;
constexpr unsigned SIGNAL_RECIEVER_GROUP = 16;

constexpr unsigned MESSAGE_ID_GROUP = 2;
constexpr unsigned MESSAGE_NAME_GROUP = 3;
constexpr unsigned MESSAGE_SIZE_GROUP = 4;
constexpr unsigned MESSAGE_NODE_GROUP = 5;

enum VALToken { Identifier = 0, CANId, SignalName, Value, Description };

struct VALObject {
	uint32_t can_id;
	std::string signal_name;
	std::vector<Signal::SignalValueDescriptions> vd;
};

DbcParser::DbcParser()
	: version_re("^(VERSION)\\s\"(.*)\"")
	, bit_timing_re("^(BS_:)")
	, name_space_re("^(NS_)\\s\\:")
	, node_re("^(BU_:)\\s((?:[\\w]+?\\s?)*)")
	, message_re("^(BO_)\\s(\\d+)\\s(\\w+)\\:\\s(\\d+)\\s(\\w+|Vector__XXX)")
	, value_re("^(VAL_)\\s(\\d+)\\s(\\w+)((?:\\s(\\d+)\\s\"([^\"]*)\")+)\\s;$")
	,
	// NOTE: No multiplex support yet
	signal_re(std::string("^") + whiteSpace + signalIdentifierPattern + whiteSpace + namePattern + whiteSpace + "\\:" + whiteSpace + bitStartPattern + "\\|"
			  + lengthPattern + "\\@" + byteOrderPattern + signPattern + whiteSpace + offsetScalePattern + whiteSpace + minMaxPattern + whiteSpace + unitPattern
			  + whiteSpace + receiverPattern) {
}

void DbcParser::parse_file(const std::string& file) {
	std::ifstream stream(file.c_str());
	std::string line;
	std::vector<std::string> lines;

	messages.clear();

	parse_dbc_header(stream);

	parse_dbc_nodes(stream);

	while (!stream.eof()) {
		utils::StreamHandler::get_next_non_blank_line(stream, line);
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

Message::ParseSignalsStatus DbcParser::parseMessage(const uint32_t message_id, const std::vector<uint8_t>& data, std::vector<double>& out_values) {
	for (const auto& message : messages) {
		if (message.id() == message_id) {
			return message.parseSignals(data, out_values);
		}
	}
	return Message::ParseSignalsStatus::ErrorUnknownID;
}

void DbcParser::parse_dbc_header(std::istream& file_stream) {
	std::string line;
	std::smatch match;

	utils::StreamHandler::get_line(file_stream, line);

	if (!std::regex_search(line, match, version_re)) {
		throw validity_error();
	}

	version = match.str(2);

	utils::StreamHandler::get_next_non_blank_line(file_stream, line);
	utils::StreamHandler::skip_to_next_blank_line(file_stream, line);
	utils::StreamHandler::get_next_non_blank_line(file_stream, line);

	if (!std::regex_search(line, match, bit_timing_re)) {
		throw validity_error();
	}
}

void DbcParser::parse_dbc_nodes(std::istream& file_stream) {
	std::string line;
	std::smatch match;

	utils::StreamHandler::get_next_non_blank_line(file_stream, line);

	if (!std::regex_search(line, match, node_re)) {
		throw validity_error();
	}

	if (match.length() > 2) {
		std::string node = match.str(2);
		utils::String::split(node, nodes);
	}
}

void DbcParser::parse_dbc_messages(const std::vector<std::string>& lines) {
	std::smatch match;

	std::vector<VALObject> signal_value;

	for (const auto& line : lines) {
		if (std::regex_search(line, match, message_re)) {
			uint32_t message_id = static_cast<uint32_t>(std::stoul(match.str(MESSAGE_ID_GROUP)));
			std::string name = match.str(MESSAGE_NAME_GROUP);
			uint8_t size = static_cast<uint8_t>(std::stoul(match.str(MESSAGE_SIZE_GROUP)));
			std::string node = match.str(MESSAGE_NODE_GROUP);

			Message msg(message_id, name, size, node);

			messages.push_back(msg);
			continue;
		}

		if (std::regex_search(line, match, signal_re)) {
			std::string name = match.str(SIGNAL_NAME_GROUP);
			bool is_multiplexed = false; // No support yet
			uint32_t start_bit = static_cast<uint32_t>(std::stoul(match.str(SIGNAL_START_BIT_GROUP)));
			uint32_t size = static_cast<uint32_t>(std::stoul(match.str(SIGNAL_SIZE_GROUP)));
			bool is_bigendian = (std::stoul(match.str(SIGNAL_ENDIAN_GROUP)) == 0);
			bool is_signed = (match.str(SIGNAL_SIGNED_GROUP) == "-");

			double factor = utils::String::convert_to_double(match.str(SIGNAL_FACTOR_GROUP));
			double offset = utils::String::convert_to_double(match.str(SIGNAL_OFFSET_GROUP));
			double min = utils::String::convert_to_double(match.str(SIGNAL_MIN_GROUP));
			double max = utils::String::convert_to_double(match.str(SIGNAL_MAX_GROUP));

			std::string unit = match.str(SIGNAL_UNIT_GROUP);

			std::vector<std::string> receivers;
			utils::String::split(match.str(SIGNAL_RECIEVER_GROUP), receivers, ',');

			Signal sig(name, is_multiplexed, start_bit, size, is_bigendian, is_signed, factor, offset, min, max, unit, receivers);
			messages.back().appendSignal(sig);
			continue;
		}

		if (std::regex_search(line, match, value_re)) {
			uint32_t message_id = static_cast<uint32_t>(std::stoul(match.str(2)));
			std::string signal_name = match.str(3);

			// Loop over the rest of the descriptions
			std::string rest_of_descriptions = match.str(4);
			std::regex description_re("\\s(\\d+)\\s\"([^\"]*)\"");

			std::sregex_iterator desc_iter(rest_of_descriptions.begin(), rest_of_descriptions.end(), description_re);
			std::sregex_iterator desc_end;

			std::vector<Signal::SignalValueDescriptions> values{};
			while (desc_iter != desc_end) {
				std::smatch desc_match = *desc_iter;
				uint32_t number = static_cast<uint32_t>(std::stoul(desc_match.str(1)));
				std::string text = desc_match.str(2);

				values.push_back(Signal::SignalValueDescriptions{number, text});
				++desc_iter;
			}

			VALObject obj{message_id, signal_name, values};

			signal_value.push_back(obj);
			continue;
		}
	}

	for (const auto& signal : signal_value) {
		for (auto& msg : messages) {
			if (msg.id() == signal.can_id) {
				msg.addValueDescription(signal.signal_name, signal.vd);
				break;
			}
		}
	}
}

}
