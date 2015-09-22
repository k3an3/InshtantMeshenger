#include <iostream>
#include <cstdint>

#include <parser.h>

using namespace std;

bool
libmeshenger::ValidateMessage(vector<uint8_t> msg)
{
	/* Validate minimum size */
	if (msg.size < 8)
		return false;

	/* Validate magic */
	if (msg[0] != 'I' || msg[1] != 'M')
		return false;

	/* Get body length */
	uint16_t bodyLength = msg[7]
	bodyLength |= (msg[6] << 8);
}
