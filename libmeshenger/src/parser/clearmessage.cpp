#include <parser.h>
#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>

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


	/* Equality */
	bool
	operator==(const ClearMessage& lhs, const ClearMessage& rhs)
	{
		if (lhs.id().size() != rhs.id().size())
			return false;

		return equal(lhs.id().begin(), lhs.id().end(), rhs.id().begin());
	}
}
