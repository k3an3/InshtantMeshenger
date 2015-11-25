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

	class Packet;
	class ClearMessage;
	class EncryptedMessage;

	class Packet final
	{
		private:
			std::vector<std::uint8_t> raw_m;
			std::uint8_t type_m;
		public:
			static const uint8_t CLEARMESSAGE_TYPE = 0x01;
			static const uint8_t ENCRYPTEDMESSAGE_TYPE = 0x02;
			/* Returns a new (copy) vector of the appropriate bytes */
			std::vector<std::uint8_t> raw() const;
			std::vector<std::uint8_t> body() const;
			std::vector<std::uint8_t> id() const;

			/* ID as string */
			std::string idString() const;

			/* Packet type */
			std::uint8_t type() const;

			/* Body length */
			std::uint16_t length() const;

			/* Construct from raw data */
			Packet(std::vector<std::uint8_t>);

			/* Construct from a ClearMessage */
			Packet(ClearMessage &);

			Packet(EncryptedMessage &);
	};


	/* Packet type 1
	 * Cleartext message
	 * */
	class ClearMessage final
	{
		private:
			std::vector<std::uint8_t> raw_m;
		public:
			/* Returns a vector copy containing the appropriate bytes */
			std::vector<std::uint8_t> body() const;
			
			/* Body length */
			std::uint16_t length() const;

			/* ID and body as strings */
			std::string bodyString() const;

			/* Construct from a Packet */
			ClearMessage(Packet);

			/* Construct from a String */
			ClearMessage(std::string);
	};

	class EncryptedMessage final
	{
		friend class CryptoEngine;
		protected:
			std::vector<uint8_t> m_body_enc;
			std::vector<uint8_t> m_body_dec;
			bool m_encrypted;
			bool m_decrypted;
			bool m_trusted;
			uint16_t m_sender;
		public:
			std::vector<uint8_t> encryptedBody();
			std::vector<uint8_t> decryptedBody();
			bool encrypted();
			bool decrypted();

			/* Return the index of the buddy that sent this message. Only
			 * valid for received, decrypted messaged */
			uint16_t sender();

			/* Whether or not the message was sent by a buddy */
			bool trusted();

			EncryptedMessage(std::string);
			EncryptedMessage(Packet&);
	};

	/* Message equality */
	bool operator==(const ClearMessage& lhs, const ClearMessage& rhs);

	/* Packet equality */
	bool operator==(const Packet& lhs, const Packet& rhs);

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
			PacketParsingException(std::string const& error);
			PacketParsingException();
	};

	class PacketStateException final : public std::runtime_error
	{
		public:
			PacketStateException(std::string const& error);
	};
}


#endif
