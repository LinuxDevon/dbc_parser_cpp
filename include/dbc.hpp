
#ifndef __DBC_HPP__
#define __DBC_HPP__

#include "exceptions/error.hpp"
#include "utils/utils.hpp"

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
		DbcParser();

		virtual ~DbcParser() = default;

		virtual void parse_file(const std::string& file) final override;

		std::string get_version() const;

	private:
		std::string version;

		const std::regex version_re;
		const std::regex bit_timing_re;
		const std::regex name_space_re;

		void parse_dbc_header(std::istream& file_stream);

	};


	struct Message {

	};

}

#endif // __DBC_HPP__
