
#ifndef __DBC_HPP__
#define __DBC_HPP__

#include "exceptions/error.hpp"
#include "utils/utils.hpp"

#include <regex>
#include <iostream>

namespace libdbc {

	struct Message {
		uint32_t id;
		std::string name;
		uint8_t size;
		std::string node;

		Message() = delete;
		explicit Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node);

		virtual bool operator==(const Message& rhs) const;
	};

	std::ostream& operator<< (std::ostream &out, const Message& msg);


	class Parser {
	public:
		virtual ~Parser() = default;

		virtual void parse_file(const std::string& file) = 0;

	protected:


	};

	class DbcParser : public Parser {
	public:
		DbcParser();

		virtual ~DbcParser() = default;

		virtual void parse_file(const std::string& file) final override;

		std::string get_version() const;
		std::vector<std::string> get_nodes() const;
		std::vector<libdbc::Message> get_messages() const;

	private:
		std::string version;
		std::vector<std::string> nodes;
		std::vector<libdbc::Message> messages;

		const std::regex version_re;
		const std::regex bit_timing_re;
		const std::regex name_space_re;
		const std::regex node_re;
		const std::regex message_re;

		void parse_dbc_header(std::istream& file_stream);
		void parse_dbc_nodes(std::istream& file_stream);
		void parse_dbc_messages(const std::vector<std::string>& lines);

	};

}

#endif // __DBC_HPP__
