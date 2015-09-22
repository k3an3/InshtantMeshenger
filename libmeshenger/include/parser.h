#ifndef __PARSER_H
#define __PARSER_H

#include <cstdint>
#include <vector>

namespace libmeshenger
{
	bool
	ValidateMessage(std::vector<std::uint8> message);
}

#endif
