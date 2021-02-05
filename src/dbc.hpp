#include "exceptions/error.hpp"
#include "util/utils.hpp"

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
		DbcParser() : version(""), version_re("^(VERSION)\\s\"(.*)\""),
					  bit_timing_re("^(BS_:)"), name_space_re("^(_NS)\\s \\:") {

		}

		virtual ~DbcParser() = default;

		virtual void parse_file(const std::string& file) final override {
			std::ifstream s(file.c_str());
			std::string line;

			parse_dbc_header(s);

			while(!s.eof()) {
				utils::StreamHandler::get_line(s, line);

			}

		}

		std::string get_version() const {
			return version;
		}

	private:
		std::string version;

		const std::regex version_re;
		const std::regex bit_timing_re;
		const std::regex name_space_re;

		void parse_dbc_header(std::istream& file_stream) {
			std::string line;
			std::smatch match;
			bool is_blank = true;
			bool not_blank = true;

			std::vector<std::string> symbols = {"CM_",
											   "BA_DEF_",
											   "BA_",
											   "VAL",
											   "CAT_DEF_",
											   "CAT_",
											   "FILTER",
											   "BA_DEF_DEF_",
											   "EV_DATA_",
											   "ENVVAR_DATA_",
											   "SGTYPE_",
											   "SGTYPE_VALTYPE_",
											   "BA_DEF_SGTYPE_",
											   "BA_SGTYPE_",
											   "SIG_TYPE_REF_",
											   "VAL_TABLE_",
											   "SIG_GROUP_",
											   "SIG_VALTYPE_",
											   "SIGTYPE_VALTYPE_",
											   "BO_TX_BU_",
											   "BA_DEF_REL_",
											   "BA_REL_",
											   "BA_DEF_DEF_REL_",
											   "BU_SG_REL_",
											   "BU_EV_REL_",
											   "BU_BO_REL_"};

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

	};

	struct Message {

	};

}
