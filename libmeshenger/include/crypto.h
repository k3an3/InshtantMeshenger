/* crypto.h */

#ifndef __CRYPTO_H
#define __CRYPTO_H

#include <cryptopp/rsa.h>
#include <parser.h>

namespace libmeshenger
{
	class CryptoEngine
	{
		friend class EncryptedMessage;
		private:
			CryptoPP::RSA::PrivateKey m_privkey;
			bool privkey_initialized;
			std::vector<Buddy> m_buddies;
		public:
			bool tryDecrypt(EncryptedMessage&);
			void encryptMessage(EncryptedMessage&, CryptoPP::RSAFunction);
			void encryptMessage(EncryptedMessage&, const Buddy&);
			void encryptMessage(EncryptedMessage&, const std::string&);
			void encryptMessage(EncryptedMessage&, uint16_t);
			void setPrivateKey(CryptoPP::InvertibleRSAFunction);
			std::vector<Buddy> buddies();
			Buddy buddy(uint16_t);
			Buddy buddy(string);

			CryptoEngine();
			CryptoEngine(CryptoPP::InvertibleRSAFunction);
	};

	class Buddy
	{
		private:
			std::string m_name;
			CryptoPP::RSA::PublicKey m_pubkey;
		public:
			CryptoPP::RSA::PublicKey pubkey();
			std::string name();

			Buddy(CryptoPP::RSA::PublicKey, std::string);
			Buddy(CryptoPP::RSA::PublicKey);
	}
}

#endif
