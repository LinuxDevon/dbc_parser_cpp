#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include <array>
#include <iostream>
#include <libdbc/signal.hpp>
#include <string>
#include <vector>

namespace libdbc {
struct Message {
	Message() = delete;
	explicit Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node);

	bool BIG_ENDIAN_SYS = false;

	enum class ParseSignalsStatus {
		Success,
		ErrorUnknownID,
		ErrorInvalidConversion,
	};

	/*!
	 * \brief parseSignals
	 * \param data
	 * \param values
	 * \return
	 */
	ParseSignalsStatus parseSignals(const std::vector<uint8_t>& data, std::vector<double>& values) const;

	void appendSignal(const Signal& signal);
	const std::vector<Signal> getSignals() const;
	uint32_t id() const;
	const std::string& name() const {
		return m_name;
	}
	void addValueDescription(const std::string& signal_name, const std::vector<Signal::SignalValueDescriptions>&);

	virtual bool operator==(const Message& rhs) const;

private:
	uint32_t m_id;
	std::string m_name;
	uint8_t m_size;
	std::string m_node;
	std::vector<Signal> m_signals;

	friend std::ostream& operator<<(std::ostream& os, const Message& dt);
};

std::ostream& operator<<(std::ostream& out, const Message& msg);

}

#endif // __MESSAGE_HPP__
