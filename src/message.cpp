#include <algorithm>
#include <libdbc/message.hpp>

#include <bitstream.h>

namespace libdbc {
	Message::Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node) :
        m_id(id), m_name(name), m_size(size), m_node(node) {}

	bool Message::operator==(const Message& rhs) const {
        return (m_id == rhs.id()) && (m_name == rhs.m_name) &&
               (m_size == rhs.m_size) && (m_node == rhs.m_node);
    }

    bool Message::parseSignals(const uint8_t* data, int size, std::vector<double>& values) const {
        // With the current approach it is not needed to prepare the message by sorting the signals
//        if (!m_prepared)
//            return false;

        if (size > 8)
            return false; // not supported yet

        // Currently only little endian will be supported, because
        // The code below was not tested with bigendian!
        // All signals must be little endian
        for (const auto& signal: m_signals) {
            if (signal.is_bigendian)
                return false;
        }

        uint64_t data_little_endian = 0;
        uint64_t data_big_endian = 0;
        for (int i=0; i < size; i++) {
            data_little_endian |= ((uint64_t)data[i]) << i * 8;
            //data_big_endian |= (uint64_t)data[i] << (size - 1 - i);
        }

        // TODO: does this also work on a big endian machine?

        const uint32_t len = size * 8;
        uint64_t v = 0;
        for (const auto& signal: m_signals) {
            if (signal.is_bigendian) {
                // Not tested!
                //  const uint32_t shiftLeft = signal.start_bit;
                //  v = data_big_endian << shiftLeft;
                //  v = v >> (shiftLeft + signal.start_bit);
            } else {
                const uint32_t shiftLeft = (len - (signal.size + signal.start_bit));
                v = data_little_endian << shiftLeft;
                v = v >> (shiftLeft + signal.start_bit);
            }
            values.push_back(v * signal.factor + signal.offset);
        }
        return true;
    }

    bool Message::parseSignals(const std::array<uint8_t,8>& data, std::vector<double>& values) const {
        return parseSignals(data.data(), data.size(), values);
    }

    bool Message::parseSignals(const std::vector<uint8_t> &data, std::vector<double>& values) const {
        return parseSignals(data.data(), data.size(), values);
    }

    void Message::appendSignal(const Signal& signal) {
        m_prepared = false;
        m_signals.push_back(signal);
    }

    const std::vector<Signal> Message::signals() const {
       return m_signals;
    }

    uint32_t Message::id() const {
        return m_id;
    }

    void Message::prepareMessage() {
//        m_prepared = false;
//        // sort signals so that the signals are ordered by the startbit
        std::sort(m_signals.begin(), m_signals.end());

//        uint32_t curr_bit = 0;
//        for (std::vector<Signal>::size_type i=0; i < m_signals.size(); i++) {
//            const auto& signal = m_signals.at(i);
//            if (signal.is_multiplexed)
//                break; // Not supported yet

//            if (curr_bit < signal.start_bit) {
//                // padding needed
//                bitstruct.push_back(BitStruct(signal.start_bit - curr_bit));
//            }
//            bitstruct.push_back(BitStruct(i, signal.size));
//            curr_bit = signal.start_bit + signal.size;
//        }
//        // Check if correct
//        if (curr_bit > m_size * 8)
//            return;

        m_prepared = true;
    }

    std::ostream& operator<< (std::ostream &out, const Message& msg) {
        out << "Message: {id: " << msg.id() << ", ";
        out << "name: " << msg.m_name << ", ";
        out << "size: " << msg.m_size << ", ";
        out << "node: " << msg.m_node << "}";
		return out;
	}
}
