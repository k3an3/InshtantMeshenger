#ifndef __PARSER_H
#define __PARSER_H

#include <cstdint>
#include <vector>

namespace libmeshenger
{
	bool
	ValidatePacket(std::vector<std::uint8_t> message);
}

#endif
