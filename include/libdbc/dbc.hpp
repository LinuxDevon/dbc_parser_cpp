#ifndef DBC_HPP
#define DBC_HPP

#include <libdbc/exceptions/error.hpp>
#include <libdbc/message.hpp>
#include <libdbc/signal.hpp>
#include <libdbc/utils/utils.hpp>

#include <regex>

namespace Libdbc {

class Parser {
public:
	virtual ~Parser() = default;

	virtual void parse_file(const std::string& file) = 0;

protected:
};

class DbcParser : public Parser {
public:
	DbcParser();

	void parse_file(const std::string& file) override;

	std::string get_version() const;
	std::vector<std::string> get_nodes() const;
	std::vector<Libdbc::Message> get_messages() const;

	Message::ParseSignalsStatus parse_message(uint32_t message_id, const std::vector<uint8_t>& data, std::vector<double>& out_values);

private:
	std::string version;
	std::vector<std::string> nodes;
	std::vector<Libdbc::Message> messages;

	std::regex version_re;
	std::regex bit_timing_re;
	std::regex name_space_re;
	std::regex node_re;
	std::regex message_re;
	std::regex value_re;
	std::regex signal_re;

	void parse_dbc_header(std::istream& file_stream);
	void parse_dbc_nodes(std::istream& file_stream);
	void parse_dbc_messages(const std::vector<std::string>& lines);
};

}

#endif // DBC_HPP
