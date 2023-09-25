#include <algorithm>
#include <cmath>
#include <libdbc/message.hpp>

namespace libdbc {
Message::Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node)
	: m_id(id)
	, m_name(name)
	, m_size(size)
	, m_node(node) {
	// Test for endianness
    short int number = 0x1;
    char *numPtr = (char*)&number;
    BIG_ENDIAN_SYS = numPtr[0] == 0;
}

bool Message::operator==(const Message& rhs) const {
	return (m_id == rhs.id()) && (m_name == rhs.m_name) && (m_size == rhs.m_size) && (m_node == rhs.m_node);
}

Message::ParseSignalsStatus Message::parseSignals(const std::vector<uint8_t>& data, std::vector<double>& values) const {
	int size = (int)data.size();

    std::vector<uint8_t> data_little_endian(data);
    std::vector<uint8_t> data_big_endian(data);

    if(BIG_ENDIAN_SYS) {
        std::reverse(data_big_endian.begin(), data_big_endian.end());
    }
    else {
        std::reverse(data_little_endian.begin(), data_little_endian.end());
    }

	const uint32_t len = size * 8;
    uint64_t v = 0;
    std::vector<uint8_t> data_correct;
    for (const auto &signal: m_signals) {
        if (signal.is_bigendian) {
            // Calculation taken from Python cantools library
            uint32_t start_bit = 8 * ((uint32_t)std::floor(signal.start_bit / 8)) + (8 - (signal.start_bit % 8));
            for (uint32_t i = 0; i < signal.size; i += 8) {
                v = (v << 8) | data_big_endian[(start_bit + i) / 8];
            }
        }
        else {
            uint32_t start_bit = signal.start_bit;
            for (uint32_t i = 0; i < signal.size; i += 8) {
                v = (v << 8) | data_little_endian[(start_bit + i) / 8];
            }
        }

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
