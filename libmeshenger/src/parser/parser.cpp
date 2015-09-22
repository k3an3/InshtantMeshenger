#include <iostream>
#include <cstdint>

#include <parser.h>

using namespace std;

static bool
validateMessage(vector<uint8_t> msg);

bool
libmeshenger::ValidatePacket(vector<uint8_t> msg)
{
	/* Validate minimum size */
	if (msg.size < 8)
		return false;

	/* Validate magic */
	if (msg[0] != 'I' || msg[1] != 'M')
		return false;

	/* Validate version (always 1 for now) */
	if (msg[2] != 1)
		return false;

	/* Get body length */
	uint16_t bodyLength = msg[7]
	bodyLength |= (msg[6] << 8);

	/* Validate body length */
	if (msg.size != bodyLength + 8)
		return false;

	/* Return true if valid empty packet */
	if (msg.size == 8 && msg[5] == 0x00)
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

static bool
validateMessage(vector<uint8_t> body)
{
	if (body.size < 144)
	{
		return false;
	}
	return true;
}
