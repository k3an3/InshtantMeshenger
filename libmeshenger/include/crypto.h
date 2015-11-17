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
			/* Attempt to decrypt a message with this engin's privkey
			 * Returns true if the messages was encrypted with our
			 * private key
			 */
			bool tryDecrypt(EncryptedMessage&);

			/* Encrypt a message with the given public key */
			void encryptMessage(EncryptedMessage&, CryptoPP::RSAFunction);

			/* Encrypt a message with the public key of the buddy */
			void encryptMessage(EncryptedMessage&, const Buddy&);

			/* Encrypt a message with the public key of a buddy, by name
			 * The buddy must have been previously added to this engine 
			 */
			void encryptMessage(EncryptedMessage&, const std::string&);

			/* Encrypt a message with the public key of a buddy, by
			 * the index of the buddy within the buddy list
			 */
			void encryptMessage(EncryptedMessage&, uint16_t);

			/* Set the engine's private key */
			void setPrivateKey(CryptoPP::InvertibleRSAFunction);

			/* Base64 */
			void setPrivateKey(std::string);

			/* Load private key from file and set*/
			void setPrivateKeyFromFile(std::string);

			/* Retrieve the vector of buddies */
			std::vector<Buddy> buddies();

			/* Retrieve a buddy by index. These indices are the same as
			 * those in the vector returned by buddies()
			 */
			Buddy buddy(uint16_t);

			/* Retrieve a buddy by name */
			Buddy buddy(std::string);

			/* Constructors, with and without an initial key */
			CryptoEngine();
			CryptoEngine(CryptoPP::InvertibleRSAFunction);

			/* Standard key generation */
			static CryptoPP::RSA::PrivateKey genkey();

			/* Private/public key serialization/deserialization functions */
			static CryptoPP::RSA::PublicKey pubkeyFromFile(std::string);
			static CryptoPP::RSA::PrivateKey privkeyFromFile(std::string);

			static void pubkeyToFile(CryptoPP::RSA::PublicKey, std::string);
			static void privkeyToFile(CryptoPP::RSA::PrivateKey, std::string);

			static CryptoPP::RSA::PublicKey pubkeyFromBase64(std::string);
			static CryptoPP::RSA::PrivateKey privkeyFromBase64(std::string);

			static std::string pubkeyToBase64(CryptoPP::RSA::PublicKey);
			static std::string privkeyToBase64(CryptoPP::RSA::PrivateKey);
	};

	/* A buddy is a simple container of a name and a public key (for now) */
	class Buddy
	{
		private:
			std::string m_name;
			CryptoPP::RSA::PublicKey m_pubkey;
		public:
			CryptoPP::RSA::PublicKey pubkey() const;
			std::string name() const;

			/* Public key, name */
			Buddy(CryptoPP::RSA::PublicKey, std::string);
			/* Public key */
			Buddy(CryptoPP::RSA::PublicKey);

			/* Base64 pubkey, name */
			Buddy(std::string, std::string);
			/* Base64 pubkey */
			Buddy(std::string);
	};
}

#endif
