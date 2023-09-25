#include <algorithm>
#include <cmath>
#include <libdbc/message.hpp>

namespace libdbc {
Message::Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node)
	: m_id(id)
	, m_name(name)
	, m_size(size)
	, m_node(node) {
}

bool Message::operator==(const Message& rhs) const {
	return (m_id == rhs.id()) && (m_name == rhs.m_name) && (m_size == rhs.m_size) && (m_node == rhs.m_node);
}

Message::ParseSignalsStatus Message::parseSignals(const std::vector<uint8_t>& data, std::vector<double>& values) const {
	int size = data.size();

	uint64_t data_little_endian = 0;
	uint64_t data_big_endian = 0;
	for (int i = 0; i < size; i++) {
		data_little_endian |= ((uint64_t)data[i]) << i * 8;
		data_big_endian = (data_big_endian << 8) | (uint64_t)data[i];
	}

	// TODO: does this also work on a big endian machine?

	const uint32_t len = size * 8;
	uint64_t v = 0;
	for (const auto& signal : m_signals) {
		if (signal.is_bigendian) {
			uint32_t start_bit = 8 * ((uint32_t)std::floor(signal.start_bit / 8)) + (7 - (signal.start_bit % 8)); // Calculation taken from python CAN
			v = data_big_endian << start_bit;
			v = v >> (len - signal.size);
		} else
			v = data_little_endian >> signal.start_bit;

		if (signal.is_signed && signal.size > 1) {
			switch (signal.size) {
			case 8:
				values.push_back((int8_t)v * signal.factor + signal.offset);
				break;
			case 16:
				values.push_back((int16_t)v * signal.factor + signal.offset);
				break;
			case 32:
				values.push_back((int32_t)v * signal.factor + signal.offset);
				break;
			case 64:
				values.push_back((int64_t)v * signal.factor + signal.offset);
				break;
			default: {
				// 2 complement -> decimal
				const int negative = (v & (1 << (signal.size - 1))) != 0;
				int64_t nativeInt;
				if (negative)
					nativeInt = v | ~((1 << signal.size) - 1); // invert all bits above signal.size
				else
					nativeInt = v & ((1 << signal.size) - 1); // masking
				values.push_back(nativeInt * signal.factor + signal.offset);
				break;
			}
			}
		} else {
			// use only the relevant bits
			v = v & ((1 << signal.size) - 1); // masking
			values.push_back(v * signal.factor + signal.offset);
		}
	}
	return ParseSignalsStatus::Success;
}

void Message::appendSignal(const Signal& signal) {
	m_signals.push_back(signal);
}

const std::vector<Signal> Message::getSignals() const {
	return m_signals;
}

uint32_t Message::id() const {
	return m_id;
}

void Message::addValueDescription(const std::string& signal_name, const std::vector<Signal::SignalValueDescriptions>& vd) {
	for (auto& s : m_signals) {
		if (s.name.compare(signal_name) == 0) {
			s.svDescriptions = vd;
			return;
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Message& msg) {
	out << "Message: {id: " << msg.id() << ", ";
	out << "name: " << msg.m_name << ", ";
	out << "size: " << msg.m_size << ", ";
	out << "node: " << msg.m_node << "}";
	return out;
}
}
