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
	/*checks to see if the packet is valid
	Reasons that a packet is not valid include: Unstructed Packet, Incorrect Encryption,Wrong Packet type or parsering error
	*/
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
	class Message final
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
			Message(Packet);
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
	/*
	*In the worse case scenario when the parser fails it would hit this parsing exception block
	* This is differnt than the InValid Packet Exception in that it would be an error in our code vs the packet it self
	*/
	class PacketParsingException final : public std::runtime_error
	{
		public:
			PacketParsingExceptio(std::string const& error);
			PacketParsingExceptio();
	};
}


#endif