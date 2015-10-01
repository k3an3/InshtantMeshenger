#include <iostream>
#include <cstdint>

#include <parser.h>

using namespace std;

namespace libmeshenger
{
	static bool
	validateClearMessage(vector<uint8_t> msg);

	/* Statics */
	static bool
	validateClearMessage(vector<uint8_t> body)
	{
		if (body.size() < 32)
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
				return validateClearMessage(body);
				break;
			default:
				return false;
		}
	}

	bool
	operator==(const Packet& lhs, const Packet& rhs)
	{
		if (lhs.size() != rhs.size())
			return false;

		if (lhs.type() != rhs.type())
			return false;

		switch (lhs.type()) {
			case 0x00: return true;
					   break;

			case 0x01: ClearMessage m1(lhs);
					   ClearMessage m2(rhs);
					   return m1 == m2;
					   break;

			default: return false;
	}

	/* Class methods */
	Packet::Packet(vector<uint8_t> data)
	: raw_m(data)
	{
		if (ValidatePacket(data) == false)
			throw InvalidPacketException();

		type_m = data[5]; /* Store type */
	}

	uint16_t
	Packet::length() const
	{
		return body().size();
	}

	vector<uint8_t>
	Packet::body() const
	{
		/* Return (by value) a copy of the body */
		return vector<uint8_t>(raw_m.begin() + 8, raw_m.end());
	}

	vector<uint8_t>
	Packet::raw() const
	{
		return vector<uint8_t>(raw_m);
	}

	uint8_t
	Packet::type() const
	{
		return type_m;
	}

	/* Exception definitions */

	InvalidPacketException::InvalidPacketException(string const& error)
		: runtime_error(error)
	{}

	InvalidPacketException::InvalidPacketException()
		: runtime_error("Invalid packet!")
	{}

	WrongPacketTypeException::WrongPacketTypeException(string const& error)
		: runtime_error(error)
	{}

	WrongPacketTypeException::WrongPacketTypeException()
		: runtime_error("Packet type mismatch!")
	{}
}
