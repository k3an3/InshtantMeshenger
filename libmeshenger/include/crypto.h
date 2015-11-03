/* crypto.h */

#ifndef __CRYPTO_H
#define __CRYPTO_H

#include <cryptopp/rsa.h>
#include <parser.h>

namespace libmeshenger
{
	class Buddy;

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
			Buddy buddy(std::string);

			CryptoEngine();
			CryptoEngine(CryptoPP::InvertibleRSAFunction);

			static CryptoPP::RSA::PublicKey pubkeyFromFile(std::string);
			static CryptoPP::RSA::PrivateKey privkeyFromFile(std::string);

			static void pubkeyToFile(CryptoPP::RSA::PublicKey, std::string);
			static void privkeyToFile(CryptoPP::RSA::PrivateKey, std::string);
	};

	class Buddy
	{
		private:
			std::string m_name;
			CryptoPP::RSA::PublicKey m_pubkey;
		public:
			CryptoPP::RSA::PublicKey pubkey() const;
			std::string name() const;

			Buddy(CryptoPP::RSA::PublicKey, std::string);
			Buddy(CryptoPP::RSA::PublicKey);
	};
}

#endif
