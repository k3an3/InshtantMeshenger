#ifndef __PARSER_H
#define __PARSER_H

#include <cstdint>
#include <vector>
#include <exception>
#include <stdexcept>
#include <string>

namespace libmeshenger
{
	bool
	ValidatePacket(std::vector<std::uint8_t> message);

	class Packet final
	{
		private:
			std::vector<std::uint8_t> raw_m;
			uint8_t type_m;
		public:
			/* Returns a new (copy) vector of the appropriate bytes */
			std::vector<std::uint8_t> raw();
			std::vector<std::uint8_t> body();

			/* Packet type */
			uint8_t type();

			/* Body length */
			uint16_t length();

			/* Construct from raw data */
			Packet(std::vector<std::uint8_t>);
	};

	/* Packet type 1 */
	class Message final
	{
		private:
			std::vector<std::uint8_t> raw_m;
		public:
			/* Returns a vector copy containing the appropriate bytes */
			std::vector<std::uint8_t> id();
			std::vector<std::uint8_t> body();
			
			/* Body length */
			uint16_t length();

			/* Construct from a Packet */
			Message(Packet);
	};

	/* Parser exceptions */
	class InvalidPacketException final : public std::runtime_error
	{
		public:
			InvalidPacketException(std::string const& error);
			InvalidPacketException();
	};

	class WrongPacketTypeException final : public std::runtime_error
	{
		public:
			WrongPacketTypeException(std::string const& error);
			WrongPacketTypeException();
	};
}


#endif
