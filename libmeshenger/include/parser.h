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
			std::vector<std::uint8_t> raw() const;
			std::vector<std::uint8_t> body() const;

			/* Packet type */
			uint8_t type() const;

			/* Body length */
			uint16_t length() const;

			/* Construct from raw data */
			Packet(std::vector<std::uint8_t>);
	};


	/* Packet type 1 */
	class ClearMessage final
	{
		private:
			std::vector<std::uint8_t> raw_m;
		public:
			/* Returns a vector copy containing the appropriate bytes */
			std::vector<std::uint8_t> id() const;
			std::vector<std::uint8_t> body() const;
			
			/* Body length */
			uint16_t length() const;

			/* Construct from a Packet */
			ClearMessage(Packet);

			/* Construct from a String */
			ClearMessage(std::string);
	};

	/* Message equality */
	bool operator==(const Message& lhs, const Message& rhs);

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
