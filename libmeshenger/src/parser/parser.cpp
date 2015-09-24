#include <iostream>
#include <cstdint>

#include <parser.h>

using namespace std;

namespace libmeshenger
{
	static bool
	validateMessage(vector<uint8_t> msg);

	/* Statics */
	static bool
	validateMessage(vector<uint8_t> body)
	{
		if (body.size() < 144)
		{
			return false;
		}
		return true;
	}

	/* Global functions */
	bool
	ValidatePacket(vector<uint8_t> msg)
	{
		/* Validate minimum size */
		if (msg.size() < 8)
			return false;

		/* Validate magic */
		if (msg[0] != 'I' || msg[1] != 'M')
			return false;

		/* Validate version (always 1 for now) */
		if (msg[2] != 1)
			return false;

		/* Get body length */
		uint16_t bodyLength = msg[7];
		bodyLength |= (msg[6] << 8);

		/* Validate body length */
		if (msg.size() != bodyLength + 8)
			return false;

		/* Return true if valid empty packet */
		if (msg.size() == 8 && msg[5] == 0x00)
			return true;

		/* Create body vector */
		vector<uint8_t> body(msg.begin() + 8, msg.end());

		/* Validate message body based on body type */
		switch (msg[5]) {
			case 0x01:
				return validateMessage(body);
				break;
			default:
				return false;
		}
	}

	/* Class methods */
	Packet::Packet(vector<uint8_t> data)
		: raw_m(data), body_m(data.begin()+8, data.end())
	{
		if (ValidatePacket(data) == false)
			//throw InvalidPacketException("Unable to validate!");
			throw 5;

		type_m = data[5];
	}

	uint16_t
	Packet::length()
	{
		return body_m.size();
	}

	vector<uint8_t>
	Packet::body()
	{
		/* Return (by value) a copy of the body */
		return vector<uint8_t>(body_m);
	}
}
