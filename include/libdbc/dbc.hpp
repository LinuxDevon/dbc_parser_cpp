
#ifndef __DBC_HPP__
#define __DBC_HPP__

#include <libdbc/exceptions/error.hpp>
#include <libdbc/utils/utils.hpp>
#include <libdbc/signal.hpp>
#include <libdbc/message.hpp>

#include <regex>

namespace libdbc {

	class Parser {
	public:
		virtual ~Parser() = default;

		virtual void parse_file(const std::string& file) = 0;

	protected:


	};

	class DbcParser : public Parser {
	public:
        DbcParser(bool sortSignals = false);

		virtual ~DbcParser() = default;

		virtual void parse_file(const std::string& file) final override;

		std::string get_version() const;
		std::vector<std::string> get_nodes() const;
		std::vector<libdbc::Message> get_messages() const;

        bool parseMessage(const uint32_t id, const std::vector<uint8_t>& data, std::vector<double>& out_values);

	private:
		std::string version;
		std::vector<std::string> nodes;
		std::vector<libdbc::Message> messages;

		const std::regex version_re;
		const std::regex bit_timing_re;
		const std::regex name_space_re;
		const std::regex node_re;
		const std::regex message_re;
		const std::regex signal_re;

        bool sortSignals{false};

		void parse_dbc_header(std::istream& file_stream);
		void parse_dbc_nodes(std::istream& file_stream);
		void parse_dbc_messages(const std::vector<std::string>& lines);

	};

}

#endif // __DBC_HPP__
