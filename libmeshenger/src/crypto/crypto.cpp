/* crypto.cpp */

#include <crypto.h>
#include <vector>
#include <cstdint>
#include <parser.h>

using namespace std;
using namespace CryptoPP;

namespace libmeshenger
{
	CryptoEngine::CryptoEngine(InvertibleRSAFunction privkey)
		: m_privkey(privkey)
	{
	}

	CryptoEngine.tryDecrypt(EncryptedMessage& em)
	{
		if (em.decrypted())
			throw PacketStateException("Message is already decrypted!");

		if (!em.encrypted())
			throw PacketStateException("Message is not encrypted!");

		vector<uint8_t> plaintext;
		vector<uint8_t> ciphertext = em.encryptedBody();

		RSAES_OAEP_SHA_Decryptor d(privkey);

		plaintext.resize(d.MaxPlaintextLength(em.m_body_enc.size()));

		d.
	}
}
