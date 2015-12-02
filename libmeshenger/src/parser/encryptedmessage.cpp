/* encryptedmessage.cpp
 */
#include <vector>
#include <string>
#include <cstdint>

#include <parser.h>

using namespace std;

namespace libmeshenger
{

	EncryptedMessage::EncryptedMessage(string s)
	{
		m_body_dec = vector<uint8_t>(s.c_str(), s.c_str()+s.size()+1);
		m_decrypted = true;
		m_encrypted = false;
	}

	EncryptedMessage::EncryptedMessage(Packet &p)
	{
		if (p.type() != 0x02)
			throw WrongPacketTypeException();

		m_body_enc = vector<uint8_t>(p.body());
		m_decrypted = false;
		m_encrypted = true;
	}

	bool
	EncryptedMessage::trusted()
	{
		return m_trusted;
	}

	uint16_t
	EncryptedMessage::sender()
	{
		return m_sender;
	}

	bool
	EncryptedMessage::encrypted()
	{
		return m_encrypted;
	}

	bool
	EncryptedMessage::decrypted()
	{
		return m_decrypted;
	}

	vector<uint8_t>
	EncryptedMessage::encryptedBody()
	{
		if (!m_encrypted) {
			throw PacketStateException("Packet not encrypted!");
		}

		return m_body_enc;
	}

	vector<uint8_t>
	EncryptedMessage::decryptedBody()
	{
		if (!m_decrypted) {
			throw PacketStateException("Packet not decrypted!");
		}

		return m_body_dec;
	}

	PacketStateException::PacketStateException(string const& error)
		: runtime_error(error)
	{}
}
