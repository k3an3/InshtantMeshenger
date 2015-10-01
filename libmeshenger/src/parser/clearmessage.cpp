#include <parser.h>
#include <string>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iterator>

using namespace std;

namespace libmeshenger
{
	uint16_t
	ClearMessage::length() const
	{
		return raw_m.size() - 16;
	}

	vector<uint8_t>
	ClearMessage::body() const
	{
		return vector<uint8_t>(raw_m.begin()+16, raw_m.end());
	}

	vector<uint8_t>
	ClearMessage::id() const
	{
		return vector<uint8_t>(raw_m.begin(), raw_m.end());
	}

	ClearMessage::ClearMessage(Packet p)
	: raw_m(p.body())
	{
		if (p.type() != 1)
			throw WrongPacketTypeException();
	}

	string
	ClearMessage::bodyString() const
	{
		string s((const char *) (body().data()));
		return s;
	}

	string
	ClearMessage::idString() const
	{
		char buffer[3];
		string s;
		for(uint8_t i = 0; i < 16; i++) {
			snprintf(buffer, 3, "%02X", id()[i]);
			s += buffer;
		}

		return s;
	}

	/* Equality */
	bool
	operator==(const ClearMessage& lhs, const ClearMessage& rhs)
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
