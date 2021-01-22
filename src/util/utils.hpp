#include <string>
#include <fstream>
#include <iostream>

namespace Utils {

	class SafeString {
	public:
		static std::istream & getline( std::istream & stream, std::string & line );

	};

}