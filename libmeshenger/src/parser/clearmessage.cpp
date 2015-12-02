#include <parser.h>
#include <string>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iterator>
#include <iostream>

using namespace std;

namespace libmeshenger
{
	uint16_t
	ClearMessage::length() const
	{
		return raw_m.size();
	}

	vector<uint8_t>
	ClearMessage::body() const
	{
		return vector<uint8_t>(raw_m.begin(), raw_m.end());
	}

	ClearMessage::ClearMessage(Packet p)
	: raw_m(p.body())
	{
		if (p.type() != 1)
			throw WrongPacketTypeException();
	}


	/* String-based constructor */
	ClearMessage::ClearMessage(string s)
	{
		uint8_t * str = (uint8_t *) s.c_str();
		vector<uint8_t> raw_body(str, str + s.size() + 1);

		raw_m = raw_body;
	}

	string
	ClearMessage::bodyString() const
	{
		/* Treat the body as a plain, ASCII string
		 * Weird output will result if it isn't */
		string s((const char *) (body().data()));
		return s;
	}

	/* Equality */
	bool
	operator==(const ClearMessage& lhs, const ClearMessage& rhs)
	{
		if (lhs.body().size() != rhs.body().size())
			return false;

		for (int i = 0; i < lhs.body().size(); i++) {
			if (lhs.body()[i] != rhs.body()[i])
				return false;
		}
		return true;
	}
}
