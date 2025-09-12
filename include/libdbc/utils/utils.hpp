
#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#if __cplusplus >= 201703L
#	include <string_view>
#endif // __cplusplus >= 201703L

#if __cpp_concepts >= 201907
#include <concepts>
template<typename Str>
concept procsys_stringable = requires(Str s) { { s.data() + s.size() } -> std::convertible_to<const char*>; };
#endif // __cpp_concepts >= 201907

namespace Utils {

class StreamHandler {
public:
	/**
	 * This is a safe non line ending specific get_ine function. This is to help with files
	 * carried over from different systems. i.e Unix file comes to Windows with LF endings
	 * instead of CRLF.
	 *
	 * @param  stream [description]
	 * @param  line   [description]
	 * @return        [description]
	 */
	static std::istream& get_line(std::istream& stream, std::string& line);

	static std::istream& get_next_non_blank_line(std::istream& stream, std::string& line);

	static std::istream& skip_to_next_blank_line(std::istream& stream, std::string& line);
};

/**
 * @brief Trims the specified characters from the beginning and end of the given string.
 * 
 * @tparam T The typeparam; must be stringable.
 * @param value The string to trim.
 * @return A new string with the specified characters trimmed.
 */
#if __cplusplus >= 201703
#	if __cpp_concepts >= 201907
	template<procsys_stringable T>
#	else
	template<typename T>
#	endif // __cpp_concepts >= 201907
constexpr auto  trim(const T& value) {
#else
template<typename T>
std::string		trim(const T& value) {
#endif // #if __cplusplus >= 201703
	T trimmedValue = value;
	trimmedValue.erase(trimmedValue.begin(), std::find_if(trimmedValue.begin(), trimmedValue.end(), [](int ch) { return !std::isspace(ch); }));
	trimmedValue.erase(std::find_if(trimmedValue.rbegin(), trimmedValue.rend(), [](int ch) { return !std::isspace(ch); }).base(), trimmedValue.end());
	return trimmedValue;
}

/**
 * @brief Trims the specified characters from the beginning and end of the given string.
 * 
 * @tparam T The typeparam; must be stringable.
 * @param value The string to trim.
 * @param trimChars The characters to trim from the string.
 * @return A new string with the specified characters trimmed.
 */
#if __cplusplus >= 201703
#	if __cpp_concepts >= 201907
	template<procsys_stringable T>
#	else
	template<typename T>
#	endif // __cpp_concepts >= 201907
constexpr auto  trim(const T& value, std::initializer_list<char>& trimChars) {
#else
template<typename T>
std::string  	trim(const T& value, const std::initializer_list<char>& trimChars) {
#endif // __cplusplus >= 201703
	T trimmedValue = value;
	trimmedValue.erase(trimmedValue.begin(), std::find_if(trimmedValue.begin(), trimmedValue.end(), [&trimChars](int ch) { return std::find(trimChars.begin(), trimChars.end(), static_cast<char>(ch)) == trimChars.end(); }));
	trimmedValue.erase(std::find_if(trimmedValue.rbegin(), trimmedValue.rend(), [&trimChars](int ch) { return std::find(trimChars.begin(), trimChars.end(), static_cast<char>(ch)) == trimChars.end(); }).base(), trimmedValue.end());
	return trimmedValue;
}

/**
 * @brief Checks if the given string is empty or contains only whitespace characters.
 * 
 * @tparam T The typeparam; must be stringable.
 * 
 * @param str The string to check against.
 * 
 * @return true If the string is empty or consists only of whitespace.
 * @return false Otherwise.
 */
template<typename T>
constexpr bool  isWhitespaceOrEmpty(const T& str) {
	return std::all_of(str.begin(), str.end(), [](char c) { return std::isspace(c); });
}

/**
 * @brief Swaps the endianness of a multi-byte value.
 * 
 * @tparam T The type of the value.
 * @param value The value to swap.
 * @return T The value with swapped endianness.
 */
template<typename T>
T                swapEndianness(T value) {
	T swappedBytes;

	char* valuePtr = reinterpret_cast<char*>(&value);
	char* swappedPtr = reinterpret_cast<char*>(&swappedBytes);

	auto sizeInBytes = sizeof(T);
	for (size_t i = 0; i < sizeInBytes; i++) {
		swappedPtr[sizeInBytes - 1 - i] = valuePtr[i];
	}

	return swappedBytes;
}

class String {
public:
	static std::string trim(const std::string& line) { return Utils::trim(line); }

	template<class Container>
	static void split(const std::string& str, Container& cont, char delim = ' ') {
		std::stringstream stream(str);
		std::string token;

		while (std::getline(stream, token, delim)) {
			cont.push_back(token);
		}
	}

	static double convert_to_double(const std::string& value, double default_value = 0);
};

}

#endif // UTILS_HPP
