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

	ClearMessage::ClearMessage(Packet p)
	: raw_m(p.body())
	{
		if (p.type() != 1)
			throw WrongPacketTypeException();
	}


	/* Equality */
	bool
	operator==(const Message& lhs, const Message& rhs)
	{
		if (lhs.id().size() != rhs.id().size())
			return false;

		for (int i = 0; i < lhs.id().size(); i++) {
			if (lhs.id()[i] != rhs.id()[i])
				return false;
		}
		return true;
	}
}
