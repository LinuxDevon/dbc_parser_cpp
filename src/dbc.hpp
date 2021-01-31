#include "exceptions/error.hpp"
#include "util/utils.hpp"

namespace libdbc {

	class Parser {
	public:
		virtual ~Parser() = default;

		virtual void parse_file(const std::string& file) = 0;

	protected:

		void verify_dbc_format(std::istream& file_stream) const {
			std::string line;

			Utils::SafeString::get_line(file_stream, line);

			if(line.find("VERSION") == std::string::npos) {
				throw validity_error();
			}
		}
	};

	class DbcParser : public Parser {
	public:
		virtual ~DbcParser() = default;

		virtual void parse_file(const std::string& file) final override {
			std::ifstream s(file.c_str());
			std::string line;

			verify_dbc_format(s);

			while(!s.eof()) {
				Utils::SafeString::get_line(s, line);

			}

		}

		std::string get_version() const {
			return "";
		}

	private:


	};

	struct Message {

	};

}
