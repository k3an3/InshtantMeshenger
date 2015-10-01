#include <parser.h>
#include <string>
#include <cstdint>

namespace libmeshenger
{
	int16_t
	ClearMessage::length()
	{
		return raw_m.size() - 16;
	}

	vector<uint8_t>
	ClearMessage::body()
	{
		return vector<uint8_t>(raw_m.begin()+16, raw_m.end());
	}

	vector<uint8_t>
	ClearMessage::id()
	{
		return vector<uint8_t>(raw_m.begin(), raw_m.end());
	}

	Message::Message(Packet p)
	: raw_m(p.body())
	{
		if (p.type() != 1)
			throw WrongPacketTypeException();
	}
}
