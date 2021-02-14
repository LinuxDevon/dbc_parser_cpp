#include <libdbc/message.hpp>

namespace libdbc {
	Message::Message(uint32_t id, const std::string& name, uint8_t size, const std::string& node) :
		id(id), name(name), size(size), node(node) {}

	bool Message::operator==(const Message& rhs) const {
		return (this->id == rhs.id) && (this->name == rhs.name) &&
			   (this->size == rhs.size) && (this->node == rhs.node);
	}

	std::ostream& operator<< (std::ostream &out, const Message& msg) {
		out << "Message: {id: " << msg.id << ", ";
		out << "name: " << msg.name << ", ";
		out << "size: " << msg.size << ", ";
		out << "node: " << msg.node << "}";
		return out;
	}
}